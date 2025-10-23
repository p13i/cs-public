import unittest
from simplify_path import SimplifyPath


class TestSimplifyPath(unittest.TestCase):
    def test_example1(self):
        self.assertEqual(SimplifyPath("/home/"), "/home")

    def test_example2(self):
        self.assertEqual(SimplifyPath("/home//foo/"), "/home/foo")

    def test_example3(self):
        self.assertEqual(
            SimplifyPath("/home/user/Documents/../Pictures"),
            "/home/user/Pictures",
        )

    def test_example4(self):
        self.assertEqual(SimplifyPath("/../"), "/")

    def test_example5(self):
        self.assertEqual(SimplifyPath("/.../a/../b/c/../d/./"), "/.../b/d")


if __name__ == "__main__":
    unittest.main()
