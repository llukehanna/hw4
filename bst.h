#ifndef BST_H
#define BST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <utility>
#include <algorithm>  // for std::max
#include <cmath>      // for std::abs

/**
 * A templated class for a Node in a search tree.
 * The getters for parent/left/right are virtual so that they can be
 * overridden for future kinds of search trees, such as Red Black trees,
 * Splay trees, and AVL trees.
 */
template <typename Key, typename Value>
class Node {
public:
    Node(const Key& key, const Value& value, Node<Key, Value>* parent);
    virtual ~Node();

    const std::pair<const Key, Value>& getItem() const;
    std::pair<const Key, Value>& getItem();
    const Key& getKey() const;
    const Value& getValue() const;
    Value& getValue();

    virtual Node<Key, Value>* getParent() const;
    virtual Node<Key, Value>* getLeft() const;
    virtual Node<Key, Value>* getRight() const;

    void setParent(Node<Key, Value>* parent);
    void setLeft(Node<Key, Value>* left);
    void setRight(Node<Key, Value>* right);
    void setValue(const Value &value);

protected:
    std::pair<const Key, Value> item_;
    Node<Key, Value>* parent_;
    Node<Key, Value>* left_;
    Node<Key, Value>* right_;
};

/*
  -----------------------------------------
  Begin implementations for the Node class.
  -----------------------------------------
*/
template<typename Key, typename Value>
Node<Key, Value>::Node(const Key& key, const Value& value, Node<Key, Value>* parent) :
    item_(key, value),
    parent_(parent),
    left_(NULL),
    right_(NULL)
{}

template<typename Key, typename Value>
Node<Key, Value>::~Node() {}

template<typename Key, typename Value>
const std::pair<const Key, Value>& Node<Key, Value>::getItem() const {
    return item_;
}

template<typename Key, typename Value>
std::pair<const Key, Value>& Node<Key, Value>::getItem() {
    return item_;
}

template<typename Key, typename Value>
const Key& Node<Key, Value>::getKey() const {
    return item_.first;
}

template<typename Key, typename Value>
const Value& Node<Key, Value>::getValue() const {
    return item_.second;
}

template<typename Key, typename Value>
Value& Node<Key, Value>::getValue() {
    return item_.second;
}

template<typename Key, typename Value>
Node<Key, Value>* Node<Key, Value>::getParent() const {
    return parent_;
}

template<typename Key, typename Value>
Node<Key, Value>* Node<Key, Value>::getLeft() const {
    return left_;
}

template<typename Key, typename Value>
Node<Key, Value>* Node<Key, Value>::getRight() const {
    return right_;
}

template<typename Key, typename Value>
void Node<Key, Value>::setParent(Node<Key, Value>* parent) {
    parent_ = parent;
}

template<typename Key, typename Value>
void Node<Key, Value>::setLeft(Node<Key, Value>* left) {
    left_ = left;
}

template<typename Key, typename Value>
void Node<Key, Value>::setRight(Node<Key, Value>* right) {
    right_ = right;
}

template<typename Key, typename Value>
void Node<Key, Value>::setValue(const Value& value) {
    item_.second = value;
}

/*
  ---------------------------------------
  End implementations for the Node class.
  ---------------------------------------
*/

/**
 * A templated unbalanced binary search tree.
 */
template <typename Key, typename Value>
class BinarySearchTree {
public:
    BinarySearchTree();                   // constructor
    virtual ~BinarySearchTree();          // destructor
    virtual void insert(const std::pair<const Key, Value>& keyValuePair);
    virtual void remove(const Key& key);
    void clear();
    bool isBalanced() const;
    void print() const;
    bool empty() const;

    template<typename PPKey, typename PPValue>
    friend void prettyPrintBST(BinarySearchTree<PPKey, PPValue> & tree);

public:
    /**
    * An internal iterator class for traversing the contents of the BST.
    */
    class iterator {
    public:
        iterator(Node<Key,Value>* ptr = nullptr);
        iterator();

        std::pair<const Key,Value>& operator*() const;
        std::pair<const Key,Value>* operator->() const;

        bool operator==(const iterator& rhs) const;
        bool operator!=(const iterator& rhs) const;

        iterator& operator++();

    protected:
        friend class BinarySearchTree<Key, Value>;
        Node<Key, Value>* current_;
    };

public:
    iterator begin() const;
    iterator end() const;
    iterator find(const Key& key) const;
    Value& operator[](const Key& key);
    Value const & operator[](const Key& key) const;

protected:
    // Mandatory helper functions
    Node<Key, Value>* internalFind(const Key& k) const;
    Node<Key, Value>* getSmallestNode() const;
    static Node<Key, Value>* predecessor(Node<Key, Value>* current);
    // Static successor function for the iterator.
    static Node<Key, Value>* successor(Node<Key, Value>* current) {
        if (!current) return nullptr;
        if (current->getRight() != nullptr) {
            Node<Key, Value>* temp = current->getRight();
            while (temp->getLeft() != nullptr)
                temp = temp->getLeft();
            return temp;
        }
        Node<Key, Value>* parent = current->getParent();
        while (parent != nullptr && current == parent->getRight()) {
            current = parent;
            parent = parent->getParent();
        }
        return parent;
    }

    // Provided helper functions
    virtual void printRoot(Node<Key, Value>* r) const;
    virtual void nodeSwap(Node<Key,Value>* n1, Node<Key,Value>* n2);

    // Helper for clear()
    void clearHelper(Node<Key, Value>* node);

    // Helper for isBalanced()
    int isBalancedHelper(Node<Key, Value>* node) const;

protected:
    Node<Key, Value>* root_;
};

/*
--------------------------------------------------------------
Begin implementations for the BinarySearchTree::iterator class.
--------------------------------------------------------------
*/
template<class Key, class Value>
BinarySearchTree<Key, Value>::iterator::iterator(Node<Key,Value>* ptr)
    : current_(ptr)
{}

template<class Key, class Value>
BinarySearchTree<Key, Value>::iterator::iterator()
    : current_(nullptr)
{}

template<class Key, class Value>
std::pair<const Key,Value>& BinarySearchTree<Key, Value>::iterator::operator*() const {
    return current_->getItem();
}

template<class Key, class Value>
std::pair<const Key,Value>* BinarySearchTree<Key, Value>::iterator::operator->() const {
    return &(current_->getItem());
}

template<class Key, class Value>
bool BinarySearchTree<Key, Value>::iterator::operator==(const iterator& rhs) const {
    return current_ == rhs.current_;
}

template<class Key, class Value>
bool BinarySearchTree<Key, Value>::iterator::operator!=(const iterator& rhs) const {
    return current_ != rhs.current_;
}

template<class Key, class Value>
typename BinarySearchTree<Key, Value>::iterator&
BinarySearchTree<Key, Value>::iterator::operator++() {
    current_ = BinarySearchTree<Key, Value>::successor(current_);
    return *this;
}

/*
-------------------------------------------------------------
End implementations for the BinarySearchTree::iterator class.
-------------------------------------------------------------
*/

/*
-----------------------------------------------------
Begin implementations for the BinarySearchTree class.
-----------------------------------------------------
*/
template<class Key, class Value>
BinarySearchTree<Key, Value>::BinarySearchTree()
    : root_(nullptr)
{}

template<typename Key, class Value>
BinarySearchTree<Key, Value>::~BinarySearchTree() {
    clear();
}

template<class Key, class Value>
bool BinarySearchTree<Key, Value>::empty() const {
    return root_ == nullptr;
}

template<class Key, class Value>
typename BinarySearchTree<Key, Value>::iterator
BinarySearchTree<Key, Value>::begin() const {
    return iterator(getSmallestNode());
}

template<class Key, class Value>
typename BinarySearchTree<Key, Value>::iterator
BinarySearchTree<Key, Value>::end() const {
    return iterator(nullptr);
}

template<class Key, class Value>
typename BinarySearchTree<Key, Value>::iterator
BinarySearchTree<Key, Value>::find(const Key& key) const {
    return iterator(internalFind(key));
}

template<class Key, class Value>
Value& BinarySearchTree<Key, Value>::operator[](const Key& key) {
    Node<Key, Value>* curr = internalFind(key);
    if(curr == nullptr) throw std::out_of_range("Invalid key");
    return curr->getValue();
}

template<class Key, class Value>
Value const & BinarySearchTree<Key, Value>::operator[](const Key& key) const {
    Node<Key, Value>* curr = internalFind(key);
    if(curr == nullptr) throw std::out_of_range("Invalid key");
    return curr->getValue();
}

/* 
-----------------------------------------------------
Mandatory Helper Functions (Definitions)
-----------------------------------------------------
*/
template<typename Key, class Value>
Node<Key, Value>* BinarySearchTree<Key, Value>::internalFind(const Key& key) const {
    Node<Key, Value>* current = root_;
    while(current != nullptr) {
        if(key < current->getKey())
            current = current->getLeft();
        else if (current->getKey() < key)
            current = current->getRight();
        else
            return current;
    }
    return nullptr;
}

template<typename Key, class Value>
Node<Key, Value>* BinarySearchTree<Key, Value>::getSmallestNode() const {
    Node<Key, Value>* current = root_;
    if(current == nullptr) return nullptr;
    while(current->getLeft() != nullptr)
        current = current->getLeft();
    return current;
}

template<typename Key, class Value>
Node<Key, Value>* BinarySearchTree<Key, Value>::predecessor(Node<Key, Value>* current) {
    if (!current) return nullptr;
    if(current->getLeft() != nullptr) {
        current = current->getLeft();
        while(current->getRight() != nullptr)
            current = current->getRight();
        return current;
    }
    Node<Key, Value>* parent = current->getParent();
    while(parent != nullptr && current == parent->getLeft()) {
        current = parent;
        parent = parent->getParent();
    }
    return parent;
}

/*
-----------------------------------------------------
Core BST Functions: insert, remove, clear, isBalanced
-----------------------------------------------------
*/
template<class Key, class Value>
void BinarySearchTree<Key, Value>::insert(const std::pair<const Key, Value>& keyValuePair) {
    if (root_ == nullptr) {
        root_ = new Node<Key, Value>(keyValuePair.first, keyValuePair.second, nullptr);
        return;
    }
    Node<Key, Value>* parent = nullptr;
    Node<Key, Value>* current = root_;
    while(current != nullptr) {
        parent = current;
        if(keyValuePair.first < current->getKey())
            current = current->getLeft();
        else if(current->getKey() < keyValuePair.first)
            current = current->getRight();
        else {
            current->setValue(keyValuePair.second);
            return;
        }
    }
    if(keyValuePair.first < parent->getKey())
        parent->setLeft(new Node<Key, Value>(keyValuePair.first, keyValuePair.second, parent));
    else
        parent->setRight(new Node<Key, Value>(keyValuePair.first, keyValuePair.second, parent));
}

template<typename Key, class Value>
void BinarySearchTree<Key, Value>::remove(const Key& key) {
    Node<Key, Value>* nodeToRemove = internalFind(key);
    if(nodeToRemove == nullptr)
        return;
    
    // If node has two children, swap with its predecessor.
    if(nodeToRemove->getLeft() != nullptr && nodeToRemove->getRight() != nullptr) {
        Node<Key, Value>* pred = predecessor(nodeToRemove);
        nodeSwap(nodeToRemove, pred);
    }
    
    // Now nodeToRemove has at most one child.
    Node<Key, Value>* child = (nodeToRemove->getLeft() != nullptr) ? nodeToRemove->getLeft() : nodeToRemove->getRight();
    Node<Key, Value>* parent = nodeToRemove->getParent();
    
    if(child != nullptr)
        child->setParent(parent);
    
    if(parent == nullptr)
        root_ = child;
    else {
        if(nodeToRemove == parent->getLeft())
            parent->setLeft(child);
        else
            parent->setRight(child);
    }
    delete nodeToRemove;
}

template<typename Key, class Value>
void BinarySearchTree<Key, Value>::clear() {
    clearHelper(root_);
    root_ = nullptr;
}

template<typename Key, class Value>
void BinarySearchTree<Key, Value>::clearHelper(Node<Key, Value>* node) {
    if(node == nullptr)
        return;
    clearHelper(node->getLeft());
    clearHelper(node->getRight());
    delete node;
}

template<typename Key, class Value>
int BinarySearchTree<Key, Value>::isBalancedHelper(Node<Key, Value>* node) const {
    if(node == nullptr)
        return 0;
    int leftHeight = isBalancedHelper(node->getLeft());
    if(leftHeight == -1)
        return -1;
    int rightHeight = isBalancedHelper(node->getRight());
    if(rightHeight == -1)
        return -1;
    if(std::abs(leftHeight - rightHeight) > 1)
        return -1;
    return std::max(leftHeight, rightHeight) + 1;
}

template<typename Key, class Value>
bool BinarySearchTree<Key, Value>::isBalanced() const {
    return isBalancedHelper(root_) != -1;
}

/*
-----------------------------------------------------
Provided Functions: printRoot and nodeSwap
-----------------------------------------------------
*/
template<typename Key, class Value>
void BinarySearchTree<Key, Value>::print() const {
    printRoot(root_);
    std::cout << "\n";
}

template<typename Key, class Value>
void BinarySearchTree<Key, Value>::printRoot(Node<Key, Value>* r) const {
    // Assuming print_bst.h provides this functionality.
}

template<typename Key, class Value>
void BinarySearchTree<Key, Value>::nodeSwap(Node<Key,Value>* n1, Node<Key,Value>* n2) {
    if(n1 == n2 || n1 == nullptr || n2 == nullptr)
        return;
    
    Node<Key, Value>* n1p = n1->getParent();
    bool n1isLeft = (n1p != nullptr && n1 == n1p->getLeft());
    Node<Key, Value>* n2p = n2->getParent();
    bool n2isLeft = (n2p != nullptr && n2 == n2p->getLeft());
    
    Node<Key, Value>* temp = n1->getParent();
    n1->setParent(n2->getParent());
    n2->setParent(temp);
    
    temp = n1->getLeft();
    n1->setLeft(n2->getLeft());
    n2->setLeft(temp);
    
    temp = n1->getRight();
    n1->setRight(n2->getRight());
    n2->setRight(temp);
    
    if(n1p != nullptr && n1p != n2) {
        if(n1isLeft)
            n1p->setLeft(n2);
        else
            n1p->setRight(n2);
    }
    if(n2p != nullptr && n2p != n1) {
        if(n2isLeft)
            n2p->setLeft(n1);
        else
            n2p->setRight(n1);
    }
    
    if(n1->getLeft() != nullptr)
        n1->getLeft()->setParent(n1);
    if(n1->getRight() != nullptr)
        n1->getRight()->setParent(n1);
    if(n2->getLeft() != nullptr)
        n2->getLeft()->setParent(n2);
    if(n2->getRight() != nullptr)
        n2->getRight()->setParent(n2);
    
    if(this->root_ == n1)
        this->root_ = n2;
    else if(this->root_ == n2)
        this->root_ = n1;
}

#endif
