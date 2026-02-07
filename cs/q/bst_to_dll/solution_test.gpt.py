#!/usr/bin/env python3
# cs/q/bst_to_dll/solution_test.gpt.py
import importlib.util
import unittest
from pathlib import Path

_mod_path = Path(__file__).resolve().parent / "solution.py"
_spec = importlib.util.spec_from_file_location("solution", _mod_path)
_mod = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(_mod)

Node = _mod.Node
RecursiveSolution = _mod.RecursiveSolution
IterativeSolution = _mod.IterativeSolution


def bst_from_sorted(vals):
    """Build a balanced BST from a sorted list."""
    if not vals:
        return None
    mid = len(vals) // 2
    node = Node(vals[mid])
    node.left = bst_from_sorted(vals[:mid])
    node.right = bst_from_sorted(vals[mid + 1 :])
    return node


def dll_to_list(head):
    """Collect values from a circular doubly-linked list."""
    if not head:
        return []
    result = [head.val]
    curr = head.right
    while curr is not head:
        result.append(curr.val)
        curr = curr.right
    return result


class RecursiveSolutionTest(unittest.TestCase):
    def test_none(self):
        self.assertIsNone(RecursiveSolution(None))

    def test_single_node(self):
        root = Node(1)
        head = RecursiveSolution(root)
        self.assertEqual(head.val, 1)
        self.assertIs(head.left, head)
        self.assertIs(head.right, head)

    def test_three_nodes(self):
        root = Node(2, Node(1), Node(3))
        head = RecursiveSolution(root)
        self.assertEqual(dll_to_list(head), [1, 2, 3])

    def test_balanced_tree(self):
        root = bst_from_sorted([1, 2, 3, 4, 5])
        head = RecursiveSolution(root)
        self.assertEqual(dll_to_list(head), [1, 2, 3, 4, 5])


class IterativeSolutionTest(unittest.TestCase):
    def test_none(self):
        self.assertIsNone(IterativeSolution(None))

    def test_single_node(self):
        root = Node(1)
        head = IterativeSolution(root)
        self.assertEqual(head.val, 1)
        self.assertIs(head.left, head)
        self.assertIs(head.right, head)

    def test_three_nodes(self):
        root = Node(2, Node(1), Node(3))
        head = IterativeSolution(root)
        self.assertEqual(dll_to_list(head), [1, 2, 3])

    def test_balanced_tree(self):
        root = bst_from_sorted([1, 2, 3, 4, 5])
        head = IterativeSolution(root)
        self.assertEqual(dll_to_list(head), [1, 2, 3, 4, 5])

    def test_left_skew(self):
        root = Node(3, Node(2, Node(1)))
        head = IterativeSolution(root)
        self.assertEqual(dll_to_list(head), [1, 2, 3])

    def test_right_skew(self):
        root = Node(1, None, Node(2, None, Node(3)))
        head = IterativeSolution(root)
        self.assertEqual(dll_to_list(head), [1, 2, 3])


if __name__ == "__main__":
    unittest.main()
