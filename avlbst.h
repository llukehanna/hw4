#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
 * A special kind of node for an AVL tree, which adds the balance as a data member.
 * (Do not modify this class.)
 */
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's balance.
    int8_t getBalance() const;
    void setBalance(int8_t balance);
    void updateBalance(int8_t diff);

    // Overridden getters for parent, left, and right.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // balance factor
};

/*-------------------------------------------------
  Begin implementations for the AVLNode class.
-------------------------------------------------*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{ }

template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode() { }

template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const {
    return balance_;
}

template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance) {
    balance_ = balance;
}

template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff) {
    balance_ += diff;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getParent() const {
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getLeft() const {
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getRight() const {
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}

/**
 * AVLTree builds on BinarySearchTree. It implements AVL insert and remove.
 */
template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value>& new_item);
    virtual void remove(const Key& key);

protected:
    // Override nodeSwap so that balance factors are swapped.
    virtual void nodeSwap(AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Helper functions for AVL insertion and removal.
    AVLNode<Key,Value>* insertHelper(AVLNode<Key,Value>* root, const std::pair<const Key, Value>& new_item, bool &taller);
    AVLNode<Key,Value>* removeHelper(AVLNode<Key,Value>* root, const Key& key, bool &shorter, bool &success);

    // Rotation and rebalance helpers.
    AVLNode<Key,Value>* rotateLeft(AVLNode<Key,Value>* root);
    AVLNode<Key,Value>* rotateRight(AVLNode<Key,Value>* root);
    AVLNode<Key,Value>* balanceLeft(AVLNode<Key,Value>* root);
    AVLNode<Key,Value>* balanceRight(AVLNode<Key,Value>* root);

    // Helper to recompute height from children (used for balance updates).
    int calcHeight(AVLNode<Key,Value>* node);
};

/*-------------------------------------------------
  Implementation for AVLTree::insert
-------------------------------------------------*/
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value>& new_item)
{
    bool taller = false;
    AVLNode<Key,Value>* avlRoot = static_cast<AVLNode<Key,Value>*>(this->root_);
    avlRoot = insertHelper(avlRoot, new_item, taller);
    if(avlRoot != nullptr)
        avlRoot->setParent(nullptr);
    this->root_ = avlRoot;
}

template<class Key, class Value>
AVLNode<Key,Value>* AVLTree<Key,Value>::insertHelper(AVLNode<Key,Value>* root, const std::pair<const Key, Value>& new_item, bool &taller)
{
    if(root == nullptr) {
         taller = true;
         return new AVLNode<Key,Value>(new_item.first, new_item.second, nullptr);
    }
    if(new_item.first < root->getKey()) {
         AVLNode<Key,Value>* leftChild = insertHelper(static_cast<AVLNode<Key,Value>*>(root->getLeft()), new_item, taller);
         root->setLeft(leftChild);
         if(leftChild != nullptr)
              leftChild->setParent(root);
         if(taller) {
              root->updateBalance(1);
              if(root->getBalance() == 0)
                   taller = false;
              else if(root->getBalance() == 2) {
                   root = balanceLeft(root);
                   taller = false;
              }
         }
    } else if(new_item.first > root->getKey()) {
         AVLNode<Key,Value>* rightChild = insertHelper(static_cast<AVLNode<Key,Value>*>(root->getRight()), new_item, taller);
         root->setRight(rightChild);
         if(rightChild != nullptr)
              rightChild->setParent(root);
         if(taller) {
              root->updateBalance(-1);
              if(root->getBalance() == 0)
                   taller = false;
              else if(root->getBalance() == -2) {
                   root = balanceRight(root);
                   taller = false;
              }
         }
    } else {
         // Key already exists: update value.
         root->setValue(new_item.second);
         taller = false;
    }
    return root;
}

/*-------------------------------------------------
  Implementation for AVLTree::remove
-------------------------------------------------*/
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    bool shorter = false;
    bool success = false;
    AVLNode<Key,Value>* avlRoot = static_cast<AVLNode<Key,Value>*>(this->root_);
    avlRoot = removeHelper(avlRoot, key, shorter, success);
    if(avlRoot != nullptr)
         avlRoot->setParent(nullptr);
    this->root_ = avlRoot;
}

template<class Key, class Value>
AVLNode<Key,Value>* AVLTree<Key,Value>::removeHelper(AVLNode<Key,Value>* root, const Key& key, bool &shorter, bool &success)
{
    if(root == nullptr) {
         shorter = false;
         success = false;
         return nullptr;
    }
    if(key < root->getKey()) {
         root->setLeft(removeHelper(static_cast<AVLNode<Key,Value>*>(root->getLeft()), key, shorter, success));
         if(root->getLeft() != nullptr)
              root->getLeft()->setParent(root);
         if(shorter)
              root->updateBalance(-1);
    } else if(key > root->getKey()) {
         root->setRight(removeHelper(static_cast<AVLNode<Key,Value>*>(root->getRight()), key, shorter, success));
         if(root->getRight() != nullptr)
              root->getRight()->setParent(root);
         if(shorter)
              root->updateBalance(1);
    } else {
         // Node found.
         success = true;
         if(root->getLeft() == nullptr || root->getRight() == nullptr) {
              AVLNode<Key,Value>* temp = (root->getLeft() != nullptr) ?
                    static_cast<AVLNode<Key,Value>*>(root->getLeft()) :
                    static_cast<AVLNode<Key,Value>*>(root->getRight());
              delete root;
              shorter = true;
              return temp;
         } else {
              // Node with two children.
              AVLNode<Key,Value>* pred = static_cast<AVLNode<Key,Value>*>(BinarySearchTree<Key,Value>::predecessor(root));
              bool isAdjacent = (root->getLeft() == pred);
              this->nodeSwap(root, pred);
              root = pred;
              // For adjacent case, the duplicate key is now in the right subtree.
              if(isAdjacent)
                  root->setRight(removeHelper(root->getRight(), key, shorter, success));
              else
                  root->setLeft(removeHelper(root->getLeft(), key, shorter, success));
              if(root->getLeft() != nullptr)
                  root->getLeft()->setParent(root);
              if(root->getRight() != nullptr)
                  root->getRight()->setParent(root);
         }
    }
    // Recompute balance factor from scratch.
    root->setBalance(calcHeight(static_cast<AVLNode<Key,Value>*>(root->getLeft())) -
                     calcHeight(static_cast<AVLNode<Key,Value>*>(root->getRight())));
    int bal = root->getBalance();
    if(bal == 2) {
         root = balanceLeft(root);
         shorter = false;
    } else if(bal == -2) {
         root = balanceRight(root);
         shorter = false;
    } else {
         if(bal == 0)
              shorter = true;
         else
              shorter = false;
    }
    return root;
}

/*-------------------------------------------------
  Rotation and Rebalance Helper Functions
-------------------------------------------------*/
template<class Key, class Value>
AVLNode<Key,Value>* AVLTree<Key,Value>::rotateRight(AVLNode<Key,Value>* root)
{
    AVLNode<Key,Value>* leftChild = static_cast<AVLNode<Key,Value>*>(root->getLeft());
    root->setLeft(leftChild->getRight());
    if(leftChild->getRight() != nullptr)
         leftChild->getRight()->setParent(root);
    leftChild->setRight(root);
    leftChild->setParent(root->getParent());
    root->setParent(leftChild);

    root->setBalance(calcHeight(static_cast<AVLNode<Key,Value>*>(root->getLeft())) -
                     calcHeight(static_cast<AVLNode<Key,Value>*>(root->getRight())));
    leftChild->setBalance(calcHeight(static_cast<AVLNode<Key,Value>*>(leftChild->getLeft())) -
                          calcHeight(static_cast<AVLNode<Key,Value>*>(leftChild->getRight())));
    return leftChild;
}

template<class Key, class Value>
AVLNode<Key,Value>* AVLTree<Key,Value>::rotateLeft(AVLNode<Key,Value>* root)
{
    AVLNode<Key,Value>* rightChild = static_cast<AVLNode<Key,Value>*>(root->getRight());
    root->setRight(rightChild->getLeft());
    if(rightChild->getLeft() != nullptr)
         rightChild->getLeft()->setParent(root);
    rightChild->setLeft(root);
    rightChild->setParent(root->getParent());
    root->setParent(rightChild);

    root->setBalance(calcHeight(static_cast<AVLNode<Key,Value>*>(root->getLeft())) -
                     calcHeight(static_cast<AVLNode<Key,Value>*>(root->getRight())));
    rightChild->setBalance(calcHeight(static_cast<AVLNode<Key,Value>*>(rightChild->getLeft())) -
                           calcHeight(static_cast<AVLNode<Key,Value>*>(rightChild->getRight())));
    return rightChild;
}

template<class Key, class Value>
AVLNode<Key,Value>* AVLTree<Key,Value>::balanceLeft(AVLNode<Key,Value>* root)
{
    AVLNode<Key,Value>* leftChild = static_cast<AVLNode<Key,Value>*>(root->getLeft());
    if(leftChild->getBalance() >= 0) {
         return rotateRight(root);
    } else {
         root->setLeft(rotateLeft(leftChild));
         if(root->getLeft() != nullptr)
              root->getLeft()->setParent(root);
         return rotateRight(root);
    }
}

template<class Key, class Value>
AVLNode<Key,Value>* AVLTree<Key,Value>::balanceRight(AVLNode<Key,Value>* root)
{
    AVLNode<Key,Value>* rightChild = static_cast<AVLNode<Key,Value>*>(root->getRight());
    if(rightChild->getBalance() <= 0) {
         return rotateLeft(root);
    } else {
         root->setRight(rotateRight(rightChild));
         if(root->getRight() != nullptr)
              root->getRight()->setParent(root);
         return rotateLeft(root);
    }
}

template<class Key, class Value>
int AVLTree<Key,Value>::calcHeight(AVLNode<Key,Value>* node)
{
    if(node == nullptr)
         return 0;
    int leftH = calcHeight(static_cast<AVLNode<Key,Value>*>(node->getLeft()));
    int rightH = calcHeight(static_cast<AVLNode<Key,Value>*>(node->getRight()));
    return 1 + std::max(leftH, rightH);
}

/*-------------------------------------------------
  Override nodeSwap for AVLNodes.
  This implementation provides special handling for adjacent nodes.
-------------------------------------------------*/
template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    // If n2 is the direct left child of n1.
    if(n1->getLeft() == n2) {
       AVLNode<Key,Value>* P = static_cast<AVLNode<Key,Value>*>(n1->getParent());
       AVLNode<Key,Value>* temp = n2->getRight();
       n2->setRight(n1);
       n1->setParent(n2);
       n1->setLeft(temp);
       if(temp) temp->setParent(n1);
       n2->setParent(P);
       if(P) {
           if(P->getLeft() == n1)
              P->setLeft(n2);
           else if(P->getRight() == n1)
              P->setRight(n2);
       }
       int8_t tempB = n1->getBalance();
       n1->setBalance(n2->getBalance());
       n2->setBalance(tempB);
       if(this->root_ == n1)
           this->root_ = n2;
       return;
    }
    // Else if n2 is the direct right child of n1.
    else if(n1->getRight() == n2) {
       AVLNode<Key,Value>* P = static_cast<AVLNode<Key,Value>*>(n1->getParent());
       AVLNode<Key,Value>* temp = n2->getLeft();
       n2->setLeft(n1);
       n1->setParent(n2);
       n1->setRight(temp);
       if(temp) temp->setParent(n1);
       n2->setParent(P);
       if(P) {
           if(P->getLeft() == n1)
              P->setLeft(n2);
           else if(P->getRight() == n1)
              P->setRight(n2);
       }
       int8_t tempB = n1->getBalance();
       n1->setBalance(n2->getBalance());
       n2->setBalance(tempB);
       if(this->root_ == n1)
           this->root_ = n2;
       return;
    }
    else {
       // For non-adjacent nodes, use the base BST nodeSwap.
       BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
       int8_t tempB = n1->getBalance();
       n1->setBalance(n2->getBalance());
       n2->setBalance(tempB);
    }
}

#endif
