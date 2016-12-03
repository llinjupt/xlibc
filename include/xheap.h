#ifndef XHEAP_H_
#define XHEAP_H_

typedef struct xheap *xheap_p;
typedef struct xheap xheap_t;

xheap_p xheap_create(void (*heap_free)(void *),
                          int (*heap_cmp)(void *, void *),
                          int (*heap_find)(void *, void *),
                          void (*heap_dump)(void *));

void *xheap_pop(xheap_p heap);
int xheap_push(xheap_p heap, void *data);
void xheap_dump(xheap_p heap);
void *xheap_find(xheap_p heap, void *data);
void xheap_destroy(xheap_p heap);

unsigned xheap_size(const xheap_p heap);
void * xheap_top(const xheap_p heap);
void *xheap_index(const xheap_p heap, int index);

void xheap_traverse(xheap_p heap, int index, void (*handler)(void *data));

#endif /* XHEAP_H_ */
