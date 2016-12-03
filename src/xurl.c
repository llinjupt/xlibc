#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>

#include "xdebug.h"
#include "xurl.h"

/* 
 * lli_njupt@163.com stolen from curl-7.35.0 lib/escape.c
 *
 * Portable character check (remember EBCDIC). Do not use isalnum() 
 * because its behavior is altered by the current locale.
 * See http://tools.ietf.org/html/rfc3986#section-2.3
 */
static int url_is_unreserved(unsigned char in)
{
  switch(in)
  {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    case 'a': case 'b': case 'c': case 'd': case 'e':
    case 'f': case 'g': case 'h': case 'i': case 'j':
    case 'k': case 'l': case 'm': case 'n': case 'o':
    case 'p': case 'q': case 'r': case 's': case 't':
    case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
    case 'A': case 'B': case 'C': case 'D': case 'E':
    case 'F': case 'G': case 'H': case 'I': case 'J':
    case 'K': case 'L': case 'M': case 'N': case 'O':
    case 'P': case 'Q': case 'R': case 'S': case 'T':
    case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
    case '-': case '.': case '_': case '~':
      return 1;
    default:
      break;
  }
  
  return 0;
}

/* 
 * if inlen is 0, then will inlen = strlen(str) 
 * NOTE: if return !null then must free it
 */
char *xurl_escape(const char *input, int inlen)
{
  size_t alloc = (inlen? (size_t) inlen : strlen(input)) + 1;

  char *ns;
  char *testing_ptr = NULL;
  
  /* we need to treat the characters unsigned */
  unsigned char in; 
  
  size_t newlen = alloc;
  size_t strindex=0;
  size_t length;

  ns = malloc(alloc);
  if(!ns)
    return NULL;

  length = alloc - 1;
  while(length--) 
  {
    in = *input;

    if(url_is_unreserved(in))
      /* just copy this */
      ns[strindex++]=in;
    else {
      /* encode it */
      newlen += 2; /* the size grows with two, since this'll become a %XX */
      if(newlen > alloc) {
        alloc *= 2;
        testing_ptr = realloc(ns, alloc);
        if(!testing_ptr) {
          free( ns );
          return NULL;
        }
        else {
          ns = testing_ptr;
        }
      }

      snprintf(&ns[strindex], 4, "%%%02X", in);

      strindex += 3;
    }
    input++;
  }
  
  ns[strindex] = 0; /* terminate it */
  
  return ns;
}

char *xurl_unescape(const char *input)
{
  char *result = NULL;
  int  count = 0;
  char hex[3] = {0};
  unsigned long temp;

  result = malloc(strlen(input) + 1);
  if(result == NULL)
    return NULL;

  /* use the null character as a loop terminator */
  while(input[count]) 
	{
	  if(input[count] == '%')
		{
		  hex[0] = input[count + 1];
		  hex[1] = input[count + 2];
		  hex[2] = '\0';
		  temp = strtoul(hex, NULL, 16);
		  result[count]=(char)temp;
		  count++; 
		  
		  input = input + 2; 
		}
	  else
		{
		  result[count] = input[count];
		  count++;
		}
	}
  result[count] = '\0'; 
	
  return result;
}

#ifdef TEST
void test_xurl()
{
  char *input = "%#+";
  char *tmp = xurl_escape(input, strlen(input));
  if(tmp)
  {
    char *tmp2 = xurl_unescape(tmp);
    
    printf("encode:%s => %s\n", input, tmp);
    printf("decode:%s => %s\n", tmp, tmp2);
    free(tmp);
    free(tmp2);
  }
}
#endif
