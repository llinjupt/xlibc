#ifndef XSOCKET_H
#define XSOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <netdb.h>

#include <linux/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/filter.h>
#include <limits.h>

/* The <limits.h> files in some gcc versions don't define LLONG_MIN,
   LLONG_MAX, and ULLONG_MAX.  Instead only the values gcc defined for
   ages are available.  */
#ifndef LLONG_MIN
#define LLONG_MIN     (-LLONG_MAX-1)
#endif
#ifndef LLONG_MAX
#define LLONG_MAX     __LONG_LONG_MAX__
#endif
#ifndef ULLONG_MAX
#define ULLONG_MAX    (LLONG_MAX * 2ULL + 1)
#endif

// <linux/socket.h> A general sockaddr The sa_data can be customized as IPv4 
// LL(Link layer) and etc 
//struct sockaddr {
//  sa_family_t  sa_family;      /* address family, AF_xxx       */
//  char        sa_data[14];    /* 14 bytes of protocol address */
//};


/*struct in_addr {
   unsigned long s_addr;
};
*/

/* <linux/in.h> INET Structure describing an Internet (IP) socket address. */
//#define __SOCK_SIZE__   16              /* sizeof(struct sockaddr)      */
//struct sockaddr_in {
//  sa_family_t      sin_family;     /* Address family               */
//  __be16          sin_port;       /* Port number                  */
//  struct in_addr  sin_addr;       /* Internet address             */

//  /* Pad to size of `struct sockaddr'. */
//  unsigned char   __pad[__SOCK_SIZE__ - sizeof(short int) -
//                  sizeof(unsigned short int) - sizeof(struct in_addr)];
//};

/* <linux/if_packet.h> the layer 2 protocol 
struct sockaddr_ll
{
  unsigned short  sll_family;
  __be16          sll_protocol;
  int             sll_ifindex;
  unsigned short  sll_hatype;
  unsigned char   sll_pkttype;
  unsigned char   sll_halen;
  unsigned char   sll_addr[8];
};*/

/* Bind addr and the length itself  */
typedef struct{
  socklen_t len;
  union{
          struct sockaddr sa;
          struct sockaddr_in sin;
  #if ENABLE_FEATURE_IPV6
          struct sockaddr_in6 sin6;
  #endif
  }u;
}lsockaddr;

/* Relevant parameters for lsockaddr accessing*/
enum {
  LSA_LEN_SIZE = offsetof(lsockaddr, u),
  LSA_SIZEOF_SA = sizeof(
                          union{
                                struct sockaddr sa;
                                struct sockaddr_in sin;
                              #if ENABLE_FEATURE_IPV6
                                struct sockaddr_in6 sin6;
                              #endif
                          }
                        )
};

/* for setsockopt operations */
extern const int const_int_1;

int xsocket(int domain, int type, int protocol);
int xbind(int fd, const char *ip, int port);
void xlisten(int s, int backlog);
void xconnect(int s, const struct sockaddr *s_addr, socklen_t addrlen);
int xselect(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, struct timeval *timeout);
                  
ssize_t xsendto(int s, const void *buf, size_t len, const struct sockaddr *to, socklen_t tolen);

extern inline int setsockopt_reuseaddr(int fd);
extern inline int setsockopt_broadcast(int fd);
extern inline int setsockopt_attachfilter(int fd, struct sock_fprog *filter);
int setsockopt_bindtodevice(int fd, const char *iface);
int setsockopt_keealive(int fd, int keep_idle, int keep_interval, int keep_count);

extern inline lsockaddr *get_sock_lsa(int fd);
extern inline lsockaddr *get_peer_lsa(int fd);                        

unsigned lookup_port(const char *port, const char *protocol, unsigned default_port);

int xunix_socket(char *sockfile);
int xunix_send(char *sockfile, void *data, int size);
int xfdset(fd_set *fds, int sockets[], int size);

#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif

#define strncpy_IFNAMSIZ(dst, src)  { strncpy(dst, src, IFNAMSIZ);}

int xtcport_used(unsigned short port);
#endif /* XSOCKET_H */
