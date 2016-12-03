#ifndef XSYSTEM_H
#define XSYSTEM_H

#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <netdb.h>
#include <ctype.h>

typedef enum
{
  ENDIAN_UNKNOWN = -1,
  ENDIAN_LITTLE,
  ENDIAN_BIG,
  ENDIAN_NET = ENDIAN_BIG
}endian_t;

endian_t xendian();

inline uint16_t xswap16(uint16_t x);
inline uint32_t xswap32(uint32_t x);
inline uint64_t xswap64(uint64_t x);

void xkill_bypid(int *pids, int size);
int xgetpid_byname(const char *procname, int *findpids, int size);
int xgetexe_bypid(int pid, char *buf, int size);
int xgetcmdline_bypid(int pid, char *buf, int size);

#endif /* XSYSTEM_H */
