#ifndef XCRC32_H
#define XCRC32_H

int crc32_file(FILE *file, unsigned long *crc32);
unsigned long crc32_buf( unsigned long incrc32, const void *buf, int buf_len);

unsigned char crc8_buf(void *buf, unsigned bytes);
#endif /* XCRC32_H */

