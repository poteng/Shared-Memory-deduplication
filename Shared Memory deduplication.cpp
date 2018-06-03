
#include <iostream>
#include <sstream>
#include <cmath>
#include <time.h>
#include <stdlib.h>

using namespace std;



struct BRnode
{
    char color;
    int id;
    int hash;
    BRnode* parent;
    BRnode* right;
    BRnode* left;

};

struct nodePage
{
    int id;         //page id
    int hash;    //hash of page content
};

class BRtree
{
    public:
        BRnode* root;
        BRnode* leavesBlack;

        BRtree()
        {
            root = NULL;
            leavesBlack = NULL;
            BRnode* leaves = new BRnode;
            leavesBlack = leaves;
            leavesBlack->id = -99;
            leavesBlack->hash = -1;
            leavesBlack->color = 'b';
        }
        //~BRtree();
        BRnode* find(int);
        BRnode* find_without_key(int);
        BRnode* recursive_find(BRnode*, int);
        void insert_br(int, int);
        void insert_fixup_br(BRnode*);

        void right_rotate_br(BRnode*);
        void left_rotate_br(BRnode*);

        void TRANSPLANT_RB(BRnode*, BRnode*);
        void DELETE_RB(BRnode*);
        void DELETE_FIXUP_RB(BRnode*);
        BRnode* MINIMUM_TREE(BRnode*);

        void cleanTree();

};

void printLevelOrder(BRnode*, int);

void Load(nodePage*, int, BRtree);
void Update(nodePage*, int, BRtree);
bool Deduplicate(nodePage*, int, BRtree*, BRtree*);

//load data to unstable tree
//(need to get the reference of object)
void Load(nodePage* listData, int size, BRtree* obj)
{
    for (int i = 0; i < size; i++)
    {
        obj->insert_br(listData[i].id, listData[i].hash);
    }

    cout << "Data was successfully loaded.\n\n";
    return;
}



void Update(nodePage* listData, int sizeList, BRtree* treeStable, BRtree* treeUnstable)
{
    BRnode* nodeInSta;
    BRnode* nodeInUnsta;

    for (int i = 0; i < sizeList; i++)
    {
        nodeInSta = treeStable->find_without_key(listData[i].id);
        nodeInUnsta = treeUnstable->find_without_key(listData[i].id);
        //page is in unstable tree
        if ( Deduplicate(listData, i, treeStable, treeUnstable) )
        {
            //nothing
        }
        else if (nodeInSta == NULL && nodeInUnsta == NULL)   //not in any
        {
            //add to unstable tree if it wasn't in either of two trees
            cout << "A page with id " << listData[i].id
                    << " has moved to unstable tree.\n";
            treeUnstable->insert_br(listData[i].id, listData[i].hash);
        }
        else if ( nodeInSta != NULL )               //page is in stable tree
        {
            //if it's in stable tree and its hash has changed, move it to unstable tree
            if (listData[i].hash != nodeInSta->hash)
            {
                cout << "A page with id " << listData[i].id
                    << " has changed. Move it to unstable tree from stable tree.\n";
                treeStable->DELETE_RB(nodeInSta);
                treeUnstable->insert_br(listData[i].id, listData[i].hash);
            }
            else
            {
                //do nothing
            }
        }
    }

    cout << "\nStable tree:\n";
    printLevelOrder(treeStable->root, 6);
    cout << "\nUnstable tree:\n";
    printLevelOrder(treeUnstable->root, 6);

    //delete unstable tree after each scan
    treeUnstable->cleanTree();

    return;
}


//find the node in unstable tree by hash
bool Deduplicate(nodePage* listData, int index, BRtree* treeStable, BRtree* treeUnstable)
{
    BRnode* nodeInUnsta;
    nodeInUnsta = treeUnstable->find(listData[index].hash);

    if (nodeInUnsta == NULL)    //no pages in unstable tree has same hash value
    {
        return false;
    }
    if (listData[index].hash == nodeInUnsta->hash && (listData[index].id != nodeInUnsta->id))
    {
        //move those two pages to stable tree
        cout << "Page with page id " << listData[index].id << " has been moved to stable tree.\n";
        treeStable->insert_br(listData[index].id, listData[index].hash);
        treeStable->insert_br(nodeInUnsta->id, nodeInUnsta->hash);
        treeUnstable->DELETE_RB(nodeInUnsta);
        return true;
    }
    else
    {
        cout << "There is no same hash in unsta\n\n";
        return false;
    }
}




void BRtree::cleanTree()
{
    root = NULL;
    return;
}

//return the node with wanted hash
BRnode* BRtree::find(int target)
{
    if (root == NULL)
    {
        return NULL;
    }
    else
    {
        //traverse to the appropriate position
        BRnode* nodeTraverse;
        nodeTraverse = root;

        while (nodeTraverse != leavesBlack)
        {
            if (nodeTraverse == NULL)
            {
                cout << "Wrongggggggggggg\n";
            }

            if (nodeTraverse == leavesBlack)        //not found
            {
                return NULL;
            }
            else if (target == nodeTraverse->hash) //found
            {
                return nodeTraverse;
            }
            else if (target > nodeTraverse->hash)
            {
                nodeTraverse = nodeTraverse->right;
            }
            else if (target < nodeTraverse->hash)
            {
                nodeTraverse = nodeTraverse->left;
            }
            else
            {
                cout << "Wrong in find.\n";
            }
        }
    }

    return NULL;
}


//return the node with wanted id
//go through all nodes in tree to find the target node
BRnode* BRtree::find_without_key(int target)
{
    return recursive_find(root, target);
}

BRnode* BRtree::recursive_find(BRnode* nodeCurrent, int target)
{
    BRnode* left = NULL;
    BRnode* right = NULL;
    if (nodeCurrent == NULL)
    {
        return NULL;
    }
    if (nodeCurrent->id == target)
    {
        return nodeCurrent;
    }
    //recursively go through all subtrees to find the target
    if (nodeCurrent->right != leavesBlack)
    {
        right = recursive_find(nodeCurrent->right, target);
        if (right != NULL) { return right;}
    }
    else if (nodeCurrent->left != leavesBlack)
    {
        left = recursive_find(nodeCurrent->left, target);
        if (right != left) { return left;}
    }
    return NULL;
}


void BRtree::insert_fixup_br(BRnode* newNode)
{
    BRnode* temp;
    while (newNode->parent->color == 'r')
    {
        //left side double red case
        if (newNode->parent == newNode->parent->parent->left)
        {
            temp = newNode->parent->parent->right;  //y node
            if (temp->color == 'r') //recolor
            {
                newNode->parent->color = 'b';
                temp->color = 'b';
                newNode->parent->parent->color = 'r';
                newNode = newNode->parent->parent;  //start with the grand p node
            }
            else    //rotate right
            {
                if (newNode == newNode->parent->right)   //case 2
                {
                    newNode = newNode->parent;
                    left_rotate_br(newNode);
                }
                //case3
                newNode->parent->color = 'b';
                newNode->parent->parent->color = 'r';
                right_rotate_br(newNode->parent->parent);
            }
        }
        //right side double red case
        else if (newNode->parent == newNode->parent->parent->right)
        {
            temp = newNode->parent->parent->left;  //y node
            if (temp->color == 'r') //recolor
            {
                newNode->parent->color = 'b';
                temp->color = 'b';
                newNode->parent->parent->color = 'r';
                newNode = newNode->parent->parent;  //start with the grand p node
            }
            else    //rotate left
            {
                if (newNode == newNode->parent->left)   //case 2
                {
                    newNode = newNode->parent;
                    right_rotate_br(newNode);
                }
                //case3
                newNode->parent->color = 'b';
                newNode->parent->parent->color = 'r';
                left_rotate_br(newNode->parent->parent);
            }
        }
        else
        {
            cout<<"wrong???????????\n";
        }

        if (newNode->parent == NULL)
        {
            cout << "XXXXXX serious error at end while loop in insert fix up\n";
        }
    }
    root->color = 'b';
    return;
}


void BRtree::insert_br(int idInsert, int hashInsert)
{
    BRnode* nodeTraverse;
    BRnode* nodeParent;
    if (root == NULL)
    {
        BRnode* nodeNew = new BRnode;
        nodeNew->id = idInsert;
        nodeNew->hash = hashInsert;
        nodeNew->parent = leavesBlack;
        nodeNew->right = leavesBlack;
        nodeNew->left = leavesBlack;
        root = nodeNew;
            //cout <<"root updated.\n";
        return;
    }

    //traverse to the appropriate position
    nodeTraverse = root;
    while (nodeTraverse != leavesBlack)
    {
        nodeParent = nodeTraverse;
        if (nodeTraverse == NULL)
        {
            cout << "Wrongggggggggggg\n";
        }
        if (hashInsert > nodeTraverse->hash)
        {
            nodeTraverse = nodeTraverse->right;
        }
        else
        {
            nodeTraverse = nodeTraverse->left;
        }
    }
    BRnode* nodeNew = new BRnode;

    if (hashInsert > nodeParent->hash)
    {
        nodeParent->right = nodeNew;
    }
    else
    {
        nodeParent->left = nodeNew;
    }

    nodeNew->id = idInsert;
    nodeNew->hash = hashInsert;
    nodeNew->color = 'r';
    nodeNew->parent = nodeParent;
    nodeNew->right = leavesBlack;
    nodeNew->left = leavesBlack;

    insert_fixup_br(nodeNew);
    return;
}


void BRtree::right_rotate_br(BRnode* nodeCurrent)
{
    BRnode* temp;
    temp = nodeCurrent->left;
    nodeCurrent->left = temp->right;    //get temp's subtree
    if (temp->right != leavesBlack)
    {
        temp->right->parent = nodeCurrent;   //give subtree to nodeCurrent
    }
    else if (temp->right == NULL)
    {
        cout <<"??\n";
    }

    temp->parent = nodeCurrent->parent; //get nodeCurrent's parent

    if (nodeCurrent->parent == leavesBlack)
    {
        root = temp;                    //get root if nodeCurrent was root
    }
    else if (nodeCurrent == nodeCurrent->parent->right)
    {
        nodeCurrent->parent->right = temp;   //set child relation
    }
    else if (nodeCurrent == nodeCurrent->parent->left)
    {
        nodeCurrent->parent->left = temp;   //set child relation
    }
    else
    {
        cout <<"Wrong hereeeeeeeee.\n";
    }

    temp->right = nodeCurrent;               //set relations
    nodeCurrent->parent = temp;
    return;
}

void BRtree::left_rotate_br(BRnode* nodeCurrent)
{
    BRnode* temp = nodeCurrent->right;
    nodeCurrent->right = temp->left;    //get temp's subtree
    if (temp->left != leavesBlack)
    {
        temp->left->parent = nodeCurrent;   //give subtree to nodeCurrent
    }
    else if (temp->left == NULL)
    {
        cout <<"??\n";
    }

    temp->parent = nodeCurrent->parent; //get nodeCurrent's parent

    if (nodeCurrent->parent == NULL){cout << "ssss\n";}

    if (nodeCurrent->parent == leavesBlack)
    {
        root = temp;                    //get root if nodeCurrent was root
    }
    else if (nodeCurrent == nodeCurrent->parent->left)
    {
        nodeCurrent->parent->left = temp;   //set child relation
    }
    else if (nodeCurrent == nodeCurrent->parent->right)
    {
        nodeCurrent->parent->right = temp;   //set child relation
    }
    else
    {
        cout <<"Wrong hereeeeeeeee.\n";
    }

    temp->left = nodeCurrent;               //set relations
    nodeCurrent->parent = temp;
    return;
}


void BRtree::TRANSPLANT_RB(BRnode* u, BRnode* v)
{
    if (u->parent == leavesBlack)
    {
        root = v;
    }
    else if (u == u->parent->left)
    {
        u->parent->left = v;
    }
    else
    {
        u->parent->right = v;
    }
    v->parent = u->parent;
    return;
}

void BRtree::DELETE_RB(BRnode* z)
{
    char colorOriginalY;
    BRnode* x;
    BRnode* y;
    y = z;
    colorOriginalY = y->color;
    if (z == NULL)
    {
        cout << "Nothing was deleted.\n\n";
    }

    if (z->left == leavesBlack)
    {
        x = z->right;
        TRANSPLANT_RB(z, z->right);
    }
    else if (z->right == leavesBlack)
    {
        x = z->left;
        TRANSPLANT_RB(z, z->left);
    }
    else
    {
        y = MINIMUM_TREE(z->right);
        colorOriginalY = y->color;
        x = y->right;
        if (y->parent == z)
        {
            x->parent = y;
        }
        else
        {
            TRANSPLANT_RB(y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        TRANSPLANT_RB(z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    if (colorOriginalY == 'b')
    {
        DELETE_FIXUP_RB(x);
    }
    return;

}
void BRtree::DELETE_FIXUP_RB(BRnode* x)
{
    BRnode* w;
    while (x != root && x->color == 'b')
    {
        if (x == x->parent->left)
        {
            w = x->parent->right;
            if (w->color == 'r')
            {
                w->color = 'b';
                x->parent->color = 'r';
                left_rotate_br(x->parent);
                w = x->parent->right;
            }
            if (w->left->color == 'b' && w->right->color == 'b')
            {
                w->color = 'r';
                x = x->parent;
            }
            else
            {
                if (w->right->color == 'b')
                {
                    w->left->color = 'b';
                    w->color = 'r';
                    right_rotate_br(w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = 'b';
                w->right->color = 'b';
                left_rotate_br(x->parent);
                x = root;
            }
        }
        else
        {

            w = x->parent->left;
            if (w->color == 'r')
            {
                w->color = 'b';
                x->parent->color = 'r';
                right_rotate_br(x->parent);
                w = x->parent->left;
            }
            if (w->right->color == 'b' && w->left->color == 'b')
            {
                w->color = 'r';
                x = x->parent;
            }
            else
            {
                if (w->left->color == 'b')
                {
                    w->right->color = 'b';
                    w->color = 'r';
                    left_rotate_br(w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = 'b';
                w->left->color = 'b';
                right_rotate_br(x->parent);
                x = root;
            }
        }
    }
    x->color = 'b';
    return;
}



BRnode* BRtree::MINIMUM_TREE(BRnode* x)
{
    while (x->left != leavesBlack)
    {
        x = x ->left;
    }
    return x;
}










//Tree display code found here:
//http://stackoverflow.com/questions/15853438/how-to-display-binary-search-tree-in-console-properly
string printLevel(BRnode *root, int level, string gap) {
  if (level==1) {
    if (root == 0) {
      //cout << ".. printLevel - " << root << ": " << gap << "-" << gap << "\n";
      return gap + "-" + gap;
    }
    stringstream out;
    out<<root->hash<<":"<<root->id;
    //cout << ".. printLevel - " << root << ": " << gap << root->data << gap << "\n";
    return gap + out.str() + gap;
  } else if (level>1) {
    string leftStr = printLevel(root ? root->left : 0, level-1, gap);
    string rightStr = printLevel(root ? root->right : 0, level-1, gap);

    //cout << ".. printLevel - " << root << ": '" << leftStr << "', '" << rightStr << "'\n";
    return leftStr + " " + rightStr;
  } else return "";
}

void printLevelOrder (BRnode* root, int depth) {
  for (int i=1; i<=depth; i++) {
    string gap="";
    for (int j=0; j<pow(2,depth-i)-1; j++) {
      gap+=" ";
    }
    string levelNodes = printLevel(root, i, gap);
    cout<<levelNodes<<endl;
  }
}


void display_data(nodePage* listData, int size)
{
    for (int i = 0; i < size; i++)
    {
        cout << "Hash: " << listData[i].hash << "  Page ID: " << listData[i].id << endl;
    }
}






int main()
{
    int input;
    string input2;
    BRtree treeSta;
    BRtree treeUnsta;
    int size_1 = 10;

    srand(time(NULL));

    nodePage data1[size_1];
    nodePage data2[size_1];
    nodePage data3[size_1];

    //first set
    for (int i = 0; i < size_1; i++)
    {
        data1[i].hash = rand()%300 + 1;
        data1[i].id = 100 + i;
    }
    //second set
    for (int i = 0; i < size_1; i++)
    {
        data2[i].hash = rand()%300 + 1;
        data2[i].id = 200 + i;
    }
    //third set
    for (int i = 0; i < size_1; i++)
    {
        data3[i].hash = rand()%300 + 1;
        data3[i].id = 300 + i;
    }


    //manipulate data for testing
    data2[4].hash = data1[4].hash;
    data2[7].hash = data2[8].hash;
    data3[2].id = data2[4].id;


    //Load()
    cout << "Load following data into unstable tree:\n";
    display_data(data1, size_1 - 5);
    cout << "\n\nClick enter to continue.\n";
    cin.ignore();
    Load(data1, size_1 - 5, &treeUnsta);
    cout << "\nStable tree:\n";
    printLevelOrder(treeSta.root, 6);
    cout << "\nUnstable tree:\n";
    printLevelOrder(treeUnsta.root, 6);

    cout << "\nAll nodes were drawn. (hash:id)\n"
        << "The black leaves all have data -1:-99\n\n";

    //Update1
    cout << "Update following data:\n";
    display_data(data2, size_1);
    cout << "\n\nClick enter to continue.\n";
    cin.ignore();

    Update(data2, size_1, &treeSta, &treeUnsta);
    cout << "\nData was successfully updated.\n\n";

    cout << "\nClick enter to continue.\n";
    cin.ignore();

    //Update2
    cout << "Update following data:\n";
    display_data(data3, size_1-5);
    cout << "\nClick enter to continue.\n";
    cin.ignore();
    Update(data3, size_1-5, &treeSta, &treeUnsta);
    cout << "\nData was successfully updated.\n\n";

    cout << "\n\nProgram ends. Thanks for using.\n";


    return 0;
}
