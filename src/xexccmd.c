#define _GNU_SOURCE

#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "xfile.h"
#include "xdebug.h"
#include "xexccmd.h"

#define REDIRECT_TO_NULL " > /dev/null 2>&1"
#define XCMD_WAIT_TIME 5

/* Warning: DO NOT run daemon commands or long run commands with Below xcmdfuns
 * Even through all xcmdfuns have set a alarm to prevent long time waiting.
 * But when you run a daemon with '&' then this prevention is lapsed. The parent 
 * is returned when timeout but the daemon is running under background. 
 */
 
/* And use _exit instead of exit because child don't need to flush buffers of 
 * file-fds opened by parent. Otherwise have open and outputed(buffed) datum 
 * of parent maybe output two times. 
 */

/* For forked child to be timeout. This alarm should not be done in parent
 * process for rushing to seize SIGALARM signal.
 */
/*static void alarm_exit(int alnum)
{
  xprintf("alarm:%d", alnum);
  exit(1);
}*/

/* Be clear when you call this may need todo setu/gid pipe opens a 
 * process by creating a pipe, forking, and invoking the shell. So 
 * all outputed from child will be printout to stdout/stderr
 */
/*size_t xcpipe_cmd(char *cmd, char **output)
{
    FILE *fp;
    char buf[PIPE_BUF];
    int len = 0;

    *output = malloc(1);
    strcpy(*output, "");
    if ((fp = popen(cmd, "r")) == NULL)
        return len;
    while((fgets(buf, PIPE_BUF, fp)) != NULL)
    {
        len = strlen(*output) + strlen(buf);
        if ((*output = realloc(*output, (sizeof(char) * (len + 1)))) == NULL)
            return len;
        strcat(*output, buf);
    }
    
    pclose(fp);
    return len;
}*/

static void sig_chld_handler(int sig)
{
  pid_t pid;
  if(sig == SIGCHLD)
  {
    while (1)
    {
      pid = wait(NULL);
      if(pid < 0 && errno == ECHILD)
        break;
      
      //printf("A child dead, current id: %d\n", pid);
    }
  }
}

/* xspipe_cmd means slient pipe cmd. So all err info will be ignored in
 * returnd output string. system functions always fork and ignored SIG_CHILD
 * But we may need to do a alarm to termate child  without blocking!
 * Note: if return is not 0 then need free(*output)
 */
size_t xspipe_cmd(char *cmd, char **output)
{
  int handle[2] = {0, 0};
  size_t len = 0;  
  pid_t pid = 0;
  
  if(pipe(handle) == -1)
    return 0;

  pid = fork();
  if(pid == 0)
  {
    close(handle[0]);

    /* redirect stdout and stderr to pipe */
    dup2(handle[1], STDOUT_FILENO);
    dup2(handle[1], STDERR_FILENO);
    close(handle[1]);

    /* close stdin */
    close(STDIN_FILENO);
    //close(1);
    //close(2);

    /* seems no use */
    /*signal(SIGALRM, alarm_exit);
    alarm(XCMD_WAIT_TIME);*/
    execl("/bin/sh", "sh", "-c", cmd, NULL);
    _exit(0);
  }
  else if(pid > 0)
  {  
    /* Preventing zombie */
    int tmp_len = 0;
    //signal(SIGCHLD, SIG_IGN);
    signal(SIGCHLD, sig_chld_handler);
    close(handle[1]);
    
    char buf[PIPE_BUF];
    *output = malloc(1);
    strcpy(*output, "");
    
    while((tmp_len = read(handle[0], buf, PIPE_BUF)) > 0)
    {
      len += tmp_len;
      if((*output = realloc(*output, (len + 1))) == NULL)
      {
        xerror("*output:%s|\n", *output);
        close(handle[0]);
         return len;
      }
      strncpy(*output + len - tmp_len, buf, tmp_len);
    }
    if(len == 0)
      free(*output);
    else
    {
      if((*output)[len - 1] == '\r' || (*output)[len - 1] == '\n')
        (*output)[len - 1] = '\0';
      else
        (*output)[len] = '\0';
    }

    close(handle[0]);
  }

  return len;
}

/* don't call it run demo, if so must give '&' at the cmd tail */
int xcmd_agent(const char *format, ...)
{
  FILE *fp = NULL;
  char buf[1024] = "";
  
  va_list arg;
  va_start(arg, format);
  vsnprintf(buf, 1024, format, arg);
  va_end(arg);
  
#define CMDAGENT_PIPE_FILE "/tmp/cmd_agent"  
  fp = fopen(CMDAGENT_PIPE_FILE, "w");
  if(fp == NULL)
      return -1;
    
  fwrite(buf, 1024, 1, fp);
  fclose(fp);
  usleep(1000);
  
  return 0;
}

size_t xscomb_cmd(char **output, const char *fmt, ...)
{
  char *cmd = NULL;
  size_t len = 0;
  va_list arg;
  
  va_start(arg, fmt);
  if(vasprintf(&cmd, fmt, arg) < 0)
  {
    xprintf("Fmt error or memory exhausted!\n");
    va_end(arg);
    goto out;
  }
  va_end(arg);
  
  if(strlen(cmd) == 0)
  {
    xprintf("cmd is empty!\n");
    goto out;
  }
  //xprintf("cmd:%s\n", cmd);
  len = xspipe_cmd(cmd, output);

out:
  if(cmd)
    free(cmd);
  
  return len;
}

/* don't dump results */
size_t xsystem(const char *fmt, ...)
{
  char *cmd = NULL;
  char real_cmd[1024] = "";
  
  size_t len = 0;
  va_list arg;
  
  va_start(arg, fmt);
  if(vasprintf(&cmd, fmt, arg) < 0)
  {
    xprintf("Fmt error or memory exhausted!\n");
    va_end(arg);
    goto out;
  }
  va_end(arg);

  len = strlen(cmd);
  if(len == 0 || len >= 1024 - strlen(REDIRECT_TO_NULL))
  {
    xprintf("cmd is empty!\n");
    goto out;
  }

  sprintf(real_cmd, "%s%s", cmd, REDIRECT_TO_NULL);
  
  //xprintf("cmd:%s\n", real_cmd);
  len = system(real_cmd);

out:
  if(cmd)
    free(cmd);
  
  return len;
}

/* Daemonize myself. */
int xdaemon(int changedir, int doclose)
{
  pid_t pid;

  pid = fork ();

  /* In case of fork is error. */
  if(pid < 0)
  {
    perror("fork");
    exit(-1);
  }

  /* In case of this is parent process. */
  if(pid != 0)
    exit(0);

  /* Become session leader and get pid. */
  pid = setsid();
  if (pid < -1)
  {
    perror ("setsid");
    return -1;
  }

  /* Change directory to root. */
  if(changedir)
    chdir ("/");

  /* File descriptor close. */
  if(doclose)
  {
    int fd;

    fd = open ("/dev/null", O_RDWR, 0);
    if(fd != -1)
    {
      dup2(fd, STDIN_FILENO);
      dup2(fd, STDOUT_FILENO);
      dup2(fd, STDERR_FILENO);
      if(fd > 2)
        close (fd);
    }
  }

  umask (0027);
  return 0;
}

/* call hook function */
size_t xcmd_func(char **output, void(*func)(void *data), void *data)
{
  FILE *fp = NULL;
  
  size_t len = 0;
  
  char name[FILENAMSIZ] = "";
  char stdout_name[FILENAMSIZ] = "";
  char stderr_name[FILENAMSIZ] = "";

  assert(output);
  fp = xtmpfile_create("a+");
  if(fp == NULL)
  {
    perror("xtmpfile_create");
    return -1;
  }

  xfile_name(name, fileno(fp));
  xfile_name(stdout_name, fileno(stdout));
  xfile_name(stderr_name, fileno(stderr));

  freopen(name, "a+", stdout);
  freopen(name, "a+", stderr);

  /* flush all data */
  fflush(NULL);

  func(data);

  freopen(stdout_name, "a+", stdout);
  freopen(stderr_name, "a+", stderr);

  *output = xfile_read(fileno(fp), &len);
  xtmpfile_remove(fp);

  return len;
}

#ifdef TEST
void test_func()
{
  printf("hello world!\n");
  fprintf(stdout,"stdout, hello world!\n");
  fprintf(stderr,"stderr, hello world!\n");
}

void test_xexccmd()
{
  int len = 0;
  char *result = NULL;

  len = xcmd_func(&result, (void *)test_func, NULL);
  if(len > 0)
  {
    printf("len:%d, result:%s", len, result);
    free(result);
  }
  
}
#endif  
