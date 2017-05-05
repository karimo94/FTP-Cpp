#include "bintree.h"

//Karim Oumghar
//CSS343 - Lab 2

//-----------------------default constructor-----------------------------
BinTree::BinTree()
{
    //ctor
    root = NULL;
    itemCount = 0;
}
//-------------------------copy constructor---------------------------
//some cases to consider
//if the current tree is the same as the referece, return
//if the reference tree is empty, make the current one empty
//else, make a new node, set its data to the reference root data,
//and call a copy Helper method to finish creating the rest of the tree

BinTree::BinTree(const BinTree &ref)
{
    if(*this == ref)
    {
        return;
    }
    if(ref.isEmpty())
    {
        makeEmpty();
        //root = NULL;
        return;
    }
    else
    {
        Node *otherRoot = ref.root;//we assume the current root is null
        Node *nObj = new Node();
        nObj->data = otherRoot->data;
        nObj->left = NULL;
        nObj->right = NULL;
        root = nObj;//root is now pointing to something
        copyHelper(otherRoot);//copy all the contents from the other tree
        itemCount = ref.itemCount;
        return;
    }
}
//------------------------destructor----------------------------
BinTree::~BinTree()
{
    makeEmpty();
    //delete root;
    //root = NULL;
}
//--------------------------isEmpty--------------------------
bool BinTree::isEmpty() const
{
	//if the root doesnt point to anything its empty

    return (root == NULL);
}
//-------------------------makeEmpty---------------------------
void BinTree::makeEmpty()
{
    //if the tree is empty, call the delete helper, set root to null...
    //otherwise, just return
    if(!isEmpty())
    {
        makeEmptyHelper(root);
        //delete root;
        root = NULL;
        itemCount = 0;
    }
    else
    {
        return;
    }
}
//----------------------MakeEmptyHelper------------------------------
void BinTree::makeEmptyHelper(Node *current)
{
    if(current != NULL)
    {
        makeEmptyHelper(current->left);//recurse on both sides
        makeEmptyHelper(current->right);
        delete current;
    }
    if(current == NULL)
    {
        return;
    }
}
//-----------------------displaySideways-----------------------------
void BinTree::displaySideways() const
{
    sideways(root, 0);
}
//-------------------------sideways method---------------------------
// Helper method for displaySideways
// Preconditions: NONE
// Postconditions: BinTree remains unchanged.
void BinTree::sideways(Node *current, int level) const
{
    if (current != NULL)
    {
      level++;
      sideways(current->right, level);

      // indent for readability, 4 spaces per depth level
      for(int i = level; i >= 0; i--)
      {
          cout << "    ";
      }

      cout << *current->data << endl;        // display information of object
      sideways(current->left, level);
   }
}
//------------------------insert method----------------------------
bool BinTree::insert(NodeData *object)
{
    //for this method, a simple iterative method will work
    Node *nObj = new Node();
    nObj->data = object;
    bool isSuccess = false;
    if(root == NULL)
    {
        root = nObj;
        isSuccess = true;
    }
    else
    {
        Node *current = root;
        Node *parent = NULL;
        //keep track of current
        //when it reaches null, parent
        //points to the previous node
        //and attach the new child to the parent
        while(current != NULL)
        {
            parent = current;
            if(*nObj->data == *current->data)
            {
                break;
            }
            if(*nObj->data < *current->data)
            {
                current = current->left;
                if(current == NULL)
                {
                    parent->left = nObj;
                    isSuccess = true;
                    itemCount++;
                }
            }
            else
            {
                current = current->right;
                if(current == NULL)
                {
                    parent->right = nObj;
                    isSuccess = true;
                    itemCount++;
                }
            }
        }
    }
    return isSuccess;
}
//--------------------------retrieve method--------------------------
bool BinTree::retrieve(const NodeData &ref1, NodeData* &target) const
{
    Node *current = root;
    bool isFound = false;
    //while it hasnt been found and nothing is null
    while(current != NULL && isFound == false)
    {
        if(ref1 == *current->data)
        {
            target = current->data;
            isFound = true;
            break;
        }
        if(ref1 < *current->data)
        {
            current = current->left;
        }
        else
        {
            current = current->right;
        }
    }
    return isFound;
}
//---------------------getHeight method-------------------------------
int BinTree::getHeight(const NodeData &ref) const
{
    return getHeightHelper(root, ref, 1);
}
//--------------------------getHeightHelper method--------------------------
int BinTree::getHeightHelper(Node *current, const NodeData &ref, int level) const
{
    //find the node first, then find the subtree height
    if(current == NULL)
    {
        return 0;
    }
    if(*current->data == ref)
    {
        int height = subtreeHeight(current);
        if(height == 0)
        {
            return level;
        }
        else
        {
            return height;
        }
    }
    //return the maximum height so add left and right
    return getHeightHelper(current->left, ref, level) +
    getHeightHelper(current->right, ref, level);
}
//------------------------bstreeToArray method----------------------------
//calls on the bstreeToArray helper to port nodes an array
//after all contents have been ported, clear the tree
void BinTree::bstreeToArray(NodeData * arr[])
{
    if(root == NULL)
    {
        return;
    }
    index = 0;
    bstreeToArrayHelper(root, arr);
    //empty the tree, if needed
    //makeEmpty();
    return;
}
//-------------------------bstreeToArrayHelper---------------------------
//do an in order recursive traversal, increment the index for each new
//insertion into the array
void BinTree::bstreeToArrayHelper(Node *current, NodeData * arr[])
{
    if(current != NULL)
    {
        //recurse on both sides (in order traversal)
        //and increment index
        bstreeToArrayHelper(current->left, arr);
        arr[index++] = current->data;
        bstreeToArrayHelper(current->right, arr);
    }
    if(current == NULL)
    {
        return;
    }
}
//------------------------arrayToBSTree method----------------------------
//returns void, checks first if there exists some tree
//if there is, return
//if theres no tree, the call on the arrayToBSTreeHelper to make a tree
//delete the array, set array pointer to null
void BinTree::arrayToBSTree(NodeData * arr[])
{
    if(root != NULL)
    {
        return;
    }
    //no null arrays
    if(arr == NULL)
    {
        return;
    }
    //get low and high indexes (we dont want null indices)
    int low = 0;
    int high = 0;
    while(arr[high] != NULL)
    {
        high++;
    }
    arrayToBSTreeHelper(arr, low, high); //because ARRAYSIZE = 100

    //delete [] arr;
    //arr = NULL;
}
//------------------------arrayToBSTreeHelper method----------------------------
//start from the middle
//insert a node into the middle index
//recurse on both sides, decrement mid for the left side
//increment mid on for the right side
void BinTree::arrayToBSTreeHelper(NodeData * arr[], int low, int high)
{
    if(low > high)//base case
    {
        return;
    }
    int mid = (low + high) / 2;

    Node *current = NULL;
    if(current == NULL && arr[mid] != NULL)
    {
        //create a new node and insert
        current = new Node();
        current->data = arr[mid];
        current->left = NULL;
        current->right = NULL;
        insert(current->data);
        //recurse on both sides
        arrayToBSTreeHelper(arr, low, mid - 1),
        arrayToBSTreeHelper(arr, mid + 1, high);
    }

}
//----------------------ouput operator overload------------------------------
//if theres something to print, call the inorderHelper
//if theres nothing, either print a space or some kind of message
//return the ostream reference
ostream& operator<<(ostream &ost, const BinTree &ref)
{
    if(ref.root == NULL) //if the tree is empty
    {
        ost <<"Empty Tree, nothing to print!";
    }
    else
    {
        ref.inorderHelper(ref.root);
    }
    ost<<endl;
    return ost;
}
//---------------------inOrderHelper method-------------------------------
//recursively traverse the tree in order
//if the pointer passed in is null, then return
void BinTree::inorderHelper(Node *current) const
{
    if(current != NULL)
    {
        //recurse on the left, print, the recurse on the right, in order traversal
        inorderHelper(current->left);
        cout<<*current->data<<" ";
        inorderHelper(current->right);

    }
    if(current == NULL)
    {
        return;
    }
}
//------------------------equals operator----------------------------------
//returns a boolean,
//calls on the equals helper method to help determine equality
bool BinTree::operator==(const BinTree &ref) const
{
    return (equalsOpHelper(root, ref.root, ref));
}
//---------------------------not equals operator---------------------------
//returns a boolean (note the ! outside the function call)
//calls on the equals helper method to help determine inequality
bool BinTree::operator!=(const BinTree &ref) const
{
    return !(equalsOpHelper(root, ref.root, ref));
}
//------------------------equalsOpHelper method----------------------------
//determines equality recursively
//basically, traverse the tree, first left, then right
//recurse both sides: return equalsHelper(left), equalsHelper(right)
//if the current node is exactly the same as the other node, return true
//else, return false (such as different itemCount, if current/other is null)
bool BinTree::equalsOpHelper(Node *current, Node *otherTreeNode,
                             const BinTree &ref) const
{
    if(current == otherTreeNode)
    {
        return true;
    }
    if(current == NULL || otherTreeNode == NULL || itemCount != ref.itemCount)
    {
        return false;
    }
    return equalsOpHelper(current->left, otherTreeNode->left, ref),
    equalsOpHelper(current->right, otherTreeNode->right, ref);
}
//-------------------------assignment operator---------------------------
//once again, we first test if both trees are the same, then return *this
//else, we clear the current tree, create the new root
//and finish by calling the copyHelper
BinTree& BinTree::operator=(const BinTree &ref)
{
    if(*this == ref)
    {
        return *this;
    }
    else
    {
        makeEmpty(); //clear the tree, then create a new node the root will point to

        Node *nObj = new Node();
        Node *otherRoot = ref.root;
        nObj->data = otherRoot->data;
        nObj->left = NULL;
        nObj->right = NULL;
        root = nObj;

        copyHelper(otherRoot);//finish copying the rest of the tree
        itemCount = ref.itemCount;
        return *this;
    }
}
//-------------------------copyHelper method---------------------------
//first, we start by declaring a Node pointer that will be used to
//create a new node, set it to null initially
//then we check if the reference node is null
//if its not, create a new node from the reference data
//and call insert
//then, recurse on both sides for the reference tree
void BinTree::copyHelper(Node * ref)
{
    //if we reach a null pointer, create a new node and insert
    Node *nObj = NULL;
    if(ref != NULL)
    {
        nObj = new Node();
        nObj->data = ref->data;
        nObj->left = NULL;
        nObj->right = NULL;
        insert(nObj->data);
        copyHelper(ref->left), copyHelper(ref->right);
    }
}
//------------------------subtreeHeight method--------------------------
int BinTree::subtreeHeight(Node *current) const
{
    //basically, find the max height of the subtree
    //since some target node was found, the last thing to do is
    //find the max height of the subtree where the node resides
    if(current == NULL)
    {
        return 0;
    }
    return 1 + max(subtreeHeight(current->left), subtreeHeight(current->right));
}
