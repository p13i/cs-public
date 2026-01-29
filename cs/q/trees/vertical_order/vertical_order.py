#!/usr/bin/env python3
# cs/q/trees/vertical_order/vertical_order.py
# Extracted from Gmail - https://leetcode.com/problems/vertical-order-traversal-of-a-binary-tree/

from __future__ import annotations

from typing import Dict, List, Optional


class TreeNode:
    def __init__(
        self,
        val: int = 0,
        left: Optional["TreeNode"] = None,
        right: Optional["TreeNode"] = None,
    ):
        self.val = val
        self.left = left
        self.right = right


# Index: result[col][row] = [values, ...]
IR = Dict[int, Dict[int, List[int]]]


def find_cols_len(ir: IR) -> int:
    """Finds the max length of the cols dimension."""
    min_c, max_c = 0, 0
    for c in ir:
        min_c = min(min_c, c)
        max_c = max(max_c, c)
    return max_c - min_c + 1


def as_row(d: Dict[int, List[int]]) -> List[int]:
    """
    The data in a column is indexed by the row number.
    This method converts a dict representation of a row into an array
    by using the dictionary keys as the array indices.
    """
    keys = sorted(d.keys())
    row: List[int] = []
    for k in keys:
        vals = d[k]
        row.extend(sorted(vals))
    return row


def ir_to_solution(ir: IR) -> List[List[int]]:
    if not ir:
        return []
    return [as_row(ir[c]) for c in sorted(ir.keys())]


def vertical_order(root: Optional[TreeNode], col: int = 0, row: int = 0) -> IR:
    """
    Driver for solution.
    With a BFS, we will always encounter nodes with successive row values as
    we continue down level by level.
    """
    this_result: IR = {}
    if not root:
        return this_result

    this_result[col] = {row: [root.val]}
    left_result = vertical_order(root.left, col=col - 1, row=row + 1)
    right_result = vertical_order(root.right, col=col + 1, row=row + 1)
    merged_results = merge_results(this_result, left_result, right_result)
    return merged_results


def merge_results(result: IR, left_result: IR, right_result: IR) -> IR:
    new_result: IR = {}

    def merge_results_helper(source: IR, destination: IR) -> None:
        for c in source:
            if c not in destination:
                destination[c] = {}
            for r in source[c]:
                if r not in destination[c]:
                    destination[c][r] = []
                destination[c][r] = merge_in_value(source, destination, c, r)

    merge_results_helper(result, new_result)
    merge_results_helper(left_result, new_result)
    merge_results_helper(right_result, new_result)
    return new_result


def merge_in_value(source: IR, destination: IR, c: int, r: int) -> List[int]:
    return destination[c][r] + source[c][r]


class Solution:
    def verticalTraversal(self, root: Optional[TreeNode]) -> List[List[int]]:
        # Basic idea: each node produces at least three lists: those for col values
        # less than itself, those with equal col values, and those with col values
        # greater than itself. Each node is responsible for merging the sublists
        # returned by the helper method.
        ir = vertical_order(root)
        solution = ir_to_solution(ir)
        return solution
