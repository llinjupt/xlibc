#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/ioctl.h>

//#include "xalloc.h"
#include "xdebug.h"
#include "xioctl.h"

/*
#define _INTSIZEOF(n)   ((sizeof(n)+sizeof(int)-1)&~(sizeof(int) - 1)) 
#define mva_start(ap,v) ( ap = (va_list)&v + _INTSIZEOF(v) )        
#define mva_arg(ap,t) ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define mva_end(ap)    ( ap = (va_list)0 ) 
*/
int xioctl(int fd, int request, void *argp)
{
  int ret;

  ret = ioctl(fd, request, argp);
  if(ret < 0)
    xerror("ioctl request %d \n", request);
  
  return ret;
}

int xxioctl(int fd, int request, void *argp)
{
  int ret;
  
  ret = ioctl(fd, request, argp);
  if(ret < 0)
    xdie("ioctl request %d \n", request);

  return ret;
}
