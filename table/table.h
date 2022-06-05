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
    OwnIterator(const OwnIterator& it)
        p = it.p;
    std::pair<KeyType, ValueType>* getPtr()
    {
        return p;
    }

    bool operator==(OwnIterator const& other) const
    {
        return p == other.p;
    }
    bool operator!=(OwnIterator const& other) const
    {
        return p != other.p;
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
        auto removeIt = std::remove_if(keyData.begin(), keyData.end(), [&](const std::pair<KeyType, ValueType>& pair)
            {
                return key == pair.first;
            });
        keyData.erase(removeIt, keyData.end());
    }
    virtual void remove(OwnIterator<KeyType, ValueType>& it) override
        keyData.erase(std::remove(keyData.begin(), keyData.end(), *it.getPtr()), keyData.end());
    virtual ValueType& operator[](const KeyType& key) override
        return *(find(key));
};

template<typename KeyType, typename ValueType>
class SortTable : public SimpleTable<KeyType, ValueType>
{
    std::vector<std::pair<KeyType, ValueType> > keyData;
public:
    OwnIterator<KeyType, ValueType> begin() override
    {
        if (keyData.size() == 0ull)
            return OwnIterator<KeyType, ValueType>();
        return &(keyData.front());
    }
    OwnIterator<KeyType, ValueType> end() override
    {
        if (keyData.size() == 0ull)
            return OwnIterator<KeyType, ValueType>();
        return &(keyData.back()) + 1ull;
    }
    OwnIterator<KeyType, ValueType> find(const KeyType& key) override
    {
        size_t left = 0;
        size_t right = keyData.size();
        while (left < right)
        {
            size_t median = (right - left) / 2 + left;
            if (keyData[median].first == key)
                return OwnIterator<KeyType, ValueType>(&keyData[0] + median);
            else if (key < keyData[median].first)
                right = median;
            else
                left = median + 1;
        }
        return end();
    }
    OwnIterator<KeyType, ValueType> insert(const KeyType& key, const ValueType& value) override
    {
        size_t left = 0;
        size_t right = keyData.size();
        size_t median = 0;
        while (left < right)
        {
            median = (right - left) / 2 + left;
            if (keyData[median].first == key)
                throw std::runtime_error("Pair already exist.");
            else if (key < keyData[median].first)
                right = median;
            else
                left = median + 1;
        }

        auto pair = std::make_pair(key, value);

        if (left == keyData.size())
        {
            keyData.push_back(std::move(pair));
            return OwnIterator<KeyType, ValueType>(&keyData.back() - 1);
        }
        else
        {
            auto it = keyData.begin();
            std::advance(it, left);
            keyData.insert(it, std::move(pair));
            return OwnIterator<KeyType, ValueType>(&keyData[left] - 1);
        }
    }
    virtual void remove(const KeyType& key) override
    {
        auto it = find(key);
        if (it == end())
            throw std::runtime_error("Don`t exist");
        else
        {
            auto begin = keyData.begin();
            std::advance(begin, it.getPtr() - &keyData[0]);
            keyData.erase(begin);
        }

    }
    virtual ValueType& operator[](const KeyType& key) override{
        return *(find(key));
};

template<typename KeyType, typename ValueType>
class HashTable
{
    using BucketType = SortTable<KeyType, ValueType>;
    static constexpr size_t defaultBucketsNumber = 50;

public:

    template<class KeyType, class ValueType>
    class HashTableIterator : public std::iterator<std::input_iterator_tag, ValueType>
    {
        friend class HashTable<KeyType, ValueType>;

        HashTableIterator(std::vector<BucketType>& buckets, typename std::vector<BucketType>::iterator itBucket, OwnIterator<KeyType, ValueType> it)
            : buckets_(buckets), itBucket_(itBucket), itPair_(it)
        { }

        OwnIterator<KeyType, ValueType>& getPairIterator()
            return itPair_;

    public:
        std::pair<KeyType, ValueType>* getPtr()
            return itPair_->getPtr();

        bool operator==(HashTableIterator const& other) const
            return itPair_ == other.itPair_;

        bool operator!=(HashTableIterator const& other) const
            return itPair_ != other.itPair_;

        typename HashTableIterator::reference operator*() const
            return *itPair_;

        virtual HashTableIterator& operator++()
        {
            if (itPair_ == itBucket_->end())
                return *this;

            if (++itPair_ != itBucket_->end())
                return *this;

            for (auto it = itBucket_ + 1; it != buckets_.end(); ++it)
                if (it->begin() != it->end()) 
                {
                    itBucket_ = it;
                    itPair_ = it->begin();
                    break;
                }

            return *this;
        }

        virtual HashTableIterator& operator+(int index)
        {
            for (int i = 0; i < index; i++)
                operator++();
            return *this;
        }

    private:
        std::vector<BucketType>& buckets_;
        typename std::vector<BucketType>::iterator itBucket_;
        OwnIterator<KeyType, ValueType> itPair_;
    };

    HashTable(size_t numBuckets = defaultBucketsNumber)
        : buckets_(numBuckets)
    { }

    HashTableIterator<KeyType, ValueType> begin()
    {
        for (auto itBucket = buckets_.begin(); itBucket != buckets_.end(); itBucket++)
            if (itBucket->begin() != itBucket->end())
                return HashTableIterator<KeyType, ValueType>(buckets_, itBucket, itBucket->begin());
        return HashTableIterator<KeyType, ValueType>(buckets_, buckets_.begin(), buckets_.begin()->begin());
    }

    HashTableIterator<KeyType, ValueType> end()
    {
        auto end = buckets_.begin();
        for (auto itBucket = buckets_.begin(); itBucket != buckets_.end(); itBucket++)
            if (itBucket->begin() != itBucket->end())
                end = itBucket;
        return HashTableIterator<KeyType, ValueType>(buckets_, end, end->end());
    }

    HashTableIterator<KeyType, ValueType> find(const KeyType& key)
    {
        auto itBucket = getBucket(key);
        auto itPair = itBucket->find(key);
        if (itPair == itBucket->end())
            return end();
        return HashTableIterator<KeyType, ValueType>(buckets_, itBucket, itPair);
    }

    HashTableIterator<KeyType, ValueType> insert(const KeyType& key, const ValueType& value)
    {
        auto itBucket = getBucket(key);
        auto itPair = itBucket->insert(key, value);
        return HashTableIterator<KeyType, ValueType>(buckets_, itBucket, itPair);
    }

    void remove(const KeyType& key)
    {
        auto itBucket = getBucket(key);
        itBucket->remove(key);
    }

    void remove(HashTableIterator<KeyType, ValueType>& it)
    {
        auto itBucket = getBucket(it.getPtr()->first());
        auto itPair = it.getPairIterator();
        *itBucket->remove(itPair);
    }

    ValueType& operator[](const KeyType& key)
        return *find(key);

private:
    std::hash<KeyType> hasher_;
    std::vector<BucketType> buckets_;

    typename std::vector<BucketType>::iterator getBucket(KeyType const& key)
    {
        std::size_t bucketIndex = hasher_(key) % buckets_.size();
        return buckets_.begin() + bucketIndex;
    }
};

}
