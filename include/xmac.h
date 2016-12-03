#ifndef XMAC_H
#define XMAC_H

int xstr2mac1(unsigned char mac[6], const char *macstr);
int xstr2mac2(unsigned char mac[6], const char *macstr);
int xstr2mac(unsigned char mac[6], const char *macstr);
int xmac2str(char *macstr, const unsigned char mac[6]);
int xmac2str2(char *macstr, const unsigned char mac[6]);

void xmac_add1(unsigned char mac[6]);
void xmac_sub1(unsigned char mac[6]);

void inline xprintmac(unsigned char mac[6]);

typedef enum
{
  UNICAST_MAC,
  BROADCAST_MAC,
  MULTICAST_MAC,
} xmac_t;

xmac_t xmac2type(unsigned char mac[6]);
#endif /* XMAC_H */
