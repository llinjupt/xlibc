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

extern inline int xtime_year();
extern inline int xtime_month();
extern inline int xtime_day();
extern inline int xtime_week();
extern inline int xtime_hour();
extern inline int xtime_minute();
extern inline int xtime_second();
#endif /* XTIME_H */
