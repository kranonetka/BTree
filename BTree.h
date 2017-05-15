#pragma once
#include "BTreeNode.h"

template <class KeyItemType, unsigned short int _deg>
class _BTree_itemNptrs
{
private:
	KeyItemType retItem;
	BTreeNode<KeyItemType, _deg> *leftPtr, *rightPtr;
	_BTree_itemNptrs() : leftPtr(0) {};
	_BTree_itemNptrs(const _BTree_itemNptrs<KeyItemType, _deg>& src) : retItem(src.retItem), leftPtr(src.leftPtr), rightPtr(src.rightPtr) {};
	_BTree_itemNptrs(const KeyItemType& _item, BTreeNode<KeyItemType, _deg>* const _leftPtr, BTreeNode<KeyItemType, _deg>* const _rightPtr) :
		retItem(_item), leftPtr(_leftPtr), rightPtr(_rightPtr) {};
	template <class KeyItemType, unsigned short int _deg> friend class BTree;
};

template <class KeyItemType, unsigned short int _deg>
class BTree
{
private:
	BTreeNode<KeyItemType, _deg> *root;
	_BTree_itemNptrs<KeyItemType, _deg> insert_search_helper(BTreeNode<KeyItemType, _deg>*& nodePtr, const KeyItemType& item);
	_BTree_itemNptrs<KeyItemType, _deg> insert_leaf_helper(BTreeNode<KeyItemType, _deg>*& nodePtr, const KeyItemType& item);
	bool isExist_helper(BTreeNode<KeyItemType, _deg>* const nodePtr, const KeyItemType& item);
	void destroy_tree(BTreeNode<KeyItemType, _deg>*& nodePtr);
	void erase_helper(BTreeNode<KeyItemType, _deg>*& const nodePtr, const KeyItemType& item);
public:
	BTree() : root(0) {};
	BTree(const KeyItemType& item) : root(new BTreeNode<KeyItemType, _deg>(item)) {};
	~BTree();
	void insert(const KeyItemType& item);
	bool isExist(const KeyItemType& item);
	void erase(const KeyItemType& item);
};

//Деструктор
template <class KeyItemType, unsigned short int _deg>
inline BTree<KeyItemType, _deg>::~BTree()
{
	destroy_tree(root);
}

template <class KeyItemType, unsigned short int _deg>
inline void BTree<KeyItemType, _deg>::destroy_tree(BTreeNode<KeyItemType, _deg>*& nodePtr)
{
	if (nodePtr)
	{
		if (nodePtr->ptrs)
		{
			for (unsigned int i = 0u; i <= nodePtr->keys_count; ++i)
				destroy_tree(nodePtr->ptrs[i]);
		}
		delete nodePtr;
		nodePtr = 0;
	}
}

//Вставка
template<class KeyItemType, unsigned short int _deg>
inline _BTree_itemNptrs<KeyItemType, _deg> BTree<KeyItemType, _deg>::insert_leaf_helper(BTreeNode<KeyItemType, _deg>*& nodePtr, const KeyItemType& item)
{
	unsigned int i = 0u;
	if (nodePtr->keys_count < MAX_KEYS) //Если лист не забит ключами
	{
		unsigned int j = nodePtr->keys_count;
		while (item >= nodePtr->keys[i] && i < nodePtr->keys_count)
			++i;
		while (i < j)
			nodePtr->keys[j--] = nodePtr->keys[j - 1u];
		nodePtr->keys[i] = item;
		++nodePtr->keys_count;
		return _BTree_itemNptrs<KeyItemType, _deg>();
	}
	else //Если лист забит ключами
	{
		unsigned int j = 0u;
		KeyItemType *temp_keys = new KeyItemType[MAX_KEYS + 1u];
		BTreeNode<KeyItemType, _deg>
			*leftPtr = new BTreeNode<KeyItemType, _deg>,
			*rightPtr = new BTreeNode<KeyItemType, _deg>;
		while (item >= nodePtr->keys[i] && i < nodePtr->keys_count)
			temp_keys[j++] = nodePtr->keys[i++];
		temp_keys[j++] = item;
		while (i < nodePtr->keys_count)
			temp_keys[j++] = nodePtr->keys[i++];
		i = j = 0u;
		while (i < _deg)
			leftPtr->keys[i++] = temp_keys[i];
		leftPtr->keys_count = _deg;
		++i;
		while (i <= MAX_KEYS)
			rightPtr->keys[j++] = temp_keys[i++];
		rightPtr->keys_count = _deg - 1u;
		_BTree_itemNptrs<KeyItemType, _deg> ret_struct(temp_keys[_deg], leftPtr, rightPtr);
		delete[] temp_keys;
		temp_keys = 0;
		delete nodePtr;
		nodePtr = 0;
		return ret_struct;
	}
}

template<class KeyItemType, unsigned short int _deg>
inline _BTree_itemNptrs<KeyItemType, _deg> BTree<KeyItemType, _deg>::insert_search_helper(BTreeNode<KeyItemType, _deg>*& nodePtr, const KeyItemType& item)
{
	unsigned int i = 0u;
	while (item >= nodePtr->keys[i] && i < nodePtr->keys_count) //Ищем, куда продолжить поиски
		++i;
	_BTree_itemNptrs<KeyItemType, _deg> result = (nodePtr->ptrs[i]->ptrs) ? insert_search_helper(nodePtr->ptrs[i], item) : insert_leaf_helper(nodePtr->ptrs[i], item);
	if (result.leftPtr) //Если с нижних уровней что-то вернулось и это нужно вставить в текущий узел
	{
		i = 0u;
		if (nodePtr->keys_count < MAX_KEYS) //Если текущий узел не забит ключами
		{
			unsigned int j = nodePtr->keys_count;
			while (result.retItem >= nodePtr->keys[i] && i < nodePtr->keys_count) //Ищем, где должно находиться возвращенное значение
				++i;
			while (i < j) //Сдвигаем ключи и указатели
			{
				nodePtr->ptrs[j + 1u] = nodePtr->ptrs[j];
				nodePtr->keys[j--] = nodePtr->keys[j - 1u];
			}
			nodePtr->keys[i] = result.retItem;
			nodePtr->ptrs[i] = result.leftPtr;
			result.leftPtr->parentPtr = nodePtr;
			nodePtr->ptrs[i + 1u] = result.rightPtr;
			result.rightPtr->parentPtr = nodePtr;
			++nodePtr->keys_count;
			return _BTree_itemNptrs<KeyItemType, _deg>();
		}
		else //Если текущий узел забит ключами и его надо разбить
		{
			unsigned int j = 0u;
			KeyItemType *temp_keys = new KeyItemType[MAX_KEYS + 1u];
			BTreeNode<KeyItemType, _deg>
				**temp_ptrs = new BTreeNode<KeyItemType, _deg>*[MAX_KEYS + 2u],
				*leftPtr = new BTreeNode<KeyItemType, _deg>,
				*rightPtr = new BTreeNode<KeyItemType, _deg>;
			leftPtr->ptrs = new BTreeNode<KeyItemType, _deg>*[MAX_KEYS + 1u];
			rightPtr->ptrs = new BTreeNode<KeyItemType, _deg>*[MAX_KEYS + 1u];
			while (result.retItem >= nodePtr->keys[i] && i < nodePtr->keys_count)
			{
				temp_ptrs[j] = nodePtr->ptrs[i];
				temp_keys[j++] = nodePtr->keys[i++];
			}
			temp_ptrs[j] = result.leftPtr;
			temp_keys[j++] = result.retItem;
			temp_ptrs[j] = result.rightPtr;
			while (i < nodePtr->keys_count)
			{
				temp_keys[j++] = nodePtr->keys[i++];
				temp_ptrs[j] = nodePtr->ptrs[i];
			}
			// temp_keys, temp_ptrs DONE
			i = j = 0u;
			while (i < _deg)
			{
				leftPtr->keys[i] = temp_keys[i];
				leftPtr->ptrs[i++] = temp_ptrs[i];
			}
			leftPtr->keys_count = _deg;
			leftPtr->ptrs[i++] = temp_ptrs[i];
			for (unsigned int iptr = 0u; iptr <= _deg; ++iptr)
				leftPtr->ptrs[iptr]->parentPtr = leftPtr;
			while (i <= MAX_KEYS)
			{
				rightPtr->keys[j] = temp_keys[i];
				rightPtr->ptrs[j++] = temp_ptrs[i++];
			}
			rightPtr->keys_count = _deg - 1u;
			rightPtr->ptrs[j] = temp_ptrs[i];
			for (unsigned int iptr = 0u; iptr < _deg; ++iptr)
				rightPtr->ptrs[iptr]->parentPtr = rightPtr;
			_BTree_itemNptrs<KeyItemType, _deg> ret_struct(temp_keys[_deg], leftPtr, rightPtr);
			delete[] temp_keys;
			temp_keys = 0;
			delete[] temp_ptrs;
			temp_ptrs = 0;
			delete nodePtr;
			nodePtr = 0;
			return ret_struct;
		}
	}
	else //Если с нижних уровней ничего не вернулось
		return _BTree_itemNptrs<KeyItemType, _deg>();
}

template<class KeyItemType, unsigned short int _deg>
inline void BTree<KeyItemType, _deg>::insert(const KeyItemType& item)
{
	if (root) //Если корень есть
	{
		_BTree_itemNptrs<KeyItemType, _deg> result = (root->ptrs) ? insert_search_helper(root, item) : insert_leaf_helper(root, item);
		if (result.leftPtr) //Если с нижних уровней что-то вернулось или разбился сам корень
		{
			if (root) //Если сам корень не разбился
			{
				unsigned int i = 0u;
				if (root->keys_count < MAX_KEYS) //Если корень не забит ключами
				{
					unsigned int j = root->keys_count;
					while (result.retItem >= root->keys[i] && i < root->keys_count) //Ищем, где должно находиться возвращенное значение
						++i;
					while (i < j) //Сдвигаем ключи и указатели
					{
						root->ptrs[j + 1u] = root->ptrs[j];
						root->keys[j--] = root->keys[j - 1u];
					}
					root->keys[i] = result.retItem;
					root->ptrs[i] = result.leftPtr;
					result.leftPtr->parentPtr = root;
					root->ptrs[i + 1u] = result.rightPtr;
					result.rightPtr->parentPtr = root;
					++root->keys_count;
				}
				else //Если корень забит ключами
				{
					unsigned int j = 0u;
					KeyItemType *temp_keys = new KeyItemType[MAX_KEYS + 1u];
					BTreeNode<KeyItemType, _deg>
						**temp_ptrs = new BTreeNode<KeyItemType, _deg>*[MAX_KEYS + 2u],
						*leftPtr = new BTreeNode<KeyItemType, _deg>,
						*rightPtr = new BTreeNode<KeyItemType, _deg>;
					leftPtr->ptrs = new BTreeNode<KeyItemType, _deg>*[MAX_KEYS + 1u];
					rightPtr->ptrs = new BTreeNode<KeyItemType, _deg>*[MAX_KEYS + 1u];
					while (result.retItem >= root->keys[i] && i < root->keys_count)
					{
						temp_ptrs[j] = root->ptrs[i];
						temp_keys[j++] = root->keys[i++];
					}
					temp_ptrs[j] = result.leftPtr;
					temp_keys[j++] = result.retItem;
					temp_ptrs[j] = result.rightPtr;
					while (i < root->keys_count)
					{
						temp_keys[j++] = root->keys[i++];
						temp_ptrs[j] = root->ptrs[i];
					}
					// temp_keys, temp_ptrs DONE
					i = j = 0u;
					while (i < _deg)
					{
						leftPtr->keys[i] = temp_keys[i];
						leftPtr->ptrs[i++] = temp_ptrs[i];
					}
					leftPtr->keys_count = _deg;
					leftPtr->ptrs[i++] = temp_ptrs[i];
					for (unsigned int iptr = 0u; iptr <= _deg; ++iptr)
						leftPtr->ptrs[iptr]->parentPtr = leftPtr;
					while (i <= MAX_KEYS)
					{
						rightPtr->keys[j] = temp_keys[i];
						rightPtr->ptrs[j++] = temp_ptrs[i++];
					}
					rightPtr->keys_count = _deg - 1u;
					rightPtr->ptrs[j] = temp_ptrs[i];
					for (unsigned int iptr = 0u; iptr < _deg; ++iptr)
						rightPtr->ptrs[iptr]->parentPtr = rightPtr;
					delete root;
					root = new BTreeNode<KeyItemType, _deg>(temp_keys[_deg], leftPtr, rightPtr);
					leftPtr->parentPtr = root;
					rightPtr->parentPtr = root;
					delete[] temp_keys;
					temp_keys = 0;
					delete[] temp_ptrs;
					temp_ptrs = 0;
				}
			}
			else //Если корень разбился
			{
				root = new BTreeNode<KeyItemType, _deg>(result.retItem, result.leftPtr, result.rightPtr);
				result.leftPtr->parentPtr = root;
				result.rightPtr->parentPtr = root;
			}
		}
	}
	else //Если корня нет
		root = new BTreeNode<KeyItemType, _deg>(item);
}

//Поиск
template<class KeyItemType, unsigned short int _deg>
inline bool BTree<KeyItemType, _deg>::isExist_helper(BTreeNode<KeyItemType, _deg>* const nodePtr, const KeyItemType& item)
{
	unsigned int i = 0u;
	while (nodePtr->keys[i] < item && i < nodePtr->keys_count)
		++i;
	if (i < nodePtr->keys_count && nodePtr->keys[i] == item)
			return true;
	return (nodePtr->ptrs) ? isExist_helper(nodePtr->ptrs[i], item) : false;
}

template<class KeyItemType, unsigned short int _deg>
inline bool BTree<KeyItemType, _deg>::isExist(const KeyItemType& item)
{
	return isExist_helper(root, item);
}

//Удаление
template<class KeyItemType, unsigned short int _deg>
inline void BTree<KeyItemType, _deg>::erase_helper(BTreeNode<KeyItemType, _deg>*& const nodePtr, const KeyItemType& item)
{
	unsigned int i = 0u;
	while (nodePtr->keys[i] < item && i < nodePtr->keys_count)
		++i;
	if (i < nodePtr->keys_count && nodePtr->keys[i] == item) //Если нашли необходимый элемент(nodePtr->keys[i])
	{
		if (nodePtr->ptrs) //Если не лист
		{

		}
		else //Если лист
		{
			if (nodePtr->keys_count > MIN_KEYS) //Если в листе ключей больше минимального количества
			{
				while (i < nodePtr->keys_count - 1u)
					nodePtr->keys[i++] = nodePtr->keys[i + 1u];
				--nodePtr->keys_count;
			}
			else //Если в листе находится минимальное количество ключей
			{
				BTreeNode<KeyItemType, _deg> *parentPtr = nodePtr->parentPtr;
				unsigned int j = 0u;
				while (item < parentPtr->keys[j] && j < parentPtr->keys_count)
					++j;
				if (j) //Если j > 0
				{
					if (j == parentPtr->keys_count) //Если j == parentPtr->keys_count и имеет только левого соседа
					{
						if (parentPtr->ptrs[j - 1u]->keys_count > MIN_KEYS) //Если в левом соседе ключей больше минимального количества
						{

						}
						else //Если в соседе минимальное количество ключей
						{

						}
					}
					else //Если j имеет обоих соседей
					{
						if (parentPtr->ptrs[j - 1u]->keys_count > MIN_KEYS) //Если в левом соседе ключей больше минимального количества
						{

						}
						else
							if (parentPtr->ptrs[j + 1u]->keys_count > MIN_KEYS) //Если в правом соседе ключей больше минимального количества
							{

							}
							else //Если у обоих соседей минимальное количество ключей
							{

							}

					}
				}
				else //Если j == 0 и имеет только правого соседа
				{

				}
			}
		}
		return;
	}
	else
		isExist_helper(nodePtr->ptrs[i], item);
}

template<class KeyItemType, unsigned short int _deg>
inline void BTree<KeyItemType, _deg>::erase(const KeyItemType& item)
{
	if (isExist(item))
		erase_helper(root, item);
	else
		throw std::exception("element doesn't exists");
}