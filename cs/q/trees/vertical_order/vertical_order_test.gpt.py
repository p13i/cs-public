#!/usr/bin/env python3
# cs/q/trees/vertical_order/vertical_order_test.gpt.py

import unittest
from collections import deque
from typing import Iterable, List, Optional

from vertical_order import Solution, TreeNode


def build_tree(values: Iterable[Optional[int]]) -> Optional[TreeNode]:
    """Build a binary tree from a level-order list with None placeholders."""
    values_iter = iter(values)
    try:
        first = next(values_iter)
    except StopIteration:
        return None
    if first is None:
        return None

    root = TreeNode(first)
    queue = deque([root])
    while queue:
        node = queue.popleft()
        try:
            left_val = next(values_iter)
        except StopIteration:
            break
        if left_val is not None:
            node.left = TreeNode(left_val)
            queue.append(node.left)
        try:
            right_val = next(values_iter)
        except StopIteration:
            break
        if right_val is not None:
            node.right = TreeNode(right_val)
            queue.append(node.right)
    return root


class VerticalOrderTraversalTests(unittest.TestCase):
    maxDiff = None

    def assert_vertical(self, values: List[Optional[int]], expected: List[List[int]]):
        root = build_tree(values)
        result = Solution().verticalTraversal(root)
        self.assertEqual(result, expected)

    def test_leetcode_example_one(self):
        self.assert_vertical(
            [3, 9, 20, None, None, 15, 7],
            [[9], [3, 15], [20], [7]],
        )

    def test_leetcode_example_two_full_tree(self):
        self.assert_vertical(
            [1, 2, 3, 4, 5, 6, 7],
            [[4], [2], [1, 5, 6], [3], [7]],
        )

    def test_same_column_same_row_sorted_by_value(self):
        # Nodes 4 and 5 share column 0, row 2 and must be value-sorted.
        self.assert_vertical(
            [1, 2, 3, None, 4, 5, 6],
            [[2], [1, 4, 5], [3], [6]],
        )

    def test_duplicate_values_preserved_in_ordering(self):
        # Two identical values at the same (row, col) should both appear.
        self.assert_vertical(
            [1, 2, 3, None, 5, 5, 6],
            [[2], [1, 5, 5], [3], [6]],
        )

    def test_single_node_tree(self):
        self.assert_vertical([42], [[42]])

    def test_left_skewed_tree(self):
        self.assert_vertical(
            [7, 6, None, 5, None, 4, None],
            [[4], [5], [6], [7]],
        )

    def test_right_skewed_tree(self):
        self.assert_vertical(
            [1, None, 2, None, 3, None, 4],
            [[1], [2], [3], [4]],
        )

    def test_sparse_tree_with_gaps(self):
        # Mixed depths with missing nodes; verifies column ordering and row ordering.
        self.assert_vertical(
            [0, 8, 1, None, None, 3, 2, None, None, 5],
            [[8], [0, 3], [1, 5], [2]],
        )

    def test_empty_input_returns_empty_list(self):
        self.assert_vertical([], [])


if __name__ == "__main__":
    unittest.main()
