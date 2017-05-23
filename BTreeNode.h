#pragma once
#include <exception>
#include <iostream>

template <class KeyItemType, unsigned short int _deg>
class BTreeNode
{
private:
	//Поля
	KeyItemType *keys;
	BTreeNode<KeyItemType, _deg> **ptrs;
	unsigned int keys_count;
	bool leaf;

	//Конструктор
	BTreeNode(const bool _leaf);

	//Деструктор
	~BTreeNode();

	//Служебные функции
	void traverse() const;
	void splitChild(const int index, BTreeNode<KeyItemType, _deg>* y);
	void removeFromLeaf(const int index);
	void removeFromNonLeaf(const int index);
	KeyItemType getPred(const int index) const;
	KeyItemType getSucc(const int index) const;
	void fill(const int index);
	void borrowFromPrev(const int index);
	void borrowFromNext(const int index);
	void merge(const int index);

	//Функции интерфейса для класса BTree
	void insertNonFull(const KeyItemType& item);
	void remove(const KeyItemType& item);

	//Обьявление дружественного класса
	template <class KeyItemType, unsigned short int _deg> friend class BTree;
};

template <class KeyItemType, unsigned short int _deg>
inline BTreeNode<KeyItemType, _deg>::BTreeNode(const bool _leaf) :
	keys(new KeyItemType[(_deg << 1u) - 1u]),
	leaf(_leaf),
	keys_count(0u),
	ptrs(new BTreeNode<KeyItemType, _deg>*[_deg << 1u])
{
	for (unsigned int i = 0u; i < (_deg << 1u); ++i)
		ptrs[i] = 0;
}

template <class KeyItemType, unsigned short int _deg>
inline BTreeNode<KeyItemType, _deg>::~BTreeNode()
{
	delete[]keys;
	keys = 0;
	delete[]ptrs;
	ptrs = 0;
}

template <class KeyItemType, unsigned short int _deg>
inline void BTreeNode<KeyItemType, _deg>::remove(const KeyItemType& item)
{
	unsigned int i = 0u;
	while (item > keys[i] && i < keys_count)
		++i;
	if (i < keys_count && keys[i] == item)
		if (leaf)
			removeFromLeaf(i);
		else
			removeFromNonLeaf(i);
	else
	{
		if (leaf)
			throw std::exception("BTreeNode::remove: item doesn't exist in the tree");
		bool flag = (i == keys_count) ? 1 : 0;
		if (ptrs[i]->keys_count < _deg)
			fill(i);
		if (flag && i > keys_count)
			ptrs[i - 1u]->remove(item);
		else
			ptrs[i]->remove(item);
	}
}

template <class KeyItemType, unsigned short int _deg>
inline void BTreeNode<KeyItemType, _deg>::removeFromLeaf(const int index)
{
	for (int i = index + 1; i < keys_count; ++i)
		keys[i - 1] = keys[i];
	--keys_count;
}

template <class KeyItemType, unsigned short int _deg>
inline void BTreeNode<KeyItemType, _deg>::removeFromNonLeaf(const int index)
{

	KeyItemType key = keys[index];
	if (ptrs[index]->keys_count >= _deg)
	{
		KeyItemType pred = getPred(index);
		keys[index] = pred;
		ptrs[index]->remove(pred);
	}
	else
		if (ptrs[index + 1]->keys_count >= _deg)
		{
			KeyItemType succ = getSucc(index);
			keys[index] = succ;
			ptrs[index + 1]->remove(succ);
		}
		else
		{
			merge(index);
			ptrs[index]->remove(key);
		}
}

template <class KeyItemType, unsigned short int _deg>
inline KeyItemType BTreeNode<KeyItemType, _deg>::getPred(const int index) const
{
	BTreeNode<KeyItemType, _deg> *cur = ptrs[index];
	while (!cur->leaf)
		cur = cur->ptrs[cur->keys_count];
	return cur->keys[cur->keys_count - 1u];
}

template <class KeyItemType, unsigned short int _deg>
inline KeyItemType BTreeNode<KeyItemType, _deg>::getSucc(const int index) const
{
	BTreeNode *cur = ptrs[index + 1];
	while (!cur->leaf)
		cur = cur->ptrs[0];
	return cur->keys[0];
}

template <class KeyItemType, unsigned short int _deg>
inline void BTreeNode<KeyItemType, _deg>::fill(const int index)
{
	if (index && ptrs[index - 1]->keys_count >= _deg)
		borrowFromPrev(index);
	else
		if (index != keys_count && ptrs[index + 1]->keys_count >= _deg)
			borrowFromNext(index);
		else
			if (index != keys_count)
				merge(index);
			else
				merge(index - 1);
}

template <class KeyItemType, unsigned short int _deg>
inline void BTreeNode<KeyItemType, _deg>::borrowFromPrev(const int index)
{
	BTreeNode<KeyItemType, _deg>
		*child = ptrs[index],
		*sibling = ptrs[index - 1];
	for (int i = child->keys_count - 1u; i >= 0; --i)
		child->keys[i + 1] = child->keys[i];
	if (!child->leaf)
		for (int i = child->keys_count; i >= 0; --i)
			child->ptrs[i + 1] = child->ptrs[i];
	child->keys[0] = keys[index - 1];
	if (!leaf)
		child->ptrs[0] = sibling->ptrs[sibling->keys_count];
	keys[index - 1] = sibling->keys[sibling->keys_count - 1];
	++child->keys_count;
	--sibling->keys_count;
}

template <class KeyItemType, unsigned short int _deg>
inline void BTreeNode<KeyItemType, _deg>::borrowFromNext(const int index)
{
	BTreeNode<KeyItemType, _deg>
		*child = ptrs[index],
		*sibling = ptrs[index + 1];
	child->keys[child->keys_count] = keys[index];
	if (!child->leaf)
		child->ptrs[child->keys_count + 1] = sibling->ptrs[0];
	keys[index] = sibling->keys[0];
	for (int i = 1; i < sibling->keys_count; ++i)
		sibling->keys[i - 1] = sibling->keys[i];
	if (!sibling->leaf)
		for (int i = 1; i <= sibling->keys_count; ++i)
			sibling->ptrs[i - 1] = sibling->ptrs[i];
	++child->keys_count;
	--sibling->keys_count;
}

template <class KeyItemType, unsigned short int _deg>
inline void BTreeNode<KeyItemType, _deg>::merge(const int index)
{
	BTreeNode<KeyItemType, _deg>
		*child = ptrs[index],
		*sibling = ptrs[index + 1];
	child->keys[_deg - 1] = keys[index];
	for (int i = 0; i < sibling->keys_count; ++i)
		child->keys[i + _deg] = sibling->keys[i];
	if (!child->leaf)
		for (unsigned int i = 0u; i <= sibling->keys_count; ++i)
			child->ptrs[i + _deg] = sibling->ptrs[i];
	for (int i = index + 1; i < keys_count; ++i)
		keys[i - 1] = keys[i];
	for (int i = index + 2; i <= keys_count; ++i)
		ptrs[i - 1] = ptrs[i];
	child->keys_count += sibling->keys_count + 1u;
	--keys_count;
	delete sibling;
	sibling = 0;
}

template <class KeyItemType, unsigned short int _deg>
inline void BTreeNode<KeyItemType, _deg>::insertNonFull(const KeyItemType& item)
{
	int i = keys_count - 1u;
	if (leaf)
	{
		while (item < keys[i] && i >= 0)
		{
			keys[i + 1] = keys[i];
			--i;
		}
		keys[i + 1] = item;
		++keys_count;
	}
	else
	{
		while (item < keys[i] && i >= 0)
			--i;
		if (ptrs[i + 1]->keys_count == (_deg << 1u) - 1u)
		{
			splitChild(i + 1, ptrs[i + 1]);
			if (keys[i + 1] < item)
				++i;
		}
		ptrs[i + 1]->insertNonFull(item);
	}
}

template <class KeyItemType, unsigned short int _deg>
inline void BTreeNode<KeyItemType, _deg>::splitChild(const int index, BTreeNode<KeyItemType, _deg>* y)
{
	BTreeNode<KeyItemType, _deg> *z = new BTreeNode<KeyItemType, _deg>(y->leaf);
	z->keys_count = _deg - 1u;
	for (unsigned int i = 0u; i < _deg - 1u; ++i)
		z->keys[i] = y->keys[i + _deg];
	if (!y->leaf)
		for (unsigned int i = 0u; i < _deg; ++i)
			z->ptrs[i] = y->ptrs[i + _deg];
	y->keys_count = _deg - 1u;
	for (int i = keys_count; i >= index + 1; --i)
		ptrs[i + 1u] = ptrs[i];
	ptrs[index + 1] = z;
	for (int i = keys_count - 1; i >= index; --i)
		keys[i + 1] = keys[i];
	keys[index] = y->keys[_deg - 1u];
	++keys_count;
}

template <class KeyItemType, unsigned short int _deg>
inline void BTreeNode<KeyItemType, _deg>::traverse() const
{
	unsigned int i = 0u;
	if (leaf)
		while (i < keys_count)
			std::cout << ' ' << keys[i++];
	else
	{
		while (i < keys_count)
		{
			ptrs[i]->traverse();
			std::cout << ' ' << keys[i++];
		}
		ptrs[i]->traverse();
	}
}