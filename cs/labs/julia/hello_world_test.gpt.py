#!/usr/bin/env python3
# cs/labs/julia/hello_world_test.gpt.py
# cs/labs/julia/hello_world_test.py
import shutil
import subprocess
import tempfile
from pathlib import Path


def main() -> None:
    root = Path(__file__).resolve().parent
    julia = shutil.which("julia")
    if julia is None:
        # Skip cleanly when Julia is not installed locally.
        print("SKIP: julia not found; install julia or use devcontainer")
        return

    with tempfile.TemporaryDirectory() as work:
        workdir = Path(work)
        shutil.copy(root / "hello_world.gpt.jl", workdir / "hello_world.jl")
        result = subprocess.run(
            [julia, "--color=no", "hello_world.jl"],
            cwd=workdir,
            check=True,
            capture_output=True,
            text=True,
        )
        output = result.stdout.strip()
        if output != "Hello World!":
            raise SystemExit(f"unexpected output: '{output}'")


if __name__ == "__main__":
    main()
