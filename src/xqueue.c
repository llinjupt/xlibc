#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "xqueue.h"

xqueue_t g_xqueue;

static inline void xqueue_lock(xqueue_t *head)
{
  pthread_mutex_lock(&head->xqueue_rwlock);
}

static inline void xqueue_unlock(xqueue_t *head)
{
  pthread_mutex_unlock(&head->xqueue_rwlock);
}

static unsigned int _xqueue_enqueue(xqueue_t *head, xqnode_t *qnode)
{
  unsigned int current_num = 0;

  xqueue_lock(head);
  xlist_add_tail((struct xlist_head *)qnode, &head->node);
  current_num = ++head->qnode_num;
  xqueue_unlock(head);
  
  return current_num;
}

xqnode_t *xqueue_dequeue(xqueue_t *head)
{
  xqueue_lock(head);

  if(head->qnode_num == 0)
  {
    xqueue_unlock(head);
    return NULL;
  }
  
  xqnode_t *node = (xqnode_t *)xlist_get(&head->node);  
  if(node)
    head->qnode_num--;    
  xqueue_unlock(head);
  
  return node;
}

/*
#define xlist_for_each_entry(pos, head, member)        \
  for (pos = xlist_entry((head)->next, typeof(*pos), member);  \
       prefetch(pos->member.next), &pos->member != (head);   \
       pos = xlist_entry(pos->member.next, typeof(*pos), member))
*/
void xqueue_dump(xqueue_t *head)
{
  xqnode_t *tmp;
  
  assert(head != NULL);
  if(head->dump == NULL)
    return
    
  xqueue_lock(head);
  xlist_for_each_entry(tmp, &head->node, node)
  {
    head->dump(tmp->data);
  }

  xqueue_unlock(head);
}

void xqueue_traverse(xqueue_t *head)
{
  xqnode_t *tmp;
  
  assert(head != NULL);
  if(head->traverse == NULL)
    return
    
  xqueue_lock(head);
  xlist_for_each_entry(tmp, &head->node, node)
  {
    head->traverse(tmp->data);
  }

  xqueue_unlock(head);
}

/*
 * if size is 0 then no limit, -1 with default 
 */
void xqueue_init(int size, xqueue_free_f free, 
                xqueue_dump_f dump, xqueue_traverse_f traverse)
{
  INIT_XLIST_HEAD(&g_xqueue.node);  
  
  //memset(&g_xqueue, 0, sizeof(xqueue_t));
  sem_init(&g_xqueue.sem, 0, 0);  
  
  if(size < 0)
    g_xqueue.max_qnode_num = AQUEUE_MAX_NODES;
  else  
    g_xqueue.max_qnode_num = size;
    
  g_xqueue.qnode_num = 0;
  g_xqueue.free = free;
  g_xqueue.dump = dump;
  g_xqueue.traverse = traverse;
  
  //g_xqueue.xqueue_rwlock = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_init(&g_xqueue.xqueue_rwlock, NULL);
}

int xqueue_enqueue(void *data)
{
  int num = 0;
  
  xqnode_t *qnode;
  if(g_xqueue.max_qnode_num 
    && g_xqueue.qnode_num >= g_xqueue.max_qnode_num)
    return -1;
    
  /*struct agetip_req *req_args = (struct agetip_req *)req->args;    
  xdumphex(p_buff, recv_bytes); */
  
  /* alloc xqueue node */
  if((qnode = (xqnode_t *)malloc(sizeof(xqnode_t)))==NULL)
  {
    printf("memory exhausted\n");
    return -1;
  }
  
  memset(qnode, 0, sizeof(xqnode_t));    
  qnode->data = data;

  /* push request into ipc queue */  
  num = _xqueue_enqueue(&g_xqueue, qnode);
  
  /* awaken handle thread */
  sem_post(&g_xqueue.sem);
  
  return num; 
}

/* must be called as thread func */
void xqueue_handle(xqueue_handle_f handle)
{  
  xqnode_t *node;
  
  assert(handle != NULL);
  
  while(1)
  {
    /* blocking for sem_post */
    sem_wait(&g_xqueue.sem);
    while((node = xqueue_dequeue(&g_xqueue)) != NULL)
    {
      handle(node->data);
      if(g_xqueue.free)
        g_xqueue.free(node->data);
      free(node);
    }
  }
}
