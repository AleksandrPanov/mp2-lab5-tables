#pragma once
#include <queue>
#include <memory>
#include <mutex>
#include <chrono>
#include <utility>
#include <algorithm>
#include "table.h"

using namespace my_tables;

class Request
{
public:
    enum class RequestType
    {
        None,
        Order,
        Registration // new user registration request
    };
    RequestType type = RequestType::None;
    //virtual ~Request(){}
};

class NewUserRequest : public Request
{
	int64_t userId = 0;
public:
	NewUserRequest(int64_t userId) : userId(userId)
	{
		type = RequestType::Registration;
	}
	int64_t get_userId()
	{
		return userId;
	}
};

class Order : public Request
{
	int64_t price = 0;
	int64_t time = 0;
	int64_t userId = 0;
public:
    enum class OrderType
    {
        BuyRequest,
        SaleRequest,
        CancelBuyRequest,
        CancelSellRequest
    };
    OrderType orderType;
    Order()
    {
        type = RequestType::Order;
    }
	Order(OrderType type, int64_t price, int64_t time, int64_t userId) : orderType(type), price(price), time(time), userId(userId) {}
	Order(const Order &order)
	{
		type = RequestType::Order;
		orderType = order.orderType;
		price = order.price;
		time = order.time;
		userId = order.userId;
	}
	int64_t getUserId()
	{
		return userId;
	}
    static uint64_t getNowTime()
    {
        const auto time = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(
            time.time_since_epoch()).count();
    }

	int64_t get_price()
	{
		return price;
	}

    friend bool operator<(const Order& a1, const Order& a2)
    {
        return a1.price < a2.price;
    }

	friend bool operator ==(const Order& a1, const Order& a2)
	{
		return  a1.orderType == a2.orderType &&
			a1.price == a2.price &&
			a1.time == a2.time &&
			a1.userId == a2.userId;
	}
	friend bool operator !=(const Order& a1, const Order& a2)
	{
		return  !(a1 == a2);
	}
};

class StockMarker
{
    std::queue<std::unique_ptr<Request>> requestQueue;
    std::mutex mutex;
    SimpleTable<int64_t, Order> orderBuyTable;  // price -> order
    SimpleTable<int64_t, Order> orderSellTable; // price -> order
    SimpleTable<int64_t, std::vector<Order> > userTable;      // user id -> orders

public:
    template<typename T>
    void addRequest(const T& request)
    {
        requestQueue.push(std::unique_ptr<Request>(new T(request)));
    }
    template<typename T>
    void addRequestSafe(const T& request)
    {
        const std::lock_guard<std::mutex> lock(mutex);
		//std::unique_ptr<Request> uniqptr(new T(request);
		requestQueue.push(
				std::unique_ptr<Request>(new T(request))
		);
    }
    void processRequest(Request& req)
    {
        if (req.type == Request::RequestType::Order)
        {
            /*try {
                Order& r = dynamic_cast<Order&>(req);
            }
            catch (...)
            {

            }
            Order* r1 = dynamic_cast<Order*>(&req);*/
            Order& r = static_cast<Order&>(req);
            if (r.orderType == Order::OrderType::BuyRequest)
            {
                // check orderSellTable
                    // if price ??<=?? orderSellTable.min //>=
                        // succ operation
                    // push order to orderBuyTable
				auto min_price = orderSellTable.get_min_by_key().getPtr();
				if (min_price != nullptr && r.get_price() >= min_price->first)
				{
					//SUCCESS
					orderSellTable.remove(min_price->first);
					std::cout << "Success: " << r.get_price() << '\n';
				}
				else
				{
					auto iter = userTable.find(r.getUserId());
					if (iter != userTable.end())
					{
						iter.getPtr()->second.push_back(r);
						orderBuyTable.insert(r.get_price(), r);
					}
					else
					{
						std::cout << "No user with this id: " << r.getUserId() << std::endl;
					}
				}
            }
            else if (r.orderType == Order::OrderType::SaleRequest)
            {
				auto max_price = orderBuyTable.get_max_by_key().getPtr();
				if (max_price != nullptr && r.get_price() <= max_price->first)
				{
					//SUCCESS
					orderBuyTable.remove(max_price->first);
					std::cout << "Success: " << r.get_price() << '\n';
				}
				else
				{
					auto iter = userTable.find(r.getUserId());
					if (iter != userTable.end())
					{
						iter.getPtr()->second.push_back(r);
						orderSellTable.insert(r.get_price(), r);
					}
					else
					{
						std::cout << "No user with this id: " << r.getUserId() << std::endl;
					}
				}
            }
            else if (r.orderType == Order::OrderType::CancelBuyRequest)
            {
                // check orderBuyTable
                    // если такой запрос есть, его нужно удалить
				if (orderBuyTable.find(r.get_price()) != orderBuyTable.end())
				{
					orderBuyTable.remove(r.get_price());
					auto iter = userTable.find(r.getUserId());
					auto vec = &(iter.getPtr()->second);
					std::remove(vec->begin(), vec->end(), r);
				}
            }
			else if (r.orderType == Order::OrderType::CancelSellRequest)
			{
				if (orderSellTable.find(r.get_price()) != orderSellTable.end())
				{
					orderSellTable.remove(r.get_price());
					auto iter = userTable.find(r.getUserId());
					auto vec = &(iter.getPtr()->second);
					std::remove(vec->begin(), vec->end(), r);
				}
			}
        }
        else if (req.type == Request::RequestType::Registration)
        {
            NewUserRequest& r = static_cast<NewUserRequest&>(req);
			userTable.insert(r.get_userId(), std::vector<Order>());
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
        const std::lock_guard<std::mutex> lock(mutex);
        if (requestQueue.empty())
            return false;
        Request& req = *requestQueue.front();
        processRequest(req);
        requestQueue.pop();
        return true;
    }
};
