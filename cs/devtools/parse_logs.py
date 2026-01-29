#!/usr/bin/env python3
# cs/devtools/parse_logs.py
#!/usr/bin/env python3
import argparse
import json
import os
import re
import sys
from typing import List, Dict

REC_RE = re.compile(
    r"^\[(?P<timestamp>\d{4}-\d{2}-\d{2}T[\d:.]+Z)\]\s"
    r"\[(?P<level>[A-Z]+)\]\s"
    r"\[(?P<file>.+?):(?P<line>\d+)\]\s?"
    r"(?P<msg>.*)$"
)


def parse_log(lines: List[str]) -> List[Dict]:
    records = []
    cur = None
    msg_lines: List[str] = []

    def flush():
        nonlocal cur, msg_lines
        if cur is not None:
            cur["message"] = "".join(msg_lines).rstrip("\n")
            records.append(cur)
            cur = None
            msg_lines = []

    for raw in lines:
        m = REC_RE.match(raw)
        if m:
            flush()
            cur = {
                "timestamp": m.group("timestamp"),
                "level": m.group("level"),
                "file": m.group("file"),
                "line": int(m.group("line")),
                "message": "",  # filled on flush
            }
            msg_lines = [m.group("msg") + ("\n" if not raw.endswith("\n") else "")]
        else:
            if cur is None:
                continue
            msg_lines.append(raw if raw.endswith("\n") else raw + "\n")

    flush()
    return records


def ts_to_filename(ts: str) -> str:
    # Make a safe, stable filename derived from the timestamp
    # Replace ':' to keep cross-platform friendliness; allow digits/letters/TZ._-
    safe = ts.replace(":", "-")
    safe = re.sub(r"[^0-9A-Za-zTZ._-]+", "_", safe)
    return f"{safe}.json"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--input", "-i", default="app.log", help="input log file path")
    ap.add_argument(
        "--outdir",
        "-d",
        default="app.log.d",
        help="directory to write per-entry JSON files",
    )
    args = ap.parse_args()

    os.makedirs(args.outdir, exist_ok=True)

    with open(args.input, "r", encoding="utf-8", errors="replace") as f:
        lines = f.readlines()

    recs = parse_log(lines)

    already_exists = 0
    written_count = 0
    for rec in recs:
        ts = rec["timestamp"]
        fname = ts_to_filename(ts)
        path = os.path.join(args.outdir, fname)
        if os.path.exists(path):
            already_exists += 1
            if False:
                print(
                    f"[parse_logs] duplicate timestamp: {ts} -> {path} (skipping)",
                    file=sys.stderr,
                )
            continue
        with open(path, "w", encoding="utf-8") as out:
            json.dump(rec, out, ensure_ascii=False, indent=2)
            written_count += 1
    print(
        f"[parse_logs] Checked {len(recs)} files, writing {written_count} files, skipping {already_exists} files."
    )


if __name__ == "__main__":
    main()
