#ifndef XIPADDR_H
#define XIPADDR_H

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

/* below funcs of addr conversion are recommand for support different 
 * domain(AF_INET/AF_INET6). Err then die!
 */
inline int xinet_ntop(int af, const void *src, char *dst, socklen_t cnt);
inline int xinet_pton(int af, const char *src, void *dst);
int is_valid_ip(char *ip);

const char *xip_subnet(const char *ip, const char *mask);
int xip_host(const char *ip, const char *mask);
int xip_subnet2(const char *ip, const char *mask);
const char *xip_create(const char *ip, const char *mask, int hostid);

/* below funcs are obsolete. */

/*struct in_addr {
   unsigned long s_addr;
};
*/

//
//struct sockaddr_in{
//  short int sin_family;              /* Address family */
//  unsigned short int sin_port;       /* Port number */
//  struct in_addr sin_addr;           /* Internet address */
//  unsigned char sin_zero[8];         /* Same size as struct sockaddr */
//};

//struct sockaddr {
//  unsigned short sa_family;         /* address family, AF_xxx */
//  char sa_data[14];                 /* 14 bytes of protocol address */
//};

#ifndef SOCKADDR_IP
#define SOCKADDR_IP(addr)   (((struct sockaddr_in *)&addr)->sin_addr.s_addr)
#endif
#ifndef SOCKADDR_PIP
#define SOCKADDR_PIP(addr)   (((struct sockaddr_in *)addr)->sin_addr.s_addr)
#endif

#endif /* XIPADDR_H */
