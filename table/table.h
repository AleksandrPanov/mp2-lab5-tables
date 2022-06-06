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
        virtual void remove(const KeyType& key) override
        {
            OwnIterator<KeyType, ValueType> it = find(key);
            keyData.erase(std::remove(keyData.begin(), keyData.end(), *it.getPtr()));
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
            size_t l = 0;
            size_t r = keyData.size() - 1;
            while (l <= r)
            {
                size_t c = (l + r) / 2;
                if (keyData[c].first == key)
                    return OwnIterator<KeyType, ValueType>(&keyData[0] + c);
                if (keyData[c].first < key)
                    l = c + 1;
                else
                    r = c - 1;
            }
            return OwnIterator<KeyType, ValueType>(&keyData[0] + keyData.size());
        }
        OwnIterator<KeyType, ValueType> insert(const KeyType& key, const ValueType& value) override
        {
            size_t l = 0;
            size_t r = keyData.size() - 1;
            size_t c = 0;
            while (l <= r)
            {
                c = (r - l) / 2 + l;
                if (keyData[c].first == key)
                    throw std::runtime_error("Insert error");
                if (keyData[c].first < key)
                    l = c + 1;
                else
                    r = c - 1;
            }

            if (left == keyData.size())
            {
                keyData.push_back(std::make_pair(key, value));
                return OwnIterator<KeyType, ValueType>(&keyData.back() - 1);
            }
            else
            {
                auto it = keyData[l + 1];
                keyData.insert(it, std::make_pair(key, value));
                return OwnIterator<KeyType, ValueType>(&keyData[l] - 1);
            }
        }
        virtual void remove(const KeyType& key) override
        {
            OwnIterator<KeyType, ValueType> it = find(key);
            keyData.erase(std::remove(keyData.begin(), keyData.end(), *it.getPtr()));
        }
        virtual ValueType& operator[](const KeyType& key) override
        {
            return *(find(key));
        }
    };

    template<typename KeyType, typename ValueType>
    class HashTable : public BaseTable<KeyType, ValueType>
    {
    private:
        using Table = SortTable<KeyType, ValueType>;
        static constexpr size_t default_size = 128;

        std::hash<KeyType> hasher_;
        std::vector<Table> table_;

        typename std::vector<Table>::iterator getTable(KeyType const& key)
        {
            std::size_t TableIndex = hasher_(key) % table_.size();
            return table_.begin() + TableIndex;
        }

    public:
        template<class KeyType, class ValueType>
        class HashTableIterator : public std::iterator<std::input_iterator_tag, ValueType>
        {
            friend class HashTable<KeyType, ValueType>;

        private:

            std::vector<Table>& table_;
            typename std::vector<Table>::iterator itTable_;
            OwnIterator<KeyType, ValueType> itPair_;

            HashTableIterator(std::vector<Table>& table, typename std::vector<Table>::iterator itTable, OwnIterator<KeyType, ValueType> it)
            {
                this->table_ = table;
                this->itTable_ = itTable;
                this->itPair_ = it;
            }

        public:
            OwnIterator<KeyType, ValueType>& getPairIterator()
            {
                return itPair_;
            }

            std::pair<KeyType, ValueType>* getPtr()
            {
                return itPair_->getPtr();
            }

            HashTableIterator::reference operator*() const
            {
                return *itPair_;
            }

            bool operator==(HashTableIterator const& other) const
            {
                if (itPair_ == other.itPair_)
                    return true;
                else
                    return false;
            }

            bool operator!=(HashTableIterator const& other) const
            {
                if (itPair_ != other.itPair_)
                    return true;
                else
                    return false;
            }

            virtual HashTableIterator& operator++()
            {
                auto pos = itTable_;
                auto iterator = itPair_;
                if (++iterator == table_[pos].end())
                {
                    while (table_.size() != pos + 1 && iterator == table_[pos].end())
                    {
                        iterator = table_[++pos].begin();
                    }

                    if (table_.size() == pos + 1)
                    {
                        return *this;
                    }
                }
                itTable_ = pos;
                itPair_ = iterator;
                return *this;
            }

            virtual HashTableIterator& operator+(int index)
            {
                for (int i = 0; i < index; i++)
                    this->operator++();
                return *this;
            }
        };

        HashTable(size_t numTable = default_size) : table_(numTable)
        { }

        HashTableIterator<KeyType, ValueType> begin()
        {
            for (auto itTable = table_.begin(); itTable != table_.end(); itTable++)
            {
                if (itTable->begin() != itTable->end())
                {
                    return HashTableIterator<KeyType, ValueType>(table_, itTable, itTable->begin());
                }
            }
            return HashTableIterator<KeyType, ValueType>(table_, table_.begin(), table_.begin()->begin());
        }

        HashTableIterator<KeyType, ValueType> end()
        {
            auto end = table_.begin();
            for (auto itTable = table_.begin(); itTable != table_.end(); itTable++)
            {
                if (itTable->begin() != itTable->end())
                {
                    end = itTable;
                }
            }
            return HashTableIterator<KeyType, ValueType>(table_, end, end->end());
        }

        HashTableIterator<KeyType, ValueType> find(const KeyType& key)
        {
            auto itTable = getTable(key);
            auto itPair = itTable->find(key);
            if (itPair == itTable->end())
            {
                return end();
            }
            return HashTableIterator<KeyType, ValueType>(table_, itTable, itPair);
        }

        HashTableIterator<KeyType, ValueType> add(const KeyType& key, const ValueType& value)
        {
            auto itTable = getTable(key);
            auto itPair = itTable->insert(key, value);
            return HashTableIterator<KeyType, ValueType>(table_, itTable, itPair);
        }

        void remove(const KeyType& key)
        {
            auto itTable = getTable(key);
            itTable->remove(key);
        }

        void remove(HashTableIterator<KeyType, ValueType>& it)
        {
            auto itTable = getTable(it.getPtr()->first());
            auto itPair = it.getPairIterator();
            *itTable->remove(itPair);
        }

        ValueType& operator[](const KeyType& key)
        {
            return *find(key);
        }
    };

    template<typename KeyType, typename ValueType>
    class AvlTable : public BaseTable<KeyType, ValueType>
    {
        struct Node
        {
            std::pair<KeyType, ValueType> pair;
            Node* left;
            Node* right;
            Node(std::pair<KeyType, ValueType> _pair)
            {
                pair = _pair;
                left = nullptr;
                right = nullptr;
            }
            ~Node()
            {
                delete left;
                delete right;
            }
        };

        Node* root;
        size_t size = 0;

        Node* insert(Node* node, const KeyType& key, const ValueType& value)
        {
            if (!node)
            {
                return new Node(std::make_pair(key, value));
            }
            else
            {
                if (key < node->pair.first)
                {
                    node->left = insert(node->left, key, value);
                }
                else
                {
                    node->right = insert(node->right, key, value);
                }
            }
            return node;
        }

        Node* find(Node* node, const KeyType& key)
        {
            Node* ans;

            if (!node)
            {
                return nullptr;
            }

            if (key == node->pair.first)
            {
                return node;
            }

            if (key < node->pair.first)
            {
                ans = find(node->left, key);
            }
            else
            {
                ans = find(node->right, key);
            }
            return ans;
        }

        Node* remove(Node* node, const KeyType& key)
        {
            if (!node)
            {
                return nullptr;
            }
            if (key < node->pair.first)
            {
                node->left = remove(node->left, key);
                return node;
            }
            else if (key > node->pair.first)
            {
                node->right = remove(node->right, key);
                return node;
            }
            else
            {
                if (!node->left && !node->right)
                {
                    return nullptr;
                }
                if (!node->left)
                {
                    return node->right;
                }
                if (!node->right)
                {
                    return node->left;
                }
                Node* minRight = node->right;
                while (minRight->left)
                {
                    minRight = minRight->left;
                }
                node->pair = minRight->pair;
                node->right = remove(node->right, node->pair.first);
                return node;
            }
        }

        void printLeft(Node* node)
        {
            if (node)
            {
                printLeft(node->left);
                std::cout << node->pair.first << " " << node->pair.second << std::endl;
                printLeft(node->right);
            }
        }
    public:
        AvlTable()
        {
            root = nullptr;
            size = 0;
        }

        virtual size_t getSize() override
        {
            return size;
        }

        virtual OwnIterator<KeyType, ValueType> find(const KeyType& key) override
        {
            auto ans = find(root, key);
            if (ans == nullptr)
            {
                return this->end();
            }
            return OwnIterator<KeyType, ValueType>(&ans->pair);
        }

        virtual OwnIterator<KeyType, ValueType> insert(const KeyType& key, const ValueType& value)
        {
            root = insert(root, key, value);
            auto ans = std::make_pair(key, value);
            size++;
            return OwnIterator<KeyType, ValueType>(&ans);
        }

        virtual void remove(const KeyType& key) override
        {
            root = remove(root, key);
        }

        virtual ValueType& operator[](const KeyType& key) override
        {
            auto ans = find(root, key);
            return ans->pair.second;
        }

        void print()
        {
            printLeft(root);
        }

        virtual OwnIterator<KeyType, ValueType> getMax() override
        {
            if (!root)
            {
                return this->begin();
            }
            Node* current = root;
            while (current->right)
            {
                current = current->right;
            }
            return OwnIterator<KeyType, ValueType>(&current->pair);
        }

        virtual OwnIterator<KeyType, ValueType> getMin() override
        {
            if (!root)
            {
                return this->begin();
            }
            Node* current = root;
            while (current->left)
            {
                current = current->left;
            }
            return OwnIterator<KeyType, ValueType>(&current->pair);
        }
    };
}