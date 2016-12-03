#ifndef XTIME_H
#define XTIME_H
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

unsigned long long xtimens(void);
unsigned long long xtimeus(void);
unsigned long long xtimems(void);
unsigned xtimesec(void);

int set_timezone();
void xprint_timestamp(FILE *fp);
void xget_local_time();
int get_system_uptime();
int set_systime(int year, int month, int day, int hour, int minute, int second);
int xtime_gmt_setime(long second);
long xtime_gmt_second();

inline int xtime_year();
inline int xtime_month();
inline int xtime_day();
inline int xtime_week();
inline int xtime_hour();
inline int xtime_minute();
inline int xtime_second();
#endif /* XTIME_H */
