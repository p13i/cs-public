#!/usr/bin/env python3
# cs/devtools/test_lint_makefile.gpt.py
import importlib.util
import unittest
from pathlib import Path

_mod_path = Path(__file__).resolve().parent / "lint_makefile.py"
_spec = importlib.util.spec_from_file_location("lint_makefile", _mod_path)
_mod = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(_mod)

is_all_caps = _mod.is_all_caps
parse_blocks = _mod.parse_blocks
_phony_targets_from_line = _mod._phony_targets_from_line


class IsAllCapsTest(unittest.TestCase):
    def test_all_caps(self):
        self.assertTrue(is_all_caps("FOO"))

    def test_with_underscores(self):
        self.assertTrue(is_all_caps("FOO_BAR"))

    def test_with_digits(self):
        self.assertTrue(is_all_caps("FOO123"))

    def test_lowercase(self):
        self.assertFalse(is_all_caps("foo"))

    def test_mixed_case(self):
        self.assertFalse(is_all_caps("Foo"))

    def test_empty(self):
        self.assertFalse(is_all_caps(""))

    def test_none(self):
        self.assertFalse(is_all_caps(None))


class PhonyTargetsFromLineTest(unittest.TestCase):
    def test_basic(self):
        targets = _phony_targets_from_line(".PHONY: build test\n")
        self.assertEqual(targets, ["build", "test"])

    def test_single(self):
        targets = _phony_targets_from_line(".PHONY: all\n")
        self.assertEqual(targets, ["all"])

    def test_no_match(self):
        targets = _phony_targets_from_line("FOO = bar\n")
        self.assertEqual(targets, [])


class ParseBlocksTest(unittest.TestCase):
    def test_empty(self):
        blocks = parse_blocks([])
        self.assertEqual(len(blocks), 0)

    def test_single_var(self):
        lines = ["FOO = bar\n"]
        blocks = parse_blocks(lines)
        var_blocks = [b for b in blocks if b.kind == "var"]
        self.assertEqual(len(var_blocks), 1)
        self.assertEqual(var_blocks[0].key, "FOO")

    def test_guard_block(self):
        lines = [
            "ifeq ($(X),1)\n",
            "  FOO = 1\n",
            "endif\n",
        ]
        blocks = parse_blocks(lines)
        guard_blocks = [b for b in blocks if b.kind == "guard"]
        self.assertEqual(len(guard_blocks), 1)

    def test_rule_block(self):
        lines = [
            "build: ## Build.\n",
            "\t@echo building\n",
        ]
        blocks = parse_blocks(lines)
        rule_blocks = [b for b in blocks if b.kind == "rule"]
        self.assertEqual(len(rule_blocks), 1)
        self.assertEqual(rule_blocks[0].key, "build")


if __name__ == "__main__":
    unittest.main()
