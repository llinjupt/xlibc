#ifndef _XBITMAP_H
#define _XBITMAP_H

/* stolen from linux kernel bitmap.h */
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

#define BITS_PER_BYTE    8
#define BITS_TO_LONGS(nr)  DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))
#define BITS_PER_LONG 32

#define DECLARE_BITMAP(name, bits) \
  unsigned long name[BITS_TO_LONGS(bits)]
  
void bitmap_dump(unsigned long *bitmap, int nbits);  
void bitmap_set(unsigned long *bitmap, int offset);
void bitmap_unset(unsigned long *bitmap, int offset);

#define BITMAP_LAST_WORD_MASK(nbits)          \
(                  \
  ((nbits) % BITS_PER_LONG) ?          \
    (1UL<<((nbits) % BITS_PER_LONG))-1 : ~0UL    \
)

void bitmap_zero(unsigned long *dst, int nbits);
void bitmap_fill(unsigned long *dst, int nbits);

void bitmap_copy(unsigned long *dst, const unsigned long *src, int nbits);

int bitmap_empty(const unsigned long *src, int nbits);
int bitmap_full(const unsigned long *src, int nbits);
int bitmap_get(unsigned long *bitmap, int offset);

int bitmap_map2str(char *str, unsigned long *bitmap, int nbits);
int bitmap_str2map(unsigned long *bitmap, int nbits, char *str);
int bitmap_find_empty(unsigned long *bitmap, int nbits);

#endif /* _XBITMAP_H */

