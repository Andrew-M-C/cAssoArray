/*******************************************************************************
	Copyright (C) 2017 by Andrew Chang <laplacezhang@126.com>
	File name: 	cAssocArray.h

	Description: 	
	    This is the main implementation of C associative array. This array uses
	several basic algorithm and data structures to implement itself:
	1. uses red-black tree (rbtree) to implemen element insert and search
	2. uses BKDR hash to generate keys
	3. uses list to avoid hash colutions
			
	History:
		2014-03-05: File rename as "cAssocArray.c" along with class name
		2014-03-04: File created as "cAssoArray.c"

	------------------------------------------------------------------------

	    This library is free software; you can redistribute it and/or modify it 
	under the terms of the GNU Lesser General Public License as published by the 
	Free Software Foundation, version 2.1 of the License. 
	    This library is distributed in the hope that it will be useful, but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
	FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
	details. 
	    You should have received a copy of the GNU Lesser General Public License 
	along with this library; if not, see <http://www.gnu.org/licenses/>.
		
********************************************************************************/

/********/
#define __HEADERS
#ifdef __HEADERS

#include "cAssocArray.h"

#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#endif


/******/
#define __DATA_STRUCTURES
#ifdef __DATA_STRUCTURES

#ifdef _DEBUG_C_ASSO_ARRAY
#include "log.h"
#define _DEBUG(fmt, args...)	AMCLog(__FILE__" %03d: "fmt, __LINE__, ##args)
#define _MARK()					_DEBUG("MARK")
#else
#define _DEBUG(fmt, args...)
#define _MARK()					printf("Warning: This lib is under development!!!\n")
#endif


typedef enum _Color {
	Color_Black = 0,
	Color_Red,
} Color_t;


typedef struct _Value {
	struct _Value *next;
	void  *value;
	char   key[1];
} Value_st;


typedef struct _Node {
	struct _Node  *parent;
	struct _Node  *left;
	struct _Node  *right;
	Color_t        color;
	long           hash;
	long           count;
	struct _Value *values;
} Node_st;


struct _cAssocArray {
	struct _Node     *children;
	BOOL              use_lock;
	pthread_rwlock_t  rw_lock;
	long              count;
};


#define _SET_BLACK(node)	do{(node)->color = Color_Black;}while(0)
#define _SET_RED(node)		do{(node)->color = Color_Red;}while(0)

#endif

/********/
#define __FORWARD_DECALRATIONS
#ifdef __FORWARD_DECALRATIONS

static inline BOOL _rb_node_is_black(Node_st *node);
static inline BOOL _rb_node_is_red(Node_st *node);
static void __rb_check_delete_node_case_1(cAssocArray *array, Node_st *node);
static void __rb_check_delete_node_case_2(cAssocArray *array, Node_st *node);
static void __rb_check_delete_node_case_3(cAssocArray *array, Node_st *node);
static void __rb_check_delete_node_case_4(cAssocArray *array, Node_st *node);
static void __rb_check_delete_node_case_5(cAssocArray *array, Node_st *node);
static void __rb_check_delete_node_case_6(cAssocArray *array, Node_st *node);

#endif


/********/
#define __LOCK_FUNCTIONS
#ifdef __LOCK_FUNCTIONS

#define _LOCK_ARRAY_READ(array)			do{/*_DEBUG("-- LOCK --");*/ if(array->use_lock){pthread_rwlock_rdlock(&((array)->rw_lock));}}while(0)
#define _UNLOCK_ARRAY_READ(array)		do{/*_DEBUG("--UNLOCK--");*/ if(array->use_lock){pthread_rwlock_unlock(&((array)->rw_lock));}}while(0)
#define _LOCK_ARRAY_WRITE(array)		do{/*_DEBUG("-- LOCK --");*/ if(array->use_lock){pthread_rwlock_wrlock(&((array)->rw_lock));}}while(0)
#define _UNLOCK_ARRAY_WRITE(array)		do{/*_DEBUG("--UNLOCK--");*/ if(array->use_lock){pthread_rwlock_unlock(&((array)->rw_lock));}}while(0)

#endif


/********/
#define __DEBUG_DUMP_FUNCTION
#ifdef __DEBUG_DUMP_FUNCTION

/* --------------------_rb_dump_node----------------------- */
static void _rb_dump_node(Node_st *node, size_t tabCount)
{
	char tab[128] = "";
	long tmp;
	for(tmp = 0; tmp < tabCount && tmp < sizeof(tab) - 1; tmp++) {
		tab[tmp] = '\t';
	}
	tab[tmp] = '\0';

	if (NULL == node) {
		return;
	}

	/* node */
	if (_rb_node_is_black(node)) {
		printf("%sNode \033[0;43m \033[0mBlack (Hash: %ld)", tab, node->hash);
	} else {
		printf("%sNode \033[0;41m \033[0mRed   (Hash: %ld)", tab, node->hash);
	}

	/* values */
	{
		Value_st *value = node->values;
		for (; value; value = value->next) {
			printf("\t\t--> [Value: %p] - Key: \"%s\"\n", value->value, value->key);
		}
	}

	/* children */
	if (node->left || node->right) {
		if (node->left) {
			_rb_dump_node(node->left, tabCount + 1);
			if (node->left->parent != node) {
				printf("ERROR: parent of %ld not %ld", node->left->hash, node->hash);
				exit(1);
			}
		} else {
			printf("%s\tNode left  <N/A>\n", tab);
		}

		if (node->right) {
			_rb_dump_node(node->right, tabCount + 1);
			if (node->right->parent != node) {
				printf("ERROR: parent of %ld not %ld", node->right->hash, node->hash);
				exit(1);
			}
		} else {
			printf("%s\tNode right <N/A>\n", tab);
		}
	}

	/* check */

	return;
}


#endif


/********/
#define __MEM_FUNCTIONS
#ifdef __MEM_FUNCTIONS

/* --------------------_new_value----------------------- */
static Value_st *_new_value(const char *key, void *value)
{
	size_t len = strlen(key);
	Value_st *newValue = malloc(len + sizeof(*newValue));
	if (newValue) {
		newValue->next = NULL;
		newValue->value = value;
		strcpy(newValue->key, key);
	}

	return newValue;
}


/* --------------------_new_node----------------------- */
static Node_st *_new_node(long hash, Color_t color)
{
	Node_st *node = malloc(sizeof(*node));
	if (node) {
		memset(node, 0, sizeof(*node));
		node->color = color;
		node->hash = hash;
	}
	return node;
}


/* --------------------_new_node_with_value----------------------- */
static Node_st *_new_node_with_value(long hash, Color_t color, const char *key, void *value)
{
	Value_st *newValue = _new_value(key, value);
	if (NULL == newValue) {
		return NULL;
	}

	Node_st *newNode = _new_node(hash, color);
	if (NULL == newNode) {
		free(newValue);
		newValue = NULL;
		return NULL;
	}

	newNode->values = newValue;
	newNode->count = 1;

	return newNode;
}


/* --------------------_free_value----------------------- */
static int _free_value(Value_st *value, BOOL shouldFree)
{
	if (NULL == value) {
		errno = EINVAL;
		return -1;
	}

	if (shouldFree && value->value) {
		_DEBUG("free value %p", value->value);
		free(value->value);
	}

	free(value);

	return 0;
}


/* --------------------_free_node----------------------- */
static int _free_node(Node_st *node, BOOL shouldFree)
{
	if (NULL == node) {
		errno = EINVAL;
		return -1;
	}

	Value_st *value = node->values;
	Value_st *nextVal = node->values;

	while(value) {
		nextVal = value->next;
		_free_value(value, shouldFree);
		value = nextVal;
	}

	free(node);
	return 0;
}


#endif


/********/
#define __SEARCH_AND_HASH_FUNCION
#ifdef __SEARCH_AND_HASH_FUNCION

/* --------------------_hash----------------------- */
static long _hash(const char *key)
{
	/* use BKDRHash */
	if (4 == sizeof(long))
	{
		const unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
		unsigned int hash = 0;
		while (*key) {
			hash = hash * seed + (*key++);
		}
		return (hash & 0x7FFFFFFF);
	}
	else	// (8 == sizeof(long))
	{
		const unsigned int seed = 131313;
		unsigned int hash = 0;
		while (*key) {
			hash = hash * seed + (*key++);
		}
		return (hash & 0x7FFFFFFFFFFFFFFF);
	}
}


/* --------------------_locate_value----------------------- */
static Value_st *_locate_value(cAssocArray *array, const char *key, const long hash, Node_st **parentNodeOut, Value_st **prevValueOut)
{
	Node_st *parentNode = array->children;
	Value_st *value = NULL;
	Value_st *prev = NULL;
	BOOL isFound = FALSE;

	/* rbtree search */
	do {
		if (hash < parentNode->hash) {
			parentNode = parentNode->left;
		}
		else if (hash > parentNode->hash) {
			parentNode = parentNode->right;
		}
		else {
			value = parentNode->values;
		}
	} while(parentNode && (NULL == value));

	/* list search */
	while(value && (FALSE == isFound))
	{
		if (0 == strcmp(key, value->key)) {
			isFound = TRUE;
		}
		else {
			prev = value;
			value = value->next;
		}
	}

	/* end */
	if (parentNodeOut) {
		if (value) {
			*parentNodeOut = parentNode;
		}
		else {
			*parentNodeOut = NULL;
		}
	}
	if (prevValueOut) {
		if (value) {
			*prevValueOut = prev;
		}
		else {
			*prevValueOut = NULL;
		}
	}
	return value;
}


/* --------------------_locate_node----------------------- */
static Node_st *_locate_node(cAssocArray *array, long hash)
{
	Node_st *ret = array->children;
	BOOL isFound = FALSE;
	
	if (NULL == ret) {
		return NULL;
	}

	/* let's start searching! */
	do
	{
		if (hash < ret->hash) {
			ret = ret->left;
		}
		else if (hash > ret->hash) {
			ret = ret->right;
		}
		else {
			isFound = TRUE;
		}
	}
	while((FALSE == isFound) && ret);

	if (FALSE == isFound) {
		ret = NULL;
	}
	return ret;
}


/* --------------------_locate_value_in_node----------------------- */
static Value_st *_locate_value_in_node(Node_st *node, const char *key, Value_st **prevOrLastOut)
{
	Value_st *ret = node->values;
	Value_st *prev = NULL;

	while(ret) 
	{
		if (0 == strcmp(ret->key, key)) {
			goto ENDS;			/* value hit */
		}
		prev = ret;
		ret = ret->next;
	}

ENDS:
	*prevOrLastOut = prev;
	return ret;
}


#endif


/********/
#define __RED_BLACK_TREE_OPERATIONS
#ifdef __RED_BLACK_TREE_OPERATIONS

/* --------------------_rb_node_is_black----------------------- */
static inline BOOL _rb_node_is_black(Node_st *node)
{
	return (Color_Black == node->color);
}


/* --------------------_rb_node_is_red----------------------- */
static inline BOOL _rb_node_is_red(Node_st *node)
{
	return (Color_Black != node->color);
}



/* --------------------_rb_node_is_root----------------------- */
static inline BOOL _rb_node_is_root(Node_st *node)
{
	return (NULL == ((node)->parent));
}


/* --------------------_rb_brother_node----------------------- */
static Node_st *_rb_brother_node(Node_st *node)
{
	if (_rb_node_is_root(node)) {
		return NULL;
	}
	else {
		if (node == node->parent->left) {
			return node->parent->right;
		}
		else {
			return node->parent->left;
		}
	}
}


/* --------------------_rb_find_min_leaf----------------------- */
static Node_st *_rb_find_min_leaf(Node_st *node)
{
	Node_st *ret = node;
	while(ret->left) {
		ret = ret->left;
	}	
	return ret;
}


/* --------------------__rb_rotate_left----------------------- */
static void __rb_rotate_left(cAssocArray *array, Node_st *node)
{
	Node_st *prevRight = node->right;
	Node_st *prevRightLeft = node->right->left;
	//_DEBUG("Rotate left %ld", node->id);
	//_DEBUG("prevRight %ld, prevRightLeft %ld", prevRight ? prevRight->id : -1, prevRightLeft ? prevRightLeft->id : -1);

	if (_rb_node_is_root(node))
	{
		array->children = prevRight;
		
		node->parent = prevRight;
		node->right = prevRightLeft;
		if (prevRightLeft)
		{
			prevRightLeft->parent = node;
		}
		
		prevRight->left = node;
		prevRight->parent = NULL;
	}
	else
	{
		Node_st *parent = node->parent;

		//_rb_dump_node(parent, 5);
		if (node == node->parent->left)
		{
			parent->left = prevRight;
		}
		else
		{
			parent->right = prevRight;
		}

		node->parent = prevRight;
		node->right = prevRightLeft;
		if (prevRightLeft)
		{
			prevRightLeft->parent = node;
		}

		prevRight->left = node;
		prevRight->parent = parent;

		//_rb_dump_node(parent, 5);
	}
	
	return;
}


/* --------------------__rb_rotate_right----------------------- */
static void __rb_rotate_right(cAssocArray *array, Node_st *node)
{
	Node_st *prevLeft = node->left;
	Node_st *prevLeftRight = node->left->right;
	//_DEBUG("Rotate right %s", node->key);

	if (_rb_node_is_root(node))
	{
		array->children = prevLeft;
		
		node->parent = prevLeft;
		node->left = prevLeftRight;
		if (prevLeftRight)
		{
			prevLeftRight->parent = node;
		}
		
		prevLeft->right = node;
		prevLeft->parent = NULL;
	}
	else
	{
		Node_st *parent = node->parent;
		if (node == node->parent->left)
		{
			parent->left = prevLeft;
		}
		else
		{
			parent->right = prevLeft;
		}

		node->parent = prevLeft;
		node->left = prevLeftRight;
		if (prevLeftRight)
		{
			prevLeftRight->parent = node;
		}

		prevLeft->right = node;
		prevLeft->parent = parent;
	}
	
	return;
}


/* --------------------__rb_check_insert_by_rb_rule----------------------- */
static void __rb_check_insert_by_rb_rule(cAssocArray *array, Node_st *node)
{
	Node_st *uncleNode = node->parent ? _rb_brother_node(node->parent) : NULL;

	if (_rb_node_is_root(node))							// root node. This is impossible because it is done previously
	{
		_DEBUG("MARK (%ld) case 1", node->hash);
		_SET_BLACK(node);
	}
	else if (_rb_node_is_black(node->parent))			// parent black. Nothing additional needed
	{
		_DEBUG("MARK (%ld) case 2", node->hash);
	}
	else if (uncleNode && _rb_node_is_red(uncleNode))	// parent and uncle node are both red
	{
		_DEBUG("MARK (%ld) case 3, uncle: %ld, parent: %ld", node->hash, uncleNode->hash, node->parent->hash);
		_SET_BLACK(node->parent);
		_SET_BLACK(uncleNode);
		_SET_RED(node->parent->parent);
		__rb_check_insert_by_rb_rule(array, node->parent->parent);
	}
	else				// parent red, meanwhile uncle is black or NULL 
	{
		// Step 1
		if ((node == node->parent->right) &&
			(node->parent == node->parent->parent->left))	// node is its parent's left child, AND parent is grandparent's right child. left-rotation needed.
		{
			_DEBUG("MARK (%ld) case 4", node->hash);
			__rb_rotate_left(array, node->parent);
			node = node->left;
			//_rb_dump_node(array->children, 0);
		}
		else if ((node == node->parent->left) &&
				(node->parent == node->parent->parent->right))
		{
			_DEBUG("MARK (%ld) case 4", node->hash);
			__rb_rotate_right(array, node->parent);
			node = node->right;
			//_rb_dump_node(array->children, 0);
		}
		else
		{}

		// Step 2 (Insert Case 5)
		_SET_BLACK(node->parent);
		_SET_RED(node->parent->parent);

		if ((node == node->parent->left) &&
			(node->parent == node->parent->parent->left))
		{
			_DEBUG("MARK (%ld) case 5", node->hash);
			__rb_rotate_right(array, node->parent->parent);
		}
		else
		{
			_DEBUG("MARK (%ld) case 5", node->hash);
			__rb_rotate_left(array, node->parent->parent);
			//_DEBUG("MARK test:");
			//_rb_dump_node(array->children, 0);
		}
	}
	return;
}


/* --------------------_rb_insert----------------------- */
static int _rb_insert(cAssocArray *array, const char *key, long hash, void *value, BOOL replaceWhenConflict, BOOL shouldFree, BOOL *isDuplicated)
{
	Node_st *node = array->children;
	Node_st *newNode = NULL;
	Value_st *newValue = NULL;
	int error = -1;
	BOOL duplicated = FALSE;
	BOOL shouldCheckIntert = FALSE;

	if (NULL == array->children)	// root
	{
		newNode = _new_node_with_value(hash, Color_Black, key, value);
		if (NULL == newNode) {
			goto ENDS;
		}

		array->children = newNode;
		array->count = 1;

		errno = 0;
		goto ENDS;
	}

	/* first of all, insert a node by binary search tree method */
	do
	{
		if (hash < node->hash)
		{
			if (NULL == node->left) {		/* found a slot */
				newNode = _new_node_with_value(hash, Color_Red, key, value);
				if (NULL == newNode) {
					goto ENDS;
				}
				node->left = newNode;
				newNode->parent = node;
				array->count ++;
				shouldCheckIntert = TRUE;
			}
			else {
				node = node->left;
			}
		}
		else if (hash > node->hash)
		{
			if (NULL == node->right) {		/* found a slot */
				newNode = _new_node_with_value(hash, Color_Red, key, value);
				if (NULL == newNode) {
					goto ENDS;
				}
				node->right = newNode;
				newNode->parent = node;
				array->count ++;
				shouldCheckIntert = TRUE;
			}
			else {
				node = node->right;
			}
		}
		else	/* conflict found */
		{
			Value_st *prevValue = NULL;
			newValue = _locate_value_in_node(node, key, &prevValue);
			newNode = node;

			if (NULL == newValue)
			{
				_DEBUG("Hash conflict: \"%s\" and \"%s\"", key, prevValue->key);
				newValue = _new_value(key, value);
				if (NULL == newValue) {
					goto ENDS;
				}

				prevValue->next = newValue;
				node->count ++;

				array->count ++;
			}
			else	// conflict
			{
				duplicated = TRUE;

				if (replaceWhenConflict) {
					if ((newValue->value != value)
						&& shouldFree)
					{
						free(newValue->value);
						newValue->value = NULL;
					}

					_DEBUG("Update key: \"%s\"", key);
					newValue->value = value;
				}
				else {
					errno = EEXIST;
					goto ENDS;
				}
			}
		}
	}
	while(NULL == newNode);

	/* special red-black tree operation */
	if (shouldCheckIntert) {
		__rb_check_insert_by_rb_rule(array, newNode);
	}

	error = 0;
ENDS:
	/* ends */
	if (isDuplicated) {
		*isDuplicated = duplicated;
	}
	return error;
}


/* --------------------_rb_delete_leaf----------------------- */
static int _rb_delete_leaf(cAssocArray *dict, Node_st *node, BOOL shouldFree)
{
	if (_rb_node_is_root(node))
	{
		dict->children = NULL;
		dict->count = 0;
		_free_node(node, shouldFree);
	}
	else
	{
		if (node == node->parent->left) {
			_DEBUG("DEL left of %ld (%ld)", node->parent->hash, node->hash);
			node->parent->left= NULL;
		}
		else {
			_DEBUG("DEL right of %ld (%ld)", node->parent->hash, node->hash);
			node->parent->right = NULL;
		}

		dict->count --;
		_free_node(node, shouldFree);
	}

	return 0;
}


/* --------------------_rb_read_node_status----------------------- */
static inline void _rb_read_node_status(const Node_st *node, BOOL *hasChild, BOOL *hasTwoChild, BOOL *hasLeftChild, BOOL *hasRightChild)
{
	if (node->left)
	{
		if (node->right)
		{
			*hasChild      = TRUE;
			*hasTwoChild   = TRUE;
			*hasLeftChild  = TRUE;
			*hasRightChild = TRUE;
		}
		else
		{
			*hasChild      = TRUE;
			*hasTwoChild   = FALSE;
			*hasLeftChild  = TRUE;
			*hasRightChild = FALSE;
		}
	}
	else
	{
		if (node->right)
		{
			*hasChild      = TRUE;
			*hasTwoChild   = FALSE;
			*hasLeftChild  = FALSE;
			*hasRightChild = TRUE;
		}
		else
		{
			*hasChild      = FALSE;
			*hasTwoChild   = FALSE;
			*hasLeftChild  = FALSE;
			*hasRightChild = FALSE;
		}
	}
}


/* --------------------__rb_delete_node_and_reconnect_with----------------------- */
static void __rb_delete_node_and_reconnect_with(cAssocArray *array, Node_st *nodeToDel, Node_st *nodeToReplace)
{
	if (_rb_node_is_root(nodeToDel))
	{
		array->count --;
		array->children = nodeToReplace;
		nodeToReplace->parent = NULL;
	}
	else
	{
		array->count --;
		nodeToReplace->parent = nodeToDel->parent;

		if (nodeToDel == nodeToDel->parent->left) {
			nodeToDel->parent->left = nodeToReplace;
		}
		else {
			nodeToDel->parent->right = nodeToReplace;
		}
	}

	return;
}


/* --------------------__rb_check_delete_node_case_6----------------------- */
static void __rb_check_delete_node_case_6(cAssocArray *array, Node_st *node)
{
	Node_st *sibling = _rb_brother_node(node);

	sibling->color = node->parent->color;
	_SET_BLACK(node->parent);

	if (node == node->parent->left) {
		_SET_BLACK(sibling->right);
		__rb_rotate_left(array, node->parent);
	}
	else {
		_SET_BLACK(sibling->left);
		__rb_rotate_right(array, node->parent);
	}

	return;
}


/* --------------------__rb_check_delete_node_case_5----------------------- */
static void __rb_check_delete_node_case_5(cAssocArray *array, Node_st *node)
{
	Node_st *sibling = _rb_brother_node(node);
	BOOL slbIsBlack;
	BOOL sblLeftIsBlack;
	BOOL sblRightIsBlack;

	if (sibling) {
		slbIsBlack = _rb_node_is_black(sibling);
		sblLeftIsBlack = (sibling->left) ? _rb_node_is_black(sibling->left) : TRUE;
		sblRightIsBlack = (sibling->right) ? _rb_node_is_black(sibling->right) : TRUE;
	}
	else {_DEBUG("DEL: case 5 warning");exit(1);
		slbIsBlack = TRUE;
		sblLeftIsBlack = TRUE;
		sblRightIsBlack= TRUE;
	}

	if (FALSE == slbIsBlack)
	{}
	else if ((node == node->parent->left) &&
		(FALSE == sblLeftIsBlack) &&
		sblRightIsBlack)
	{
		if (sibling) {
			_SET_RED(sibling);
		}
		if (sibling && sibling->left) {
			_SET_BLACK(sibling->left);
		}
		__rb_rotate_right(array, sibling);
	}
	else if ((node == node->parent->right) &&
			sblLeftIsBlack &&
			(FALSE == sblRightIsBlack))
	{
		if (sibling) {
			_SET_RED(sibling);
		}
		if (sibling && sibling->right) {
			_SET_BLACK(sibling->right);
		}
		__rb_rotate_left(array, sibling);
	}
	else
	{}

	__rb_check_delete_node_case_6(array, node);
}


/* --------------------__rb_check_delete_node_case_4----------------------- */
static void __rb_check_delete_node_case_4(cAssocArray *array, Node_st *node)
{
	Node_st *sibling = _rb_brother_node(node);
	BOOL sblLeftIsBlack;
	BOOL sblRightIsBlack;
	BOOL sblIsBlack;

	if (NULL == sibling) {
		sblIsBlack = TRUE;
		sblLeftIsBlack = TRUE;
		sblRightIsBlack = TRUE;
	}
	else {
		sblIsBlack = _rb_node_is_black(sibling);
		sblLeftIsBlack = (sibling->left) ? _rb_node_is_black(sibling->left) : TRUE;
		sblRightIsBlack = (sibling->right) ? _rb_node_is_black(sibling->right) : TRUE;
	}

	if (_rb_node_is_red(node->parent)
		&& sblIsBlack
		&& sblLeftIsBlack
		&& sblRightIsBlack)
	{
		_DEBUG("DEL: case 4");
		if (sibling) {
			_SET_RED(sibling);
		}
		_SET_BLACK(node->parent);
	}
	else
	{
		__rb_check_delete_node_case_5(array, node);
	}
}


/* --------------------__rb_check_delete_node_case_3----------------------- */
static void __rb_check_delete_node_case_3(cAssocArray *array, Node_st *node)
{
	Node_st *sibling = _rb_brother_node(node);
	BOOL sblLeftIsBlack;
	BOOL sblRightIsBlack;
	BOOL sblIsBlack;

	if (NULL == sibling) {
		sblIsBlack = TRUE;
		sblLeftIsBlack = TRUE;
		sblRightIsBlack = TRUE;
	}
	else {
		sblIsBlack = _rb_node_is_black(sibling);
		sblLeftIsBlack = (sibling->left) ? _rb_node_is_black(sibling->left) : TRUE;
		sblRightIsBlack = (sibling->right) ? _rb_node_is_black(sibling->right) : TRUE;
	}

	if (_rb_node_is_black(node->parent)
		&& sblLeftIsBlack
		&& sblRightIsBlack
		&& sblIsBlack)
	{
		_DEBUG("DEL: case 3");
		if (sibling) {
			_SET_RED(sibling);
		}
		__rb_check_delete_node_case_1(array, node->parent);
	}
	else
	{
		__rb_check_delete_node_case_4(array, node);
	}
}


/* --------------------__rb_check_delete_node_case_2----------------------- */
static void __rb_check_delete_node_case_2(cAssocArray *array, Node_st *node)
{
	Node_st *sibling = _rb_brother_node(node);

	if (sibling && _rb_node_is_red(sibling))
	{
		_DEBUG("DEL: case 2");
		_SET_RED(node->parent);
		_SET_BLACK(sibling);

		if (node == node->parent->left) {
			__rb_rotate_left(array, node->parent);
		}
		else {
			__rb_rotate_right(array, node->parent);
		}
	}
	else
	{}

	return __rb_check_delete_node_case_3(array, node);
}


/* --------------------__rb_check_delete_node_case_1----------------------- */
static void __rb_check_delete_node_case_1(cAssocArray *array, Node_st *node)
{
	/* let's operate with diferent cases */
	if (_rb_node_is_root(node)) {
		_DEBUG("DEL: case 1");
		return;
	}
	else {		// Right child version
		__rb_check_delete_node_case_2(array, node);
	}
}


/* --------------------_rb_delete_node----------------------- */
static int _rb_delete_node(cAssocArray *array, Node_st *node, BOOL freeObject)
{
	BOOL nodeHasChild, nodeHasTwoChildren, nodeHasLeftChild, nodeHasRightChild;
	_rb_read_node_status(node, &nodeHasChild, &nodeHasTwoChildren, &nodeHasLeftChild, &nodeHasRightChild);

	if (FALSE == nodeHasChild)
	{
		_DEBUG("DEL: node %ld is leaf", node->hash);
		_rb_delete_leaf(array, node, freeObject);
		return 0;
	}
	else if (nodeHasTwoChildren)
	{
		Node_st tmpNode;
		Node_st *smallestRightChild = _rb_find_min_leaf(node->right);
		_DEBUG("DEL: node %ld has 2 children, replace with %ld (%s)", node->hash, smallestRightChild->hash, smallestRightChild->values->key);

		/* replace with smallest node */
		tmpNode.hash = node->hash;
		tmpNode.values = node->values;
		
		node->hash = smallestRightChild->hash;
		node->values = smallestRightChild->values;

		smallestRightChild->hash = tmpNode.hash;
		smallestRightChild->values = tmpNode.values;

		/* start operation with the smallest one */
		return _rb_delete_node(array, smallestRightChild, freeObject);
	}
	else
	{
		Node_st *child = (node->left) ? node->left : node->right;

		if (_rb_node_is_red(node))		// if node is red, both parent and child nodes are black. We could simply replace it with its child
		{
			_DEBUG("DEL: node %ld red", node->hash);
			__rb_delete_node_and_reconnect_with(array, node, child);
			_free_node(node, freeObject);
			node = NULL;
			return 0;
		}
		else if (_rb_node_is_red(child))		// if node is black but its child is red. we could repace it with its child and refill the child as black
		{
			_DEBUG("DEL: node %ld black but child red", node->hash);
			__rb_delete_node_and_reconnect_with(array, node, child);
			_SET_BLACK(child);
			_free_node(node, freeObject);
			node = NULL;
			return 0;
		}
		else				// both node and its child are all black. This is the most complex one.
		{
			/* first of all, we should replace node with child */
			__rb_delete_node_and_reconnect_with(array, node, child);
			_free_node(node, freeObject);	/* "node" val is useless */
			node = NULL;
			__rb_check_delete_node_case_1(array, child);
			return 0;
		}
	}
}


/* --------------------_rb_delete_value_in_node----------------------- */
static int _rb_delete_value_in_node(Node_st *node, const char *key, BOOL freeObject)
{
	Value_st *value = NULL;
	Value_st *prevVal = NULL;

	if (1 == node->count) {
		return -1;
	}

	value = _locate_value_in_node(node, key, &prevVal);
	if (value) {
		if (NULL == prevVal) {
			node->values = value->next;				
		} else {
			prevVal->next = value->next;
		}

		node->count --;
		_free_value(value, freeObject);
		return 0;
	}
	else {
		errno = ENOENT;
		return -1;
	}
}


/* --------------------_rb_delete_value_in_array----------------------- */
static int _rb_delete_value_in_array(cAssocArray *array, const char *key, BOOL freeObject)
{
	long hash = _hash(key);
	Node_st *node = _locate_node(array, hash);
	if (NULL == node) {
		errno = ENOENT;
		return -1;
	}

	Value_st *value = NULL;
	Value_st *prevVal = NULL;

	value = _locate_value_in_node(node, key, &prevVal);
	if (value) {
		if (1 == node->count) {
			return _rb_delete_node(array, node, freeObject);
		}
		else {
			if (NULL == prevVal) {
				node->values = value->next;				
			} else {
				prevVal->next = value->next;
			}

			node->count --;
			_free_value(value, freeObject);
			return 0;
		}
	}
	else {
		errno = ENOENT;
		return -1;
	}
}




#endif


/********/
#define __PUBLIC_INTERFACES
#ifdef __PUBLIC_INTERFACES

/* --------------------cAssocArray_Create----------------------- */
cAssocArray *cAssocArray_Create(BOOL locked)
{
	cAssocArray *array = malloc(sizeof(*array));
	if (NULL == array) {
		return NULL;
	}

	memset(array, 0, sizeof(*array));

	if (locked) {
		array->use_lock = TRUE;
		pthread_rwlock_init(&(array->rw_lock), NULL);
	}
	else {
		array->use_lock = FALSE;
	}

	return array;
}


/* --------------------cAssocArray_Delete----------------------- */
int cAssocArray_Delete(cAssocArray *array)
{
	// TODO:
	return -1;
}


/* --------------------cAssocArray_Set----------------------- */
int cAssocArray_SetValue(cAssocArray *array, const char *key, void *value, BOOL freeDuplicate)
{
	long hash = 0;
	int ret = 0;

	if ((NULL == array)
		|| (NULL == key)
		|| (NULL == value)
		|| (0 == strlen(key)))
	{
		errno = EINVAL;
		return -1;
	}

	hash = _hash(key);
	_DEBUG("Now insert Hash value for \"%s\": %ld", key, hash);
	_LOCK_ARRAY_WRITE(array);
	ret = _rb_insert(array, key, hash, value, TRUE, freeDuplicate, NULL);
	_UNLOCK_ARRAY_WRITE(array);
	
	return ret;
}


/* --------------------cAssocArray_Get----------------------- */
void *cAssocArray_GetValue(cAssocArray *array, const char *key)
{
	void *ret = NULL;
	long hash = 0;

	if ((NULL == array)
		|| (NULL == key)
		|| (0 == strlen(key)))
	{
		errno = EINVAL;
		return NULL;
	}

	hash = _hash(key);
	_LOCK_ARRAY_READ(array);
	ret = _locate_value(array, key, hash, NULL, NULL);
	_UNLOCK_ARRAY_READ(array);

	return ret;
}


/* --------------------cAssocArray_Size----------------------- */
long cAssocArray_Size(cAssocArray *array)
{
	if (array) {
		return array->count;
	}
	else {
		return -1;
	}
}


/* --------------------cAssocArray_Size----------------------- */
int cAssocArray_RemoveValue(cAssocArray *array, const char *key, BOOL shouldFree)
{
	int ret = 0;

	if ((NULL == array)
		|| (NULL == key)
		|| (0 == strlen(key)))
	{
		errno = EINVAL;
		return -1;
	}

	_DEBUG("Delete %s", key);
	_LOCK_ARRAY_WRITE(array);
	ret = _rb_delete_value_in_array(array, key, shouldFree);
	_UNLOCK_ARRAY_WRITE(array);
	_DEBUG("Delete %s done", key);
	
	return ret;
}


/* --------------------cAssocArray_Detach----------------------- */
void *cAssocArray_DetachValue(cAssocArray *array, const char *key)
{
	void *ret = NULL;
	long hash = 0;

	if ((NULL == array)
		|| (NULL == key)
		|| (0 == strlen(key)))
	{
		errno = EINVAL;
		return NULL;
	}

	hash = _hash(key);

	_LOCK_ARRAY_WRITE(array);
	{
		Node_st *node = NULL;
		Value_st *prevValue = NULL;
		Value_st *value = _locate_value(array, key, hash, &node, &prevValue);

		if (value) {
			ret = value->value;

			if (1 == node->count) {
				_rb_delete_node(array, node, FALSE);
			}
			else {
				_rb_delete_value_in_node(node, key, FALSE);
			}
		}
		else {
			errno = ENOENT;
		}
	}
	_UNLOCK_ARRAY_WRITE(array);
	
	return ret;
}


/* --------------------cAssocArray_Add----------------------- */
int cAssocArray_AddValue(cAssocArray *array, const char *key, void *value)
{
	int ret = 0;
	long hash = 0;
	BOOL isDuplicated = FALSE;

	if ((NULL == array)
		|| (NULL == key)
		|| (NULL == value)
		|| (0 == strlen(key)))
	{
		errno = EINVAL;
		return -1;
	}

	hash = _hash(key);
	_LOCK_ARRAY_WRITE(array);
	ret = _rb_insert(array, key, hash, value, FALSE, FALSE, &isDuplicated);
	_UNLOCK_ARRAY_WRITE(array);

	if (isDuplicated) {
		errno = EEXIST;
		ret = -1;
	}	
	return ret;
}


/* --------------------cAssocArray_Update----------------------- */
int cAssocArray_UpdateValue(cAssocArray *array, const char *key, void *value, BOOL freeDuplicate, void **prevValueOut)
{
	int ret = 0;
	long hash = 0;
	void *prevValue = NULL;
	Value_st *valueObj = NULL;

	if ((NULL == array)
		|| (NULL == key)
		|| (NULL == value)
		|| (0 == strlen(key)))
	{
		errno = EINVAL;
		return -1;
	}

	hash = _hash(key);
	_LOCK_ARRAY_WRITE(array);
	{
		valueObj = _locate_value(array, key, hash, NULL, NULL);
		if (NULL == valueObj) {
			errno = ENOENT;
		}
		else {
			if (valueObj->value == value) {
				/* no change */
			}
			else {
				prevValue = valueObj->value;
				valueObj->value = value;
			}
		}
	}
	_UNLOCK_ARRAY_WRITE(array);

	if (freeDuplicate) {
		_DEBUG("free value %p", prevValue);
		free(prevValue);
		prevValue = NULL;
	}

	if (prevValueOut) {
		*prevValueOut = prevValue;
	}
	return ret;
}


/* --------------------cAssocArray_DumpToStdout----------------------- */
void cAssocArray_DumpToStdout(cAssocArray *array)
{
	if (NULL == array) {
		return;
	}

	_LOCK_ARRAY_READ(array);
	printf("cAssocArray [%p], size %ld:\n", array, array->count);
	_rb_dump_node(array->children, 1);
	_UNLOCK_ARRAY_READ(array);

	return;
}


#endif
/* EOF */


