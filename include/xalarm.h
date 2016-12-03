#ifndef XALARM_H
#define XALARM_H

#include <string.h>
#include <limits.h>

#include "xdebug.h"
#include "xlist.h"

#define MAX_ALARMS_NUM 256
/* alarm flags  */
#define SA_REPEAT     0x01   /* keep repeating every X seconds */
#define ALARM_NAME_LEN (32)

struct xalarm_node
{
  char name[ALARM_NAME_LEN];   /* name for this alarm */
  struct timeval wait;        /* interval from register time */
  struct timeval left;        /* left time till alarming */

  unsigned int flags;          /* if with SA_REPEAT then loop alarm */

  void   *arg;
  void (*callback)(void *);

#ifdef TEST_DEBUG
  /* this is for debug */
  double s_usec;
#endif 
    
  struct xlist_head node;
};

struct xalarm_head
{
  volatile unsigned counter;   /* a counter for alarm list */
  double comp;                /* compensation  */
  
  struct xlist_head head;
  
  /* volatile is for optimization like -O2 */
  volatile char locked;        /* 1 means a alarm list is handling */
};

/* the ones you should need */
int xalarm_unregister(char name[ALARM_NAME_LEN]);
void xalarm_unregister_all(void);

/* return the unique alarm id or -1 when error */
int xalarm_register(char name[ALARM_NAME_LEN],
                    unsigned int sec,
                    unsigned int flags,
                    void (*callback)(void *),
                    void *arg);

/* the ones you shouldn't */
int xalarm_list_create(void);
void xalarm_list_dump();
void xalarm_list_destroy();
void xalarm_handler(int sig);

#endif /* XALARM_H */
