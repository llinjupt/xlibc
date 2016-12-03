/* Report a memory allocation failure and exit.

   Copyright (C) 1997, 1998, 1999, 2000, 2002, 2003, 2004, 2006, 2009, 2010
   Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */
   
#ifndef XALLOC_H
#define XALLOC_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <fcntl.h>
#include <malloc.h>
#include <stddef.h>

/**************************************Trace FUNCS**************************************/

//#define XALLOC_TRACER

#ifdef XALLOC_TRACER
#define alloc_insert_hook alloc_node_insert2
#define alloc_remove_hook alloc_node_remove2
#else
#define alloc_insert_hook(x...) 
#define alloc_remove_hook(x...) (0)
#endif

/* 
 * 不同分配内存的函数对应不同的分配类型，如果发现没有收录的，但是却会动态分配内存的函数，
 * 应该在这里添加新的类型，Note:如果修改了此结构体，一并需要修改 alloc_type_names
 */
typedef enum alloc_type
{
  /* FREE_TYPE must be -1, do not change it */
  FREE_TYPE = -1,
  MALLOC_TYPE,
  ZALLOC_TYPE,
  CALLOC_TYPE,
  REALLOC_TYPE,
  STRDUP_TYPE,
  FOPEN_TYPE,
  VASPRINTF_TYPE,
  VSPRINTF_TYPE,
  NVRAM_TYPE,
  RECORD_TYPE,
  /* add new type below */

  /* this must be the tail */
  ALLOC_MAX_TYPE,
}alloc_type_t;

/* 一个内存分配/释放点，它包含文件名，函数名，行号; 分配内存的地址，大小 */
typedef struct alloc_node
{
  void *addr;

  size_t size;          /* size allocted to current addr */
    
  char file[64];
  char func[32];

  int line;

  alloc_type_t type;             /* alloc type */
}alloc_node_t;

/* 一个内存分配/释放点，它包含文件名，函数名，行号; 分配内存的地址，大小 */
typedef struct alloc_node2
{
  /* if malloc then ++, if free then -- */
  unsigned int count;
  
  char file[64];
  char func[32];

  int line;

  alloc_type_t type;  /* alloc type */

  /* an malloc array, size was indicated by count */
  void     **addr_array;
  unsigned *size_array;
}alloc_node2_t;

int xalloc_hash_create();
void xalloc_hash_destroy();
void xalloc_hash_show();

alloc_node_t *alloc_node_insert(const char *file, const char *func, int line,
                                void *addr, int size, alloc_type_t atype);
int alloc_node_remove(void *addr);

alloc_node2_t *alloc_node_insert2(const char *file, const char *func, int line,
                                void *addr, int size, alloc_type_t atype);
int alloc_node_remove2(void *addr);

/**************************************MEM FUNCS**************************************/
FILE *memcheck_fopen(const char *file,
                      int line,
                      const char *func,
                      const char *path,
                      const char *mode);

int memcheck_fclose(const char *file,
                      int line,
                      const char *func,
                      FILE *fp);

#define xfopen(path, mode) \
   memcheck_fopen(__FILE__,__LINE__, __FUNCTION__, path, mode)
#define xfclose(fp) \
   memcheck_fclose(__FILE__,__LINE__, __FUNCTION__, fp)
   
void *memcheck_malloc(const char *file,
                      int line,
                      const char *func,
                      size_t size);

void *memcheck_zalloc(const char *file,
                      int line,
                      const char *func,
                      size_t size);

void memcheck_free(const char *file,
                   int line,
                   const char *func,
                   void *ptr);

int memcheck_vasprintf(const char *file,
                   int line,
                   const char *func,
                   char **result,
                   const char *fmt,
                   va_list arg);

int memcheck_vsprintf(const char *file,
                   int line,
                   const char *func,
                   char **result,
                   const char *fmt, ...);

void memcheck_nvram_get(const char *file,
                      int line,
                      const char *func,
                      void *addr);

void memcheck_record(const char *file,
                      int line,
                      const char *func,
                      void *addr);

/* alloc funs without zeroed */
#define xnvram_get(addr) \
   memcheck_nvram_get(__FILE__,__LINE__, __FUNCTION__, addr)

#define xalloc_trace_record(addr) \
   memcheck_record(__FILE__,__LINE__, __FUNCTION__, addr)
   
/* alloc funs without zeroed */
#define xmalloc(size) \
   memcheck_malloc(__FILE__,__LINE__, __FUNCTION__, size)

#define xfree(ptr) \
   memcheck_free(__FILE__,__LINE__, __FUNCTION__, ptr)

#define xvasprintf(ret, fmt, arg) \
   memcheck_vasprintf(__FILE__,__LINE__, __FUNCTION__, ret, fmt, arg)

#define xvsprintf(ret, fmt, x...) \
   memcheck_vsprintf(__FILE__,__LINE__, __FUNCTION__, ret, fmt, x)

/* alloc funs with zeroed */
#define xcalloc(nmemb,size) \
   memcheck_calloc(__FILE__,__LINE__, __FUNCTION__, nmemb, size)

#define xzalloc(size) \
   memcheck_zalloc(__FILE__,__LINE__, __FUNCTION__, size)

char *memcheck_strdup(const char *file,
                      int line,
                      const char *func,
                      const char *str);
#define xstrdup(str) \
   memcheck_strdup(__FILE__, __LINE__, __FUNCTION__, str)

void *memcheck_realloc(const char *file,
                       int line,
                       const char *func,
                       void *ptr,
                       size_t size);
#define xrealloc(ptr, size) \
   memcheck_realloc(__FILE__, __LINE__, __FUNCTION__, ptr, size)

void *xnalloc(size_t n, size_t s);
//inline void *xalloca(size_t n);

/* clone funs */
void *xmemdup(void const *p, size_t s);
char* xstrndup(const char *s, int n);

#ifndef xalloc
#define xalloc(n) xmalloc(n)
#endif

#ifndef xalloc_die
#define xalloc_die()  xdie("Memory exhausted!\n")
#endif 

/*
 * Xalloca must be defined as a MACRO for which is allocate mem
 * from stack and will be released automatically during current fun 
 * returns. We have to give it pointer x for MACRO limited! 
 */
#define xalloca(x, n) (((x = alloca(n)) == NULL) ? xmalloc(-1) : x)

/* Return 1 if an array of N objects, each of size S, cannot exist due
   to size arithmetic overflow.  S must be positive and N must be
   nonnegative.  This is a macro, not an inline function, so that it
   works correctly even when SIZE_MAX < N.

   By gnulib convention, SIZE_MAX represents overflow in size
   calculations, so the conservative dividend to use here is
   SIZE_MAX - 1, since SIZE_MAX might represent an overflowed value.
   However, malloc (SIZE_MAX) fails on all known hosts where
   sizeof (ptrdiff_t) <= sizeof (size_t), so do not bother to test for
   exactly-SIZE_MAX allocations on such hosts; this avoids a test and
   branch when S is known to be 1.  */
#define xalloc_oversized(n, s) \
    ((size_t) (sizeof (ptrdiff_t) <= sizeof (size_t) ? -1 : -2) / (s) < (n))

#endif /* XALLOC_H */
