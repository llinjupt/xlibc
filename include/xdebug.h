/*
 * Powered by BiTForest Co., Ltd 
 *
 * Copyright (C) 2018-2021 Reid Liu <lli_njupt@163.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#ifndef XDEBUG_H
#define XDEBUG_H
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>

#define XDEBUG

//#define XDEBUG_PURE
//#define XDEBUG_CGI

/* For embeded OS should open it to accelerate */
#define XDUMP_MACROS
#define XLOG_WITH_PRINT

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))
#endif

void _xprintf(FILE *fp, 
        const char *filename, 
              const int line, 
              const char *funcname, 
              const char *fmt, ...)__attribute__((format(printf, 5, 6)));

void _xsyslog(int priority,
              const char *filename,
              const int line,
              const char *funcname,
              const char *fmt, ...)__attribute__((format(printf, 5, 6)));

int xlogfile(const char *file);
int xloglevel(int level);
int xlogfsize(long long fsize);

void _xassert(FILE *fp,
        const char *filename,
              const int line,
              const char *funcname,
              const char *expstr);

void xdumphex(const void *addr, unsigned int len);
/*
 * IMPORTANT: Due to the way these color codes are emulated on Windows,
 * write them only using printf(), fprintf(), and fputs(). In particular,
 * do not use puts() or write().
 */
#define COLOR_NORMAL  ""
#define COLOR_RESET    "\033[0m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_RED    "\033[31m"
#define COLOR_GREEN    "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA  "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_BOLD_RED  "\033[1;31m"
#define COLOR_BOLD_GREEN  "\033[1;32m"
#define COLOR_BOLD_YELLOW  "\033[1;33m"
#define COLOR_BOLD_BLUE  "\033[1;34m"
#define COLOR_BOLD_MAGENTA  "\033[1;35m"
#define COLOR_BOLD_CYAN  "\033[1;36m"
#define COLOR_BG_RED  "\033[41m"
#define COLOR_BG_GREEN  "\033[42m"
#define COLOR_BG_YELLOW  "\033[43m"
#define COLOR_BG_BLUE  "\033[44m"
#define COLOR_BG_MAGENTA  "\033[45m"
#define COLOR_BG_CYAN  "\033[46m"

void color_fprintf(FILE *fp, const char *color, const char *fmt, ...)__attribute__((format(printf, 3, 4)));
 
/* xcprintf dump info with color discipline
 * color can be "foreground background textstyle"
 * foreground/background can be "red/green/yellow/blue/magenata/cyan"
 * textstyle can be "bold/empty"
 */
#define xcprintf(color, fmt...) color_fprintf(stdout, color, fmt)

/* xwprintf throws out warning info with red color */
#define xwprintf(fmt...)  xcprintf("red bold", fmt)
/* xwprintf throws out normal info with green color */
#define xiprintf(fmt...)  xcprintf("green bold", fmt)

/* At least given one parameter for gcc variable length parameter usage 
 * xprintf(), xerror, xdie()... will encounter complaint by GCC.
 */

#ifdef XDUMP_MACROS
#ifdef XDEBUG

/*xprintf dump info to stdout */
#ifdef XDEBUG_CGI
#define xprintf(x...)  _xprintf(NULL, __FILE__, __LINE__, __FUNCTION__, x)
#else
#define xprintf(x...)  _xprintf(stdout, __FILE__, __LINE__, __FUNCTION__, x)
#endif

/*xerror dump info to stderr */
#define xerror(x...)  {\
                        _xprintf(stderr, __FILE__, __LINE__, __FUNCTION__, x); \
                        color_fprintf(stderr, "red bold", "Tracing: %s\n", errno ? strerror(errno) : "Tracing failed"); \
                      }

#ifdef XDEBUG_CGI
#define xassert(exp)  ((exp) ? (void)0 : _xassert(NULL, __FILE__, __LINE__, __FUNCTION__, #exp))
#else
#define xassert(exp)  ((exp) ? (void)0 : _xassert(stdout, __FILE__, __LINE__, __FUNCTION__, #exp))
#endif

#define xlog(priority, fmt...)  _xsyslog(priority, __FILE__, __LINE__, __FUNCTION__, fmt)
#define xlogerr(fmt...)    _xsyslog(LOG_ERR, __FILE__, __LINE__, __FUNCTION__, fmt)
#define xloginfo(fmt...)        _xsyslog(LOG_INFO, __FILE__, __LINE__, __FUNCTION__, fmt)
#define xlogwarn(fmt...)        _xsyslog(LOG_WARNING, __FILE__, __LINE__, __FUNCTION__, fmt)

#else
#define xprintf(x...) 
#define xerror(x...) {}
#define xdie(x...) {}

#define xassert(exp) 
#define xlog(priority, fmt...)
#define xlogerr(fmt...)  
#define xloginfo(fmt...)        
#define xlogwarn(fmt...)      

#endif
/* xdie dump info and laster error to stderr and exit with abort */                        
#define xdie(x...)   do{    xlogerr(x);\
          xerror(x);\
          abort();\
      }while(0)

          
#else

void xprintf(const char *s, ...);
void xerror(const char *s, ...);
void xdie(const char *s, ...);
#endif

#endif /* XDEBUG_H */
