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
public:
    OwnIterator() {}
    OwnIterator(std::pair<KeyType, ValueType>* ptr): p(ptr) {}
public:
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
    virtual typename OwnIterator::reference operator*() const
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
    {
        keyData.erase(std::remove(keyData.begin(), keyData.end(), *it.getPtr()), keyData.end());
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
        {
            throw std::runtime_error("Pair with same key don`t exist.");
        }
        else
        {
            auto begin = keyData.begin();
            std::advance(begin, it.getPtr() - &keyData[0]);
            keyData.erase(begin);
        }

    }
    virtual ValueType& operator[](const KeyType& key) override
    {
        return *(find(key));
    }
};

template<typename KeyType, typename ValueType>
class HashTable
{
private:
    using Map = std::vector<SortTable<KeyType, ValueType> >;

    static constexpr size_t _baseMapSize = 100;

    Map _map = Map(_baseMapSize, SortTable<KeyType, ValueType>());
    std::hash<KeyType> _hasher;

    size_t getPos(const KeyType& key)
    {
        return _hasher(key) % _map.size();
    }

public:
    template<class KeyType, class ValueType>
    class HashIterator : std::iterator<std::input_iterator_tag, ValueType>
    {
        friend class HashTable<KeyType, ValueType>;

    public:
        HashIterator::reference operator*() const
        {
            return *_pairIterator;
        }
        HashIterator<KeyType, ValueType>& operator++()
        {
            int tmpPos = _pos;
            auto tmpIterator = ++_pairIterator;
            if (tmpIterator == _map[tmpPos].end())
            {
                while (_map.size() != tmpPos + 1 && tmpIterator == _map[tmpPos].end())
                    tmpIterator = _map[++tmpPos].begin();
                
                if (_map.size() == tmpPos + 1)
                    return *this;
            }
            _pos = tmpPos;
            _pairIterator = tmpIterator;
            return *this;
        }
        HashIterator<KeyType, ValueType>& operator+(int index)
        {
            for (int i = 0; i < index; i++)
                this->operator++();
            return *this;
        }
        bool operator==(const HashIterator<KeyType, ValueType>& sec) const
        {
            return _pairIterator == sec._pairIterator;
        }
        bool operator!=(const HashIterator<KeyType, ValueType>& sec) const
        {
            return _pairIterator != sec._pairIterator;
        }

    private:
        Map& _map;
        size_t _pos;
        OwnIterator<KeyType, ValueType> _pairIterator;

        HashIterator(Map& map, size_t pos, OwnIterator<KeyType, ValueType> it) :
            _map(map),
            _pos(pos),
            _pairIterator(it)
        {   }
    };

    HashIterator<KeyType, ValueType> begin()
    {
        for (size_t i = 0; i < _map.size(); i++)
        {
            auto& pairs = _map[i];
            if (pairs.begin() != pairs.end())
                return HashIterator<KeyType, ValueType>(_map, i, pairs.begin());
        }
        return HashIterator<KeyType, ValueType>(_map, _map.size() - 1, _map.back().end());
    }
    HashIterator<KeyType, ValueType> end()
    {
        for (int i = _map.size() - 1; i >= 0; i--)
        {
            auto& pairs = _map[i];
            if (pairs.begin() != pairs.end())
                return HashIterator<KeyType, ValueType>(_map, i, pairs.end());
        }
        return HashIterator<KeyType, ValueType>(_map, _map.size() - 1, _map.back().end());
    }
    HashIterator<KeyType, ValueType> find(const KeyType& key)
    {
        auto pos = getPos(key);
        auto pairIt = _map[pos].find(key);
        return HashIterator<KeyType, ValueType>(_map, pos, pairIt);
    }
    HashIterator<KeyType, ValueType> insert(const KeyType& key, const ValueType& value)
    {
        auto pos = getPos(key);
        auto pairIt = _map[pos].insert(key, value);
        return HashIterator<KeyType, ValueType>(_map, pos, pairIt);
    }
    void remove(const KeyType& key)
    {
        auto pos = getPos(key);
        auto& pairs = _map[pos];
        pairs.remove(key);
    }
    ValueType& operator[](const KeyType& key)
    {
        return *find(key);
    }
};

template<class TKey, class TValue>
class AVLTable 
{
    struct Node
    {
        Node(const TKey& key, const TValue& value) :
            key(key),
            value(value)
        {   }

        TKey key;
        TValue value;
        std::shared_ptr<Node> left = nullptr;
        std::shared_ptr<Node> right = nullptr;
        int height = 1;
    };

    template<class TKey, class TValue>
    class AVLTableIterator : public std::iterator<std::input_iterator_tag, TValue>
    {
    public:
        AVLTableIterator(std::weak_ptr<AVLTable::Node> node) :
            _node(node)
        {   }

        bool operator==(AVLTableIterator const& other) const
        {
            return p == other.p;
        }
        bool operator!=(AVLTableIterator const& other) const
        {
            return p != other.p;
        }
        AVLTableIterator::reference operator*() const
        {
            return _node.lock()->value;
        }
        AVLTableIterator& operator++()
        {
            return *this;
        }
    private:
        std::weak_ptr<AVLTable::Node> _node = nullptr;
    };

public:
    AVLTableIterator<TKey, TValue> insert(const TKey& key, const TValue& value)
    {
        _root = insertNode(_root, key, value);
        return AVLTableIterator<TKey, TValue>(_root);
    }

    AVLTableIterator<TKey, TValue> find(const TKey& key)
    {
        return AVLTableIterator<TKey, TValue>(findKey(_root, key));
    }

    void remove(const TKey& key)
    {
        removeNode(_root, key);
    }

    TValue& operator[](const TKey& key)
    {
        return *find(_root, key);
    }

private:
    std::shared_ptr<Node> _root = nullptr;

    std::shared_ptr<Node> findKey(std::shared_ptr<Node> root, const TKey& key)
    {
        auto current = root;
        while (current && current->key != key)
            current = key < current->key ? current->left : current->right;
        return current;
    }

    int getHeight(std::shared_ptr<Node> node) 
    {
        if (node == nullptr) 
            return 0;
        return node->height;
    }

    std::shared_ptr<Node> rightRotate(std::shared_ptr<Node>& y)
    {
        auto x = y->left;
        auto T2 = x->right;

        x->right = y;
        y->left = T2;
        
        y->height = std::max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = std::max(getHeight(x->left), getHeight(x->right)) + 1;
        return x;
    }

    std::shared_ptr<Node> leftRotate(std::shared_ptr<Node>& x) 
    {
        auto y = x->right;
        auto T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = std::max(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = std::max(getHeight(y->left), getHeight(y->right)) + 1;
        return y;
    }

    int getBalanceFactor(std::shared_ptr<Node> node) 
    {
        if (node == nullptr) return 0;
        return getHeight(node->left) - getHeight(node->right);
    }

    std::shared_ptr<Node> insertNode(std::shared_ptr<Node> root, const TKey& key, const TValue& value) 
    {
        if (root == nullptr) 
            return std::make_shared<Node>(key, value);
        
        if (key < root->key)
            root->left = insertNode(root->left, key, value);
        else if (key > root->key)
            root->right = insertNode(root->right, key, value);
        else
            throw std::runtime_error("Node with that key already exist");

        root->height = 1 + std::max(getHeight(root->left), getHeight(root->right));
        auto balanceFactor = getBalanceFactor(root);
        if (balanceFactor > 1)
        {
            if (key < root->left->key)
            {
                return rightRotate(root);
            }
            else if (key > root->left->key)
            {
                root->left = leftRotate(root->left);
                return rightRotate(root);
            }
        }
        if (balanceFactor < -1)
        {
            if (key > root->right->key)
            {
                return leftRotate(root);
            }
            else if (key < root->right->key) 
            {
                root->right = rightRotate(root->right);
                return leftRotate(root);
            }
        }
        return root;
    }

    std::shared_ptr<Node> getMimumKeyNode(std::shared_ptr<Node> node) 
    {
        auto current = node;
        while (current->left != nullptr)
            current = current->left;
        return current;
    }

    std::shared_ptr<Node> removeNode(std::shared_ptr<Node> root, const TKey& key) 
    {
        if (root == nullptr) 
            return root;

        if (key < root->key)
        {
            root->left = removeNode(root->left, key);
        }
        else if (key > root->key)
        {
            root->right = removeNode(root->right, key);
        }
        else
        {
            if ((root->left == nullptr) || (root->right == nullptr))
            {
                auto temp = root->left ? root->left : root->right;
                if (temp == nullptr)
                {
                    temp = root;
                    root = nullptr;
                }
                else
                {
                    *root = *temp;
                }
            }
            else
            {
                auto temp = getMimumKeyNode(root->right);
                root->key = temp->key;
                root->right = removeNode(root->right, temp->key);
            }
        }

        if (root == nullptr)
            return root;

        root->height = 1 + std::max(getHeight(root->left), getHeight(root->right));

        int balanceFactor = getBalanceFactor(root);
        if (balanceFactor > 1)
        {
            if (getBalanceFactor(root->left) >= 0)
            {
                return rightRotate(root);
            }
            else
            {
                root->left = leftRotate(root->left);
                return rightRotate(root);
            }
        }
        if (balanceFactor < -1)
        {
            if (getBalanceFactor(root->right) <= 0)
            {
                return leftRotate(root);
            }
            else {
                root->right = rightRotate(root->right);
                return leftRotate(root);
            }
        }
        return root;
    }
};

}
