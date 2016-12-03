#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netinet/tcp.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <arpa/inet.h>
#include <limits.h>

#include "xdebug.h"
#include "xsocket.h"
//#include "xnetif.h"

/* for setsockopt operations */
const int const_int_1 = 1;

/*
 * Die with an error message if we can't open a new socket.
 */
int xsocket(int domain, int type, int protocol)
{
  int r = socket(domain, type, protocol);

  if (r < 0) 
    xdie("socket\n");

  return r;
}

int xbind(int fd, const char *ip, int port)
{
  struct sockaddr_in addr;

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  if(ip)
    inet_aton(ip, &addr.sin_addr);
  else
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  return bind(fd, (const struct sockaddr *)&addr, sizeof(addr));  
}

/* 
 * On error, -1 is returned, and errno is set
 * appropriately; the sets and timeout become undefined, 
 * so do not rely on their contents after an error.
*/
int xselect(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, struct timeval *timeout)
{
  return select(nfds, readfds, writefds, exceptfds, timeout);
}

/* Die with an error message if we can't listen for connections on a socket.*/
void xlisten(int s, int backlog)
{
  if(listen(s, backlog))
    xdie("listen\n");
}

/* Die with an error message if sendto failed.
 * Return bytes sent otherwise  
 */
ssize_t xsendto(int s, const void *buf, size_t len, const struct sockaddr *to,
        socklen_t tolen)
{
  //ssize_t ret = sendto(s, buf, len, MSG_DONTWAIT | MSG_NOSIGNAL, to, tolen);
  ssize_t ret = sendto(s, buf, len, 0, to, tolen);
  if (ret < 0) 
  {
    close(s);
    xdie("sendto\n");
  }
  
  return ret;
}

void xconnect(int s, const struct sockaddr *s_addr, socklen_t addrlen)
{
  if(connect(s, s_addr, addrlen) < 0)
  {
    close(s);
    if(s_addr->sa_family == AF_INET)
    {
      xdie("can't connect to remote host %s:%d\n",
           inet_ntoa(((struct sockaddr_in *)&s_addr)->sin_addr),
           ntohs(((struct sockaddr_in *)&s_addr)->sin_port));
    }

    xdie("can't connect to remote host\n");
  }
}

/* below are setsockopt funcs to accelerate developing */
inline int setsockopt_reuseaddr(int fd)
{
  return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &const_int_1, sizeof(const_int_1));
}

inline int setsockopt_broadcast(int fd)
{
  return setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &const_int_1, sizeof(const_int_1));
}

inline int setsockopt_attachfilter(int fd, struct sock_fprog *filter)
{
  return setsockopt(fd, SOL_SOCKET, SO_ATTACH_FILTER, filter, sizeof(struct sock_fprog));
}

int setsockopt_bindtodevice(int fd, const char *iface)
{
  int ret;
  struct ifreq ifr;
  strncpy_IFNAMSIZ(ifr.ifr_name, iface);
  
  /* NB: passing (iface, strlen(iface) + 1) does not work!
   * (maybe it works on _some_ kernels, but not on 2.6.26)
   * Actually, ifr_name is at offset 0, and in practice
   * just giving char[IFNAMSIZ] instead of struct ifreq works too.
   * But just in case it's not true on some obscure arch... */
   
  ret = setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof(struct ifreq));
  if(ret)
    xerror("can't bind to interface %s", iface);
  
  return ret;
}

/*
 * keep_idle:     如果在 5 秒内没有任何数据交互,则进行探测. 缺省值:7200(s)
 * keep_interval:  探测时发探测包的时间间隔为3秒. 缺省值:    75(s)
 * keep_count:    探测重试的次数. 全部超时则认定连接失效..缺省值:9(次)
 *
 * refer to procfs:
 * /proc/sys/net/ipv4/tcp_keepalive_time
 * /proc/sys/net/ipv4/tcp_keepalive_intvl
 * /proc/sys/net/ipv4/tcp_keepalive_probes
 */
int setsockopt_keealive(int fd, int keep_idle, int keep_interval, int keep_count)
{
  int ret = 0;
  
  int keep_alive = 1;     /* 开启 keepalive 属性. 缺省值: 0(关闭) */
  ret = setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, (void*)&keep_idle, sizeof(keep_idle));
  if(ret < 0)
  {
    perror("setsockopt TCP_KEEPIDLE");
    return -1;
  }
  
  ret = setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, (void*)&keep_interval, sizeof(keep_interval));
  if(ret < 0)
  {
    perror("setsockopt TCP_KEEPINTVL");
    return -1;
  }

  ret = setsockopt(fd, SOL_TCP, TCP_KEEPCNT, (void*)&keep_count, sizeof(keep_count));
  if(ret < 0)
  {
    perror("setsockopt TCP_KEEPCNT");
    return -1;
  }

  ret = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void*)&keep_alive, sizeof(keep_alive));
  if(ret < 0)
  {
    perror("setsockopt SO_KEEPALIVE");
    return -1;
  }
  
  return 0;    
}

/* below are sockaddr oprations funcs */
static lsockaddr *get_lsa(int fd, 
                          int (*get_name)(int fd, struct sockaddr *addr, socklen_t *addrlen))
{
  lsockaddr lsa;
  lsockaddr *lsa_ptr;

  lsa.len = LSA_SIZEOF_SA;
  if(get_name(fd, &lsa.u.sa, &lsa.len) != 0)
    return NULL;

  lsa_ptr = malloc(LSA_LEN_SIZE + lsa.len);
  
  if(lsa_ptr == NULL)
    return NULL;

  memset(lsa_ptr, 0, LSA_LEN_SIZE + lsa.len);
  
  /* rarely (if ever) happens */
  if(lsa.len > LSA_SIZEOF_SA)
  { 
    lsa_ptr->len = lsa.len;
    get_name(fd, &lsa_ptr->u.sa, &lsa_ptr->len);
  } 
  else
    memcpy(lsa_ptr, &lsa, LSA_LEN_SIZE + lsa.len);
  
  return lsa_ptr;
}

inline lsockaddr *get_sock_lsa(int fd)
{
  return get_lsa(fd, getsockname);
}

inline lsockaddr *get_peer_lsa(int fd)
{
  return get_lsa(fd, getpeername);
}

/* below are socket port opration funcs */

/* Port find function which is normally used for consultant
 * Return port number for a service.
 * If "port" is a number use it as the port.
 * If "port" is a name it is looked up in /etc/services,
 * if it isnt found return default_port
 */
unsigned lookup_port(const char *port, const char *protocol, 
                      unsigned default_port)
{
  unsigned port_nr = default_port;
  
  if(port)
  {
    int old_errno;

    /* Since this is a lib function, we're not allowed to reset errno to 0.
     * Doing so could break an app that is deferring checking of errno. */
    old_errno = errno;
    port_nr = strtoul(port, NULL, 10);
    
    if (errno || port_nr <= 0)    
    {
      struct servent *tserv = getservbyname(port, protocol);
      port_nr = default_port;
      if (tserv)
        port_nr = ntohs(tserv->s_port);
    }
    errno = old_errno;
  }
  
  return port_nr;
}

#if 0
/* Get/set port APIs */
inline int get_port(const struct sockaddr *sa)
{

#if ENABLE_FEATURE_IPV6
  if(sa->sa_family == AF_INET6)
    return ((struct sockaddr_in6*)sa)->sin6_port;  
#endif

  if(sa->sa_family == AF_INET)
    return ((struct sockaddr_in*)sa)->sin_port;

  /* What? UNIX socket? IPX?? :) */
  return -1;
}

inline int set_port(lsockaddr *lsa, unsigned port)
{
#if ENABLE_FEATURE_IPV6
  if(lsa->u.sa.sa_family == AF_INET6)
  {
    lsa->u.sin6.sin6_port = port;
    return 0;
  }
#endif
  if(lsa->u.sa.sa_family == AF_INET)
  {
    lsa->u.sin.sin_port = port;
    return 0;
  }
  /* What? UNIX socket? IPX?? :) */
  return -1;
}
#endif

/* create a AF_UNIX socket with SOCK_DGRAM 
 * you can listen on it to read data or write 
 */
int xunix_socket(char *sockfile)
{
  struct sockaddr_un sunx;
  int len;
  
  int sock_fd;

  if(!sockfile)
  {
    xprintf("Must provide the sockfile!\n");
    return -1;
  }
  
  unlink(sockfile);

  memset(&sunx, 0, sizeof (sunx));
  sunx.sun_family = AF_UNIX;
  strncpy (sunx.sun_path, sockfile, sizeof(sunx.sun_path));
  if((sock_fd = socket (AF_UNIX, SOCK_DGRAM, 0)) < 0)
  {
    xprintf("Couldn't get file descriptor %s for socket ", sockfile);
    return -1;
  }
  
  len = sizeof(sunx.sun_family) + strlen(sunx.sun_path);
  if(bind(sock_fd, (struct sockaddr *)&sunx, len) < 0)
  {
    xprintf("Could not connect to socket %s\n", sockfile);
    unlink(sockfile);  
    return -1;
  }
  
  if(chmod(sockfile, 0666) < 0)
    xprintf("Could not set permission on :%s\n", sockfile);
  
  return sock_fd;
}

int xunix_send(char *sockfile, void *data, int size)
{
  struct sockaddr_un address;
  int sockfd;
  int len;
  int bytes;
  int result;
  
  if(!sockfile)
  {
    xprintf("Must provide the sockfile!\n");
    return -1;
  }
  
  /*创建socket,AF_UNIX通信协议,SOCK_STREAM数据方式*/
  if((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
  {
    xprintf("socket\n");
    return -1;
  }

  address.sun_family = AF_UNIX;
  strcpy(address.sun_path, sockfile);
  len = sizeof(address);

  /*向服务器发送连接请求*/
  result = connect(sockfd, (struct sockaddr *)&address, len);
  if(result == -1)
  {
    printf ("ensure the server is up\n");
    xprintf("connect\n");
    close(sockfd);
    return -1;;
  }

  if((bytes = write(sockfd, &data, size)) == -1)
  { 
    /*发消息给服务器*/
    xprintf("write\n");
    close(sockfd);
    return -1;
  }

  close(sockfd);
  return 0;
}

/* add sockets[] with size sockets to fdset 
 * Note: fdset will be inited firstly. Return the maxfd
 */
int xfdset(fd_set *fds, int sockets[], int size)
{
  int i = 0, maxfd = 0;
  
  if(size <= 0)
    return -1;
  
  FD_ZERO(fds);
  
  for(; i < size; i++)
  {
    if(sockets[i] < 0)
    {
      xprintf("Wrong sockets\n");
      return -1;
    }
    
    if(sockets[i] > maxfd)
      maxfd = sockets[i];
      
    FD_SET(sockets[i], fds);  
  }
  
  return maxfd;  
}

/* refer to netstat source, 1 used, 0 not use, -1 error */
int xtcport_used(unsigned short port)
{
  char local_addr[64] = "", rem_addr[64] = "";
  char more[512] = "";
  int num, local_port, rem_port, d, state, timer_run, uid, timeout;

  unsigned long rxq, txq, time_len, retr, inode;

#define _PATH_PROCNET_TCP "/proc/net/tcp"
  char buffer[8192] = "";
  FILE *fp = fopen(_PATH_PROCNET_TCP, "r");
  if(fp == NULL)
  {
    xlogerr("open proc %s error!\n", _PATH_PROCNET_TCP);
    return -1;
  }
  
  while(fgets(buffer, sizeof(buffer), fp))
  {
    more[0] = '\0';
    num = sscanf(buffer, "%d: %64[0-9A-Fa-f]:%X %64[0-9A-Fa-f]:%X %X %lX:%lX %X:%lX %lX %d %d %ld %512s\n",
                         &d, local_addr, &local_port,
                         rem_addr, &rem_port, &state,
                         &txq, &rxq, &timer_run, &time_len, &retr, &uid, &timeout, &inode, more);
    if(num < 10)
      continue;
    
    if(local_port == port)
    {
      fclose(fp);
      return 1;
    }
  }

  fclose(fp);
  return 0;  
}

#ifdef TEST
void test_socket()
{
  printf("xtcport_used:%d\n", xtcport_used(34780));
  printf("%d\n", lookup_port("ftp", "udp", 0));
}
#endif

