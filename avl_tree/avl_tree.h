struct avl_node
{
  int data;
  struct avl_node* left_ptr;
  struct avl_node* right_ptr;
  int degree;
};

bool numberValidation();
struct avl_node* createNode(int);
struct avl_node* insertNode(struct avl_node *,int);
struct avl_node* deleteNode(struct avl_node *,int);
void displayTree(struct avl_node*);
bool searchElement(struct avl_node * root,int);
int degree(struct avl_node*);
int maximum(int,int);
struct avl_node* getMinimumNode(struct avl_node*);
int balance(struct avl_node*);
struct avl_node* leftRotate(struct avl_node*);
struct avl_node* rightRotate(struct avl_node*);
