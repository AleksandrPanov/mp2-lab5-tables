#include <iostream>
#include <string>
#include <ctime>
#include <thread>
#include "table.h"
using namespace std;
int main()
{
    //hash
    std::cout << "HashTable test" << std::endl;
    HashTable<std::string, std::string> table2;
    table2.insert("Circle", "black");
    table2.insert("Square", "white");
    table2.insert("Triangle", "blue");
    table2.remove("Square");
    auto it3 = table2.find("Triangle");
    if (it3 != table2.end())
        std::cout << "Triangle is " << *it3 << std::endl;
    //avl
    std::cout << "AvlTable test" << std::endl;
    AVLTable<std::string, std::string> table3;
    table3.insert("Circle", "black");
    table3.insert("Square", "white");
    table3.insert("Triangle", "blue");
    table3.remove("Square");
    auto* pair1 = table3.find("Circle");
    if (pair1 != nullptr)
        std::cout << "Circle is " << pair1->second << std::endl;
     return 0;
}