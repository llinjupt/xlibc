#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include "xcharstr.h"
#include "xdebug.h"

/* The  strcpy()  function copies the string pointed to by src, 
 * including the terminating null byte (¡¯\0¡¯), to the buffer pointed 
 * to by dest.  The strings may not overlap, and the destination 
 * string dest must be  large  enough  to receive the copy. Else 
 * occur buffer Overflow.
 * xstrcpy can do overlapping copys. But still with buffer Overflow issue.
 * and return length which have been copyed not a char * to dst!
 */
int xstrcpy(char *dst, const char *src)
{
  size_t len = 0;
  
  xassert(src && dst);
  while((*dst = *src) != '\0')
  {
    dst++;
    src++;
    len++;
  }
  
  /* don't include the '\0' */
  return len;
}

/*
 * Appends src to string dst of size siz (unlike strncat, siz is the
 * full size of dst, not space left).  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz <= strlen(dst)).
 * Returns strlen(src) + MIN(siz, strlen(initial dst)).
 * If retval >= siz, truncation occurred.
 */
int xstrncat(char *dst, const char *src, size_t siz)
{
  char *d = dst;
  const char *s = src;
  size_t n = siz;
  size_t dlen;

  /* Find the end of dst and adjust bytes left but don't go past end */
  while(n-- != 0 && *d != '\0')
    d++;
  dlen = d - dst;
  n = siz - dlen;

  if(n == 0)
    return(dlen + strlen(s));
    
  while (*s != '\0')
  {
    if (n != 1)
    {
      *d++ = *s;
      n--;
    }
    s++;
  }
  *d = '\0';

  /* don't include the '\0' */
  return  (dlen + (s - src));
}

/* 
 * Like strncpy but make sure the resulting string is always 0 terminated, 
 * so only size - 1 copied if src's lenght exceed size. */
int xstrncpy(char *dst, const char *src, size_t size)
{
  size_t len = 0;
 
  if(!size)
    return 0;

  xassert(src && dst);
 
  for(; len < size && src[len] != '\0'; len++)
    dst[len] = src[len];

  if(len == size)
    len -= 1;
  
  dst[len]= '\0';

  /* don't include the '\0' */
  return len;
}

/* do string to lower string */
int xstrlower(char *str)
{
  char *tmp = NULL;
  
  for(tmp = str; *tmp; ++tmp)
    *tmp = tolower(*tmp);
    
  return tmp - str;
}

/* do string to upper string */
int xstrupper(char *str)
{
  char *tmp = NULL;
  
  for(tmp = str; *tmp; ++tmp)
    *tmp = toupper(*tmp);
    
  return tmp - str;
}

void xprintstr(unsigned char *cp, int len)
{
  register int s;

  putchar('\n');
  while(len > 0)
  {
    s = *cp++;
    len--;
    
/* isprint is more suitable
 * #ifndef isprint
 * #define isprint(c) ((c) >= ' ' && (c) <= '~')
 * #endif
 */    
    if(!isgraph(s) 
       &&(s != '\t' && s != ' ' && s != '\n' && s != '\r'))
      putchar('.');
    else
      putchar(s);
  }
}

/* 
 * strcmp can't handle NULL str, which causes segment error 
 * xstrcmp will hanle NULL carefully 
 */
int xstrcmp(const char *s1, const char *s2)
{
  if(s1 == NULL)
  {
    if(s2 == NULL)
      return 0;
    else
      return -s2[0];
  }

  if(s2 == NULL)
    return s1[0];

  return strcmp(s1, s2);
}

/* same as xstrcmp, will handle NULL correctly */
int xstrcasecmp(const char *s1, const char *s2)
{
  if(s1 == NULL)
  {
    if(s2 == NULL)
      return 0;
    else
      return -s2[0];
  }

  if(s2 == NULL)
    return s1[0];

  return strcasecmp(s1, s2);
}

/* same as xstrcmp, will handle NULL correctly */
int xstrncmp(const char *s1, const char *s2, size_t len)
{
  if(s1 == NULL)
  {
    if(s2 == NULL)
      return 0;
    else
      return -s2[0];
  }

  if(s2 == NULL)
    return s1[0];

  return strncmp(s1, s2, len);
}

/* same as xstrcmp, will handle NULL correctly */
int xstrncasecmp(const char *s1, const char *s2, size_t len)
{
  if(s1 == NULL)
  {
    if(s2 == NULL)
      return 0;
    else
      return -s2[0];
  }

  if(s2 == NULL)
    return s1[0];

  return strncasecmp(s1, s2, len);
}

/*
 * same as xstrcmp, will handle NULL as 0, strlen can't accept 
 * NULL which will cause segment error.
 */
inline size_t xstrlen(const char *s)
{
  if(s == NULL)
    return 0;
  
  return strlen(s);
}

/* same as xstrcmp, will handle NULL correctly */
char *xstrstr(const char *haystack, const char *needle)
{
  xassert(needle);
  
  if(haystack == NULL)
    return NULL;

  return strstr(haystack, needle);
}

/* same as xstrcmp, will handle NULL correctly */
char *xstrcasestr(const char *haystack, const char *needle)
{
  xassert(needle);
  
  if(haystack == NULL)
    return NULL;

  return strcasestr(haystack, needle);
}

/*
 * xstrstrip - Removes leading and trailing whitespace from @s.
 * @s: The string to be stripped.
 *
 * Note that the first trailing whitespace is replaced with a %NUL-terminator
 * in the given string @s. Returns a pointer to the first non-whitespace
 * character in @s.
 */
char *xstrstrip(char *s)
{
  size_t size;
  char *end;

  size = strlen(s);

  if (!size)
    return s;

  end = s + size - 1;
  while (end >= s && isspace(*end))
    end--;
  *(end + 1) = '\0';

  while (*s && isspace(*s))
    s++;

  return s;
}

/* remove all space characters from string */
char *xstrspace(char *p)
{
  int i = 0, j = 0;
  
  for(i = 0; p[i] != '\0'; i++) 
  {
    if(!isspace(p[i]))
      p[j++] = p[i];
  }

  p[j] = '\0';
  
  return p;
}

/* function similar to strtok() more convenient when we know the
 * max number of tokens, to tokenize with a single call.
 * Unlike strtok(), xstrtok() is thread safe.
 *
 * ARGS:
 *   'sep' is a string that contains all the delimiter characters
 *   'str' is the string to tokenize, that will be modified
 *   'tptrs' is an array of char* poiters that will contain the token pointers
 *   'nptrs' is the length of the 'tptrs' array.
 *
 * RETURN VALUE:
 *   The number of extracted tokens is returned.
 */
size_t xstrtok(char *sep, char *str, char **tptrs, size_t nptrs)
{
  size_t i, j = 0;
  int inside = 0;
  size_t seplen = strlen(sep);
  
  while(*str)
  {
    for(i = 0; i < seplen; i++)
    {
      if(sep[i] == *str)
        break;
    }
    
    /* no match */
    if(i == seplen)
    { 
      if(!inside)
      {
        tptrs[j++] = str;
        inside = 1;
      }
    }
    else  /* match */
    { 
      if(inside)
      {
        *str = '\0';
        if(j == nptrs)
          return j;
          
        inside = 0;
      }
    }
    
    str++;
  }
  
  return j;
}

/* 
 * index/rindex can't handle NULL str, which causes segment error 
 * xindex/rindex will hanle NULL carefully 
 */
char *xindex(const char *s, int c)
{
  if(s == NULL)
    return NULL;

  return index(s, c);
}

char *xrindex(const char *s, int c)
{
  if(s == NULL)
    return NULL;

  return rindex(s, c);
}

/* reverse the order of bytes within a multi-byte memory region */
int xu8_reverse(void *memory, int size) 
{
  register char *lower = (char *)(memory);
  register char *upper = lower + size;
  
  if(!memory || size <= 0)
    return -1;
    
  while (lower < upper)
  {
    register char byte = *lower;
    *lower++ = *(--upper);
    *upper = byte;
  }
  
  return 0;
}

#ifdef TEST
void test_xcharstr()
{
  char dst[16] = "";
  char src[16] = "";

  assert(xstrncpy(dst, src, 2) == 0);
  strcpy(src, "LE");
  assert(xstrncpy(dst, src, 2) == 1);
  printf("dst:%s\n", dst);
  assert(xstrncpy(dst, src, 3) == 2);
  printf("dst:%s\n", dst);
  assert(xstrncpy(dst, src, 4) == 2);

  strcpy(src, "LE");
  assert(xstrcpy(dst, src) == 2);
}
#endif
