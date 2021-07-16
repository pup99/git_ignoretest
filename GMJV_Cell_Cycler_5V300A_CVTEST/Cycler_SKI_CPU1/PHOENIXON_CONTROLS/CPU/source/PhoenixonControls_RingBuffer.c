/*
 * PhoenixonControls_RingBuffer.c
 *
 *  Created on: 2018. 6. 2.
 *      Author: BGKim
 */

#include "../../../common/include/F28x_Project.h"
#include "../../../common/include/driverlib.h"
#include "../../../common/include/device.h"

//#pragma CODE_SECTION(, ".TI.ramfunc");
//#pragma CODE_SECTION(, ".TI.ramfunc");
//#pragma CODE_SECTION(, ".TI.ramfunc");

void RINGBUFFER_Clear(S_RINGBUFFER_FILE *s_File)
{
	(*s_File).Head = CLEAR;
	(*s_File).Tail = CLEAR;
}

Uint32 RINGBUFFER_GetSize(S_RINGBUFFER_FILE *s_File, Uint32 ui_MAX_BufferLength)
{
	return ((*s_File).Head - (*s_File).Tail + ui_MAX_BufferLength) % ui_MAX_BufferLength;
}

Uint32 RINGBUFFER_Put(S_RINGBUFFER_FILE *s_File, Uint32 ui_MAX_BufferLength)
{
	Uint32 ul_Temp = (*s_File).Head;
	if(RINGBUFFER_GetSize(s_File, ui_MAX_BufferLength) == ui_MAX_BufferLength - 1) return (ul_Temp);//SUCCESS Or FAIL

	(*s_File).Head++;
	(*s_File).Head %= ui_MAX_BufferLength;

	return (ul_Temp);//SUCCESS Or FAIL
}

Uint32 RINGBUFFER_Get(S_RINGBUFFER_FILE *s_File, Uint32 ui_MAX_BufferLength)
{
	Uint32 ul_Temp = (*s_File).Tail;
	if(RINGBUFFER_GetSize(s_File, ui_MAX_BufferLength) == 0) return (ul_Temp);//SUCCESS Or FAIL
	(*s_File).Tail++;
	(*s_File).Tail %= ui_MAX_BufferLength;

	return (ul_Temp);//SUCCESS Or FAIL
}
//
// End of file
//
