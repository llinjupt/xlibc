#ifndef XFILE_H
#define XFILE_H

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <linux/limits.h>

#ifndef FILENAMSIZ
#define FILENAMSIZ (NAME_MAX + 1) /* # chars in a file name */
#endif

/* file lock only used between different processes */
int xflock(char *filename, int flags);
int xfunlock(int fd);

int xfdlock(int fd, int flags);
int xfdunlock(int fd);
long xfsize(FILE *fd);
long xfsize_file(const char *file);

ssize_t xread(int fd, void *buf, size_t count);
ssize_t xwrite(int fd, const void *buf, size_t count);
void *xfile_read(int fd, size_t *maxsz_p);

void xfile_remove(char *file);
int xfzero(char *fname);

int xfd_nonblk(int sfd, int set);
void xfds_nonblk(int *fd, int maxfds, int set);

FILE *xtmpfile_create(const char *mode);
int xtmpfile_remove(FILE  *fp);

inline int xfile_fd(FILE *fp);
inline FILE *xfile_fp(int fd, const char *mode);

int xfile_name(char name[FILENAMSIZ], int fd);
int xfile_name2(char name[FILENAMSIZ], FILE *fp);
#endif /* XFILE_H */
