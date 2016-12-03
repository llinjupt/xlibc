#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "xheap.h"
#include "xalloc.h"
#include "xdebug.h"

#define XHEAP_DEFAULT_SLOTS (16)

#define DATA_SIZE (sizeof (void *))
#define PARENT_OF(x) ((x - 1) >> 1)
#define LEFT_OF(x)  (2 * x + 1)
#define RIGHT_OF(x) (2 * x + 2)
#define HAVE_CHILD(x,q) (x < (q)->size >> 1)

struct xheap
{
  /* have allocted slots num */
  unsigned int size;

  /* slots is a dynamic array */
  void  **slots;

  /* have used slots */
  unsigned int used;
  
  void (*heap_free)(void *);
  int (*heap_cmp)(void *, void *);
  int (*heap_find)(void *, void *);
  void (*heap_dump)(void *);
};

/* create a new heap. */
xheap_p xheap_create(void (*heap_free)(void *),
                    int (*heap_cmp)(void *, void *),
                    int (*heap_find)(void *, void *),
                    void (*heap_dump)(void *))
{
  xheap_p heap = NULL;
  
  if(heap_free == NULL
    || heap_cmp == NULL)
    return NULL;

  /* malloc the heap indexs */
  heap = xmalloc(sizeof(xheap_t));

  heap->slots = (void  **)xzalloc(sizeof(void *) * XHEAP_DEFAULT_SLOTS);
 
  heap->size = XHEAP_DEFAULT_SLOTS;
  heap->used = 0;
  
  heap->heap_free = heap_free;
  heap->heap_cmp = heap_cmp;
  heap->heap_find = heap_find;
  heap->heap_dump = heap_dump;
  
  return heap;
}

static void xheap_shift_up(xheap_p heap, unsigned hole_index, void *data)
{
  unsigned parent = (hole_index - 1) >> 1;

  while(hole_index
        && heap->heap_cmp(heap->slots[parent], data) > 0)
  {
    heap->slots[hole_index] = heap->slots[parent];
    hole_index = parent;
    parent = (hole_index - 1) >> 1;
  }

  heap->slots[hole_index] = data;
}

static void xheap_shift_down(xheap_p heap, unsigned hole_index, void *data)
{
   int cmp = 0;
   unsigned left = heap->used - 1;
   unsigned min_child = 2 * (hole_index + 1);

  while(min_child <= left)
  {
    cmp = heap->heap_cmp(heap->slots[min_child], heap->slots[min_child - 1]);
    min_child -= cmp > 0 ? 1 : 0;

    if(heap->heap_cmp(data, heap->slots[min_child]) <= 0)
      break;

    heap->slots[hole_index] = heap->slots[min_child];
    hole_index = min_child;
    min_child = 2 * (hole_index + 1);
  }
  
  heap->slots[hole_index] = data;
}

void *xheap_pop(xheap_p heap)
{
  if(heap->used)
  {
    void *data = heap->slots[0];
    
    xheap_shift_down(heap, 0, heap->slots[heap->used - 1]);
    heap->used--;
    
    return data;
  }

  return NULL;
}

/* NOTE: don't free it outside */
void *xheap_find(xheap_p heap, void *data)
{
  int i = 0;

  if(heap == NULL || heap->heap_find == NULL)
    return NULL;

  for(; i < heap->used; i++)
  {
    if(heap->heap_find(heap->slots[i], data))
      return heap->slots[i];
  }

  return NULL;
}

int xheap_push(xheap_p heap, void *data)
{
  /* need extend ? */
  if(heap->used >= heap->size)
  {
    heap->size *= 2;
    heap->slots = (void  **)realloc(heap->slots, sizeof(void *) * heap->size);
    if(heap->slots == NULL)
      return -1;
  }

  xheap_shift_up(heap, heap->used, data);
  heap->used++;
  
  return 0;
}

void xheap_dump(xheap_p heap)
{
  if(heap)
  {
    int i = 0;
    for(; i < heap->used; i++)
    {
      printf("index %d\n", i);
      heap->heap_dump(heap->slots[i]);
    }
  }
}

void xheap_destroy(xheap_p heap)
{
  if(heap == NULL)
    return;

  int i = 0;
  for(; i < heap->used; i++)
    heap->heap_free(heap->slots[i]);
  
  free(heap->slots);
  xfree(heap);
}

unsigned xheap_size(const xheap_p heap)
{
  assert(heap != NULL);
  
  return heap->used;
}

void *xheap_top(const xheap_p heap)
{
  assert(heap != NULL);

  return heap->used ? heap->slots[0] : NULL;
}

void *xheap_index(const xheap_p heap, int index)
{
  assert(heap != NULL);
  
  if(index >= heap->used)
    return NULL;

  return heap->slots[index];
}

void xheap_traverse(xheap_p heap, int index, void (*handler)(void *data))
{
  if(index < heap->used)
  {
    xheap_traverse(heap, LEFT_OF(index), handler);
    (*handler)(heap->slots[index]);
    xheap_traverse(heap, RIGHT_OF(index), handler);
  }
}

#ifdef TEST
static void *int_alloc(int data)
{
  void *new = malloc(sizeof(int));
  *(int *)new = data;
  
  return new;
}

static void int_free(void *data)
{
  free(data);
}

static int int_cmp(void *a, void *b)
{
  return *(int *)a > *(int *)b;
}

static void int_dump(void *a)
{
  printf("\t%d\n", *(int *)a);
}

void test_xheap()
{
  xheap_p heap = xheap_create(int_free, int_cmp, NULL, int_dump);
  if(heap == NULL)
    return;

  int i = 0;
  for(; i < 10; i++)
    xheap_push(heap, int_alloc(100 - i));

  xheap_push(heap, int_alloc(-1));
  xheap_push(heap, int_alloc(-2));
  xheap_push(heap, int_alloc(0));
  
  for(i = 10; i > 0; i--)
    xheap_push(heap, int_alloc(i));
  xheap_push(heap, int_alloc(-13));
  xheap_dump(heap);

  printf("do xheap_traverse:\n");
  xheap_traverse(heap, 0, int_dump);
  printf("do xheap_traverse over!\n");
  
  printf("xheap_size: %d\n", xheap_size(heap));
  
  void *tmp = NULL;
  while((tmp = xheap_pop(heap)) != NULL)
  {
    int_dump(tmp);
    free(tmp);
  }
  
  printf("xheap_size: %d\n", xheap_size(heap));
  xheap_destroy(heap);
}
#endif
