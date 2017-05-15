#pragma once
#include <exception>

#define MAX_KEYS ((_deg << 1u) - 1u) // == (_deg * 2 - 1)
#define MIN_KEYS (_deg - 1u)

template <class KeyItemType, unsigned short int _deg>
class BTreeNode
{
private:
	BTreeNode<KeyItemType, _deg> *parentPtr, **ptrs;
	KeyItemType *keys;
	unsigned int keys_count;

	//Конструкторы и деструктор
	BTreeNode() : parentPtr(0), ptrs(0), keys_count(0u), keys(new KeyItemType[MAX_KEYS])
	{
		if (!keys)
			throw std::exception("CANNOT ALLOCATE MEMORY");
	}

	BTreeNode(const KeyItemType& item) : parentPtr(0), keys_count(1u), ptrs(0), keys(new KeyItemType[MAX_KEYS])
	{
		if (keys)
			keys[0] = item;
		else
			throw std::exception("CANNOT ALLOCATE MEMORY");
	}

	BTreeNode(const KeyItemType& item, BTreeNode<KeyItemType, _deg>* const leftPtr, BTreeNode<KeyItemType, _deg>* const rightPtr) : parentPtr(0), keys_count(1u), keys(new KeyItemType[MAX_KEYS]), ptrs(new BTreeNode<KeyItemType, _deg>*[MAX_KEYS + 1])
	{
		if (keys && ptrs)
		{
			keys[0] = item;
			ptrs[0] = leftPtr;
			ptrs[1] = rightPtr;
		}
		else
			throw std::exception("CANNOT ALLOCATE MEMORY");
	}

	~BTreeNode()
	{
		delete[]keys;
		keys = 0;
		if (ptrs)
		{
			delete[]ptrs;
			ptrs = 0;
		}
	}

	//Обьявление дружественного класса
	template <class KeyItemType, unsigned short int _deg> friend class BTree;
};