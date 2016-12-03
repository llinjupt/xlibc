#include <stdio.h>
#include "xlist.h"

struct xlist_head *xlist_get(struct xlist_head *head)
{
  struct xlist_head *node;

  if(head == NULL)
    return NULL;

  node = head->next;
  if(node == head)
    return NULL;

  xlist_del(node);
  return node;
}
