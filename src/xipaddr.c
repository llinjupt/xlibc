#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "xdebug.h"
#include "xipaddr.h"

/* Why I convert network address between 0x.... to xx.xx.xx.xx have to call
 * struct sockaddr_in? sockaddr_in is none business of it. So we call inet_ntop
 * or inet_pton which can support both of ipv4  and ip6.
 */

inline int xinet_ntop(int af, const void *src, char *dst, socklen_t cnt)
{
  inet_ntop(af, src, dst, cnt);
  if(dst == NULL)
    return -1;
  // xerror("xinet_ntop \n");
  
  return 0; 
}

inline int xinet_pton(int af, const char *src, void *dst)
{
  /* inet_pton() returns a negative value and sets errno to EAFNOSUPPORT if af 
   * does not contain a valid address fam-ily.   0 is  returned  if  src does 
   * not contain a character string representing a valid network address in the
   *  specified address family.
   */
  int ret = inet_pton(af, src, dst);
  if(ret <= 0)
    return -1;

  return 0; 
}

/* 
 * if ip is "192.168.10.1", mask is "255.255.255.0"
 * then return subnet as "192.168.10.0"
 */
const char *xip_subnet(const char *ip, const char *mask)
{
  struct in_addr ip_addr, mask_addr;
  
  xassert(ip);
  xassert(mask);

  inet_aton(ip, &ip_addr);
  inet_aton(mask, &mask_addr);
  
  ip_addr.s_addr &= mask_addr.s_addr;
  return inet_ntoa(ip_addr);
}

/* get the net byteorder subnet */
int xip_subnet2(const char *ip, const char *mask)
{
  struct in_addr addr;
  const char *subnet = xip_subnet(ip, mask);

  if(subnet == NULL)
    return 0;

  inet_aton(subnet, &addr);
  return addr.s_addr;
}

const char *xip_create(const char *ip, const char *mask, int hostid)
{
  xassert(ip);
  xassert(mask);

  struct in_addr addr = {0};
  int subnet = xip_subnet2(ip, mask);

  addr.s_addr = subnet | htonl(hostid);
  return inet_ntoa(addr);
}

/* 
 * if ip is "192.168.10.1", mask is "255.255.255.0"
 * then return host as 1
 */
int xip_host(const char *ip, const char *mask)
{
  struct in_addr ip_addr, mask_addr;
  
  xassert(ip);
  xassert(mask);

  inet_aton(ip, &ip_addr);
  inet_aton(mask, &mask_addr);
  
  ip_addr.s_addr &= (~mask_addr.s_addr);
  return ntohl(ip_addr.s_addr);
}

/* should we extend inet_addr and etc. ? */
int is_valid_ip(char *ip_string)
{
  int ip[4];
  int i=0;
  int ret =0;
  if(sscanf(ip_string,"%d.%d.%d.%d",&ip[0],&ip[1],&ip[2],&ip[3]) != 4)
    return -1;
  for(i=0;i<4;i++)
    {
      if(ip[i] >255 || ip[i] < 0)
        return -1;
      if((ip[i] != 255) && (ip[i] != 0))
        ret =1;
    }
  if(ret == 0)  
    return -1;
    
  return 0;  
}

#ifdef TEST
void test_xipaddr()
{
  const char *ip = "192.168.10.1";
  const char *mask = "192.168.10.1";
  
  printf("%s\n", xip_create(ip, mask, 11));
}

#endif
