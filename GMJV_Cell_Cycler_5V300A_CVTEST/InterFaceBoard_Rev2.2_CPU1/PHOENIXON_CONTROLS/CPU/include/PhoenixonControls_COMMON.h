/*
 * PhoenixonControls_COMMON.h
 *
 *  Created on: 2018. 5. 20.
 *      Author: BGKim
 */

#ifndef PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_COMMON_H_
#define PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_COMMON_H_


#define SET 1
#define CLEAR 0

#define ADDR32(A)			(*((volatile ulong *)(A)))
#define ADDR16(A)			(*((volatile ushort *)(A)))
#define ADDR8(A)			(*((volatile uchar *)(A)))

// Byte swapping.
#define SWAP8(A)		(A)
#define SWAP16(A)		((((A << 8 ) & 0xFF00)) | ((A >> 8)& 0x00FF))
#define SWAP32(A)		((((A<<24)&0xFF000000)) | (((A<<8)&0x00FF0000)) | (((A>>8)&0x0000FF00)) | (((A>>24)&0x000000FF)))

#define NEWLINE					"\n"
#endif /* PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_COMMON_H_ */
