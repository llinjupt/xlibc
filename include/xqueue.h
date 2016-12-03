/*
 * The asynchronous queue to slow down some hasty request
 */
#ifndef AQUEUE_H
#define AQUEUE_H

#include <semaphore.h>
#include <pthread.h>

#include "xlist.h"

#define AQUEUE_MAX_NODES 1024

typedef void (*xqueue_dump_f)(void *data);
typedef void (*xqueue_traverse_f)(void *data);
typedef void (*xqueue_free_f)(void *data);
typedef void (*xqueue_handle_f)(void *data);

/* asynchronous queue node */
typedef struct
{
  struct xlist_head node;
  void *data;
}xqnode_t;

/* asynchronous queue header */
typedef struct
{
  struct xlist_head node;  
  sem_t sem;
  
  unsigned int qnode_num;    /* current nodes */
  unsigned int max_qnode_num;
  
  xqueue_dump_f dump;
  xqueue_traverse_f traverse;
  xqueue_free_f free;

  /* lock for this queue */
  pthread_mutex_t  xqueue_rwlock;
}xqueue_t;

void xqueue_init(int size, xqueue_free_f free, 
     xqueue_dump_f dump, xqueue_traverse_f traverse);
void xqueue_traverse(xqueue_t *head);
void xqueue_dump(xqueue_t *head);

int xqueue_enqueue(void *data);
/* must be called as thread func */
void xqueue_handle(xqueue_handle_f handle);

#endif /* AQUEUE_H */
