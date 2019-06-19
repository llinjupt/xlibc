#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "xfcfg.h"
#include "xcharstr.h"
#include "xdebug.h"
#include "xmac.h"

/* bool */
int xfcfg_parse_bool(const char *value, void *arg)
{
  char *dest = arg;
  int ret = 0;

  if(strcasecmp(value, "yes") == 0
     || strcasecmp(value, "true") == 0
     || strcasecmp(value, "1") == 0)
    *dest = 1;
  else if(strcasecmp(value, "no") == 0
     || strcasecmp(value, "false") == 0
     || strcasecmp(value, "0") == 0)
    *dest = 0;
  else
    ret = -1;

  return ret;
}

/* ipv4 like 192.168.0.1 */
int xfcfg_parse_ipv4(const char *value, void *arg)
{
  struct in_addr *addr = arg;
  
  if(inet_aton(value, addr) == 0)
    return -1;

  return 0;
}

int xfcfg_parse_ipv4str(const char *value, void *arg)
{
  struct in_addr addr;
  char *ipstr = arg;
  
  if(inet_aton(value, &addr) == 0)
    return -1;
  
  memcpy(ipstr, inet_ntoa(addr), 16);
  return 0;
}

int xfcfg_parse_str(const char *value, void *arg)
{
  char *tmp = arg;
  if(value == NULL)
    return -1;

  xstrncpy(tmp, value, XFCFG_DEFAULT_MAX_LEN);

  return 0;
}

int xfcfg_parse_int(const char *value, void *arg)
{
  long int *dest = arg;
  char *endptr;
  
  *dest = strtol(value, &endptr, 0);
  
  return endptr[0] == '\0';
}

int xfcfg_parse_mac(const char *value, void *arg)
{
  return xstr2mac((unsigned char *)arg, value);
}

int xfcfg_parse_size(const char *value, void *arg)
{
  long *val = (long *)arg;
  
  unsigned char unit = 0;
  int len = 0;
  long scale = 0;

  len = strlen(value);
  unit = value[len - 1];

  switch (unit) 
  {
    case 'K':
    case 'k':
      len--;
      scale = 1024;
      break;

    case 'M':
    case 'm':
      len--;
      scale = 1024 * 1024;
      break;
    case 'G':
    case 'g':
      len--;
      scale = 1024 * 1024 * 1024;
      break;
    default:
      scale = 1;
  }

  *val = strtol(value, NULL, 10) * scale;
  return 0;
}

int xfcfg_from_file(xfcfg_option_t *xfcfg_options, char *file)
{
  int i = 0;
  int ret = 0, line = 0;
  
  char read_buf[1024] = "";
  char *line_comment = NULL;
  char *name = NULL, *val = NULL;

  FILE *fp = fopen(file, "r");
  if(fp == NULL)
  {
    printf("Unable to open config file: %s\n", file);
    return -1;
  }

  /* load default value */
  for(i = 0; xfcfg_options[i].parse; i++)
  {
    if(strlen(xfcfg_options[i].def) == 0)
      continue;
    
    ret = xfcfg_options[i].parse(xfcfg_options[i].def, xfcfg_options[i].value);
    if(ret < 0)
      printf("parse [%s] default value error\n", xfcfg_options[i].name);
  }

  while(fgets(read_buf, 1024, fp) != 0)
  {
    line++;

    if(read_buf[0] == '#')
      continue;

    line_comment = strchr(read_buf, '#');
    if(line_comment)
      *line_comment = '\0';

    name = read_buf;
    name = xstrstrip(name);
    if(strlen(name) == 0)
      continue;

    val = strchr(name, '=');
    if(val == NULL)
    {
      printf("Warning: line %d: can't analysing %s\n", line, read_buf);
      continue;
    }
    *val = '\0'; val++;
    
    name = xstrstrip(read_buf); 
    val = xstrstrip(val); 
    if(*val == '\0')
     continue;

    printf("%s=%s\n", name, val);
    
    for(i = 0; xfcfg_options[i].parse; i++)
    {
      if(strcmp(xfcfg_options[i].name, name) != 0)
        continue;

      ret = xfcfg_options[i].parse(val, xfcfg_options[i].value);
      if(ret < 0)
        printf("parse [%s] value error\n", xfcfg_options[i].name);
      break;
    }
  }

  fclose(fp);
  return 0;
}

#ifdef TEST
void test_xfcfg()
{
  int xfcfg_cfg_bool = 0, xfcfg_cfg_int = 0;
  char xfcfg_cfg_str[XFCFG_DEFAULT_MAX_LEN] = "";
  int xfcfg_cfg_ipv4 = 0;
  char xfcfg_cfg_ipv4str[16] = "";
  unsigned char xfcfg_cfg_mac[6] = {0};
  long xfcfg_cfg_size = 0;

  xfcfg_option_t xfcfg_options[] = 
  {
    {"cfg_bool",    xfcfg_parse_bool,  &xfcfg_cfg_bool,   "0"},
    {"cfg_int",     xfcfg_parse_int,   &xfcfg_cfg_int,   "0"},
    {"cfg_str",      xfcfg_parse_str,   xfcfg_cfg_str,     "0"},
    {"cfg_ipv4",    xfcfg_parse_ipv4,  &xfcfg_cfg_ipv4,   "0"},
    {"cfg_ipv4str",  xfcfg_parse_ipv4str,xfcfg_cfg_ipv4str,"192.168.10.1"},
    {"cfg_mac",      xfcfg_parse_mac,    xfcfg_cfg_mac,    "00:11:22:33:44:55"},
    {"cfg_size",    xfcfg_parse_size,   &xfcfg_cfg_size,  "1024"},
    
    {"", NULL, NULL, ""},
  };
  
  xfcfg_from_file(xfcfg_options, "./src/xfcfg/xfcfg.conf");
  printf("xfcfg_cfg_bool\t=%d\n", xfcfg_cfg_bool);
  printf("xfcfg_cfg_str\t=%s\n", xfcfg_cfg_str);
  printf("xfcfg_cfg_int\t=%d\n", xfcfg_cfg_int);
  printf("xfcfg_cfg_int\t=0x%x\n", xfcfg_cfg_ipv4);
  printf("xfcfg_cfg_ipv4str\t=%s\n", xfcfg_cfg_ipv4str);
  printf("xfcfg_cfg_mac\t=%02x%02x%02x%02x%02x%02x\n", 
         xfcfg_cfg_mac[0], xfcfg_cfg_mac[1], xfcfg_cfg_mac[2],
         xfcfg_cfg_mac[3], xfcfg_cfg_mac[4], xfcfg_cfg_mac[5]);
  
  printf("cfg_size\t=%ld\n", xfcfg_cfg_size);
}
#endif
