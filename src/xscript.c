#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

void xscript_remove(char *sfile)
{
  unlink(sfile);
}

int xscript_add(char *sfile, const char *format, ...)
{
  FILE *fp = NULL;
  va_list arg;

  fp = fopen(sfile, "a+");
  if(fp == NULL) 
    return -1;
    
  va_start(arg, format);
  vfprintf(fp, format, arg);
  va_end(arg);
  
  fprintf(fp, "\n");
  fclose(fp);
  
  return 0;
}

int xscript_create(char *sfile)
{
  FILE *fp = NULL;

  fp = fopen(sfile, "w");
  if(fp == NULL) 
    return -1;
    
  fprintf(fp, "#!/bin/sh\n");
  fclose(fp);

  return 0;
}

void xscript_run(char *sfile)
{
  chmod(sfile, 0x777);
  system(sfile);
}

#ifdef TEST
#define XSCRTIPT_PATH "/tmp/xscript.sh"
void test_xscript()
{
  xscript_create(XSCRTIPT_PATH);

  xscript_add(XSCRTIPT_PATH, "echo %s", "hello");
  xscript_add(XSCRTIPT_PATH, "ls -l");
  xscript_run(XSCRTIPT_PATH);

  xscript_remove(XSCRTIPT_PATH);
}

#endif
