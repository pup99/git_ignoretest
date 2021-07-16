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
		s_SDRAMp[ui_i] = (S_COMMUNICATION_DRAM *)(ul_Temp); //�ܺ� ���� ������ ������ �޸� ��������
	}							  //�ȵǴ� ������ �־� Ȯ�� �غ� ��� �ּ� 0x80000000��
}								  //������ �־� ������ ���� �־� �־� �����.

Uint16 COMMUNICATION_Ethernet_STEPRead(volatile S_COMMUNICATION_DRAM **sp_IF, Uint16 *buf, Uint32 len)
{
	Uint16 ui_i = 0;
	volatile static Uint16 ui_Channel = 0;
	volatile static Uint16 ui_StartFlag = 0;
	volatile static Uint32 ul_DataCount = 0;
	volatile static Uint32 ul_SumCount	= 0;
	volatile static Uint32 PointerCount[COMMUNICATION_CHANNEL];

//	memcpy_fast_far(sp_IF[0], buf, len / 2);//sp_IF ���� ������ �Դϴ�.
// �޸� �׽�Ʈ ����Դϴ�. ���� ���޹޾Ƽ� �ִ� ���� ������ �Դϴ�.
//	memcpy_fast_far(&((*sp_IF[0]).STEP) + PointerCount[ui_i], buf, len / 2);

//	return 0; //�޸� ī�� ������ ������ ������.

	if(!ui_StartFlag)
	{	ui_StartFlag = HEADER_DATA;

		ul_SumCount  = ((buf[1] * COMMUNICATION_INTEGER_LENGTH_RX) + COMMUNICATION_INTEGER_LENGTH_HEADER) * 2; //���� DATA �� ���� ���� ����
		ui_Channel 	 = buf[2]; //��� ���� ����
		//���� ǥ�� ���� �ʱ�ȭ
		ul_DataCount = CLEAR;
		for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)PointerCount[ui_i] = CLEAR;
	}
	ul_DataCount += len; //8BIT ������ Ethernet���� ���ŵ� BigEndian �ڷ���� ����

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4���� ä�ο��� �ش� �Ǵ� Channel�� Ȯ���ϰ� �ش� SETP DATA�� �ڷ� ����

		if(ui_Channel & (0x1 << ui_i))
		{
			memcpy_fast_far(&((*sp_IF)[ui_i].STEP.Header) + PointerCount[ui_i], buf, len / 2);
			PointerCount[ui_i] += (len / 2);
//			(*sp_IF)[ui_i].ReadFlagStep = SET;
		}
	}

	if(ul_DataCount >= ul_SumCount)//Data �� ������ ������ Counting�� ������ ���� ���ų�
	{							   //������(���� ����)�۾� �����ϰ� �ش� Flag �ʱ�ȭ ��.
		//���� ǥ�� ���� �ʱ�ȭ
		ui_StartFlag = CLEAR;

		ul_SumCount  = CLEAR;
		ui_Channel   = CLEAR;

		ul_DataCount = CLEAR;
		for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)PointerCount[ui_i] = CLEAR;
	}

	return(ui_StartFlag); //SUCCESS Or FAIL
						  //���ϰ����� ���� �� ���� �Ϸ�� 0�� ó�����϶��� HEADER_DATA�� ����.
}

Uint16 COMMUNICATION_Ethernet_JUMPRead(volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1,
		volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2, Uint16 *buf, Uint32 len)
{
	Uint16 ui_i = 0;
	Uint16 ui_Channel = 0;
	Uint32 ul_DataCount = 0;
	Uint32 ul_SumCount	= 0;
	Uint32 PointerCount = 0;

	ul_DataCount = len; //8BIT ������ Ethernet���� ���ŵ� BigEndian �ڷ���� ����

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4���� ä�ο��� �ش� �Ǵ� Channel�� Ȯ���ϰ� �ش� SETP DATA�� �ڷ� ����

		ul_SumCount  = buf[1 + PointerCount]; //���� DATA �� ���� ���� ����
		ui_Channel 	 = buf[2 + PointerCount]; //��� ���� ����

		if(HEADER_JUMP == buf[PointerCount])
		if(ui_Channel & (0x1 << ui_i))// && (sp_GSRAM_CPU1[ui_i]).CPU1_Start == (sp_GSRAM_CPU2[ui_i]).CPU2_Start)
		{
			memcpy_fast_far(&(sp_GSRAM_CPU1[ui_i]).CPU1_JUMP.Header, buf + PointerCount, (ul_SumCount / 2));
			(sp_GSRAM_CPU1[ui_i]).CPU1_Jump++;// = ~(sp_GSRAM_CPU2[ui_i]).CPU2_Jump;
			PointerCount += (ul_SumCount / 2);
		}

		if(ul_DataCount < ul_SumCount)break;
		else
		{
			ul_DataCount -= ul_SumCount;
			if(ul_DataCount % ul_SumCount)break;
		}
	}

	return(CLEAR); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_Ethernet_COMMANDRead(volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1,
		volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2, Uint16 *buf, Uint32 len)
{
	Uint16 ui_i = 0;
	Uint16 ui_Channel = 0;
	Uint32 ul_DataCount = 0;
	Uint32 ul_SumCount	= 0;
	Uint32 PointerCount = 0;

	ul_DataCount = len; //8BIT ������ Ethernet���� ���ŵ� BigEndian �ڷ���� ����

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4���� ä�ο��� �ش� �Ǵ� Channel�� Ȯ���ϰ� �ش� SETP DATA�� �ڷ� ����

		ul_SumCount  = buf[1 + PointerCount]; //���� DATA �� ���� ���� ����
		ui_Channel 	 = buf[2 + PointerCount]; //��� ���� ����

		if(HEADER_COMMAND == buf[PointerCount])
		if(ui_Channel & (0x1 << ui_i))// && (sp_GSRAM_CPU1[ui_i]).CPU1_Start == (sp_GSRAM_CPU2[ui_i]).CPU2_Start)
		{
			memcpy_fast_far(&(sp_GSRAM_CPU1[ui_i]).CPU1_COMMAND.Header, buf + PointerCount, (ul_SumCount / 2));
			(sp_GSRAM_CPU1[ui_i]).CPU1_Command++;
			PointerCount += (ul_SumCount / 2);
		}

		if(ul_DataCount < ul_SumCount)break;
		else
		{
			ul_DataCount -= ul_SumCount;
			if(ul_DataCount % ul_SumCount)break;
		}
	}

	return(CLEAR); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_Ethernet_STARTRead(volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1,
		volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2, Uint16 *buf, Uint32 len)
{
	Uint16 ui_i = 0;
	Uint16 ui_Channel = 0;
	Uint32 ul_DataCount = 0;
	Uint32 ul_SumCount	= 0;
	Uint32 PointerCount = 0;

	ul_DataCount = len; //8BIT ������ Ethernet���� ���ŵ� BigEndian �ڷ���� ����

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4���� ä�ο��� �ش� �Ǵ� Channel�� Ȯ���ϰ� �ش� SETP DATA�� �ڷ� ����

		ul_SumCount  = buf[1 + PointerCount]; //���� DATA �� ���� ���� ����
		ui_Channel 	 = buf[2 + PointerCount]; //��� ���� ����

		if(HEADER_START == buf[PointerCount])
		if(ui_Channel & (0x1 << ui_i))// && (sp_GSRAM_CPU1[ui_i]).CPU1_Start == (sp_GSRAM_CPU2[ui_i]).CPU2_Start)
		{
			memcpy_fast_far(&(sp_GSRAM_CPU1[ui_i]).CPU1_START.Header, buf + PointerCount, (ul_SumCount / 2));
			(sp_GSRAM_CPU1[ui_i]).CPU1_Start++;// = ~(sp_GSRAM_CPU2[ui_i]).CPU2_Start;
			PointerCount += (ul_SumCount / 2);
		}

		if(ul_DataCount < ul_SumCount)break;
		else
		{
			ul_DataCount -= ul_SumCount;
			if(ul_DataCount % ul_SumCount)break;
		}
	}

	return(CLEAR); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_Ethernet_PAUSERead(volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1,
		volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2, Uint16 *buf, Uint32 len)
{
	Uint16 ui_i = 0;
	Uint16 ui_Channel = 0;
	Uint32 ul_DataCount = 0;
	Uint32 ul_SumCount	= 0;
	Uint32 PointerCount = 0;

	ul_DataCount = len; //8BIT ������ Ethernet���� ���ŵ� BigEndian �ڷ���� ����

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4���� ä�ο��� �ش� �Ǵ� Channel�� Ȯ���ϰ� �ش� SETP DATA�� �ڷ� ����

		ul_SumCount  = buf[1 + PointerCount]; //���� DATA �� ���� ���� ����
		ui_Channel 	 = buf[2 + PointerCount]; //��� ���� ����

		if(HEADER_PAUSE == buf[PointerCount])
		if(ui_Channel & (0x1 << ui_i))// && (sp_GSRAM_CPU1[ui_i]).CPU1_Start == (sp_GSRAM_CPU2[ui_i]).CPU2_Start)
		{
			memcpy_fast_far(&(sp_GSRAM_CPU1[ui_i]).CPU1_PAUSE.Header, buf + PointerCount, (ul_SumCount / 2));
			(sp_GSRAM_CPU1[ui_i]).CPU1_Pause++;
			PointerCount += (ul_SumCount / 2);
		}

		if(ul_DataCount < ul_SumCount)break;
		else
		{
			ul_DataCount -= ul_SumCount;
			if(ul_DataCount % ul_SumCount)break;
		}
	}

	return(CLEAR); //SUCCESS Or FAIL
}

Uint32 COMMUNICATION_Ethernet_DATAWrite(volatile S_COMMUNICATION_DRAM **sp_IF, Uint16 *buf)
{
//	Uint16 ui_Channel = 0;									//������ ����ؼ� �׻� ������ ��.
	Uint16 ui_i = 0;										//ī���ʹ� �󸶳� ���� ����غ��� �� �� ����.
	Uint32 BufferCount = 0;									//���ͷ�Ʈ�� �־ ��� �ؾ� �� �� ����.
	Uint32 Index = 0;
	Uint32 Count  = 0;

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4���� ä�ο��� �ش� �Ǵ� Channel�� Ȯ���ϰ� �ش� SETP DATA�� �ڷ� ����
		Count = RINGBUFFER_GetSize((S_RINGBUFFER_FILE *)&((*sp_IF)[ui_i].RING_DATA), COMMUNICATION_TX_DATA_LENGTH);//����Ÿ �������� ī��
//############Test������ if���� 1�־� ������ �̰� ����ž� ��.
		if(Count || 1)// && (*sp_IF)[ui_i].WriteFlagData == SET)//����Ÿ�� �ִ��� Ȯ��
		{
			Index = RINGBUFFER_Get((S_RINGBUFFER_FILE *)&((*sp_IF)[ui_i].RING_DATA), COMMUNICATION_TX_DATA_LENGTH);    //����Ÿ �������� ī��
			Index = 0;//������ ����
			(*sp_IF)[ui_i].DATA[Index].Header = HEADER_DATA;
			(*sp_IF)[ui_i].DATA[Index].DataCount = (COMMUNICATION_INTEGER_LENGTH_DATA * 2);
			(*sp_IF)[ui_i].DATA[Index].ChannelIndex = 1 << ui_i;

			memcpy_fast_far(buf + BufferCount, &((*sp_IF)[ui_i].DATA[Index].Header), (COMMUNICATION_INTEGER_LENGTH_DATA));
			BufferCount += COMMUNICATION_INTEGER_LENGTH_DATA;
			//(*sp_IF)[ui_i].WriteFlagData = CLEAR; // ���ͳ� �۽� �÷���
		}
	}
	return(BufferCount * 2); //SUCCESS Or FAIL
}

Uint32 COMMUNICATION_Ethernet_ENDWrite(volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1,
		volatile  S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2, Uint16 *buf)
{
	Uint16 ui_Channel = 0;									//������ ����ؼ� �׻� ������ ��.
	Uint16 ui_i = 0;										//ī���ʹ� �󸶳� ���� ����غ��� �� �� ����.
	Uint32 BufferCount = 0;									//���ͷ�Ʈ�� �־ ��� �ؾ� �� �� ����.

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{
		ui_Channel |= (sp_GSRAM_CPU2[ui_i]).CPU2_END.ChannelIndex;//�÷��׺� ������ �����Ѵ�. ******
	}

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4���� ä�ο��� �ش� �Ǵ� Channel�� Ȯ���ϰ� �ش� SETP DATA�� �ڷ� ����
		if(ui_Channel & (0x1 << ui_i) && (sp_GSRAM_CPU1[ui_i]).CPU1_End != (sp_GSRAM_CPU2[ui_i]).CPU2_End)// (if))//����Ÿ�� �ִ��� Ȯ���ؾ���. �ϴ� ����
		{
			memcpy_fast_far(buf + BufferCount, &((sp_GSRAM_CPU2[ui_i]).CPU2_END.Header), COMMUNICATION_INTEGER_LENGTH_END);
			BufferCount += COMMUNICATION_INTEGER_LENGTH_END;
			(sp_GSRAM_CPU1[ui_i]).CPU1_End = (sp_GSRAM_CPU2[ui_i]).CPU2_End; // ���ͳ� �۽� �÷���
		}
	}
	return(BufferCount * 2); //SUCCESS Or FAIL �̴��� (8bit) ���� ��
}

Uint32 COMMUNICATION_Ethernet_ALARMWrite(volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1,
		volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2, Uint16 *buf)
{
	Uint16 ui_Channel = 0;									//������ ����ؼ� �׻� ������ ��.
	Uint16 ui_i = 0;										//ī���ʹ� �󸶳� ���� ����غ��� �� �� ����.
	Uint32 BufferCount = 0;									//���ͷ�Ʈ�� �־ ��� �ؾ� �� �� ����.

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{
		ui_Channel |= (sp_GSRAM_CPU2[ui_i]).CPU2_ALARM.ChannelIndex;//�÷��׺� ������ �����Ѵ�. ******
	}

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4���� ä�ο��� �ش� �Ǵ� Channel�� Ȯ���ϰ� �ش� SETP DATA�� �ڷ� ����
		if(ui_Channel & (0x1 << ui_i) && (sp_GSRAM_CPU1[ui_i]).CPU1_Alarm != (sp_GSRAM_CPU2[ui_i]).CPU2_Alarm)// (if))//����Ÿ�� �ִ��� Ȯ���ؾ���. �ϴ� ����
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
	//CPU2���� index���� �Ͽ� �÷��׸� �ָ� �ش� �Ǵ� ����Ÿ�� �Ѱܾ��Ѵ�.
	//Start ��ȣ�� ���� �׶� ���� ����Ÿ�� �ѱ��.
	//End ��ȣ�� ���� �׶� ���� ����Ÿ�� �ѱ��� �ʴ´�.

	Uint16 ui_Channel = 0;
	Uint16 ui_i = 0;
	Uint16 ui_Index = 0;
	Uint16 ui_IndexStage1 = 0;
	Uint16 ui_IndexStage2 = 0;

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{
		ui_Channel |= (*sp_IF)[ui_i].STEP.ChannelIndex;//�÷��׺� ������ �����Ѵ�. ******
	}

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4���� ä�ο��� �ش� �Ǵ� Channel�� Ȯ���ϰ� �ش� SETP DATA�� �ڷ� ����

		//if((sp_GSRAM_CPU2[ui_i]).CPU2_StepIndex != (sp_GSRAM_CPU1[ui_i]).CPU1_StepIndex)//���� Ŀ�ǵ� ��� ���� ��� ���� �Ҽ� �ְ� ���� ��!!!!!
		if(ui_Channel & (0x1 << ui_i))// && (sp_GSRAM_CPU1[ui_i]).CPU1_Step == (sp_GSRAM_CPU2[ui_i]).CPU2_Step)// (if))//����Ÿ�� �ִ��� Ȯ���ؾ���. �ϴ� ����
		{
			(sp_GSRAM_CPU1[ui_i]).CPU1_StepHeader = (*sp_IF)[ui_i].STEP.Header;
			(sp_GSRAM_CPU1[ui_i]).CPU1_StepDataCount = (*sp_IF)[ui_i].STEP.DataCount;
			(sp_GSRAM_CPU1[ui_i]).CPU1_StepChannelIndex = (*sp_IF)[ui_i].STEP.ChannelIndex;

			ui_Index = (sp_GSRAM_CPU2[ui_i]).CPU2_StepIndex;// �ҷ��� ���� �ּ�

			if(ui_Index == 0)//Index = 0 �϶�
			{
				ui_IndexStage1 = 0;
				ui_IndexStage2 = 1;
			}
			else if((sp_GSRAM_CPU1[ui_i]).CPU1_JUMP.STEP_INDEX)// Jump ����� ��� ���� ���.
			{
				ui_Index = (sp_GSRAM_CPU1[ui_i]).CPU1_JUMP.STEP_INDEX;
				(sp_GSRAM_CPU1[ui_i]).CPU1_JUMP.STEP_INDEX = CLEAR;
			}
			else
			{
				ui_IndexStage1 = ui_Index;
				ui_IndexStage2 = ui_Index;
			}

			//First Step Level
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[0 ] = (*sp_IF)[ui_i].STEP.STEP[ui_IndexStage1];
			//Second Step Level
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[1 ] = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[ui_IndexStage2].END_TIME_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[2]  = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[ui_IndexStage2].END_CURRENT_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[3]  = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[ui_IndexStage2].END_CAPACITY_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[4]  = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[ui_IndexStage2].END_WATT_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[5]  = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[ui_IndexStage2].END_VOLTAGE_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[6]  = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[ui_IndexStage2].GOTO_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_STEP[7]  = (*sp_IF)[ui_i].STEP.STEP[((sp_GSRAM_CPU1[ui_i]).CPU1_STEP[ui_IndexStage2].RETURN_INDEX)];
			(sp_GSRAM_CPU1[ui_i]).CPU1_Step++;// = ~(sp_GSRAM_CPU1[ui_i]).CPU1_Step;//������Ʈ ����Ÿ �Ѵ�.
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
		ui_Channel |= ((sp_GSRAM_CPU2[ui_i]).CPU2_Data != (sp_GSRAM_CPU1[ui_i]).CPU1_Data) << ui_i;//�÷��׺� ������ �����Ѵ�. ******
	}

	for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
	{	//4���� ä�ο��� �ش� �Ǵ� Channel�� Ȯ���ϰ� �ش� SETP DATA�� �ڷ� ����
		if(ui_Channel & (0x1 << ui_i))// (if))//����Ÿ�� �ִ��� Ȯ���ؾ���. �ϴ� ����
		{
			ul_Index = RINGBUFFER_Put((S_RINGBUFFER_FILE *)&(*sp_IF)[ui_i].RING_DATA, COMMUNICATION_TX_DATA_LENGTH);
			ul_Index = 0;
			memcpy_fast_far(&((*sp_IF)[ui_i].DATA[ul_Index].Header), &((sp_GSRAM_CPU2[ui_i]).CPU2_DATA.Header), COMMUNICATION_INTEGER_LENGTH_DATA);

			(sp_GSRAM_CPU1[ui_i]).CPU1_Data = (sp_GSRAM_CPU2[ui_i]).CPU2_Data;
//			(*sp_IF)[ui_i].WriteFlagData = SET;  // �̴��� �۽� �÷��� ON
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

#pragma CODE_SECTION(COMMUNICATION_RS422A_TxRunSync, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_RS422A_TxDataCall, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_RS422A_TxStepSend, ".TI.ramfunc");
#pragma CODE_SECTION(COMMUNICATION_RS422A_RxUnPacket, ".TI.ramfunc");

//#pragma CODE_SECTION(, ".TI.ramfunc");
//#pragma CODE_SECTION(, ".TI.ramfunc");

Uint16 COMMUNICATION_RS422A_TxRunSync(volatile Uint16 *TxBuff, volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1, volatile Uint16 Channel)
{
	Uint16 StartFlag[DSP_SCIA_HEADER_LENGTH] ={DSP_SCIA_RUN_SYNC_FLAG, DSP_SCIA_RUN_SYNC_FLAG, DSP_SCIA_RUN_SYNC_FLAG, 0x0000, 0x0000};
	Uint16 Counter = 0;
	Uint16 CRC16 = 0;

	StartFlag[DSP_SCIA_START_FLAG_LENGTH] =
			((Channel << 8) & 0xFF00) | (Channel & 0x00FF);

	StartFlag[DSP_SCIA_START_FLAG_LENGTH + 1] =
			((Channel << 8) & 0xFF00) | (Channel & 0x00FF);


	memcpy_fast_far(TxBuff, StartFlag, DSP_SCIA_HEADER_LENGTH);
	Counter += DSP_SCIA_HEADER_LENGTH;

	CRC16 = CRC16_Conversion(TxBuff, DSP_SCIA_RUN_SYNC_LENGTH - DSP_SCIA_CRC16_LENGTH);

	memcpy_fast_far(TxBuff + Counter, &CRC16, DSP_SCIA_CRC16_LENGTH);
	Counter += DSP_SCIA_CRC16_LENGTH;

	DSP_SCIA_Write(TxBuff, Counter);
	Counter  = 0;

	return(SUCCESS); //SUCCESS Or FAIL

}

Uint16 COMMUNICATION_RS422A_TxDataCall(volatile Uint16 *TxBuff, volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1, volatile Uint16 Channel)
{
	Uint16 StartFlag[DSP_SCIA_HEADER_LENGTH] ={DSP_SCIA_DATA_CALL_FLAG, DSP_SCIA_DATA_CALL_FLAG, DSP_SCIA_DATA_CALL_FLAG, 0x0000, 0x0000};
	Uint16 Counter = 0;
	Uint16 CRC16 = 0;

	StartFlag[DSP_SCIA_START_FLAG_LENGTH] =
			((Channel << 8) & 0xFF00) | (Channel & 0x00FF);

	StartFlag[DSP_SCIA_START_FLAG_LENGTH + 1] =
			((Channel << 8) & 0xFF00) | (Channel & 0x00FF);


	memcpy_fast_far(TxBuff, StartFlag, DSP_SCIA_HEADER_LENGTH);
	Counter += DSP_SCIA_HEADER_LENGTH;

	CRC16 = CRC16_Conversion(TxBuff, DSP_SCIA_DATA_CALL_LENGTH - DSP_SCIA_CRC16_LENGTH);

	memcpy_fast_far(TxBuff + Counter, &CRC16, DSP_SCIA_CRC16_LENGTH);
	Counter += DSP_SCIA_CRC16_LENGTH;

	DSP_SCIA_Write(TxBuff, Counter);
	Counter  = 0;

	return(SUCCESS); //SUCCESS Or FAIL
}
Uint16 COMMUNICATION_RS422A_TxStepSend(volatile Uint16 *TxBuff, volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1, volatile Uint16 Channel)
{
	Uint16 StartFlag[DSP_SCIA_HEADER_LENGTH] ={DSP_SCIA_STEP_SEND_FLAG, DSP_SCIA_STEP_SEND_FLAG, DSP_SCIA_STEP_SEND_FLAG, 0x0000, 0x0000};
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

	//Bank = //�����۷� ��ũ ī��Ʈ ����.
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

volatile S_COMMUNICATION_TX s_DATA;
volatile S_COMMUNICATION_RX_JUMP s_JUMP;
volatile S_COMMUNICATION_RX_COMMAD s_COMMAND;
volatile S_COMMUNICATION_RX_START s_START;
volatile S_COMMUNICATION_RX_PAUSE s_PAUSE;

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

Uint16 COMMUNICATION_STEPRead(S_OPERATION_FILE* sp_OP, volatile S_COMMUNICATION_GSRAM_CPU1 *spGSRAM_STPE, volatile S_COMMUNICATION_GSRAM_CPU2 *spGSRAM_DATA)
{
	Uint32 TempUint32 = 0;
	Uint32 StepIndexStage = 0;
//Todo : ���ܿ� ���� ���������� ���� ����� ������.
	if(spGSRAM_DATA[0].CPU2_Step != spGSRAM_STPE[0].CPU1_Step)//���� ���ǿ� ����
	{
		spGSRAM_STPE[0].CPU1_StepHeader;
		spGSRAM_STPE[0].CPU1_StepDataCount; //���� ī��Ʈ ����..
		spGSRAM_STPE[0].CPU1_StepChannelIndex;
		spGSRAM_STPE[0].CPU1_StepIndex;



		(*sp_OP).COMMAND 					=  spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].COMMAND;
		(*sp_OP).MODE 						=  spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].MODE;
		(*sp_OP).OPERATIN 					=  spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].OPERATIN;
		(*sp_OP).SAMPLING_TIME	 			=  spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].SAMPLING_TIME;
		(*sp_OP).SEND_TIME 					=  spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].SEND_TIME;
		(*sp_OP).STEP_INDEX 				=  spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].STEP_INDEX;
		(*sp_OP).CYCLE_LOOP_COUNT 			=  spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].CYCLE_LOOP_COUNT;
		(*sp_OP).GOTO_INDEX 				=  spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].GOTO_INDEX;
		(*sp_OP).RETURN_INDEX 				=  spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].RETURN_INDEX;

		TempUint32 							=  (((Uint32)spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].VOLTAGE_REFx16 << 16) & (Uint32)0xFFFF0000);
		TempUint32 							|= ((Uint32)spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].VOLTAGE_REFx00 & (Uint32)0x0000FFFF);
		(*sp_OP).VARIABLE_VoltageRef		= (float)(TempUint32) * (float)(0.001);

		(*sp_OP).END_VOLTAGE_INDEX 			=  spGSRAM_STPE[0].CPU1_STEP[0].END_VOLTAGE_INDEX;

		TempUint32	 						=  (((Uint32)spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].CURRENT_REF_16 << 16) & (Uint32)0xFFFF0000);
		TempUint32 							|= ((Uint32)spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].CURRENT_REFx00 & (Uint32)0x0000FFFF);
		(*sp_OP).VARIABLE_CurrentRef		= (float)(TempUint32) * (float)(0.001);

		(*sp_OP).END_TIME_CONDITION 		=  (((Uint32)spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].END_TIME_CONDITIONx16 << 16) & (Uint32)0xFFFF0000);
		(*sp_OP).END_TIME_CONDITION 		|= ((Uint32)spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].END_TIME_CONDITIONx00 & (Uint32)0x0000FFFF);

		(*sp_OP).PATTERN_PRIOD 				=  spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].PATTERN_PRIOD;
		(*sp_OP).END_TIME_INDEX 			=  spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].END_TIME_INDEX;

		TempUint32 							=  (((Uint32)spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].END_CURRENT_CONDITIONx16 << 16) & (Uint32)0xFFFF0000);
		TempUint32 							|= ((Uint32)spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].END_CURRENT_CONDITIONx00 & 0x0000FFFF);
		(*sp_OP).END_CURRENT_CONDITION	 	=  (float)(TempUint32) * (float)(0.001);

		(*sp_OP).END_CURRENT_INDEX 			=  spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].END_CURRENT_INDEX;

		TempUint32 							=  (((Uint32)spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].END_CAPACITY_CONDITIONx16 << 16) & (Uint32)0xFFFF0000);
		TempUint32 							|= ((Uint32)spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].END_CAPACITY_CONDITIONx00 & (Uint32)0x0000FFFF);
		(*sp_OP).END_CAPACITY_CONDITION 	=  (float)(TempUint32) * (float)(0.001);

		(*sp_OP).END_CAPACITY_INDEX 		=  spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].END_CAPACITY_INDEX;

		TempUint32 							=  (((Uint32)spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].END_WATT_CONDITIONx16 << 16) & (Uint32)0xFFFF0000);
		TempUint32 							|= ((Uint32)spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].END_WATT_CONDITIONx00 & (Uint32)0x0000FFFF);
		(*sp_OP).END_WATT_CONDITION 		=  (float)(TempUint32) * (float)(0.001);

		(*sp_OP).END_WATT_INDEX 			=  spGSRAM_STPE[0].CPU1_STEP[StepIndexStage].END_WATT_INDEX;

		spGSRAM_DATA[0].CPU2_Step = spGSRAM_STPE[0].CPU1_Step;
	}

	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_JUMPRead(S_OPERATION_FILE* sp_OP, volatile S_COMMUNICATION_GSRAM_CPU1 *spGSRAM_STPE, volatile S_COMMUNICATION_GSRAM_CPU2 *spGSRAM_DATA)
{
	if((*spGSRAM_DATA).CPU2_Jump != (*spGSRAM_STPE).CPU1_Jump)
	{
		(*spGSRAM_DATA).CPU2_StepIndex = (*spGSRAM_STPE).CPU1_JUMP.STEP_INDEX; //¬Ǫ�� �δ콺�� �ҷ��ͼ� �������ּ���.
											//��� ���� 0�̿� 0 �ּҿ� ��� ���� �־� �ֽÿ�.
		(*spGSRAM_DATA).CPU2_Jump = (*spGSRAM_STPE).CPU1_Jump;
	}
	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_COMMANDRead(S_OPERATION_FILE* sp_OP, volatile S_COMMUNICATION_GSRAM_CPU1 *spGSRAM_STPE, volatile S_COMMUNICATION_GSRAM_CPU2 *spGSRAM_DATA)
{
	if((*spGSRAM_DATA).CPU2_Command != (*spGSRAM_STPE).CPU1_Command)
	{
		(*spGSRAM_DATA).CPU2_StepIndex = 0; //��ְ��� �ְ� Ŀ�ǵ带 �����Ͻÿ�. Ŀ�ǵ�� ���� ���� �� ����. ������ �¿��� ����. �ҷ��ͼ� �������ּ���.
//		(*spSTEP).COMMAND = (*spGSRAM_STPE).CPU1_COMMAND.COMMAD;	//��� ���� 0�̿� 0 �ּҿ� ��� ���� �־� �ֽÿ�.
		(*spGSRAM_DATA).CPU2_Command = (*spGSRAM_STPE).CPU1_Command;
	}

	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_STARTRead(S_OPERATION_FILE* sp_OP, volatile S_COMMUNICATION_GSRAM_CPU1 *spGSRAM_STPE, volatile S_COMMUNICATION_GSRAM_CPU2 *spGSRAM_DATA)
{
	if((*spGSRAM_DATA).CPU2_Start != (*spGSRAM_STPE).CPU1_Start)
	{
		(*spGSRAM_DATA).CPU2_StepIndex = 1; 				//���� 1�� �ҷ��ͼ� �������ּ���.
		CPUTimer_stopTimer(CPUTIMER0_BASE);
		CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);
//		(*spSENSING_DATA).TIME_SPAN = CLEAR;

		CPUTimer_startTimer(CPUTIMER0_BASE);
		(*spGSRAM_DATA).CPU2_Start = (*spGSRAM_STPE).CPU1_Start;//��� ���� 0�̿� 0 �ּҿ� ��� ���� �־� �ֽÿ�.
	}																//DATA �� ī���� ��� �ʱ�ȭ ���ֱ� �ٶ��.


	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_PAUSERead(S_OPERATION_FILE* sp_OP, volatile S_COMMUNICATION_GSRAM_CPU1 *spGSRAM_STPE, volatile S_COMMUNICATION_GSRAM_CPU2 *spGSRAM_DATA)
{
	static Uint16 OldStepIndex = 0;

	if((*spGSRAM_DATA).CPU2_Pause != (*spGSRAM_STPE).CPU1_Pause)
	{

		switch((*spGSRAM_STPE).CPU1_PAUSE.COMMAND)
		{
		case OPERATION_PAUSE_STOP:
			(*spGSRAM_DATA).CPU2_StepIndex = OldStepIndex; //���� ������ ��ȯ��
			break;
		case OPERATION_PAUSE_PAUSE:
//			OldStepIndex = (*spSTEP).STEP_INDEX;
			(*spGSRAM_DATA).CPU2_StepIndex = 0; //��� ���� �ҷ����ÿ�
			break;
		case OPERATION_PAUSE_COMPLETE:
			(*spGSRAM_DATA).CPU2_StepIndex = 0; //��� ���� �ҷ����ÿ� �����ΰ� ����
												//�̵��� ���� �ʱ�ȭ �ؾ� �� �� ������ ���̾�..
			break;
		}
		(*spGSRAM_DATA).CPU2_Pause = (*spGSRAM_STPE).CPU1_Pause;
	}
	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_DATAWrite(volatile S_COMMUNICATION_GSRAM_CPU1 *spGSRAM_STPE, volatile S_COMMUNICATION_TX *spDATA, S_OPERATION_FILE *sp_OP)
{
	//������ ������Ʈ �Ǹ� �ѹ� ���� �ǰ� ������Ʈ �սô�.
	//�׸��� �̰� �Ѱ��� ����Ÿ�� ���� �Ǵ� ��ũ�� ����Ͽ��� �մϴ�.
	//Ÿ�̸� ��ƾ�� ���� �մϴ�.
	//�ȱ׷� �ȵǿ� �������� Ÿ�� ���ҵ� ���⿡ �����ô�.
	//1ms 100�� ����Ÿ ����ô�.
	Uint32 Temp32 = 0;
	Uint64 Temp64 = 0;

	(*spDATA).SAMPLING_TIME = (*sp_OP).SAMPLING_TIME;
	(*spDATA).STEP_INDEX = (*sp_OP).STEP_INDEX;
	(*spDATA).COMMAND = (*sp_OP).COMMAND;
	(*spDATA).END_CONDITION_STATE;
	(*spDATA).OPERATIN = (*sp_OP).VARIABLE_OPERATIN;
	(*spDATA).MODE = (*sp_OP).MODE;

	Temp32 = (Uint32)(ABS_F((*sp_OP).VARIABLE_VoltageResult) * 1000.0);
	(*spDATA).VOLTAGEx16 = (Uint16)((Temp32 >> 16) & 0x0000FFFF);
	(*spDATA).VOLTAGEx00 = (Uint16)((Temp32      ) & 0x0000FFFF);

	Temp32 = (Uint32)(ABS_F((*sp_OP).VARIABLE_CurrentResult) * 1000.0);
	(*spDATA).CURRENTx16 = (Uint16)((Temp32 >> 16) & 0x0000FFFF);
	(*spDATA).CURRENTx00 = (Uint16)((Temp32      ) & 0x0000FFFF);

	Temp64 = (Uint64)((*sp_OP).VARIABLE_TimeSpanResult);
	(*spDATA).TIME_SPANx64= (Uint16)((Temp64 >> 48) & 0x000000000000FFFF);
	(*spDATA).TIME_SPANx32= (Uint16)((Temp64 >> 32) & 0x000000000000FFFF);
	(*spDATA).TIME_SPANx16= (Uint16)((Temp64 >> 16) & 0x000000000000FFFF);
	(*spDATA).TIME_SPANx00= (Uint16)((Temp64      ) & 0x000000000000FFFF);

	Temp32 = (Uint32)((*sp_OP).VARIABLE_EndConditionResult);//�̰� ����ü ���淡 �ִ°��� ���� ����
	(*spDATA).END_CONDITION_VALUEx16= (Uint16)((Temp32 >> 16) & 0x0000FFFF);
	(*spDATA).END_CONDITION_VALUEx00= (Uint16)((Temp32      ) & 0x0000FFFF);

	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_ENDWrite(volatile S_COMMUNICATION_GSRAM_CPU1 *spGSRAM_STPE, volatile S_COMMUNICATION_GSRAM_CPU2 *spGSRAM_DATA, S_OPERATION_FILE* sp_OP)
{
	static Uint16 UpDateFlag = CLEAR;

	if((*sp_OP).VARIABLE_STEP_INDEX == 0 && UpDateFlag == CLEAR)
	{
		UpDateFlag = SET;
		(*spGSRAM_DATA).CPU2_END.Header = HEADER_END;
		(*spGSRAM_DATA).CPU2_END.ChannelIndex = (1 << COMMUNICATION_CHANNEL_NUMBER);
		(*spGSRAM_DATA).CPU2_END.DataCount = COMMUNICATION_INTEGER_LENGTH_END * 2;

		(*spGSRAM_DATA).CPU2_StepIndex = 0; //������ 0�� ���ֽÿ�. �ҷ��ͼ� �������ּ���.
												//��� ���� 0�̿� 0 �ּҿ� ��� ���� �־� �ֽÿ�.
		(*spGSRAM_DATA).CPU2_End++;
	}
	else if((*sp_OP).VARIABLE_STEP_INDEX != 0)
	{
		UpDateFlag = CLEAR;
	}
	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 COMMUNICATION_ALARMWrite(volatile S_COMMUNICATION_GSRAM_CPU1 *spGSRAM_STPE, volatile S_COMMUNICATION_GSRAM_CPU2 *spGSRAM_DATA, S_OPERATION_FILE* sp_OP)
{
	static Uint16 UpDateFlag = CLEAR;

	if((*sp_OP).VARIABLE_ALARM_Flag == SET && UpDateFlag == CLEAR)
	{
		UpDateFlag = SET;
		(*spGSRAM_DATA).CPU2_ALARM.Header = HEADER_ALARM;
		(*spGSRAM_DATA).CPU2_ALARM.ChannelIndex = (1 << COMMUNICATION_CHANNEL_NUMBER);
		(*spGSRAM_DATA).CPU2_ALARM.DataCount = COMMUNICATION_INTEGER_LENGTH_ALARM * 2;

		(*spGSRAM_DATA).CPU2_ALARM.WARING.all 	=  (*sp_OP).VARIABLE_ALARM_WARING.all;
		(*spGSRAM_DATA).CPU2_ALARM.GRID.all 	=  (*sp_OP).VARIABLE_ALARM_GRID.all;
		(*spGSRAM_DATA).CPU2_ALARM.DC.all 		=  (*sp_OP).VARIABLE_ALARM_DC.all;
		(*spGSRAM_DATA).CPU2_ALARM.FAULT.all 	=  (*sp_OP).VARIABLE_ALARM_FAULT.all;

		(*spGSRAM_DATA).CPU2_StepIndex = 0; //������ 0�� ���ֽÿ�. �ҷ��ͼ� �������ּ���.
												//��� ���� 0�̿� 0 �ּҿ� ��� ���� �־� �ֽÿ�.
		(*spGSRAM_DATA).CPU2_Alarm++;
	}
	else if((*sp_OP).VARIABLE_ALARM_Flag != SET)
	{
		UpDateFlag = CLEAR;
	}
	return(SUCCESS); //SUCCESS Or FAIL
}
#endif //End #ifdef DEVICE_CPU2

//
// End of file
//

