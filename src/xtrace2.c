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
#include <pthread.h>

#include "xdebug.h"
#include "xhash.h"
#include "xalloc.h"

pthread_mutex_t g_alloc_rwlock = PTHREAD_MUTEX_INITIALIZER;
static struct hash *g_alloc_hash = NULL;

static void alloc_lock()
{
  pthread_mutex_lock(&g_alloc_rwlock);
}

static void alloc_unlock()
{
  pthread_mutex_unlock(&g_alloc_rwlock);
}

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

static void alloc_node_dump(alloc_node2_t *data)
{
  int i = 0;
  printf("%s/%s(%d)= (%-8s %d)\n", data->file, data->func, data->line,
         alloc_type_names[data->type], data->count);

  for(; i < data->count; i++)
  {
    printf("\t[%d] %p %-8u\n", i, data->addr_array[i], data->size_array[i]);
  }
}

/* use addr to cacualte key */
static unsigned int alloc_node_hash_key(alloc_node2_t *node)
{
  int i = 0;
  unsigned int key = 0;

  while(node->file[i] != '\0')
  {
    key += node->file[i];
    i++;
  }
  
  i = 0;
  while(node->func[i] != '\0')
  {
    key += node->func[i];
    i++;
  }
  
  key += node->line;
  
  return key;
}

static int alloc_node_hash_cmp_key(alloc_node2_t *node1, alloc_node2_t *node2)
{
  if(strcmp(node1->file, node2->file) == 0
     && strcmp(node1->func, node2->func) == 0
     && (node1->line == node2->line))
    return 1;
  
   return 0;
}

static int alloc_node_hash_cmp_other(alloc_node2_t *node1, alloc_node2_t *node2)
{
  return 1;
}

static alloc_node2_t *alloc_node_find(alloc_node2_t *data, alloc_node2_t *pattern)
{
  if(alloc_node_hash_cmp_key(data, pattern) == 1)
    return data;

  return NULL;
}

static alloc_node2_t *alloc_node_find_byaddr(alloc_node2_t *data, void *addr)
{
  int i = 0;
  assert(data->count > 0);

  for(; i < data->count; i++)
  {
    if(data->addr_array[i] == addr)
      return data;
  }

  return NULL;
}

static void alloc_node_remove_addr(alloc_node2_t *data, void *addr)
{
  int i = 0;
  assert(data->count > 0);

  for(; i < data->count; i++)
  {
    if(data->addr_array[i] == addr)
      break;
  }

  if(i == data->count)
    return;

  data->count--;
  
  /* not last one, swap with the last one */
  if(i != data->count)
  {
    data->addr_array[i] = data->addr_array[data->count];
    data->size_array[i] = data->size_array[data->count];
  }
  
  if(data->count == 0)
  {
    if(data->addr_array)
    {
      free(data->addr_array);
      data->addr_array = NULL;
    }
    
    if(data->size_array)
    {
      free(data->size_array);
      data->size_array = NULL;
    }
  }
}

static void alloc_node_free(alloc_node2_t *node)
{
  if(node == NULL)
    return;
  
  if(node->addr_array)
  {
    free(node->addr_array);
    node->addr_array = NULL;
  }

  if(node->size_array)
  {
    free(node->size_array);
    node->size_array = NULL;
  }  
  
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

static alloc_node2_t *alloc_node_new(const char *file, const char *func, 
                                  int line,  void *addr, int size, alloc_type_t type)
{
  alloc_node2_t *new = NULL;

  new = malloc(sizeof(alloc_node2_t));
  if(new == NULL)
    return NULL;

  memset(new, 0, sizeof(alloc_node2_t));

  if(file)
    strncpy(new->file, filename_get(file), 64);

  if(func)
    strncpy(new->func, func, 32);

  new->line = line;
  new->type = type;

  /* init the count */
  new->count = 1;

  new->addr_array = realloc(new->addr_array, sizeof(void *));
  assert(new->addr_array);
  new->addr_array[0] = addr;

  new->size_array = realloc(new->size_array, sizeof(unsigned));
  assert(new->size_array);
  new->size_array[0] = (unsigned)size;

  return new;
}

/* insert func atype can't be FREE_TYPE */
alloc_node2_t *alloc_node_insert2(const char *file, const char *func, int line,
                                void *addr, int size, alloc_type_t atype)
{
  alloc_node2_t *find = NULL;
  alloc_node2_t *new = NULL;
  
  assert(atype != FREE_TYPE);

  new = alloc_node_new(file, func, line, addr, size, atype);
  assert(new);

  /* first search it */
  alloc_lock();
  find = hash_find(g_alloc_hash, new);
  if(find == NULL)
  {
    /* a new one */
    new = hash_set(g_alloc_hash, new);
    if(new == NULL)
      xprintf("%s %s %d error, may be malloc error\n", file, func, line);

    alloc_unlock();
    return new;
   }

  find->count++;
  find->addr_array = realloc(find->addr_array, find->count * sizeof(void *));
  assert(find->addr_array);
  find->addr_array[find->count - 1] = addr;

  find->size_array = realloc(find->size_array, find->count * sizeof(unsigned));
  assert(find->size_array);
  find->size_array[find->count - 1] = size;
  alloc_unlock();
  alloc_node_free(new);

  return find;
}

inline void hash_lock(struct hash *hash);
inline void hash_unlock(struct hash *hash);
static void *node_hash_find_byaddr(struct hash *hash, void *addr)
{
  int i = 0;
  void *ret = NULL;
  
  struct hash_bucket *hb;
  struct hash_bucket *next;

  if(hash->hash_find == NULL)
    return 0;
  
  hash_lock(hash);
  for (i = 0; i < hash->size; i++)
  {
    for(hb = hash->index[i]; hb; hb = next)
    {
      next = hb->next;
      ret = alloc_node_find_byaddr(hb->data, addr);
      if(ret)
      {
        hash_unlock(hash);
        return ret;
      }
    }
  }
  
  hash_unlock(hash);
  return NULL;
}

int alloc_node_remove2(void *addr)
{
  int ret = 0;

  /* first search it */
  alloc_node2_t *find = NULL;

  alloc_lock();
  find = node_hash_find_byaddr(g_alloc_hash, addr);
  if(find == NULL)
  {
     xwprintf("shouldn't happen, can't find addr %p while do free!\n", addr);
    alloc_unlock();
     return -1;
   }

  alloc_node_remove_addr(find, addr);
  if(find->count == 0)
    ret = hash_delete(g_alloc_hash, find);

  alloc_unlock();
  return ret;
}

void alloc_node_traverse2(alloc_node2_t *data, void *patten)
{
  if(data == NULL)
    return;
  
  alloc_node_dump(data);
}

inline void xalloc_hash_show()
{
  if(!g_alloc_hash)
    return;

  printf("\t---------------------------------------------------------->\n");
  hash_traverse(g_alloc_hash, NULL);
  printf("\t<----------------------------------------------------------\n\n\n");
}

int xalloc_hash_create()
{
  g_alloc_hash = hash_create(alloc_node_hash_key,
                     alloc_node_hash_cmp_key,
                     alloc_node_hash_cmp_other,
                     (void(*)(void *))alloc_node_free,
                     (void(*)(void *))alloc_node_traverse2,
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

void xalloc_hash_destroy()
{
  if(!g_alloc_hash)
    return;

  hash_destroy(g_alloc_hash);
  g_alloc_hash = NULL;
}
