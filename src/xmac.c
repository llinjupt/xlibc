#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "xdebug.h"
#include "xmac.h"

/* 
 * convert macstring "00:01:02:03:04:05" to mac array[6] 
 * = {0, 1, 2, 3, 4, 5}.
 */
int xstr2mac1(unsigned char mac[6], const char *macstr)
{
  int i = 0;
  char tmp[3] = "";
  
  int len = strlen(macstr);
  
  if(len < 17)
    return -1;

  if(macstr == NULL || mac == NULL)
    return -1;

  for(i = 0; i < 17; i++)
  {
    if((i + 1) % 3 == 0)
    {
      if(macstr[i] != ':')
        return -1;
    }
    else if(isxdigit(macstr[i]) == 0)
      return -1;
  }

  for(i = 0; i < 6; i++)
  {
    strncpy(tmp, macstr + i * 3, 2);
    mac[i] = strtol(tmp, NULL, 16);
  }

  return 0;
}

/* like xstr2mac1 but ignore `:'  */
int xstr2mac2(unsigned char mac[6], const char *macstr)
{
  int i = 0;
  int len = 0;
  char tmp[3] = "";

  if(macstr == NULL || mac == NULL)
    return -1;

  len = strlen(macstr);
  if(len < 12)
    return -1;

  for(i = 0; i < 12; i++)
  {
    if(isxdigit(macstr[i]) == 0)
      return -1;
  }
  
  for(i = 0; i < 6; i++)
  {
    strncpy(tmp, macstr + i * 2, 2);
    mac[i] = strtol(tmp, NULL, 16);
  }
  
  return 0;
}

/* support both "11:22:33:44:55:66" and "112233445566" */
int xstr2mac(unsigned char mac[6], const char *str)
{
  int ret = xstr2mac1(mac, str);
  
  if(ret == 0)
    return 0;
  
  return xstr2mac2(mac, str);
}

/* reverse as str2mac and out as upper characters */
int xmac2str(char *macstr, const unsigned char mac[6])
{
  int len = 0;
  
  if(macstr == NULL || mac == NULL)
    return -1;

  len = sprintf(macstr, "%02x:%02x:%02x:%02x:%02x:%02x", 
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  
  if(len < 0)
    return -1;
    
  return 0;
}

/* like mac2str but ignore `:'  */
int xmac2str2(char *macstr, const unsigned char mac[6])
{
  int len = 0;
  
  len = sprintf(macstr, "%02x%02x%02x%02x%02x%02x", 
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  
  if(len < 0)
    return -1;
    
  return 0;
}

/* mac add 1, if all 0xff then return all 0x00 */
void xmac_add1(unsigned char mac[6])
{
  int i = 5;
  for(; i >= 0; i--)
  {
    if(mac[i] != 0xff)
    {
      mac[i]++;
      return;
    }
    else
      mac[i] = 0x00;
  }
}

/* mac add 1, if all 0x00 then return all 0xff */
void xmac_sub1(unsigned char mac[6])
{
  int i = 5;
  for(; i >= 0; i--)
  {
    if(mac[i] > 0)
    {
      mac[i]--;
      return;
    }
    else
      mac[i] = 0xff;
  }
}

/* return mac type like UNICAST_MAC */
xmac_t xmac2type(unsigned char mac[6])
{
  if(mac[0] & 0x01)
  {
    if((mac[0] & mac[1] & mac[2] & mac[3] & mac[4] & mac[5]) == 0xff)
      return BROADCAST_MAC;
    else
      return MULTICAST_MAC;
  }
  
  return UNICAST_MAC;
}

void inline xprintmac(unsigned char mac[6])
{
  printf("%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

#ifdef TEST
void test_xmac()
{
  int i = 0;
  unsigned char mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xf0};
  
  for(; i < 20; i++)
  {
    xprintmac(mac);
    xmac_add1(mac);
  }
}
#endif
