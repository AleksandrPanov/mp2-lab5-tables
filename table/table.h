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
class BST;

// https://habr.com/ru/post/265491/
template<typename KeyType, typename ValueType>
class OwnIterator: public std::iterator<std::input_iterator_tag, ValueType>
{
    friend class BaseTable<KeyType, ValueType>;
    friend class SimpleTable<KeyType, ValueType>;
    friend class SortTable<KeyType, ValueType>;
    friend class HashTable<KeyType, ValueType>;
    friend class BST<KeyType, ValueType>;
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

    virtual bool operator==(const BaseTable& table) const {
        return true;
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
        return OwnIterator<KeyType, ValueType>(&keyData.back());
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
};


template<typename KeyType, typename ValueType>
class SortTable : public SimpleTable<KeyType, ValueType>
{
    std::vector<std::pair<KeyType, ValueType> > keyData;
public:
    OwnIterator<KeyType, ValueType> find(const KeyType& key) override
    {
        size_t left = 0;
        size_t right = keyData.size() - 1;
        size_t middle;
        while (left <= right) {
            middle = left + (right - left) / 2;
            if (key == keyData[middle].first)
                return OwnIterator<KeyType, ValueType>(&keyData[0] + middle);
            if (key < keyData[middle].first)
                right = middle - 1;
            else
                left = middle + 1;
        }
        return this->end();
    }

    OwnIterator<KeyType, ValueType> insert(const KeyType& key, const ValueType& value) override
    {
        std::pair<KeyType, ValueType> elem = std::make_pair(key, value);
        auto pos = std::upper_bound(keyData.begin(), keyData.end(), elem);
        keyData.insert(pos, elem);
        return OwnIterator<KeyType, ValueType>(&elem);
    }

    virtual ValueType& operator[](const KeyType& key) override
    {
        return *(find(key));
    }

    virtual void remove(const KeyType& key) override
    {
        OwnIterator<KeyType, ValueType> iter = find(key);
        auto begin = keyData.begin();
        std::advance(begin, iter.getPtr() - &keyData[0]);
        keyData.erase(begin);
    }

    virtual OwnIterator<KeyType, ValueType> getMin() override  
    { 
        return OwnIterator<KeyType, ValueType>(&keyData.front());
    }

    virtual OwnIterator<KeyType, ValueType> getMax() override
    {
        return OwnIterator<KeyType, ValueType>(&keyData.back());
    }
};


template<typename KeyType, typename ValueType>
class HashTable : public BaseTable<KeyType, ValueType>
{
    static const int default_size = 8;
    constexpr static const double rehash_size = 0.75;
    struct Node {
        std::pair<KeyType, ValueType> pair;
        bool state;
        Node(const std::pair<KeyType, ValueType>& _pair) {
            pair = _pair;
            state = true;
        }
    };
    Node** arr;
    int size;
    int buffer_size;
    int size_all_non_nullptr;

    int HashFunc1(int k, int s) {
        return k % s;
    }
    int HashFunc2(int k, int s) {
        return (k * s - 1) % s;
    }
public:
    
    HashTable() {
        buffer_size = default_size;
        size = 0;
        size_all_non_nullptr = 0;
        arr = new Node*[buffer_size];
        for (size_t i = 0; i < buffer_size; ++i)
            arr[i] = nullptr;
    }

    void print() {
        for (int i = 0; i < buffer_size; i++) {
            if (arr[i] != nullptr)
                std::cout << arr[i]->pair.first << " " << arr[i]->pair.second << std::endl;
        }
    }

    void resize() {
        int past_buffer_size = buffer_size;
        buffer_size *= 2;
        size_all_non_nullptr = 0;
        size = 0;
        Node** arr2 = new Node * [buffer_size];
        for (size_t i = 0; i < buffer_size; ++i)
            arr2[i] = nullptr;
        std::swap(arr, arr2);
        for (size_t i = 0; i < past_buffer_size; i++) {
            if (arr2[i] && arr2[i]->state)
                insert(arr2[i]->pair.first, arr2[i]->pair.second);
        }
        for (size_t i = 0; i < past_buffer_size; i++)
            if (arr2[i])
                delete arr2[i];
        delete[] arr2;
    }

    void rehash() {
        size_all_non_nullptr = 0;
        size = 0;
        Node** arr2 = new Node * [buffer_size];
        for (size_t i = 0; i < buffer_size; i++)
            arr2[i] = nullptr;
        std::swap(arr, arr2);
        for (size_t i = 0; i < buffer_size; i++) {
            if(arr2[i] && arr2[i]->state)
                insert(arr2[i]->pair.first, arr2[i]->pair.second);
        }
        for (size_t i = 0; i < buffer_size; i++)
            if (arr2[i])
                delete arr2[i];
        delete[] arr2;
    }

    virtual OwnIterator<KeyType, ValueType> find(const KeyType& key) override
    {
        int h1 = HashFunc1(key, buffer_size);
        int h2 = HashFunc2(key, buffer_size);
        size_t i = 0;
        while (arr[h1] != nullptr && i < buffer_size) {
            if (arr[h1]->pair.first == key && arr[h1]->state)
                return OwnIterator<KeyType, ValueType>(&arr[h1]->pair);
            h1 = (h1 + h2) % buffer_size;
            i++;
        }
        return end();
    }
    virtual OwnIterator<KeyType, ValueType> insert(const KeyType& key, const ValueType& value) override
    {
        auto elem = std::make_pair(key, value);
        if (size + 1 > int(rehash_size * buffer_size))
            resize();
        else if (size_all_non_nullptr > 2 * size)
            rehash();
        int h1 = HashFunc1(key, buffer_size);
        int h2 = HashFunc2(key, buffer_size);
        int i = 0;
        int first_deleted = -1;
        while (arr[h1] != nullptr && i < buffer_size) {
            if (arr[h1]->pair == elem && arr[h1]->state)
                return end();
            if (!arr[h1]->state && first_deleted == -1)
                first_deleted = h1;
            h1 = (h1 + h2) % buffer_size;
            i++;
        }
        if (first_deleted == -1) {
            arr[h1] = new Node(elem);
            size_all_non_nullptr++;
        }
        else {
            arr[first_deleted]->pair = elem;
            arr[first_deleted]->state = true;
        }
        size++;
        return OwnIterator<KeyType, ValueType>(&elem);
    }
    virtual void remove(const KeyType& key) override
    {
        int h1 = HashFunc1(key, buffer_size);
        int h2 = HashFunc2(key, buffer_size);
        size_t i = 0;
        while (arr[h1] != nullptr && i < buffer_size) {
            if (arr[h1]->pair.first == key && arr[h1]->state) {
                arr[h1]->state = false;
                --size;
            }
            h1 = (h1 + h2) % buffer_size;
            i++;
        }
    }
    virtual ValueType& operator[](const KeyType& key) override
    {
        return *(find(key));
    }
    virtual OwnIterator<KeyType, ValueType> end() override {
        return OwnIterator<KeyType, ValueType>();
    }
    ~HashTable() {
        for (size_t i = 0; i < buffer_size; ++i)
            if (arr[i])
                delete arr[i];
        delete[] arr;
    }
};


template<typename KeyType, typename ValueType>
class BST : public BaseTable<KeyType, ValueType>
{
    struct Node {
        std::pair<KeyType, ValueType> pair;
        Node* left;
        Node* right;
    };

    size_t size = 0;
    Node* root;

    Node* insert(Node* node, const KeyType& key, const ValueType& value) {
        if (!node)
            return new Node(std::make_pair(key, value));

        if (key < node->pair.first) {
            node->left = insert(node->left, key, value);
        }
        else {
            node->right = insert(node->right, key, value);
        }

        return node;
    }

    Node* find(Node* node, const KeyType& key) {
        Node* res;
        
        if (!node)
            return nullptr;

        if (key == node->pair.first)
            return node;

        if (key < node->pair.first)
            res = find(node->left, key);
        else
            res = find(node->right, key);

        return res;
    }

    Node* remove(Node* node, const KeyType& key) {
        if (!node)
            return nullptr;
        
        if (key < node->pair.first) {
            node->left = remove(node->left, key);
            return node;
        }
        else if (key > node->pair.first) {
            node->right = remove(node->right, key);
            return node;
        }
        else {
            --size;
            if (!node->left && !node->right) {
                return nullptr;
            }
            if (!node->left)
                return node->right;
            if (!node->right)
                return node->left;

            Node* minFromRight = node->right;
            while (minFromRight->left)
                minFromRight = minFromRight->left;
            node->pair = minFromRight->pair;
            node->right = remove(node->right, node->pair.first);
            return node;
        }
    }

    void printLeft(Node* node) {
        if (node) {
            printLeft(node->left);
            std::cout << node->pair.first << " " << node->pair.second << std::endl;
            printLeft(node->right);
        }
    }
public:
    BST() {
        root = nullptr;
    }

    virtual OwnIterator<KeyType, ValueType> find(const KeyType& key) override
    {
        auto elem = find(root, key);
        if (elem == nullptr)
            return this->end();
        return OwnIterator<KeyType, ValueType>(&elem->pair);
    }

    virtual OwnIterator<KeyType, ValueType> insert(const KeyType& key, const ValueType& value)
    {
        root = insert(root, key, value);
        auto elem = std::make_pair(key, value);
        size++;
        return OwnIterator<KeyType, ValueType>(&elem);
    }

    virtual void remove(const KeyType& key) override
    {
        root = remove(root, key);
    }

    virtual ValueType& operator[](const KeyType& key) override
    {
        auto elem = find(root, key);
        return elem->pair.second;
    }

    void print() {
        printLeft(root);
    }

    virtual OwnIterator<KeyType, ValueType> getMax() override
    { 
        if (!root)
            return this->begin();
        Node* current = root;
        while (current->right) {
            current = current->right;
        }
        return OwnIterator<KeyType, ValueType>(&current->pair);
    }

    virtual OwnIterator<KeyType, ValueType> getMin() override
    {
        if (!root)
            return this->begin();
        Node* current = root;
        while (current->left) {
            current = current->left;
        }
        return OwnIterator<KeyType, ValueType>(&current->pair);
    }

    virtual size_t getSize() override
    {
        return size;
    }
};


template<typename KeyType, typename ValueType>
class AVLTree
{
    struct Node {
        //KeyType key;
        //ValueType value;
        std::pair<KeyType, ValueType> pair;
        Node* left;
        Node* right;
        int height;
    };

    Node* root;

    Node* insert(Node* node, const KeyType& key, const ValueType& value)
    {
        if (node == nullptr)
        {
            node = new Node;
            node->pair.first= key;
            node->pair.second = value;
            node->height = 0;
            node->left = node->right = nullptr;
        }
        else if (key < node->pair.first)
        {
            node->left = insert(node->left, key, value);
            if (height(node->left) - height(node->right) == 2)
            {
                if (key < node->left->pair.first)
                    node = singleRightRotate(node);
                else
                    node = doubleRightRotate(node);
            }
        }
        else if (key > node->pair.first)
        {
            node->right = insert(node->right, key, value);
            if (height(node->right) - height(node->left) == 2)
            {
                if (key > node->right->pair.first)
                    node = singleLeftRotate(node);
                else
                    node = doubleLeftRotate(node);
            }
        }

        node->height = std::max(height(node->left), height(node->right)) + 1;
        return node;
    }

    Node* singleRightRotate(Node*& node)
    {
        Node* u = node->left;
        node->left = u->right;
        u->right = node;
        node->height = std::max(height(node->left), height(node->right)) + 1;
        u->height = std::max(height(u->left), node->height) + 1;
        return u;
    }

    Node* singleLeftRotate(Node*& node)
    {
        Node* u = node->right;
        node->right = u->left;
        u->left = node;
        node->height = std::max(height(node->left), height(node->right)) + 1;
        u->height = std::max(height(node->right), node->height) + 1;
        return u;
    }

    Node* doubleLeftRotate(Node*& node)
    {
        node->right = singleRightRotate(node->right);
        return singleLeftRotate(node);
    }

    Node* doubleRightRotate(Node*& node)
    {
        node->left = singleLeftRotate(node->left);
        return singleRightRotate(node);
    }

    Node* findMin(Node* node)
    {
        if (node == nullptr)
            return nullptr;
        else if (node->left == nullptr)
            return node;
        else
            return findMin(node->left);
    }

    Node* findMax(Node* node)
    {
        if (node == nullptr)
            return nullptr;
        else if (node->right == nullptr)
            return node;
        else
            return findMax(node->right);
    }

    Node* find(Node* node, const KeyType& key) {
        if (node == nullptr)
            return nullptr;

        if (key == node->pair.first) {
            return node;
        }
        else if (key < node->pair.first) {
            return find(node->left, key);
        }
        else {
            return find(node->right, key);
        }
    }

    Node* remove(Node* node, const KeyType& key)
    {
        Node* temp;

        if (node == nullptr)
            return nullptr;

        else if (key < node->pair.first)
            node->left = remove(node->left, key);
        else if (key > node->pair.first)
            node->right = remove(node->right, key);

        else if (node->left && node->right)
        {
            temp = findMin(node->right);
            node->pair.first = temp->pair.first;
            node->pair.second = temp->pair.second;
            node->right = remove(node->right, node->pair.first);
        }
        else
        {
            temp = node;
            if (node->left == nullptr)
                node = node->right;
            else if (node->right == nullptr)
                node = node->left;
            delete temp;
        }
        if (node == nullptr)
            return node;

        node->height = std::max(height(node->left), height(node->right)) + 1;

        if (height(node->left) - height(node->right) == 2)
        {
            if (height(node->left->left) - height(node->left->right) == 1)
                return singleLeftRotate(node);
            else
                return doubleLeftRotate(node);
        }
        else if (height(node->right) - height(node->left) == 2)
        {
            if (height(node->right->right) - height(node->right->left) == 1)
                return singleRightRotate(node);
            else
                return doubleRightRotate(node);
        }
        return node;
    }

    int height(Node* node)
    {
        return (node == nullptr ? -1 : node->height);
    }

    int getBalance(Node* node)
    {
        if (node == nullptr)
            return 0;
        else
            return height(node->left) - height(node->right);
    }

    void print(Node* node)
    {
        if (node == nullptr)
            return;
        print(node->left);
        std::cout << node->pair.first << " " << node->pair.second << std::endl;
        print(node->right);
    }

public:
    AVLTree()
    {
        root = nullptr;
    }

    void insert(const KeyType& key, const ValueType& value)
    {
        root = insert(root, key, value);
    }

    void remove(const KeyType& key)
    {
        root = remove(root, key);
    }

    std::pair<KeyType, ValueType>* find(KeyType key) {

        return &(find(root, key)->pair);
    }

    void print()
    {
        print(root);
        std::cout << std::endl;
    }
};



}



