#ifndef BINTREE_H
#define BINTREE_H

#include "nodedata.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

//Karim Oumghar
//CSS343 - Lab 2 - Oct 24, 2015

//BinTree declarations
//Any public methods this class will have are non recursive in that...
//they either return void, an integer, or bool
//any recursive helper methods will be private
//for example: inorder printing, equality checking, deep copying...
//this BST will keep count of the number of nodes within
//as additional criteria for equality/nonequality checking



//Node object
//Node is a struct, much simpler to keep it a struct and to...
//have its implementation inside the .h

class BinTree
{
    public:
        BinTree();
        BinTree(const BinTree &ref);
        ~BinTree();
        bool isEmpty() const;
        void makeEmpty();
        bool retrieve(const NodeData &ref1, NodeData * &target) const;
        void displaySideways() const;
        bool insert(NodeData *object);
        int getHeight(const NodeData &ref) const;
        void bstreeToArray(NodeData * arr[]);
        void arrayToBSTree(NodeData * arr[]);

        //operator overloads, assignment, equals/not equals, and output
        BinTree& operator=(const BinTree &ref);
        bool operator==(const BinTree &ref) const;
        bool operator!=(const BinTree &ref) const;
        friend ostream& operator<<(ostream &ost, const BinTree &ref);

    private:
        struct Node
        {
            NodeData *data;
            Node *left;
            Node *right;
        };

        Node *root;//root pointer

        int itemCount;//keep track of the number of items in the tree

        int index;//to be used for copying things to the array

        //helper printing methods: inorder, sideways
        void inorderHelper(Node *current) const;
        void sideways(Node *current, int level) const;

        //a helper to delete all contents of the tree
        void makeEmptyHelper(Node *current);

        //helpers: getHeight - finds a target node
        //subtreeHeight - obtains the height from the target node
        int getHeightHelper(Node *current,
                            const NodeData &ref, int level) const;
        int subtreeHeight(Node *current) const;

        //helper used for detecting equality for two trees
        bool equalsOpHelper(Node *current, Node * otherTreeNode,
                            const BinTree &ref) const;

        //helper used for assignment operator functionality
        void copyHelper(Node *ref);

        //helper functions for the array operations
        void bstreeToArrayHelper(Node *current, NodeData * arr[]);
        void arrayToBSTreeHelper(NodeData * arr[],
                                 int low, int high);

};

#endif // BINTREE_H
