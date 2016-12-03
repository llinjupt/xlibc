#ifndef XCHARSTR_H
#define XCHARSTR_H

#include <string.h>
#include <limits.h>
#include "xdebug.h"

int xstrcpy(char *dst, const char *src);
int xstrncpy(char *dst, const char *src, size_t size);
int xstrncat(char *dst, const char *src, size_t siz);

int xstrlower(char *str);
int xstrupper(char *str);

inline size_t xstrlen(const char *s);
int xstrcmp(const char *s1, const char *s2);
int xstrcasecmp(const char *s1, const char *s2);
int xstrncmp(const char *s1, const char *s2, size_t len);
int xstrncasecmp(const char *s1, const char *s2, size_t len);
char *xstrstr(const char *haystack, const char *needle);
char *xstrcasestr(const char *haystack, const char *needle);

char *xstrspace(char *p);
char *xstrstrip(char *s);

void xprintstr(unsigned char *cp, int len);

/* xdumphex(const unsigned char *buf, unsigned int len) */
#define xprinthexstr  xdumphex

char *xindex(const char *s, int c);
char *xrindex(const char *s, int c);

size_t xstrtok(char *sep, char *str, char **tptrs, size_t nptrs);

int xu8_reverse(void *memory, int size);

#endif /* XCHARSTR_H */
