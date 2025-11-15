#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int helper(Node* curNode);

bool equalPaths(Node * root)
{
    // if helper returns -1, then the tree is not balanced otherwise it will return the depth of the tree
    return helper(root) != -1;
}

int helper(Node* curNode)
{
    if (curNode == nullptr) 
    {
        return 0;
    }
    
    // get left and right subtrees
    Node* lSub = curNode->left;
    Node* rSub = curNode->right;
    
    if (lSub == nullptr && rSub == nullptr) 
    {
        return 0;
    }
    
    if (lSub == nullptr) 
    {
        int rDep = helper(rSub);
        if (rDep == -1) 
        {
            return -1;
        }
        return rDep + 1;
    }
    
    if (rSub == nullptr) 
    {
        int lDep = helper(lSub);
        if (lDep == -1) 
        {
            return -1;
        }
        return lDep + 1;
    }
    
    int lDep = helper(lSub);
    int rDep = helper(rSub);
    
    // check if subtrees are balanced and have equal depth
    if (lDep == -1 || rDep == -1 || lDep != rDep) 
    {
        return -1;
    }
    
    return lDep + 1;
}

