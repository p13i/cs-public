#!/usr/bin/env python3
# cs/devtools/test_parse_logs.gpt.py
import importlib.util
import unittest
from pathlib import Path

_mod_path = Path(__file__).resolve().parent / "parse_logs.py"
_spec = importlib.util.spec_from_file_location("parse_logs", _mod_path)
_mod = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(_mod)

parse_log = _mod.parse_log
ts_to_filename = _mod.ts_to_filename


class ParseLogTest(unittest.TestCase):
    def test_empty_input(self):
        self.assertEqual(parse_log([]), [])

    def test_single_record(self):
        lines = [
            "[2024-01-15T12:30:00.000Z] [INFO] [main.cc:42] Hello world\n",
        ]
        recs = parse_log(lines)
        self.assertEqual(len(recs), 1)
        self.assertEqual(recs[0]["timestamp"], "2024-01-15T12:30:00.000Z")
        self.assertEqual(recs[0]["level"], "INFO")
        self.assertEqual(recs[0]["file"], "main.cc")
        self.assertEqual(recs[0]["line"], 42)
        self.assertEqual(recs[0]["message"], "Hello world")

    def test_multi_line_message(self):
        lines = [
            "[2024-01-15T12:30:00.000Z] [ERROR] [server.cc:10] Start\n",
            "  continuation line 1\n",
            "  continuation line 2\n",
        ]
        recs = parse_log(lines)
        self.assertEqual(len(recs), 1)
        self.assertIn("continuation line 1", recs[0]["message"])
        self.assertIn("continuation line 2", recs[0]["message"])

    def test_multiple_records(self):
        lines = [
            "[2024-01-15T12:30:00.000Z] [INFO] [a.cc:1] First\n",
            "[2024-01-15T12:30:01.000Z] [WARN] [b.cc:2] Second\n",
        ]
        recs = parse_log(lines)
        self.assertEqual(len(recs), 2)
        self.assertEqual(recs[0]["level"], "INFO")
        self.assertEqual(recs[1]["level"], "WARN")

    def test_orphan_lines_ignored(self):
        lines = [
            "some random text\n",
            "more random text\n",
        ]
        recs = parse_log(lines)
        self.assertEqual(len(recs), 0)

    def test_no_trailing_newline(self):
        lines = [
            "[2024-01-15T12:30:00.000Z] [DEBUG] [x.cc:5] NoNewline",
        ]
        recs = parse_log(lines)
        self.assertEqual(len(recs), 1)
        self.assertEqual(recs[0]["message"], "NoNewline")


class TsToFilenameTest(unittest.TestCase):
    def test_basic(self):
        result = ts_to_filename("2024-01-15T12:30:00.000Z")
        self.assertTrue(result.endswith(".json"))
        self.assertNotIn(":", result)

    def test_safe_characters(self):
        result = ts_to_filename("2024-01-15T12:30:00.123456789Z")
        self.assertTrue(result.endswith(".json"))


if __name__ == "__main__":
    unittest.main()
