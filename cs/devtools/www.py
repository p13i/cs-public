#!/usr/bin/env python3
"""
www.py — Serve a live log webpage and run `make up` in a loop, restarting when upstream changes
or when the UI button "Fetch & Deploy" is clicked.

- Streams logs to web UI, console, and a log file (default: <repo>/www.log)
- UI: adds a "Fetch & Deploy" button next to "Clear" that triggers an immediate restart
- No third-party deps. Pure stdlib.
"""

import argparse
import collections
import http.server
import os
import queue
import signal
import subprocess
import sys
import threading
import time
from typing import Optional
import shutil

# ---------------------------
# Log hub for SSE + sinks
# ---------------------------


class LogHub:
    def __init__(
        self,
        max_lines: int = 2000,
        console: bool = True,
        log_path: Optional[str] = None,
    ):
        self._buf = collections.deque(maxlen=max_lines)
        self._clients = set()  # set[queue.Queue[str]]
        self._lock = threading.Lock()
        self._console = console
        self._log_fp = None  # opened lazily when set_logfile is called
        if log_path:
            self.set_logfile(log_path)

    def set_console(self, enabled: bool) -> None:
        with self._lock:
            self._console = enabled

    def set_logfile(self, path: str) -> None:
        """Open/rotate the log file (append, line-buffered)."""
        fp = open(path, "a", buffering=1, encoding="utf-8")
        with self._lock:
            old = self._log_fp
            self._log_fp = fp
        if old:
            try:
                old.close()
            except Exception:
                pass

    def close(self) -> None:
        with self._lock:
            fp = self._log_fp
            self._log_fp = None
        if fp:
            try:
                fp.close()
            except Exception:
                pass

    def _stamp(self) -> str:
        return time.strftime("%Y-%m-%d %H:%M:%S")

    def broadcast(self, text: str, src: Optional[str] = None) -> None:
        """Emit lines to: in-memory buffer, SSE clients, console (optional), and log file (optional)."""
        if text is None:
            return
        ts = self._stamp()
        prefix = f"[{ts}] [{src}] " if src else f"[{ts}] "
        lines = text.splitlines() or [""]
        out_lines = [prefix + ln for ln in lines]

        with self._lock:
            self._buf.extend(out_lines)
            clients = list(self._clients)
            console = self._console
            fp = self._log_fp

        if console:
            for ln in out_lines:
                try:
                    sys.stdout.write(ln + "\n")
                except Exception:
                    pass
            try:
                sys.stdout.flush()
            except Exception:
                pass

        if fp:
            try:
                for ln in out_lines:
                    fp.write(ln + "\n")
                fp.flush()
            except Exception:
                pass

        for ln in out_lines:
            for q in list(clients):
                try:
                    q.put_nowait(ln)
                except queue.Full:
                    pass

    def register(self) -> queue.Queue:
        q = queue.Queue(maxsize=4000)
        with self._lock:
            self._clients.add(q)
            backlog = list(self._buf)
        for line in backlog:
            try:
                q.put_nowait(line)
            except queue.Full:
                break
        return q

    def deregister(self, q: queue.Queue) -> None:
        with self._lock:
            self._clients.discard(q)


# ---------------------------
# Subprocess helpers
# ---------------------------


def _pipe_reader(stream, hub: LogHub, src: str):
    try:
        for line in iter(stream.readline, ""):
            hub.broadcast(line.rstrip("\n"), src)
    finally:
        try:
            stream.close()
        except Exception:
            pass


def popen_stream(cmd, cwd, hub: LogHub, src: str, env=None, pg_leader=False):
    """
    Start a process with stdout/stderr piped and stream to hub.
    Returns (Popen, (thread_out, thread_err)).
    """
    kwargs = dict(
        cwd=cwd,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        bufsize=1,
        universal_newlines=True,
        env=env or os.environ.copy(),
    )
    if pg_leader and hasattr(os, "setsid"):
        kwargs["preexec_fn"] = os.setsid  # new process group (POSIX)
    proc = subprocess.Popen(cmd, **kwargs)
    threading.Thread(
        target=_pipe_reader, args=(proc.stdout, hub, src), daemon=True
    ).start()
    threading.Thread(
        target=_pipe_reader, args=(proc.stderr, hub, src), daemon=True
    ).start()
    return proc


def graceful_stop(proc: subprocess.Popen, hub: LogHub, name="proc"):
    """Try INT → TERM → KILL on whole process group; log steps."""
    try:
        pgid = os.getpgid(proc.pid)
    except Exception:
        pgid = None

    def send(sig, label):
        try:
            if pgid is not None:
                os.killpg(pgid, sig)
            else:
                proc.send_signal(sig)
            hub.broadcast(f"sent {label}", name)
        except ProcessLookupError:
            pass

    for sig, label, wait in (
        (signal.SIGINT, "SIGINT", 5),
        (signal.SIGTERM, "SIGTERM", 5),
    ):
        if proc.poll() is None:
            send(sig, label)
            try:
                proc.wait(timeout=wait)
                return
            except subprocess.TimeoutExpired:
                pass
    if proc.poll() is None:
        send(signal.SIGKILL, "SIGKILL")
        try:
            proc.wait(timeout=2)
        except Exception:
            pass


# ---------------------------
# Git helpers (repo-aware)
# ---------------------------


def git_root(start_path: Optional[str] = None) -> str:
    """Return absolute repo root for `start_path` (or CWD) using `git -C`."""
    cmd = ["git"]
    if start_path:
        cmd += ["-C", start_path]
    cmd += ["rev-parse", "--show-toplevel"]
    out = subprocess.run(cmd, text=True, capture_output=True, check=True)
    return os.path.abspath(out.stdout.strip())


def detect_upstream(repo: str, env) -> str:
    """Resolve upstream (env UPSTREAM → branch upstream → origin/HEAD → origin/main)."""
    env_up = env.get("UPSTREAM")
    if env_up:
        return env_up
    try:
        out = subprocess.run(
            [
                "git",
                "-C",
                repo,
                "rev-parse",
                "--abbrev-ref",
                "--symbolic-full-name",
                "@{u}",
            ],
            text=True,
            capture_output=True,
            check=True,
        )
        return out.stdout.strip()
    except subprocess.CalledProcessError:
        try:
            out = subprocess.run(
                ["git", "-C", repo, "symbolic-ref", "refs/remotes/origin/HEAD"],
                text=True,
                capture_output=True,
                check=True,
            )
            ref = out.stdout.strip()  # e.g. refs/remotes/origin/main
            return ref.replace("refs/remotes/", "")
        except subprocess.CalledProcessError:
            return "origin/main"


def rev(repo: str, ref: str) -> str:
    out = subprocess.run(
        ["git", "-C", repo, "rev-parse", ref],
        text=True,
        capture_output=True,
        check=True,
    )
    return out.stdout.strip()


def is_ancestor(repo: str, anc: str, desc: str) -> bool:
    # True if `anc` is an ancestor of `desc`
    return (
        subprocess.run(
            ["git", "-C", repo, "merge-base", "--is-ancestor", anc, desc],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        ).returncode
        == 0
    )


def ref_relation(repo: str, local_sha: str, remote_sha: str) -> str:
    """
    Returns one of: 'equal', 'local_ahead', 'remote_ahead', 'diverged'
    """
    if local_sha == remote_sha:
        return "equal"
    if is_ancestor(repo, remote_sha, local_sha):
        return "local_ahead"  # local contains remote (unpushed commits)
    if is_ancestor(repo, local_sha, remote_sha):
        return "remote_ahead"  # remote contains local (upstream advanced)
    return "diverged"


# ---------------------------
# HTTP server (SSE + HTML)
# ---------------------------

HTML_PAGE = """<!doctype html>
<meta charset="utf-8">
<title>www — live logs</title>
<style>
  :root { color-scheme: dark light; }
  html, body { height:100%; margin:0; }
  body { font: 13px ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", monospace; }
  header { position: sticky; top:0; background: #111; color:#eee; padding:8px 12px; border-bottom:1px solid #333; }
  main { padding: 0; height: calc(100% - 42px); }
  pre#log { margin:0; padding: 12px; overflow:auto; height:100%; white-space:pre-wrap; }
  .muted { opacity: 0.7 }
  button { margin-right: 6px; }
</style>
<header>
  <button id="clear">Clear</button>
  <button id="deploy">Fetch & Deploy</button>
  <span class="muted">Streaming from: make/docker/git — <span id="state">connecting…</span></span>
</header>
<main><pre id="log"></pre></main>
<script>
  const log = document.getElementById('log');
  const state = document.getElementById('state');
  const clearBtn = document.getElementById('clear');
  const deployBtn = document.getElementById('deploy');
  clearBtn.onclick = () => { log.textContent=''; };
  let autoscroll = true;
  log.addEventListener('scroll', () => {
    autoscroll = (log.scrollTop + log.clientHeight + 8) >= log.scrollHeight;
  });
  function add(line) {
    log.append(document.createTextNode(line + "\\n"));
    if (autoscroll) log.scrollTop = log.scrollHeight;
  }
  async function deploy() {
    try {
      deployBtn.disabled = true;
      const prev = state.textContent;
      state.textContent = 'deploying…';
      const res = await fetch('/deploy', { method: 'POST' });
      if (!res.ok) throw new Error('deploy request failed');
      state.textContent = 'deploy requested';
      setTimeout(() => state.textContent = prev, 1500);
    } catch (e) {
      state.textContent = 'deploy error';
      setTimeout(() => state.textContent = 'connected', 3000);
    } finally {
      setTimeout(() => { deployBtn.disabled = false; }, 1500);
    }
  }
  deployBtn.onclick = deploy;
  function connect() {
    const es = new EventSource('/events');
    es.onopen = () => state.textContent = 'connected';
    es.onerror = () => state.textContent = 'disconnected — retrying…';
    es.onmessage = (e) => add(e.data);
  }
  connect();
</script>
"""


class Handler(http.server.BaseHTTPRequestHandler):
    hub: LogHub = None  # injected
    restart_evt: threading.Event = None  # injected

    def log_message(self, format, *args):
        # Silence default stderr logging; we stream via hub instead.
        pass

    def do_GET(self):
        if self.path == "/" or self.path.startswith("/index.html"):
            self.send_response(200)
            self.send_header("Content-Type", "text/html; charset=utf-8")
            self.send_header("Cache-Control", "no-store")
            self.end_headers()
            self.wfile.write(HTML_PAGE.encode("utf-8"))
            return

        if self.path == "/health":
            self.send_response(200)
            self.send_header("Content-Type", "text/plain; charset=utf-8")
            self.end_headers()
            self.wfile.write(b"ok")
            return

        if self.path == "/events":
            self.send_response(200)
            self.send_header("Content-Type", "text/event-stream")
            self.send_header("Cache-Control", "no-cache")
            self.send_header("Connection", "keep-alive")
            self.end_headers()

            q = self.hub.register()
            keepalive_every = 20
            last_ping = time.time()
            try:
                while True:
                    try:
                        line = q.get(timeout=1.0)
                        data = line.replace("\r", "")
                        for sub in data.split("\n"):
                            self.wfile.write(b"data: " + sub.encode("utf-8") + b"\n")
                        self.wfile.write(b"\n")
                        self.wfile.flush()
                    except queue.Empty:
                        pass
                    if time.time() - last_ping > keepalive_every:
                        self.wfile.write(b": ping\n\n")
                        self.wfile.flush()
                        last_ping = time.time()
            except (ConnectionResetError, BrokenPipeError):
                pass
            finally:
                self.hub.deregister(q)
            return

        self.send_response(404)
        self.end_headers()

    def do_POST(self):
        if self.path == "/deploy":
            # Trigger an immediate restart cycle
            try:
                if self.restart_evt:
                    self.restart_evt.set()
                self.hub.broadcast("manual deploy requested via UI", "www")
                self.send_response(202)
                self.send_header("Content-Type", "text/plain; charset=utf-8")
                self.end_headers()
                self.wfile.write(b"deploy requested")
            except Exception as e:
                self.send_response(500)
                self.end_headers()
                try:
                    self.wfile.write(str(e).encode("utf-8"))
                except Exception:
                    pass
            return
        self.send_response(404)
        self.end_headers()


def serve_http(
    hub: LogHub,
    host: str,
    port: int,
    stop_evt: threading.Event,
    restart_evt: threading.Event,
):
    Handler.hub = hub
    Handler.restart_evt = restart_evt
    server = http.server.ThreadingHTTPServer((host, port), Handler)
    server.daemon_threads = True

    def _serve():
        hub.broadcast(f"Web server listening on http://{host}:{port}", "www")
        try:
            while not stop_evt.is_set():
                server.handle_request()
        finally:
            try:
                server.server_close()
            except Exception:
                pass

    t = threading.Thread(target=_serve, daemon=True)
    t.start()
    return t


# ---------------------------
# Compose detection
# ---------------------------


def ensure_compose(env: dict, hub) -> None:
    """Detect Docker Compose, set env['DC'] to the full command, and log the choice."""
    extra_dirs = ["/usr/bin", "/usr/local/bin", "/bin", "/usr/sbin", "/usr/local/sbin"]
    current = env.get("PATH", "")
    parts = [p for p in current.split(":") if p]
    for d in extra_dirs:
        if d not in parts:
            parts.insert(0, d)
    env["PATH"] = ":".join(parts)

    if env.get("DC"):
        hub.broadcast(f'compose: using provided DC="{env["DC"]}"', "www")
        return

    docker = shutil.which("docker", path=env["PATH"])
    v1 = shutil.which("docker-compose", path=env["PATH"])

    if docker:
        try:
            r = subprocess.run(
                [docker, "compose", "version"], text=True, capture_output=True
            )
            if r.returncode == 0:
                env["DOCKER"] = docker
                env["DC"] = f"{docker} compose"
                hub.broadcast(f'compose: using v2 via "{env["DC"]}"', "www")
                return
        except Exception:
            pass

    if v1:
        env["DC"] = v1
        hub.broadcast(f'compose: using v1 via "{env["DC"]}"', "www")
        return

    hub.broadcast(
        'ERROR: Neither "docker compose" nor "docker-compose" found on PATH.', "www"
    )
    hub.broadcast(
        "Hint: install docker-compose-plugin or ensure /usr/bin/docker is on PATH.",
        "www",
    )
    hub.broadcast(f"PATH: {env.get('PATH','')}", "www")
    hub.broadcast(
        f"which docker: {shutil.which('docker', path=env.get('PATH',''))}", "www"
    )
    hub.broadcast(
        f"which docker-compose: {shutil.which('docker-compose', path=env.get('PATH',''))}",
        "www",
    )
    sys.exit(1)


# ---------------------------
# Main watcher loop
# ---------------------------


def main():
    parser = argparse.ArgumentParser(
        description="Serve logs & restart `make up` on upstream changes or UI request."
    )
    parser.add_argument(
        "--repo",
        type=str,
        default=os.getenv("REPO"),
        help="Path to the Git repository (default: current repo)",
    )
    parser.add_argument(
        "--poll",
        type=int,
        default=int(os.getenv("POLL", "10")),
        help="Seconds between upstream checks",
    )
    parser.add_argument(
        "--upstream",
        type=str,
        default=os.getenv("UPSTREAM"),
        help="Upstream to watch, e.g., origin/main (default: branch upstream)",
    )
    parser.add_argument("--host", type=str, default=os.getenv("HOST", "127.0.0.1"))
    parser.add_argument("--port", type=int, default=int(os.getenv("PORT", "8090")))
    parser.add_argument(
        "--log-file",
        type=str,
        default=os.getenv("LOG_FILE"),
        help="Path to log file (default: <repo>/www.log)",
    )
    parser.add_argument(
        "--no-console",
        action="store_true",
        help="Disable echo to terminal (still streams to web + file)",
    )
    args = parser.parse_args()

    try:
        repo = git_root(args.repo) if args.repo else git_root()
    except subprocess.CalledProcessError:
        print("www.py: the provided --repo is not a Git repository.", file=sys.stderr)
        return 2

    log_path = args.log_file or os.path.join(repo, "www.log")
    hub = LogHub(max_lines=2000, console=(not args.no_console), log_path=log_path)
    stop_evt = threading.Event()
    restart_evt = threading.Event()
    serve_http(hub, args.host, args.port, stop_evt, restart_evt)

    env = os.environ.copy()
    ensure_compose(env, hub)
    upstream = args.upstream or detect_upstream(repo, env)
    hub.broadcast(f"repo: {repo}", "www")
    hub.broadcast(f"log:  {log_path}", "www")
    hub.broadcast(f"upstream: {upstream}", "www")
    hub.broadcast(f"poll: {args.poll}s", "www")
    hub.broadcast(f"UI: http://{args.host}:{args.port}/", "www")

    def on_sig(sig, frame):
        stop_evt.set()

    signal.signal(signal.SIGINT, on_sig)
    signal.signal(signal.SIGTERM, on_sig)

    while not stop_evt.is_set():
        hub.broadcast("starting: make up", "make")
        make_proc = popen_stream(
            ["make", "--no-print-directory", "up"],
            cwd=repo,
            hub=hub,
            src="make",
            env=env,
            pg_leader=True,
        )

        changed = False
        try:
            while make_proc.poll() is None and not stop_evt.is_set():
                # Manual restart requested from UI?
                if restart_evt.is_set():
                    hub.broadcast("manual deploy requested; restarting…", "www")
                    changed = True
                    restart_evt.clear()
                    break

                # Poll upstream
                popen_stream(
                    ["git", "-C", repo, "fetch", "--progress"],
                    cwd=None,
                    hub=hub,
                    src="git",
                ).wait()
                try:
                    local = rev(repo, "HEAD")
                    remote = rev(repo, upstream)
                    rel = ref_relation(repo, local, remote)

                    if rel == "remote_ahead":
                        hub.broadcast(
                            f"upstream advanced {local[:7]} → {remote[:7]}; restarting…",
                            "git",
                        )
                        changed = True
                        break
                    elif rel == "diverged":
                        # choose your policy; here we restart so you notice/resolve it
                        hub.broadcast(
                            f"local and upstream diverged (local {local[:7]}, upstream {remote[:7]}); restarting…",
                            "git",
                        )
                        changed = True
                        break
                    elif rel == "local_ahead":
                        # Do NOT restart when you have local commits not pushed yet
                        hub.broadcast(
                            f"local is ahead of {upstream} (contains {remote[:7]}); not restarting.",
                            "git",
                        )
                        # keep running without interrupting `make up`
                        pass
                    # else 'equal' → no action
                except subprocess.CalledProcessError as e:
                    hub.broadcast(f"warning: rev-parse failed: {e}", "git")
                time.sleep(args.poll)
        except KeyboardInterrupt:
            stop_evt.set()

        hub.broadcast("bringing stack down…", "make")
        popen_stream(
            ["make", "--no-print-directory", "down"], cwd=repo, hub=hub, src="make"
        ).wait()
        if make_proc.poll() is None:
            hub.broadcast("waiting for current docker build to finish…", "www")
            try:
                make_proc.wait(timeout=600)  # allow build to finish gracefully
            except subprocess.TimeoutExpired:
                hub.broadcast("forcing stop of make up (took too long)", "www")
                graceful_stop(make_proc, hub, name="make")

        if changed and not stop_evt.is_set():
            hub.broadcast("pulling latest…", "git")
            popen_stream(
                ["git", "-C", repo, "pull", "--rebase", "--autostash"],
                cwd=None,
                hub=hub,
                src="git",
            ).wait()
            continue

        if stop_evt.is_set():
            break

        hub.broadcast("`make up` exited; leaving.", "www")
        break

    hub.broadcast("server stopping.", "www")
    stop_evt.set()
    time.sleep(0.3)
    hub.close()
    return 0


if __name__ == "__main__":
    sys.exit(main())
