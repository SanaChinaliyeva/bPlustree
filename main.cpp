#include <iostream>
#include "btree.h"
using namespace std;

int main() {
  string filename;
  cout << "Enter file name: ";
  cin >> filename;
  BTree tree(filename);
  if (tree.getState())
  {
    cout << "Building an initial B+-tree..." << endl 
         << "Launching the B+-tree test program…" << endl;
    
    string command;
    while (command != "Quit" && command != "quit")
    {
      cout << "Waiting for your commands: ";
      cin >> command;
      cout << endl;
      if (command == "insert")
      {
        string value;
        cin >> value;
        tree.insert(value);
        cout << "The key " << value << " has been inserted in the B+-tree!" << endl << endl;
      } else if (command == "delete")
      {
        string value;
        cin >> value;
        tree.remove(value);
        cout << "The key " << value << " has been deleted in the B+-tree." << endl << endl;
      } 
      else if (command == "search")
      {
        string low, high;
        cin >> low >> high;
        tree.search(low, high);
        cout << endl;
      } 
      else if(command == "Print" || command == "print")
      {
        cout << "B+ tree: " << endl << endl;
        tree.print();
        cout << endl;
      } else if(command == "Stats" || command == "stats")
      {
        tree.statistics();
      }
    }

    cout << "The program is terminated.";
    return 0;
  }
  cout << "File not found" << endl;
  return 1;
}