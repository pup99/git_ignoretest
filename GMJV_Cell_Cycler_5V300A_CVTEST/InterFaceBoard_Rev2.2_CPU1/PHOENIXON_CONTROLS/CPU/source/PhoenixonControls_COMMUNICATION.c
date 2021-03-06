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

extern volatile S_COMMUNICATION_RX_TEMP_CONV read_chamber_temp_conv[4];

#pragma CODE_SECTION(COMMUNICATION_Ethernet_STEPRead, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_Ethernet_JUMPRead, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_Ethernet_COMMANDRead, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_Ethernet_STARTRead, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_Ethernet_PAUSERead, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_Ethernet_TempRead, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_Ethernet_DATAWrite, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_Ethernet_ENDWrite, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_Ethernet_ALARMWrite, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_DRAM_STEPWrite, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_DRAM_DATARead, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_Write, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_Read, ".TI.ramfunc");
//#pragma CODE_SECTION(, ".TI.ramfunc");
__attribute__((far)) volatile S_COMMUNICATION_DRAM s_SDRAM[COMMUNICATION_CHANNEL];

volatile Uint16 gui_Channel = 0;
volatile Uint16 gui_StartFlag = 0;
volatile Uint32 gul_DataCount = 0;
volatile Uint32 gul_SumCount  = 0;
volatile Uint32 gPointerCount[COMMUNICATION_CHANNEL];

void COMMUNICATION_INIT(void)
{
	Uint16 ui_i = 0;
	Uint32 ul_Temp = 0;

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{
		ul_Temp = (Uint32)&(s_SDRAM[ui_i]);
		ul_Temp |= EMIF_CS0_BASE_ADDRESS;
		s_SDRAMp[ui_i] = (S_COMMUNICATION_DRAM *)(ul_Temp); //???? ???? ?????? ?????? ?????? ????????
	}							  //?????? ?????? ???? ???? ???? ???? ???? 0x80000000??
}								  //?????? ???? ?????? ???? ???? ???? ??????.

// ?????????? COMMUNICATION_Ethernet_STEPRead ???? ????????.

Uint16 COMMUNICATION_Ethernet_STEPRead(volatile S_COMMUNICATION_DRAM **sp_IF, Uint16 *buf, Uint32 len)
{
  Uint16 ui_i = 0;
  Uint16 length = 0;
  Uint16 tx_len = 0;

  length = len;

    while (length)
    {
      if(!gui_StartFlag)
      {   gui_StartFlag = HEADER_DATA;

          gul_SumCount  = ((buf[1] * COMMUNICATION_INTEGER_LENGTH_RX) + COMMUNICATION_INTEGER_LENGTH_HEADER) * 2; //???? DATA ?? ???? ???? ????
          gui_Channel   = buf[2]; //???? ???? ????
          //???? ???? ???? ??????
          gul_DataCount = CLEAR;
          for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)gPointerCount[ui_i] = CLEAR;
      }
      gul_DataCount += length; //8BIT ?????? Ethernet???? ?????? BigEndian ???????? ????

      for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
      {   //4???? ???????? ???? ???? Channel?? ???????? ???? SETP DATA?? ???? ????

          if(gul_DataCount > gul_SumCount)
              tx_len = length - (gul_DataCount - gul_SumCount);
          else
              tx_len = length;

          if(gui_Channel & (0x1 << ui_i))
          {
              memcpy_fast_far(&((*sp_IF)[ui_i].STEP.Header) + gPointerCount[ui_i], buf, tx_len / 2);
              gPointerCount[ui_i] += (tx_len / 2);
          }
      }

      if(gul_DataCount >= gul_SumCount)//Data ?? ?????? ?????? Counting?? ?????? ?????? ??????
      {                              //??????(???? ????)???? ???????? ???? Flag ?????? ??.
          if( (gul_DataCount > gul_SumCount) && (buf[(length - (gul_DataCount - gul_SumCount))/2] == HEADER_DATA) )
          {
              buf = buf + ((length - (gul_DataCount - gul_SumCount)) / 2);
              length = gul_DataCount - gul_SumCount;
              gui_StartFlag = CLEAR;
          }
          else
          {
              length = 0;
              //???? ???? ???? ??????
              gui_StartFlag = CLEAR;

              gul_SumCount  = CLEAR;
              gui_Channel   = CLEAR;

              gul_DataCount = CLEAR;
              for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)gPointerCount[ui_i] = CLEAR;
          }
      }
      else
          length = 0;
    }

    return(gui_StartFlag);   //SUCCESS Or FAIL
                            //?????????? ???? ?? ???? ?????? 0?? ???????????? HEADER_DATA?? ????.
}

Uint16 COMMUNICATION_Ethernet_JUMPRead(volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1,
		volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2, Uint16 *buf, Uint32 len)
{
	Uint16 ui_i 			 = 0;
	Uint32 PointerCount 	 = 0;
	Uint16 ui_Channel 		 = 0;
	Uint16 ui_ChannelIndex   = 0;
	Uint32 ul_DataCount 	 = 0;
	Uint16 ul_DataCountTotal = 0;

	ul_DataCountTotal = (Uint16)((Uint32)(len) / (Uint32)(sizeof(S_COMMUNICATION_RX_JUMP) * 2)); //8BIT ?????? Ethernet???? ?????? BigEndian ???????? ????

	for(ui_i = 0; ui_i < ul_DataCountTotal; ui_i++)
	{	//4???? ???????? ???? ???? Channel?? ???????? ???? SETP DATA?? ???? ????
	    ui_ChannelIndex = CLEAR;
		ul_DataCount  = buf[1 + PointerCount]; //???? DATA ?? ???? ???? ????
		ui_Channel 	  = buf[2 + PointerCount]; //???? ???? ????

		//????????
		while((ui_Channel /= 2)) ui_ChannelIndex++;

		if(HEADER_JUMP == buf[PointerCount])
			if(ui_ChannelIndex < COMMUNICATION_CHANNEL)// && (sp_GSRAM_CPU1[ui_i]).CPU1_Start == (sp_GSRAM_CPU2[ui_i]).CPU2_Start)
			{
				memcpy_fast_far(&(sp_GSRAM_CPU1[ui_ChannelIndex]).CPU1_JUMP.Header, buf + PointerCount, (ul_DataCount / 2));
				(sp_GSRAM_CPU1[ui_ChannelIndex]).CPU1_Jump++;// = ~(sp_GSRAM_CPU2[ui_i]).CPU2_Start;
			}
		PointerCount += (ul_DataCount / 2);
	}

	return(CLEAR); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_Ethernet_COMMANDRead(volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1,
		volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2, Uint16 *buf, Uint32 len)
{
	Uint16 ui_i 			 = 0;
	Uint32 PointerCount 	 = 0;
	Uint16 ui_Channel 		 = 0;
	Uint16 ui_ChannelIndex   = 0;
	Uint32 ul_DataCount 	 = 0;
	Uint16 ul_DataCountTotal = 0;

	ul_DataCountTotal = (Uint16)((Uint32)(len) / (Uint32)(sizeof(S_COMMUNICATION_RX_COMMAD) * 2)); //8BIT ?????? Ethernet???? ?????? BigEndian ???????? ????

	for(ui_i = 0; ui_i < ul_DataCountTotal; ui_i++)
	{	//4???? ???????? ???? ???? Channel?? ???????? ???? SETP DATA?? ???? ????
	    ui_ChannelIndex = CLEAR;
		ul_DataCount  = buf[1 + PointerCount]; //???? DATA ?? ???? ???? ????
		ui_Channel 	  = buf[2 + PointerCount]; //???? ???? ????

		//????????
		while((ui_Channel /= 2)) ui_ChannelIndex++;

		if(HEADER_COMMAND == buf[PointerCount])
			if(ui_ChannelIndex < COMMUNICATION_CHANNEL)// && (sp_GSRAM_CPU1[ui_i]).CPU1_Start == (sp_GSRAM_CPU2[ui_i]).CPU2_Start)
			{
				memcpy_fast_far(&(sp_GSRAM_CPU1[ui_ChannelIndex]).CPU1_COMMAND.Header, buf + PointerCount, (ul_DataCount / 2));
				(sp_GSRAM_CPU1[ui_ChannelIndex]).CPU1_Command++;// = ~(sp_GSRAM_CPU2[ui_i]).CPU2_Start;
			}
		PointerCount += (ul_DataCount / 2);
	}

	return(CLEAR); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_Ethernet_STARTRead(volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1,
		volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2, Uint16 *buf, Uint32 len)
{
	Uint16 ui_i 			 = 0;
	Uint32 PointerCount 	 = 0;
	Uint16 ui_Channel 		 = 0;
	Uint16 ui_ChannelIndex   = 0;
	Uint32 ul_DataCount 	 = 0;
	Uint16 ul_DataCountTotal = 0;

	ul_DataCountTotal = (Uint16)((Uint32)(len) / (Uint32)(sizeof(S_COMMUNICATION_RX_START) * 2)); //8BIT ?????? Ethernet???? ?????? BigEndian ???????? ????

	for(ui_i = 0; ui_i < ul_DataCountTotal; ui_i++)
	{	//4???? ???????? ???? ???? Channel?? ???????? ???? SETP DATA?? ???? ????
	    ui_ChannelIndex = CLEAR;
		ul_DataCount  = buf[1 + PointerCount]; //???? DATA ?? ???? ???? ????
		ui_Channel 	  = buf[2 + PointerCount]; //???? ???? ????

		//????????
		while((ui_Channel /= 2)) ui_ChannelIndex++;

		if(HEADER_START == buf[PointerCount])
			if(ui_ChannelIndex < COMMUNICATION_CHANNEL)// && (sp_GSRAM_CPU1[ui_i]).CPU1_Start == (sp_GSRAM_CPU2[ui_i]).CPU2_Start)
			{
				memcpy_fast_far(&(sp_GSRAM_CPU1[ui_ChannelIndex]).CPU1_START.Header, buf + PointerCount, (ul_DataCount / 2));
				(sp_GSRAM_CPU1[ui_ChannelIndex]).CPU1_Start++;// = ~(sp_GSRAM_CPU2[ui_i]).CPU2_Start;
			}
        PointerCount += (ul_DataCount / 2);
	}

	return(CLEAR); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_Ethernet_PAUSERead(volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1,
		volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2, Uint16 *buf, Uint32 len)
{
	Uint16 ui_i 			 = 0;
	Uint32 PointerCount 	 = 0;
	Uint16 ui_Channel 		 = 0;
	Uint16 ui_ChannelIndex   = 0;
	Uint32 ul_DataCount 	 = 0;
	Uint16 ul_DataCountTotal = 0;

	ul_DataCountTotal = (Uint16)((Uint32)(len) / (Uint32)(sizeof(S_COMMUNICATION_RX_PAUSE) * 2)); //8BIT ?????? Ethernet???? ?????? BigEndian ???????? ????

	for(ui_i = 0; ui_i < ul_DataCountTotal; ui_i++)
	{	//4???? ???????? ???? ???? Channel?? ???????? ???? SETP DATA?? ???? ????
	    ui_ChannelIndex = CLEAR;
		ul_DataCount  = buf[1 + PointerCount]; //???? DATA ?? ???? ???? ????
		ui_Channel 	  = buf[2 + PointerCount]; //???? ???? ????

		//????????
		while((ui_Channel /= 2)) ui_ChannelIndex++;

		if(HEADER_PAUSE == buf[PointerCount])
			if(ui_ChannelIndex < COMMUNICATION_CHANNEL)// && (sp_GSRAM_CPU1[ui_i]).CPU1_Start == (sp_GSRAM_CPU2[ui_i]).CPU2_Start)
			{
				memcpy_fast_far(&(sp_GSRAM_CPU1[ui_ChannelIndex]).CPU1_PAUSE.Header, buf + PointerCount, (ul_DataCount / 2));
				(sp_GSRAM_CPU1[ui_ChannelIndex]).CPU1_Pause++;// = ~(sp_GSRAM_CPU2[ui_i]).CPU2_Start;
			}
		PointerCount += (ul_DataCount / 2);
	}

	return(CLEAR); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_Ethernet_TempRead(volatile S_COMMUNICATION_RX_TEMP *chamber, Uint16 *buf, Uint32 len)
{
    Uint16 ui_i = 0;
    Uint32 PointerCount = 0;
    Uint16 ui_Channel = 0;
    Uint16 ui_ChannelIndex = 0;
    Uint32 ul_DataCount = 0;
    Uint16 ul_DataCountTotal = 0;

    ul_DataCountTotal = (Uint16) ((Uint32) (len) / (Uint32) (sizeof(S_COMMUNICATION_RX_COMMAD) * 2));  //8BIT ?????? Ethernet???? ?????? BigEndian ???????? ????

    for (ui_i = 0; ui_i < ul_DataCountTotal; ui_i++) {   //4???? ???????? ???? ???? Channel?? ???????? ???? SETP DATA?? ???? ????
        ui_ChannelIndex = CLEAR;
        ul_DataCount = buf[1 + PointerCount];  //???? DATA ?? ???? ???? ????
        ui_Channel = buf[2 + PointerCount];  //???? ???? ????

        //????????
        while ((ui_Channel /= 2))
            ui_ChannelIndex++;

        if (HEADER_TEMP == buf[PointerCount]) {
            if (ui_ChannelIndex < COMMUNICATION_CHANNEL) {
                memcpy_fast_far(&chamber[ui_ChannelIndex], buf + PointerCount, (ul_DataCount / 2));
                if (chamber[ui_ChannelIndex].TEMP & 0x8000) {
                    read_chamber_temp_conv[ui_ChannelIndex].sign = 1;
                } else {
                    read_chamber_temp_conv[ui_ChannelIndex].sign = 0;
                }
                read_chamber_temp_conv[ui_ChannelIndex].temp = (float) (chamber[ui_ChannelIndex].TEMP & 0x7FFF) * (float) (0.1);
                read_chamber_temp_conv[ui_ChannelIndex].new_count++;
            }
        }
        PointerCount += (ul_DataCount / 2);
    }

    return (CLEAR);  //SUCCESS Or FAIL
}

Uint32 COMMUNICATION_Ethernet_DATAWrite(volatile S_COMMUNICATION_DRAM **sp_IF, Uint16 *buf)
{
//	Uint16 ui_Channel = 0;									//?????? ???????? ???? ?????? ??.
	Uint16 ui_i = 0;										//???????? ?????? ???? ?????????? ?? ?? ????.
	Uint32 BufferCount = 0;									//?????????? ?????? ???? ???? ?? ?? ????.
	Uint32 Index = 0;
	Uint32 Count  = 0;

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4???? ???????? ???? ???? Channel?? ???????? ???? SETP DATA?? ???? ????
		Count = RINGBUFFER_GetSize((S_RINGBUFFER_FILE *)&((*sp_IF)[ui_i].RING_DATA), COMMUNICATION_TX_DATA_LENGTH);//?????? ???????? ????
//############Test?????? if???? 1???? ?????? ???? ???????? ??.
		if(Count)// && (*sp_IF)[ui_i].WriteFlagData == SET)//???????? ?????? ????
		{
			Index = RINGBUFFER_Get((S_RINGBUFFER_FILE *)&((*sp_IF)[ui_i].RING_DATA), COMMUNICATION_TX_DATA_LENGTH);    //?????? ???????? ????
			(*sp_IF)[ui_i].DATA[Index].Header = HEADER_DATA;
			(*sp_IF)[ui_i].DATA[Index].DataCount = (COMMUNICATION_INTEGER_LENGTH_DATA * 2);
			(*sp_IF)[ui_i].DATA[Index].ChannelIndex = 1 << ui_i;

			memcpy_fast_far(buf + BufferCount, &((*sp_IF)[ui_i].DATA[Index].Header), (COMMUNICATION_INTEGER_LENGTH_DATA));
			BufferCount += COMMUNICATION_INTEGER_LENGTH_DATA;
#if(ETX_VER == 1)
            buf[BufferCount] = 0x4021;
            BufferCount++;
#endif
			//(*sp_IF)[ui_i].WriteFlagData = CLEAR; // ?????? ???? ??????
		}
	}
	return(BufferCount * 2); //SUCCESS Or FAIL
}

Uint32 COMMUNICATION_Ethernet_ENDWrite(volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1,
		volatile  S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2, Uint16 *buf)
{
	Uint16 ui_Channel = 0;									//?????? ???????? ???? ?????? ??.
	Uint16 ui_i = 0;										//???????? ?????? ???? ?????????? ?? ?? ????.
	Uint32 BufferCount = 0;									//?????????? ?????? ???? ???? ?? ?? ????.

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{
		ui_Channel |= (sp_GSRAM_CPU2[ui_i]).CPU2_END.ChannelIndex;//???????? ?????? ????????. ******
	}

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4???? ???????? ???? ???? Channel?? ???????? ???? SETP DATA?? ???? ????
		if(ui_Channel & (0x1 << ui_i) && (sp_GSRAM_CPU1[ui_i]).CPU1_End != (sp_GSRAM_CPU2[ui_i]).CPU2_End)// (if))//???????? ?????? ??????????. ???? ????
		{
			memcpy_fast_far(buf + BufferCount, &((sp_GSRAM_CPU2[ui_i]).CPU2_END.Header), COMMUNICATION_INTEGER_LENGTH_END);
			BufferCount += COMMUNICATION_INTEGER_LENGTH_END;
			(sp_GSRAM_CPU1[ui_i]).CPU1_End = (sp_GSRAM_CPU2[ui_i]).CPU2_End; // ?????? ???? ??????
		}
	}
	return(BufferCount * 2); //SUCCESS Or FAIL ?????? (8bit) ???? ??
}

Uint32 COMMUNICATION_Ethernet_ALARMWrite(volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1,
		volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2, Uint16 *buf)
{
	Uint16 ui_Channel = 0;									//?????? ???????? ???? ?????? ??.
	Uint16 ui_i = 0;										//???????? ?????? ???? ?????????? ?? ?? ????.
	Uint32 BufferCount = 0;									//?????????? ?????? ???? ???? ?? ?? ????.

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{
		ui_Channel |= (sp_GSRAM_CPU2[ui_i]).CPU2_ALARM.ChannelIndex;//???????? ?????? ????????. ******
	}

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4???? ???????? ???? ???? Channel?? ???????? ???? SETP DATA?? ???? ????
		if(ui_Channel & (0x1 << ui_i) && (sp_GSRAM_CPU1[ui_i]).CPU1_Alarm != (sp_GSRAM_CPU2[ui_i]).CPU2_Alarm)// (if))//???????? ?????? ??????????. ???? ????
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
	//CPU2???? index???? ???? ???????? ???? ???? ???? ???????? ??????????.
	//Start ?????? ???? ???? ???? ???????? ??????.
	//End ?????? ???? ???? ???? ???????? ?????? ??????.

	Uint16 ui_Channel = 0;
	Uint16 ui_i = 0;
	Uint16 ui_Index = 0;
	Uint16 ui_IndexStage1 = 0;
	Uint16 ui_IndexStage2 = 0;

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{
		ui_Channel |= (*sp_IF)[ui_i].STEP.ChannelIndex;//???????? ?????? ????????. ******
	}

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4???? ???????? ???? ???? Channel?? ???????? ???? SETP DATA?? ???? ????

		//if((sp_GSRAM_CPU2[ui_i]).CPU2_StepIndex != (sp_GSRAM_CPU1[ui_i]).CPU1_StepIndex)//???? ?????? ???? ???? ???? ???? ???? ???? ???? ??!!!!!
		if(ui_Channel & (0x1 << ui_i))// && (sp_GSRAM_CPU1[ui_i]).CPU1_Step == (sp_GSRAM_CPU2[ui_i]).CPU2_Step)// (if))//???????? ?????? ??????????. ???? ????
		{
			(sp_GSRAM_CPU1[ui_i]).CPU1_StepHeader = (*sp_IF)[ui_i].STEP.Header;
			(sp_GSRAM_CPU1[ui_i]).CPU1_StepDataCount = (*sp_IF)[ui_i].STEP.DataCount;
			(sp_GSRAM_CPU1[ui_i]).CPU1_StepChannelIndex = (*sp_IF)[ui_i].STEP.ChannelIndex;

			ui_Index = (sp_GSRAM_CPU2[ui_i]).CPU2_StepIndex;// ?????? ???? ????

			if((sp_GSRAM_CPU1[ui_i]).CPU1_Jump != (sp_GSRAM_CPU2[ui_i]).CPU2_Jump)// Jump ?????? ???? ???? ????.
			{
				ui_Index = (sp_GSRAM_CPU1[ui_i]).CPU1_JUMP.STEP_INDEX;
			}

			if(ui_Index == 0)//Index = 0 ????
			{
				ui_IndexStage1 = 0;
				ui_IndexStage2 = 0;
			}
			else
			{
				ui_IndexStage1 = ui_Index;
				ui_IndexStage2 = ui_Index;
			}

			//First Step Level
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[0]   = (*sp_IF)[ui_i].STEP.STEP[ui_IndexStage1];
			//Second Step Level
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[1]   = (*sp_IF)[ui_i].STEP.STEP[((*sp_IF)[ui_i].STEP.STEP[ui_IndexStage2].END_TIME_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[2]   = (*sp_IF)[ui_i].STEP.STEP[((*sp_IF)[ui_i].STEP.STEP[ui_IndexStage2].END_CURRENT_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[3]   = (*sp_IF)[ui_i].STEP.STEP[((*sp_IF)[ui_i].STEP.STEP[ui_IndexStage2].END_CAPACITY_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[4]   = (*sp_IF)[ui_i].STEP.STEP[((*sp_IF)[ui_i].STEP.STEP[ui_IndexStage2].END_WATT_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[5]   = (*sp_IF)[ui_i].STEP.STEP[((*sp_IF)[ui_i].STEP.STEP[ui_IndexStage2].END_VOLTAGE_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[6]   = (*sp_IF)[ui_i].STEP.STEP[((*sp_IF)[ui_i].STEP.STEP[ui_IndexStage2].GOTO_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[7]   = (*sp_IF)[ui_i].STEP.STEP[((*sp_IF)[ui_i].STEP.STEP[ui_IndexStage2].RETURN_INDEX)];

#ifdef DEV_SCHEDULE
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[8]   = (*sp_IF)[ui_i].STEP.STEP[((*sp_IF)[ui_i].STEP.STEP[ui_IndexStage2].CYCLE_END_VOLTAGE_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[9]   = (*sp_IF)[ui_i].STEP.STEP[((*sp_IF)[ui_i].STEP.STEP[ui_IndexStage2].CYCLE_END_TIME_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[10]  = (*sp_IF)[ui_i].STEP.STEP[((*sp_IF)[ui_i].STEP.STEP[ui_IndexStage2].CYCLE_END_CAPACITY_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[11]  = (*sp_IF)[ui_i].STEP.STEP[((*sp_IF)[ui_i].STEP.STEP[ui_IndexStage2].CYCLE_END_WATT_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[12]   = (*sp_IF)[ui_i].STEP.STEP[((*sp_IF)[ui_i].STEP.STEP[ui_IndexStage2].CYCLE_END_LOW_VOLTAGE_INDEX)];
#endif
			(sp_GSRAM_CPU1[ui_i]).CPU1_Step++;// = ~(sp_GSRAM_CPU1[ui_i]).CPU1_Step;//???????? ?????? ????.
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
		ui_Channel |= ((sp_GSRAM_CPU2[ui_i]).CPU2_Data != (sp_GSRAM_CPU1[ui_i]).CPU1_Data) << ui_i;//???????? ?????? ????????. ******
	}

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4???? ???????? ???? ???? Channel?? ???????? ???? SETP DATA?? ???? ????
		if(ui_Channel & (0x1 << ui_i))// (if))//???????? ?????? ??????????. ???? ????
		{
			if((sp_GSRAM_CPU2[ui_i]).CPU2_StepIndex == 0)
			{
				RINGBUFFER_Clear((S_RINGBUFFER_FILE *)&((*sp_IF)[ui_i].RING_DATA));
			}
			ul_Index = RINGBUFFER_Put((S_RINGBUFFER_FILE *)&(*sp_IF)[ui_i].RING_DATA, COMMUNICATION_TX_DATA_LENGTH);
			memcpy_fast_far(&((*sp_IF)[ui_i].DATA[ul_Index].Header), &((sp_GSRAM_CPU2[ui_i]).CPU2_DATA.Header), COMMUNICATION_INTEGER_LENGTH_DATA);

			(sp_GSRAM_CPU1[ui_i]).CPU1_Data = (sp_GSRAM_CPU2[ui_i]).CPU2_Data;
//			(*sp_IF)[ui_i].WriteFlagData = SET;  // ?????? ???? ?????? ON
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
extern volatile Uint16 ui_UpdateflagCPU1;
extern volatile Uint16 ui_UpdateflagCPU2;

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

	//Bank = //???????? ???? ?????? ????.
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
		spGSRAM_STPE[0].CPU1_StepDataCount; //???? ?????? ????..
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
		(*spGSRAM_DATA).CPU2_StepIndex = (*spGSRAM_STPE).CPU1_JUMP.STEP_INDEX; //?????? ???????? ???????? ????????????.
											//???? ???? 0???? 0 ?????? ???? ???? ???? ??????.
		(*spGSRAM_DATA).CPU2_Jump = ~(*spGSRAM_STPE).CPU1_Jump;
	}
	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_COMMANDRead(volatile S_COMMUNICATION_RX_COMMAD* spCOMMAND, volatile S_COMMUNICATION_GSRAM_CPU1 *spGSRAM_STPE, volatile S_COMMUNICATION_GSRAM_CPU2 *spGSRAM_DATA, volatile S_OPERATION_STEP* spSTEP)
{
	if((*spGSRAM_DATA).CPU2_Command != (*spGSRAM_STPE).CPU1_Command)
	{
		(*spGSRAM_DATA).CPU2_StepIndex = 0; //???????? ???? ???????? ??????????. ???????? ???? ???? ?? ????. ?????? ?????? ????. ???????? ????????????.
		(*spSTEP).COMMAND = (*spGSRAM_STPE).CPU1_COMMAND.COMMAD;	//???? ???? 0???? 0 ?????? ???? ???? ???? ??????.
		(*spGSRAM_DATA).CPU2_Command = ~(*spGSRAM_STPE).CPU1_Command;
	}

	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_STARTRead(volatile S_COMMUNICATION_RX_START* spSTART, volatile S_COMMUNICATION_GSRAM_CPU1 *spGSRAM_STPE, volatile S_COMMUNICATION_GSRAM_CPU2 *spGSRAM_DATA, volatile S_OPERATION_SENSING_DATA *spSENSING_DATA)
{
	if((*spGSRAM_DATA).CPU2_Start != (*spGSRAM_STPE).CPU1_Start)
	{
		(*spGSRAM_DATA).CPU2_StepIndex = 1; 				//???? 1?? ???????? ????????????.
		CPUTimer_stopTimer(CPUTIMER0_BASE);
		CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);
		(*spSENSING_DATA).TIME_SPAN = CLEAR;

		CPUTimer_startTimer(CPUTIMER0_BASE);
		(*spGSRAM_DATA).CPU2_Start = ~(*spGSRAM_STPE).CPU1_Start;//???? ???? 0???? 0 ?????? ???? ???? ???? ??????.
	}																//DATA ?? ?????? ???? ?????? ?????? ??????.


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
			(*spGSRAM_DATA).CPU2_StepIndex = OldStepIndex; //???? ?????? ??????
			break;
		case OPERATION_PAUSE_PAUSE:
			OldStepIndex = (*spSTEP).STEP_INDEX;
			(*spGSRAM_DATA).CPU2_StepIndex = 0; //???? ???? ??????????
			break;
		case OPERATION_PAUSE_COMPLETE:
			(*spGSRAM_DATA).CPU2_StepIndex = 0; //???? ???? ?????????? ???????? ????
												//?????? ???? ?????? ???? ?? ?? ?????? ??????..
			break;
		}
		(*spGSRAM_DATA).CPU2_Pause = ~(*spGSRAM_STPE).CPU1_Pause;
	}
	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_DATAWrite(volatile S_COMMUNICATION_TX* spDATA, volatile S_OPERATION_STEP* spSTEP, volatile S_OPERATION_SENSING_DATA *spSENSING_DATA)
{
	//?????? ???????? ???? ???? ???? ???? ???????? ??????.
	//?????? ???? ?????? ???????? ???? ???? ?????? ?????????? ??????.
	//?????? ?????? ???????? ??????.
	//?????? ?????? ???????? ???? ?????? ?????? ????????.
	//1ms 100?? ?????? ????????.
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

	Temp32 = (Uint32)((*spSENSING_DATA).END_CONDITION_VALUE * 1000.0);//???? ?????? ?????? ???????? ?????? ????
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

		(*spGSRAM_DATA).CPU2_StepIndex = 0; //?????? 0?? ????????. ???????? ????????????.
											//???? ???? 0???? 0 ?????? ???? ???? ???? ??????.
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

