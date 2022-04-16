#pragma once
#include <queue>
#include <memory>
#include <mutex>
#include <chrono>

#include "table.h"

#define logl(message) std::cout << message << "\n"

static uint64_t getNowTime()
{
    const auto time = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(
        time.time_since_epoch()
    ).count();
}

class Request
{
public:
    enum class Type
    {
        NONE,
        ORDER,
        CANCEL_ORDER,
        REGISTRATION
    };

    Type type;

protected:
    Request(Type type) :
        type(type)
    {    }
};

class Registration : public Request
{
public:
    Registration(int64_t userId) : 
        Request(Request::Type::REGISTRATION),
        userId(userId)
    {   }

    int64_t userId;
};

class Order : public Request
{
public:
    enum class Type
    {
        BUY,
        SELL
    };

    Type orderType;
    int64_t price;
    int64_t time;
    int64_t userId;

    Order(Type type, int64_t price, int64_t userId) :
        Request(Request::Type::ORDER),
        orderType(type),
        price(price),
        time(getNowTime()),
        userId(userId)
    {   }

    uint64_t getHashId() // TODO: improve
    {
        return userId + time + price;
    }

    friend bool operator<(const Order& a1, const Order& a2)
    {
        return a1.price < a2.price;
    }

    friend bool operator==(const Order& a1, const Order& a2)
    {
        return a1.price == a2.price && a1.time == a2.time && a1.userId == a2.userId;
    }
};

class CancelOrder : public Request
{
public:
    Order order;

    CancelOrder(const Order& order) :
        Request(Request::Type::CANCEL_ORDER),
        order(order)
    {   }
};

class StockMarket
{
    std::queue<std::unique_ptr<Request>> requestQueue;

    SimpleTable<int64_t, Order> orderBuyTable;                  // price -> order
    SimpleTable<int64_t, Order> orderSellTable;                 // price -> order
    SimpleTable<int64_t, std::vector<Order> > userTable;        // user id -> orders

    std::mutex queueMutex;

public:
    template<class T>
    void addRequest(const T& request)
    {
        logl("Request added to queue");
        requestQueue.push(std::unique_ptr<Request>(new T(request)));
    }

    template<class T>
    void addRequestSafe(const T& request)
    {
        logl("Request added safe to queue");
        std::lock_guard<std::mutex> lock(queueMutex);
        requestQueue.push(std::unique_ptr<Request>(new T(request)));
    }

    void processRequest(Request& request)
    {
        if (request.type == Request::Type::ORDER)
        {
            Order& r = static_cast<Order&>(request);
            if (r.orderType == Order::Type::BUY)
            {
                logl("Order(Buy) processed");
                auto minIt = orderSellTable.getMin();

                if (minIt == orderSellTable.end() || (*minIt).price > r.price)
                {
                    orderBuyTable.insert(r.price, r);
                    userTable[r.userId].push_back(r);
                    return;
                }

                logl("Operation success");
                orderSellTable.remove(minIt);
            }
            else if (r.orderType == Order::Type::SELL)
            {
                logl("Order(Sell) processed");
                auto maxIt = orderBuyTable.getMax();

                if (maxIt == orderSellTable.end() || (*maxIt).price < r.price)
                {
                    orderSellTable.insert(r.price, r);
                    userTable[r.userId].push_back(r);
                    return;
                }

                logl("Operation success");
                orderBuyTable.remove(maxIt);
            }
        }
        else if (request.type == Request::Type::REGISTRATION)
        {
            logl("Registration processed");
            auto& reg = static_cast<Registration&>(request);
            if (userTable.find(reg.userId) != userTable.end())
                throw std::runtime_error("User already exist");

            userTable.insert(reg.userId, std::vector<Order>());
        }
        else if (request.type == Request::Type::CANCEL_ORDER)
        {
            logl("Cancel processed");
            auto& cancel = static_cast<CancelOrder&>(request);
            if (cancel.order.orderType == Order::Type::BUY)
                tryCancelFromTable(cancel, orderBuyTable);
            else if (cancel.order.orderType == Order::Type::SELL)
                tryCancelFromTable(cancel, orderSellTable);
        }
    }

    bool processFirstRequest()
    {
        Request& req = *requestQueue.front();
        if (requestQueue.empty())
            return false;
        processRequest(req);
        requestQueue.pop();
        return true;
    }

    bool processFirstRequestSafe()
    {
        const std::lock_guard<std::mutex> lock(queueMutex);
        if (requestQueue.empty())
            return false;
        Request& req = *requestQueue.front();
        processRequest(req);
        requestQueue.pop();
        return true;
    }

private:
    void tryCancelFromTable(CancelOrder& cancel, SimpleTable<int64_t, Order>& table)
    {
        auto it = table.find(cancel.order.price);
        if (it == table.end())
            throw std::runtime_error("No active order with that price");

        table.remove(it);
        logl("Order canceled success");
    }
};
