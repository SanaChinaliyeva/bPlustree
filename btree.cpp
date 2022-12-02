#include "btree.h"
#include <fstream>
#include <iostream>

BTree::BTree(string filename) : root(nullptr), filename(filename) {
  ifstream file(filename, ios::in);
  if (file.is_open()) {
    state = true;
    string value;
    while (file >> value)
      insert(value);

    file.close();
  } else {
    state = false;
  }
}

void BTree::insert(string k) {
  if (!root) {
    root = new Node();
    root->leaf = true;
    root->key[0] = k;
    root->size = 1;
  } else {
    Node *temp = rangeSearch(root, k);
    if (temp->size < 4) {
      temp->key = keyInsert(temp->key, k, temp->size);
      temp->size += 1;
      temp->children[temp->size] = temp->children[temp->size - 1];
      temp->children[temp->size - 1] = nullptr;
    } else {
      Node *helper = new Node();
      helper->leaf = true;
      helper->parent = temp->parent;

      string *_key = new string[temp->size + 1];
      for (size_t i = 0; i < temp->size; i++)
        _key[i] = temp->key[i];

      _key = keyInsert(_key, k, temp->size);

      temp->size = 2;
      helper->size = 3;

      for (size_t i = 0; i < temp->size; i++)
        temp->key[i] = _key[i];

      for (size_t i = 0; i < helper->size; i++)
        helper->key[i] = _key[temp->size + i];

      temp->children[temp->size] = helper;
      helper->children[helper->size] = temp->children[4];
      temp->children[4] = nullptr;

      delete[] _key;

      string key = helper->key[0];
      if (!temp->parent) {
        Node *parent = new Node();
        parent->key[0] = key;
        parent->size += 1;

        temp->parent = parent;
        helper->parent = parent;

        parent->children[0] = temp;
        parent->children[1] = helper;

        root = parent;
      } else {
        parentInsert(temp->parent, helper, key);
      }
    }
  }
}

string *BTree::keyInsert(string *keys, string key, size_t size) {
  size_t i;
  for (i = 0; i < size; i++) {
    if (key < keys[i])
      break;
  }

  for (size_t j = size; j > i; j--)
    keys[j] = keys[j - 1];

  keys[i] = key;

  return keys;
}

Node *BTree::rangeSearch(Node *node, string key) {
  if (!node)
    return nullptr;

  else {
    Node *temp = node;

    while (!temp->leaf) {
      for (size_t i = 0; i < temp->size; i++) {
        if (key < temp->key[i]) {
          temp = temp->children[i];
          break;
        }
        if (i == temp->size - 1) {
          temp = temp->children[i + 1];
          break;
        }
      }
    }

    return temp;
  }
}

Node *BTree::keyInsert(Node *node, Node *child, string key) {
  size_t keyIndex;
  size_t childIndex;
  for (size_t i = 0; i < node->size; i++) {
    if (key < node->key[i]) {
      keyIndex = i;
      childIndex = i + 1;
      break;
    } else if (i == node->size - 1) {
      keyIndex = node->size;
      childIndex = node->size + 1;
      break;
    }
  }

  for (size_t j = node->size; j > keyIndex; j--)
    node->key[j] = node->key[j - 1];

  for (size_t j = node->size + 1; j > childIndex + 1; j--)
    node->children[j] = node->children[j - 1];

  node->key[keyIndex] = key;
  node->children[childIndex] = child;

  return node;
}

Node **BTree::childInsert(Node **children, Node *child, size_t size,
                          size_t index) {
  for (size_t i = size; i > index; i--)
    children[i] = children[i - 1];

  children[index] = child;
  return children;
}

size_t BTree::findIndex(string *keys, string key, size_t size) {
  size_t i;
  for (i = 0; i < size; i++) {
    if (key < keys[i])
      break;
  }
  return i;
}

void BTree::parentInsert(Node *parent, Node *child, string key) {
  Node *temp = parent;

  if (temp->size < 4) {
    temp = keyInsert(temp, child, key);
    temp->size += 1;
  } else {
    Node *helper = new Node();
    helper->parent = temp->parent;

    string *_key = new string[temp->size + 1];
    for (size_t i = 0; i < temp->size; i++)
      _key[i] = temp->key[i];

    _key = keyInsert(_key, key, temp->size);

    Node **copy = new Node *[temp->size + 2];
    for (size_t i = 0; i <= temp->size; i++)
      copy[i] = temp->children[i];

    copy[temp->size + 1] = nullptr;
    copy = childInsert(copy, child, temp->size + 1,
                       findIndex(_key, key, temp->size + 1));

    temp->size = 5 / 2;
    helper->size = 5 / 2;

    for (size_t i = 0; i < temp->size; i++)
      temp->key[i] = _key[i], temp->children[i] = copy[i];

    temp->children[temp->size] = copy[temp->size];

    for (size_t i = 0; i < helper->size; i++)
      helper->key[i] = _key[i + temp->size + 1],
      helper->children[i] = copy[i + temp->size + 1],
      helper->children[i]->parent = helper;

    helper->children[helper->size] = copy[temp->size + helper->size + 1];
    helper->children[helper->size]->parent = helper;

    string __key = _key[5 / 2];

    delete[] _key;
    delete[] copy;

    if (!temp->parent) {
      Node *parent = new Node();
      parent->key[0] = __key;
      parent->size++;

      temp->parent = parent;
      helper->parent = parent;

      parent->children[0] = temp;
      parent->children[1] = helper;

      root = parent;
    } else {
      parentInsert(temp->parent, helper, __key);
    }
  }
}

void BTree::parentRemove(Node *node, Node *parent, size_t index) {
  Node *remove = node;
  Node *temp = parent;
  string val = temp->key[index];

  if (temp == root && temp->size == 1) { // root case
    if (remove == temp->children[0]) {
      delete[] remove->key;
      delete[] remove->children;
      delete remove;

      root = temp->children[1];
      delete[] temp->key;
      delete[] temp->children;
      delete temp;
      return;
    }

    else if (remove == temp->children[1]) {
      delete[] remove->key;
      delete[] remove->children;
      delete remove;

      this->root = temp->children[0];
      delete[] temp->key;
      delete[] temp->children;
      delete temp;
      return;
    }
  }

  for (int i = index; i < temp->size - 1; i++)
    temp->key[i] = temp->key[i + 1];

  temp->key[temp->size - 1] = "";

  int indexR = -1;
  for (int i = 0; i < temp->size + 1; i++)
    if (temp->children[i] == remove)
      indexR = i;

  if (indexR == -1)
    return;

  for (int i = indexR; i < temp->size; i++)
    temp->children[i] = temp->children[i + 1];

  temp->children[temp->size] = nullptr;
  temp->size -= 1;

  if (temp == root)
    return;

  if (temp->size < 2) {
    int indexR = -1;
    for (int i = 0; i <= temp->parent->size; i++)
      if (temp == temp->parent->children[i])
        indexR = i;

    int left = indexR - 1, right = indexR + 1;

    if (left >= 0) {
      Node *helper = temp->parent->children[left];
      if (helper->size > 2) {
        string *_key = new string[temp->size + 1];
        for (int i = 0; i < temp->size; i++)
          _key[i] = temp->key[i];

        keyInsert(_key, temp->parent->key[left], temp->size);
        for (int i = 0; i < temp->size + 1; i++)
          temp->key[i] = _key[i];

        temp->parent->key[left] = helper->key[helper->size - 1];
        delete[] _key;

        Node **copy = new Node *[temp->size + 2];
        for (int i = 0; i <= temp->size; i++)
          copy[i] = temp->children[i];

        childInsert(copy, helper->children[helper->size], temp->size, 0);
        for (int i = 0; i <= temp->size + 1; i++)
          temp->children[i] = copy[i];

        delete[] copy;

        temp->size++;
        helper->size--;
        return;
      }
    }

    if (right <= temp->parent->size) {
      Node *helper = temp->parent->children[right];

      if (helper->size > 2) {
        string *_key = new string[temp->size + 1];

        for (int i = 0; i < temp->size; i++)
          _key[i] = temp->key[i];

        keyInsert(_key, temp->parent->key[indexR], temp->size);
        for (int i = 0; i < temp->size + 1; i++)
          temp->key[i] = _key[i];

        temp->parent->key[indexR] = helper->key[0];
        delete[] _key;

        temp->children[temp->size + 1] = helper->children[0];
        for (int i = 0; i < helper->size; i++)
          helper->children[i] = helper->children[i + 1];

        helper->children[helper->size] = nullptr;
        temp->size += 1;
        helper->size--;
        return;
      }
    }
    if (left >= 0) {
      Node *helper = temp->parent->children[left];
      helper->key[helper->size] = temp->parent->key[left];

      for (int i = 0; i < temp->size; i++)
        helper->key[helper->size + i + 1] = temp->key[i];

      for (int i = 0; i <= temp->size; i++)
        helper->children[helper->size + i + 1] = temp->children[i],
                                            temp->children[i]->parent = helper;

      for (int i = 0; i < temp->size + 1; i++)
        temp->children[i] = nullptr;

      helper->size = helper->size + temp->size + 1;
      parentRemove(temp, temp->parent, left);
      return;
    }
    if (right <= temp->parent->size) {
      Node *helper = temp->parent->children[right];

      temp->key[temp->size] = temp->parent->key[right - 1];
      for (int i = 0; i < helper->size; i++)
        temp->key[temp->size + 1 + i] = helper->key[i];

      for (int i = 0; i < helper->size + 1; i++)
        temp->children[temp->size + i + 1] = helper->children[i],
                                        helper->children[i]->parent = helper;

      for (int i = 0; i <= helper->size; i++)
        helper->children[i] = nullptr;

      helper->size = helper->size + temp->size + 1;
      parentRemove(helper, temp->parent, right - 1);
      return;
    }
  }
}

void BTree::clear(Node *node) {
  if (node) {
    if (!node->leaf)
      for (int i = 0; i <= node->size; i++)
        clear(node->children[i]);

    delete[] node->key;
    delete[] node->children;
    delete node;
  }
}

void BTree::remove(string k) {
  Node *temp = root;

  temp = rangeSearch(temp, k);
  size_t index = -1;
  for (size_t i = 0; i < temp->parent->size + 1; i++) {
    if (temp == temp->parent->children[i])
      index = i;
  }
  size_t left = index - 1, right = index + 1;

  index = -1;
  for (size_t i = 0; i < temp->size; i++) {
    if (temp->key[i] == k) {
      index = i;
      break;
    }
  }

  if (index == -1)
    return;

  for (size_t i = index; i < temp->size - 1; i++)
    temp->key[i] = temp->key[i + 1];

  temp->key[temp->size - 1] = "";
  temp->size -= 1;

  if (temp == root && !temp->size) {
    clear(root);
    root = nullptr;
    return;
  }

  temp->children[temp->size] = temp->children[temp->size + 1];
  temp->children[temp->size + 1] = nullptr;

  if (temp == root)
    return;

  if (temp->size < 2) {
    if (left >= 0) {
      Node *helper = temp->parent->children[left];

      if (helper->size > 2) {
        string *_key = new string[temp->size + 1];
        for (size_t i = 0; i < temp->size; i++)
          _key[i] = temp->key[i];

        keyInsert(_key, helper->key[helper->size - 1], temp->size);

        for (size_t i = 0; i < temp->size + 1; i++)
          temp->key[i] = _key[i];

        temp->size += 1;
        delete[] _key;

        temp->children[temp->size] = temp->children[temp->size - 1];
        temp->children[temp->size - 1] = nullptr;

        helper->key[helper->size - 1] = "";
        helper->size -= 1;
        helper->children[helper->size] = helper->children[helper->size + 1];
        helper->children[helper->size + 1] = nullptr;

        temp->parent->key[left] = temp->key[0];

        return;
      }
    }
    if (right <= temp->parent->size) {
      Node *helper = temp->parent->children[right];

      if (helper->size > 2) {
        string *_key = new string[temp->size + 1];
        for (size_t i = 0; i < temp->size; i++)
          _key[i] = temp->key[i];

        keyInsert(_key, helper->key[0], temp->size);

        for (size_t i = 0; i < temp->size + 1; i++)
          temp->key[i] = _key[i];

        temp->size += 1;
        delete[] _key;

        temp->children[temp->size] = temp->children[temp->size - 1];
        temp->children[temp->size - 1] = nullptr;

        for (size_t i = 0; i < helper->size - 1; i++)
          helper->key[i] = helper->key[i + 1];

        helper->key[helper->size - 1] = "";
        helper->size -= 1;
        helper->children[helper->size] = helper->children[helper->size + 1];
        helper->children[helper->size + 1] = nullptr;

        temp->parent->key[right - 1] = helper->key[0];

        return;
      }
    }
    if (left >= 0) {
      Node *helper = temp->parent->children[left];

      for (size_t i = 0; i < temp->size; i++)
        helper->key[helper->size + i] = temp->key[i];

      helper->children[helper->size] = nullptr;
      helper->size = helper->size + temp->size;
      helper->children[helper->size] = temp->children[temp->size];

      parentRemove(temp, temp->parent, left);

      for (int i = 0; i < temp->size; i++)
        temp->key[i] = "", temp->children[i] = nullptr;

      temp->children[temp->size] = nullptr;

      delete[] temp->key;
      delete[] temp->children;
      delete temp;

      return;
    }
    if (right <= temp->parent->size) {
      Node *helper = temp->parent->children[right];

      for (int i = 0; i < helper->size; i++)
        temp->key[i + temp->size] = helper->key[i];

      temp->children[temp->size] = nullptr;
      temp->size = helper->size + temp->size;
      temp->children[temp->size] = helper->children[helper->size];

      parentRemove(helper, temp->parent, right - 1);

      for (int i = 0; i < helper->size; i++)
        helper->key[i] = "", helper->children[i] = nullptr;

      helper->children[helper->size] = nullptr;

      delete[] helper->key;
      delete[] helper->children;
      delete helper;
      return;
    }
  }
}

void BTree::print() { printNode(root); }

void BTree::printNode(Node *node) {
  if (node) {
    for (size_t i = 0; i < node->size; i++)
      cout << node->key[i] << " ";

    cout << endl;

    if (!node->leaf)
      for (size_t i = 0; i <= node->size; i++)
        printNode(node->children[i]);
  }
}

Node *BTree::find(string k) {
  if (!root)
    return nullptr;
  else {
    Node *temp = root;

    while (!temp->leaf) {
      for (int i = 0; i < temp->size; i++) {
        if (k < temp->key[i]) {
          temp = temp->children[i];
          break;
        }
        if (i == temp->size - 1) {
          temp = temp->children[i + 1];
          break;
        }
      }
    }

    for (int i = 0; i < temp->size; i++)
      if (temp->key[i] == k)
        return temp;

    return nullptr;
  }
}

void BTree::search(string k1, string k2) {
  Node *start = find(k1);
  Node *end = find(k2);

  if (!start || !end || k1 > k2) {
    cout << "Invalid range!" << endl;
    return;
  } else {
    size_t i;
    for (i = 0; i < start->size; i++)
      if (start->key[i] == k1)
        break;

    for (; i <= start->size; i++) {
      if (i == start->size) {
        start = start->children[i];
        i = 0;
        continue;
      }
      cout << start->key[i] << " ";
      if (start->key[i] == k2) {
        cout << endl;
        return;
      }
    }
  }
}

size_t BTree::nodes(Node *node) {
  if (!node)
    return 0;
  if (node->leaf)
    return 1;
  size_t count = 1;
  for (size_t i = 0; i < node->size; i++) {
    count += height(node->children[i]);
  }
  return count;
}

size_t BTree::height(Node *node) {
  if (!node)
    return 0;
  if (node->leaf)
    return 1;
  return 1 + height(node->children[0]);
}

size_t BTree::entries() {
  if (!root)
    return 0;

  Node *temp = root;
  while (!temp->leaf)
    temp = temp->children[0];

  size_t counter = 0;
  for (size_t i = 0; i <= temp->size; i++) {
    if (!temp)
      break;
    if (temp->key[i] != "")
      counter += 1;
    if (i == temp->size) {
      temp = temp->children[i];
      i = 0;
      continue;
    }
  }

  return counter;
}

float BTree::fillfactor() {
  if (!root)
    return 0;

  Node *temp = root;
  while (!temp->leaf)
    temp = temp->children[0];

  float total = 0.0;
  size_t count = 0;
  while (temp) {
    count += 1;
    total += (float)temp->size / 5.0;
    temp = temp->children[temp->size];
  }
  return total / count;
}

void BTree::statistics() {
  cout << "Statistics of the B+-tree: " << endl;
  cout << "Total number of nodes: " << nodes(root) << endl;
  cout << "Total number of data entries: " << entries() << endl;
  cout << "Total number of index entries: " << height(root) << endl;
  cout << "Average fill factor: " << fillfactor() << endl;
  cout << "Height of tree: " << height(root) << endl;
}

BTree::~BTree()
{
  clear(root);
}