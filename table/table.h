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
    {
        p = it.p;
    }
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
    virtual void remove(const KeyType& key) override
    {
        auto it = find(key);
        if (it != end())
            remove(it);
    }
    virtual void remove(OwnIterator<KeyType, ValueType>& it) override
    {
        keyData.erase(std::remove(keyData.begin(), keyData.end(), *it.getPtr()));
    }
    virtual ValueType& operator[](const KeyType& key) override
    {
        return *(find(key));
    }

protected:
    std::vector<std::pair<KeyType, ValueType>> keyData;
};


template<typename KeyType, typename ValueType>
class SortTable : public SimpleTable<KeyType, ValueType>
{
public:
    OwnIterator<KeyType, ValueType> find(const KeyType& key) override
    {
        if (this->keyData.size() == 0) 
            return OwnIterator<KeyType, ValueType>();
        return OwnIterator<KeyType, ValueType>(&this->keyData[0] + binarySearch(key));
    }
    OwnIterator<KeyType, ValueType> insert(const KeyType& key, const ValueType& value) override
    {
        size_t i = findSuitablePosition(key);
        this->keyData.insert(this->keyData.begin() + i, std::make_pair(key, value));
        return OwnIterator<KeyType, ValueType>(&this->keyData.at(i));
    }
private:
    size_t binarySearch(const KeyType& key)
    {
        size_t lower_i = 0;
        size_t upper_i = this->keyData.size();
        while (lower_i != upper_i)
        {
            size_t i = (upper_i + lower_i) / 2;
            if (this->keyData[i].first == key)
                return i;
            if (this->keyData[i].first > key)
                upper_i = i;
            else if (lower_i != i)
                lower_i = i;
            else
                break;
        }
        return this->keyData.size();
    }
    size_t findSuitablePosition(const KeyType& key)
    {
        size_t lower_i = 0;
        size_t upper_i = this->keyData.size();
        size_t i = 0;
        while (lower_i != upper_i)
        {
            i = (upper_i + lower_i) / 2;
            if (this->keyData[i].first == key) {
                while (i != this->keyData.size() && this->keyData[i].first == key)
                    i++;
                break;
            }
            if (this->keyData[i].first > key)
                upper_i = i;
            else if (lower_i != i)
                lower_i = i;
            else {
                i++;
                break;
            }
        }
        return i;
    }
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
        {
            return itPair_;
        }

    public:
        std::pair<KeyType, ValueType>* getPtr()
        {
            return itPair_->getPtr();
        }

        bool operator==(HashTableIterator const& other) const
        {
            return itPair_ == other.itPair_;
        }

        bool operator!=(HashTableIterator const& other) const
        {
            return itPair_ != other.itPair_;
        }

        typename HashTableIterator::reference operator*() const
        {
            return *itPair_;
        }

        virtual HashTableIterator& operator++()
        {
            if (itPair_ == itBucket_->end())
                return *this;

            if (++itPair_ != itBucket_->end())
                return *this;

            for (auto it = itBucket_ + 1; it != buckets_.end(); ++it)
                if (it->begin() != it->end()) {
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
    {
        return *find(key);
    }

private:
    std::hash<KeyType> hasher_;
    std::vector<BucketType> buckets_;

    typename std::vector<BucketType>::iterator getBucket(KeyType const& key)
    {
        std::size_t bucketIndex = hasher_(key) % buckets_.size();
        return buckets_.begin() + bucketIndex;
    }
};

template<typename KeyType, typename ValueType>
class AvlTable
{
    struct Node 
    {
        Node(std::pair<KeyType, ValueType> data, Node* parent) 
            : data(data), parent(parent)
        { }

        ~Node()
        {
            delete left;
            delete right;
        }

        std::pair<KeyType, ValueType> data;
        int balance = 0;
        Node* left = nullptr;
        Node* right = nullptr;
        Node* parent;
    };

public:

    ~AvlTable()
    {
        delete root_;
    }

    void insert(const KeyType& key, const KeyType& value)
    {
        if (root_ == nullptr)
        {
            root_ = new Node(std::make_pair(key, value), nullptr);
            return;
        }

        Node* node = root_;
        Node* parent;

        while (true)
        {
            if (node->data.first == key)
                return;

            parent = node;

            bool goLeft = node->data.first > key;
            node = goLeft ? node->left : node->right;

            if (node == nullptr)
            {
                if (goLeft)
                    parent->left = new Node(std::make_pair(key, value), parent);
                else
                    parent->right = new Node(std::make_pair(key, value), parent);

                rebalance(parent);
                break;
            }
        }
    }

    std::pair<KeyType, ValueType>* find(const KeyType& key)
    {
        if (root_ == nullptr)
            return nullptr;

        Node* node = root_;
        Node* child = root_;

        while (child != nullptr)
        {
            node = child;
            child = key >= node->data.first ? node->right : node->left;
            if (key == node->data.first)
                return &(node->data);
        }
        return nullptr;
    }

    void remove(const KeyType& key)
    {
        if (root_ == nullptr)
            return;

        Node* node = root_;
        Node* parent = root_;
        Node* child = root_;
        Node* target = nullptr;

        while (child != nullptr)
        {
            parent = node;
            node = child;
            child = key >= node->data.first ? node->right : node->left;
            if (key == node->data.first)
                target = node;
        }

        if (target != nullptr)
        {
            target->data = node->data;

            child = node->left != nullptr ? node->left : node->right;

            if (root_->data.first == key)
            {
                root_ = child;
            }
            else
            {
                if (parent->left == node)
                    parent->left = child;
                else
                    parent->right = child;
                rebalance(parent);
            }
        }
    }

    ValueType& operator[](const KeyType& key)
    {
        return find(key)->second;
    }

private:
    Node* root_ = nullptr;

    void rebalance(Node* node) 
    {
        updateBalance(node);

        if (node->balance == -2) 
        {
            if (computeHeight(node->left->left) >= computeHeight(node->left->right))
                node = rotateRight(node);
            else
                node = rotateLeftRight(node);
        }
        else if (node->balance == 2) 
        {
            if (computeHeight(node->right->right) >= computeHeight(node->right->left))
                node = rotateLeft(node);
            else
                node = rotateRightLeft(node);
        }

        if (node->parent != nullptr) 
            rebalance(node->parent);
        else 
            root_ = node;
    }

    int computeHeight(Node* node)
    {
        if (node == nullptr)
            return -1;
        return 1 + std::max(computeHeight(node->left), computeHeight(node->right));
    }

    void updateBalance(Node* node)
    {
        node->balance = computeHeight(node->right) - computeHeight(node->left);
    }

    Node* rotateLeft(Node* node) 
    {
        Node* rightNode = node->right;
        rightNode->parent = node->parent;
        node->right = rightNode->left;

        if (node->right != nullptr)
            node->right->parent = node;

        rightNode->left = node;
        node->parent = rightNode;

        if (rightNode->parent != nullptr) 
        {
            if (rightNode->parent->right == node) 
                rightNode->parent->right = rightNode;
            else 
                rightNode->parent->left = rightNode;
        }

        updateBalance(node);
        updateBalance(rightNode);
        return rightNode;
    }

    Node* rotateRight(Node* node) 
    {
        Node* leftNode = node->left;
        leftNode->parent = node->parent;
        node->left = leftNode->right;

        if (node->left != nullptr)
            node->left->parent = node;

        leftNode->right = node;
        node->parent = leftNode;

        if (leftNode->parent != nullptr) 
        {
            if (leftNode->parent->right == node) 
                leftNode->parent->right = leftNode;
            else 
                leftNode->parent->left = leftNode;
        }

        updateBalance(node);
        updateBalance(leftNode);
        return leftNode;
    }

    Node* rotateLeftRight(Node* node) 
    {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    Node* rotateRightLeft(Node* node)
    {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
};

}
