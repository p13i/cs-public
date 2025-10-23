import unittest
from simplify_path import SimplifyPath


class TestSimplifyPath(unittest.TestCase):
    # Basic functionality tests
    def test_root_directory(self):
        self.assertEqual(SimplifyPath("/"), "/")

    def test_trailing_slash(self):
        self.assertEqual(SimplifyPath("/home/"), "/home")

    def test_multiple_trailing_slashes(self):
        self.assertEqual(SimplifyPath("/abc/def///"), "/abc/def")

    def test_multiple_consecutive_slashes(self):
        self.assertEqual(SimplifyPath("/home//foo/bar///baz"), "/home/foo/bar/baz")

    def test_only_multiple_slashes(self):
        self.assertEqual(SimplifyPath("//////"), "/")

    def test_single_current_dir_root(self):
        self.assertEqual(SimplifyPath("/./"), "/")

    def test_single_current_dir_in_path(self):
        self.assertEqual(SimplifyPath("/foo/./bar"), "/foo/bar")

    def test_multiple_current_dirs(self):
        self.assertEqual(SimplifyPath("/././foo/."), "/foo")

    # Parent directory (..) handling
    def test_single_parent_dir(self):
        self.assertEqual(SimplifyPath("/foo/bar/.."), "/foo")

    def test_single_parent_dir_with_trailing(self):
        self.assertEqual(SimplifyPath("/foo/bar/../"), "/foo")

    def test_multiple_parent_dirs(self):
        # Going up two levels from /foo/bar/baz
        self.assertEqual(SimplifyPath("/foo/bar/baz/../.."), "/foo")

    def test_parent_dir_at_root(self):
        self.assertEqual(SimplifyPath("/../"), "/")
        self.assertEqual(SimplifyPath("/.."), "/")

    def test_multiple_parent_dir_at_root(self):
        self.assertEqual(SimplifyPath("/../../.."), "/")

    def test_parent_then_directory(self):
        # Going up beyond root then into folder
        self.assertEqual(SimplifyPath("/../../folder"), "/folder")

    # Mixed current (.) and parent (..) references
    def test_mixed_current_and_parent(self):
        self.assertEqual(SimplifyPath("/a/./b/../c/"), "/a/c")

    def test_mixed_numbers_and_dirs(self):
        self.assertEqual(SimplifyPath("/1/2/../3/./4/../5"), "/1/3/5")

    def test_double_slash_and_dots_combined(self):
        self.assertEqual(SimplifyPath("/a//b/../..//c/."), "/c")

    def test_multiple_slashes_with_dot_name(self):
        # "///...//." -> multiple slashes compress, "..." is a directory, last "." removed
        self.assertEqual(SimplifyPath("///...//."), "/...")

    # Directory names with periods (not just "." or "..")
    def test_directory_named_triple_dots(self):
        # "..." is a valid directory name
        self.assertEqual(SimplifyPath("/.../"), "/...")

    def test_directory_named_four_dots(self):
        self.assertEqual(SimplifyPath("/...."), "/....")

    def test_directory_name_starts_with_dot(self):
        # .hidden is a valid name (not a current-dir marker)
        self.assertEqual(SimplifyPath("/.hidden"), "/.hidden")

    def test_hidden_directory_navigation(self):
        # .hidden treated as normal dir, then .. goes up to root
        self.assertEqual(SimplifyPath("/.hidden/../visible"), "/visible")

    def test_name_starts_with_double_dot(self):
        # "..hidden" should be treated as a regular name, not as parent reference
        self.assertEqual(SimplifyPath("/..hidden"), "/..hidden")

    def test_name_ends_with_double_dot(self):
        self.assertEqual(SimplifyPath("/hidden.."), "/hidden..")

    def test_remove_name_ending_with_double_dot(self):
        # "hidden.." is a dir name, ".." removes it
        self.assertEqual(SimplifyPath("/hidden../.."), "/")

    def test_name_with_double_dot_inside(self):
        # "..bar" is a normal name starting with ".."
        self.assertEqual(SimplifyPath("/foo/..bar/.."), "/foo")

    def test_name_with_dot_inside(self):
        # ".bar" is a normal name (starts with '.', more chars after)
        self.assertEqual(SimplifyPath("/foo/.bar/.."), "/foo")

    def test_name_with_dots_and_underscore(self):
        # Complex name with dots and underscores, followed by parent and current dir ops
        path = "/folder.name_/sub..dir/../file.txt/./"
        self.assertEqual(SimplifyPath(path), "/folder.name_/file.txt")

    # Stress tests for large inputs
    def test_max_length_single_segment(self):
        # Path of length 3000: "/" + 2999 "a"s
        path = "/" + "a" * 2999
        expected = path  # only one huge directory name, no changes expected
        self.assertEqual(SimplifyPath(path), expected)

    def test_max_segments_path(self):
        # Path with 1500 "a" directories: "/a/a/a/... (1500 times)"
        segments = ["a"] * 1500
        path = "/" + "/".join(segments)
        expected = "/" + "/".join(segments)  # same as path (already canonical)
        self.assertEqual(SimplifyPath(path), expected)

    def test_many_parent_operations(self):
        # 1000 parent directory operations from root
        ups = [".."] * 1000
        path = "/" + "/".join(ups)
        # Can't go above root, should remain "/"
        self.assertEqual(SimplifyPath(path), "/")

    def test_alternating_nested_operations(self):
        # Pattern: 10 "x" dirs then 10 "..", repeated 60 times (max length ~3000)
        parts = []
        for i in range(60):
            parts.extend(["x"] * 10)
            parts.extend([".."] * 10)
        path = "/" + "/".join(parts)
        # Each block of 10 "x" followed by 10 ".." cancels out, ending at root
        self.assertEqual(SimplifyPath(path), "/")


if __name__ == "__main__":
    unittest.main()
