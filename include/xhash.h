#ifndef XHASH_H
#define XHASH_H

#include <pthread.h>

/* Default hash table size.  */ 
#define HASH_TABLE_SIZE     32

struct hash_bucket
{
  /* Linked list.  */
  struct hash_bucket *next;

  /* Hash key. */
  unsigned int key;

  /* Data.  */
  void *data;
};

struct hash
{
  /* Hash bucket. */
  struct hash_bucket **index;

  /* Hash table size. */
  unsigned int size;

  /* Key make function. */
  unsigned int (*hash_key)();

  /* Data compare function. */
  int (*hash_cmp_key)();
  int (*hash_cmp_other)();
  
  void (*hash_traverse)();
  void *(*hash_find)();
//  void *(*hash_new_data)();
  void (*hash_free_data)();
  
  int (*hash_buffer)();
  
  /* Bucket alloc. */
  unsigned long count;

  /* lock for hash read and write */
  pthread_mutex_t  hash_rwlock;
};

struct hash *hash_create_size(unsigned int size, 
                              unsigned int (*hash_key)(),
                              int (*hash_cmp_key)(),
                              int (*hash_cmp_other)(),
                            //  void *(*hash_new_data)(),
                              void (*hash_free_data)(),
                              void (*hash_traverse)(),
                              void *(*hash_find)(),
                              int (*hash_buffer_func)());


struct hash *hash_create(unsigned int (*hash_key)(), 
                         int (*hash_cmp_key)(),
                        int (*hash_cmp_other)(),
                        //void *(*hash_new_data)(),
                        void (*hash_free_data)(),
                        void (*hash_traverse)(),
                        void *(*hash_find)(),
                        int (*hash_buffer_func)());
                        
void *hash_get (struct hash *, void *);
void *hash_set(struct hash *hash, void *data);
void *hash_alloc_intern (void *);
void *hash_lookup (struct hash *, void *);
void *hash_release (struct hash *, void *);

/* diff from hash_lookup with pattern */
void *hash_find(struct hash *table, void*);
void hash_traverse(struct hash *table, void*);
void hash_destroy(struct hash *hash);
int hash_delete(struct hash *hash, void *data);

void hash_iterate (struct hash *, 
       void (*) (struct hash_bucket *, void *), void *);

void hash_clean (struct hash *);
void hash_free (struct hash *);
int hash_buffer(struct hash *hash, char **data);

extern inline void hash_lock(struct hash *hash);
extern inline void hash_unlock(struct hash *hash);
#endif /* XHASH_H */
