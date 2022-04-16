#pragma once
#include <vector>
#include <chrono>
#include <algorithm>
namespace {

template<typename KeyType, typename ValueType>
class BaseTable;

template<typename KeyType, typename ValueType>
class SimpleTable;

template<typename KeyType, typename ValueType>
class SortTable;

template<typename KeyType, typename ValueType>
class HashTable;

// https://habr.com/ru/post/265491/
template<typename KeyType, typename ValueType>
class OwnIterator: public std::iterator<std::input_iterator_tag, ValueType>
{
    friend class BaseTable<KeyType, ValueType>;
    friend class SimpleTable<KeyType, ValueType>;
    friend class SortTable<KeyType, ValueType>;
    friend class HashTable<KeyType, ValueType>;
private:
    OwnIterator() {}
    OwnIterator(std::pair<KeyType, ValueType>* ptr): p(ptr) {}
public:
    //OwnIterator(const OwnIterator& it) {}
    std::pair<KeyType, ValueType>* getPtr()
    {
        return p;
    }

    bool operator==(OwnIterator const& other) const
    {
        if (p == nullptr || other.p == nullptr) return p == other.p;
        return p->first == other.p->first;
    }
    bool operator!=(OwnIterator const& other) const
    {
        if (p == nullptr || other.p == nullptr) return p != other.p;
        return p->first != other.p->first;
    }
    typename OwnIterator::reference operator*() const
    {
        return p->second;
    }
    virtual OwnIterator& operator++()
    {
        ++p;
        return *this;
    }
    virtual OwnIterator& operator+(int index)
    {
        p += index;
        return *this;
    }
private:
    std::pair<KeyType, ValueType>* p = nullptr;
};

template<typename KeyType, typename ValueType>
class BaseTable
{
public:
    BaseTable() {}
    virtual OwnIterator<KeyType, ValueType> begin()
    {
        return OwnIterator<KeyType, ValueType>();
    }
    virtual OwnIterator<KeyType, ValueType> end()
    {
        return OwnIterator<KeyType, ValueType>();
    }
    // find возвращает указатель на данные
    // если данные не найдены, то возвращается указатель равный end()
    // end() = адрес последнего элемента + 1
    virtual OwnIterator<KeyType, ValueType> find(const KeyType& key) = 0;
    virtual OwnIterator<KeyType, ValueType> insert(const KeyType& key, const ValueType& value) = 0;
    virtual void remove(const KeyType& key) = 0;
    virtual void remove(OwnIterator<KeyType, ValueType>& it) {}

    virtual ValueType& operator[](const KeyType& key) = 0;
    virtual size_t getSize()
    {
        return end().getPtr() - begin().getPtr();
    }

    virtual OwnIterator<KeyType, ValueType> getMin()
    { 
        if (getSize() == 0)
            return end();

        OwnIterator<KeyType, ValueType> minIt = begin();
        for (OwnIterator<KeyType, ValueType> it = ++begin(); it != end(); ++it)
            if (minIt.p->first > it.p->first)
                minIt = it;

        return minIt;
    }
    virtual OwnIterator<KeyType, ValueType> getMax()
    { 
        if (getSize() == 0)
            return end();

        OwnIterator<KeyType, ValueType> maxIt = begin();
        for (OwnIterator<KeyType, ValueType> it = ++begin(); it != end(); ++it)
            if (maxIt.p->first < it.p->first)
                maxIt = it;

        return maxIt;
    }
};

template<typename KeyType, typename ValueType>
class SimpleTable : public BaseTable<KeyType, ValueType>
{
    std::vector<std::pair<KeyType, ValueType> > keyData;
public:
    virtual OwnIterator<KeyType, ValueType> begin() override
    {
        if (keyData.size() == 0ull)
            return OwnIterator<KeyType, ValueType>();
        return &(keyData.front());
    }
    virtual OwnIterator<KeyType, ValueType> end() override
    {
        if (keyData.size() == 0ull)
            return OwnIterator<KeyType, ValueType>();
        return &(keyData.back()) + 1ull;
    }
    OwnIterator<KeyType, ValueType> find(const KeyType& key) override
    {
        for (size_t i = 0; i < keyData.size(); i++)
        {
            if (keyData[i].first == key)
                return OwnIterator<KeyType, ValueType>(&keyData[0] + i);
        }
        return end();
    }
    OwnIterator<KeyType, ValueType> insert(const KeyType& key, const ValueType& value) override
    {
        keyData.push_back(std::make_pair(key, value));
        return OwnIterator<KeyType, ValueType>(&(keyData.back()));
    }
    virtual void remove(const KeyType& key)
    {

    }
    virtual void remove(OwnIterator<KeyType, ValueType>& it) override
    {
        keyData.erase(std::remove(keyData.begin(), keyData.end(), *it.getPtr()));
    }
    virtual ValueType& operator[](const KeyType& key) override
    {
        return *(find(key));
    }
};

template<typename KeyType, typename ValueType>
class SortTable : public SimpleTable<KeyType, ValueType>
{
    std::vector<std::pair<KeyType, ValueType> > keyData;
public:
    OwnIterator<KeyType, ValueType> find(const KeyType& key) override
    {
        // реализовать бинарный поиск
        return OwnIterator<KeyType, ValueType>(&keyData[0] + keyData.size());
    }
    OwnIterator<KeyType, ValueType> insert(const KeyType& key, const ValueType& value) override
    {
        keyData.push_back(std::make_pair(key, value));
        return OwnIterator<KeyType, ValueType>(&keyData.back());
    }
    virtual void remove(const KeyType& key) override
    {

    }
    virtual ValueType& operator[](const KeyType& key) override
    {
        return *(find(key));
    }
};

template<typename KeyType, typename ValueType>
class HashTable : public BaseTable<KeyType, ValueType>
{
public:
    virtual OwnIterator<KeyType, ValueType>& find(const KeyType& key) override
    {

    }
    virtual OwnIterator<KeyType, ValueType>& add(const KeyType& key, const ValueType& value) override
    {

    }
    virtual void remove(const KeyType& key) override
    {

    }
    virtual ValueType& operator[](const KeyType& key) override
    {

    }
};

}
