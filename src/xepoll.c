#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>

#include "xdebug.h"
#include "xalloc.h"
#include "xepoll.h"

int xepoll_create(void)
{
  return epoll_create(XMAX_EVENTS_POLL);
}

int xepoll_add(int epfd, int fd)
{
  int ret = 0;  
  struct epoll_event ev;

  ev.data.fd = fd;
  /* 
   * NOTE: Must be clear the differences between EPOLLLT and  
   * EPOLLET. Default mode is EPOLLLT, here don't set EPOLLET
  */
  //ev.events = EPOLLIN | EPOLLET;
  ev.events = EPOLLIN;
  ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
  
  return ret;
}

void xepoll_adds(int epfd, int *fd, int maxfds)
{
  int i = 0;

  xassert(maxfds > 0 && fd);

  for(; i < maxfds; i++)
  {
    if(xepoll_add(epfd, fd[i]) < 0)
      perror("EPOLL_CTL_ADD");
  }  
}

int xepoll_del(int epfd, int fd)
{
  int ret = 0;  
  struct epoll_event ev;

  ev.data.fd = fd;
  ev.events = EPOLLIN;
  ret = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);

  return ret;
}

void xepoll_dels(int epfd, int *fd, int maxfds)
{
  int i = 0;

  xassert(maxfds > 0 && fd);

  for(; i < maxfds; i++)
  {
    if(xepoll_del(epfd, fd[i]) < 0)
      perror("EPOLL_CTL_DEL");
  }
}

int xepoll_wait(int epfd, int timeout)
{
  struct epoll_event events[XMAX_EVENTS_POLL];
  
  return epoll_wait(epfd, events, XMAX_EVENTS_POLL, timeout);
}
