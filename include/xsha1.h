/* -*- buffer-read-only: t -*- vi: set ro: */
/* DO NOT EDIT! GENERATED AUTOMATICALLY! */
/* Declarations of functions and data types used for SHA1 sum
   library functions.
   Copyright (C) 2000, 2001, 2003, 2005, 2006, 2008, 2009, 2010 Free Software
   Foundation, Inc.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 3, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifndef XSHA1_H
#define XSHA1_H

#include <stdio.h>
#include <stdint.h>

#ifndef SHA1_DIGEST_SIZE
#define SHA1_DIGEST_SIZE 20
#endif

#define SHA1_MAC_LEN (SHA1_DIGEST_SIZE)

struct SHA1Context {
  uint32_t state[5];
  uint32_t count[2];
  unsigned char buffer[64];
};

typedef struct SHA1Context SHA1_CTX;

void SHA1Init(SHA1_CTX *context);
void SHA1Update(SHA1_CTX *context, const void *data, uint32_t len);
void SHA1Final(unsigned char digest[20], SHA1_CTX *context);

struct HMACContext {
  SHA1_CTX context;
  uint8_t key[64];
  size_t key_len;
};
typedef struct HMACContext HMAC_CTX;

void HMACInit(HMAC_CTX *context, const uint8_t *key, size_t key_len);
void HMACUpdate(HMAC_CTX *context, const void *data, uint32_t len);
void HMACFinal(unsigned char digest[20], HMAC_CTX *context);

void sha1_vector(size_t num_elem, const uint8_t *addr[], const size_t *len,
    uint8_t *mac);
void hmac_sha1_vector(const uint8_t *key, size_t key_len, size_t num_elem,
    const uint8_t *addr[], const size_t *len, uint8_t mac[SHA1_MAC_LEN]);
void hmac_sha1(const uint8_t *key, size_t key_len,
    const uint8_t *data, size_t data_len, uint8_t mac[SHA1_MAC_LEN]);
void sha1_prf(const uint8_t *key, size_t key_len, const char *label,
    const uint8_t *data, size_t data_len, uint8_t *buf, size_t buf_len);

void *xsha1file(const char *file, unsigned char digest[SHA1_DIGEST_SIZE]);
void *xsha1buf(const char *buffer, size_t len, unsigned char digest[SHA1_DIGEST_SIZE]);

#endif /* XSHA1_H */
