#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "xsystem.h"

/* a simple test function for system endian 
 *     -------------------
 * addr:0x...   0   | 1  |  2  | 3
 *    -------------------
 * little    0x78|0x56|0x34|0x12
 *    -------------------
 *   big     0x12|0x34|0x56|0x78
 *    -------------------
 * little : return ENDIAN_LITTLE;
 * big    : return ENDIAN_BIG;
 * else   : return ENDIAN_UNKNOWN;
 */
endian_t xendian()
{
  unsigned int seed = 0x12345678;
  char *p = (char *)&seed;
  unsigned char uint_len = sizeof(seed);
   
  /* big endian for higher address is 0x78(high bytes) */
  if(*(p + uint_len - 4) == 0x12 
      && *(p + uint_len - 3) == 0x34
      && *(p + uint_len - 2) == 0x56
      && *(p + uint_len - 1) == 0x78
      )
  return ENDIAN_BIG;
  
  if(*p == 0x78 
      && *(p + 1) == 0x56
      && *(p + 2) == 0x34
      && *(p + 3) == 0x12
    )
  return ENDIAN_LITTLE;
  
  return ENDIAN_UNKNOWN;
}

inline uint16_t xswap16(uint16_t x)
{
  return (((x & 0x00ff) << 8) |
          ((x & 0xff00) >> 8));
}

inline uint32_t xswap32(uint32_t x)
{
  return (((x & 0x000000ffU) << 24) |
          ((x & 0x0000ff00U) <<  8) |
          ((x & 0x00ff0000U) >>  8) |
          ((x & 0xff000000U) >> 24));
}

inline uint64_t xswap64(uint64_t x)
{
  return (((x & 0x00000000000000ffULL) << 56) |
          ((x & 0x000000000000ff00ULL) << 40) |
          ((x & 0x0000000000ff0000ULL) << 24) |
          ((x & 0x00000000ff000000ULL) <<  8) |
          ((x & 0x000000ff00000000ULL) >>  8) |
          ((x & 0x0000ff0000000000ULL) >> 24) |
          ((x & 0x00ff000000000000ULL) >> 40) |
          ((x & 0xff00000000000000ULL) >> 56));
}

/* get cmdline from pid. Read progress info form /proc/$pid. */
int xgetcmdline_bypid(int pid, char *buf, int size)
{
  char cmdname[PATH_MAX + 1] = "";
  int fd = 0;

  int n = 0;
  int i = 0;

  if(pid < 1 || buf == NULL || size < 0)
    return -1;
  
  snprintf(cmdname, 32, "/proc/%d/cmdline", pid);
  fd = open(cmdname, O_RDONLY);
  if(fd < 0)
  {
    perror("open");
    return -1;
  }
  n = read(fd, buf, size);
  close(fd);

  if(n <= 0)
    return -1;

  if(buf[n - 1] == '\n')
    buf[--n] = '\0';

  for(i = 0; i < n; i++)
  {
    if(isprint(buf[i]) == 0)
      buf[i] = ' ';
  }

  if(n > 0 && buf[0])
    return 0;

  return -1;  
}

int xgetexe_bypid(int pid, char *buf, int size)
{
  int len = 0;
  char pathname[PATH_MAX + 1] = "";

  if(pid < 1 || buf == NULL || size < 0)
    return -1;
  
  snprintf(pathname, PATH_MAX, "/proc/%d/exe", pid);
  len = readlink(pathname, buf, size);
  if(len < 0)
    return -1;
    
  buf[len] = '\0';
  
  return 0;
}

/* 
 * get pids by traverse /proc/xx, 
 * if error ret -1, if not find ret 0, otherwise ret count 
 * the name is the same as `ps' cmd showed, not the exe. 
 *
 * for example:
 * # ls -l /usr/sbin/udhcpc 
 * lrwxrwxrwx    1 root     root            6 Apr 10 03:02 /usr/sbin/udhcpc -> udhcpd
 *
 * then if run udhcpc, then you need give udhcpc as name, not udhcpd even it's the real 
 * exe
 */
int xgetpid_byname(const char *procname, int *findpids, int size)
{
  DIR *dir = NULL;
  struct dirent *d = NULL;
  
  int pid = 0;
  int i = 0;
  
  char *pname = NULL;
  int plen = strlen(procname);
  
  i = 0;
  findpids[0] = 0;
  
  /* Open the /proc directory. */
  dir = opendir("/proc");
  if(!dir)
  {
    perror("xgetpid_byname opendir");
    return -1;
  }

  /* Walk through the directory. */
  while((d = readdir(dir)) != NULL && i < size) 
  {
    char path[PATH_MAX + 1] = "";
    int namelen;
  
    /* see if this is a process */
    if((pid = atoi(d->d_name)) == 0)       
      continue;
  
    if(xgetcmdline_bypid(pid, path, PATH_MAX + 1) < 0)
      continue;

    /* split cmd and parameters */
    pname = strchr(path, ' ');
    if(pname)
      *pname = '\0';

    /* find procname */
    pname = strrchr(path, '/');
    if(pname)
      pname++;
    else
      pname = path;
  
    /* we don't need small name len */
    namelen = strlen(pname);
    //printf("pname = %s.\n", pname);
    if(namelen < plen)     
      continue;

    if(!strncmp(procname, pname, plen)) 
    {
      /* to avoid subname like search proc "abc" but proc "abcxxx" matched */
      if(pname[plen] == ' ' || pname[plen] == '\0') 
      {
        findpids[i] = pid;
        i++;
      }
    }
  }

  closedir(dir);
  return  i;
}

void xkill_bypid(int *pids, int size)
{
  int i = 0;
  
  for(i = 0; i < size; i++)
  {
    if(pids[i] == 0)
      continue;

    kill(pids[i], SIGKILL);
  }
}

#ifdef TEST
void test_xsystem(int argc, char *argv[])
{
  int count = 0;
  int pid;
  
  if(argc != 2)
  {
    printf("Usage %s procname\n", argv[0]);
    return;
  }
  
  count = xgetpid_byname(argv[1], &pid, 1);
  if(count <= 0)
    return;
  
  printf("pid:%d\n", pid);
  //xkill_bypid(pids, ret);
}

void test_xgetcmdline(int argc, char *argv[])
{
  int pid = 0;
  int ret = 0;
  char cmdline[1024] = "";
  
  if(argc != 2)
  {
    printf("Usage %s pid\n", argv[0]);
    return;
  }
  
  pid = atoi(argv[1]);
  ret = xgetcmdline_bypid(pid, cmdline, 1024);
  if(ret == 0)
    printf("cmdline :%s\n", cmdline);

  ret = xgetexe_bypid(pid, cmdline, 1024);
  if(ret == 0)
    printf("exe :%s\n", cmdline);
}

#endif
