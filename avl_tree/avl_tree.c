#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "avl_tree.h"
struct avl_node* createNode(int value)
{
      struct avl_node* root=(struct avl_node*)malloc(sizeof(struct avl_node));
      root->data=value;
      root->left_ptr=NULL;
      root->right_ptr=NULL;
      root->degree=1;
      return root;
}

struct avl_node* insertNode(struct avl_node* root,int value)
{
      if(root==NULL)
      {
          return createNode(value);
      }
      if(value < root->data)
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
      if(balance_factor > 1 && value >root->left_ptr->data)
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

struct avl_node* deleteNode(struct avl_node* root,int value)
{
      if(!root)
      {
          return root;
      }
      if(value < root->data)
      {
          root->left_ptr=deleteNode(root->left_ptr,value);
      }
      else if(value > root->data)
      {
          root->right_ptr=deleteNode(root->right_ptr,value);
      }
      else
      {
            if(root->left_ptr==NULL || root->right_ptr==NULL)
            {
                struct avl_node* temp=root->left_ptr? root->left_ptr :
                                                      root->right_ptr;
                if(!temp)
                {
                    temp=root;
                    root=NULL;
                }
                else
                {
                    *root=*temp;
                    free(temp);
                }
            }
        else
        {
            struct avl_node* temp=getMinimumNode(root->right_ptr);
            root->data=temp->data;
            root->right_ptr=deleteNode(root->right_ptr,temp->data);
        }
      }
      if(!root)
      {
          return root;
      }
      root->degree=1 + maximum(degree(root->left_ptr),degree(root->right_ptr));
      int balance_factor=balance(root);
      if(balance_factor < -1 && balance(root->right_ptr)>0)
      {
          root->right_ptr=rightRotate(root->right_ptr);
          return leftRotate(root);
      }
      if(balance_factor < -1 && balance(root->right_ptr)<=0)
      {
          return leftRotate(root);
      }
      if(balance_factor > 1 && balance(root->left_ptr)>=0)
      {
          return rightRotate(root);
      }
      if(balance_factor > 1 && balance(root->left_ptr)<0)
      {
          root->left_ptr=leftRotate(root->left_ptr);
          return rightRotate(root);
      }
      return root;
}
struct avl_node* leftRotate(struct avl_node* temp_node)
{
  struct avl_node* mid_node=temp_node->right_ptr;
  struct avl_node* left_node=temp_node;
  left_node->right_ptr=mid_node->left_ptr;
  mid_node->left_ptr=left_node;
  mid_node->degree=1 + maximum(degree(mid_node->left_ptr),
                               degree(mid_node->right_ptr));
  left_node->degree=1 + maximum(degree(left_node->left_ptr),
                                degree(left_node->right_ptr));
  return mid_node;
}

struct avl_node* rightRotate(struct avl_node* temp_node)
{
  struct avl_node* mid_node=temp_node->left_ptr;
  struct avl_node* right_node=temp_node;
  right_node->left_ptr=mid_node->right_ptr;
  mid_node->right_ptr=right_node;
  mid_node->degree=1 + maximum(degree(mid_node->left_ptr),
                               degree(mid_node->right_ptr));
  right_node->degree=1 + maximum(degree(right_node->left_ptr),
                                 degree(right_node->right_ptr));
  return mid_node;
}

struct avl_node* getMinimumNode(struct avl_node* root)
{
    while(root->left_ptr!=NULL)
    {
        root=root->left_ptr;
    }
    return root;
}
int balance(struct avl_node* root)
{
    if(root==NULL)
    {
        return 0;
    }
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
    if(!root)
    {
        return;
    }
    displayTree(root->left_ptr);
    printf("%d\t",root->data);
    displayTree(root->right_ptr);
}
bool searchElement(struct avl_node * root,int value)
{
    if(root->data==value)
    {
        return true;
    }
    else
    {
        while(root!=NULL)
        {
            if(root->data==value)
            {
                return true;
            }
            root=(value>root->data)?root->right_ptr:root->left_ptr;

        }
    }
    return false;
}
bool numberValidation()
{
    if(getchar()!='\n')
    {
        while(getchar()!='\n');
        printf("\nEnter valid Number\n");
    }
    else
    {
        return true;
    }
}
int main()
{
    struct avl_node* root=NULL;
    int choice,value;
    bool loop_run=true;
    while(loop_run)
    {
        printf("\n      AVL TREE      \n");
    do
	{
        printf("\n1-- Insert a node\n2-- Remove a node\
                \n3-- Display a node\n4-- search element\
                \n5-- Exit\n\nEnter your choice :");
        scanf("%d",&choice);
	} while(!numberValidation());
    printf("\n");
    switch(choice)
    {
        case 1:
            do
          	{
                 printf("\nEnter the value to be inserted =");
                 scanf("%d",&value);
          	} while(!numberValidation());
            root=insertNode(root,value);
            break;
      case 2:
            if(root)
            {
                do
                {
                     printf("\nEnter the value to be deleted =");
                     scanf("%d",&value);
                } while(!numberValidation());
                root=deleteNode(root,value);
            }
            else
            {
                printf("!!No element is present to remove!!\n");
            }
            break;
      case 3:
            if(root)
            {
                printf("Nodes present : ");
                displayTree(root);
            }
            else
            {
                printf("!!No element is present to display!!\n");
            }
            break;
      case 4:
            if(root)
            {
                do
                {
                    printf("\nEnter the element to be searched =");
                    scanf("%d",&value);
                } while(!numberValidation());
                (searchElement(root,value))?printf("Given element is present\n"):
                                         printf("Given element is not present\n");
            }
            else
            {
                printf("!!No element is present to search!!\n");
            }
            break;
      case 5:
            loop_run=false;
            break;
      default:
            printf("enter the right choice\n");
            break;
    }
    }
    return 0;
}
