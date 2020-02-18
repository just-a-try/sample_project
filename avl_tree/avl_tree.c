#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "avl_tree.h"
// macro passed a as argument to display function for initial space count
#define INITIAL_SPACE_COUNT 0
enum menu{INSERT_NODE=1,DELETE_NODE,DISPLAY_TREE,SEARCH_ELEMENT,EXIT};

// Function to create new node
struct avl_node* createNode(int value)
{
      struct avl_node* root=(struct avl_node*)malloc(sizeof(struct avl_node));
      root->data=value;
      root->left_ptr=NULL;
      root->right_ptr=NULL;
      return root;
}
//Function to get the max height of the given node
int height(struct avl_node *root)
{
    if(!root)
    {
        return 0;
    }
    else
    {
        int height_left=height(root->left_ptr);
        int height_right=height(root->right_ptr);
        if(height_left>height_right)
        {
            return height_left+1;
        }
        else
        {
            return height_right+1;
        }
    }
}
// Function to insert a node with balancing conditions
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
          printf("\n !! Given element is already present in the tree !! \n");
          return root;
      }

// Getting the balance factor of the current node
      int balance_factor=balance(root);

// avl balancing rotations
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

// Function to delete a node with balancing conditions
struct avl_node* deleteNode(struct avl_node* root,int value)
{
      if(!searchElement(root,value))
      {
          printf("\n!! Given element is not present !!\n");
          return root;
      }
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
                    free(temp);
                    printf("Elemnet is deleted \n");
                }
                else
                {
                    *root=*temp;
                    free(temp);
                    printf("Elemnet is deleted \n");
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

// Getting the balance factor of the current node
      int balance_factor=balance(root);

// avl balancing rotations
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

// Function for avl tree left rotation
struct avl_node* leftRotate(struct avl_node* temp_node)
{
    struct avl_node* mid_node=temp_node->right_ptr;
    struct avl_node* left_node=mid_node->left_ptr;
    mid_node->left_ptr=temp_node;
    temp_node->right_ptr=left_node;
    return mid_node;
}

// Function for avl tree right rotation
struct avl_node* rightRotate(struct avl_node* temp_node)
{
    struct avl_node* mid_node=temp_node->left_ptr;
    struct avl_node* right_node=mid_node->right_ptr;
    mid_node->right_ptr=temp_node;
    temp_node->left_ptr=right_node;
    return mid_node;
}

// Function to get the minimum node for replacing in deletion
struct avl_node* getMinimumNode(struct avl_node* root)
{
    while(root->left_ptr!=NULL)
    {
        root=root->left_ptr;
    }
    return root;
}

// Function to get the balance factor of the given node
int balance(struct avl_node* root)
{
    if(root==NULL)
    {
        return 0;
    }
    return (height(root->left_ptr)-height(root->right_ptr));
}

// Function to display the nodes present like tree
void displayTree(struct avl_node * root,int space_count)
{
    if(!root)
    {
        return;
    }
    space_count=space_count+1;
    displayTree(root->right_ptr,space_count);
    for(int temp=space_count;temp>1;temp--)
    {
        printf("   ");
    }
    printf("%d\t",root->data);
    printf("\n");
    space_count--;
    displayTree(root->left_ptr,space_count+1);
}

//Function to search given element in the tree
bool searchElement(struct avl_node * root,int value)
{
    if(!root)
    {
        return false;
    }
    if(root->data==value)
    {
        return true;
    }
    else
    {
        root=(value>root->data)?root->right_ptr:root->left_ptr;
        return searchElement(root,value);
    }
}

// Function for number validation
bool numberValidation()
{
    if(getchar()!='\n')
    {
        while(getchar()!='\n');
        return false;
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
            if(numberValidation())
            {
                break;
            }
            else
            {
                printf("\nEnter the right choice\n");
            }
    	} while(1);
        printf("\n");
        switch(choice)
        {
            case INSERT_NODE:
                do
              	{
                     printf("\nEnter the value to be inserted =");
                     scanf("%d",&value);
                     if(numberValidation())
                     {
                         break;
                     }
                     else
                     {
                         printf("\nEnter valid number\n");
                     }
              	} while(1);
                root=insertNode(root,value);
                break;
          case DELETE_NODE:
                if(root)
                {
                    do
                    {
                         printf("\nEnter the value to be deleted =");
                         scanf("%d",&value);
                         if(numberValidation())
                         {
                             break;
                         }
                         else
                         {
                             printf("\nEnter valid number\n");
                         }
                    } while(1);
                    root=deleteNode(root,value);
                }
                else
                {
                    printf("!!No element is present to remove!!\n");
                }
                break;
          case DISPLAY_TREE:
                if(root)
                {
                    printf("Nodes present : \n");
                    displayTree(root,INITIAL_SPACE_COUNT);
                }
                else
                {
                    printf("!!No element is present to display!!\n");
                }
                break;
          case SEARCH_ELEMENT:
                if(root)
                {
                    do
                    {
                        printf("\nEnter the element to be searched =");
                        scanf("%d",&value);
                        if(numberValidation())
                        {
                            break;
                        }
                        else
                        {
                            printf("\nEnter valid number\n");
                        }
                    } while(1);
                    (searchElement(root,value))?printf("Given element is present\n"):
                                             printf("Given element is not present\n");
                }
                else
                {
                    printf("!!No element is present to search!!\n");
                }
                break;
          case EXIT:
                loop_run=false;
                break;
          default:
                printf("Enter the right choice\n");
                break;
        }
    }
    return 0;
}
