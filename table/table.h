#pragma once
#include <vector>
#include <chrono>
#include <algorithm>
#include <list>
#include <functional>

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
        //if (!(this->p)) return ValueType();
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
    void print()
    {
        for (int i = 0; i < keyData.size(); i++)
        {
            cout << "(" << keyData[i].first << "," << keyData[i].second << ") ";
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
        // бинарный поиск
        size_t l = 0, r = keyData.size();  // [l, r)
        while (l + 1 < r) {
            int m = (l + r) / 2;
            if (keyData[m].first <= key)
                l = m;
            else
                r = m;
        }
        if (l < keyData.size() && keyData[l].first == key)
            return OwnIterator<KeyType, ValueType>(&keyData[0] + l);
        else
            return OwnIterator<KeyType, ValueType>(&keyData[0] + keyData.size());
    }
    OwnIterator<KeyType, ValueType> insert(const KeyType& key, const ValueType& value) override
    {
        std::pair<KeyType, ValueType> elem = std::make_pair(key, value);
        keyData.push_back(elem);
        for (int i = (keyData.size() - 1); i > 0; i--)
        {
            if (keyData[i - 1].first > keyData[i].first)
            {
                keyData[i] = keyData[i - 1];
                keyData[i - 1] = elem;
            } 
            else return OwnIterator<KeyType, ValueType>(&keyData[i]);
        }
        return OwnIterator<KeyType, ValueType>(&keyData[0]);
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

    virtual OwnIterator<KeyType, ValueType> getMin()
    {
        return begin();
    }
    virtual OwnIterator<KeyType, ValueType> getMax()
    {
        if (keyData.size() == 0ull)
            return OwnIterator<KeyType, ValueType>();
        return &(keyData.back());
    }

    void print()
    {
        for (int i = 0; i < keyData.size(); i++)
        {
            cout <<"("<<keyData[i].first << "," << keyData[i].second << ") ";
        }
    }
};

template<typename KeyType, typename ValueType>
class HashTable  
{
private:
    static const int default_size = 128; // степень двойки
    struct Node
    {
        std::pair<KeyType, ValueType> pair;
        bool state; // state = true - элемент добавлен; state = false - элемент удален
        Node() { state = true; pair = std::pair<KeyType, ValueType>(); }
        Node(const std::pair<KeyType, ValueType>& p) : state(true), pair(p) {}
        Node(const KeyType& key_, const ValueType& value_)
        {
            pair.first = key_;
            pair.second = value_;
            state = true;
        }
        KeyType getKey() { return pair.first; }
        ValueType getValue() { return pair.second; }

    };
    std::vector<Node*> arr;
    int size; // количество значащих элементов в массиве без учета deleted
    int buffer_size; // размер массива
    int size_all_non_nullptr; // количество элементов в массиве с учетом deleted

    size_t HashFunction1(const KeyType& key)
    {
            return HashFunction(key, static_cast<size_t>(buffer_size - 1)); // ключи должны быть взаимопросты, используем числа <размер таблицы> плюс и минус один.
    };
    size_t HashFunction2(const KeyType& key)
    {
            size_t hash_result = HashFunction(key, static_cast<size_t>(buffer_size + 1));
            return (2*hash_result+1); // нечетное значение => взаимнопростое по отношению к размеру таблицы (степень двойки)
    };

    size_t HashFunction(const KeyType& key, const size_t prime)
    {
        std::hash<KeyType> hash;
        size_t hash_result = (hash(key + static_cast<KeyType>(prime)) + prime) % static_cast<size_t>(buffer_size);
        return hash_result;
    }

public:
    HashTable()
    {
        buffer_size = default_size;
        size = 0;
        size_all_non_nullptr = 0;
        for (int i = 0; i < buffer_size; i++)
            arr.push_back(nullptr);             // инициализация массива указателей значениями nullptr
    }
    ~HashTable() 
    {
        for (int i = 0; i < buffer_size; i++)
            delete arr[i];
    }

    void resize(size_t k = 1) // k - коэффициент увеличение размера таблицы
    {
        int old_buffer_size = buffer_size;
        buffer_size *= 2*k;
        size_all_non_nullptr = 0;
        size = 0;

        std::vector<Node*> new_arr(buffer_size, nullptr);
        for (int i = 0; i < old_buffer_size; ++i)
        {
            if (arr[i])
            {
                Node& currNode = *arr[i];
                if (currNode.state)
                    insert(currNode.getKey(), currNode.getValue()); // добавляем элементы в новый массив
                delete arr[i]; // освобождаем память элемента в старом массиве
            }
        }
        // move new_arr into arr
        arr.clear();
        arr = move(new_arr);
    }

    void rehash()
    {
        size_all_non_nullptr = 0;
        size = 0;

        std::vector<Node*> new_arr(buffer_size, nullptr);
        for (int i = 0; i < buffer_size; ++i)
        {
            if (arr[i])
            {
                Node& currNode = *arr[i];
                if (currNode.state)
                    insert(currNode.getKey(), currNode.getValue()); // добавляем элементы в новый массив
                delete arr[i]; // освобождаем память элемента в старом массиве
            }
        }
        // move new_arr into arr
        arr.clear();
        arr = move(new_arr);
    }

    bool find(const KeyType& key)
    {
        size_t h1 = HashFunction1(key); // значение, отвечающее за начальную позицию
        size_t h2 = HashFunction2(key); // значение, ответственное за "шаг" по таблице
        int i = 0;
        while (arr[h1] != nullptr && i < buffer_size)
        {
            Node& currNode = *arr[h1];
            if (currNode.getKey() == key && currNode.state)
                return true; // такой элемент есть (!!!)
            h1 = (h1 + h2) % buffer_size;
            ++i;
        }
        return false;
    }
    bool insert(const KeyType& key, const ValueType& value)
    {
        if (size_all_non_nullptr + 1 > int(0.75 * buffer_size)) // 0,75 - процент заполенности таблицы
            resize();
        else if (size_all_non_nullptr > 2 * size) // если удаленных+значащих элементов в 2 раза больше значащих
            rehash();
        int h1 = HashFunction1(key);
        int h2 = HashFunction2(key);
        int i = 0;
        int first_deleted = -1; // запоминаем первый подходящий (удаленный) элемент
        while (arr[h1] != nullptr && i < buffer_size)
        {

        }
        if (first_deleted == -1) // если не нашлось подходящего места, создаем новый Node
        {
            arr[h1] = new Node(key, value);
            ++size_all_non_nullptr; // так как мы заполнили один пробел, не забываем записать, что это место теперь занято
        }
        else
        {
            *arr[first_deleted]  = Node(key, value);
        }
        ++size; // и в любом случае мы увеличили количество элементов
        return true;
    }
    bool remove(const KeyType& key)
    {
        int h1 = HashFunction1(key); // значение, отвечающее за начальную позицию
        int h2 = HashFunction2(key); // значение, ответственное за "шаг" по таблице
        int i = 0;
        while (arr[h1] != nullptr && i < buffer_size)
        {
            Node& currNode = *arr[h1];
            if (currNode.getKey() == key && currNode.state)
            {
                currNode.state = false;
                --size;
                return true;
            }
            h1 = (h1 + h2) % buffer_size;
            ++i;
        }
        return false;
    }

    void print()
    {
        for (int i = 0; i < arr.size(); i++)
        {
            if (arr[i])
            {
                Node& currNode = *arr[i];
                if (currNode.state)
                    cout << "(" << currNode.getKey() << "," << currNode.getValue() << ") ";
            }
        }
    }
    
    bool change(const KeyType& key, const ValueType& value) // ключ, новое значение
    {
        size_t h1 = HashFunction1(key); // значение, отвечающее за начальную позицию
        size_t h2 = HashFunction2(key); // значение, ответственное за "шаг" по таблице
        int i = 0;
        while (arr[h1] != nullptr && i < buffer_size)
        {
            Node& currNode = *arr[h1];
            if (currNode.getKey() == key && currNode.state)
            {
                currNode.pair.second = value;
                return true; // элемент изменен
            }
            h1 = (h1 + h2) % buffer_size;
            ++i;
        }
        return false; // элемент не найден
    }

    int getSize()
    {
        return size;
    }

    std::pair<KeyType, ValueType> begin()
    {
        for (int i = 0; i < arr.size(); i++)
        {
            if (arr[i])
            {
                Node& currNode = *arr[i];
                if (currNode.state)
                    return currNode.pair;
            }
        }
        return std::pair<KeyType, ValueType>();
    }

    std::pair<KeyType, ValueType> end()
    {
        for (int i = arr.size() - 1; i >= 0; i--)
        {
            if (arr[i])
            {
                Node& currNode = *arr[i];
                if (currNode.state)
                    cout << currNode.pair;
            }
        }
    }

    std::pair<KeyType, ValueType>& operator[](const KeyType& key)
    {
        size_t h1 = HashFunction1(key); // значение, отвечающее за начальную позицию
        size_t h2 = HashFunction2(key); // значение, ответственное за "шаг" по таблице
        int i = 0;
        while (arr[h1] != nullptr && i < buffer_size)
        {
            Node& currNode = *arr[h1];
            if (currNode.getKey() == key && currNode.state)
                return currNode.pair; // такой элемент есть (!!!)
            h1 = (h1 + h2) % buffer_size;
            ++i;
        }
        return std::pair<KeyType, ValueType>();
    }

    ValueType getData(const KeyType& key)
    {
        size_t h1 = HashFunction1(key); // значение, отвечающее за начальную позицию
        size_t h2 = HashFunction2(key); // значение, ответственное за "шаг" по таблице
        int i = 0;
        while (arr[h1] != nullptr && i < buffer_size)
        {
            Node& currNode = *arr[h1];
            if (currNode.getKey() == key && currNode.state)
                return currNode.pair.second; // такой элемент есть (!!!)
            h1 = (h1 + h2) % buffer_size;
            ++i;
        }
        return ValueType();
    }

    KeyType getMin()
    {
        if (size == 0) return KeyType();

        KeyType min;
        bool firstEl = true; // true - первый значащий элемент не найден, false - первый значащий элемент уже найден
        for (int i = 0; i < arr.size(); i++)
        {
            if (arr[i])
            {
                Node& currNode = *arr[i];
                if (currNode.state)
                    if (firstEl)
                    {
                        min = currNode.pair.first;
                        firstEl = false;
                    }
                    else if (min > currNode.pair.first)
                        min = currNode.pair.first;

            }
        }
        return min;
    }

    KeyType getMax()
    {
        if (size == 0) return KeyType();

        KeyType max;
        bool firstEl = true; // true - первый значащий элемент не найден, false - первый значащий элемент уже найден
        for (int i = 0; i < arr.size(); i++)
        {
            if (arr[i])
            {
                Node& currNode = *arr[i];
                if (currNode.state)
                    if (firstEl)
                    {
                        max = currNode.pair.first;
                        firstEl = false;
                    }
                    else if (max < currNode.pair.first)
                        max = currNode.pair.first;

            }
        }
        return max;
    }

};

template<typename KeyType, typename ValueType>
class AVLTable
{
private:
    enum Balance { LeftHeavy = -1, Balanced, RightHeavy};
    struct Node
    {
        std::pair<KeyType, ValueType> pair;
        Balance balance;                   // разность между высотой левого и правого поддерева данной вершины (hr - hl). 
        Node* left;
        Node* right;
        Node() { pair = std::pair<KeyType, ValueType>(); balance = Balance::Balanced; left = nullptr; right = nullptr; }
        Node(const KeyType& key, const ValueType& value)
        { 
            pair = std::pair<KeyType, ValueType>(key,value); balance = Balance::Balanced; left = nullptr; right = nullptr;
        }
        ~Node() { delete left; delete right; }
    };

    Node* root;
    int size;

public:
    AVLTable() : root(nullptr), size(0) {}
    AVLTable(const KeyType& key, const ValueType& value) { root = new Node(key, value); size = 1; }

    ~AVLTable() { delete root; }

    bool insert(const KeyType& key, const ValueType& value)
    {
        if (!root)
        {
            root = new Node(key, value);
            size++;
            return true;
        }
        else
        {
            bool checkInsert = false;                   // проверка успешности вставки
            addRecursive(root, nullptr, key, value, checkInsert);
            if (checkInsert)
                size++;
            return checkInsert;
        }
    }

    bool remove(const KeyType& key)
    {
        if (!root)                                      // проверка, есть ли элементы?
        {
            return false;
        }
        else
        {
            bool checkRemove = false;                   // проверка успешности удаления
            removeRecursive(root, nullptr, key, checkRemove);
            if (checkRemove)
                size--;
            return checkRemove;
        }
    }

    bool find(const KeyType& key)
    {
        if (!root)                                      // проверка, есть ли элементы?
        {
            return false;
        }
        else
        {
            return findRecursive(root, key);
        }
    }

    ValueType getData(const KeyType& key)
    {
        if (!root)                                      // проверка, есть ли элементы?
        {
            return ValueType();
        }
        else
        {
            return getDataRecursive(root, key);
        }
    }

    int getSize() 
    {
        return size;
    }
private:
    bool addRecursive(Node* parent, Node* grandParent, const KeyType& key, const ValueType& value, bool& checkInsert)
    {
        bool grew;                                        // логическая переменная, означающая выросло ли поддерево после выставки в него элемента
        if (key == parent->pair.first)                    // ключи равны
        {
            parent->pair.second = value;
            checkInsert = true;
            return false;
        }
        else if (key < parent->pair.first)                // ключ меньше
        {
            if (!parent->left) // если левого элемента нет (nullptr) => вставка
            {
                parent->left = new Node(key, value);
                checkInsert = true;
                grew = true;
            } else
                grew = addRecursive(parent->left, parent, key, value, checkInsert);

            if (!grew)
                return false;
            else
            {
                if (parent->balance == Balance::RightHeavy)
                {
                    parent->balance = Balance::Balanced; // правое дерево было выше на 1, но и левое дерево выросло на 1 => поддерево сбалансировано 
                    return false; 
                }
                else if (parent->balance == Balance::Balanced)
                {
                    parent->balance = Balance::LeftHeavy; // левое дерево выросло, но дерево ещё является сбалансированным
                    return true;
                }
                else // if (parent->balance == Balance::LeftHeavy)
                {
                    rebalanceLeftSubtree(parent, grandParent);         // левое дерево выше правого на 2, нужна перебалансировка
                    return false;
                }
            }
        }
        else                                              // ключ больше
        {
            if (!parent->right) // если правого элемента нет (nullptr)
            {
                parent->right = new Node(key, value);
                checkInsert = true;
                grew = true;
            } else
                grew = addRecursive(parent->right, parent, key, value, checkInsert);

            if (!grew)
                return false;
            else
            {
                if (parent->balance == Balance::LeftHeavy)
                {
                    parent->balance = Balance::Balanced;    // левое дерево было выше на 1, но и правое дерево выросло на 1 => поддерево сбалансировано 
                    return false;
                }
                else if (parent->balance == Balance::Balanced)
                {
                    parent->balance = Balance::RightHeavy; // правое дерево выросло, но дерево ещё является сбалансированным
                    return true;
                }
                else // if (parent->balance == Balance::RightHeavy)
                {
                    rebalanceRightSubtree(parent, grandParent);          // правое дерево выше левого на 2, нужна перебалансировка
                    return false;
                }
            }
        }
    }

    void rebalanceLeftSubtree(Node* parent, Node* grandParent)
    {
        Node* child = parent->left;
        if (child->balance == Balance::LeftHeavy)      // требуется малое правое вращение
        {
            parent->left = child->right;
            child->right = parent;
            parent->balance = Balance::Balanced;
            child->balance = Balance::Balanced;
            // замена parent на child
            if (!grandParent)                       // если grandParent == nullptr, то root == parent
                root = child;
            else if (grandParent->right && grandParent->right == parent)
                grandParent->right = child;
            else grandParent->left = child;
        }
        else if (child->balance == Balance::RightHeavy)  // требуется врещение влево-вправо
        {
            Node* grandChild = child->right;
            child->right = grandChild->left;
            grandChild->left = child;
            parent->left = grandChild->right;
            grandChild->right = parent;
            if (grandChild->balance == Balance::LeftHeavy)
                parent->balance = Balance::RightHeavy;
            else
                parent->balance = Balance::Balanced;
            if (grandChild->balance == Balance::RightHeavy)
                child->balance = Balance::LeftHeavy;
            else
                child->balance = Balance::Balanced;
            grandChild->balance = Balance::Balanced;
            // замена parent на grandChild
            if (!grandParent)                       // если grandParent == nullptr, то root == parent
                root = grandChild;
            else if (grandParent->right && grandParent->right == parent)
                grandParent->right = grandChild;
            else grandParent->left = grandChild;
        }
    }

    void rebalanceRightSubtree(Node* parent, Node* grandParent)
    {
        Node* child = parent->right;
        if (child->balance == Balance::RightHeavy)      // требуется малое левое вращение
        {
            parent->right = child->left;
            child->left = parent;
            parent->balance = Balance::Balanced;
            child->balance = Balance::Balanced;
            // замена parent на child
            if (!grandParent)                           // если grandParent == nullptr, то root == parent
                root = child;
            else if (grandParent->right && grandParent->right == parent)
                grandParent->right = child;
            else grandParent->left = child;
        }
        else if (child->balance == Balance::LeftHeavy)  // требуется врещение вправо-влево
        {
            Node* grandChild = child->left;
            child->left = grandChild->right;
            grandChild->right = child;
            parent->right = grandChild->left;
            grandChild->left = parent;
            if (grandChild->balance == Balance::RightHeavy)
                parent->balance = Balance::LeftHeavy;
            else 
                parent->balance = Balance::Balanced;
            if (grandChild->balance == Balance::LeftHeavy)
                child->balance = Balance::RightHeavy;
            else
                child->balance = Balance::Balanced;
            grandChild->balance = Balance::Balanced;
            // замена parent на grandChild
            if (!grandParent)                       // если grandParent == nullptr, то root == parent
                root = grandChild;
            else if (grandParent->right && grandParent->right == parent)
                grandParent->right = grandChild;
            else grandParent->left = grandChild;
        }
    }

    bool removeRecursive(Node* parent, Node* grandParent, const KeyType& key, bool& checkRemove)
    {
        bool shrunk;            // переменная shrunk означает, сократилась ли длина поддерева после удаления
        if (key < parent->pair.first)
        {
            if (!parent->left) // если левого элемента нет (nullptr) => удаление не удалось
            {
                checkRemove = false;
                return false; 
            }
            else 
                shrunk = removeRecursive(parent->left, parent, key, checkRemove);

            if (shrunk)
                return rebalanceLeftShrunk(parent, grandParent); 
            else
                return false;

        }
        else if (key > parent->pair.first)
        {
            if (!parent->right)
            {
                checkRemove = false;
                return false;
            }
            else
                shrunk = removeRecursive(parent->right, parent, key, checkRemove);

            if (shrunk)
                return rebalanceRightShrunk(parent, grandParent);
            else
                return false;
        }
        else // if (key == parent->pair.first)
        {
            // найден элемент для удалаения
            if (!parent->right && parent->left) // у parent есть только левый потомок
            {
                // замена parent на его левого потомка
                if (!grandParent) // если grandParent == nullptr, то parent == root
                    root = parent->left;
                else if (grandParent->right && grandParent->right == parent)
                    grandParent->right = parent->left;
                else grandParent->left = parent->left;
                shrunk = true;
            }
            else if (!parent->left && parent->right) // у parent есть только правый потомок
            {
                // замена parent на его правого потомка
                if (!grandParent) // если grandParent == nullptr, то parent == root
                    root = parent->right;
                else if (grandParent->right && grandParent->right == parent)
                    grandParent->right = parent->right;
                else grandParent->left = parent->right;
                shrunk = true;
            }
            else if (parent->left && parent->right) // есть и левый и правый узлы
            {
                Node* newParent;
                shrunk = replaceRightmost(grandParent, parent, parent, parent->left, &newParent);
                if (shrunk)
                    shrunk = rebalanceLeftShrunk(newParent, grandParent);
            }
            else // нет обоих потомков
            {
                if (!grandParent) // если grandParent == nullptr, то parent == root
                    root = nullptr;
                else if (grandParent->right && grandParent->right == parent)
                    grandParent->right = nullptr;
                else grandParent->left = nullptr;
                shrunk = true;
            }
            parent->left = nullptr;
            parent->right = nullptr;
            delete parent;
            checkRemove = true;
            return shrunk;
        }
    }

    bool rebalanceLeftShrunk(Node* parent, Node* grandParent)
    {
        bool shrunk = false;
        Node* child;
        Node* grandChild;
        Balance child_bal, grandChild_bal;
        if (parent->balance == Balance::LeftHeavy)
        {
            parent->balance = Balance::Balanced;
            return true;
        }
        else if (parent->balance == Balance::Balanced)
        {
            parent->balance = Balance::RightHeavy;
            return false;
        }
        else
        {
            child = parent->right;
            child_bal = child->balance;
            if (child->balance != Balance::LeftHeavy)
            {
                // Вращение влево
                parent->right = child->left;
                child->left = parent;
                if (child->balance == Balance::Balanced)
                {
                    parent->balance = Balance::RightHeavy;
                    child->balance = Balance::LeftHeavy;
                    shrunk = false;
                }
                else
                {
                    parent->balance = Balance::Balanced;
                    child->balance = Balance::Balanced;
                    shrunk = true;
                }
                if (!grandParent)                           // если grandParent == nullptr, то root == parent
                    root = child;
                else if (grandParent->right && grandParent->right == parent)
                    grandParent->right = child;
                else grandParent->left = child;
                return shrunk;
            }
            else
            {
                // вращение влево-вправо
                grandChild = child->left;
                grandChild_bal = grandChild->balance;
                child->left = grandChild->right;
                grandChild->right = child;
                parent->right = grandChild->left;
                grandChild->left = parent;

                if (grandChild_bal == Balance::RightHeavy)
                    parent->balance = Balance::LeftHeavy;
                else
                    parent->balance = Balance::Balanced;
                if (grandChild_bal == Balance::LeftHeavy)
                    child->balance = Balance::RightHeavy;
                else
                    child->balance = Balance::Balanced;
                grandChild->balance = Balance::Balanced;
                if (!grandParent)                           // если grandParent == nullptr, то root == parent
                    root = grandChild;
                else if (grandParent->right && grandParent->right == parent)
                    grandParent->right = grandChild;
                else grandParent->left = grandChild;
                return false;
            }
        }
    }

    bool rebalanceRightShrunk(Node* parent, Node* grandParent) // primary
    {
        bool shrunk = false;
        Node *child, *grandChild;
        Balance child_bal, grandChild_bal;
        if (parent->balance == Balance::RightHeavy)
        {
            parent->balance = Balance::Balanced;
            return true;
        }
        else if (parent->balance == Balance::Balanced)
        {
            parent->balance = Balance::LeftHeavy;
            return false;
        }
        else
        {
            child = parent->left;
            child_bal = child->balance;
            if (child->balance != Balance::RightHeavy)
            {
                // Вращение вправо
                parent->left = child->right;
                child->right = parent;
                if (child->balance == Balance::Balanced)
                {
                    parent->balance = Balance::LeftHeavy;
                    child->balance = Balance::RightHeavy;
                    shrunk = false;
                }
                else
                {
                    parent->balance = Balance::Balanced;
                    child->balance = Balance::Balanced;
                    shrunk = true;
                }
                if (!grandParent)                           // если grandParent == nullptr, то root == parent
                    root = child;
                else if (grandParent->right && grandParent->right == parent)
                    grandParent->right = child;
                else grandParent->left = child;
                return shrunk;
            }
            else
            {
                // вращение влево-вправо
                grandChild = child->right;
                grandChild_bal = grandChild->balance;
                child->right = grandChild->left;
                grandChild->left = child;
                parent->left = grandChild->right;
                grandChild->right = parent;

                if (grandChild_bal == Balance::LeftHeavy)
                    parent->balance = Balance::RightHeavy;
                else
                    parent->balance = Balance::Balanced;
                if (grandChild_bal == Balance::RightHeavy)
                    child->balance = Balance::LeftHeavy;
                else
                    child->balance = Balance::Balanced;
                grandChild->balance = Balance::Balanced; 
                if (!grandParent)                           // если grandParent == nullptr, то root == parent
                    root = grandChild;
                else if (grandParent->right && grandParent->right == parent)
                    grandParent->right = grandChild;
                else grandParent->left = grandChild;
                return false;
            }
        }
    }

    bool replaceRightmost(Node* target_parent, Node* target, Node* repl_parent, Node* repl, Node **newParent)
    {   
        bool shrunk = false;
        Node* old_repl;
        if (!repl->right)
        {
            // repl - это узел, которым заменится искомый (target).
            // Запоминание положения узла
            old_repl = repl;
            if (repl_parent != target)
            {   // Случай, когда в левом поддереве крайний правый элемент не является дочерним для target.
                // Замена repl его левым дочерним узлом
                repl_parent->right = repl->left;
                // Замена искомого узла переменной old_repl
                old_repl->left = target->left;
            }
            else // случай, когда в левом поддерерве один элемент
                repl_parent->left = repl->left;
            // Замена target переменной old_repl
            old_repl->right = target->right;
            old_repl->balance = target->balance;
            if (!target_parent) // если target_parent == nullptr, то target == root
                root = old_repl;
            else if (target_parent->right && target_parent->right == target)
                target_parent->right = old_repl;
            else target_parent->left = old_repl;

            *newParent = old_repl;
            return true;
        }
        else
        {
            // Рассмотрение правых ветвей
            shrunk = replaceRightmost(target_parent, target, repl,  repl->right, newParent);
            if (shrunk)
                return rebalanceRightShrunk(repl, repl_parent);
        }
    }

    bool findRecursive(Node* parent, const KeyType& key)
    {
        if (key < parent->pair.first)
        {
            if (!parent->left) // если левого элемента нет (nullptr) => удаление не удалось
            {
                return false;
            }
            else
                return findRecursive(parent->left, key);
        }
        else if (key > parent->pair.first)
        {
            if (!parent->right)
            {
                return false;
            }
            else
                return findRecursive(parent->right, key);
        }
        else // if (key == parent->pair.first)
        {
            return true;
        }
    }

    ValueType getDataRecursive(Node* parent, const KeyType& key)
    {
        if (key < parent->pair.first)
        {
            if (!parent->left) // если левого элемента нет (nullptr) => удаление не удалось
            {
                return ValueType();
            }
            else
                return getDataRecursive(parent->left, key);
        }
        else if (key > parent->pair.first)
        {
            if (!parent->right)
            {
                return ValueType();
            }
            else
                return getDataRecursive(parent->right, key);
        }
        else // if (key == parent->pair.first)
        {
            return parent->pair.second;
        }
    }
};

}
