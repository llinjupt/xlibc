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

#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <alloca.h>

#include "xdebug.h"
#include "xalloc.h"

#define ALLOC_DUMP_FD stdout
#ifdef DEBUG
#define alloc_printf _xprintf
#else
#define alloc_printf(x...)
#endif

FILE *memcheck_fopen(const char *file,
                      int line,
                      const char *func,
                      const char *path,
                      const char *mode)
{
  FILE *fp = fopen(path, mode);

  return fp;
}

int memcheck_fclose(const char *file,
                      int line,
                      const char *func,
                      FILE *fp)
{
  return fclose(fp); 
}

/* #define xalloc_die  xdie("Memory exhausted!\n") */

void *memcheck_malloc(const char *file,
                      int line,
                      const char *func,
                      size_t size)
{
  void *p = malloc(size);
  
  if(p == NULL)
  {
    _xprintf(ALLOC_DUMP_FD, file, line, func, "xmalloc:%p, %s!\n", p, p ? "OK" : "Fail");  
    xalloc_die();
  }
  alloc_insert_hook(file, func, line,  p, size, MALLOC_TYPE);

  return p; 
}

void memcheck_nvram_get(const char *file,
                      int line,
                      const char *func,
                      void *addr)
{
  if(addr == NULL)
  {
    char *tmp = rindex(file, '/');
    fprintf(stderr, "=%s/%s(%d)= encount null addr\n", tmp ? tmp + 1 : file, func, line);   
    return;
  }

  alloc_insert_hook(file, func, line,  addr, 0, NVRAM_TYPE);
}

void memcheck_record(const char *file,
                      int line,
                      const char *func,
                      void *addr)
{
  if(addr == NULL)
  {
    char *tmp = rindex(file, '/');
    fprintf(stderr, "=%s/%s(%d)= encount null addr\n", tmp ? tmp + 1 : file, func, line);   
    return;
  }

  alloc_insert_hook(file, func, line,  addr, 0, RECORD_TYPE);
}

/*
 * xalloca must be defined as a MACRO for which is allocate mem from stack 
 * and will be released automatically when current returned
 *  #define xalloca(n) in xalloc.h
 */

/* Change the size of an allocated block of memory P to N bytes,
   with error checking.  */
void *memcheck_realloc(const char *file,
                       int line,
                       const char *func,
                       void *ptr,
                       size_t size)
{
  void *p = realloc(ptr, size);
  if(p == NULL)
  {
    _xprintf(ALLOC_DUMP_FD, file, line, func, "xrealloc:%p, %s!\n", p, p ? "OK" : "Fail");  
    xalloc_die();
  }
    
  if(alloc_remove_hook(ptr) < 0)
  {
    char *tmp = rindex(file, '/');
    fprintf(stderr, "=%s/%s(%d)= ", tmp ? tmp + 1 : file, func, line);    
  }
  
  alloc_insert_hook(file, func, line,  p, size, REALLOC_TYPE);  
  
  return p;
}  

void *memcheck_zalloc(const char *file,
                       int line,
                       const char *func,
                       size_t size)
{
  void *p = malloc(size);
  if(p == NULL)
  {
    _xprintf(ALLOC_DUMP_FD, file, line, func, "xzalloc:%p, %s!\n", p, p ? "OK" : "Fail");  
    xalloc_die();
  }
    
  alloc_insert_hook(file, func, line,  p, size, ZALLOC_TYPE);
  memset(p, 0, size);
  
  return p;
}  

/* Allocate without zeroed memory for N elements of S bytes, with error
   checking.  S must be nonzero.  */
void *xnalloc(size_t n, size_t s)
{
  return xmalloc(n * s);
}

/* Allocate zeroed memory for N elements of S bytes, with error
   checking.  S must be nonzero.  */
void *memcheck_calloc(const char *file,
                      int line,
                      const char *func,
                      size_t n,
                      size_t size)
{
  void *p = calloc(n, size);
  if(p == NULL)
  {
    _xprintf(ALLOC_DUMP_FD, file, line, func, "xcalloc:%p, %s!\n", p, p ? "OK" : "Fail");  
    xalloc_die();
  }
  
  alloc_insert_hook(file, func, line,  p, size, CALLOC_TYPE);

  return p;
}

/* Clone an object P of size S, with error checking.  There's no need
   for xnmemdup (P, N, S), since xmemdup (P, N * S) works without any
   need for an arithmetic overflow check.  */
void *xmemdup(void const *p, size_t s)
{
  return memcpy(xmalloc(s), p, s);
}

char *memcheck_strdup(const char *file,
                      int line,
                      const char *func,
                      const char *str)
{
  char *p = strdup(str);
  if(p == NULL)
  {
    _xprintf(ALLOC_DUMP_FD, file, line, func, "xstrdup:%p, %s!\n", p, p ? "OK" : "Fail");  
    xalloc_die();
  }
  
  alloc_insert_hook(file, func, line,  p, strlen(str), STRDUP_TYPE);

  return p;
}

/* Die if we can't allocate n+1 bytes (space for the null terminator) 
 * and copy the (possibly truncated to length n) string into it.
 * So carefully given n should be suitable without wastage and n>=0.
*/
char* xstrndup(const char *s, int n)
{
  int len = 0;
  char *p;
  
  if(s)
    len = strlen(s);
  
  p = xmalloc(n + 1);
  p[n] = '\0';

  len = len > n ? n : len;

  return memcpy(p, s, len);
}

void memcheck_free(const char *file,
                   int line,
                   const char *func,
                   void *ptr)
{
  alloc_printf(ALLOC_DUMP_FD, file, line, func, "xfree:%p!\n", ptr);  
  if(ptr)
    free(ptr);

  if(alloc_remove_hook(ptr) < 0)
  {
    char *tmp = rindex(file, '/');
    fprintf(stderr, "=%s/%s(%d)= ", tmp ? tmp + 1 : file, func, line);    
  }
  
  return;
}

int memcheck_vsprintf(const char *file,
                   int line,
                   const char *func,
                   char **result,
                   const char *fmt, ...)
{
  int ret = 0;
  va_list arg;

  *result = NULL;

  va_start(arg, fmt);
  ret = vasprintf(result, fmt, arg);
  if(ret < 0)
  {
    va_end(arg);
    return ret;
  }
  va_end(arg);

  alloc_insert_hook(file, func, line,  *result, strlen(*result), VSPRINTF_TYPE);

  return ret;
}

int memcheck_vasprintf(const char *file,
                   int line,
                   const char *func,
                   char **result,
                   const char *fmt,
                   va_list arg)
{
  int ret = 0;
  
  ret = vasprintf(result, fmt, arg);
  if(ret < 0)
    return ret;

  alloc_insert_hook(file, func, line,  *result, strlen(*result), VASPRINTF_TYPE);

  return ret;
}

#ifdef TEST

/* if test this, please enable XALLOC_TRACER in xalloc.h */
#define MALLOC_TIMES 100
static void test_xrealloc()
{
  int i  = 0;
  char *ret[MALLOC_TIMES];

  for(; i < MALLOC_TIMES; i++)
  {
    ret[i] = xmalloc(100);

    if(i % 3 != 0)
    {
      if(ret[i])
        ret[i] = xrealloc(ret[i], 200);
    }
  }

  for(i = 0; i < MALLOC_TIMES; i++)
  {
    if(ret[i])
      xfree(ret[i]);
  }
}

static void test_xstrdup()
{
  int i  = 0;

  char *ret[MALLOC_TIMES];

  for(; i < MALLOC_TIMES; i++)
  {
    ret[i] = xstrdup("hello");

    if(i % 10 != 0)
    {
      xfree(ret[i]);
      ret[i] = NULL;
    }
  }

  for(i = 0; i < MALLOC_TIMES; i++)
  {
    if(ret[i])
      xfree(ret[i]);
  }
}

static void test_xmalloc()
{
  int i = 0;
  char *ret[MALLOC_TIMES];

  for(; i < MALLOC_TIMES; i++)
  {
    ret[i] = xmalloc(10 + i);
    xassert(ret[i]);
    
    if(i % 10 != 0)
    {
      xfree(ret[i]);
      ret[i] = NULL;
    }
  }

  for(i = 0; i < MALLOC_TIMES; i++)
  {
    if(ret[i])
      xfree(ret[i]);
  }
}

void test_xvasprintf(const char *fmt, ...)
{
  int ret = 0;
  char *val = NULL;

  va_list arg;
  va_start(arg, fmt);
  ret = xvasprintf(&val, fmt, arg);
  if(ret < 0)
  {
    xprintf("Memory exhausted or fmt error!\n");
    va_end(arg);
  }
  va_end(arg);

  printf("val:%s\n", val);
  xfree(val);
}

static void test_xvsprintf()
{
  char *ret = NULL;
  int num = xvsprintf(&ret, "%s %d\n", "hello", 1);
  printf("xvsprintf num %d, ret:%s\n", num, ret);
  
  xfree(ret);
}

void test_xalloc()
{
  xalloc_hash_create();  
  
  test_xmalloc();
  xalloc_hash_show();
  
  test_xrealloc();
  xalloc_hash_show();
  
  test_xstrdup();
  
  test_xvsprintf();
  test_xvasprintf("%s %d\n", "hello", 123);
  xalloc_hash_show();
}
#endif
