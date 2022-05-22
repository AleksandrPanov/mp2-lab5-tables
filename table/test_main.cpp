#include <iostream>
#include <string>
#include <ctime>
#include <thread>

#include "table.h"
#include "stock_market.h"

using namespace std;

int HashFunctionHorner(const std::string& s, int table_size, const int key)
{
    int hash_result = 0;
    for (int i = 0; s[i] != s.size(); ++i)
        hash_result = (key * hash_result + s[i]) % table_size;
    hash_result = (hash_result * 2 + 1) % table_size;
    return hash_result;
}
struct HashFunction1
{
    int operator()(const std::string& s, int table_size) const
    {
        return HashFunctionHorner(s, table_size, table_size - 1); // ключи должны быть взаимопросты, используем числа <размер таблицы> плюс и минус один.
    }
};
struct HashFunction2
{
    int operator()(const std::string& s, int table_size) const
    {
        return HashFunctionHorner(s, table_size, table_size + 1);
    }
};

void tableTest()
{
    SimpleTable<int, int> table;
    table.insert(1, 2);
    table.insert(2, 3);
    auto res = table.find(1);
    cout << *res << endl;
    cout << table.getSize() << endl;
    vector<int> t;
}
void timeTest()
{
    // UNIX Time // POSIX-time
    // 00:00:00 1.01.1970
    int64_t t = getNowTime();
    cout << t << endl;
    struct tm tm;
    localtime_s(&tm, &t);
    char date[20];
    strftime(date, sizeof(date), "%Y-%m-%d", &tm);
    string d = date;
    cout << d << endl;
}
void hashTabe()
{
    std::hash<int64_t> hash64; // hash64 - функтор
    //cout << hash64(2) << " " << hash64(4) << " " << hash64(8) << " " << hash64(16) << " " << hash64(32) << " " << hash64(64) << endl;
    //cout << hash64(128)<<" "<<hash64(256) << " " << hash64(512) << " " << hash64(1024) << " " << hash64(2048) << " "<< hash64(4096) << " " << hash64(8192) << endl;
    //cout << hash64(1) << " " << hash64(2) << " " << hash64(3) << " " << hash64(4) << " " << hash64(5) << " " << hash64(6) << endl;

    // hash
    // 1. детерминированная, key -> x1
    // 2. быстрое вычисление
    // 3. невозможность почитать по хэш функции изначальное значение ключа
    // 4. небольшое изменение ключа должно прнивести к значительному изменению хеша
    // 5. невозможность быстро найти ключи с одинаковой хеш функцией

    // Key % p = hash(key)

    // SHA-3
    // N байт - 256 бит
    // hash(x), x - 8 byte
    // hash(25 byte) -> 1 byte -> 

    // SHA-3
}
void threadTest()
{
    StockMarket market;
    Order req(Order::Type::BUY, 950, 100);
    Order order(Order::Type::BUY, 900, 90);

    const int numReq = 1000;

    std::thread thread([&]() {
        for (int i = 0; i < numReq; i++)
        {
            market.addRequestSafe(order);
        }
        });
    int counter = 0;
    while (counter != numReq)
    {
        if (market.processFirstRequestSafe())
            counter++;
    }
    thread.join();
}

void testTables()
{
    //sort
    std::cout << "SortTable test" << std::endl;
    SortTable<std::string, std::string> table1;
    table1.insert("Circle", "black");
    table1.insert("Square", "white");
    table1.insert("Triangle", "blue");
    table1.remove("Square");

    auto it1 = table1.find("Circle");
    if (it1 != table1.end())
        std::cout << "Circle is " << *it1 << std::endl;

    auto it2 = table1.find("Square");
    if (it2 != table1.end())
        std::cout << "Square is " << *it2 << std::endl;

    std::cout << "Triangle is " << table1["Triangle"] << std::endl;

    for (auto& val : table1)
        std::cout << val << " ";
    std::cout << "\n\n";

    //hash
    std::cout << "HashTable test" << std::endl;
    HashTable<std::string, std::string> table2;
    table2.insert("Circle", "black");
    table2.insert("Square", "white");
    table2.insert("Triangle", "blue");
    table2.remove("Square");

    auto it3 = table2.find("Circle");
    if (it3 != table2.end())
        std::cout << "Circle is " << *it3 << std::endl;

    auto it4 = table2.find("Square");
    if (it4 != table2.end())
        std::cout << "Square is " << *it4 << std::endl;

    std::cout << "Triangle is " << table2["Triangle"] << std::endl;

    for(auto& val : table2)
        std::cout << val << " ";
    std::cout << "\n\n";


    //avl
    std::cout << "AvlTable test" << std::endl;
    AvlTable<std::string, std::string> table3;
    table3.insert("Circle", "black");
    table3.insert("Square", "white");
    table3.insert("Triangle", "blue");
    table3.remove("Square");

    auto* pair1 = table3.find("Circle");
    if (pair1 != nullptr)
        std::cout << "Circle is " << pair1->second << std::endl;

    auto* pair2 = table3.find("Square");
    if (pair2 != nullptr)
        std::cout << "Square is " << pair2->second << std::endl;

    std::cout << "Triangle is " << table3["Triangle"] << std::endl;
    std::cout << "\n\n";
}

void main()
{
    testTables();

    //StockMarket market;

    //market.addRequestSafe(Registration(10));
    //market.addRequestSafe(Registration(12));

    //market.addRequestSafe(Order(Order::Type::BUY, 1000, 10));
    //market.addRequestSafe(Order(Order::Type::SELL, 1001, 12));
    //market.addRequestSafe(Order(Order::Type::SELL, 1000, 12));
    //market.addRequestSafe(Order(Order::Type::BUY, 1100, 10));

    //market.addRequestSafe(Order(Order::Type::BUY, 1100, 10));
    //market.addRequestSafe(Order(Order::Type::BUY, 1100, 10));
    //market.addRequestSafe(Order(Order::Type::BUY, 1100, 10));
    //market.addRequestSafe(Order(Order::Type::BUY, 1100, 10));
    //market.addRequestSafe(Order(Order::Type::BUY, 1100, 10));


    //market.addRequestSafe(Order(Order::Type::SELL, 1000, 12));

    //auto order = Order(Order::Type::BUY, 100, 12);
    //market.addRequestSafe(order);
    //market.addRequestSafe(CancelOrder(order));

    //for (int i = 0; i < 10000; i++)
    //    market.processFirstRequestSafe();
}
