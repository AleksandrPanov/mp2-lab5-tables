#include <iostream>
#include "table.h"

using namespace std;

void testTable(BaseTable<int, int>* table) {
    table->insert(1, 100);
    table->insert(2, 104);
    table->insert(3, 98);
    table->insert(4, 106);
    table->remove(2);
    auto it1 = table->find(3);
    if (it1 != table->end())
        cout << *it1 << endl;
    auto it2 = table->find(2);
    if (it2 == table->end())
        cout << "There is no element with key = 2 after remove" << endl << endl;
}

void testAvl(AVLTree<int, int>* tree) {
    tree->insert(1, 100);
    tree->insert(2, 104);
    tree->insert(3, 98);
    tree->insert(4, 106);
    tree->remove(2);
    pair<int, int>* elem = tree->find(3);
    if (elem != nullptr)
        cout << elem->second << endl;

    pair<int, int>* elem2 = tree->find(2);
    if (elem2 == nullptr)
        cout << "There is no element with key = 2 after remove" << endl << endl;  
}

void testTables()
{
    cout << "SimpleTable test" << std::endl;
    SimpleTable<int, int>* table1 = new SimpleTable<int, int>();
    testTable(table1);

    cout << "SortTable test" << std::endl;
    SortTable<int, int>* table2 = new SortTable<int, int>();
    testTable(table2);

    cout << "HashTable test" << std::endl;
    HashTable<int, int>* table3 = new HashTable<int, int>();
    testTable(table3);

    cout << "AVL tree test" << std::endl;
    AVLTree<int, int>* table4 = new AVLTree<int, int>();
    testAvl(table4);

    cout << endl;
}

void main()
{
    testTables();
}