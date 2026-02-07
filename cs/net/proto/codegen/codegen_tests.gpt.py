#!/usr/bin/env python3
# cs/net/proto/codegen/codegen_tests.gpt.py
# cs/net/proto/codegen/codegen_tests.py
"""Aggregated test runner for codegen."""
import importlib.util
import unittest
from pathlib import Path

from cs.net.proto.codegen.tests import test_cli_modes
from cs.net.proto.codegen.tests import test_generation_outputs
from cs.net.proto.codegen.tests import test_parser_and_validator


def _load_gpt_test_module(name: str):
    """Load a test module from a .gpt.py file (name has a dot)."""
    tests_dir = Path(__file__).resolve().parent / "tests"
    path = tests_dir / f"{name}.py"
    spec = importlib.util.spec_from_file_location(name.replace(".", "_"), path)
    if spec is None or spec.loader is None:
        raise ImportError(f"Cannot load spec for {path}")
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


def load_tests(
    loader: unittest.TestLoader, tests: unittest.TestSuite, pattern: str | None
) -> unittest.TestSuite:
    suite = unittest.TestSuite()
    test_toobject_fromobject_generation = _load_gpt_test_module(
        "test_toobject_fromobject_generation.gpt"
    )
    for module in (
        test_cli_modes,
        test_generation_outputs,
        test_parser_and_validator,
        test_toobject_fromobject_generation,
    ):
        suite.addTests(loader.loadTestsFromModule(module))
    return suite


if __name__ == "__main__":
    runner = unittest.TextTestRunner()
    result = runner.run(load_tests(unittest.TestLoader(), unittest.TestSuite(), None))
    raise SystemExit(not result.wasSuccessful())
