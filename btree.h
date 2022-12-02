#include <string>
using namespace std;

struct Node{
  bool leaf;
  size_t size;
  string* key;
  Node** children;
  Node* parent;
  
  Node()
  {
    leaf = false;
    size = 0; 
    
    key = new string[4];
    for (size_t i = 0; i < 4; i++)
      key[i] = "";
    
    children = new Node*[5];
    for (size_t i = 0; i < 5; i++)
      children[i] = nullptr;
    
    parent = nullptr;
  }
};

class BTree
{
private:
  Node *root;
  bool state;
  size_t size;
  string filename;

  Node*   rangeSearch(Node* root, string key);
  string* keyInsert(string* keys, string key, size_t size);
  Node*   keyInsert(Node* root, Node* child, string key);
  void    parentInsert(Node* parent, Node* child, string key);\
  Node**  childInsert(Node** children, Node* child, size_t size, size_t index);
  size_t  findIndex(string* keys, string key, size_t size);
  void    printNode(Node* node);
  void    clear(Node* node);
  void    parentRemove(Node* node, Node* parent, size_t index);
  Node*   find(string k);
  size_t  height(Node* node);
  size_t  nodes(Node* node);
  size_t  entries();
  size_t  ientries();
  float   fillfactor();
public:
  BTree(string filename);
  void insert(string k);
  void remove(string k);
  void search(string k1, string k2);
  void statistics();
  void print();
  bool getState() { return state; }
  ~BTree();
};