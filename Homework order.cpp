#include "stdafx.h"
#include <iostream>

//Przy pisaniu projektu posłużyłem się pseudokodem zawartym w slajdach wykładu dr. inż. Krzysztofa Manuszewskiego

#define SIZE 1000

using namespace std;

struct node
{
	int key;
	int degree;
	node* child;
	node* sibling;
	node* parent;
};

class hashNode
{
public:
	int key;
	node* value;
	hashNode* next;
	hashNode(int key, node* value)
	{
		this->key = key;
		this->value = value;
		this->next = NULL;
	}
};

class HashTable
{
private:
	hashNode * * table;
public:
	HashTable()
	{
		table = new hashNode*[SIZE];
		for (int i = 0; i < SIZE; i++)
			table[i] = NULL;
	}
	~HashTable()
	{
		for (int i = 0; i < SIZE; ++i)
		{
			hashNode* entry = table[i];
			while (entry != NULL)
			{
				hashNode* prev = entry;
				entry = entry->next;
				delete prev;
			}
		}
		delete[] table;
	}
	int hashFunc(int key)
	{
		return key % SIZE;
	}
	void Insert(int key, node* value)
	{
		int hash_val = hashFunc(key);
		hashNode* prev = NULL;
		hashNode* entry = table[hash_val];
		while (entry != NULL && entry->key != key)
		{
			prev = entry;
			entry = entry->next;
		}
		if (entry == NULL)
		{
			entry = new hashNode(key, value);
			if (prev == NULL)
				table[hash_val] = entry;
			else
				prev->next = entry;
		}
		else
			entry->value = value;
	}
	node* getRoot(int key)
	{
		int hash_val = hashFunc(key);
		hashNode* entry = table[hash_val];
		while (entry != NULL)
		{
			if (entry->key == key)
			{
				return entry->value;
			}
			entry = entry->next;
		}
		return NULL;
	}
};

node* newNode(int key)
{
	node *temp = new node;
	temp->key = key;
	temp->degree = 0;
	temp->child = temp->parent = temp->sibling = NULL;
	return temp;
}

node* maxHeap(node* heap)
{
	node* max = heap;
	node* current = heap;
	while (current != NULL)
	{
		if (current->key > max->key)
			max = current;
		current = current->sibling;
	}
	return max;
}

void linkTree(node* newSon, node* newParent)
{
	newSon->parent = newParent;
	newSon->sibling = newParent->child;
	newParent->child = newSon;
	newParent->degree = newParent->degree + 1;
}

node* mergeHeap(node* h1, node* h2)
{
	node* ret = NULL;
	node* current = NULL;
	node* next;
	node* temp;
	while (h1 != NULL && h2 != NULL)
	{
		if (h1->degree <= h2->degree)
		{
			next = h1;
			h1 = h1->sibling;
		}
		else
		{
			next = h2;
			h2 = h2->sibling;
		}
		if (ret == NULL)
		{
			ret = next;
			current = next;
		}
		else
		{
			current->sibling = next;
			current = next;
		}
	}
	if (h1 == NULL)
	{
		temp = h1;
		h1 = h2;
		h2 = temp;
	}
	if (ret == NULL)
		return h1;
	else
		current->sibling = h1;
	return ret;
}

node* unionHeap(node* y, node* z)
{
	node* ret = mergeHeap(y, z);
	if (ret == NULL)
		return NULL;
	node* prevX = NULL;
	node* X = ret;
	node* nextX = ret->sibling;
	while (nextX != NULL)
	{
		if (X->degree != nextX->degree || (nextX->sibling != NULL && nextX->degree == nextX->sibling->degree))
		{
			prevX = X;
			X = nextX;
			nextX = nextX->sibling;
		}
		else if (X->key >= nextX->key)
		{
			X->sibling = nextX->sibling;
			nextX->sibling = NULL;
			linkTree(nextX, X);
			nextX = X->sibling;
		}
		else
		{
			if (prevX == NULL)
				ret = nextX;
			else
				prevX->sibling = nextX;
			X->sibling = NULL;
			linkTree(X, nextX);
			X = nextX;
			nextX = nextX->sibling;
		}
	}
	return ret;
}

node* insertHeap(node* heap, int key)
{
	node* n = newNode(key);
	return unionHeap(heap, n);
}

node* deleteNode(node* head, node *n)
{
	if (head == n && head->sibling != NULL)
	{
		head->key = head->sibling->key;
		head->degree = head->sibling->degree;
		head->child = head->sibling->child;
		head->parent = head->sibling->parent;

		head->sibling = head->sibling->sibling;

		return head;
	}
	if (head == n && head->sibling == NULL)
		return NULL;

	node *prev = head;
	while (prev->sibling != NULL && prev->sibling != n)
		prev = prev->sibling;

	prev->sibling = prev->sibling->sibling;

	return head;
}

node* reverse(node* son)
{
	if (son != NULL && son->sibling != NULL)
	{
		node* prev = NULL;
		node* current = son;
		node* next = NULL;
		while (current != NULL)
		{
			current->parent = NULL;
			next = current->sibling;
			current->sibling = prev;
			prev = current;
			current = next;
		}
		son = prev;
	}
	return son;
}

node* extractMaxHeap(node* heap)
{
	if (heap == NULL)
	{
		printf("na\n");
		return heap;
	}
	node* ret = maxHeap(heap);
	printf("%d\n", ret->key);
	node* sons = ret->child;
	if (sons != NULL)
		sons->parent = NULL;
	heap = deleteNode(heap, ret);
	node* h1 = reverse(sons);
	heap = unionHeap(heap, h1);
	return heap;
}

void incKeyHeap(node* n, int newKey)
{
	if (n == NULL || newKey < n->key)
	{
		printf("na\n");
		return;
	}
	int temp;
	n->key = newKey;
	while (n->parent != NULL && n->key > n->parent->key)
	{
		temp = n->key;
		n->key = n->parent->key;
		n->parent->key = temp;
		n = n->parent;
	}
}

node* search(node* n, int key)
{
	node* result;
	while (n != NULL)
	{
		if (n->key == key)
			return n;
		if (n->key > key)
		{
			result = search(n->child, key);
			if (result != NULL)
				return result;
		}
		n = n->sibling;
	}
	return NULL;
}

node* searchForNode(node* heap, int key)
{
	node* result;
	node* crawl = heap;

	while (crawl != NULL)
	{
		if (crawl->key == key)
			return crawl;
		if (crawl->key > key)
		{
			result = search(crawl, key);
			if (result != NULL)
				return result;
		}
		crawl = crawl->sibling;
	}
	return NULL;
}

void printTree(node *h)
{
	while (h)
	{
		printf("%d ", &h->key);
		printTree(h->child);
		h = h->sibling;
	}
}

void printHeap(HashTable &t, int group)
{
	node* root = t.getRoot(group);
	while (root)
	{
		printf("%d ", &root->key);
		printTree(root->child);
		root = root->sibling;
	}
	printf("\n");
}

void addElement(HashTable& t, int group, int value)
{
	node* root = t.getRoot(group);
	if (root == NULL)
		root = newNode(value);
	else
		root = insertHeap(root, value);

	t.Insert(group, root);
}

void extractElement(HashTable &t, int group)
{
	node* root = t.getRoot(group);
	root = extractMaxHeap(root);
	t.Insert(group, root);
}

void mergeGroups(HashTable &t, int group1, int group2)
{
	node *root1, *root2;
	root1 = t.getRoot(group1);
	root2 = t.getRoot(group2);
	root1 = unionHeap(root1, root2);
	root2 = NULL;
	t.Insert(group1, root1);
	t.Insert(group2, root2);
}

void increaseValue(HashTable &t, int group, int oldV, int newV)
{
	node *root = t.getRoot(group);
	incKeyHeap(searchForNode(root, oldV), newV);
}

int main()
{
	HashTable t;
	int n, group1, group2, value, oldV, newV;
	char c;

	scanf("%d", &n);
	for (int i = 0; i < n; i++)
	{
		cin >> c;
		if (c == 'a')
		{
			scanf("%d %d", &group1, &value);
			addElement(t, group1, value);
		}
		else if (c == 'e')
		{
			scanf("%d", &group1);
			extractElement(t, group1);
		}
		else if (c == 'p')
		{
			scanf("%d", &group1);
			printHeap(t, group1);
		}
		else if (c == 'm')
		{
			scanf("%d %d", &group1, &group2);
			mergeGroups(t, group1, group2);
		}
		else if (c == 'i')
		{
			scanf("%d %d %d", &group1, &oldV, &newV);
			increaseValue(t, group1, oldV, newV);
		}
	}

	return 0;
}

