#include <iostream>
#include <string>
#include <ctime>
#include <algorithm>
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

void testMarket()
{
    StockMarket market;

    market.addRequestSafe(Registration(10));
    market.addRequestSafe(Registration(12));

    market.addRequestSafe(Order(Order::Type::BUY, 1000, 10));
    market.addRequestSafe(Order(Order::Type::SELL, 1001, 12));
    market.addRequestSafe(Order(Order::Type::SELL, 1000, 12));
    market.addRequestSafe(Order(Order::Type::BUY, 1100, 10));

    market.addRequestSafe(Order(Order::Type::BUY, 1100, 10));
    market.addRequestSafe(Order(Order::Type::BUY, 1100, 10));
    market.addRequestSafe(Order(Order::Type::BUY, 1100, 10));
    market.addRequestSafe(Order(Order::Type::BUY, 1100, 10));
    market.addRequestSafe(Order(Order::Type::BUY, 1100, 10));

    market.addRequestSafe(Order(Order::Type::SELL, 1000, 12));

    auto order = Order(Order::Type::BUY, 100, 12);
    market.addRequestSafe(order);
    market.addRequestSafe(CancelOrder(order));

    for (int i = 0; i < 10000; i++)
        market.processFirstRequestSafe();
}

void testSortTable()
{
    SortTable<std::string, int> table;

    table.insert("begin2", 21);
    table.insert("begin", 21);

    table.insert("begin1", 21);
    table.insert("begin6", 21);
    table.insert("begin4", 21);
    table.insert("begin5", 21);
    table.insert("begin3", 21);
                                    
    auto it3 = table.find("begin3");
    auto it1 = table.find("begin1");
    auto it2 = table.find("begin2");
    auto it6 = table.find("begin6");
    auto it7 = table.find("begin7");

    auto check1 = it6 == table.end();
    auto check2 = it7 == table.end();
}

void testHashTable()
{
    HashTable<std::string, int> table;

    for (auto it = table.begin(); it != table.end(); ++it)
        std::cout << *it << " ";

    table.insert("begin", 0);
    table.insert("begin1", 11);
    table.insert("begin2", 222);
    table.insert("begin3", 3333);
    table.insert("begin4", 444);
    table.insert("begin5", 55);
    table.insert("begin6", 6);

    auto it1 = table.find("begin3");
    auto val = *it1;

    //table.insert("begin3", 2241);

    auto it6 = table.find("begin100");

    auto check1 = it6 == table.end();
    auto check2 = it1 == table.end();

    table.remove("begin1");

    auto it3 = table.find("begin1");

    auto it4 = ++it1;
    auto it5 = ++it4;

    auto it8 = table.end();

    for (auto it = table.begin(); it != table.end(); ++it)
        std::cout << *it << " ";
}

void testAVLTable()
{
    auto table = AVLTable<int, int>();

    auto it = table.insert(100, 100);
    table.insert(1001, 222);
    table.insert(99, 33);

    table.insert(1002, 44);
    table.insert(1003, 55);
    table.insert(1004, 66);

    table.remove(1002);
    table.remove(1003);
    table.remove(1005);

    auto it1 = table.find(1004);
    auto itNull = table.find(1005);

    table.insert(1005, 5);
    table.insert(1006, 6);
    table.insert(1007, 7);
    table.insert(1008, 8);
    table.insert(1009, 9);

    auto it7 = table.find(1007);
    auto it11 = table.find(1001);

    auto stable = AVLTable<std::string, std::string>();

    stable.insert("123", "123");
    stable.insert("1234", "1234");
    stable.insert("12345", "12345");
    stable.insert("123456", "123456");
    stable.insert("1234567", "1234567");

    stable.remove("123456");

    auto b = it7 == it11;
    auto b1 = it7 == it7;
    auto b2 = it7 == table.find(1111);
    auto b3 = table.find(1111) == table.find(1111);

    auto& u = stable["123"];
    stable["123"] = "0000";
}

void main()
{
    //testMarket();

    //testSortTable();

    //testHashTable();

    testAVLTable();
}
