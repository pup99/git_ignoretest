/*
 * PhoenixonControls_COMMUNICATION.c
 *
 *  Created on: 2018. 5. 20.
 *      Author: BGKim
 */
#include "../../../common/include/F28x_Project.h"
#include "../../../common/include/driverlib.h"
#include "../../../common/include/device.h"
#include "../include/PhoenixonControls_FPU.h"

//
//INTERFACE_CPU1
//
#ifdef INTERFACE_CPU1

extern volatile S_COMMUNICATION_GSRAM_CPU1 s_CPU_1_GSRAM[];
extern volatile S_COMMUNICATION_GSRAM_CPU2 s_CPU_2_GSRAM[];
volatile S_COMMUNICATION_DRAM *s_SDRAMp[COMMUNICATION_CHANNEL];
extern volatile Uint16 ui_UpdateflagCPU1;
extern volatile Uint16 ui_UpdateflagCPU2;

#pragma CODE_SECTION(COMMUNICATION_Ethernet_STEPRead, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_Ethernet_JUMPRead, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_Ethernet_COMMANDRead, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_Ethernet_STARTRead, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_Ethernet_PAUSERead, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_Ethernet_DATAWrite, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_Ethernet_ENDWrite, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_Ethernet_ALARMWrite, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_DRAM_STEPWrite, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_DRAM_DATARead, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_Write, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_Read, ".TI.ramfunc");
//#pragma CODE_SECTION(, ".TI.ramfunc");
__attribute__((far)) volatile S_COMMUNICATION_DRAM s_SDRAM[COMMUNICATION_CHANNEL];

void COMMUNICATION_INIT(void)
{
	Uint16 ui_i = 0;
	Uint32 ul_Temp = 0;

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{
		ul_Temp = (Uint32)&(s_SDRAM[ui_i]);
		ul_Temp |= EMIF_CS0_BASE_ADDRESS;
		s_SDRAMp[ui_i] = (S_COMMUNICATION_DRAM *)(ul_Temp); //외부 램을 변수로 잡으면 메모리 엑세스가
	}							  //안되는 현상이 있어 확인 해본 결과 주소 0x80000000가
}								  //현상이 있어 강제로 값을 넣어 주어 사용함.

Uint16 COMMUNICATION_Ethernet_STEPRead(volatile S_COMMUNICATION_DRAM **sp_IF, Uint16 *buf, Uint32 len)
{
	Uint16 ui_i = 0;
	volatile static Uint16 ui_Channel = 0;
	volatile static Uint16 ui_StartFlag = 0;
	volatile static Uint32 ul_DataCount = 0;
	volatile static Uint32 ul_SumCount	= 0;
	volatile static Uint32 PointerCount[COMMUNICATION_CHANNEL];

//	memcpy_fast_far(sp_IF[0], buf, len / 2);//sp_IF 더블 포인터 입니다.
// 메모리 테스트 펑션입니다. 인자 전달받아서 주는 더블 포인터 입니다.
//	memcpy_fast_far(&((*sp_IF[0]).STEP) + PointerCount[ui_i], buf, len / 2);

//	return 0; //메모리 카피 구조상 문제는 없었음.

	if(!ui_StartFlag)
	{	ui_StartFlag = HEADER_DATA;

		ul_SumCount  = ((buf[1] * COMMUNICATION_INTEGER_LENGTH_RX) + COMMUNICATION_INTEGER_LENGTH_HEADER) * 2; //수신 DATA 총 갯수 정보 복사
		ui_Channel 	 = buf[2]; //장비 정보 복사
		//상태 표시 변수 초기화
		ul_DataCount = CLEAR;
		for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)PointerCount[ui_i] = CLEAR;
	}
	ul_DataCount += len; //8BIT 단위의 Ethernet에서 수신된 BigEndian 자료수신 갯수

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4개의 채널에서 해당 되는 Channel을 확인하고 해당 SETP DATA에 자료 저장

		if(ui_Channel & (0x1 << ui_i))
		{
			memcpy_fast_far(&((*sp_IF)[ui_i].STEP.Header) + PointerCount[ui_i], buf, len / 2);
			PointerCount[ui_i] += (len / 2);
//			(*sp_IF)[ui_i].ReadFlagStep = SET;
		}
	}

	if(ul_DataCount >= ul_SumCount)//Data 총 갯수와 실제로 Counting한 변수를 비교후 같거나
	{							   //많으면(수신 에러)작업 종료하고 해당 Flag 초기화 함.
		//상태 표시 변수 초기화
		ui_StartFlag = CLEAR;

		ul_SumCount  = CLEAR;
		ui_Channel   = CLEAR;

		ul_DataCount = CLEAR;
		for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)PointerCount[ui_i] = CLEAR;
	}

	return(ui_StartFlag); //SUCCESS Or FAIL
						  //리턴값으로 수신 및 복사 완료로 0을 처리중일때는 HEADER_DATA을 갖음.
}

Uint16 COMMUNICATION_Ethernet_JUMPRead(volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1,
		volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2, Uint16 *buf, Uint32 len)
{
	Uint16 ui_i = 0;
	volatile static Uint16 ui_Channel = 0;
	volatile static Uint16 ui_StartFlag = 0;
	volatile static Uint32 ul_DataCount = 0;
	volatile static Uint32 ul_SumCount	= 0;
	volatile static Uint32 PointerCount[COMMUNICATION_CHANNEL];

//	memcpy_fast_far(sp_IF[0], buf, len / 2);//sp_IF 더블 포인터 입니다.
// 메모리 테스트 펑션입니다. 인자 전달받아서 주는 더블 포인터 입니다.
//	memcpy_fast_far(&((*sp_IF[0]).STEP) + PointerCount[ui_i], buf, len / 2);

	if(!ui_StartFlag)
	{	ui_StartFlag = HEADER_JUMP;

		ul_SumCount  = buf[1]; //수신 DATA 총 갯수 정보 복사
		ui_Channel 	 = buf[2]; //장비 정보 복사
		//상태 표시 변수 초기화
		ul_DataCount = CLEAR;
		for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)PointerCount[ui_i] = CLEAR;
	}
	ul_DataCount += len; //8BIT 단위의 Ethernet에서 수신된 BigEndian 자료수신 갯수

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4개의 채널에서 해당 되는 Channel을 확인하고 해당 SETP DATA에 자료 저장
		if(ui_Channel & (0x1 << ui_i))// && (sp_GSRAM_CPU1[ui_i]).CPU1_Jump == (sp_GSRAM_CPU2[ui_i]).CPU2_Jump)
		{
			memcpy_fast_far(&(sp_GSRAM_CPU1[ui_i]).CPU1_JUMP.Header + PointerCount[ui_i], buf, len / 2);
			PointerCount[ui_i] += (len / 2);
			(sp_GSRAM_CPU1[ui_i]).CPU1_Jump++;// = ~(sp_GSRAM_CPU2[ui_i]).CPU2_Jump;
		}
	}

	if(ul_DataCount >= ul_SumCount)//Data 총 갯수와 실제로 Counting한 변수를 비교후 같거나
	{							   //많으면(수신 에러)작업 종료하고 해당 Flag 초기화 함.
		//상태 표시 변수 초기화
		ui_StartFlag = CLEAR;

		ul_SumCount  = CLEAR;
		ui_Channel   = CLEAR;

		ul_DataCount = CLEAR;
		for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)PointerCount[ui_i] = CLEAR;
	}

	return(ui_StartFlag); //SUCCESS Or FAIL
						  //리턴값으로 수신 및 복사 완료로 0을 처리중일때는 HEADER_DATA을 갖음.
}

Uint16 COMMUNICATION_Ethernet_COMMANDRead(volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1,
		volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2, Uint16 *buf, Uint32 len)
{
	Uint16 ui_i = 0;
	volatile static Uint16 ui_Channel = 0;
	volatile static Uint16 ui_StartFlag = 0;
	volatile static Uint32 ul_DataCount = 0;
	volatile static Uint32 ul_SumCount	= 0;
	volatile static Uint32 PointerCount[COMMUNICATION_CHANNEL];

//	memcpy_fast_far(sp_IF[0], buf, len / 2);//sp_IF 더블 포인터 입니다.
// 메모리 테스트 펑션입니다. 인자 전달받아서 주는 더블 포인터 입니다.
//	memcpy_fast_far(&((*sp_IF[0]).STEP) + PointerCount[ui_i], buf, len / 2);

	if(!ui_StartFlag)
	{	ui_StartFlag = HEADER_COMMAND;

		ul_SumCount  = buf[1]; //수신 DATA 총 갯수 정보 복사
		ui_Channel 	 = buf[2]; //장비 정보 복사
		//상태 표시 변수 초기화
		ul_DataCount = CLEAR;
		for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)PointerCount[ui_i] = CLEAR;
	}
	ul_DataCount += len; //8BIT 단위의 Ethernet에서 수신된 BigEndian 자료수신 갯수

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4개의 채널에서 해당 되는 Channel을 확인하고 해당 SETP DATA에 자료 저장
		if(ui_Channel & (0x1 << ui_i))// && (sp_GSRAM_CPU1[ui_i]).CPU1_Command == (sp_GSRAM_CPU2[ui_i]).CPU2_Command)
		{
			memcpy_fast_far(&(sp_GSRAM_CPU1[ui_i]).CPU1_COMMAND.Header + PointerCount[ui_i], buf, len / 2);
			PointerCount[ui_i] += (len / 2);
			(sp_GSRAM_CPU1[ui_i]).CPU1_Command++;// = ~(sp_GSRAM_CPU2[ui_i]).CPU2_Command;
		}
	}

	if(ul_DataCount >= ul_SumCount)//Data 총 갯수와 실제로 Counting한 변수를 비교후 같거나
	{							   //많으면(수신 에러)작업 종료하고 해당 Flag 초기화 함.
		//상태 표시 변수 초기화
		ui_StartFlag = CLEAR;

		ul_SumCount  = CLEAR;
		ui_Channel   = CLEAR;

		ul_DataCount = CLEAR;
		for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)PointerCount[ui_i] = CLEAR;
	}

	return(ui_StartFlag); //SUCCESS Or FAIL
						  //리턴값으로 수신 및 복사 완료로 0을 처리중일때는 HEADER_DATA을 갖음.
}

Uint16 COMMUNICATION_Ethernet_STARTRead(volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1,
		volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2, Uint16 *buf, Uint32 len)
{
	Uint16 ui_i = 0;
	volatile static Uint16 ui_Channel = 0;
	volatile static Uint16 ui_StartFlag = 0;
	volatile static Uint32 ul_DataCount = 0;
	volatile static Uint32 ul_SumCount	= 0;
	volatile static Uint32 PointerCount[COMMUNICATION_CHANNEL];

//	memcpy_fast_far(sp_IF[0], buf, len / 2);//sp_IF 더블 포인터 입니다.
// 메모리 테스트 펑션입니다. 인자 전달받아서 주는 더블 포인터 입니다.
//	memcpy_fast_far(&((*sp_IF[0]).STEP) + PointerCount[ui_i], buf, len / 2);

	if(!ui_StartFlag)
	{	ui_StartFlag = HEADER_START;

		ul_SumCount  = buf[1]; //수신 DATA 총 갯수 정보 복사
		ui_Channel 	 = buf[2]; //장비 정보 복사
		//상태 표시 변수 초기화
		ul_DataCount = CLEAR;
		for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)PointerCount[ui_i] = CLEAR;
	}
	ul_DataCount += len; //8BIT 단위의 Ethernet에서 수신된 BigEndian 자료수신 갯수

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4개의 채널에서 해당 되는 Channel을 확인하고 해당 SETP DATA에 자료 저장
		if(ui_Channel & (0x1 << ui_i))// && (sp_GSRAM_CPU1[ui_i]).CPU1_Start == (sp_GSRAM_CPU2[ui_i]).CPU2_Start)
		{
			memcpy_fast_far(&(sp_GSRAM_CPU1[ui_i]).CPU1_START.Header + PointerCount[ui_i], buf, len / 2);
			PointerCount[ui_i] += (len / 2);
			(sp_GSRAM_CPU1[ui_i]).CPU1_Start++;// = ~(sp_GSRAM_CPU2[ui_i]).CPU2_Start;
		}
	}

	if(ul_DataCount >= ul_SumCount)//Data 총 갯수와 실제로 Counting한 변수를 비교후 같거나
	{							   //많으면(수신 에러)작업 종료하고 해당 Flag 초기화 함.
		//상태 표시 변수 초기화
		ui_StartFlag = CLEAR;

		ul_SumCount  = CLEAR;
		ui_Channel   = CLEAR;

		ul_DataCount = CLEAR;
		for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)PointerCount[ui_i] = CLEAR;
	}

	return(ui_StartFlag); //SUCCESS Or FAIL
						  //리턴값으로 수신 및 복사 완료로 0을 처리중일때는 HEADER_DATA을 갖음.
}

Uint16 COMMUNICATION_Ethernet_PAUSERead(volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1,
		volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2, Uint16 *buf, Uint32 len)
{
	Uint16 ui_i = 0;
	volatile static Uint16 ui_Channel = 0;
	volatile static Uint16 ui_StartFlag = 0;
	volatile static Uint32 ul_DataCount = 0;
	volatile static Uint32 ul_SumCount	= 0;
	volatile static Uint32 PointerCount[COMMUNICATION_CHANNEL];

	if(!ui_StartFlag)
	{	ui_StartFlag = HEADER_PAUSE;

		ul_SumCount  = buf[1]; //수신 DATA 총 갯수 정보 복사
		ui_Channel 	 = buf[2]; //장비 정보 복사
		//상태 표시 변수 초기화
		ul_DataCount = CLEAR;
		for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)PointerCount[ui_i] = CLEAR;
	}
	ul_DataCount += len; //8BIT 단위의 Ethernet에서 수신된 BigEndian 자료수신 갯수

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4개의 채널에서 해당 되는 Channel을 확인하고 해당 SETP DATA에 자료 저장
		if(ui_Channel & (0x1 << ui_i))// && (sp_GSRAM_CPU1[ui_i]).CPU1_Pause == (sp_GSRAM_CPU2[ui_i]).CPU2_Pause)
		{
			memcpy_fast_far(&(sp_GSRAM_CPU1[ui_i]).CPU1_PAUSE.Header + PointerCount[ui_i], buf, len / 2);
			PointerCount[ui_i] += (len / 2);
			(sp_GSRAM_CPU1[ui_i]).CPU1_Pause++;// = ~(sp_GSRAM_CPU2[ui_i]).CPU2_Pause;
		}
	}

	if(ul_DataCount >= ul_SumCount)//Data 총 갯수와 실제로 Counting한 변수를 비교후 같거나
	{							   //많으면(수신 에러)작업 종료하고 해당 Flag 초기화 함.
		//상태 표시 변수 초기화
		ui_StartFlag = CLEAR;

		ul_SumCount  = CLEAR;
		ui_Channel   = CLEAR;

		ul_DataCount = CLEAR;
		for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)PointerCount[ui_i] = CLEAR;
	}

	return(ui_StartFlag); //SUCCESS Or FAIL
						  //리턴값으로 수신 및 복사 완료로 0을 처리중일때는 HEADER_DATA을 갖음.
}

Uint32 COMMUNICATION_Ethernet_DATAWrite(volatile S_COMMUNICATION_DRAM **sp_IF, Uint16 *buf)
{
//	Uint16 ui_Channel = 0;									//링버퍼 사용해서 항상 보내야 함.
	Uint16 ui_i = 0;										//카운터는 얼마나 할찌 고민해봐야 할 것 같다.
	Uint32 BufferCount = 0;									//인터럽트에 넣어서 사용 해야 할 것 같다.
	Uint32 Index = 0;
	Uint32 Count  = 0;

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4개의 채널에서 해당 되는 Channel을 확인하고 해당 SETP DATA에 자료 저장
		Count = RINGBUFFER_GetSize((S_RINGBUFFER_FILE *)&((*sp_IF)[ui_i].RING_DATA), COMMUNICATION_TX_DATA_LENGTH);//데이타 한프레임 카피
		if(Count)// && (*sp_IF)[ui_i].WriteFlagData == SET)//데이타가 있는지 확인
		{
			Index = RINGBUFFER_Get((S_RINGBUFFER_FILE *)&((*sp_IF)[ui_i].RING_DATA), COMMUNICATION_TX_DATA_LENGTH);    //데이타 한프레임 카피
			(*sp_IF)[ui_i].DATA[Index].Header = HEADER_DATA;
			(*sp_IF)[ui_i].DATA[Index].DataCount = (COMMUNICATION_INTEGER_LENGTH_DATA * 2);
			(*sp_IF)[ui_i].DATA[Index].ChannelIndex = 1 << ui_i;

			memcpy_fast_far(buf + BufferCount, &((*sp_IF)[ui_i].DATA[Index].Header), (COMMUNICATION_INTEGER_LENGTH_DATA));
			BufferCount += COMMUNICATION_INTEGER_LENGTH_DATA;
			//(*sp_IF)[ui_i].WriteFlagData = CLEAR; // 이터넷 송신 플레그
		}
	}
	return(BufferCount * 2); //SUCCESS Or FAIL
}

Uint32 COMMUNICATION_Ethernet_ENDWrite(volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1,
		volatile  S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2, Uint16 *buf)
{
	Uint16 ui_Channel = 0;									//링버퍼 사용해서 항상 보내야 함.
	Uint16 ui_i = 0;										//카운터는 얼마나 할찌 고민해봐야 할 것 같다.
	Uint32 BufferCount = 0;									//인터럽트에 넣어서 사용 해야 할 것 같다.

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{
		ui_Channel |= (sp_GSRAM_CPU2[ui_i]).CPU2_END.ChannelIndex;//플러그별 데이터 취합한다. ******
	}

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4개의 채널에서 해당 되는 Channel을 확인하고 해당 SETP DATA에 자료 저장
		if(ui_Channel & (0x1 << ui_i) && (sp_GSRAM_CPU1[ui_i]).CPU1_End != (sp_GSRAM_CPU2[ui_i]).CPU2_End)// (if))//데이타가 있는지 확인해야함. 일단 생략
		{
			memcpy_fast_far(buf + BufferCount, &((sp_GSRAM_CPU2[ui_i]).CPU2_END.Header), COMMUNICATION_INTEGER_LENGTH_END);
			BufferCount += COMMUNICATION_INTEGER_LENGTH_END;
			(sp_GSRAM_CPU1[ui_i]).CPU1_End = (sp_GSRAM_CPU2[ui_i]).CPU2_End; // 이터넷 송신 플레그
		}
	}
	return(BufferCount * 2); //SUCCESS Or FAIL 이더넷 (8bit) 전송 수
}

Uint32 COMMUNICATION_Ethernet_ALARMWrite(volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1,
		volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2, Uint16 *buf)
{
	Uint16 ui_Channel = 0;									//링버퍼 사용해서 항상 보내야 함.
	Uint16 ui_i = 0;										//카운터는 얼마나 할찌 고민해봐야 할 것 같다.
	Uint32 BufferCount = 0;									//인터럽트에 넣어서 사용 해야 할 것 같다.

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{
		ui_Channel |= (sp_GSRAM_CPU2[ui_i]).CPU2_ALARM.ChannelIndex;//플러그별 데이터 취합한다. ******
	}

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4개의 채널에서 해당 되는 Channel을 확인하고 해당 SETP DATA에 자료 저장
		if(ui_Channel & (0x1 << ui_i) && (sp_GSRAM_CPU1[ui_i]).CPU1_Alarm != (sp_GSRAM_CPU2[ui_i]).CPU2_Alarm)// (if))//데이타가 있는지 확인해야함. 일단 생략
		{
			memcpy_fast_far(buf + BufferCount, &((sp_GSRAM_CPU2[ui_i]).CPU2_ALARM.Header), COMMUNICATION_INTEGER_LENGTH_ALARM);
			BufferCount += COMMUNICATION_INTEGER_LENGTH_ALARM;
			(sp_GSRAM_CPU1[ui_i]).CPU1_Alarm = (sp_GSRAM_CPU2[ui_i]).CPU2_Alarm;
		}
	}
	return(BufferCount * 2); //SUCCESS Or FAIL
}

void COMMUNICATION_DRAM_STEPWrite(volatile S_COMMUNICATION_DRAM **sp_IF,
		volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1, volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2)
{
	//CPU2에서 index관련 하여 플레그를 주면 해당 되는 데이타를 넘겨야한다.
	//Start 신호가 오면 그때 부터 데이타를 넘긴다.
	//End 신호가 오면 그때 부터 데이타를 넘기지 않는다.

	Uint16 ui_Channel = 0;
	Uint16 ui_i = 0;
	Uint16 ui_Index = 0;

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{
		ui_Channel |= (*sp_IF)[ui_i].STEP.ChannelIndex;//플러그별 데이터 취합한다. ******
	}

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4개의 채널에서 해당 되는 Channel을 확인하고 해당 SETP DATA에 자료 저장

		if((sp_GSRAM_CPU2[ui_i]).CPU2_StepIndex != (sp_GSRAM_CPU1[ui_i]).CPU1_StepIndex)//점프 커맨드 들어 왔을 경우 실행 할수 있게 조건 넝!!!!!
		if(ui_Channel & (0x1 << ui_i))// && (sp_GSRAM_CPU1[ui_i]).CPU1_Step == (sp_GSRAM_CPU2[ui_i]).CPU2_Step)// (if))//데이타가 있는지 확인해야함. 일단 생략
		{
			(sp_GSRAM_CPU1[ui_i]).CPU1_StepHeader = (*sp_IF)[ui_i].STEP.Header;
			(sp_GSRAM_CPU1[ui_i]).CPU1_StepDataCount = (*sp_IF)[ui_i].STEP.DataCount;
			(sp_GSRAM_CPU1[ui_i]).CPU1_StepChannelIndex = (*sp_IF)[ui_i].STEP.ChannelIndex;

			ui_Index = (sp_GSRAM_CPU2[ui_i]).CPU2_StepIndex;// 불러올 스텝 주소
			//First Step Level
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[0 ] = (*sp_IF)[ui_i].STEP.STEP[ui_Index];
			//Second Step Level
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[1]  = (*sp_IF)[ui_i].STEP.STEP[((*sp_IF)[ui_i].STEP.STEP[ui_Index].GOTO_INDEX)];
			//Third Step Level
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[7 ] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[1].GOTO_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[8 ] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[1].RETURN_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[9 ] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[1].END_VOLTAGE_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[10] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[1].END_TIME_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[11] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[1].END_CURRENT_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[12] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[1].END_CAPACITY_INDEX)];

			//Second Step Level
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[2]  = (*sp_IF)[ui_i].STEP.STEP[((*sp_IF)[ui_i].STEP.STEP[ui_Index].RETURN_INDEX)];
			//Third Step Level
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[13] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[2].GOTO_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[14] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[2].RETURN_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[15] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[2].END_VOLTAGE_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[16] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[2].END_TIME_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[17] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[2].END_CURRENT_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[18] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[2].END_CAPACITY_INDEX)];

			//Second Step Level
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[3]  = (*sp_IF)[ui_i].STEP.STEP[((*sp_IF)[ui_i].STEP.STEP[ui_Index].END_VOLTAGE_INDEX)];
			//Third Step Level
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[19] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[3].GOTO_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[20] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[3].RETURN_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[21] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[3].END_VOLTAGE_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[22] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[3].END_TIME_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[23] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[3].END_CURRENT_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[24] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[3].END_CAPACITY_INDEX)];

			//Second Step Level
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[4]  = (*sp_IF)[ui_i].STEP.STEP[((*sp_IF)[ui_i].STEP.STEP[ui_Index].END_TIME_INDEX)];
			//Third Step Level
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[25] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[4].GOTO_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[26] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[4].RETURN_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[27] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[4].END_VOLTAGE_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[28] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[4].END_TIME_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[29] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[4].END_CURRENT_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[30] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[4].END_CAPACITY_INDEX)];

			//Second Step Level
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[5]  = (*sp_IF)[ui_i].STEP.STEP[((*sp_IF)[ui_i].STEP.STEP[ui_Index].END_CURRENT_INDEX)];
			//Third Step Level
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[31] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[5].GOTO_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[32] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[5].RETURN_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[33] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[5].END_VOLTAGE_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[34] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[5].END_TIME_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[35] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[5].END_CURRENT_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[36] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[5].END_CAPACITY_INDEX)];

			//Second Step Level
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[6]  = (*sp_IF)[ui_i].STEP.STEP[((*sp_IF)[ui_i].STEP.STEP[ui_Index].END_CAPACITY_INDEX)];
			//Third Step Level
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[37] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[6].GOTO_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[38] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[6].RETURN_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[39] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[6].END_VOLTAGE_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[40] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[6].END_TIME_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[41] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[6].END_CURRENT_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[42] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[6].END_CAPACITY_INDEX)];

			(sp_GSRAM_CPU1[ui_i]).CPU1_Step++;// = ~(sp_GSRAM_CPU1[ui_i]).CPU1_Step;//업데이트 데이타 한다.
//			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP = (*sp_IF)[ui_i].STEP.STEP[ui_Index];
//			memcpy_fast_far(&((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[0]), &((*sp_IF)[ui_i].STEP.STEP[ui_Index]), COMMUNICATION_INTEGER_LENGTH_STEP);
			(sp_GSRAM_CPU1[ui_i]).CPU1_StepIndex = ui_Index;
		}
	}
}

Uint16 COMMUNICATION_DRAM_DATARead(volatile S_COMMUNICATION_DRAM **sp_IF,
		volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1, volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2)
{
	Uint16 ui_Channel = 0;
	Uint16 ui_i = 0;
	Uint32 ul_Index = 0;


	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{
		ui_Channel |= ((sp_GSRAM_CPU2[ui_i]).CPU2_Data != (sp_GSRAM_CPU1[ui_i]).CPU1_Data) << ui_i;//플러그별 데이터 취합한다. ******
	}

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4개의 채널에서 해당 되는 Channel을 확인하고 해당 SETP DATA에 자료 저장
		if(ui_Channel & (0x1 << ui_i))// (if))//데이타가 있는지 확인해야함. 일단 생략
		{
			ul_Index = RINGBUFFER_Put((S_RINGBUFFER_FILE *)&(*sp_IF)[ui_i].RING_DATA, COMMUNICATION_TX_DATA_LENGTH);
			memcpy_fast_far(&((*sp_IF)[ui_i].DATA[ul_Index]), &((sp_GSRAM_CPU2[ui_i]).CPU2_DATA), COMMUNICATION_INTEGER_LENGTH_DATA);

			(sp_GSRAM_CPU1[ui_i]).CPU1_Data = (sp_GSRAM_CPU2[ui_i]).CPU2_Data;
//			(*sp_IF)[ui_i].WriteFlagData = SET;  // 이더넷 송신 플레그 ON
		}
	}
	return (ui_Channel);
}

void COMMUNICATION_Write(volatile S_COMMUNICATION_DRAM **sp_IF, volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1,
		volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2)
{
    COMMUNICATION_DRAM_STEPWrite(sp_IF, sp_GSRAM_CPU1, sp_GSRAM_CPU2);
}
void COMMUNICATION_Read(volatile S_COMMUNICATION_DRAM **sp_IF, volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1,
		volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2)
{
    COMMUNICATION_DRAM_DATARead(sp_IF, sp_GSRAM_CPU1, sp_GSRAM_CPU2);
}
#endif //End #ifdef INTERFACE_CPU1

//
//INTERFACE_CPU2
//
#ifdef INTERFACE_CPU2

extern volatile S_COMMUNICATION_GSRAM_CPU1 s_CPU_1_GSRAM[];
extern volatile S_COMMUNICATION_GSRAM_CPU2 s_CPU_2_GSRAM[];
extern volatile Uint16 ui_UpdateflagCPU1;
extern volatile Uint16 ui_UpdateflagCPU2;

extern E_DSP_SCIA_FLAG e_SCIA_RxState[COMMUNICATION_CHANNEL];
extern E_DSP_SCIA_FLAG e_SCIA_TxState;

#pragma CODE_SECTION(COMMUNICATION_RS422A_TxPacket, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_RS422A_RxUnPacket, ".TI.ramfunc");
//#pragma CODE_SECTION(, ".TI.ramfunc");
//#pragma CODE_SECTION(, ".TI.ramfunc");
//#pragma CODE_SECTION(, ".TI.ramfunc");
//#pragma CODE_SECTION(, ".TI.ramfunc");

Uint16 COMMUNICATION_RS422A_TxPacket(volatile Uint16 *TxBuff, volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1, volatile Uint16 Channel)
{
	Uint16 StartFlag[DSP_SCIA_HEADER_LENGTH] ={0xAAAA, 0xAAAA, 0xAAAA, 0x0000, 0x0000};
	Uint16 Counter = 0;
	Uint16 CRC16 = 0;
	Uint16 Dumy[DSP_SCIA_TX_DUMY_LENGTH] = {0,};

	for(Counter = 0; Counter < DSP_SCIA_TX_DUMY_LENGTH; Counter++)Dumy[Counter] = DSP_SCIA_TX_DUMY;
	Counter = 0;
	StartFlag[DSP_SCIA_START_FLAG_LENGTH] =
			((Channel << 8) & 0xFF00) | (Channel & 0x00FF);

	StartFlag[DSP_SCIA_START_FLAG_LENGTH + 1] =
			((Channel << 8) & 0xFF00) | (Channel & 0x00FF);


	memcpy_fast_far(TxBuff, StartFlag, DSP_SCIA_HEADER_LENGTH);
	Counter += DSP_SCIA_HEADER_LENGTH;

	memcpy_fast_far(TxBuff + Counter, &(sp_GSRAM_CPU1[Channel]), DSP_SCIA_TX_DATA_LENGTH);
	Counter += DSP_SCIA_TX_DATA_LENGTH;


	memcpy_fast_far(TxBuff + Counter, Dumy, DSP_SCIA_TX_DUMY_LENGTH);
	Counter += DSP_SCIA_TX_DUMY_LENGTH;

	CRC16 = CRC16_Conversion(TxBuff, DSP_SCIA_TX_MAX_DATA_LENGTH - DSP_SCIA_CRC16_LENGTH);

	memcpy_fast_far(TxBuff + Counter, &CRC16, DSP_SCIA_CRC16_LENGTH);
	Counter += DSP_SCIA_CRC16_LENGTH;

	DSP_SCIA_Write(TxBuff, Counter);
	Counter  = 0;
	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_RS422A_RxUnPacket(volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2, volatile Uint16 *RxBuff, volatile Uint16 Channel)
{
	memcpy_fast_far(&(sp_GSRAM_CPU2[Channel]), RxBuff, sizeof(S_COMMUNICATION_GSRAM_CPU2));
	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_WritePass(volatile Uint16 *TxBuff, volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1)
{
	return(SUCCESS); //SUCCESS Or FAIL
}
Uint16 COMMUNICATION_ReadPass(volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2, volatile Uint16 *RxBuff)
{
	return(SUCCESS); //SUCCESS Or FAIL
}

#endif //End #ifdef INTERFACE_CPU2


//
//DEVICE_CPU2
//
#ifdef DEVICE_CPU2

extern volatile S_COMMUNICATION_GSRAM_CPU1 GSRAM_STPE[];
extern volatile S_COMMUNICATION_GSRAM_CPU2 GSRAM_DATA[];
extern volatile Uint32 ui_UpdateflagCPU1;
extern volatile Uint32 ui_UpdateflagCPU2;

extern E_DSP_SCIA_FLAG e_SCIA_RxState;
extern E_DSP_SCIA_FLAG e_SCIA_TxState;



#pragma CODE_SECTION(COMMUNICATION_RS422A_TxPacket, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_RS422A_RxUnPacket, ".TI.ramfunc");
//#pragma CODE_SECTION(, ".TI.ramfunc");

Uint16 COMMUNICATION_RS422A_TxPacket(volatile Uint16 *TxBuff, volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2, volatile Uint16 Channel, volatile Uint16 Bank)
{
	Uint16 StartFlag[DSP_SCIA_HEADER_LENGTH] ={0xAAAA, 0xAAAA, 0xAAAA, 0x0000, 0x0000};
	Uint16 Counter = 0;
	Uint16 CRC16 = 0;
	Uint16 Dumy[DSP_SCIA_TX_DUMY_LENGTH] = {0,};

	for(Counter = 0; Counter < DSP_SCIA_TX_DUMY_LENGTH; Counter++)Dumy[Counter] = DSP_SCIA_TX_DUMY;
	Counter = 0;
	StartFlag[DSP_SCIA_START_FLAG_LENGTH] =
			((Channel << 8) & 0xFF00) | (Channel & 0x00FF);

	StartFlag[DSP_SCIA_START_FLAG_LENGTH + 1] =
			((Channel << 8) & 0xFF00) | (Channel & 0x00FF);


	memcpy_fast_far(TxBuff, StartFlag, DSP_SCIA_HEADER_LENGTH);
	Counter += DSP_SCIA_HEADER_LENGTH;

	memcpy_fast_far(TxBuff + Counter, &(sp_GSRAM_CPU2[Bank]), DSP_SCIA_TX_DATA_LENGTH);
	Counter += DSP_SCIA_TX_DATA_LENGTH;

	memcpy_fast_far(TxBuff + Counter, Dumy, DSP_SCIA_TX_DUMY_LENGTH);
	Counter += DSP_SCIA_TX_DUMY_LENGTH;

	//Bank = //링버퍼로 뱅크 카운트 조절.
	CRC16 = CRC16_Conversion(TxBuff, DSP_SCIA_TX_MAX_DATA_LENGTH - DSP_SCIA_CRC16_LENGTH);

	memcpy_fast_far(TxBuff + Counter, &(CRC16), DSP_SCIA_CRC16_LENGTH);
	Counter += DSP_SCIA_CRC16_LENGTH;

	DSP_SCIA_Write(TxBuff, Counter);

	return(SUCCESS); //SUCCESS Or FAIL
}


Uint16 COMMUNICATION_RS422A_RxUnPacket(volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1, volatile Uint16 *RxBuff, volatile Uint16 Channel)
{
	Uint32 Bank = 0;
	memcpy_fast_far(&(sp_GSRAM_CPU1[Bank]), RxBuff, sizeof(S_COMMUNICATION_GSRAM_CPU1));
	return(SUCCESS); //SUCCESS Or FAIL
}
#endif //End #ifdef DEVICE_CPU2

//
//DEVICE_CPU1
//
#ifdef DEVICE_CPU1
extern volatile S_COMMUNICATION_GSRAM_CPU1 GSRAM_STPE[];
extern volatile S_COMMUNICATION_GSRAM_CPU2 GSRAM_DATA[];
extern volatile Uint16 ui_UpdateflagCPU1;
extern volatile Uint16 ui_UpdateflagCPU2;

volatile S_OPERATION_STEP s_STEP;
volatile S_COMMUNICATION_TX s_DATA;
volatile S_COMMUNICATION_RX_JUMP s_JUMP;
volatile S_COMMUNICATION_RX_COMMAD s_COMMAND;
volatile S_COMMUNICATION_RX_START s_START;
volatile S_COMMUNICATION_RX_PAUSE s_PAUSE;

volatile S_OPERATION_SENSING_DATA s_SENSING_DATA;
volatile S_COMMUNICATION_TX_END   s_SENSING_END;
volatile S_COMMUNICATION_TX_ALARM s_SENSING_ALARM;



#pragma CODE_SECTION(COMMUNICATION_STEPRead, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_JUMPRead, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_COMMANDRead, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_STARTRead, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_PAUSERead, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_DATAWrite, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_ENDWrite, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_ALARMWrite, ".TI.ramfunc");

Uint16 COMMUNICATION_STEPRead(volatile S_OPERATION_STEP* spSTEP, volatile S_COMMUNICATION_GSRAM_CPU1 *spGSRAM_STPE, volatile S_COMMUNICATION_GSRAM_CPU2 *spGSRAM_DATA)
{
	if(spGSRAM_DATA[0].CPU2_Step != spGSRAM_STPE[0].CPU1_Step)
	{
		spGSRAM_STPE[0].CPU1_StepHeader;
		spGSRAM_STPE[0].CPU1_StepDataCount; //스탭 카운트 갯수..
		spGSRAM_STPE[0].CPU1_StepChannelIndex;
		spGSRAM_STPE[0].CPU1_StepIndex;

		(*spSTEP).COMMAND 					=  spGSRAM_STPE[0].CPU1_STEP.COMMAND;
		(*spSTEP).MODE 						=  spGSRAM_STPE[0].CPU1_STEP.MODE;
		(*spSTEP).OPERATIN 					=  spGSRAM_STPE[0].CPU1_STEP.OPERATIN;
		(*spSTEP).SAMPLING_TIME 			=  spGSRAM_STPE[0].CPU1_STEP.SAMPLING_TIME;
		(*spSTEP).SEND_TIME 				=  spGSRAM_STPE[0].CPU1_STEP.SEND_TIME;
		(*spSTEP).STEP_INDEX 				=  spGSRAM_STPE[0].CPU1_STEP.STEP_INDEX;
		(*spSTEP).CYCLE_LOOP_COUNT 			=  spGSRAM_STPE[0].CPU1_STEP.CYCLE_LOOP_COUNT;
		(*spSTEP).GOTO_INDEX 				=  spGSRAM_STPE[0].CPU1_STEP.GOTO_INDEX;
		(*spSTEP).RETURN_INDEX 				=  spGSRAM_STPE[0].CPU1_STEP.RETURN_INDEX;

		(*spSTEP).VOLTAGE_REF 				=  (((Uint32)spGSRAM_STPE[0].CPU1_STEP.VOLTAGE_REFx16 << 16) & (Uint32)0xFFFF0000);
		(*spSTEP).VOLTAGE_REF 				|= ((Uint32)spGSRAM_STPE[0].CPU1_STEP.VOLTAGE_REFx00 & (Uint32)0x0000FFFF);
		(*spSTEP).F_VOLTAGE_REF				= (float)((*spSTEP).VOLTAGE_REF) * (float)(0.001);

		(*spSTEP).END_VOLTAGE_INDEX 		=  spGSRAM_STPE[0].CPU1_STEP.END_VOLTAGE_INDEX;

		(*spSTEP).CURRENT_REF	 			=  (((Uint32)spGSRAM_STPE[0].CPU1_STEP.CURRENT_REF_16 << 16) & (Uint32)0xFFFF0000);
		(*spSTEP).CURRENT_REF 				|= ((Uint32)spGSRAM_STPE[0].CPU1_STEP.CURRENT_REFx00 & (Uint32)0x0000FFFF);
		(*spSTEP).F_CURRENT_REF				= (float)((*spSTEP).CURRENT_REF) * (float)(0.001);

		(*spSTEP).END_TIME_CONDITION 		=  (((Uint32)spGSRAM_STPE[0].CPU1_STEP.END_TIME_CONDITIONx16 << 16) & (Uint32)0xFFFF0000);
		(*spSTEP).END_TIME_CONDITION 		|= ((Uint32)spGSRAM_STPE[0].CPU1_STEP.END_TIME_CONDITIONx00 & (Uint32)0x0000FFFF);

		(*spSTEP).PATTERN_PRIOD 			=  spGSRAM_STPE[0].CPU1_STEP.PATTERN_PRIOD;
		(*spSTEP).END_TIME_INDEX 			=  spGSRAM_STPE[0].CPU1_STEP.END_TIME_INDEX;

		(*spSTEP).END_CURRENT_CONDITION 	=  (((Uint32)spGSRAM_STPE[0].CPU1_STEP.END_CURRENT_CONDITIONx16 << 16) & (Uint32)0xFFFF0000);
		(*spSTEP).END_CURRENT_CONDITION 	|= ((Uint32)spGSRAM_STPE[0].CPU1_STEP.END_CURRENT_CONDITIONx00 & 0x0000FFFF);
		(*spSTEP).F_END_CURRENT_CONDITION 	=  (float)((*spSTEP).END_CURRENT_CONDITION) * (float)(0.001);

		(*spSTEP).END_CURRENT_INDEX 		=  spGSRAM_STPE[0].CPU1_STEP.END_CURRENT_INDEX;

		(*spSTEP).END_CAPACITY_CONDITION 	=  (((Uint32)spGSRAM_STPE[0].CPU1_STEP.END_CAPACITY_CONDITIONx16 << 16) & (Uint32)0xFFFF0000);
		(*spSTEP).END_CAPACITY_CONDITION 	|= ((Uint32)spGSRAM_STPE[0].CPU1_STEP.END_CAPACITY_CONDITIONx00 & (Uint32)0x0000FFFF);
		(*spSTEP).F_END_CAPACITY_CONDITION 	=  (float)((*spSTEP).END_CAPACITY_CONDITION) * (float)(0.001);

		(*spSTEP).END_CAPACITY_INDEX 		=  spGSRAM_STPE[0].CPU1_STEP.END_CAPACITY_INDEX;

		(*spSTEP).END_WATT_CONDITION 		=  (((Uint32)spGSRAM_STPE[0].CPU1_STEP.END_WATT_CONDITIONx16 << 16) & (Uint32)0xFFFF0000);
		(*spSTEP).END_WATT_CONDITION 		|= ((Uint32)spGSRAM_STPE[0].CPU1_STEP.END_WATT_CONDITIONx00 & (Uint32)0x0000FFFF);
		(*spSTEP).F_END_WATT_CONDITION 		=  (float)((*spSTEP).END_WATT_CONDITION) * (float)(0.001);

		(*spSTEP).END_WATT_INDEX 			=  spGSRAM_STPE[0].CPU1_STEP.END_WATT_INDEX;

		spGSRAM_DATA[0].CPU2_Step = spGSRAM_STPE[0].CPU1_Step;
	}

	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_JUMPRead(volatile S_COMMUNICATION_RX_JUMP* spJUMP, volatile S_COMMUNICATION_GSRAM_CPU1 *spGSRAM_STPE, volatile S_COMMUNICATION_GSRAM_CPU2 *spGSRAM_DATA)
{
	if((*spGSRAM_DATA).CPU2_Jump != (*spGSRAM_STPE).CPU1_Jump)
	{
		(*spGSRAM_DATA).CPU2_StepIndex = (*spGSRAM_STPE).CPU1_JUMP.STEP_INDEX; //짭푸할 인댁스를 불러와서 시작해주세요.
											//노멀 값은 0이오 0 주소에 노멀 값을 넣어 주시오.
		(*spGSRAM_DATA).CPU2_Jump = ~(*spGSRAM_STPE).CPU1_Jump;
	}
	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_COMMANDRead(volatile S_COMMUNICATION_RX_COMMAD* spCOMMAND, volatile S_COMMUNICATION_GSRAM_CPU1 *spGSRAM_STPE, volatile S_COMMUNICATION_GSRAM_CPU2 *spGSRAM_DATA, volatile S_OPERATION_STEP* spSTEP)
{
	if((*spGSRAM_DATA).CPU2_Command != (*spGSRAM_STPE).CPU1_Command)
	{
		(*spGSRAM_DATA).CPU2_StepIndex = 0; //노멀값을 넣고 커맨드를 실행하시오. 커맨드는 별거 없을 것 같다. 릴레이 온오프 정도. 불러와서 시작해주세요.
		(*spSTEP).COMMAND = (*spGSRAM_STPE).CPU1_COMMAND.COMMAD;	//노멀 값은 0이오 0 주소에 노멀 값을 넣어 주시오.
		(*spGSRAM_DATA).CPU2_Command = ~(*spGSRAM_STPE).CPU1_Command;
	}

	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_STARTRead(volatile S_COMMUNICATION_RX_START* spSTART, volatile S_COMMUNICATION_GSRAM_CPU1 *spGSRAM_STPE, volatile S_COMMUNICATION_GSRAM_CPU2 *spGSRAM_DATA, volatile S_OPERATION_SENSING_DATA *spSENSING_DATA)
{
	if((*spGSRAM_DATA).CPU2_Start != (*spGSRAM_STPE).CPU1_Start)
	{
		(*spGSRAM_DATA).CPU2_StepIndex = 1; 				//스텝 1번 불러와서 시작해주세요.
		CPUTimer_stopTimer(CPUTIMER0_BASE);
		CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);
		(*spSENSING_DATA).TIME_SPAN = CLEAR;

		CPUTimer_startTimer(CPUTIMER0_BASE);
		(*spGSRAM_DATA).CPU2_Start = ~(*spGSRAM_STPE).CPU1_Start;//노멀 값은 0이오 0 주소에 노멀 값을 넣어 주시오.
	}																//DATA 및 카운터 등등 초기화 해주길 바라오.


	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_PAUSERead(volatile S_COMMUNICATION_RX_PAUSE* spPAUSE, volatile S_COMMUNICATION_GSRAM_CPU1 *spGSRAM_STPE, volatile S_COMMUNICATION_GSRAM_CPU2 *spGSRAM_DATA, volatile S_OPERATION_STEP* spSTEP)
{
	static Uint16 OldStepIndex = 0;

	if((*spGSRAM_DATA).CPU2_Pause != (*spGSRAM_STPE).CPU1_Pause)
	{

		switch((*spGSRAM_STPE).CPU1_PAUSE.COMMAND)
		{
		case OPERATION_PAUSE_STOP:
			(*spGSRAM_DATA).CPU2_StepIndex = OldStepIndex; //이전 데이터 소환술
			break;
		case OPERATION_PAUSE_PAUSE:
			OldStepIndex = (*spSTEP).STEP_INDEX;
			(*spGSRAM_DATA).CPU2_StepIndex = 0; //노멀 값을 불러오시오
			break;
		case OPERATION_PAUSE_COMPLETE:
			(*spGSRAM_DATA).CPU2_StepIndex = 0; //노멀 값을 불러오시오 정지인것 같아
												//이따는 값도 초기화 해야 할 것 같은데 말이야..
			break;
		}
		(*spGSRAM_DATA).CPU2_Pause = ~(*spGSRAM_STPE).CPU1_Pause;
	}
	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_DATAWrite(volatile S_COMMUNICATION_TX* spDATA, volatile S_OPERATION_STEP* spSTEP, volatile S_OPERATION_SENSING_DATA *spSENSING_DATA)
{
	//스텝이 업데이트 되면 한번 진행 되고 업데이트 합시다.
	//그리고 이건 한개의 데이타만 진행 되니 뱅크를 사용하여야 합니다.
	//타이머 루틴에 들어가야 합니다.
	//안그럼 안되요 ㅋㅋㅋㅋ 타임 스팬드 여기에 넣읍시다.
	//1ms 100개 데이타 만듭시다.
	Uint32 Temp32 = 0;
	Uint64 Temp64 = 0;

	(*spDATA).SAMPLING_TIME = (*spSTEP).SAMPLING_TIME;
	(*spDATA).STEP_INDEX = (*spSTEP).STEP_INDEX;
	(*spDATA).COMMAND = (*spSTEP).COMMAND;
	(*spDATA).END_CONDITION_STATE;
	(*spDATA).OPERATIN = (*spSTEP).OPERATIN;
	(*spDATA).MODE = (*spSTEP).MODE;

	Temp32 = (Uint32)((*spSENSING_DATA).VOLTAGE * 1000.0);
	(*spDATA).VOLTAGEx16 = (Uint16)((Temp32 >> 16) & 0x0000FFFF);
	(*spDATA).VOLTAGEx00 = (Uint16)((Temp32      ) & 0x0000FFFF);

	Temp32 = (Uint32)((*spSENSING_DATA).CURRENT * 1000.0);
	(*spDATA).CURRENTx16 = (Uint16)((Temp32 >> 16) & 0x0000FFFF);
	(*spDATA).CURRENTx00 = (Uint16)((Temp32      ) & 0x0000FFFF);

	Temp64 = (Uint64)((*spSENSING_DATA).TIME_SPAN);
	(*spDATA).TIME_SPANx64= (Uint16)((Temp64 >> 48) & 0x000000000000FFFF);
	(*spDATA).TIME_SPANx32= (Uint16)((Temp64 >> 32) & 0x000000000000FFFF);
	(*spDATA).TIME_SPANx16= (Uint16)((Temp64 >> 16) & 0x000000000000FFFF);
	(*spDATA).TIME_SPANx00= (Uint16)((Temp64      ) & 0x000000000000FFFF);

	Temp32 = (Uint32)((*spSENSING_DATA).END_CONDITION_VALUE * 1000.0);//이건 도데체 뭐길래 있는거지 설명도 없고
	(*spDATA).END_CONDITION_VALUEx16= (Uint16)((Temp32 >> 16) & 0x0000FFFF);
	(*spDATA).END_CONDITION_VALUEx00= (Uint16)((Temp32      ) & 0x0000FFFF);

	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_ENDWrite(volatile S_COMMUNICATION_GSRAM_CPU1 *spGSRAM_STPE, volatile S_COMMUNICATION_GSRAM_CPU2 *spGSRAM_DATA, volatile S_COMMUNICATION_TX_END *spSENSING_END)
{
	if((*spGSRAM_DATA).CPU2_End == (*spGSRAM_STPE).CPU1_End)
	{
		(*spSENSING_END).Header = HEADER_END;
		(*spSENSING_END).ChannelIndex = (1 << COMMUNICATION_CHANNEL_NUMBER);
		(*spSENSING_END).DataCount = COMMUNICATION_INTEGER_LENGTH_END * 2;

		(*spGSRAM_DATA).CPU2_StepIndex = 0; //끝났소 0을 써주시오. 불러와서 시작해주세요.
											//노멀 값은 0이오 0 주소에 노멀 값을 넣어 주시오.
		(*spGSRAM_DATA).CPU2_END = (*spSENSING_END);
		(*spGSRAM_DATA).CPU2_End = ~(*spGSRAM_STPE).CPU1_End;
	}

	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_ALARMWrite(volatile S_COMMUNICATION_GSRAM_CPU1 *spGSRAM_STPE, volatile S_COMMUNICATION_GSRAM_CPU2 *spGSRAM_DATA, volatile S_COMMUNICATION_TX_ALARM *spSENSING_ALARM)
{
	if((*spGSRAM_DATA).CPU2_Alarm == (*spGSRAM_STPE).CPU1_Alarm)
	{
		(*spSENSING_ALARM).Header = HEADER_ALARM;
		(*spSENSING_ALARM).ChannelIndex = (1 << COMMUNICATION_CHANNEL_NUMBER);
		(*spSENSING_ALARM).DataCount = COMMUNICATION_INTEGER_LENGTH_ALARM * 2;

		(*spGSRAM_DATA).CPU2_ALARM =  (*spSENSING_ALARM);
		(*spGSRAM_DATA).CPU2_Alarm = ~(*spGSRAM_STPE).CPU1_Alarm;
	}

	return(SUCCESS); //SUCCESS Or FAIL
}
#endif //End #ifdef DEVICE_CPU2

//
// End of file
//

