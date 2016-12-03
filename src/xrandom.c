#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

#include "xdebug.h"
#include "xrandom.h"

/* The rc4 sbox */
static unsigned char rc4_sbox[256];

/* This flags is used to initialize the sbox the first time,
 * without an explicit intialization step outside this file. */
static int rc4_seedflag = 0;

/* Initialize the sbox with pseudo random data */
static void xrand_rc4_init()
{
  int i, fd;

  /* Strong sbox initialization */
  fd = open("/dev/urandom", O_RDONLY);
  if(fd > 0)
  {
    read(fd, rc4_sbox, 256);
    close(fd);
  }
  
  /* Weaker sbox initialization */
  for(i = 0; i < 256; i++) 
  {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    if(i & 1)
      rc4_sbox[i] ^= (tv.tv_usec >> (i&0xF)) & 0xFF;
    else
      rc4_sbox[i] ^= (tv.tv_sec  >> (i&0xF)) & 0xFF;
  }
  
  rc4_seedflag = 1;
}

/* Generates a 32bit random number using an RC4-like algorithm */
unsigned int xrand_rc4()
{
  unsigned int r = 0;
  static unsigned int i = 0, j = 0;
  unsigned int si, sj, x;
  unsigned char *rc = (unsigned char*)&r;
  
  /* initialization, only needed the first time */
  if(!rc4_seedflag)
  {
    xrand_rc4_init();
  }  
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  
  return r;
}

/* Generates a 32bit random number using an RC4-like algorithm */
unsigned int xrand_rc41()
{
  return xrand_rc4();
}

/* Generates a 32bit random number using an RC4-like algorithm */
unsigned int xrand_rc42()
{
  unsigned int r = 0;
  static unsigned int i = 0, j = 0;
  unsigned int si, sj, x;
  unsigned char *rc = (unsigned char*)&r;
  
  /* initialization, only needed the first time */
  if(!rc4_seedflag)
  {
    xrand_rc4_init();
  }  
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  
  return r;
}

/* Generates a 32bit random number using an RC4-like algorithm */
unsigned int xrand_rc43()
{
  unsigned int r = 0;
  static unsigned int i = 0, j = 0;
  unsigned int si, sj, x;
  unsigned char *rc = (unsigned char*)&r;
  
  /* initialization, only needed the first time */
  if(!rc4_seedflag)
  {
    xrand_rc4_init();
  }  
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  
  return r;
}

/* Generates a 32bit random number using an RC4-like algorithm */
unsigned int xrand_rc44()
{
  unsigned int r = 0;
  static unsigned int i = 0, j = 0;
  unsigned int si, sj, x;
  unsigned char *rc = (unsigned char*)&r;
  
  /* initialization, only needed the first time */
  if(!rc4_seedflag)
  {
    xrand_rc4_init();
  }  
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
    /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
            
  return r;
}

/* Generates a 32bit random number using an RC4-like algorithm */
unsigned int xrand_rc45()
{
  unsigned int r = 0;
  static unsigned int i = 0, j = 0;
  unsigned int si, sj, x;
  unsigned char *rc = (unsigned char*)&r;
  
  /* initialization, only needed the first time */
  if(!rc4_seedflag)
  {
    xrand_rc4_init();
  }  
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
    /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
            
  return r;
}

/* Generates a 32bit random number using an RC4-like algorithm */
unsigned int xrand_rc46()
{
  unsigned int r = 0;
  static unsigned int i = 0, j = 0;
  unsigned int si, sj, x;
  unsigned char *rc = (unsigned char*)&r;
  
  /* initialization, only needed the first time */
  if(!rc4_seedflag)
  {
    xrand_rc4_init();
  }
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
    /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
            
  return r;
}

/* Generates a 32bit random number using an RC4-like algorithm */
unsigned int xrand_rc47()
{
  unsigned int r = 0;
  static unsigned int i = 0, j = 0;
  unsigned int si, sj, x;
  unsigned char *rc = (unsigned char*)&r;
  
  /* initialization, only needed the first time */
  if(!rc4_seedflag)
  {
    xrand_rc4_init();
  }  
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
    /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
            
  return r;
}

/* Generates a 32bit random number using an RC4-like algorithm */
unsigned int xrand_rc48()
{
  unsigned int r = 0;
  static unsigned int i = 0, j = 0;
  unsigned int si, sj, x;
  unsigned char *rc = (unsigned char*)&r;
  
  /* initialization, only needed the first time */
  if(!rc4_seedflag)
  {
    xrand_rc4_init();
  }  
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
  /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
    /* generates 4 bytes of pseudo-random data using RC4 */
  for(x = 0; x < 4; x++)
  {
    i = (i + 1) & 0xff;
    si = rc4_sbox[i];
    j = (j + si) & 0xff;
    sj = rc4_sbox[j];
    rc4_sbox[i] = sj;
    rc4_sbox[j] = si;
    *rc++ = rc4_sbox[(si + sj) & 0xff];
  }
            
  return r;
}

/* random choice a value in [min, max] */
unsigned int xrand_range(unsigned int min, unsigned int max)
{
  unsigned offset = max - min;
  
  /* first step generates [0 - min] */
  return min + (xrand_rc4() % (offset + 1));
}
