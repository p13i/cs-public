#!/usr/bin/env python3
# cs/labs/typescript/hello_world_test.gpt.py
# cs/labs/typescript/hello_world_test.py
import os
import shutil
import subprocess
import tempfile
from pathlib import Path


def main() -> None:
    root = Path(__file__).resolve().parent
    with tempfile.TemporaryDirectory() as work:
        workdir = Path(work)
        for fname in ["hello_world.gpt.ts", "node_globals.d.ts", "tsconfig.json"]:
            shutil.copy(root / fname, workdir / fname)

        env = dict(os.environ)
        npm_cache = workdir / ".npm-cache"
        npm_cache.mkdir(parents=True, exist_ok=True)
        env.update(
            {
                "HOME": str(workdir),
                "NPM_CONFIG_CACHE": str(npm_cache),
            }
        )
        tsc = shutil.which("tsc")
        if tsc is None:
            npm = shutil.which("npm")
            if npm is None:
                raise SystemExit(
                    "tsc not found and npm missing; install TypeScript or use devcontainer"
                )
            pkg_dir = workdir / "npm"
            subprocess.run(
                [
                    npm,
                    "install",
                    "--prefix",
                    str(pkg_dir),
                    "--no-package-lock",
                    "typescript@5.6.3",
                ],
                check=True,
                env=env,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
            )
            tsc = str(pkg_dir / "node_modules" / ".bin" / "tsc")

        subprocess.run(
            [tsc, "--project", "tsconfig.json", "--pretty", "false"],
            cwd=workdir,
            check=True,
            env=env,
        )
        result = subprocess.run(
            ["node", "dist/hello_world.gpt.js"],
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
