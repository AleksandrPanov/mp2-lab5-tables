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
        //OwnIterator(const OwnIterator& it) {}
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

        virtual OwnIterator<KeyType, ValueType> getMin() { return end(); }
        virtual OwnIterator<KeyType, ValueType> getMax() { return end(); }
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
            return OwnIterator<KeyType, ValueType>(&keyData[0] + keyData.size());
        }
        OwnIterator<KeyType, ValueType> insert(const KeyType& key, const ValueType& value) override
        {
            keyData.push_back(std::make_pair(key, value));
            return OwnIterator<KeyType, ValueType>(&keyData.back());
        }
        virtual void remove(const KeyType& key) override
        {
            OwnIterator<KeyType, ValueType> iter = find(key);
            keyData.erase(std::remove(keyData.begin(), keyData.end(), *iter.getPtr()));

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
                if (key < keyData[median].first)
                    right = median;
                else
                    left = median + 1;
            }
            auto pair = std::make_pair(key, value);
            if (left == keyData.size())
            {
                keyData.push_back(pair);
                return OwnIterator<KeyType, ValueType>(&keyData.back() - 1);
            }
            auto it = keyData.begin();
            std::advance(it, left);
            keyData.insert(it, pair);
            return OwnIterator<KeyType, ValueType>(&keyData[left] - 1);
        }
        virtual void remove(const KeyType& key) override
        {
            OwnIterator<KeyType, ValueType> iter = find(key);
            auto begin = keyData.begin();
            std::advance(begin, iter.getPtr() - &keyData[0]);
            keyData.erase(begin);
        }
        virtual ValueType& operator[](const KeyType& key) override
        {
            return *(find(key));
        }
    };

    template<typename KeyType, typename ValueType>
    class HashTable
    {
    public:

        template<class KeyType, class ValueType>
        class HashTableIterator : public std::iterator<std::input_iterator_tag, ValueType>
        {
            friend class HashTable<KeyType, ValueType>;

            HashTableIterator(std::vector<SortTable<KeyType, ValueType>>& buckets, size_t pos, OwnIterator<KeyType, ValueType> it)
                : buckets(buckets), pos(pos), itPair(it)
            { }

        public:

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
                if (itPair == buckets[pos].end())
                    return *this;
                size_t tmpPos = pos;
                auto tmpIterator = ++itPair;
                if (tmpIterator == buckets[pos].end())
                {
                    for (size_t i = tmpPos + 1; i < buckets.size(); i++)
                        if (buckets[i].begin() != buckets[i].end()) {
                            tmpPos = i;
                            tmpIterator = buckets[i].begin();
                            break;
                        }
                }
                pos = tmpPos;
                itPair = tmpIterator;
                return *this;
            }

            virtual HashTableIterator& operator+(int index)
            {
                for (int i = 0; i < index; i++)
                    operator++();
                return *this;
            }

        private:
            std::vector<SortTable<KeyType, ValueType>>& buckets;
            size_t pos;
            OwnIterator<KeyType, ValueType> itPair;
        };

        HashTable(size_t numBuckets = 10)
            : buckets(numBuckets)
        { }

        HashTableIterator<KeyType, ValueType> begin()
        {
            for (size_t i = 0; i < buckets.end(); i++)
                if (buckets[i].begin() != buckets[i].end())
                    return HashTableIterator<KeyType, ValueType>(buckets, i, buckets[i].begin());
            return HashTableIterator<KeyType, ValueType>(buckets, buckets.size() - 1, buckets.back().end());
        }

        HashTableIterator<KeyType, ValueType> end()
        {
            for (int i = buckets.size() - 1; i >= 0; i--)
                if (buckets[i].begin() != buckets[i].end())
                    return HashTableIterator<KeyType, ValueType>(buckets, i, buckets[i].end());
            return HashTableIterator<KeyType, ValueType>(buckets, buckets.size() - 1, buckets.back().end());
        }

        HashTableIterator<KeyType, ValueType> find(const KeyType& key)
        {
            auto pos = getPos(key);
            auto itPair = buckets[pos].find(key);
            return HashTableIterator<KeyType, ValueType>(buckets, pos, itPair);
        }

        HashTableIterator<KeyType, ValueType> insert(const KeyType& key, const ValueType& value)
        {
            auto pos = getPos(key);
            auto itPair = buckets[pos].insert(key, value);
            return HashTableIterator<KeyType, ValueType>(buckets, pos, itPair);
        }

        void remove(const KeyType& key)
        {
            auto pos = getPos(key);
            buckets[pos].remove(key);
        }

        ValueType& operator[](const KeyType& key)
        {
            return *find(key);
        }

    private:
        std::hash<KeyType> hasher;
        std::vector<SortTable<KeyType, ValueType>> buckets;

        size_t getPos(KeyType const& key)
        {
            return  hasher(key) % buckets.size();
        }
    };
    template<typename KeyType, typename ValueType>
    class AVLTable
    {
        struct Node
        {
            Node(std::pair<KeyType, ValueType> data, Node* parent) : data(data), parent(parent) { }
            std::pair<KeyType, ValueType> data;
            Node* left = nullptr;
            Node* right = nullptr;
            Node* parent;
            int balance = 0;
            ~Node()
            {
                delete left;
                delete right;
            }
        };
    public:
        Node* root = nullptr;
        void insert(const KeyType& key, const KeyType& value)
        {
            if (!root)
            {
                root = new Node(std::make_pair(key, value), nullptr);
                return;
            }
            Node* node = root;
            Node* parent;

            while (1)
            {
                if (node->data.first == key)
                    return;
                parent = node;
                if (node->data.first > key)
                {
                    node = node->left;
                }
                else
                {
                    node = node->right;
                }
                if (!node)
                {
                    if (parent->data.first > key)
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
            if (root == nullptr)
                return nullptr;

            Node* node = root;
            Node* tmp = root;

            while (tmp)
            {
                node = tmp;
                if (key > node->data.first)
                {
                    tmp = node->right;
                }
                else if (key < node->data.first)
                {
                    tmp = node->left;
                }
                else if (key == node->data.first)
                    return &(node->data);
            }
            return nullptr;
        }
        void remove(const KeyType& key)
        {
            if (root == nullptr)
                return;

            Node* node = root;
            Node* parent = root;
            Node* child = root;
            Node* tmp = nullptr;

            while (child)
            {
                parent = node;
                node = child;
                if (key >= node->data.first)
                {
                    child = node->right;
                }
                else
                {
                    child = node->left;
                }
                if (key == node->data.first)
                    tmp = node;
            }
            if (tmp)
            {
                tmp->data = node->data;

                if (node->left)
                {
                    child = node->left;
                }
                else
                {
                    child = node->right;
                }

                if (root->data.first == key)
                {
                    root = child;
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
        void rebalance(Node* node)
        {
            updateBalance(node);

            if (node->balance == -2)
            {
                if (calculateBalance(node->left) <= 0)
                    node = rotateRight(node);
                else
                    node = rotateLeftRight(node);
            }
            else if (node->balance == 2)
            {
                if (calculateBalance(node->right) >= 0)
                    node = rotateLeft(node);
                else
                    node = rotateRightLeft(node);
            }
        }

        int calculateHeight(Node* node)
        {
            if (node == nullptr)
                return -1;
            return 1 + std::max(calculateHeight(node->left), calculateHeight(node->right));
        }

        void updateBalance(Node* node)
        {
            node->balance = calculateHeight(node->right) - calculateHeight(node->left);
        }

        size_t calculateBalance(Node* node)
        {
            return calculateHeight(node->right) - calculateHeight(node->left);
        }

        Node* rotateLeft(Node* p)
        {
            Node* q = p->right;
            q->parent = p->parent;
            p->right = q->left;
            q->left = p;
            p->parent = q;
            if (q->parent)
            {
                if (q->parent->right == p)
                {
                    q->parent->right = q;
                }
                else
                {
                    q->parent->left = q;
                }
            }
            updateBalance(p);
            updateBalance(q);
            return q;
        }

        Node* rotateRight(Node* p)
        {
            Node* q = p->left;
            q->parent = p->parent;
            p->left = q->right;
            q->right = p;
            p->parent = q;
            if (q->parent)
            {
                if (q->parent->right == p)
                {
                    q->parent->right = q;
                }
                else
                {
                    q->parent->left = q;
                }
            }
            updateBalance(p);
            updateBalance(q);
            return q;
        }

        Node* rotateLeftRight(Node* p)
        {
            p->left = rotateLeft(p->left);
            return rotateRight(p);
        }

        Node* rotateRightLeft(Node* p)
        {
            p->right = rotateRight(p->right);
            return rotateLeft(p);
        }

    };
}