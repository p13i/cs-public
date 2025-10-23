# Leetcode 426: Convert Binary Search Tree to Sorted Doubly-Linked List
# https://leetcode.com/problems/convert-binary-search-tree-to-sorted-doubly-linked-list/

from typing import Optional, Any


class Node:
    def __init__(self, val, left=None, right=None):
        self.val = val
        self.left = left
        self.right = right


# Use a pointer wrapper over the first and last nodes to avoid using global/nonlocal variables.
class Pointer:
    value = None

    def __init__(self, value: Optional[Any]):
        self.value = value


def RecursiveSolution(root: Optional[Node]) -> Optional[Node]:
    if not root:
        return None
    first_ptr, last_ptr = Pointer(None), Pointer(None)
    BSTtoDLLRecursive(root, first_ptr, last_ptr)
    last_ptr.value.right = first_ptr.value
    first_ptr.value.left = last_ptr.value
    return first_ptr.value


def BSTtoDLLRecursive(
    node: Optional[Node], first_ptr: Pointer, last_ptr: Pointer
) -> Optional[Node]:
    if not node:
        return None

    BSTtoDLLRecursive(node.left, first_ptr, last_ptr)

    if last_ptr.value:
        # Last element's next should be this node (in-order traversal).
        last_ptr.value.right = node
        # Previous element of this node should link to last element found so far.
        node.left = last_ptr.value
    else:
        # No last, we are at first elem
        first_ptr.value = node
    # This node will be the last-found value thus far.
    last_ptr.value = node

    BSTtoDLLRecursive(node.right, first_ptr, last_ptr)

    return node


def IterativeSolution(root: Optional[Node]) -> Optional[Node]:
    return BSTtoDLLIterative(root)


def BSTtoDLLIterative(node: Optional[Node]) -> Optional[Node]:
    if not node:
        return None

    # Simulate DFS using a stack-based iteration approach
    stack = []
    curr, first, last = node, None, None
    while stack or curr:
        # Collect all left-most-side nodes.
        while curr:
            stack.append(curr)
            curr = curr.left
        curr = stack.pop()
        if last:
            last.right = curr
            curr.left = last
        else:
            # This is the first elem
            first = curr
        last = curr
        # Go to right-subtree that has larger values than left side.
        curr = curr.right

    if first:
        first.left = last
    if last:
        last.right = first

    return first


class Solution:
    def treeToDoublyList(self, root: "Optional[Node]") -> "Optional[Node]":
        USE_RECURSIVE_APPROACH = False
        if USE_RECURSIVE_APPROACH:
            return RecursiveSolution(root)
        else:
            return IterativeSolution(root)
