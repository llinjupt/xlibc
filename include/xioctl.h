#ifndef XIOCTL_H
#define XIOCTL_H

#include <stdio.h>
#include <stdarg.h>

int xioctl(int fd, int request, void *argp);
int xxioctl(int fd, int request, void *argp);

#endif /* XIOCTL_H */
