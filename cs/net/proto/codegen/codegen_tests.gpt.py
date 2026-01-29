#!/usr/bin/env python3
# cs/net/proto/codegen/codegen_tests.gpt.py
# cs/net/proto/codegen/codegen_tests.py
"""Aggregated test runner for codegen."""
import unittest

from cs.net.proto.codegen.tests import test_cli_modes
from cs.net.proto.codegen.tests import test_generation_outputs
from cs.net.proto.codegen.tests import test_parser_and_validator


def load_tests(
    loader: unittest.TestLoader, tests: unittest.TestSuite, pattern: str | None
) -> unittest.TestSuite:
    suite = unittest.TestSuite()
    for module in (
        test_cli_modes,
        test_generation_outputs,
        test_parser_and_validator,
    ):
        suite.addTests(loader.loadTestsFromModule(module))
    return suite


if __name__ == "__main__":
    runner = unittest.TextTestRunner()
    result = runner.run(load_tests(unittest.TestLoader(), unittest.TestSuite(), None))
    raise SystemExit(not result.wasSuccessful())
