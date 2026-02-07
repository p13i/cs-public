#!/usr/bin/env python3
# cs/devtools/test_embed_files.gpt.py
import importlib.util
import unittest
from pathlib import Path

_mod_path = Path(__file__).resolve().parent / "embed_files.py"
_spec = importlib.util.spec_from_file_location("embed_files", _mod_path)
_mod = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(_mod)

norm_symbol = _mod.norm_symbol
bytes_literal = _mod.bytes_literal


class NormSymbolTest(unittest.TestCase):
    def test_simple(self):
        self.assertEqual(norm_symbol("hello"), "hello")

    def test_replaces_special_chars(self):
        self.assertEqual(norm_symbol("foo-bar.baz"), "foo_bar_baz")

    def test_leading_digit(self):
        self.assertEqual(norm_symbol("123abc"), "_123abc")

    def test_no_leading_digit(self):
        self.assertEqual(norm_symbol("abc123"), "abc123")

    def test_slashes(self):
        self.assertEqual(norm_symbol("a/b/c"), "a_b_c")

    def test_empty(self):
        self.assertEqual(norm_symbol(""), "")


class BytesLiteralTest(unittest.TestCase):
    def test_empty(self):
        self.assertEqual(bytes_literal(b""), "")

    def test_single_byte(self):
        self.assertEqual(bytes_literal(b"\x00"), "0x00")

    def test_multiple_bytes(self):
        result = bytes_literal(b"\x48\x69")
        self.assertEqual(result, "0x48, 0x69")

    def test_all_ff(self):
        result = bytes_literal(b"\xff\xff")
        self.assertEqual(result, "0xff, 0xff")


if __name__ == "__main__":
    unittest.main()
