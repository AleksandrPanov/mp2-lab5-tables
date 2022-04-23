#pragma once
#include <vector>
#include <chrono>
#include <algorithm>
namespace my_tables{

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
        return p->first == other.p->first;
    }
    bool operator!=(OwnIterator const& other) const
    {
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

    virtual OwnIterator<KeyType, ValueType> getMin() { return end(); }
    virtual OwnIterator<KeyType, ValueType> getMax() { return end(); }
};

template<typename KeyType, typename ValueType>
class SimpleTable : public BaseTable<KeyType, ValueType>
{
    std::vector<std::pair<KeyType, ValueType> > keyData;
public:
    virtual OwnIterator<KeyType, ValueType> begin() override//???
    {
        if (keyData.size() == 0ull)
            return OwnIterator<KeyType, ValueType>(nullptr);
        return &(keyData.front());
    }
    virtual OwnIterator<KeyType, ValueType> end() override
    {
        if (keyData.size() == 0ull)
            return OwnIterator<KeyType, ValueType>(nullptr);
        return &(keyData.back()) + 1ull;
    }
    OwnIterator<KeyType, ValueType> find(const KeyType& key) override
    {
        for (size_t i = 0; i < keyData.size(); i++)
        {
            if (keyData[i].first == key)
                return OwnIterator<KeyType, ValueType>(&keyData[i]);
        }
        return OwnIterator<KeyType, ValueType>(&keyData.back() + 1ull);
    }
    OwnIterator<KeyType, ValueType> insert(const KeyType& key, const ValueType& value) override
    {
        keyData.push_back(std::make_pair(key, value));
        return OwnIterator<KeyType, ValueType>(&keyData.back() - 1ull);
    }
	virtual void remove(const KeyType& key) override
	{
		OwnIterator<KeyType, ValueType> iter(find(key));
		if (iter != end())
		{
			keyData.erase(keyData.begin() + (iter.getPtr() - begin().getPtr()));
		}
	}
    virtual ValueType& operator[](const KeyType& key) override
    {
		return *(find(key));
    }
	OwnIterator<KeyType, ValueType> get_min_by_key()
	{
		if (keyData.size() == 0ull)
			return OwnIterator<KeyType, ValueType>(nullptr);
		auto iter = begin();
		auto min = iter;
		while (iter != end())
		{
			if (iter.getPtr()->first < min.getPtr()->first)
				min = iter;
			++iter;
		}
		return min;
	}
	OwnIterator<KeyType, ValueType> get_max_by_key()
	{
		if (keyData.size() == 0ull)
			return OwnIterator<KeyType, ValueType>(nullptr);
		auto iter = begin();
		auto max = iter;
		while (iter != end())
		{
			if (iter.getPtr()->first > max.getPtr()->first)
				max = iter;
			++iter;
		}
		return max;
	}
};

template<typename KeyType, typename ValueType>
class SortTable : public SimpleTable<KeyType, ValueType>
{
    std::vector<std::pair<KeyType, ValueType> > keyData;
	int binarySearch(int l, int r, int x)
	{
		while (l <= r) {
			int m = l + (r - l) / 2;
			if (keyData[m] == x)
				return m;
			if (keyData[m] < x)
				l = m + 1;
			else
				r = m - 1;
		}
		return -1;
	}
public:
	virtual OwnIterator<KeyType, ValueType> begin() override//???
	{
		if (keyData.size() == 0ull)
			return OwnIterator<KeyType, ValueType>(nullptr);
		return &(keyData.front());
	}
	virtual OwnIterator<KeyType, ValueType> end() override
	{
		if (keyData.size() == 0ull)
			return OwnIterator<KeyType, ValueType>(nullptr);
		return &(keyData.back()) + 1ull;
	}
    OwnIterator<KeyType, ValueType> find(const KeyType& key) override
    {
		int index = binarySearch(0, keyData.size() - 1, key);
		if (index == -1)
		{
			return end();
		}
		else
		{
			return OwnIterator<KeyType, ValueType>(&keyData[index]);
		}
    }
    OwnIterator<KeyType, ValueType> insert(const KeyType& key, const ValueType& value) override
    {
		auto iter = keyData.insert(std::upper_bound(keyData.begin(), keyData.end(), key));
        return OwnIterator<KeyType, ValueType>(&(*iter));
    }
    virtual void remove(const KeyType& key) override
    {
		int index = binarySearch(0, keyData.size() - 1, key);
		if (index != -1)
			keyData.erase(keyData.begin() + index);
    }
    virtual ValueType& operator[](const KeyType& key) override
    {
        return *(find(key));
    }
};

template<typename KeyType, typename ValueType>
class HashTable : public BaseTable<KeyType, ValueType>
{
	std::vector<bool> is_occupied;
	std::vector<std::pair<KeyType, ValueType> > keyData;
	int max_size = 1009; //должно быть простым 
	uint32_t my_hash(int key)
	{
		return key % max_size;
	}
	uint32_t my_another_hash(int key)
	{
		return key % (max_size - 1) + 1;
	}
public:
	HashTable()
	{
		keyData.resize(max_size);
		is_occupied.resize(max_size);
		fill(is_occupied.begin(), is_occupied.end(), false);
	}
    virtual OwnIterator<KeyType, ValueType>& find(const KeyType& key) override//needs check for emptiness
    {
		int i = 0;
		uint32_t addr = (my_hash(key) + i * my_another_hash(key)) % max_size;
		while (is_occupied[addr] && keyData[addr].first != key)
		{
			++i;
			addr = (my_hash(key) + i * my_another_hash(key)) % max_size;
		}
		return OwnIterator<KeyType, ValueType>(keyData[addr]);
    }
    virtual OwnIterator<KeyType, ValueType>& add(const KeyType& key, const ValueType& value) override//needs collision solving
    {
		int i = 0;
		uint32_t addr = (my_hash(key) + i * my_another_hash(key)) % max_size;
		while (is_occupied[addr])
		{
			++i;
			addr = (my_hash(key) + i * my_another_hash(key)) % max_size;
		}
		is_occupied[addr] = true;
		keyData[addr].first = key;
		keyData[addr].second = value;
    }
    virtual void remove(const KeyType& key) override
    {
		auto iter = find(key);
		int index = iter.getPtr() - begin();
		is_occupied[index] = false;
    }
    virtual ValueType& operator[](const KeyType& key) override
    {
		auto iter = find(key);
		int index = iter.getPtr() - begin();
		if (is_occupied[index])
		{
			return keyData[index].second;
		}
    }
	virtual OwnIterator<KeyType, ValueType> begin() override
	{
		if (keyData.size() == 0ull)
			return OwnIterator<KeyType, ValueType>(nullptr);
		return &(keyData.front());
	}
	virtual OwnIterator<KeyType, ValueType> end() override
	{
		if (keyData.size() == 0ull)
			return OwnIterator<KeyType, ValueType>(nullptr);
		return &(keyData.back()) + 1ull;
	}
};

template<typename KeyType, typename ValueType>
class BinarySearchTable : public BaseTable<KeyType, ValueType>
{
public:
	OwnIterator<KeyType, ValueType> find(const KeyType& key) override
	{
		return nullptr;
	}
	OwnIterator<KeyType, ValueType> remove(const KeyType& key) override
	{
		return nullptr;
	}
	OwnIterator<KeyType, ValueType> insert(const KeyType& key) override
	{
		return nullptr;
	}
	/*OwnIterator<KeyType, ValueType>::OwnIterator& operator++() = delete;
	OwnIterator<KeyType, ValueType>::OwnIterator& operator+() = delete;*/
private:
	class TNode {
		//TNode(int key) : Key(key), Left(nullptr), Right(nullptr)
		//{
		//}
		KeyType Key;
		ValueType Value;
		TNode* parent = nullptr;
		TNode* Left = nullptr;
		TNode* Right = nullptr;
	};
	TNode root = nullptr;
};

}
