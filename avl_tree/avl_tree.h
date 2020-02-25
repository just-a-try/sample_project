#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// macro for initial space count passed in  display function
#define INITIAL_SPACE_COUNT 0

enum menu
{
    INSERT_NODE=1,
    DELETE_NODE,
    DISPLAY_TREE,
    SEARCH_ELEMENT,
    EXIT
};
struct avl_node
{
  int data;
  struct avl_node* left_ptr;
  struct avl_node* right_ptr;
};

int getNumberAndValidate(char *);
struct avl_node* createNode(int);
struct avl_node* insertNode(struct avl_node *,int);
struct avl_node* deleteNode(struct avl_node *,int);
void displayTree(struct avl_node*,int);
bool searchElement(struct avl_node * root,int);
struct avl_node* getMinimumNode(struct avl_node*);
int getBalanceFactor(struct avl_node*);
struct avl_node* leftRotate(struct avl_node*);
struct avl_node* rightRotate(struct avl_node*);
int height(struct avl_node*);
