#include <iostream>
#include <string>
//#include <ctime>
//#include <thread>

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

int main()
{
    std::cout << "SimpleTable test" << std::endl;
    SimpleTable<int, int> simpleTable;
    simpleTable.insert(0, 5);
    simpleTable.insert(15, 12);
    simpleTable.insert(99, 8);
    simpleTable.insert(4, 2);
    simpleTable.print();
    cout << "Remove value " << *(simpleTable.find(15)) << endl;
    simpleTable.remove(15);
    simpleTable.print();
    cout << endl << endl;

    std::cout << "SortTable test" << std::endl;
    SortTable<int, int> sortTable;
    sortTable.insert(0, 5);
    sortTable.insert(15, 12);
    sortTable.insert(99, 8);
    sortTable.insert(4, 2);
    sortTable.print();
    cout << "Remove value " << *(sortTable.find(15)) << endl;
    sortTable.remove(15);
    sortTable.print();
    cout << endl << endl;

    std::cout << "HashTable test" << std::endl;
    HashTable<int, int> hashTable;
    hashTable.insert(0, 5);
    hashTable.insert(15, 12);
    hashTable.insert(99, 8);
    hashTable.insert(4, 2);
    hashTable.print();
    cout << "Remove value " << *(hashTable.find(15)) << endl;
    hashTable.remove(15);
    hashTable.print();
    cout << endl << endl;

    return 0;
}