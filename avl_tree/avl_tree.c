#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
struct avl_node
{
  int data;
  struct avl_node* left_ptr;
  struct avl_node* right_ptr;
  int degree;
};

struct avl_node* createNode(int);
struct avl_node* insertNode(struct avl_node *,int);
void displayTree(struct avl_node *);
int degree(struct avl_node *);
int maximum(int,int);
int balance(struct avl_node*);
struct avl_node* leftRotate(struct avl_node *);
struct avl_node* rightRotate(struct avl_node *);


struct avl_node* createNode(int value)
{
  struct avl_node* root=(struct avl_node*)malloc(sizeof(struct avl_node));
  root->data=value;
  root->left_ptr=NULL;
  root->right_ptr=NULL;
  root->degree=1;
  return root;
}

struct avl_node* insertNode(struct avl_node * root,int value)
{
  if(root==NULL)
  {
    return createNode(value);
  }
  else if(value < root->data)
  {
    root->left_ptr=insertNode(root->left_ptr,value);
  }
  else if(value > root->data)
  {
    root->right_ptr=insertNode(root->right_ptr,value);
  }
  else
  {
    return root;
  }

  root->degree=1 + maximum(degree(root->left_ptr),degree(root->right_ptr));

  int balance_factor=balance(root);

  if(balance_factor > 1 && value < root->left_ptr->data)
  {
    return rightRotate(root);
  }
  if(balance_factor > -1 && value >root->left_ptr->data)
  {
    root->left_ptr=leftRotate(root->left_ptr);
    return rightRotate(root);
  }
  if(balance_factor < -1 && value > root->right_ptr->data)
  {
    return leftRotate(root);
  }
  if(balance_factor < -1 && value < root->right_ptr->data)
  {
    root->right_ptr=rightRotate(root->right_ptr);
    return leftRotate(root);
  }

    return root;


}

struct avl_node* leftRotate(struct avl_node* temp_node)
{
  struct avl_node* mid_node=temp_node->right_ptr;
  struct avl_node* left_node=temp_node;
  left_node->right_ptr=mid_node->left_ptr;
  mid_node->left_ptr=left_node;
  mid_node->degree=1 + maximum(degree(mid_node->left_ptr),degree(mid_node->right_ptr));
  left_node->degree=1 + maximum(degree(left_node->left_ptr),degree(left_node->right_ptr));
  return mid_node;
}

struct avl_node* rightRotate(struct avl_node* temp_node)
{
  struct avl_node* mid_node=temp_node->left_ptr;
  struct avl_node* right_node=temp_node;
  right_node->left_ptr=mid_node->right_ptr;
  mid_node->right_ptr=right_node;
  mid_node->degree=1 + maximum(degree(mid_node->left_ptr),degree(mid_node->right_ptr));
  right_node->degree=1 + maximum(degree(right_node->left_ptr),degree(right_node->right_ptr));
  return mid_node;
}

int balance(struct avl_node* root)
{
  return (degree(root->left_ptr)-degree(root->right_ptr));
}

int degree(struct avl_node* root)
{
  return ((root==NULL)? 0 : root->degree);
}

int maximum(int left_degree,int right_degree)
{
  return (left_degree>right_degree)? left_degree : right_degree;
}
void displayTree(struct avl_node * root)
{
  if(root==NULL)
  {
    return;
  }
  else
  {
    displayTree(root->left_ptr);
    printf("%d\t",root->data);
    displayTree(root->right_ptr);
  }
}

int main()
{
  struct avl_node* root=NULL;
  int choise,value;
  bool loop_run=true;
  while(loop_run)
  {
    printf("\n      AVL TREE      \n");
    printf("1-- Insert a node\n2-- Remove a node\n3-- Display a node\n4-- Exit\n\nEnter your choise :");
    scanf("%d",&choise);
    printf("\n");
    switch(choise)
    {
      case 1:
        printf("Enter the value to be inserted =");
        scanf("%d",&value);
        root=insertNode(root,value);
        break;
      case 2:
        // if(root->right_ptr->left_ptr==NULL && root->right_ptr->right_ptr==NULL)
        // {
        // printf("root value :%d",root->data);
        // }
        // printf("Enter the value to be removed =");
        // //scanf("%d",&value);
        //root=insertNode(root,value);
        break;
      case 3:
        printf("Nodes present : ");
        displayTree(root);
        break;
      case 4:
        loop_run=false;
        break;
      default:
        printf("enter the right choise\n");
        break;
    }
  }
  return 0;
}
