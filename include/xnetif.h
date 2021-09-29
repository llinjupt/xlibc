#ifndef XNETIF_H
#define XNETIF_H

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>

#define _(String) (String)

/* This structure defines protocol families and their handlers. */
struct aftype {
  char *name;
  char *title;
  int af;
  int alen;
  char *(*print) (unsigned char *);
  char *(*sprint) (struct sockaddr *, int numeric);
  int (*input) (int type, char *bufp, struct sockaddr *);
  void (*herror) (char *text);
  int (*rprint) (int options);
  int (*rinput) (int typ, int ext, char **argv);

  /* may modify src */
  int (*getmask) (char *src, struct sockaddr * mask, char *name);

  int fd;
  char *flag_file;
};

void aftrans_def(char *tool, char *argv0, char *dflt);
int aftrans_opt(const char *arg);
struct aftype *get_afntype(int af);
struct aftype *get_aftype(const char *name);

/* This structure defines hardware protocols and their handlers. */
struct hwtype {
    char *name;
    char *title;
    int type;
    int alen;
    char *(*print) (unsigned char *);
    int (*input) (char *, struct sockaddr *);
    int (*activate) (int fd);
    int suppress_null_addr;
};

struct hwtype *get_hwtype(const char *name);
struct hwtype *get_hwntype(int type);
void          print_hwlist(int type);
struct aftype *get_aftype(const char *name);
struct aftype *get_afntype(int type);
void          print_aflist(int type);
int           hw_null_address(struct hwtype *hw, void *addr);

extern inline int xgetif_flags(short *flags, const char *ifname);
int xsetif_flag(const char *ifname, short flag);
int xclrif_flag(const char *ifname, short flag);

extern inline int xgetif_hwaddr(unsigned char hwaddr[6], const char *ifname);
int xsetif_hwaddr(unsigned char hwaddr[6], const char *ifname);

extern inline int xgetif_metric(int *metric, const char *ifname);
extern inline int xgetif_mtu(int *mtu, const char *ifname);
extern inline int xgetif_txqlen(int *txqlen, const char *ifname);
extern inline int xgetif_index(int *index, const char *ifname);

/* very usefull functions when you want to bind IF info to a socket */
extern inline int xgetif_addr(struct sockaddr *addr, const char *ifname);
extern inline int xgetif_dstaddr(struct sockaddr *addr, const char *ifname);
extern inline int xgetif_brdaddr(struct sockaddr *addr, const char *ifname);
extern inline int xgetif_maskaddr(struct sockaddr *addr, const char *ifname);

int xgetif_strip(char *addrstr, const char *ifname,  \
                 int(*xgetif_fun)(struct sockaddr*, const char *));
#define xgetif_straddr(addrstr, ifname) xgetif_strip(addrstr, ifname, xgetif_addr)
#define xgetif_strdstaddr(addrstr, ifname) xgetif_strip(addrstr, ifname, xgetif_dstaddr)
#define xgetif_strbrdaddr(addrstr, ifname) xgetif_strip(addrstr, ifname, xgetif_brdaddr)
#define xgetif_strmaskaddr(addrstr, ifname) xgetif_strip(addrstr, ifname, xgetif_maskaddr)

extern inline int xsetif_addr(struct sockaddr *addr, const char *ifname);
extern inline int xsetif_maskaddr(struct sockaddr *addr, const char *ifname);

int xsetif_strip(char *addrstr, const char *ifname,  \
                 int(*xsetif_fun)(struct sockaddr*, const char *));
#define xsetif_straddr(addrstr, ifname) xsetif_strip(addrstr, ifname, xsetif_addr)
#define xsetif_strmaskaddr(addrstr, ifname) xsetif_strip(addrstr, ifname, xsetif_maskaddr)

extern inline int xsetif_promisc(const char *ifname);
extern inline int xclrif_promisc(const char *ifname);

int get_default_gw(unsigned char gateway[16]);
int set_default_gw(unsigned char gateway[16]);

int xarp_getmac(char macstr[18], const char *req_ip);
int xarp_getip(char ipstr[16], const char *req_mac);

int xlease_getmac(char macstr[18], const char *req_ip);
#endif /* XNETIF_H */
