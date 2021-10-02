#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <ctype.h>

#include "xbitmap.h"
#include "xdebug.h"

void bitmap_dump(unsigned long *bitmap, int nbits)
{
  printf("Hex dump bitmap: ");
  if(nbits <= BITS_PER_LONG)
    printf("%08lx\n", *bitmap);
  else
  {
    int len = BITS_TO_LONGS(nbits);
    int i = len - 1;
    for(; i >= 0; i--)
      printf("%08lx ", bitmap[i]);
    
    printf("\n");
  }
}

int bitmap_find_empty(unsigned long *bitmap, int nbits)
{
  int size = BITS_TO_LONGS(nbits);
  int offset = 0;
  int i = 0, j;
  
  for(; i < size; i++)
  {
    if(bitmap[i] == (~0))
    {
      offset += BITS_PER_LONG;
      continue;
    }
    
    for(j = 0; j < BITS_PER_LONG; j++)
    {
      if(((bitmap[i] >> j) & 0x01) == 0)
      {
        offset += j;
        return offset >= nbits ? -1 : offset;
      }
    }
  }
  
  return -1;
}

/* 
 * conver to string like 0000000010ffee000 so must be enough len 
 * return strlen if -1 failed
*/
int bitmap_map2str(char *str, unsigned long *bitmap, int nbits)
{
  int out_len = -1;
  if(nbits <= BITS_PER_LONG)
    out_len = sprintf(str, "%08lx", *bitmap);
  else
  {
    int len = BITS_TO_LONGS(nbits);
    int i = len - 1;
    
    out_len = 0;
    for(; i >= 0; i--)
    {
      len = sprintf(str + out_len, "%08lx", bitmap[i]);
      if(len < 0)
        return -1;
      out_len += len;
    }
    
    str[out_len] = '\0';
  }
  
  return out_len;
}

int bitmap_str2map(unsigned long *bitmap, int nbits, char *str)
{
  int i = 0;
  int len = strlen(str);
  int size = BITS_TO_LONGS(nbits);
  char tmp[16] = "";
  char *end;
  
  if(len / 8 != size)
    return -1;
  
  tmp[8] = '\0';
  for(i = 0; i < len; i += 8)
  {
    strncpy(tmp, str + i, 8);
    bitmap[size - i / 8 - 1] = strtoul(tmp, &end, 16);
    if(*end != '\0')
      return -1;
  }

  return 0;
}

void bitmap_zero(unsigned long *dst, int nbits)
{
  if (nbits <= BITS_PER_LONG)
    *dst = 0UL;
  else {
    int len = BITS_TO_LONGS(nbits) * sizeof(unsigned long);
    memset(dst, 0, len);
  }
}

void bitmap_fill(unsigned long *dst, int nbits)
{
  size_t nlongs = BITS_TO_LONGS(nbits);
  if (nlongs > 1) {
    int len = (nlongs - 1) * sizeof(unsigned long);
    memset(dst, 0xff,  len);
  }
  dst[nlongs - 1] = BITMAP_LAST_WORD_MASK(nbits);
}

void bitmap_copy(unsigned long *dst, const unsigned long *src,
      int nbits)
{
  if (nbits <= BITS_PER_LONG)
    *dst = *src;
  else {
    int len = BITS_TO_LONGS(nbits) * sizeof(unsigned long);
    memcpy(dst, src, len);
  }
}

static int __bitmap_empty(const unsigned long *bitmap, int bits)
{
  int k, lim = bits/BITS_PER_LONG;
  for (k = 0; k < lim; ++k)
    if (bitmap[k])
      return 0;

  if (bits % BITS_PER_LONG)
    if (bitmap[k] & BITMAP_LAST_WORD_MASK(bits))
      return 0;

  return 1;
}

int bitmap_empty(const unsigned long *src, int nbits)
{
  if (nbits <= BITS_PER_LONG)
    return ! (*src & BITMAP_LAST_WORD_MASK(nbits));
  else
    return __bitmap_empty(src, nbits);
}

static int __bitmap_full(const unsigned long *bitmap, int bits)
{
  int k, lim = bits/BITS_PER_LONG;
  for (k = 0; k < lim; ++k)
    if (~bitmap[k])
      return 0;

  if (bits % BITS_PER_LONG)
    if (~bitmap[k] & BITMAP_LAST_WORD_MASK(bits))
      return 0;

  return 1;
}

int bitmap_full(const unsigned long *src, int nbits)
{
  if (nbits <= BITS_PER_LONG)
    return ! (~(*src) & BITMAP_LAST_WORD_MASK(nbits));
  else
    return __bitmap_full(src, nbits);
}

/* set bitmap offset bit as 1 so must assure bitmap with enough size */
void bitmap_set(unsigned long *bitmap, int offset)
{
  if(offset < BITS_PER_LONG)
    bitmap[0] |= (1 << offset);
  else
  {
    int offset_longs = offset / BITS_PER_LONG;
    int left = offset - offset_longs * BITS_PER_LONG;
    
    bitmap[offset_longs] |= 1 << left;
  }
}

int bitmap_get(unsigned long *bitmap, int offset)
{
  unsigned long ret = 0;
  
  if(offset < BITS_PER_LONG)
    ret = bitmap[0] & (1 << offset);
  else
  {
    int offset_longs = offset / BITS_PER_LONG;
    int left = offset - offset_longs * BITS_PER_LONG;
    
    ret = bitmap[offset_longs] & (1 << left);
  }
  
  return ret ? 1 : 0;
}

/* set bitmap offset bit as 0 so must assure bitmap with enough size */
void bitmap_unset(unsigned long *bitmap, int offset)
{
  if(offset < BITS_PER_LONG)
    bitmap[0] &= ~(1 << offset);
  else
  {
    int offset_longs = offset / BITS_PER_LONG;
    int left = offset - offset_longs * BITS_PER_LONG;
    
    bitmap[offset_longs] &= ~(1 << left);
  }
}

#ifdef TEST
int test_bitmap()
{
  #define TEST_BITMAP_SIZE 64
  int i = 0;
  char str[65] = "003fffffffffffff";
  
  DECLARE_BITMAP(bitmap, TEST_BITMAP_SIZE);
  bitmap_zero(bitmap, TEST_BITMAP_SIZE);
  printf("after bitmap_zero:\n");
  bitmap_dump(bitmap, TEST_BITMAP_SIZE);
  
  printf("after bitmap_fill TEST_BITMAP_SIZE:\n");
  for(; i < TEST_BITMAP_SIZE; i++)
  {
    bitmap_set(bitmap, i);
    printf("set %d :", i);
    bitmap_dump(bitmap, TEST_BITMAP_SIZE);
    
    bitmap_map2str(str, bitmap, TEST_BITMAP_SIZE);
    printf("str:%s\n", str);
  }
  bitmap_fill(bitmap, TEST_BITMAP_SIZE);
  
  bitmap_dump(bitmap, TEST_BITMAP_SIZE);
  for(i = 0; i < TEST_BITMAP_SIZE; i++)
  {
    bitmap_unset(bitmap, i);
    bitmap_dump(bitmap, TEST_BITMAP_SIZE);
  }
  
  printf("%d\n", bitmap_str2map(bitmap, TEST_BITMAP_SIZE, str));
  bitmap_dump(bitmap, TEST_BITMAP_SIZE);
  bitmap_fill(bitmap, TEST_BITMAP_SIZE - 1);
  bitmap_dump(bitmap, TEST_BITMAP_SIZE);
  
  printf("%d\n", bitmap_find_empty(bitmap, TEST_BITMAP_SIZE));
  
  for(i = 0; i < TEST_BITMAP_SIZE; i++)
  {
     printf("%d\n", bitmap_get(bitmap, i));
  }
  return 0;
}


#endif
