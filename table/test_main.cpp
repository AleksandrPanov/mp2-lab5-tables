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
void myTests()
{
    SortTable<int, int> tbl;
    tbl.insert(10, 15);
    tbl.insert(6, 64);
    tbl.insert(50, 5);
    tbl.insert(7, 96);
    tbl.insert(0, 1);
    cout << endl;
    tbl.print();
    cout << endl;
    cout << "Data with min key: " << *(tbl.getMin()) << endl;
    cout << "Data with max key: " << *(tbl.getMax()) << endl;
    cout << "Find elem (7,96): key = 7, data = " << *(tbl.find(7)) << endl;
    cout << "Delete elem (7,96)." << endl;
    tbl.remove(7);
    tbl.print();

    HashTable<int, int> tbl2;
    tbl2.insert(10, 15);
    tbl2.insert(6, 64);
    tbl2.insert(50, 5);
    tbl2.insert(7, 96);
    tbl2.insert(0, 1);
    cout << endl;
    cout << endl;
    tbl2.print();
    cout << endl;
    cout << "Data with min key: " << "("<< tbl2.getMin()<<","<<tbl2.getData(tbl2.getMin())<<")" << endl;
    cout << "Data with max key: " << "(" << tbl2.getMax() << "," << tbl2.getData(tbl2.getMax()) << ")" << endl;
    cout << "Data with key = 50: " << tbl2.getData(50) << endl;
    cout << "Found Elem (7,96)? " << tbl2.find(7) << endl;
    cout << "Delete elem (7,96)." << endl;
    tbl2.remove(7);
    tbl2.print();
    std::pair<int, int> p = tbl2.begin();
    cout << endl << p.first<<" "<<p.second;
    cout << endl;

    AVLTable<int, float> tbl3;
    //tbl3.insert(4, 0.4);
    //tbl3.insert(5, 0.5);
    //cout<<endl<<"Uspeh? "<<tbl3.insert(7, 0.7);
    //tbl3.insert(2, 0.2);
    //tbl3.insert(1, 0.1);
    //tbl3.insert(3, 0.3);
    //tbl3.insert(6, 0.6);
    for (int i = 0; i < 12; i++)
    {
        int a;
        cin >> a;
        tbl3.insert(a, 0.0);
    }
    tbl3.insert(78, 0.0);

}

void main()
{
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

    myTests();
}
