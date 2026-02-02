#!/usr/bin/env python3
# cs/devtools/test_publish_code_unit.gpt.py
"""
Unit tests for publish_code.py (CheckACLs, WalkACLs, clean_directory_except_git, sanitize_env_content).

Run via Bazel: bazel test //cs/devtools:test_publish_code_unit
"""

import importlib.util
import json
import os
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

_publish_code_path = Path(__file__).resolve().parent / "publish_code.py"
_spec = importlib.util.spec_from_file_location("publish_code", _publish_code_path)
publish_code = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(publish_code)

CheckACLs = publish_code.CheckACLs
WalkACLs = publish_code.WalkACLs
clean_directory_except_git = publish_code.clean_directory_except_git
sanitize_env_content = publish_code.sanitize_env_content
ANY = publish_code.ANY


class TestCheckACLs(unittest.TestCase):
    """Test CheckACLs(acls, principal, resource) - pure policy check."""

    def test_allow_matching_principal_and_resource(self):
        acls = {
            "policies": [
                {"principals": ["alice"], "resources": ["foo.txt"], "allow": True},
            ]
        }
        self.assertTrue(CheckACLs(acls, "alice", "foo.txt"))

    def test_deny_no_matching_allow(self):
        acls = {
            "policies": [
                {"principals": ["alice"], "resources": ["foo.txt"], "allow": True},
            ]
        }
        self.assertFalse(CheckACLs(acls, "bob", "foo.txt"))
        self.assertFalse(CheckACLs(acls, "alice", "other.txt"))

    def test_any_in_principals_matches_any_principal(self):
        acls = {
            "policies": [
                {"principals": [ANY], "resources": ["foo.txt"], "allow": True},
            ]
        }
        self.assertTrue(CheckACLs(acls, "alice", "foo.txt"))
        self.assertTrue(CheckACLs(acls, "bob", "foo.txt"))

    def test_any_in_resources_matches_any_resource(self):
        acls = {
            "policies": [
                {"principals": ["alice"], "resources": [ANY], "allow": True},
            ]
        }
        self.assertTrue(CheckACLs(acls, "alice", "foo.txt"))
        self.assertTrue(CheckACLs(acls, "alice", "bar.txt"))

    def test_first_matching_allow_wins(self):
        acls = {
            "policies": [
                {"principals": [ANY], "resources": [ANY], "allow": False},
                {"principals": [ANY], "resources": ["foo.txt"], "allow": True},
            ]
        }
        self.assertTrue(CheckACLs(acls, "alice", "foo.txt"))

    def test_explicit_deny_no_allow(self):
        acls = {
            "policies": [
                {"principals": [ANY], "resources": ["allowed.txt"], "allow": True},
                {"principals": [ANY], "resources": [ANY], "allow": False},
            ]
        }
        self.assertTrue(CheckACLs(acls, "alice", "allowed.txt"))
        self.assertFalse(CheckACLs(acls, "alice", "denied.txt"))


class TestWalkACLs(unittest.TestCase):
    """Test WalkACLs(repo_dir, principal, resource) - walk path, load acl.json, CheckACLs."""

    def test_allow_all_root_only_resource(self):
        with tempfile.TemporaryDirectory() as tmp:
            self.assertTrue(WalkACLs(tmp, ANY, "root.txt"))

    def test_allow_all_root_and_subdir(self):
        with tempfile.TemporaryDirectory() as tmp:
            root_acl = Path(tmp) / "acl.json"
            root_acl.write_text(
                json.dumps(
                    {
                        "policies": [
                            {"principals": [ANY], "resources": [ANY], "allow": True}
                        ]
                    }
                )
            )
            (Path(tmp) / "a").mkdir()
            (Path(tmp) / "a" / "acl.json").write_text(
                json.dumps(
                    {
                        "policies": [
                            {"principals": [ANY], "resources": [ANY], "allow": True}
                        ]
                    }
                )
            )
            (Path(tmp) / "a" / "b").mkdir()
            (Path(tmp) / "a" / "b" / "acl.json").write_text(
                json.dumps(
                    {
                        "policies": [
                            {"principals": [ANY], "resources": [ANY], "allow": True}
                        ]
                    }
                )
            )
            self.assertTrue(WalkACLs(tmp, ANY, "a/b/file.txt"))

    def test_deny_at_intermediate(self):
        with tempfile.TemporaryDirectory() as tmp:
            (Path(tmp) / "acl.json").write_text(
                json.dumps(
                    {
                        "policies": [
                            {"principals": [ANY], "resources": [ANY], "allow": True}
                        ]
                    }
                )
            )
            sub = Path(tmp) / "a"
            sub.mkdir()
            (sub / "acl.json").write_text(
                json.dumps(
                    {
                        "policies": [
                            {"principals": [ANY], "resources": ["a"], "allow": True},
                            {"principals": [ANY], "resources": [ANY], "allow": False},
                        ]
                    }
                )
            )
            (sub / "b").mkdir()
            self.assertFalse(WalkACLs(tmp, ANY, "a/b/file.txt"))

    def test_missing_acl_returns_false(self):
        with tempfile.TemporaryDirectory() as tmp:
            (Path(tmp) / "a").mkdir()
            (Path(tmp) / "a" / "b").mkdir()
            self.assertFalse(WalkACLs(tmp, ANY, "a/b/file.txt"))

    def test_root_only_acl_root_level_file_never_loads_acl(self):
        with tempfile.TemporaryDirectory() as tmp:
            (Path(tmp) / "acl.json").write_text(
                json.dumps(
                    {
                        "policies": [
                            {"principals": [ANY], "resources": [ANY], "allow": False}
                        ]
                    }
                )
            )
            self.assertTrue(WalkACLs(tmp, ANY, "root.txt"))


class TestCleanDirectoryExceptGit(unittest.TestCase):
    """Test clean_directory_except_git(directory_path) - remove all except .git."""

    def test_removes_files_and_subdirs(self):
        with tempfile.TemporaryDirectory() as tmp:
            (Path(tmp) / "f1").write_text("x")
            (Path(tmp) / "d1").mkdir()
            (Path(tmp) / "d1" / "f2").write_text("y")
            clean_directory_except_git(tmp)
            self.assertFalse((Path(tmp) / "f1").exists())
            self.assertFalse((Path(tmp) / "d1").exists())

    def test_keeps_git_untouched(self):
        with tempfile.TemporaryDirectory() as tmp:
            git_dir = Path(tmp) / ".git"
            git_dir.mkdir()
            (git_dir / "HEAD").write_text("ref: refs/heads/main\n")
            (Path(tmp) / "f1").write_text("x")
            clean_directory_except_git(tmp)
            self.assertFalse((Path(tmp) / "f1").exists())
            self.assertTrue((git_dir / "HEAD").exists())

    def test_non_directory_prints_error_and_returns(self):
        with tempfile.NamedTemporaryFile(delete=False) as f:
            path = f.name
        try:
            with patch("builtins.print") as mock_print:
                clean_directory_except_git(path)
            mock_print.assert_called_once()
            call_arg = str(mock_print.call_args[0][0])
            self.assertIn("not a valid directory", call_arg)
        finally:
            os.unlink(path)


class TestSanitizeEnvContent(unittest.TestCase):
    """Test sanitize_env_content - strip values except GID, UID, VERSION, COMMIT."""

    def test_allowed_keys_keep_values_disallowed_stripped(self):
        inp = (
            "GID=1000\n"
            "NAMECHEAP_API_KEY=secret\n"
            "UID=1000\n"
            "VERSION=v1.0.0\n"
            "COMMIT=abc1234\n"
            "OPENAI_API_KEY=sk-xxx\n"
        )
        out = sanitize_env_content(inp)
        self.assertIn("GID=1000\n", out)
        self.assertIn("UID=1000\n", out)
        self.assertIn("VERSION=v1.0.0\n", out)
        self.assertIn("COMMIT=abc1234\n", out)
        self.assertIn("NAMECHEAP_API_KEY=\n", out)
        self.assertIn("OPENAI_API_KEY=\n", out)

    def test_comments_and_blank_lines_preserved(self):
        inp = "# comment\n\nGID=1\n\nFOO=bar\n"
        out = sanitize_env_content(inp)
        self.assertEqual(out, "# comment\n\nGID=1\n\nFOO=\n")

    def test_trailing_newline_preserved(self):
        inp = "VERSION=v1\n"
        out = sanitize_env_content(inp)
        self.assertTrue(out.endswith("\n"))
        self.assertEqual(out, "VERSION=v1\n")


if __name__ == "__main__":
    unittest.main()
