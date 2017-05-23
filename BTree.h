#pragma once

#include "BTreeNode.h"

template <class KeyItemType, unsigned short int _deg>
class BTree
{
private:
	//Поля
	BTreeNode<KeyItemType, _deg> *root;

	//Служебные функции
	void destroy_tree(BTreeNode<KeyItemType, _deg>*& nodePtr);
	bool isExist_helper(BTreeNode<KeyItemType, _deg>* const nodePtr, const KeyItemType& item) const;
public:
	//Конструктор
	BTree();
	//Деструктор
	~BTree();

	//Функции интерфейса
	void traverse();
	void insert(const KeyItemType& item);
	void remove(const KeyItemType& item);
	bool isExist(const KeyItemType& item) const;
};

template <class KeyItemType, unsigned short int _deg>
inline BTree<KeyItemType, _deg>::BTree() : root(0) {}

template <class KeyItemType, unsigned short int _deg>
inline void BTree<KeyItemType, _deg>::destroy_tree(BTreeNode<KeyItemType, _deg>*& nodePtr)
{
	if (nodePtr)
	{
		if (!nodePtr->leaf)
			for (unsigned int i = 0u; i <= nodePtr->keys_count; ++i)
				destroy_tree(nodePtr->ptrs[i]);
		delete nodePtr;
		nodePtr = 0;
	}
}

template <class KeyItemType, unsigned short int _deg>
inline BTree<KeyItemType, _deg>::~BTree()
{
	destroy_tree(root);
}

template <class KeyItemType, unsigned short int _deg>
inline void BTree<KeyItemType, _deg>::traverse()
{
	if (root)
		root->traverse();
}

template <class KeyItemType, unsigned short int _deg>
inline void BTree<KeyItemType, _deg>::insert(const KeyItemType& item)
{
	if (root)
	{
		if (root->keys_count < (_deg << 1u) - 1u)
			root->insertNonFull(item);
		else
		{
			BTreeNode<KeyItemType, _deg> *s = new BTreeNode<KeyItemType, _deg>(0);
			s->ptrs[0] = root;
			s->splitChild(0u, root);
			unsigned int i = 0u;
			if (s->keys[0] < item)
				i++;
			s->ptrs[i]->insertNonFull(item);
			root = s;
		}
	}
	else
	{
		root = new BTreeNode<KeyItemType, _deg>(1);
		root->keys[0] = item;
		root->keys_count = 1u;
	}
	std::cout << item << " inserted" << std::endl;
}

template <class KeyItemType, unsigned short int _deg>
inline void BTree<KeyItemType, _deg>::remove(const KeyItemType& item)
{
	if (!root)
		throw std::exception("BTree::remove: root ptr is NULL");
	root->remove(item);
	if (root->keys_count == 0u)
	{
		BTreeNode<KeyItemType, _deg> *tmp = root;
		if (root->leaf)
			root = 0;
		else
			root = root->ptrs[0];
		delete tmp;
		tmp = 0;
	}
}

template <class KeyItemType, unsigned short int _deg>
inline bool BTree<KeyItemType, _deg>::isExist_helper(BTreeNode<KeyItemType, _deg>* const nodePtr, const KeyItemType& item) const
{
	unsigned int i = 0u;
	while (item > nodePtr->keys[i] && i < nodePtr->keys_count)
		++i;
	if (i < nodePtr->keys_count && nodePtr->keys[i] == item)
		return 1;
	else
		if (nodePtr->leaf)
			return 0;
		else
			return isExist_helper(nodePtr->ptrs[i], item);
}

template <class KeyItemType, unsigned short int _deg>
inline bool BTree<KeyItemType, _deg>::isExist(const KeyItemType & item) const
{
	return isExist_helper(root, item);
}