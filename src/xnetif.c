#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <ctype.h>

#include <libx.h>

/* Core function to get IF dev info. If you need to add new ioctl
 * then inset into it, And define another exported one too.
 */
static int xgetif_info(void *info, int size, int req, const char *ifname)
{
  int skfd = -1;
  struct ifreq ifr;
  
  /* now we only support ipv4 */
  skfd = xsocket(AF_INET, SOCK_DGRAM, 0);

  strncpy(ifr.ifr_name, ifname, IFNAMSIZ);  
  if(ioctl(skfd, req, &ifr) < 0)
  {
    memset(info, 0, size);
    close(skfd);
    return -1;
  }
  else
  {
    switch(req)
    {
      case SIOCGIFFLAGS:
        memcpy(info, &(ifr.ifr_flags), size);
        break;
      case SIOCGIFHWADDR:
        memcpy(info, ifr.ifr_hwaddr.sa_data, size);        
        break;
      case SIOCGIFMETRIC:
        memcpy(info, &(ifr.ifr_metric), size);  
        break;        
      case SIOCGIFMTU:
        memcpy(info, &(ifr.ifr_mtu), size);  
        break;
      case SIOCGIFTXQLEN:
        memcpy(info, &(ifr.ifr_qlen), size);  
        break;
      case SIOCGIFADDR:
        memcpy(info, &(ifr.ifr_addr), size);  
        break;
      case SIOCGIFDSTADDR:
        memcpy(info, &(ifr.ifr_dstaddr), size);  
        break;
      case SIOCGIFBRDADDR:
        memcpy(info, &(ifr.ifr_broadaddr), size);  
        break;                
      case SIOCGIFNETMASK:
        memcpy(info, &(ifr.ifr_netmask), size);  
        break;
      case SIOCGIFINDEX:
        memcpy(info, &(ifr.ifr_ifindex), size);
        break;
      default:
        xprintf("xgetif_info Not support req :0x%x\n", req);
        memset(info, 0, size);
        close(skfd);
        return -1;
    }    
  }
  close(skfd);
  return 0;
}

inline int xgetif_flags(short *flags, const char *ifname)
{
  return xgetif_info(flags, sizeof(short), SIOCGIFFLAGS, ifname);
}

/* Set a certain interface flag. */
int xsetif_flag(const char *ifname, short flag)
{
  struct ifreq ifr;
  int skfd = -1;
  
  /* now we only support ipv4 */
  skfd = xsocket(AF_INET, SOCK_DGRAM, 0);

  strncpy(ifr.ifr_name, ifname, IFNAMSIZ);  
  if(ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0)
  {
    xerror("xgetif_info Failed for req :0x%x\n", SIOCGIFFLAGS);
    close(skfd);
    return -1;
  }

  ifr.ifr_flags |= flag;
  if(ioctl(skfd, SIOCSIFFLAGS, &ifr) < 0)
  {
    xerror("SIOCSIFFLAGS");
    close(skfd);
    return -1;
  }
  
  close(skfd);
  return 0;
}

int xclrif_flag(const char *ifname, short flag)
{
  struct ifreq ifr;
  int skfd = -1;
  
  /* now we only support ipv4 */
  skfd = xsocket(AF_INET, SOCK_DGRAM, 0);

  strncpy(ifr.ifr_name, ifname, IFNAMSIZ);  
  if(ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0)
  {
    xerror("xgetif_info Failed for req :0x%x\n", SIOCGIFFLAGS);
    close(skfd);
    return -1;
  }
    
  ifr.ifr_flags &= ~flag;
  if(ioctl(skfd, SIOCSIFFLAGS, &ifr) < 0)
  {
    xerror("SIOCSIFFLAGS");
    close(skfd);
    return -1;
  }
  
  close(skfd);
  return 0;
}

inline int xsetif_promisc(const char *ifname)
{
  return xsetif_flag(ifname, IFF_PROMISC);
}

inline int xclrif_promisc(const char *ifname)
{
  return xclrif_flag(ifname, IFF_PROMISC);
}

inline int xgetif_hwaddr(unsigned char hwaddr[6], const char *ifname)
{
  return xgetif_info(hwaddr, 6 * sizeof(unsigned char)/* Be 6 for AF_INET*/, SIOCGIFHWADDR, ifname);
}

/* normally the metric value should add 1 */
inline int xgetif_metric(int *metric, const char *ifname)
{
  return xgetif_info(metric, sizeof(int), SIOCGIFMETRIC, ifname);
}

inline int xgetif_mtu(int *mtu, const char *ifname)
{
  return xgetif_info(mtu, sizeof(int), SIOCGIFMTU, ifname);
}

inline int xgetif_txqlen(int *txqlen, const char *ifname)
{
  return xgetif_info(txqlen, sizeof(int), SIOCGIFTXQLEN, ifname);
}

inline int xgetif_index(int *index, const char *ifname)
{
  return xgetif_info(index, sizeof(int), SIOCGIFINDEX, ifname);
}

inline int xgetif_addr(struct sockaddr *addr, const char *ifname)
{
  return xgetif_info(addr, sizeof(struct sockaddr), SIOCGIFADDR, ifname);
}

static int xsetif_info(struct sockaddr *addr, int req, const char *ifname)
{
  struct ifreq ifr;
  int skfd = -1;

  memset(&ifr, 0, sizeof(struct ifreq));
  switch(req)
  {
    case SIOCSIFADDR:
    case SIOCSIFNETMASK:
      memcpy(&ifr.ifr_addr, addr, sizeof(struct sockaddr));
      break;
    default:
      xprintf("xsetif_info Not support req :0x%x\n", req);
      return -1;
  }

  /* now we only support ipv4 */
  skfd = xsocket(AF_INET, SOCK_DGRAM, 0);
  strncpy(ifr.ifr_name, ifname, IFNAMSIZ);  
  if(ioctl(skfd, req, &ifr) < 0)
  {
    close(skfd);
    return -1;
  }
  
  close(skfd);
  return 0;
}

inline int xsetif_addr(struct sockaddr *addr, const char *ifname)
{
  return xsetif_info(addr, SIOCSIFADDR, ifname);
}

int xsetif_hwaddr(unsigned char hwaddr[6], const char *ifname)
{
  int result_len = 0;
  char *result = NULL;
  char macstr[18] = "";
  
  if(xmac2str(macstr, hwaddr) != 0)
    return -1;
  result_len = xscomb_cmd(&result, "ifconfig %s down && ifconfig %s hw ether %s && ifconfig %s up", 
		          ifname, ifname, macstr, ifname);
  if(result_len == 0)
    return 0;
  
  free(result);
  return -1;  
}

inline int xsetif_maskaddr(struct sockaddr *addr, const char *ifname)
{
  return xsetif_info(addr, SIOCSIFNETMASK, ifname);
}

inline int xgetif_dstaddr(struct sockaddr *addr, const char *ifname)
{
  return xgetif_info(addr, sizeof(struct sockaddr), SIOCGIFDSTADDR, ifname);
}

inline int xgetif_brdaddr(struct sockaddr *addr, const char *ifname)
{
  return xgetif_info(addr, sizeof(struct sockaddr), SIOCGIFBRDADDR, ifname);
}

inline int xgetif_maskaddr(struct sockaddr *addr, const char *ifname)
{
  return xgetif_info(addr, sizeof(struct sockaddr), SIOCGIFNETMASK, ifname);
}

int xgetif_strip(char *addrstr, const char *ifname, 
                 int(*xgetif_fun)(struct sockaddr*, const char *))
{
  int ret = 0;
  struct sockaddr addr;
  ret = (*xgetif_fun)(&addr, ifname);
  if(ret == 0)
    return xinet_ntop(AF_INET, (unsigned char *)(&SOCKADDR_IP(addr)), 
                addrstr, 16 /* AF_INET */);
  
  return ret;
}

int xsetif_strip(char *addrstr, const char *ifname, 
                 int(*xsetif_fun)(struct sockaddr*, const char *))
{
  int ret = 0;
  unsigned int ip = 0;
  struct sockaddr addr;
  struct sockaddr_in *sin;
  
  ret = xinet_pton(AF_INET, addrstr, &ip);
  if(ret != 0)
    return ret;

  sin = (struct sockaddr_in *)&addr;
  memset(sin, 0, sizeof(struct sockaddr_in));
    addr.sa_family = AF_INET;  
  memcpy(&(sin->sin_addr), &ip, 4);
  if(ret == 0)
    ret = (*xsetif_fun)(&addr, ifname);

  return ret;
}

/* get default gateway need "ip" command */
int get_default_gw(unsigned char gateway[16])
{
  int result_len = 0;
  char *result = NULL;
  char *tmp;

  result_len = xspipe_cmd("ip route", &result);

  if(result_len == 0)
    return -1;
  tmp = strstr(result, "default");
  if(tmp == NULL)
  {
    free(result);
    return -1;
  }
  sscanf(tmp, "%*s%*s%s", gateway);
  
  return 0;
}

int set_default_gw(unsigned char gateway[16])
{  
  int result_len = 0;
  char *result = NULL;

  /* delete old default gw */
  if(xspipe_cmd("route del default", &result) > 0)
    free(result);
  
  result_len = xscomb_cmd(&result, "route add default gw %s", gateway);
  if(result_len)
  {
    free(result);
    return -1;
  }
  
  return 0;
}

/* Get an IP's MAC address from the /fp/net/arp ARP cache. */
#define PROC_NET_ARP_FILE "/proc/net/arp"
int xarp_getmac(char macstr[18], const char *req_ip)
{
  int ret = -1;
  
  FILE *fp = NULL;
  char ip[16] = "";
  char mac[18] = "";

  fp = fopen(PROC_NET_ARP_FILE, "r");
  if(fp == NULL)
    return -1;

  /* Skip first line */
  while (!feof(fp) && fgetc(fp) != '\n');

  /* Find ip, copy mac in reply */
  while(!feof(fp) 
         && (fscanf(fp, " %15[0-9.] %*s %*s %17[A-Fa-f0-9:] %*s %*s", ip, mac) == 2))
  {
    if(xstrcmp(ip, req_ip) == 0) 
    {
      memcpy(macstr, mac, 18);
      ret = 0;
      break;
    }
  }
  
  fclose(fp);
  return ret;
}

/* Get an MAC's IP address from the /proc/net/arp ARP cache. */
int xarp_getip(char ipstr[16], const char *req_mac)
{
  int ret = -1;
  
  FILE *fp = NULL;
  char ip[16] = "";
  char mac[18] = "";

  fp = fopen(PROC_NET_ARP_FILE, "r");
  if(fp == NULL)
    return -1;

  /* Skip first line */
  while (!feof(fp) && fgetc(fp) != '\n');

  /* Find ip, copy mac in reply */
  while(!feof(fp) 
         && (fscanf(fp, " %15[0-9.] %*s %*s %17[A-Fa-f0-9:] %*s %*s", ip, mac) == 2))
  {
    if(xstrcasecmp(mac, req_mac) == 0) 
    {
      memcpy(ipstr, ip, 16);
      ret = 0;
      break;
    }
  }
  
  fclose(fp);
  return ret;
}

#define HOSTNAME_LEN 16
typedef struct
{
  unsigned char chaddr[16];
  unsigned int yiaddr;
  unsigned int expires;
  unsigned char hostname[HOSTNAME_LEN];
}leases_t;

#define DHCPD_LEASE_FILE  "/var/udhcpd.leases"
int xlease_getmac(char macstr[18], const char *req_ip)
{
  leases_t lease;
  struct in_addr addr = {0};

  FILE *fp = fopen(DHCPD_LEASE_FILE, "r");
  if(fp == NULL) 
  {
    xlogerr("could not open input file!\n");
    return -1;
  }

  /* "00:00:00:00:00:00 255.255.255.255 Wed Jun 30 21:49:08 1993" */
  while(fread(&lease, sizeof(lease), 1, fp)) 
  {
    addr.s_addr = lease.yiaddr;
    if(xstrcmp(req_ip, inet_ntoa(addr)) != 0)
      continue;
    
    sprintf(macstr, "%02x:%02x:%02x:%02x:%02x:%02x", 
            lease.chaddr[0], lease.chaddr[1], lease.chaddr[2],
            lease.chaddr[3], lease.chaddr[4], lease.chaddr[5]);
    
    fclose(fp);            
    return 0;
  }
  
  fclose(fp);
  return -1;
}

#ifdef TEST
void test_xnetif()
{
  char mac[18] = "";
  char ip[16] = "";

  if(xarp_getmac(mac, "192.168.10.1") == 0)
    printf("mac:%s\n", mac);
  
  if(xarp_getip(ip, "78:24:af:c6:0A:5f") == 0)
    printf("ip:%s\n", ip);
}
#endif
