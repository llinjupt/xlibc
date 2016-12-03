#ifndef IPC_UDP_H
#define IPC_UDP_H

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct ipcmsg_hdr
{
  /* guest pid */
  //pid_t pid;

  /* msg length */
  unsigned int len;
}ipcmsg_hdr_t;

typedef struct ipcmsg_tcp_hdr
{
  int magic;
  unsigned int len;
}ipcmsg_tcp_hdr_t;

/* run on server side */
int ipcmsg_server(const char *ip, int port);

/* run on client side */
int ipcmsg_connect(const char *ip, int port);
int ipcmsg_close(int fd);

int ipcmsg_write(int fd, void *data, unsigned int len);
int ipcmsg_send(int fd, struct sockaddr_in *caddr, 
                   void *data, unsigned int len);

/* run on server side and data if not NULL must be free*/
int ipcmsg_recv(int fd, struct sockaddr_in *caddr, void **data);

int ipcmsg_tcp_server(const char *ip, int port);
int ipcmsg_tcp_connect(const char *ip, int port, int msec);
int ipcmsg_tcp_send(int fd, const void *buffer, unsigned int len);
int ipcmsg_tcp_recv(int fd, void *buffer, unsigned int bufflen);

#endif /* IPC_UDP_H */
