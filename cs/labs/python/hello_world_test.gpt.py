#!/usr/bin/env python3
# cs/labs/python/hello_world_test.gpt.py
# cs/labs/python/hello_world_test.py
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
sys.path.insert(0, str(ROOT))

# Import the renamed module
import importlib.util

spec = importlib.util.spec_from_file_location(
    "hello_world", ROOT / "hello_world.gpt.py"
)
hello_world = importlib.util.module_from_spec(spec)
spec.loader.exec_module(hello_world)


def _run() -> None:
    assert hello_world.say_hello("World") == "Hello World!"
    assert hello_world.say_hello("") == "Hello !"
    assert hello_world.say_hello() == "Hello World!"


if __name__ == "__main__":
    _run()
