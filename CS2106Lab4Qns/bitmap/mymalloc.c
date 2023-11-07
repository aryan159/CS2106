#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"
#include "bitmap.h"
#include "llist.h"

char _heap[MEMSIZE] = {0};
TNode *root = NULL;

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
    print_map(_heap, MEMSIZE / 8);
}

// Allocates size bytes of memory and returns a pointer
// to the first byte.
void *mymalloc(size_t size)
{

    // Find index in bitmap
    long index = search_map(_heap, MEMSIZE / 8, size);
    if (index == -1)
    {
        return NULL;
    }

    // Create and insert node into linked list
    TData *tData = malloc(sizeof(TData));
    tData->len = size;
    TNode *tNode = make_node(index, tData);
    insert_node(&root, tNode, ASCENDING);

    // Allocate the bitmap
    allocate_map(_heap, index, size);

    // Return bitmap pointer?
    return &_heap[index];
}

// Frees memory pointer to by ptr.
void myfree(void *ptr)
{

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
    long length = node->pdata->len;

    // Reset the bitmap
    free_map(_heap, index, length);

    // Delete node from linked list
    free(node->pdata);
    delete_node(&root, node);
}