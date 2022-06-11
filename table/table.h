#pragma once
#include <vector>
#include <chrono>
#include <algorithm>

template<typename KeyType, typename ValueType>
class BaseTable;

template<typename KeyType, typename ValueType>
class SimpleTable;

template<typename KeyType, typename ValueType>
class SortTable;

template<typename KeyType, typename ValueType>
class HashTable;

template<typename KeyType, typename ValueType>
class OwnIterator : public std::iterator<std::input_iterator_tag, ValueType>
{
    friend class BaseTable<KeyType, ValueType>;
    friend class SimpleTable<KeyType, ValueType>;
    friend class SortTable<KeyType, ValueType>;
    friend class HashTable<KeyType, ValueType>;
private:
    OwnIterator() {}
    OwnIterator(std::pair<KeyType, ValueType>* ptr) : p(ptr) {}
public:
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
        OwnIterator<KeyType, ValueType> iter = find(key);
        if (iter != end())
            remove(iter);
    }
    virtual void remove(OwnIterator<KeyType, ValueType>& it) override
    {
        keyData.erase(std::remove(keyData.begin(), keyData.end(), *it.getPtr()));
    }
    virtual ValueType& operator[](const KeyType& key) override
    {
        return *(find(key));
    }
    void print()
    {
        for (int i = 0; i < keyData.size(); i++)
        {
            std::cout << keyData[i].first << "-" << keyData[i].second << std::endl;
        }
    }
};

template<typename KeyType, typename ValueType>
class SortTable : public SimpleTable<KeyType, ValueType>
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
        if (this->keyData.size() == 0)
            return OwnIterator<KeyType, ValueType>();

        size_t l = 0;
        size_t r = keyData.size();

        while (l < r)
        {
            size_t m = (r - l) / 2 + l;
            if (keyData[m].first == key)
                return OwnIterator<KeyType, ValueType>(&keyData[0] + m);
            else if (key < keyData[m].first)
                r = m;
            else
                l = m + 1;
        }
        return end();
    }
    OwnIterator<KeyType, ValueType> insert(const KeyType& key, const ValueType& value) override
    {
        size_t l = 0;
        size_t r = keyData.size();
        std::pair<KeyType, ValueType> el = std::make_pair(key, value);

        while (l < r)
        {
            size_t m = (r - l) / 2 + l;
            if (key < keyData[m].first)
                r = m;
            else
                l = m + 1;
        }

        if (l == keyData.size())
        {
            keyData.push_back(el);
            return OwnIterator<KeyType, ValueType>(&keyData.back() - 1);
        }

        auto iter = keyData.begin();
        keyData.insert(iter + l, el);
        return OwnIterator<KeyType, ValueType>(&keyData[l] - 1);
    }
    virtual void remove(const KeyType& key) override
    {
        OwnIterator<KeyType, ValueType> iter = find(key);
        keyData.erase(std::remove(keyData.begin(), keyData.end(), *iter.getPtr()));
    }
    virtual ValueType& operator[](const KeyType& key) override
    {
        return *(find(key));
    }

    void print()
    {
        for (int i = 0; i < keyData.size(); i++)
        {
            std::cout << keyData[i].first << "-" << keyData[i].second << std::endl;
        }
    }
};

template<typename KeyType, typename ValueType>
class HashTable
{
    std::hash<KeyType> hasher;
    std::vector<SortTable<KeyType, ValueType>> nodes;

    size_t getPos(KeyType const& key)
    {
        return  hasher(key) % nodes.size();
    }


public:
    template<class KeyType, class ValueType>
    class HashTableIterator : public std::iterator<std::input_iterator_tag, ValueType>
    {
        friend class HashTable<KeyType, ValueType>;

    public:

        HashTableIterator(std::vector<SortTable<KeyType, ValueType>>& nodes, size_t pos, OwnIterator<KeyType, ValueType> it)
            : nodes(nodes), pos(pos), itPair(it) { }

        std::pair<KeyType, ValueType>* getPtr()
        {
            return itPair->getPtr();
        }

        bool operator==(HashTableIterator const& other) const
        {
            return itPair == other.itPair;
        }

        bool operator!=(HashTableIterator const& other) const
        {
            return itPair != other.itPair;
        }

        typename HashTableIterator::reference operator*() const
        {
            return *itPair;
        }

        virtual HashTableIterator& operator++()
        {
            if (itPair == nodes[pos].end())
                return *this;

            OwnIterator<KeyType, ValueType> tmpIter = itPair;
            size_t tmpPos = pos;

            if (++itPair != nodes[pos].end())
                return *this;

            for (size_t i = tmpPos + 1; i < nodes.size(); i++)
                if (nodes[i].begin() != nodes[i].end())
                {
                    tmpPos = i;
                    tmpIter = nodes[i].begin();
                    break;
                }

            pos = tmpPos;
            itPair = tmpIter;
            return *this;
        }

        virtual HashTableIterator& operator+(int ind)
        {
            for (int i = 0; i < ind; i++)
                operator++();
            return *this;
        }

    private:
        std::vector<SortTable<KeyType, ValueType>>& nodes;
        size_t pos;
        OwnIterator<KeyType, ValueType> itPair;
    };

    HashTable(int num = 5) : nodes(num) { }
    HashTableIterator<KeyType, ValueType> begin()
    {
        for (size_t i = 0; i < nodes.end(); i++)
            if (nodes[i].begin() != nodes[i].end())
                return HashTableIterator<KeyType, ValueType>(nodes, i, nodes[i].begin());
        return HashTableIterator<KeyType, ValueType>(nodes, nodes.size() - 1, nodes.back().end());
    }
    HashTableIterator<KeyType, ValueType> end()
    {
        for (int i = nodes.size() - 1; i >= 0; i--)
            if (nodes[i].begin() != nodes[i].end())
                return HashTableIterator<KeyType, ValueType>(nodes, i, nodes[i].end());
        return HashTableIterator<KeyType, ValueType>(nodes, nodes.size() - 1, nodes.back().end());
    }
    HashTableIterator<KeyType, ValueType> find(const KeyType& key)
    {
        size_t pos = getPos(key);
        OwnIterator<KeyType, ValueType> itPair = nodes[pos].find(key);
        return HashTableIterator<KeyType, ValueType>(nodes, pos, itPair);
        return end();
    }

    HashTableIterator<KeyType, ValueType> insert(const KeyType& key, const ValueType& value)
    {
        size_t pos = getPos(key);
        OwnIterator<KeyType, ValueType> itPair = nodes[pos].insert(key, value);
        return HashTableIterator<KeyType, ValueType>(nodes, pos, itPair);
    }

    void remove(const KeyType& key)
    {
        size_t pos = getPos(key);
        nodes[pos].remove(key);
    }

    virtual ValueType& operator[](const KeyType& key)
    {
        return *(find(key));
    }

    void print()
    {
        for (int i = 0; i < nodes.size(); i++)
        {
            nodes[i].print();
        }
    }

};