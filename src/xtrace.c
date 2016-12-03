/* 
 * Report a memory allocation failure and exit.
 * Stole from inetutils-1.8 lib/x(xxx).c for net-walker project. All of below funs'
 * returned value can be freed directly without check NULL.
 * Red_liu <lli_njupt@163.com>  07/27/2010. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>

#include "xdebug.h"
#include "xhash.h"
#include "xalloc.h"

static struct hash *g_alloc_hash = NULL;

const static char alloc_type_names[ALLOC_MAX_TYPE][16] = 
{
  "malloc",
  "zalloc",
  "calloc",
  "realloc",
  "strdup",
  "fopen",
  "vasprintf",
  "vsprintf",
  "nvram_get",
  "record",
};

static void alloc_node_dump(alloc_node_t *data)
{
  printf("%s/%s(%d)= (%-8s,\t%d, %p)\n", data->file, data->func, data->line,
         alloc_type_names[data->type], data->size, data->addr);
}

/* use addr to cacualte key */
static unsigned int alloc_node_hash_key(alloc_node_t *node)
{
  return (unsigned int)node->addr;
}

static int alloc_node_hash_cmp_key(alloc_node_t *node1, alloc_node_t *node2)
{
  return node1->addr == node2->addr;
}

static int alloc_node_hash_cmp_other(alloc_node_t *node1, alloc_node_t *node2)
{
  return 1;
}

static alloc_node_t *alloc_node_find(alloc_node_t *data, void *addr)
{
  if(data->addr == addr)
    return data;
    
  return NULL;
}

static void alloc_node_free(alloc_node_t *node)
{
  if(node == NULL)
    return;

  free(node);
}

/* if filename is ./woola, return woola */
static const char *filename_get(const char *path)
{
  xassert(path);
  char *tmp = rindex(path, '/');

  if(tmp)
    return tmp + 1;

  return path;
}

static alloc_node_t *alloc_node_new(const char *file, const char *func, 
                                  int line,  void *addr, int size, 
                                  alloc_type_t type)
{
  alloc_node_t *new = NULL;

  new = malloc(sizeof(alloc_node_t));
  if(new == NULL)
    return NULL;

  memset(new, 0, sizeof(alloc_node_t));
  new->addr = addr;
  new->size = size;
  
  if(file)
    strncpy(new->file, filename_get(file), 64);

  if(func)
    strncpy(new->func, func, 32);

  new->line = line;
  new->type = type;

  return new;
}

/* insert func atype can't be FREE_TYPE */
alloc_node_t *alloc_node_insert(const char *file, const char *func, int line,
                                void *addr, int size, alloc_type_t atype)
{
  alloc_node_t *find = NULL;
  alloc_node_t *new = NULL;
  
  assert(atype != FREE_TYPE);

  new = alloc_node_new(file, func, line, addr, size, atype);
  assert(new);

  /* first search it */
  find = hash_find(g_alloc_hash, addr);
  if(find == NULL)
  {
    /* a new one */
    new = hash_set(g_alloc_hash, new);
    if(new == NULL)
      xprintf("%s %s %d error, may be malloc error\n", file, func, line);

    return new;
   }

   xprintf("shouldn't happen, file:%s, func:%s, line:%d, new->addr:%p\n", 
             file, func, line, addr);
  alloc_node_dump(find);

   alloc_node_free(new);

  return find;
}

int alloc_node_remove(void *addr)
{
  int ret = 0;
  struct alloc_node *new = alloc_node_new(NULL, NULL, 0, addr, 0, FREE_TYPE);

  if(new == NULL)
    return -1;

  ret = hash_delete(g_alloc_hash, new);

  return ret;
}

void alloc_node_traverse(alloc_node_t *data, void *patten)
{
  if(data == NULL)
    return;
  
  alloc_node_dump(data);
}

inline void xalloc_hash_show0()
{
  if(!g_alloc_hash)
    return;

  printf("\t---------------------------------------------------------->\n");
  hash_traverse(g_alloc_hash, NULL);
  printf("\t<----------------------------------------------------------\n\n\n");
}

int xalloc_hash_create0()
{
  g_alloc_hash = hash_create(alloc_node_hash_key,
                     alloc_node_hash_cmp_key,
                     alloc_node_hash_cmp_other,
                     (void(*)(void *))alloc_node_free,
                     (void(*)(void *))alloc_node_traverse,
                     (void *(*)(void *))alloc_node_find,
                     NULL);

  if(!g_alloc_hash)
    return -1;

#ifdef XALLOC_TRACER
  atexit(xalloc_hash_destroy);
#else
  xalloc_hash_destroy();
#endif  

  return 0;
}

void xalloc_hash_destroy0()
{
  if(!g_alloc_hash)
    return;

  hash_destroy(g_alloc_hash);
  g_alloc_hash = NULL;
}
