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
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    if (this->root_ == NULL) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
        return;
    }
    
    // standard BST insert
    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = NULL;
    bool insertLeft = false;
    
    while (current != NULL) {
        parent = current;
        
        if (new_item.first == current->getKey()) {
            current->setValue(new_item.second);
            return;
        } else if (new_item.first < current->getKey()) {
            insertLeft = true;
            current = current->getLeft();
        } else {
            insertLeft = false;
            current = current->getRight();
        }
    }
    
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    
    if (insertLeft) {
        parent->setLeft(newNode);
    } else {
        parent->setRight(newNode);
    }
    
    // fix balance going up
    AVLNode<Key, Value>* node = parent;
    
    while (node != NULL) {
        if (newNode->getKey() < node->getKey()) {
            node->setBalance(node->getBalance() + 1);
        } else {
            node->setBalance(node->getBalance() - 1);
        }
        
        if (node->getBalance() == 0) {
            break;
        } else if (node->getBalance() == 1 || node->getBalance() == -1) {
            node = node->getParent();
        } else {
            // unbalanced - need rotation
            if (node->getBalance() == 2) {
                // left heavy
                AVLNode<Key, Value>* leftChild = node->getLeft();
                if (leftChild->getBalance() >= 0) {
                    rotateRight(node);  // LL case
                } else {
                    rotateLeft(leftChild);  // LR case
                    rotateRight(node);
                }
            } else {
                // right heavy
                AVLNode<Key, Value>* rightChild = node->getRight();
                if (rightChild->getBalance() <= 0) {
                    rotateLeft(node);  // RR case
                } else {
                    rotateRight(rightChild);  // RL case
                    rotateLeft(node);
                }
            }
            break;
        }
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    AVLNode<Key, Value>* nodeToRemove = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    
    if (nodeToRemove == NULL) {
        return;
    }
    
    // swap with predecessor if has two children
    if (nodeToRemove->getLeft() != NULL && nodeToRemove->getRight() != NULL) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(
            BinarySearchTree<Key, Value>::predecessor(nodeToRemove)
        );
        nodeSwap(nodeToRemove, pred);
    }
    
    AVLNode<Key, Value>* child = NULL;
    if (nodeToRemove->getLeft() != NULL) {
        child = nodeToRemove->getLeft();
    } else if (nodeToRemove->getRight() != NULL) {
        child = nodeToRemove->getRight();
    }
    
    AVLNode<Key, Value>* parent = nodeToRemove->getParent();
    
    // track which side for balance updates
    bool removedFromLeft = false;
    if (parent != NULL) {
        removedFromLeft = (parent->getLeft() == nodeToRemove);
    }
    
    if (parent == NULL) {
        this->root_ = child;
        if (child != NULL) {
            child->setParent(NULL);
        }
    } else {
        if (parent->getLeft() == nodeToRemove) {
            parent->setLeft(child);
        } else {
            parent->setRight(child);
        }
        
        if (child != NULL) {
            child->setParent(parent);
        }
    }
    
    delete nodeToRemove;
    
    // walk up and rebalance if needed
    AVLNode<Key, Value>* node = parent;
    
    while (node != NULL) {
        if (removedFromLeft) {
            node->setBalance(node->getBalance() - 1);
        } else {
            node->setBalance(node->getBalance() + 1);
        }
        
        if (node->getBalance() == 1 || node->getBalance() == -1) {
            break;
        } else if (node->getBalance() == 0) {
            parent = node->getParent();
            if (parent != NULL) {
                removedFromLeft = (parent->getLeft() == node);
            }
            node = parent;
        } else {
            // handle imbalance
            AVLNode<Key, Value>* tallerChild = NULL;
            
            if (node->getBalance() == 2) {
                tallerChild = node->getLeft();
                if (tallerChild->getBalance() >= 0) {
                    rotateRight(node);
                    // might need to stop here
                    if (tallerChild->getBalance() == 0) {
                        break;
                    }
                } else {
                    rotateLeft(tallerChild);
                    rotateRight(node);
                }
            } else {
                tallerChild = node->getRight();
                if (tallerChild->getBalance() <= 0) {
                    rotateLeft(node);
                    if (tallerChild->getBalance() == 0) {
                        break;
                    }
                } else {
                    rotateRight(tallerChild);
                    rotateLeft(node);
                }
            }
            
            // keep going up
            parent = node->getParent();
            if (parent != NULL && parent->getParent() != NULL) {
                removedFromLeft = (parent->getParent()->getLeft() == parent);
            }
            node = parent;
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* z)
{
    AVLNode<Key, Value>* y = z->getLeft();
    AVLNode<Key, Value>* T2 = y->getRight();
    AVLNode<Key, Value>* parent = z->getParent();
    
    // rotate
    y->setRight(z);
    z->setParent(y);
    
    z->setLeft(T2);
    if (T2 != NULL) {
        T2->setParent(z);
    }
    
    // fix parent pointer
    y->setParent(parent);
    if (parent == NULL) {
        this->root_ = y;
    } else if (parent->getLeft() == z) {
        parent->setLeft(y);
    } else {
        parent->setRight(y);
    }
    
    // update balances
    z->setBalance(z->getBalance() - 1 - (y->getBalance() > 0 ? y->getBalance() : 0));
    y->setBalance(y->getBalance() - 1 + (z->getBalance() < 0 ? z->getBalance() : 0));
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* z)
{
    AVLNode<Key, Value>* y = z->getRight();
    AVLNode<Key, Value>* T2 = y->getLeft();
    AVLNode<Key, Value>* parent = z->getParent();
    
    y->setLeft(z);
    z->setParent(y);
    
    z->setRight(T2);
    if (T2 != NULL) {
        T2->setParent(z);
    }
    
    y->setParent(parent);
    if (parent == NULL) {
        this->root_ = y;
    } else if (parent->getLeft() == z) {
        parent->setLeft(y);
    } else {
        parent->setRight(y);
    }
    
    z->setBalance(z->getBalance() + 1 - (y->getBalance() < 0 ? y->getBalance() : 0));
    y->setBalance(y->getBalance() + 1 + (z->getBalance() > 0 ? z->getBalance() : 0));
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
