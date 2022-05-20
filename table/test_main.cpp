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
    int64_t t = Order::getNowTime();
    cout << t << endl;
    struct tm* tm = localtime(&t);
    char date[20];
    strftime(date, sizeof(date), "%Y-%m-%d", tm);
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
    StockMarker market;
    Request req;
    Order order;
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

void simple_test()
{
	StockMarker burse;
	for (int i = 0; i < 50; ++i)
	{
		burse.addRequestSafe(NewUserRequest(115 * i));
	}
	for (int i = 0; i < 50; ++i)
	{
		burse.addRequestSafe(Order(Order::OrderType::BuyRequest, 250 - rand() % 100, 0, 115 * i));
	}
	for (int i = 0; i < 50; ++i)
	{
		burse.addRequestSafe(Order(Order::OrderType::SaleRequest, 200 - rand() % 100, 0, 115 * i));
	}
	for (int i = 0; i < 150; ++i)
	{
		burse.processFirstRequestSafe();
		if (i == 99)
		{
			std::cout << ' ';
		}
	}
	getchar();
}

int  main()
{
	simple_test();
    return 0;
}

