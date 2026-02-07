#!/usr/bin/env python3
# cs/devtools/test_inline_using.gpt.py
import importlib.util
import unittest
from pathlib import Path

_mod_path = Path(__file__).resolve().parent / "inline_using.py"
_spec = importlib.util.spec_from_file_location("inline_using", _mod_path)
_mod = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(_mod)

extract_usings = _mod.extract_usings
apply_inlines = _mod.apply_inlines


class ExtractUsingsTest(unittest.TestCase):
    def test_empty(self):
        self.assertEqual(extract_usings(""), {})

    def test_single_using(self):
        block = "using ::cs::Result;\n"
        result = extract_usings(block)
        self.assertEqual(result, {"Result": "cs::Result"})

    def test_multiple_usings(self):
        block = "using ::cs::Ok;\nusing ::cs::Error;\n"
        result = extract_usings(block)
        self.assertEqual(result["Ok"], "cs::Ok")
        self.assertEqual(result["Error"], "cs::Error")

    def test_non_using_lines_ignored(self):
        block = "int x = 5;\nusing ::cs::Result;\nfoo();\n"
        result = extract_usings(block)
        self.assertEqual(result, {"Result": "cs::Result"})

    def test_leading_double_colon_stripped(self):
        block = "using ::std::string;\n"
        result = extract_usings(block)
        self.assertEqual(result["string"], "std::string")


class ApplyInlinesTest(unittest.TestCase):
    def test_no_anonymous_namespace(self):
        src = "#include <string>\nint main() {}\n"
        self.assertEqual(apply_inlines(src), src)

    def test_removes_anonymous_namespace(self):
        src = (
            "namespace {\n"
            "using ::cs::Result;\n"
            "}  // namespace\n"
            "Result foo();\n"
        )
        result = apply_inlines(src)
        self.assertNotIn("namespace {", result)
        self.assertIn("cs::Result", result)

    def test_preserves_already_qualified(self):
        src = "namespace {\n" "using ::cs::Ok;\n" "}\n" "cs::Ok bar();\n" "Ok baz();\n"
        result = apply_inlines(src)
        self.assertIn("cs::Ok bar();", result)
        self.assertIn("cs::Ok baz();", result)

    def test_empty_anonymous_namespace(self):
        src = "namespace {\n}\n"
        result = apply_inlines(src)
        self.assertNotIn("namespace {", result)


if __name__ == "__main__":
    unittest.main()
