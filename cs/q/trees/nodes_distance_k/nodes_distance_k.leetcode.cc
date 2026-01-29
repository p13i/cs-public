// cs/q/trees/nodes_distance_k/nodes_distance_k.leetcode.cc

// Leetcode-ready single-file solution (copy/paste into the
// editor).
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using std::queue;
using std::unordered_map;
using std::unordered_set;
using std::vector;

/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
class Solution {
 public:
  vector<int> distanceK(TreeNode* root, TreeNode* target,
                        int k) {
    if (root == nullptr || target == nullptr) return {};
    if (k == 0) return {target->val};

    // Build parent map via DFS.
    unordered_map<TreeNode*, TreeNode*> parent;
    parent[root] = nullptr;
    std::vector<TreeNode*> stack;
    stack.push_back(root);
    while (!stack.empty()) {
      TreeNode* node = stack.back();
      stack.pop_back();
      if (node->left) {
        parent[node->left] = node;
        stack.push_back(node->left);
      }
      if (node->right) {
        parent[node->right] = node;
        stack.push_back(node->right);
      }
    }

    // BFS outward from target until distance k.
    queue<TreeNode*> q;
    unordered_set<TreeNode*> visited;
    q.push(target);
    visited.insert(target);

    int dist = 0;
    while (!q.empty() && dist < k) {
      int sz = q.size();
      for (int i = 0; i < sz; ++i) {
        TreeNode* node = q.front();
        q.pop();
        auto try_push = [&](TreeNode* n) {
          if (n && !visited.count(n)) {
            visited.insert(n);
            q.push(n);
          }
        };
        try_push(node->left);
        try_push(node->right);
        auto it = parent.find(node);
        if (it != parent.end()) {
          try_push(it->second);
        }
      }
      ++dist;
    }

    vector<int> out;
    while (!q.empty()) {
      out.push_back(q.front()->val);
      q.pop();
    }
    return out;
  }
};
