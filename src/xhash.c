#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

#include <libx.h>
#include "xhash.h"

inline void hash_lock(struct hash *hash)
{
  pthread_mutex_lock(&hash->hash_rwlock);
}

inline void hash_unlock(struct hash *hash)
{
  pthread_mutex_unlock(&hash->hash_rwlock);
}

/* Allocate a new hash.  */
struct hash *hash_create_size(unsigned int size, 
                              unsigned int (*hash_key)(),
                              int (*hash_cmp_key)(),
                              int (*hash_cmp_other)(),
                              //void *(*hash_new_data)(),
                              void (*hash_free_data)(),
                              void (*hash_traverse)(),
                              void *(*hash_find)(),
                              int (*hash_buffer_func)())
{
  struct hash *hash;
  
  /* malloc the hash indexs */
  hash = malloc(sizeof (struct hash));
  if(hash == NULL)
    return NULL;

  hash->index = malloc(sizeof(struct hash_bucket *) * size);
  if(hash->index == NULL)
  {
    free(hash);
    return NULL;
  }

  memset(hash->index, 0, sizeof(struct hash_bucket *) * size);

  hash->size = size;
  hash->hash_key = hash_key;
  hash->hash_cmp_key = hash_cmp_key;
  hash->hash_cmp_other = hash_cmp_other;
  hash->hash_free_data = hash_free_data;
// hash->hash_new_data = hash_new_data;
  hash->hash_find = hash_find;
  hash->hash_traverse = hash_traverse;
  hash->hash_buffer = hash_buffer_func;
  
  hash->count = 0;
  
  pthread_mutex_init(&hash->hash_rwlock, NULL);
  
  return hash;
}

/* Allocate a new hash with default hash size.  */
struct hash *hash_create(unsigned int (*hash_key)(), 
                         int (*hash_cmp_key)(),
                        int (*hash_cmp_other)(),
                        //void *(*hash_new_data)(),
                        void (*hash_free_data)(),
                        void (*hash_traverse)(),
                        void *(*hash_find)(),
                        int (*hash_buffer_func)())
{
  return hash_create_size(HASH_TABLE_SIZE, hash_key, 
                          hash_cmp_key, 
                          hash_cmp_other, 
                          //hash_new_data, 
                          hash_free_data,
                          hash_traverse,
                          hash_find,
                          hash_buffer_func);
}

/* Utility function for hash_get().  When this function is specified
   as alloc_func, return arugment as it is.  This function is used for
   intern already allocated value.  */
void *hash_alloc_intern (void *arg)
{
  return arg;
}

/* Lookup and return hash bucket in hash.  If there is no
   corresponding hash bucket and alloc_func is specified, create new
   hash bucket.  */
void *hash_get(struct hash *hash, void *data)
{
  unsigned int key;
  unsigned int index;
  struct hash_bucket *bucket;

  key = (*hash->hash_key)(data);
  index = key % hash->size;

  hash_lock(hash);
  for(bucket = hash->index[index]; bucket != NULL; bucket = bucket->next)
  {
    if(bucket->key == key && (*hash->hash_cmp_key)(bucket->data, data))
    {
      hash_unlock(hash);
      return bucket->data;
    }
  }
  hash_unlock(hash);

  return NULL;
}

/* delete data by data key */
int hash_delete(struct hash *hash, void *data)
{
  unsigned int key;
  unsigned int index;
  struct hash_bucket *bucket, *prev;

  if(hash->hash_free_data == NULL)
  {
    printf("should provide free function!\n");
    return -1;
  }
  
  key = (*hash->hash_key)(data);
  index = key % hash->size;

  hash_lock(hash);  
  for(prev = bucket = hash->index[index]; bucket != NULL; bucket = bucket->next)
  {
    if(bucket->key == key && (*hash->hash_cmp_key)(bucket->data, data))
    {
      if(bucket == prev)
        hash->index[index] = bucket->next;
      else
        prev->next = bucket->next;
        
      (*(hash->hash_free_data))(bucket->data);
      free(bucket);
      hash->count--;
      hash_unlock(hash);
      return 0;
    }
    else
      prev = bucket;  
  }
  hash_unlock(hash);  
  
  return -1;
}

/* Lookup and replace it. Note: the data must be malloced and not be free any more.
   Even it have been existed.*/
void *hash_set(struct hash *hash, void *data)
{
  unsigned int key;
  unsigned int index;
  
  struct hash_bucket *bucket;
  
  if(data == NULL)
    return NULL;
  
  key = (*hash->hash_key)(data);
  index = key % hash->size;
  
  hash_lock(hash);
  for(bucket = hash->index[index]; bucket != NULL; bucket = bucket->next)
  {
    if(bucket->key == key && (*hash->hash_cmp_key)(bucket->data, data))
    {
      if((*hash->hash_cmp_other)(bucket->data, data))
      {
        /* if existed free the data parameter so no need to free it out this func! */
        (*hash->hash_free_data)(data);
        hash_unlock(hash);
        return bucket->data;
      }
      else
      {
        /* free the old and update as new data */
        (*hash->hash_free_data)(bucket->data);
        bucket->data = data;
        hash_unlock(hash);
        return bucket->data;
      }
    }
  }
  
  /* if not searched then alloc a new bucker */
  bucket = malloc(sizeof (struct hash_bucket));
  bucket->data = data;
  bucket->key = key;
  bucket->next = hash->index[index];
  hash->index[index] = bucket;
  hash->count++;

  hash_unlock(hash);  
  
  return bucket->data;
}

/* Hash lookup.  */
void *hash_lookup(struct hash *hash, void *data)
{
  return hash_get(hash, data);
}

/* This function release registered value from specified hash.  When
   release is successfully finished, return the data pointer in the
   hash bucket. */
void *hash_release(struct hash *hash, void *data)
{
  void *ret;
  unsigned int key;
  unsigned int index;
  struct hash_bucket *bucket;
  struct hash_bucket *pp;

  key = (*hash->hash_key) (data);
  index = key % hash->size;

  hash_lock(hash);
  for(bucket = pp = hash->index[index]; bucket; bucket = bucket->next)
  {
    if (bucket->key == key && (*hash->hash_cmp_key)(bucket->data, data)) 
    {
      if(bucket == pp)
        hash->index[index] = bucket->next;
      else
        pp->next = bucket->next;

      ret = bucket->data;
      free(bucket);

      hash->count--;
      hash_unlock(hash);
      
      return ret;
    }
    
    pp = bucket;
  }

  hash_unlock(hash);  
  return NULL;
}

/* Iterator function for hash.  */
void hash_iterate (struct hash *hash, 
                   void(*func)(struct hash_bucket *, void *),
                   void *arg)
{
  int i;
  struct hash_bucket *hb;
  
  hash_lock(hash);
  for (i = 0; i < hash->size; i++)
    for (hb = hash->index[i]; hb; hb = hb->next)
      (*func) (hb, arg);
  hash_unlock(hash);    
}

/* Clean up hash.  */
void hash_clean(struct hash *hash)
{
  int i;
  struct hash_bucket *hb;
  struct hash_bucket *next;
  
  if(hash->hash_free_data == NULL)
    return;

  hash_lock(hash);  
  for(i = 0; i < hash->size; i++)
  {
    for(hb = hash->index[i]; hb; hb = next)
    {
      next = hb->next;
      
      if(hb->data)
        (*(hash->hash_free_data))(hb->data);
      
      free(hb);
      hash->count--;
    }
      
    hash->index[i] = NULL;
  }
  hash_unlock(hash);
}

/* Clean up hash.  */
void hash_traverse(struct hash *hash, void *filter)
{
  int i = 0;
  
  struct hash_bucket *hb;
  struct hash_bucket *next;

  if(hash->hash_traverse == NULL)
    return;
  
  hash_lock(hash);
  for (i = 0; i < hash->size; i++)
  {
    //xpprintf("index:%d\n", i);
    for(hb = hash->index[i]; hb; hb = next)
    {
      next = hb->next;
      (*hash->hash_traverse)(hb->data, filter);
    }
  }
  hash_unlock(hash);
}

void *hash_find(struct hash *hash, void *pattern)
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
      ret = (*hash->hash_find)(hb->data, pattern);
      
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

int hash_buffer(struct hash *hash, char **data)
{
  /* if data not enough then extened it by XMALLOC_STEP bytes */  
#define XMALLOC_STEP  128
  
  int i = 0, len = 0;
  int used_len = 0, empty_len = 0, blocks = 1;
  
  char *tmp = NULL;
  struct hash_bucket *hb;
  struct hash_bucket *next;
  
  if(hash->hash_buffer == NULL)
    return -1;

  empty_len = XMALLOC_STEP * (blocks++);
  *data = malloc(empty_len);
  used_len = 0;
  
  hash_lock(hash);
  for(i = 0; i < hash->size; i++)
  {
    for(hb = hash->index[i]; hb; hb = next)
    {
      next = hb->next;
      
      if(hb->data == NULL)
        continue;
        
      len = (*(hash->hash_buffer))(hb->data, &tmp);
      
      //xdumphex(tmp, len);      
      if(len > 0)
      {
        if(len > empty_len)
        {
          *data = realloc(*data, XMALLOC_STEP * (blocks++));
          empty_len += XMALLOC_STEP;
        }
        
        memcpy(*data + used_len, tmp, len);
        used_len += len;
        empty_len -= len;
        
        free(tmp);
      }
    }
  }
  hash_unlock(hash);
  
  if(used_len == 0)
    free(data);
  
  return used_len == 0 ? -1 : used_len;
}

/* Free hash memory.  You may call hash_clean before call this
   function.  */
void hash_free(struct hash *hash)
{
  free(hash->index);
  free(hash);
}

void hash_destroy(struct hash *hash)
{
  if(hash)
  {
    hash_clean(hash);
    hash_free(hash);
    hash = NULL;
  }
}
