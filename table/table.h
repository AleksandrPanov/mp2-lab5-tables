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

template<typename KeyType, typename ValueType>
class BaseTable
{
public:
	class OwnIterator : public std::iterator<std::input_iterator_tag, ValueType>
	{
	public:
		OwnIterator() {}
		OwnIterator(std::pair<KeyType, ValueType>* ptr) : p(ptr) {}
		//owniterator(const owniterator& it) {}
		std::pair<KeyType, ValueType>* getPtr()
		{
			return p;
		}
		bool operator ==(OwnIterator const& other) const
		{
			return p->first == other.p->first;
		}
		bool operator !=(OwnIterator const& other) const
		{
			return p->first != other.p->first;
		}
		virtual ValueType &operator *() const
		{
			return p->second;
		}
		virtual OwnIterator &operator ++()
		{
			++p;
			return *this;
		}
		virtual OwnIterator &operator +(int index)
		{
			p += index;
			return *this;
		}
	private:
		std::pair<KeyType, ValueType>* p = nullptr;
	};
    virtual OwnIterator begin()
    {
        return OwnIterator();
    }
    virtual OwnIterator end()
    {
        return OwnIterator();
    }
    // find возвращает указатель на данные
    // если данные не найдены, то возвращается указатель равный end()
    // end() = адрес последнего элемента + 1
    virtual OwnIterator find(const KeyType& key) = 0;
    virtual OwnIterator insert(const KeyType& key, const ValueType& value) = 0;
    virtual void remove(const KeyType& key) = 0;
    virtual void remove(OwnIterator& it) {}
    virtual ValueType& operator[](const KeyType& key) = 0;
    virtual size_t getSize()
    {
        return end().getPtr() - begin().getPtr();
    }
    virtual OwnIterator getMin() { return end(); }
    virtual OwnIterator getMax() { return end(); }
};

template<typename KeyType, typename ValueType>
class SimpleTable : public BaseTable<KeyType, ValueType>
{
public:
	virtual BaseTable< KeyType, ValueType>::OwnIterator begin() override//???
    {
        if (keyData.size() == 0ull)
            return BaseTable< KeyType, ValueType>::OwnIterator(nullptr);
        return &(keyData.front());
    }
    virtual BaseTable< KeyType, ValueType>::OwnIterator end() override
    {
        if (keyData.size() == 0ull)
            return BaseTable< KeyType, ValueType>::OwnIterator(nullptr);
        return &(keyData.back()) + 1ull;
    }
	BaseTable< KeyType, ValueType>::OwnIterator find(const KeyType& key) override
    {
        for (size_t i = 0; i < keyData.size(); i++)
        {
            if (keyData[i].first == key)
                return BaseTable< KeyType, ValueType>::OwnIterator(&keyData[i]);
        }
        return BaseTable< KeyType, ValueType>::OwnIterator(&keyData.back() + 1ull);
    }
	BaseTable< KeyType, ValueType>::OwnIterator insert(const KeyType& key, const ValueType& value) override
    {
        keyData.push_back(std::make_pair(key, value));
        return BaseTable< KeyType, ValueType>::OwnIterator(&keyData.back() - 1ull);
    }
	virtual void remove(const KeyType& key) override
	{
		typename BaseTable< KeyType, ValueType>::OwnIterator iter(find(key));
		if (iter != end())
		{
			keyData.erase(keyData.begin() + (iter.getPtr() - begin().getPtr()));
		}
	}
    virtual ValueType& operator[](const KeyType& key) override
    {
		return *(find(key));
    }
	BaseTable< KeyType, ValueType>::OwnIterator get_min_by_key()
	{
		if (keyData.size() == 0ull)
			return BaseTable< KeyType, ValueType>::OwnIterator(nullptr);
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
	BaseTable< KeyType, ValueType>::OwnIterator get_max_by_key()
	{
		if (keyData.size() == 0ull)
			return BaseTable< KeyType, ValueType>::OwnIterator(nullptr);
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
private:
	std::vector<std::pair<KeyType, ValueType> > keyData;
};

template<typename KeyType, typename ValueType>
class SortTable : public SimpleTable<KeyType, ValueType> //sorted ascending
{
    std::vector<std::pair<KeyType, ValueType> > keyData;
	int binarySearch(int l, int r, const KeyType& x)
	{
		while (l <= r) {
			int m = l + (r - l) / 2;
			if (keyData[m].first == x)
				return m;
			else if (keyData[m].first < x)
				l = m + 1;
			else
				r = m - 1;
		}
		return -1;
	}
	typename std::vector<std::pair<KeyType, ValueType>>::iterator binaryInsertion(int l, int r, const KeyType &x, const ValueType &value)
	{
		while (l <= r) {
			int m = l + (r - l) / 2;
			if (keyData[m].first == x)
			{
				auto iter = keyData.begin() + m + 1;
				iter = keyData.emplace(iter, std::make_pair(x, value));
				return iter;
			}
			else if (keyData[m].first < x)
				l = m + 1;
			else if (keyData[m].first > x)
				r = m - 1;
		}
		auto iter = keyData.begin() + l;
		iter = keyData.emplace(iter, std::make_pair(x, value));
		return iter;
	}
public:
	virtual BaseTable< KeyType, ValueType>::OwnIterator begin() override
	{
		if (keyData.size() == 0ull)
			return BaseTable< KeyType, ValueType>::OwnIterator(nullptr);
		return &(keyData.front());
	}
	virtual BaseTable< KeyType, ValueType>::OwnIterator end() override
	{
		if (keyData.size() == 0ull)
			return BaseTable< KeyType, ValueType>::OwnIterator(nullptr);
		return &(keyData.back()) + 1ull;
	}
	BaseTable< KeyType, ValueType>::OwnIterator find(const KeyType& key) override
    {
		int index = binarySearch(0, keyData.size() - 1, key);
		if (index == -1)
		{
			return end();
		}
		else
		{
			return BaseTable< KeyType, ValueType>::OwnIterator(&keyData[index]);
		}
    }
	BaseTable< KeyType, ValueType>::OwnIterator insert(const KeyType& key, const ValueType& value) override
	{
		if (keyData.size() == 0)
		{
			keyData.push_back(std::make_pair(key, value));
			return begin();
		}
		auto iter = binaryInsertion(0, keyData.size() - 1, key, value);

		return BaseTable< KeyType, ValueType>::OwnIterator(&(*iter));
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
	BaseTable< KeyType, ValueType>::OwnIterator get_min_by_key()
	{
		if (keyData.size() == 0ull)
			return BaseTable< KeyType, ValueType>::OwnIterator(nullptr);
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
	BaseTable< KeyType, ValueType>::OwnIterator get_max_by_key()
	{
		if (keyData.size() == 0ull)
			return BaseTable< KeyType, ValueType>::OwnIterator(nullptr);
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
class HashTable
{
	std::vector<bool> is_occupied;
	std::vector<std::pair<KeyType, ValueType> > keyData;
	uint64_t cells_occupied;
	uint64_t max_size;
	uint64_t prime;
	uint32_t my_hash(int key)
	{
		return key % max_size;
	}
	uint32_t my_another_hash(int key)
	{
		return prime - (key % prime);
	}
public:
	class HashIterator : public std::iterator<std::input_iterator_tag, ValueType>
	{
	public:
		HashIterator() {}
		HashIterator(std::pair<KeyType, ValueType>* ptr, std::vector<bool> &is_occupied, std::pair<KeyType, ValueType> *begin) : p(ptr), is_occupied(is_occupied), begin(begin){}
		HashIterator(const HashIterator &other) = default;
		HashIterator& operator =(const HashIterator &other)
		{
			p = other.p;
			is_occupied = other.is_occupied;
			begin = other.begin;
			return *this;
		}
		std::pair<KeyType, ValueType>* getPtr()
		{
			return p;
		}
		bool operator ==(HashIterator const& other) const
		{
			return p->first == other.p->first;
		}
		bool operator !=(HashIterator const& other) const
		{
			return p->first != other.p->first;
		}
		ValueType &operator *() const
		{
			return p->second;
		}
		HashIterator &operator ++()
		{
			if (p != nullptr)
			{
				int index = p - begin;
				int size = is_occupied.size();
				do
				{
					index++;
					++p;
				} while (index != size && !is_occupied[index]);
			}
			return *this;
		}
		HashIterator &operator +(int index)
		{
			p += index;
			return *this;
		}
	private:
		std::pair<KeyType, ValueType> *p = nullptr; 
		std::pair<KeyType, ValueType> *begin = nullptr;
		std::vector<bool> &is_occupied;
	};
	HashTable()
	{
		cells_occupied = 0ull;
		max_size = 1009ull;//must be a prime number
		prime = 997ull;
		keyData.resize(max_size);
		is_occupied.resize(max_size);
		fill(is_occupied.begin(), is_occupied.end(), false);
	}
	HashTable<KeyType, ValueType>::HashIterator begin()
	{
		if (cells_occupied == 0)
			return HashTable<KeyType, ValueType>::HashIterator(nullptr, is_occupied, &(keyData[0]));
		int i = 0;
		while (!is_occupied[i])
		{
			++i;
		}
		return HashTable<KeyType, ValueType>::HashIterator(&(keyData[i]), is_occupied, &(keyData[0]));
	}
	HashTable<KeyType, ValueType>::HashIterator end()
	{
		if (keyData.size() == 0ull)
			return HashTable<KeyType, ValueType>::HashIterator(nullptr, is_occupied, &keyData[0]);
		return HashTable<KeyType, ValueType>::HashIterator(&(keyData.back()) + 1ull, is_occupied, &(keyData[0]));
	}
    HashTable<KeyType, ValueType>::HashIterator find(const KeyType& key)
    {
		if (cells_occupied == 0ull)
			return HashTable<KeyType, ValueType>::HashIterator(nullptr, is_occupied, &(keyData[0]));
		int i = 0;
		int int_key = static_cast<int>(key);
		uint32_t addr = (my_hash(int_key) + i * my_another_hash(int_key)) % max_size;
		while (is_occupied[addr] && keyData[addr].first != key)
		{
			++i;
			addr = (my_hash(int_key) + i * my_another_hash(int_key)) % max_size;
		}
		return HashTable<KeyType, ValueType>::HashIterator(&(keyData[addr]), is_occupied, &(keyData[0]));
    }
    HashTable<KeyType, ValueType>::HashIterator insert(const KeyType &key, const ValueType &value)
    {
		if (cells_occupied == max_size)//TODO add repacking after overflow
		{
			std::cout << "Table is full" << std::endl;
			return HashTable<KeyType, ValueType>::HashIterator(nullptr, is_occupied, &(keyData[0]));
		}
		int i = 0;
		int int_key = static_cast<int>(key);
		uint32_t addr = (my_hash(int_key) + i * my_another_hash(int_key)) % max_size;
		while (is_occupied[addr])
		{
			++i;
			addr = (my_hash(int_key) + i * my_another_hash(int_key)) % max_size;
		}
		is_occupied[addr] = true;
		keyData[addr].first = key;
		keyData[addr].second = value;
		cells_occupied++;
		return HashTable<KeyType, ValueType>::HashIterator(&(keyData[addr]), is_occupied, &(keyData[0]));
    }
    virtual void remove(const KeyType& key) 
    {
		auto iter = find(key);
		int index = iter.getPtr() - begin().getPtr();
		is_occupied[index] = false;
		cells_occupied--;
    }
    virtual ValueType& operator[](const KeyType& key) 
    {
		auto iter = find(key);
		int index = iter.getPtr() - &keyData[0];
		if (is_occupied[index])
		{
			return keyData[index].second;
		}
    }
	HashTable<KeyType, ValueType>::HashIterator get_min_by_key()
	{
		if (cells_occupied == 0ull)
			return HashTable<KeyType, ValueType>::HashIterator(nullptr, is_occupied, &(keyData[0]));
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
	HashTable<KeyType, ValueType>::HashIterator get_max_by_key()
	{
		if (cells_occupied == 0ull)
			return HashTable<KeyType, ValueType>::HashIterator(nullptr, is_occupied, &(keyData[0]));
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


}
