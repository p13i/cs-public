#!/usr/bin/env python3
# cs/labs/javascript/hello_world_test.gpt.py
# cs/labs/javascript/hello_world_test.py
import subprocess
from pathlib import Path


def main() -> None:
    root = Path(__file__).resolve().parent
    script = root / "hello_world.gpt.js"
    result = subprocess.run(
        ["node", str(script)],
        check=True,
        capture_output=True,
        text=True,
    )
    output = result.stdout.strip()
    if output != "Hello World!":
        raise SystemExit(f"unexpected output: '{output}'")


if __name__ == "__main__":
    main()
