#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <cmath>
#include <iostream>

using namespace std;

enum labelType{
    identifier,
    constant,
    op,
};

struct TreeNode {
    labelType label;
    int tokenCode;
    TreeNode* left;
    TreeNode* right;

    void printNode(){
        
        printf("TreeNode: \n");
        printf(" -------- \n");
        printf("|   %d   |\n", label);
        printf(" -------- \n");
        printf("|   %d   |\n", tokenCode);
        printf(" -------- \n");
    }
};

/* Tree functions for creating syntax tree */
TreeNode* makeNode(labelType l, int tknCode, TreeNode* leftChild, TreeNode* rightChild){
    TreeNode* newNode = new TreeNode();
    newNode->label = l;
    newNode->tokenCode = tknCode;
    newNode->left = leftChild;
    newNode->right = rightChild;
    
    return newNode;
}

int main(){
    TreeNode* treePtr =  makeNode(identifier, 12, nullptr, nullptr);
    treePtr->printNode();
}