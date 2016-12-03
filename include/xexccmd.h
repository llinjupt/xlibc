#ifndef XEXCCMD_H
#define XEXCCMD_H

/* pipe command and 's' means silent running */
size_t xpipe_cmd(char *cmd, char **output);
size_t xspipe_cmd(char *cmd, char **output);
size_t xcmd_func(char **output, void(*func)(void *data), void *data);

size_t xscomb_cmd(char **output, const char *fmt, ...);
size_t xsystem(const char *fmt, ...);/* don't dump results */
int xcmd_agent(const char *format, ...);

int xdaemon(int changedir, int doclose);

#endif /* XEXCCMD_H */
