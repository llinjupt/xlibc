#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/sysinfo.h>

#include "xtime.h"
#include "xexccmd.h"

/* stolen from busybox libbb time.c */

/* get the time of nanosecond */
unsigned long long xtimens(void)
{
  struct timeval tv;
  
  gettimeofday(&tv, NULL);
  
  return tv.tv_sec * 1000000000ULL + tv.tv_usec * 1000;
}

/* get the time of microsecond */
unsigned long long xtimeus(void)
{
  struct timeval tv;
  
  gettimeofday(&tv, NULL);

  return tv.tv_sec * 1000000ULL + tv.tv_usec;
}

/* get the time of millisecond */
unsigned long long xtimems(void)
{
  struct timeval tv;
  
  gettimeofday(&tv, NULL);

  return tv.tv_sec * 1000ULL + tv.tv_usec / 1000;
}

/* seconde */
unsigned xtimesec(void)
{
  return time(NULL);
}

void xprint_timestamp(FILE *fp)
{
  struct timeval tv;
  char *tstr;

  memset(&tv, 0, sizeof(tv));
  gettimeofday(&tv, NULL);

  tstr = asctime(localtime(&tv.tv_sec));
  tstr[strlen(tstr)-1] = 0;
  fprintf(fp, "Timestamp: %s\n", tstr);
}

int set_timezone()
{
  if(putenv("TZ=GMT-8" ) == -1)
        return -1;
  else
   tzset();
  
  return 0;
}

static struct tm *xtime_local()
{
  time_t timep;

  time(&timep);
  return localtime(&timep);
}

/*2011年 10月 31日 星期一 16:49:24 */
void xget_local_time()
{
  //char *wday[]={"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  char *wday[]={"日", "一", "二", "三", "四", "五", "六"};
  struct tm *p;

  p = xtime_local();
  if(p)
  {
    printf ("%04d年 %02d月 %02d日 ", (1900 + p->tm_year),(1 + p->tm_mon), p->tm_mday);
    printf("星期%s %d:%d:%d\n", wday[p->tm_wday],p->tm_hour, p->tm_min, p->tm_sec);
  }
}

/* 
 * seconds since boot, absolutely time, can be used 
 * as timeout reference
 */
int get_system_uptime()
{
  struct sysinfo info;

  if(sysinfo(&info))
     return -1;

  return info.uptime;
}

/* date -s 2011.05.13-12:26:05 */
#define SET_SYSTIME_CMD "/bin/date -s %d.%d.%d-%d:%d:%d"
int set_systime(int year, int month, int day, int hour, int minute, int second)
{
  //printf(SET_SYSTIME_CMD, year, month, day, hour, minute, second);
  return xcmd_agent(SET_SYSTIME_CMD, year, month, 
                   day, hour, minute, second);
}

int xtime_year()
{
  struct tm *p;

  p = xtime_local();
  return p ? 1900 + p->tm_year : -1;
}

/* 0 - 11 mean Jan to Desc */
int xtime_month()
{
  struct tm *p;

  p = xtime_local();
  return p ? p->tm_mon : -1;
}

int xtime_day()
{
  struct tm *p;

  p = xtime_local();
  return p ? p->tm_mday : -1;
}

int xtime_week()
{
  struct tm *p;

  p = xtime_local();
  return p ? p->tm_wday : -1;
}

int xtime_hour()
{
  struct tm *p;

  p = xtime_local();
  return p ? p->tm_hour : -1;
}

int xtime_minute()
{
  struct tm *p;

  p = xtime_local();
  return p ? p->tm_min : -1;
}

int xtime_second()
{
  struct tm *p;

  p = xtime_local();
  return p ? p->tm_sec : -1;
}

/* this second is from 1970-01-01, and set GMT/UTC time */
int xtime_gmt_setime(long second)
{
  time_t tmp = (time_t)second;
  struct tm *p = gmtime(&tmp);

  return set_systime(1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
}

long xtime_gmt_second()
{
  time_t timep;
  time(&timep);

  return (long)timep;
}

#ifdef TEST
void test_xtime()
{
  xtime_gmt_setime(10000);
  printf("%ld\n", xtime_gmt_second());
}
#endif

