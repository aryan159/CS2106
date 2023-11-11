#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"
#include "llist.h"

char _heap[MEMSIZE] = {0};
TNode *root = NULL;

int initialized = 0;

void init()
{
    TData *tData = malloc(sizeof(TData));
    tData->length = MEMSIZE;
    tData->isAllocated = 0;

    TNode *tNode = make_node(0, tData);
    insert_node(&root, tNode, ASCENDING);

    initialized = 1;
}

// Do not change this. Used by the test harness.
// You may however use this function in your code if necessary.
long get_index(void *ptr)
{
    if (ptr == NULL)
        return -1;
    else
        return (long)((char *)ptr - &_heap[0]);
}

void print_memlist()
{
    TNode *curNode = root;
    while (curNode != NULL)
    {
        if (curNode->pdata->isAllocated == 1)
        {
            printf("Status: ALLOCATED Start index: %d Length: %d\n", curNode->key, curNode->pdata->length);
        }
        else
        {
            printf("Status: FREE Start index: %d Length: %d\n", curNode->key, curNode->pdata->length);
        }
        curNode = curNode->next;
    }
}

// Allocates size bytes of memory and returns a pointer
// to the first byte.
void *mymalloc(size_t size)
{
    if (initialized == 0)
    {
        init();
    }

    TNode *selectedNode = NULL;

    TNode *curNode = root;
    while (curNode != NULL)
    {
        if (curNode->pdata->isAllocated == 0 && curNode->pdata->length >= size)
        {
            if (selectedNode == NULL || curNode->pdata->length < selectedNode->pdata->length)
            {
                selectedNode = curNode;
            }
        }
        curNode = curNode->next;
    }

    if (selectedNode == NULL)
    {
        return NULL;
    }

    int nextNodeFreeSpace = selectedNode->pdata->length - size;
    int startIndex = selectedNode->key;

    selectedNode->pdata->isAllocated = 1;
    selectedNode->pdata->length = size;

    if (nextNodeFreeSpace > 0)
    {
        TData *tData = malloc(sizeof(TData));
        tData->length = nextNodeFreeSpace;
        tData->isAllocated = 0;

        TNode *tNode = make_node(startIndex + size, tData);
        insert_node(&root, tNode, ASCENDING);
    }

    // Return pointer
    return &_heap[startIndex];
}

// Frees memory pointer to by ptr.
void myfree(void *ptr)
{
    if (initialized == 0)
    {
        init();
    }

    long index = get_index(ptr);
    if (index == -1)
    {
        return;
    }

    // Get length from linked list
    TNode *node = find_node(root, index);
    if (node == NULL)
    {
        return;
    }
    long length = node->pdata->length;
    node->pdata->isAllocated = 0;

    if ((node->next != NULL) && (node->next->pdata->isAllocated == 0))
    {
        int curLength = node->pdata->length;
        int nextNodeLength = node->next->pdata->length;
        node->pdata->length = curLength + nextNodeLength;

        free(node->next->pdata);
        delete_node(&root, node->next);
    }

    if ((node->prev != NULL) && (node->prev->pdata->isAllocated == 0))
    {
        node = node->prev;

        int curLength = node->pdata->length;
        int nextNodeLength = node->next->pdata->length;
        node->pdata->length = curLength + nextNodeLength;

        free(node->next->pdata);
        delete_node(&root, node->next);
    }
}