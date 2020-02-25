#include "avl_tree.h"

 /* @author: Subhash S
  * @function: creates a node when insert function is called
  * @param 1:integer value is passed in the function to create node with it
  * @return : structure pointer of the created node is returned*/
struct avl_node* createNode(int value)
{
      struct avl_node* root = (struct avl_node*)malloc(sizeof(struct avl_node));
      if(!root)
      {
          printf("!! Memory not allocated !!\n");
          return NULL;
      }
      root->data = value;
      root->left_ptr = NULL;
      root->right_ptr = NULL;
      return root;
}
/* @author: Subhash S
 * @function: to find the height of the given node
 * @param 1: structure pointer of the node whose height to be found
 * @return : returns the height of the give node */
int height(struct avl_node *root)
{
    if(!root)
    {
        return 0;
    }
    else
    {
        int height_left = height(root->left_ptr);
        int height_right = height(root->right_ptr);
        if(height_left > height_right)
        {
            return height_left+1;
        }
        else
        {
            return height_right+1;
        }
    }
}
/* @author: Subhash S
 * @function: to insert a node in the tree
 * @param 1: structure pointer of the root node
 * @param 2: value of the node to be inserted
 * @return : returns the root node's structure pointer */
struct avl_node* insertNode(struct avl_node* root,int value)
{
      int balance_factor;
      if(root == NULL)
      {
          return createNode(value);
      }
      if(value < root->data)
      {
          root->left_ptr = insertNode(root->left_ptr,value);
      }
      else if(value > root->data)
      {
          root->right_ptr = insertNode(root->right_ptr,value);
      }
      else
      {
          printf("\n !! Given element is already present in the tree !! \n");
          return root;
      }

/*Getting the balance factor of the current node*/
       balance_factor = getBalanceFactor(root);

/* avl balancing rotations*/
      if(balance_factor > 1)
      {
          if(value < root->left_ptr->data)
          {
              root = rightRotate(root);
          }
          else
          {
              root->left_ptr = leftRotate(root->left_ptr);
              root = rightRotate(root);
          }
      }
      else if(balance_factor < -1)
      {
          if(value > root->right_ptr->data)
          {
              root = leftRotate(root);
          }
          else
          {
              root->right_ptr = rightRotate(root->right_ptr);
              root = leftRotate(root);
          }
      }
      return root;
}

/* @author: Subhash S
 * @function: to delete a node in the tree
 * @param 1: structure pointer of the root node
 * @param 2: value of the node to be deleted
 * @return : returns the structure pointer of the root node of the tree */
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
          root->left_ptr = deleteNode(root->left_ptr,value);
      }
      else if(value > root->data)
      {
          root->right_ptr = deleteNode(root->right_ptr,value);
      }
      else
      {
            if(root->left_ptr == NULL || root->right_ptr == NULL)
            {
                struct avl_node* temp = root->left_ptr? root->left_ptr :
                                                      root->right_ptr;
                if(!temp)
                {
                    temp = root;
                    root = NULL;
                }
                else
                {
                    *root =  *temp;
                }
                free(temp);
                printf("Elemnet is deleted \n");
            }
            else
            {
                struct avl_node* temp = getMinimumNode(root->right_ptr);
                root->data = temp->data;
                root->right_ptr = deleteNode(root->right_ptr,temp->data);
            }
      }

      if(!root)
      {
          return root;
      }

/* Getting the balance factor of the current node */
      int balance_factor = getBalanceFactor(root);

/* avl balancing rotations */
      if(balance_factor < -1)
      {
          if(getBalanceFactor(root->right_ptr) > 0)
          {
              root->right_ptr = rightRotate(root->right_ptr);
              root = leftRotate(root);
          }
          else if(getBalanceFactor(root->right_ptr) <= 0)
          {
              root = leftRotate(root);
          }
      }
      else if(balance_factor > 1)
      {
          if(getBalanceFactor(root->left_ptr) >= 0)
          {
              root = rightRotate(root);
          }
          else if(getBalanceFactor(root->left_ptr) < 0)
          {
              root->left_ptr = leftRotate(root->left_ptr);
              root = rightRotate(root);
          }
      }
      return root;
}

/* @author: Subhash S
 * @function: to rotate the nodes with left rotation logic
 * @param 1: structure pointer of the node which is unbalanced
 * @return : returns the structure pointer of the balanced node */
struct avl_node* leftRotate(struct avl_node* node)
{
    if(!node)
    {
        return NULL;
    }
    struct avl_node* mid_node = node->right_ptr;
    struct avl_node* left_node = mid_node->left_ptr;
    mid_node->left_ptr = node;
    node->right_ptr = left_node;
    return mid_node;
}

/* @author: Subhash S
 * @function: to rotate the nodes with right rotation logic
 * @param 1: structure pointer of the node which is unbalanced
 * @return : returns the structure pointer of the balanced node */
struct avl_node* rightRotate(struct avl_node* node)
{
    if(!node)
    {
        return NULL;
    }
    struct avl_node* mid_node = node->left_ptr;
    struct avl_node* right_node = mid_node->right_ptr;
    mid_node->right_ptr = node;
    node->left_ptr = right_node;
    return mid_node;
}

/* @author: Subhash S
 * @function: to get the minimum node while deleting
 * @param 1: structure pointer of the root node from where the minimum node is found
 * @return : returns the structure pointer of the minimum node */
struct avl_node* getMinimumNode(struct avl_node* root)
{
    if(!root)
    {
        return NULL;
    }
    while(root->left_ptr != NULL)
    {
        root = root->left_ptr;
    }
    return root;
}

/* @author: Subhash S
 * @function: to get the balance factor of the given node
 * @param 1: structure pointer of the node whose balance factor to be found
 * @return : returns the interger value as balance factor */
int getBalanceFactor(struct avl_node* root)
{
    if(!root)
    {
        return NULL;
    }
    return (height(root->left_ptr)-height(root->right_ptr));
}

/* @author: Subhash S
 * @function: to dispaly the entire tree
 * @param 1: structure pointer of the root node
 * @param 2: initial space count for printing */
void displayTree(struct avl_node * root,int space_count)
{
    if(space_count < 0 && !root)
    {
        return;
    }
    space_count = space_count+1;
    displayTree(root->right_ptr,space_count);
    for(int temp = space_count;temp>1;temp--)
    {
        printf("   ");
    }
    printf("%d\t",root->data);
    printf("\n");
    space_count--;
    displayTree(root->left_ptr,space_count+1);
}
/* @author: Subhash S
 * @function: to search given element in the tree
 * @param 1: structure pointer of the root node of the tree from where serach is done
 * @param 2: integer value of the node to be searched
 * @return : returns a boolean value true if element is present or returns false*/
bool searchElement(struct avl_node * root,int value)
{
    if(!root)
    {
        return false;
    }
    if(root->data == value)
    {
        return true;
    }
    else
    {
        root = (value > root->data) ? root->right_ptr:root->left_ptr;
        return searchElement(root,value);
    }
}

/* @authhor: Subhash S
 * @function: to get the number and validate
 * @param 1: character pointer with validation message string
 * @return: returns interger value 0 if validation fails or returns the valid integer number
 */
int getNumberAndValidate(char *validation_msg)
{
    int number=0;
    while(!number)
    {
        scanf("%d",&number);
        if(getchar() != '\n')
        {
            while(getchar() != '\n');
            printf("%s",validation_msg);
        }
    }
    return number;
}
int main()
{
    struct avl_node* root = NULL;
    int choice,value;
    bool loop_run = true;
    char validation_msg1[]="enter the right choice ";
    char validation_msg2[]="enter the valid number ";
    while(loop_run)
    {
        printf("\n      AVL TREE      \n");
        printf("\n1-- Insert a node\n2-- Remove a node\
                    \n3-- Display a node\n4-- search element\
                    \n5-- Exit\n\nEnter your choice :");
    	choice = getNumberAndValidate(validation_msg1);
        printf("\n");
        switch(choice)
        {
            case INSERT_NODE:
                printf("\nEnter the value to be inserted  = ");
                value = getNumberAndValidate(validation_msg2);
                root = insertNode(root,value);
                break;
            case DELETE_NODE:
                if(root)
                {
                    value = getNumberAndValidate(validation_msg2);
                    root = deleteNode(root,value);
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
                    value = getNumberAndValidate(validation_msg2);
                    (searchElement(root,value))?printf("Given element is present\n"):
                                             printf("Given element is not present\n");
                }
                else
                {
                    printf("!!No element is present to search!!\n");
                }
                break;
            case EXIT:
                loop_run = false;
                break;
            default:
                printf("Enter the right choice\n");
                break;
        }
    }
    return 0;
}
