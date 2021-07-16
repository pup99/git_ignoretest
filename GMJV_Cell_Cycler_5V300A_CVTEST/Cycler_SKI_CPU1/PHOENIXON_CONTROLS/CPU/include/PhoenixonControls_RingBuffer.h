/*
 * PhoenixonControls_RingBuffer.h
 *
 *  Created on: 2018. 6. 2.
 *      Author: BGKim
 */

#ifndef PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_RINGBUFFER_H_
#define PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_RINGBUFFER_H_

#define RINGBUFFER						0x40		//'@'

typedef struct
{
	Uint16 Head;
	Uint16 Tail;
	Uint16 Count;
}S_RINGBUFFER_FILE;

void RINGBUFFER_Clear(S_RINGBUFFER_FILE *s_File);
Uint32 RINGBUFFER_GetSize(S_RINGBUFFER_FILE *s_File, Uint32 ui_MAX_BufferLength);
Uint32 RINGBUFFER_Put(S_RINGBUFFER_FILE *s_File, Uint32 ui_MAX_BufferLength);
Uint32 RINGBUFFER_Get(S_RINGBUFFER_FILE *s_File, Uint32 ui_MAX_BufferLength);


#endif /* PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_RINGBUFFER_H_ */
