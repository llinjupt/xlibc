#ifndef XEPOLL_H
#define XEPOLL_H

#include <sys/epoll.h>

/*
 * This macro specifies the maximum number of events that can be
 * processed by the ioqueue on a single poll cycle, on implementation
 * that supports it. The value is only meaningfull when specified
 * during PJLIB build.
 */
#ifndef XMAX_EVENTS_POLL
#define XMAX_EVENTS_POLL     (16)
#endif

int xepoll_create(void);

int xepoll_add(int epfd, int fd);
void xepoll_adds(int epfd, int *fd, int maxfds);

int xepoll_del(int epfd, int fd);
void xepoll_dels(int epfd, int *fd, int maxfds);

#endif /* XEPOLL_H */
