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
#include "xfile.h"

/* 
 * kernel should set CONFIG_FILE_LOCKING
 * F_RDLCK reading lock 
 * F_WRLCK writing lock
 * F_UNLCK unlock
 */
int xfdlock(int fd, int lock_type)
{
  int ret = 0;
  struct flock s_flock;
  
  s_flock.l_type = lock_type; 
  s_flock.l_whence = SEEK_SET; 
  s_flock.l_start = 0; 
  s_flock.l_len = 0; 
  s_flock.l_pid = getpid();
  
  /* F_SETLKW blocking F_SETLK not blocking */
  ret = fcntl(fd, F_SETLK, &s_flock);
  return ret;  
}

int xfdunlock(int fd)
{
  return xfdlock(fd, F_UNLCK);
}

/* Normally flags is O_WRONLY|O_CREAT 
 * can be used for multex locking between two processess 
 * or multex read/write opts
 */
int xflock(char *filename, int flags)
{ 
  int fd;
  
  fd = open(filename, flags, 0777);
  if(fd < 0)
    return -1;

  return xfdlock(fd, F_WRLCK);
}

int xfunlock(int fd)
{ 
  int ret = xfdunlock(fd);  
  
  close(fd);  
  return ret;
}

void xfile_remove(char *file)
{
  unlink(file);
}

long xfsize(FILE *fp)
{
  long offset, len = 0;
  
  if(fp == NULL)
    return -1;
  
  offset = ftell(fp);
  
  fseek(fp, 0L, SEEK_END);
  len = ftell(fp);
  fseek(fp, offset, SEEK_SET);

  return len;
}

long xfsize_file(const char *file)
{
  long len = -1;

  FILE *fp = fopen(file, "r");
  if(fp == NULL)
    return 0;
  
  len = xfsize(fp);  
  fclose(fp); 

  return len;
}

/* for little datum writing */
ssize_t __xwrite(int fd, const void *buf, size_t count)
{
  ssize_t n;

  do{
    n = write(fd, buf, count);
  }while (n < 0 && errno == EINTR);

  return n;
}

/*
 * Write all of the supplied buffer out to a file.
 * This does multiple writes as necessary.
 * Returns the amount written, or -1 on an error.
 */
ssize_t xwrite(int fd, const void *buf, size_t len)
{
  ssize_t cc;
  ssize_t total;

  total = 0;

  while(len)
  {
    cc = __xwrite(fd, buf, len);

    if(cc < 0)
    {
      if(total)
      {
        /* we already wrote some! */
        /* user can do another write to know the error code */
        return total;
      }
      return cc;  /* write() returns -1 on failure. */
    }

    total += cc;
    buf = ((const char *)buf) + cc;
    len -= cc;
  }

  return total;
}

ssize_t __xread(int fd, void *buf, size_t count)
{
  ssize_t n;

  do
  {
    n = read(fd, buf, count);

  }while(n < 0 && errno == EINTR);

  return n;
}

/*
 * Read all of the supplied buffer from a file.
 * This does multiple reads as necessary.
 * Returns the amount read, or -1 on an error.
 * A short read is returned on an end of file.
 */
ssize_t xread(int fd, void *buf, size_t len)
{
  ssize_t cc;
  ssize_t total;

  total = 0;

  while(len)
  {
    cc = __xread(fd, buf, len);
    if(cc < 0)
    {
      if(total)
      {
        /* we already have some! */
        /* user can do another read to know the error code */
        return total;
      }
      return cc; /* read() returns -1 on failure. */
    }
    if(cc == 0)
      break;
      
    buf = ((char *)buf) + cc;
    total += cc;
    len -= cc;
  }

  return total;
}

/*
 * cleanup file contents, only support regular file or 
 * symbolic link. OK ret 0 otherwise ret -1
 */
int xfzero(char *fname)
{
  FILE *fp = NULL;
  struct stat status;
  
  if(stat(fname, &status) < 0)
    return -1;

  if(!S_ISREG(status.st_mode) 
    && !S_ISLNK(status.st_mode))
    return -1;
  
  fp = fopen(fname, "w");
  if(fp == NULL)
    return -1;
  
  fclose(fp);
  return 0;
}

/* set fd with O_NONBLOCK or not  */
int xfd_nonblk(int sfd, int set)
{
  int flags, s;

  flags = fcntl (sfd, F_GETFL, 0);
  if(flags < 0)
  {
    perror("fd_set_nonblock F_GETFL");
    return -1;
  }

  if(set)
    flags |= O_NONBLOCK;
  else
    flags &= ~O_NONBLOCK;

  s = fcntl (sfd, F_SETFL, flags);
  if (s == -1)
    perror("fd_set_nonblock F_SETFL");

  return s;
}

void xfds_nonblk(int *fd, int maxfds, int set)
{
  int i = 0;

  xassert(maxfds > 0 && fd);

  for(; i < maxfds; i++)
    xfd_nonblk(fd[i], set);
}

/* create a temporary file and return the fp */
FILE *xtmpfile_create(const char *mode)
{
  int fd = 0;
  FILE *fp = NULL;
  
  char template[] = "/tmp/xfile.XXXXXX";
  fd = mkstemp(template);
  if(fd < 0)
  {
    perror("mkstemp");
    return NULL;
  }
  close(fd);
  
  fp = fopen(template, mode);
  if(fp == NULL)
    unlink(template);
  
  return fp;
}

/* close fp and delete the file name */
int xtmpfile_remove(FILE  *fp)
{
  int ret = 0;
  char name[256] = "";

  ret = xfile_name(name, fileno(fp));
  if(ret < 0)
  {
    fclose(fp);
    return -1;
  }

  fclose(fp);
  unlink(name);
  
  return 0;
}

int xfile_name(char name[FILENAMSIZ], int fd)
{
  int ret = 0;
  char proc[FILENAMSIZ] = "";
  
  snprintf(proc, FILENAMSIZ, "/proc/%d/fd/%d", getpid(), fd);  
  ret = readlink(proc, name, FILENAMSIZ);
  if(ret < 0)
  {
    perror("xfile_name readlink");
    return -1;
  }
  name[FILENAMSIZ - 1] = '\0';

  return 0;
}

int xfile_name2(char name[FILENAMSIZ], FILE *fp)
{ 
  int fd = fileno(fp);
  if(fd < 0)
  {
    perror("xfile_name2 fileno");
    return -1;
  }

  return xfile_name(name, fd);
}

/* Note: direct close fp, and don't close fd directly. */
inline int xfile_fd(FILE *fp)
{
  int fd = fileno(fp);
  if(fd < 0)
    perror("xfile_fd fileno");
  
  return fd;
}

/* Note: direct close fp, and need to close fd. */
inline FILE *xfile_fp(int fd, const char *mode)
{
  FILE *fp = fdopen(fd, mode);
  if(fp == NULL)
    perror("xfile_fp fdopen");
  
  return fp;
}

/* read a file to a malloc buffer, so must free if not NULL */
void *xfile_read(int fd, size_t *size)
{
  char buf[256] = "";
  char *result = NULL;
  
  size_t total = 0;
  size_t len = 0;

  do
  {
    char *old = result;
    
    len = read(fd, buf, 256);
    if(len <= 0)
      break;

    result = realloc(old, total + len + 1);
    if(result == NULL)
    {
      free(old);
      return NULL;
    }

    memcpy(result + total, buf, len);
    total += len;
  }while(len > 0);

  if(total)
  {
    result[total] = '\0';
    if(size)
      *size = total;
  }
  
  return result;
}

#ifdef TEST
#define TEST_FILE "/tmp/tmp"
void test_xfile()
{
  /* 1. test xfile_fd */
  FILE *fp = fopen("/dev/null", "w");
  xassert(fp);
  
  /* no need to close fd */
  int fd = xfile_fd(fp);
  fclose(fp); 

  /* 2. test xfile_fp */
  fd = open("/dev/null", O_RDONLY);
  xassert(fd >= 0);
  fp = xfile_fp(fd, "r");
  xassert(fp);

  /* no need to close fd */
  fclose(fp);
  
  /* 3. test temp file */
  fp = xtmpfile_create("w+");
  xassert(fp);
  xtmpfile_remove(fp);

#if 0
  /* 4. test file lock */
  fd = open(TEST_FILE, O_WRONLY|O_CREAT);
  printf("lock ret : %d\n", xfdlock(fd,  F_WRLCK));
  fp = fdopen(fd, "w");

  fprintf(fp, "nameserver %s\n", "192.168.0.1");
  fclose(fp);
  
  xfdlock(fd,  F_UNLCK);
  close(fd);
#endif
  fd = open("/tmp/test", O_WRONLY);
  if(fd >= 0)
  {
    char *result = NULL;
    size_t count = 0;
    result = xfile_read(fd, &count);
    
    printf("count:%ld\n", count);
    if(count > 0)
    {
      printf("result:%s\n", result);
      free(result);
    }
    close(fd);
  }
}
#endif
