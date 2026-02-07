#!/usr/bin/env python3
# cs/devtools/test_newfile.gpt.py
import importlib.util
import unittest
from pathlib import Path

_mod_path = Path(__file__).resolve().parent / "newfile.py"
_spec = importlib.util.spec_from_file_location("newfile", _mod_path)
_mod = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(_mod)

generate_header_guard = _mod.generate_header_guard
generate_namespace = _mod.generate_namespace


class GenerateHeaderGuardTest(unittest.TestCase):
    def test_basic(self):
        result = generate_header_guard("cs/util/string.hh")
        self.assertIn("#ifndef", result)
        self.assertIn("#define", result)
        self.assertIn("CS_UTIL_STRING_HH", result)

    def test_nested_path(self):
        result = generate_header_guard("cs/net/http/server.hh")
        self.assertIn("CS_NET_HTTP_SERVER_HH", result)

    def test_uppercase_conversion(self):
        result = generate_header_guard("cs/foo.hh")
        self.assertIn("CS_FOO_HH", result)


class GenerateNamespaceTest(unittest.TestCase):
    def test_basic(self):
        result = generate_namespace("cs/util/string.hh")
        self.assertEqual(result, "namespace cs::util")

    def test_nested(self):
        result = generate_namespace("cs/net/http/server.hh")
        self.assertEqual(result, "namespace cs::net::http")

    def test_single_dir(self):
        result = generate_namespace("cs/foo.hh")
        self.assertEqual(result, "namespace cs")


if __name__ == "__main__":
    unittest.main()
