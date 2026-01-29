#!/usr/bin/env python3
# cs/devtools/load_generator.py
"""Generate sustained high QPS load against HTTP endpoints."""
import argparse
import json
import subprocess
import sys
import threading
import time
import urllib.error
import urllib.request
from datetime import datetime
from typing import Optional


def get_replica_count() -> int:
    """Get the number of running web replicas by querying Docker."""
    import subprocess
    import re

    try:
        # Query docker for running containers matching cs-web-* pattern
        result = subprocess.run(
            ["docker", "ps", "--filter", "name=cs-web-", "--format", "{{.Names}}"],
            capture_output=True,
            text=True,
            timeout=2,
        )
        if result.returncode == 0:
            # Count unique container names matching cs-web-{number}
            names = result.stdout.strip().split("\n")
            if not names or names == [""]:
                return 3  # Default fallback

            # Extract numbers from cs-web-{number} pattern
            pattern = re.compile(r"cs-web-(\d+)")
            numbers = set()
            for name in names:
                match = pattern.match(name.strip())
                if match:
                    numbers.add(int(match.group(1)))

            if numbers:
                return max(numbers)  # Return highest number found

    except (subprocess.TimeoutExpired, subprocess.SubprocessError, ValueError):
        pass

    # Fallback: try docker compose ps if available
    try:
        result = subprocess.run(
            ["docker", "compose", "ps", "web", "--format", "{{.Name}}"],
            capture_output=True,
            text=True,
            timeout=2,
        )
        if result.returncode == 0:
            names = result.stdout.strip().split("\n")
            if names and names != [""]:
                # Extract numbers from container names
                pattern = re.compile(r"cs-web-(\d+)")
                numbers = set()
                for name in names:
                    match = pattern.match(name.strip())
                    if match:
                        numbers.add(int(match.group(1)))
                if numbers:
                    return max(numbers)
    except (subprocess.TimeoutExpired, subprocess.SubprocessError):
        pass

    # Final fallback
    return 3


def find_max_qps(
    url: str,
    test_duration: int,
    min_qps: int,
    max_qps: int,
    target_success_rate: float,
    num_nodes: int = None,
) -> int:
    """Find maximum QPS that maintains target success rate using binary search."""
    print("=== Finding Maximum QPS ===")
    print(f"Target success rate: {target_success_rate}%")
    print(f"Test duration per iteration: {test_duration}s")
    print(f"Search range: {min_qps} - {max_qps} QPS")
    print()

    low = min_qps
    high = max_qps
    best_qps = 0
    convergence_threshold = 100  # Stop when range is smaller than this
    max_iterations = 20  # Safety limit
    iteration = 0

    print("Progress:")
    print("QPS      | Success Rate | Status")
    print("-" * 40)

    while (high - low) > convergence_threshold and iteration < max_iterations:
        iteration += 1
        mid = (low + high) // 2

        # Create generator for this test
        generator = LoadGenerator(url, mid, test_duration, num_nodes)
        success_rate = generator.run_test()

        status = "✓ PASS" if success_rate >= target_success_rate else "✗ FAIL"
        print(f"{mid:8d} | {success_rate:11.2f}% | {status}")

        if success_rate >= target_success_rate:
            best_qps = max(best_qps, mid)
            low = mid + 1
        else:
            high = mid - 1

    print("-" * 40)
    print()

    if best_qps > 0:
        # Verify the best QPS one more time
        print(f"Verifying maximum QPS: {best_qps}")
        generator = LoadGenerator(url, best_qps, test_duration, num_nodes)
        final_success_rate = generator.run_test()
        print(f"Final verification: {final_success_rate:.2f}% success rate")
        print()
        print(f"=== Result ===")
        print(f"Maximum QPS: {best_qps} ({final_success_rate:.2f}% success rate)")
        return best_qps
    else:
        print("=== Result ===")
        print(f"No QPS found that meets {target_success_rate}% success rate")
        print(f"Best attempt: {low} QPS")
        return 0


class LoadGenerator:
    """Generates HTTP load at a specified QPS rate."""

    def __init__(self, url: str, qps: int, duration: int, num_nodes: int = None):
        self.url = url
        self.qps = qps
        self.duration = duration
        self.running = True
        self.requests_sent = 0
        self.requests_completed = 0
        self.requests_failed = 0
        # Calculate workers: balance parallelism with lock contention
        # Too many workers causes lock contention; too few can't maintain rate with latency
        # Strategy: Aim for ~8 req/s per worker to reduce lock contention while handling latency
        # This provides a good balance: fewer workers = less lock contention, but enough to overlap requests
        if qps < 10:
            self.workers = max(1, qps)  # For very low QPS, use QPS workers
        else:
            # Aim for ~8 req/s per worker (more aggressive to reduce lock contention)
            # QPS=62 -> 8 workers (7.75 req/s per worker) - much better than 31
            # QPS=100 -> 12 workers (8.33 req/s per worker)
            self.workers = max(2, int(qps / 8))
            # Cap at 2000 to prevent excessive workers at very high QPS
            self.workers = min(self.workers, 2000)
        self.lock = threading.Lock()
        # Get number of web nodes from Docker or use provided value
        self.num_nodes = num_nodes if num_nodes is not None else get_replica_count()
        # Token bucket for rate limiting: track when tokens are available
        # Start with tokens available immediately
        self.next_token_time = time.time()
        self.token_interval = 1.0 / self.qps if self.qps > 0 else 1.0

    def worker(self):
        """Worker thread that continuously makes requests at the target rate."""
        request_count = 0
        while self.running:
            # Acquire a token from the shared rate limiter
            current_time = time.time()
            with self.lock:
                # Calculate when the next token is available
                if current_time < self.next_token_time:
                    sleep_time = self.next_token_time - current_time
                    token_available_time = self.next_token_time
                else:
                    sleep_time = 0
                    token_available_time = current_time
                # Reserve the token by advancing the next token time
                self.next_token_time = token_available_time + self.token_interval

            # Sleep if needed (outside the lock to avoid blocking other workers)
            if sleep_time > 0:
                time.sleep(sleep_time)

            # Make the request
            request_start = time.time()
            try:
                req = urllib.request.Request(self.url)
                urllib.request.urlopen(req, timeout=1)
                with self.lock:
                    self.requests_completed += 1
            except (urllib.error.URLError, OSError, TimeoutError):
                with self.lock:
                    self.requests_failed += 1
            except Exception:
                with self.lock:
                    self.requests_failed += 1
            with self.lock:
                self.requests_sent += 1
                request_count += 1

    def get_load(self, node: int) -> Optional[float]:
        """Get current load value from a web node."""
        try:
            result = subprocess.run(
                [
                    "docker",
                    "exec",
                    f"cs-web-{node}",
                    "curl",
                    "-s",
                    "http://localhost:8080/load",
                ],
                capture_output=True,
                text=True,
                timeout=2,
            )
            if result.returncode == 0:
                data = json.loads(result.stdout)
                return data.get("load", 0.0)
        except (
            subprocess.TimeoutExpired,
            subprocess.SubprocessError,
            json.JSONDecodeError,
        ):
            pass
        return None

    def monitor(self):
        """Monitor load values and request rates."""
        # Define column widths
        time_width = 8
        web_width = 7  # Enough for "web-10" (6 chars) with 1 space padding
        reqs_width = 7
        completed_width = 10
        failed_width = 6

        # Dynamically generate header based on number of nodes
        header_parts = [f"{'Time':<{time_width}}"]
        for i in range(1, self.num_nodes + 1):
            header_parts.append(f"{f'web-{i}':<{web_width}}")
        header_parts.extend(
            [
                f"{'Req/s':<{reqs_width}}",
                f"{'Completed':<{completed_width}}",
                f"{'Failed':<{failed_width}}",
            ]
        )
        header = " | ".join(header_parts)

        separator_parts = ["-" * time_width]
        for _ in range(self.num_nodes):
            separator_parts.append("-" * web_width)
        separator_parts.extend(
            ["-" * reqs_width, "-" * completed_width, "-" * failed_width]
        )
        separator = " | ".join(separator_parts)

        print(header)
        print(separator)
        last_sent = 0
        last_completed = 0

        while self.running:
            time.sleep(1)
            now = datetime.now().strftime("%H:%M:%S")

            # Get load values for all nodes
            loads = [self.get_load(i) for i in range(1, self.num_nodes + 1)]
            loads_str = " | ".join(
                [
                    f"{l:{web_width}.3f}" if l is not None else f"{'N/A':<{web_width}}"
                    for l in loads
                ]
            )

            # Calculate rates
            with self.lock:
                current_sent = self.requests_sent
                current_completed = self.requests_completed
                current_failed = self.requests_failed

            req_per_sec = current_sent - last_sent
            completed_per_sec = current_completed - last_completed

            last_sent = current_sent
            last_completed = current_completed

            # Format with proper alignment matching header widths
            print(
                f"{now:<{time_width}} | {loads_str} | {req_per_sec:<{reqs_width}d} | {current_completed:<{completed_width}d} | {current_failed:<{failed_width}d}"
            )

    def run(self):
        """Run the load generator."""
        print("=== Load Generator ===")
        print(f"URL: {self.url}")
        print(f"Target QPS: {self.qps}")
        print(f"Duration: {self.duration}s")
        print(f"Workers: {self.workers}")
        print()

        # Start worker threads
        threads = []
        for _ in range(self.workers):
            t = threading.Thread(target=self.worker, daemon=True)
            t.start()
            threads.append(t)

        # Start monitor
        monitor_thread = threading.Thread(target=self.monitor, daemon=True)
        monitor_thread.start()

        # Run for duration
        try:
            time.sleep(self.duration)
            self.running = False
        except KeyboardInterrupt:
            self.running = False
            print("\nStopping load generator...")

        # Wait for threads to finish
        time.sleep(2)

        print()
        print("=== Summary ===")
        with self.lock:
            print(f"Total requests sent: {self.requests_sent}")
            print(f"Total requests completed: {self.requests_completed}")
            print(f"Total requests failed: {self.requests_failed}")
            if self.requests_sent > 0:
                success_rate = self.requests_completed / self.requests_sent * 100
                print(f"Success rate: {success_rate:.2f}%")

    def get_success_rate(self) -> float:
        """Get the success rate after a test run."""
        with self.lock:
            if self.requests_sent == 0:
                return 0.0
            return (self.requests_completed / self.requests_sent) * 100.0

    def run_test(self) -> float:
        """Run a single QPS test and return the success rate."""
        # Reset counters and rate limiter state
        with self.lock:
            self.requests_sent = 0
            self.requests_completed = 0
            self.requests_failed = 0
            self.running = True
            self.next_token_time = time.time()

        # Start worker threads
        threads = []
        for _ in range(self.workers):
            t = threading.Thread(target=self.worker, daemon=True)
            t.start()
            threads.append(t)

        # Run for duration
        try:
            time.sleep(self.duration)
            self.running = False
        except KeyboardInterrupt:
            self.running = False

        # Wait for threads to finish
        time.sleep(2)

        return self.get_success_rate()


def find_max_qps(
    url: str,
    test_duration: int,
    min_qps: int,
    max_qps: int,
    target_success_rate: float,
    num_nodes: int = None,
) -> int:
    """Find maximum QPS that maintains target success rate using binary search."""
    print("=== Finding Maximum QPS ===")
    print(f"Target success rate: {target_success_rate}%")
    print(f"Test duration per iteration: {test_duration}s")
    print(f"Search range: {min_qps} - {max_qps} QPS")
    print()

    low = min_qps
    high = max_qps
    best_qps = 0
    convergence_threshold = 100  # Stop when range is smaller than this
    max_iterations = 20  # Safety limit
    iteration = 0

    print("Progress:")
    print("QPS      | Success Rate | Status")
    print("-" * 40)

    while (high - low) > convergence_threshold and iteration < max_iterations:
        iteration += 1
        mid = (low + high) // 2

        # Create generator for this test
        generator = LoadGenerator(url, mid, test_duration, num_nodes)
        success_rate = generator.run_test()

        status = "✓ PASS" if success_rate >= target_success_rate else "✗ FAIL"
        print(f"{mid:8d} | {success_rate:11.2f}% | {status}")

        if success_rate >= target_success_rate:
            best_qps = max(best_qps, mid)
            low = mid + 1
        else:
            high = mid - 1

    print("-" * 40)
    print()

    if best_qps > 0:
        # Verify the best QPS one more time
        print(f"Verifying maximum QPS: {best_qps}")
        generator = LoadGenerator(url, best_qps, test_duration, num_nodes)
        final_success_rate = generator.run_test()
        print(f"Final verification: {final_success_rate:.2f}% success rate")
        print()
        print(f"=== Result ===")
        print(f"Maximum QPS: {best_qps} ({final_success_rate:.2f}% success rate)")
        return best_qps
    else:
        print("=== Result ===")
        print(f"No QPS found that meets {target_success_rate}% success rate")
        print(f"Best attempt: {low} QPS")
        return 0


def main():
    """Main entry point."""
    # Force line-buffered output for non-TTY environments
    import sys
    import os

    if not os.isatty(1):
        sys.stdout = os.fdopen(sys.stdout.fileno(), "w", 1)  # Line buffered
    if not os.isatty(2):
        sys.stderr = os.fdopen(sys.stderr.fileno(), "w", 1)  # Line buffered

    parser = argparse.ArgumentParser(
        description="Generate sustained HTTP load at specified QPS"
    )
    # Detect if running in devcontainer and use host.docker.internal
    default_url = "http://localhost:8080/"
    if os.path.exists("/.devcontainer") or os.environ.get("DEVCONTAINER"):
        # Try host.docker.internal first (works on Docker Desktop)
        default_url = "http://host.docker.internal:8080/"

    parser.add_argument(
        "url",
        nargs="?",
        default=default_url,
        help=f"Target URL (default: {default_url})",
    )
    parser.add_argument(
        "--qps",
        type=int,
        default=10000,
        help="Target queries per second (default: 10000)",
    )
    parser.add_argument(
        "--duration",
        type=int,
        default=60,
        help="Duration in seconds (default: 60)",
    )
    parser.add_argument(
        "--nodes",
        type=int,
        default=None,
        help="Number of web nodes to monitor (default: auto-detect from Docker)",
    )
    parser.add_argument(
        "--find-max-qps",
        action="store_true",
        help="Enable automated binary search to find maximum QPS with target success rate",
    )
    parser.add_argument(
        "--test-duration",
        type=int,
        default=30,
        help="Duration in seconds for each QPS test in binary search (default: 30)",
    )
    parser.add_argument(
        "--min-qps",
        type=int,
        default=100,
        help="Minimum QPS to start binary search (default: 100)",
    )
    parser.add_argument(
        "--max-qps",
        type=int,
        default=100000,
        help="Maximum QPS to test (default: 100000)",
    )
    parser.add_argument(
        "--target-success-rate",
        type=float,
        default=99.9,
        help="Target success rate threshold in percentage (default: 99.9)",
    )

    args = parser.parse_args()

    if args.find_max_qps:
        return find_max_qps(
            args.url,
            args.test_duration,
            args.min_qps,
            args.max_qps,
            args.target_success_rate,
            args.nodes,
        )

    generator = LoadGenerator(args.url, args.qps, args.duration, args.nodes)
    generator.run()

    return 0


if __name__ == "__main__":
    sys.exit(main())
