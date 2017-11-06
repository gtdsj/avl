#ifndef __AVL_TREE_H_PROTECTED_
#define __AVL_TREE_H_PROTECTED_

#include "Comparable.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <vector>
using namespace std;

enum dir_t { LEFT = 0, RIGHT = 1 };
enum balance_t { LEFT_HEAVY = -1, BALANCED = 0, RIGHT_HEAVY = 1 };
enum height_effect_t { HEIGHT_NOCHANGE = 0, HEIGHT_CHANGE = 1 };
enum TraversalOrder { LTREE, KEY, RTREE };

template <typename KeyType>
class AvlNode
{
public:
	AvlNode(Comparable<KeyType>* item);
	short height() const { return _height };
	cmp_t compare(KeyType key) { return _data->Compare(key); }

	static Comparable<KeyType>* insert(AvlNode<KeyType>* &root, Comparable<KeyType>* &item);
	static Comparable<KeyType>* search(AvlNode<KeyType>* &root, KeyType key);
	static Comparable<KeyType>* remove(AvlNode<KeyType>* &root, KeyType key);

	static void Dump(TraversalOrder order, const AvlNode<KeyType> * node, int level = 0);
	static void Dump(AvlNode<KeyType>* &root, int level = 0);
protected:
	static short calc_height(AvlNode<KeyType>* &root);
	static dir_t opposite(dir_t dir) { return dir_t(1 - int(dir)); }
	static Comparable<KeyType>* insert(AvlNode<KeyType>* &root, Comparable<KeyType>* &item, int &change);
	static short get_balance(AvlNode<KeyType>* &root);
	static bool left_heavy(AvlNode<KeyType>* &root);
	static bool right_heavy(AvlNode<KeyType>* &root);
	static void rotate_once(AvlNode<KeyType>* &root, dir_t dir);
	static void rotate_twice(AvlNode<KeyType>* &root, dir_t dir);
	static void rebalance(AvlNode<KeyType>* &root);
private:
	Comparable<KeyType>* _data;
	AvlNode* _child[2];
	short _height;
};

template <typename KeyType>
class AvlTree
{
public:
	AvlTree() : _root(NULL) {};

	Comparable<KeyType>* search(KeyType key) {
		return AvlNode<KeyType>::search(_root, key);
	}

	Comparable<KeyType>* insert(Comparable<KeyType>* item) {
		return AvlNode<KeyType>::insert(_root, item);
	}

	Comparable<KeyType>* remove(KeyType key) {
		return AvlNode<KeyType>::remove(_root, key);
	}

	void DumpTree()  {
		AvlNode<KeyType>::Dump(_root, 0);
	}
protected:
private:
	AvlNode<KeyType>* _root;
};


template <typename KeyType>
AvlNode<KeyType>::AvlNode(Comparable<KeyType>* item)
	:_height(1), _data(item)
{
	_child[LEFT] = _child[RIGHT] = NULL;
}

template <typename KeyType>
Comparable<KeyType>* AvlNode<KeyType>::search(AvlNode<KeyType>* &root, KeyType key)
{
	cmp_t result;
	dir_t dir;
	while (root && (result = root->compare(key))){
		if (result == CMP_EQ) {
			return root;
		}else {
			dir = result == CMP_MIN ? RIGHT : LEFT;
			root = root[dir];
		}
	}
	return NULL;
}

template <typename KeyType>
Comparable<KeyType>* AvlNode<KeyType>::remove(AvlNode<KeyType>* &root, KeyType key) {
	if (root == NULL) {
		return NULL;
	}
	Comparable<KeyType> * found = NULL;
	cmp_t result = root->_data->Compare(key);
	if (result != CMP_EQ) {
		dir_t dir = result == CMP_MIN ? RIGHT : LEFT;
		Comparable<KeyType>* found = remove(root->_child[dir], key);
	}else {
		found = root->_data;
		if (root->_height == 1) {
			// no child, just remove it
			delete root;
			root = NULL;
		}else if (root->_child[LEFT] == NULL || root->_child[RIGHT] == NULL) {
			AvlNode<KeyType>* tmp = root;
			root = root->_child[root->_child[LEFT] == NULL ? RIGHT : LEFT];
			tmp->_child[LEFT] = tmp->_child[RIGHT] = NULL;
			delete tmp;
		}else {
			// have two children
			AvlNode<KeyType>* tmp = root->_child[RIGHT];
			while (tmp->_child[LEFT] != NULL){
				tmp = tmp->_child[LEFT];
			}
			root->_data = tmp->_data;
			found = remove(root->_child[RIGHT], tmp->_data->Key());
		}
	}

	if (found != NULL && root != NULL) {
		short balance = abs(get_balance(root));
		if (balance > RIGHT_HEAVY) {
			rebalance(root);
		}
		else {
			root->_height = calc_height(root);
		}
	}
	return found;
}

template <typename KeyType>
Comparable<KeyType>* AvlNode<KeyType>::insert(AvlNode<KeyType>* &root, Comparable<KeyType>* &item)
{
	int change = BALANCED;
	return insert(root, item, change);
}

template <typename KeyType>
Comparable<KeyType>* AvlNode<KeyType>::insert(AvlNode<KeyType>* &root, Comparable<KeyType>*& item, int &change)
{
	if (root == NULL) {
		root = new AvlNode<KeyType>(item);
		change = HEIGHT_CHANGE;
		return NULL;
	}
	short old_height = root->_height;
	cmp_t result = root->_data->Compare(item->Key());
	if (result != CMP_EQ) {
		dir_t dir = result == CMP_MIN ? RIGHT : LEFT;
		Comparable<KeyType>* found = insert(root->_child[dir], item, change);
		if (found != NULL) return found;
	}else {
		change = HEIGHT_NOCHANGE;
		return root->_data;
	}

	// 
	if (change == HEIGHT_CHANGE) {
		//root->_height = calc_height(root);
		short balance = abs(get_balance(root));
		if (balance > RIGHT_HEAVY) {
			rebalance(root);
		}else {
			root->_height = calc_height(root);
		}
		change = root->_height != old_height ? HEIGHT_CHANGE : HEIGHT_NOCHANGE;
	}
	return NULL;
}

template <typename KeyType>
void AvlNode<KeyType>::rotate_once(AvlNode<KeyType>* &root, dir_t dir)
{	
	dir_t other_dir = opposite(dir);
	AvlNode<KeyType>* old_root = root;

	root = old_root->_child[other_dir];
	old_root->_child[other_dir] = root->_child[dir];
	root->_child[dir] = old_root;
	
	old_root->_height = calc_height(old_root);
	root->_height = calc_height(root);
}


/* LR(B rotates to the left, then C rotates to the right):
         k3                    k3                       k2
        /  \                  /  \                     /  \
       k1   D                k2   D                   k1   k3
      /  \         ==>      /  \        ==>          / \   / \
     A    k2               k1   C                   A  B  C   D
         /  \             /  \
        B    C           A    B
   RL(D rotates to the right, then C rotates to the left):
		k3                         k3                          k2
		/  \                       /  \                        /  \
	   A    k1                    A    k2                     k3   k1
	   	   /  \       ==>             /  \         ==>       /  \  / \
	   	  k2   B                     C    k1                A   C D   B
	   	 /  \                            /  \
	   	 C    D                          D    B
*/

template <typename KeyType>
void AvlNode<KeyType>::rotate_twice(AvlNode<KeyType>* &root, dir_t dir)
{
	dir_t other_dir = opposite(dir);
	AvlNode<KeyType>* old_root = root;
	AvlNode<KeyType>* old_other_dir_child = old_root->_child[other_dir];

	root = old_other_dir_child->_child[dir];
	old_other_dir_child->_child[dir] = root->_child[other_dir];
	old_other_dir_child->_height = calc_height(old_other_dir_child);

	root->_child[other_dir] = old_other_dir_child;
	
	old_root->_child[other_dir] = root->_child[dir];
	old_root->_height = calc_height(old_root);
	
	root->_child[dir] = old_root;
	root->_height = calc_height(root);
}

template <typename KeyType>
void AvlNode<KeyType>::rebalance(AvlNode<KeyType>* &root)
{
	short balance = BALANCED;
	if (left_heavy(root)) {
		balance = get_balance(root->_child[LEFT]);
		if (balance == RIGHT_HEAVY) {
			// rotate twice
			rotate_twice(root, RIGHT);
		}else {
			// right rotate
			rotate_once(root, RIGHT);
		}
	}else if (right_heavy(root)) {
		balance = get_balance(root->_child[LEFT]);
		if (balance == LEFT_HEAVY) {
			// rotate twice
			rotate_twice(root, LEFT);
		}else {
			// left rotate
			rotate_once(root, LEFT);
		}
	}
}

template <typename KeyType>
bool AvlNode<KeyType>::left_heavy(AvlNode<KeyType>* &root)
{
	return get_balance(root) < LEFT_HEAVY;
}

template <typename KeyType>
bool AvlNode<KeyType>::right_heavy(AvlNode<KeyType>* &root)
{
	return get_balance(root) > RIGHT_HEAVY;
}

template <typename KeyType>
short AvlNode<KeyType>::get_balance(AvlNode<KeyType>* &root)
{
	if (root == NULL){
		return 0;
	}
	short left_height = root->_child[LEFT] == NULL ? 0 : root->_child[LEFT]->_height;
	short right_height = root->_child[RIGHT] == NULL ? 0 : root->_child[RIGHT]->_height;
	return right_height - left_height;
}

template <typename KeyType>
short AvlNode<KeyType>::calc_height(AvlNode<KeyType>* &root)
{
	short left_height = root->_child[LEFT] == NULL ? 0 : root->_child[LEFT]->_height;
	short right_height = root->_child[RIGHT] == NULL ? 0 : root->_child[RIGHT]->_height;
	return std::max(left_height, right_height) + 1;
}


static inline void Indent(int len) {
	for (int i = 0; i < len; i++) {
		std::cout << ' ';
	}
}



template <class KeyType>
void AvlNode<KeyType>::Dump(TraversalOrder order,
	const AvlNode<KeyType> * node,
	int level)
{
	unsigned  len = (level * 5) + 1;
	if ((order == LTREE) && (node->_child[LEFT] == NULL)) {
		Indent(len);
		std::cout << "     **NULL**" << endl;
	}
	if (order == KEY) {
		Indent(len);
		std::cout << node->_data->Key() << ":" << node->_height << endl;
	}
	if ((order == RTREE) && (node->_child[RIGHT] == NULL)) {
		Indent(len);
		std::cout << "     **NULL**" << endl;
	}
}

template <typename KeyType>
void AvlNode<KeyType>::Dump(AvlNode<KeyType>* &root, int level)
{
	if (root != NULL) {
		Dump(RTREE, root, level);
		if (root->_child[RIGHT] != NULL) {
			Dump(root->_child[RIGHT], level + 1);
		}
		Dump(KEY, root, level);
		if (root->_child[LEFT] != NULL) {
			Dump(root->_child[LEFT], level + 1);
		}
		Dump(LTREE,root, level);
	}
}

#endif // _DEBUG
