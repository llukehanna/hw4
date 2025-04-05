// equal-paths.cpp
#include "equal-paths.h"

// Helper function that returns the depth of leaves in the subtree if uniform,
// or -1 if the depths differ.
int checkPaths(Node* node, int depth)
{
    if (node == nullptr)  // Should not happen in our recursion
        return depth; // For safety; we won't call this on a null root.

    // If it's a leaf, return its depth.
    if (node->left == nullptr && node->right == nullptr)
        return depth;

    int leftDepth = -1;
    int rightDepth = -1;

    // Recurse on the left subtree if it exists.
    if (node->left)
    {
        leftDepth = checkPaths(node->left, depth + 1);
        if (leftDepth == -1)
            return -1;
    }
    
    // Recurse on the right subtree if it exists.
    if (node->right)
    {
        rightDepth = checkPaths(node->right, depth + 1);
        if (rightDepth == -1)
            return -1;
    }
    
    // If both children exist, they must have the same leaf depth.
    if (node->left && node->right)
    {
        if (leftDepth != rightDepth)
            return -1;
        return leftDepth; // They’re equal.
    }
    
    // If only one child exists, return its depth.
    return node->left ? leftDepth : rightDepth;
}

bool equalPaths(Node * root)
{
    // An empty tree is considered to have equal paths.
    if (root == nullptr)
        return true;
    
    // Check the subtree. If the helper returns -1, paths are not equal.
    return checkPaths(root, 0) != -1;
}
