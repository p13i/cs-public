# Leetcode 987. Vertical Order Traversal of a Binary Tree

https://leetcode.com/problems/vertical-order-traversal-of-a-binary-tree/

**Difficulty:** Hard  
**Topics:** Hash Table, Tree, Depth-First Search,
Breadth-First Search, Sorting, Binary Tree

## Description

Given the `root` of a binary tree, return the vertical order
traversal. For each node at position `(row, col)`, its
children are at `(row+1, col-1)` and `(row+1, col+1)`.
Columns are reported from leftmost to rightmost; if multiple
nodes share the same `(row, col)`, order them by value.

## Examples

**Example 1**  
ASCII tree:

```
    3
   / \
  9  20
     / \
    15  7
```

Input: `root = [3,9,20,null,null,15,7]`  
Output: `[[9],[3,15],[20],[7]]`

**Example 2**  
ASCII tree:

```
      1
     / \
    2   3
   / \ / \
  4  5 6  7
```

Input: `root = [1,2,3,4,5,6,7]`  
Output: `[[4],[2],[1,5,6],[3],[7]]`

**Example 3**  
ASCII tree:

```
      1
     / \
    2   3
   / \ / \
  4  6 5  7
```

Input: `root = [1,2,3,4,6,5,7]`  
Output: `[[4],[2],[1,5,6],[3],[7]]`

## Constraints

- `1 <= #nodes <= 1000`
- `0 <= Node.val <= 1000`

---

# Solutions

**@p13i**:

Every node has a "rank" which is the number of steps away
from the root that the node. Being to the left adds a rank
of -1 and being to the right adds a rank of +1.

The result of this algorithm is a 2D array where each
subarray is a list of nodes from the top to bottom of nodes
of the same rank.

Each given node can produce three lists:

1. A list of nodes down from the left child down that have
   rank -1 relative to the given node.
2. A list of nodes down from the given
3. A list of nodes down from the right child down that have
   rank +1 relative to the given node.

After receiving each of the three sublists, the caller can
merge these lists into the larger return list.

Pseudo-code:

1.
