/**
 * support MD5 for PPPoE CHAP mode
 */

#ifndef PHOENIXON_W5300_MD5_H
#define PHOENIXON_W5300_MD5_H

#include "PhoenixonControls_W5300_types.h"

/* MD5 context. */
typedef struct {
        uint32 state[4];    /* state (ABCD)                            */
        uint32 count[2];    /* number of bits, modulo 2^64 (lsb first) */
        uint8  buffer[64];  /* input buffer                            */
      } md5_ctx;

extern void md5_init(md5_ctx *context);
extern void md5_update(md5_ctx *context, uint8 *buffer, uint32 length);
extern void md5_final(uint8 result[16], md5_ctx *context);

#endif	// __md5_H
