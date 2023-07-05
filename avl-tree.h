#ifndef AVLTREE_H
#define AVLTREE_H

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>

template<typename Ty>
class AVLNode
{
public:
    AVLNode(Ty&);
    ~AVLNode();
    int balance();

    bool isContains(const Ty&);
    bool isRoot();
    bool isLeaf();

    AVLNode<Ty>* insert(const Ty&);
    AVLNode<Ty>* search(const Ty&);
    AVLNode<Ty>* remove(const Ty&);

    bool makeDOT(const char* filename);

    static void _makeEmptyRec(AVLNode<Ty>* node);

private:
    AVLNode(AVLNode<Ty>*, Ty);

    static int _height(AVLNode<Ty>*);

    std::string _nodeDOT();

    bool _isRightChild();
    bool _isLeftChild();

    AVLNode<Ty>* _simpleInsert(const Ty&);
    AVLNode<Ty>* _simpleRemove(const Ty&);
    AVLNode<Ty>* _minChild();
    AVLNode<Ty>* _rotateToRight();
    AVLNode<Ty>* _rotateToLeft();
    AVLNode<Ty>* _rebalance();

    AVLNode<Ty>* _parent;
    AVLNode<Ty>* _left;
    AVLNode<Ty>* _right;

    Ty _data;
};

template<typename Ty>
class AVLTree
{
public:
    AVLTree();
    AVLTree(std::vector<Ty>);
    ~AVLTree();

    bool isContains(const Ty&);

    void insert(const Ty&);
    void remove(const Ty&);

    bool makeDOT(const char*);

private:
    AVLNode<Ty>* _root;
};

template<typename Ty>
AVLNode<Ty>::AVLNode(Ty& rootValue)
    : _parent(nullptr)
    , _right(nullptr)
    , _left(nullptr)
    , _data(rootValue)
{}

template<typename Ty>
inline AVLNode<Ty>::AVLNode(AVLNode<Ty>* parent, Ty value)
    : _parent(parent)
    , _right(nullptr)
    , _left(nullptr)
    , _data(value)
{}

template<typename Ty>
inline AVLNode<Ty>::~AVLNode()
{
    _data.~Ty();
}

template<typename Ty>
inline int AVLNode<Ty>::balance()
{
    return _height(_right) - _height(_left);
}

template<typename Ty>
inline bool AVLNode<Ty>::_isRightChild()
{
    return !isRoot() && _parent->_right == this;
}

template<typename Ty>
inline bool AVLNode<Ty>::_isLeftChild()
{
    return !isRoot() && _parent->_left == this;
}

template<typename Ty>
inline int AVLNode<Ty>::_height(AVLNode<Ty>* tree)
{
    if (tree == nullptr) {
        return 0;
    }
    return 1 + std::max(_height(tree->_right), _height(tree->_left));
}

template<typename Ty>
inline std::string AVLNode<Ty>::_nodeDOT()
{
    std::string DOT;
    DOT = std::to_string(_data) + "\n";

    if (_left) {
        DOT += std::to_string(_data) + "->";
        DOT += std::to_string(_left->_data) + "[color=blue] [label=L]" + "\n";
        DOT += _left->_nodeDOT();
    }

    if (_right) {
        DOT += std::to_string(_data) + "->";
        DOT += std::to_string(_right->_data) + "[color=red] [label=R]" + "\n";
        DOT += _right->_nodeDOT();
    }

    return DOT;
}

template<typename Ty>
inline AVLNode<Ty>* AVLNode<Ty>::_simpleInsert(const Ty &value)
{
    if (value < _data) {
        return (
            _left == nullptr
                ? _left = new AVLNode(this, value)
                : _left->_simpleInsert(value)
        );
    }
    else if (value > _data) {
        return (
            _right == nullptr
                ? _right = new AVLNode(this, value)
                : _right->_simpleInsert(value)
        );
    }
    return this;
}

template<typename Ty>
inline AVLNode<Ty>* AVLNode<Ty>::_simpleRemove(const Ty& value)
{
    AVLNode<Ty>* toRemove = search(value);

    if (toRemove == nullptr) {
        return this;
    }

    AVLNode<Ty>* replacement;

    if (toRemove->isLeaf()) {
        replacement = nullptr;
    }
    else if (toRemove->_right == nullptr) {
        replacement = toRemove->_left;
    }
    else if (toRemove->_left == nullptr) {
        replacement = toRemove->_right;
    }
    else {
        AVLNode<Ty>* minChild = toRemove->_right->_minChild();
        toRemove->_data = minChild->_data;
        toRemove = minChild;
        replacement = toRemove->_right;
    }

    if (toRemove->_isLeftChild()) {
        toRemove->_parent->_left = replacement;
    }
    else if (toRemove->_isRightChild()) {
        toRemove->_parent->_right = replacement;
    }
    if (replacement) {
        replacement->_parent = toRemove->_parent;
    }
    replacement = toRemove->_parent;
    delete toRemove;
    return replacement;
}

template<typename Ty>
inline AVLNode<Ty>* AVLNode<Ty>::_minChild()
{
    return (
        _left == nullptr
            ? this
            : _left->_minChild()
    );
}

template<typename Ty>
inline AVLNode<Ty>* AVLNode<Ty>::_rotateToRight()
{
    _left->_parent = _parent;

    if (_isRightChild()) {
        _parent->_right = _left;
    }
    else if (_isLeftChild()) {
        _parent->_left = _left;
    }

    _parent = _left;
    _left = _left->_right;
    _parent->_right = this;

    if (_left != nullptr) {
        _left->_parent = this;
    }
    return _parent; // replacement for root
 }

template<typename Ty>
inline AVLNode<Ty>* AVLNode<Ty>::_rotateToLeft()
{
    _right->_parent = _parent;

    if (_isRightChild()) {
        _parent->_right = _right;
    }
    else if (_isLeftChild()) {
        _parent->_left = _right;
    }
    _parent = _right;
    _right = _right->_left;
    _parent->_left = this;

    if (_right != nullptr) {
        _right->_parent = this;
    }
    return _parent; // replacement for root
}

template<typename Ty>
inline AVLNode<Ty>* AVLNode<Ty>::_rebalance()
{
    int disbalance = balance();

    if (disbalance == 2) {
        if (_height(_right->_left) <= _height(_right->_right)) {
            return _rotateToLeft();
        }
        else {
            return _right->_rotateToRight()->_parent->_rotateToLeft();
        }
    }
    else if (disbalance == -2) {
        if (_height(_left->_right) <= _height(_left->_left)) {
            return _rotateToRight();
        }
        else {
            return _left->_rotateToLeft()->_parent->_rotateToRight();
        }
    }
    return this;
}

template<typename Ty>
inline bool AVLNode<Ty>::isContains(const Ty& value)
{
    if (value < _data) {
        return (
            _left ? _left->isContains(value)
                  : false
        );
    }
    else if (value > _data) {
        return (
            _right ? _right->isContains(value)
                   : false
        );
    }
    return true;
}

template<typename Ty>
AVLNode<Ty>* AVLNode<Ty>::insert(const Ty& value)
{
    AVLNode<Ty>* disbalanceNode = _simpleInsert(value);

    int disbalance = disbalanceNode->balance();
    while (std::abs(disbalance) <= 1) {
        if (disbalanceNode->isRoot()) {
            return disbalanceNode;
        }
        disbalanceNode = disbalanceNode->_parent;
        disbalance = disbalanceNode->balance();
    }
    disbalanceNode = disbalanceNode->_rebalance();
    return (
        disbalanceNode->isRoot() ? disbalanceNode
                                 : this
    );
}

template<typename Ty>
inline AVLNode<Ty>* AVLNode<Ty>::search(const Ty& value)
{
    if (value < _data) {
        return (
            _left ? _left->search(value)
                  : nullptr
        );
    }
    else if (value > _data) {
        return (
            _right ? _right->search(value)
                   : nullptr
        );
    }
    return this;
}

template<typename Ty>
inline bool AVLNode<Ty>::isRoot() { return _parent == nullptr; }

template<typename Ty>
inline bool AVLNode<Ty>::isLeaf()
{
    return _left == nullptr && _right == nullptr;
}

template<typename Ty>
inline AVLNode<Ty>* AVLNode<Ty>::remove(const Ty& value)
{
    AVLNode<Ty>* walkingNode = _simpleRemove(value);

    int disbalance = walkingNode->balance();

    while (walkingNode->isRoot() == false) {
        if (std::abs(disbalance) > 1) {
            walkingNode = walkingNode->_rebalance();
        }
        walkingNode = walkingNode->_parent;
        disbalance = walkingNode->balance();
    }
    return walkingNode;
}

template<typename Ty>
inline bool AVLNode<Ty>::makeDOT(const char* filename)
{
    std::ofstream outFile(filename);
    if (!outFile) {
        return false;
    }
    outFile << "digraph G {\n" << _nodeDOT() << "}\n";
    return true;
}

template<typename Ty>
inline AVLTree<Ty>::AVLTree()
    : _root(nullptr)
{}

template<typename Ty>
inline AVLTree<Ty>::AVLTree(std::vector<Ty> values)
{
    _root = new AVLNode<Ty>(values[0]);
    for (auto v : values) {
        _root = _root->insert(v);
    }
}

template<typename Ty>
inline AVLTree<Ty>::~AVLTree()
{
    AVLNode<Ty>::_makeEmptyRec(_root);
}

template<typename Ty>
inline bool AVLTree<Ty>::isContains(const Ty& value)
{
    return _root->isContains(value);
}

template<typename Ty>
inline void AVLTree<Ty>::insert(const Ty& value)
{
    _root = _root->insert(value);
}

template<typename Ty>
inline void AVLTree<Ty>::remove(const Ty& value)
{
    _root = _root->remove(value);
}

template<typename Ty>
inline bool AVLTree<Ty>::makeDOT(const char* filename)
{
    return _root->makeDOT(filename);
}

template<class Ty>
inline void AVLNode<Ty>::_makeEmptyRec(AVLNode<Ty>* node)
{
    if (node) {
        _makeEmptyRec(node->_left);
        _makeEmptyRec(node->_right);
        delete node;
    }
}

#endif
