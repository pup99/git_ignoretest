/*
 * PhoenixonControls_SCI.c
 *
 *  Created on: 2018. 5. 20.
 *      Author: BGKim
 */
#include "../../../common/include/F28x_Project.h"
#include "../../../common/include/driverlib.h"
#include "../../../common/include/device.h"

#pragma CODE_SECTION(DSP_SCIA_Put, ".TI.ramfunc");
#pragma CODE_SECTION(DSP_SCIA_Write, ".TI.ramfunc");
#pragma CODE_SECTION(DSP_SCIA_Get, ".TI.ramfunc");
#pragma CODE_SECTION(DSP_SCIA_Read, ".TI.ramfunc");
#pragma CODE_SECTION(DSP_SCIA_TxStart, ".TI.ramfunc");
#pragma CODE_SECTION(DSP_SCIABufferClear, ".TI.ramfunc");
#pragma CODE_SECTION(DSP_SCIABufferGetSize, ".TI.ramfunc");
#pragma CODE_SECTION(DSP_SCIABufferPut, ".TI.ramfunc");
#pragma CODE_SECTION(DSP_SCIABufferGet, ".TI.ramfunc");
#pragma CODE_SECTION(DSP_SCIA_RX_ISR, ".TI.ramfunc");
#pragma CODE_SECTION(DSP_SCIA_TX_ISR, ".TI.ramfunc");

volatile Uint16 SCIARxCount = 0;
volatile Uint16 SCIATxCount = 0;
volatile Uint16 SCIARxChannel = 0;
volatile Uint16 SCIATxChannel = 0;
volatile Uint16 SCIATxData[DSP_SCIA_TX_MAX_DATA_LENGTH] 	= {0,}; //Int 형으로 데이타 버퍼임.
volatile Uint16 SCIARxData[DSP_SCIA_RX_MAX_DATA_LENGTH] 	= {0,};
extern volatile Uint16 SCIATxBuff[];
extern volatile Uint16 SCIARxBuff[];
volatile S_DSP_SCIA_RINGBUFFER_FILE SCIARxRing 				= {0,0,0};
volatile S_DSP_SCIA_RINGBUFFER_FILE SCIATxRing 				= {0,0,0};
volatile E_DSP_SCIA_FLAG e_SCIA_RxState 					= SCIA_INIT;
volatile E_DSP_SCIA_FLAG e_SCIA_TxState 					= SCIA_INIT;

#ifdef _DEBUG
Uint16 DEBUG_RX_LENGTH = 0;
Uint16 DEBUG_RX_COUNT = 0;
Uint16 DEBUG_RX_CRC16 = 0;
#endif//_DEBUG END

void DSP_SCIA_Init(void)
{
//    //
//    // GPIO9 is the SCI Rx pin.
//    //
//    GPIO_setMasterCore(9, GPIO_CORE_CPU1);
//    GPIO_setPinConfig(GPIO_9_SCIRXDA);
//    GPIO_setDirectionMode(9, GPIO_DIR_MODE_IN);
//    GPIO_setPadConfig(9, GPIO_PIN_TYPE_STD);
//    GPIO_setQualificationMode(9, GPIO_QUAL_ASYNC);
//
//    //
//    // GPIO8 is the SCI Tx pin.
//    //
//    GPIO_setMasterCore(8, GPIO_CORE_CPU1);
//    GPIO_setPinConfig(GPIO_8_SCITXDA);
//    GPIO_setDirectionMode(8, GPIO_DIR_MODE_OUT);
//    GPIO_setPadConfig(8, GPIO_PIN_TYPE_STD);
//    GPIO_setQualificationMode(8, GPIO_QUAL_ASYNC);

    //
    // 8 char bits, 1 stop bit, no parity. Baud rate is 9600.
    //
    SCI_setConfig(SCIA_BASE, DEVICE_LSPCLK_FREQ, DSP_SCIA_BAUD, (SCI_CONFIG_WLEN_8 |
                                                        SCI_CONFIG_STOP_ONE |
                                                        SCI_CONFIG_PAR_NONE));
    SCI_enableModule(SCIA_BASE);
//    SCI_enableLoopback(SCIA_BASE);
    SCI_disableLoopback(SCIA_BASE);
    SCI_resetChannels(SCIA_BASE);
//    SCI_enableFIFO(SCIA_BASE);

    //
    // RX and TX FIFO Interrupts Enabled
    //
//    SCI_enableInterrupt(SCIA_BASE, (SCI_INT_RXFF | SCI_INT_TXFF));
    SCI_enableInterrupt(SCIA_BASE, SCI_INT_RXRDY_BRKDT);
    SCI_disableInterrupt(SCIA_BASE, SCI_INT_RXERR);

//    SCI_setFIFOInterruptLevel(SCIA_BASE, SCI_FIFO_TX0, SCI_FIFO_RX1);
    SCI_performSoftwareReset(SCIA_BASE);

//    SCI_resetTxFIFO(SCIA_BASE);
//    SCI_resetRxFIFO(SCIA_BASE);

}
void DSP_SCIA_TX_Enable(void)
{
	GpioDataRegs.GPBSET.bit.GPIO55 = 1; //cpu 1 설정.
	asm(" RPT #40 || NOP");
}
void DSP_SCIA_TX_Disable(void)
{
	GpioDataRegs.GPBCLEAR.bit.GPIO55 = 1;
	asm(" RPT #40 || NOP");
}
void DSP_SCIA_Put(volatile Uint16 Data)
{
    while(!SciaRegs.SCICTL2.bit.TXEMPTY);
    SciaRegs.SCITXBUF.all = Data;
    while(!SciaRegs.SCICTL2.bit.TXEMPTY);
}

void DSP_SCIA_Write(volatile Uint16 *Data, Uint32 Length)
{
	Uint32 ul_i = 0;
	Uint16 Temp = 0;

	DSP_SCIA_TX_Enable();

	for(ul_i = 0; ul_i < Length; ul_i++)
	{
		Temp = Data[ul_i];;
		DSP_SCIA_Put(((Temp >> 8) & 0x00FF));
		DSP_SCIA_Put(((Temp     ) & 0x00FF));
	}

	DSP_SCIA_TX_Disable();
	//SCI_writeCharArray(SCIA_BASE, (Uint16*)Data, Length);
}

Uint16 DSP_SCIA_Get(void)
{
//    if(SciaRegs.SCIFFRX.bit.RXFFST == 0)return 0;
	return (Uint16)(SciaRegs.SCIRXBUF.all);
}

void DSP_SCIA_Read(Uint16 *Data, Uint32 Length)
{
	Uint32 ui_i = 0;

	for(ui_i = 0; ui_i < Length; ui_i++)
		Data[ui_i] = SCI_readCharBlockingFIFO(SCIA_BASE);
}

void DSP_SCIA_TxStart(void)
{
    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXFF);
}

void DSP_SCIABufferClear(S_DSP_SCIA_RINGBUFFER_FILE *s_File)
{
	(*s_File).Head = CLEAR;
	(*s_File).Tail = CLEAR;
}

Uint32 DSP_SCIABufferGetSize(S_DSP_SCIA_RINGBUFFER_FILE *s_File, Uint32 MaxBufferLength)
{
	(*s_File).Count = ((*s_File).Head - (*s_File).Tail + MaxBufferLength) % MaxBufferLength;
	return (Uint32)((*s_File).Count);
}

Uint32 DSP_SCIABufferPut(S_DSP_SCIA_RINGBUFFER_FILE *s_File, Uint32 MaxBufferLength)
{
	Uint32 Temp = (*s_File).Head;

	if(DSP_SCIABufferGetSize(s_File, MaxBufferLength) == MaxBufferLength - 1) return ((*s_File).Head);//SUCCESS Or FAIL
	(*s_File).Head++;
	(*s_File).Head %= MaxBufferLength;

	return (Uint32)(Temp);//SUCCESS Or FAIL
}

Uint32 DSP_SCIABufferGet(S_DSP_SCIA_RINGBUFFER_FILE *s_File, Uint32 MaxBufferLength)
{
	Uint32 Temp = (*s_File).Tail;

	if(DSP_SCIABufferGetSize(s_File, MaxBufferLength) == 0) return ((*s_File).Tail);
	(*s_File).Tail++;
	(*s_File).Tail %= MaxBufferLength;

	return (Temp);//SUCCESS Or FAIL
}

E_DSP_SCIA_FLAG DSP_SCIA_RxUpDate(volatile Uint16 *Data, volatile Uint16 *Buff, volatile Uint16 *Count, volatile Uint16 *Channel)
{
		Uint16 ui_i = 0;
		Uint16 ui_j = 0;
		Uint16 CRC16 = 0;

		if(!((*Count) >= DSP_SCIA_RX_MAX_BUFFER_LENGTH))return(SCIA_ERROR_DATA);
		for(ui_i = 0, ui_j = 0; ui_i < (DSP_SCIA_RX_MAX_DATA_LENGTH); ui_i++)
		{
			Data[ui_i]  = ((Buff[ui_j++] << 8) & 0xFF00);
			Data[ui_i] |= ((Buff[ui_j++] << 0) & 0x00FF);
		}
		(*Count) = CLEAR;	//인터럽트 들어가려면 스킵해줘야 함.
		CRC16 = CRC16_Conversion((Uint16 *)Data, (DSP_SCIA_RX_MAX_DATA_LENGTH - DSP_SCIA_CRC16_LENGTH));
		if(CRC16 != Data[DSP_SCIA_RX_MAX_DATA_LENGTH - DSP_SCIA_CRC16_LENGTH])return(SCIA_ERROR_CRC16);
		(*Channel) = (Data[DSP_SCIA_START_FLAG_LENGTH] & 0x000F);
		if((*Channel) > COMMUNICATION_CHANNEL)return(SCIA_ERROR_CHANNEL);

		return(SCIA_SUCCESS);
}

__interrupt void DSP_SCIA_RX_ISR(void)
{
	Uint16 buff = DSP_SCIA_Get();

	if(SCIARxCount < DSP_SCIA_RX_MAX_BUFFER_LENGTH)
	{
		SCIARxBuff[SCIARxCount++] = buff;
	}

	SCI_clearOverflowStatus(SCIA_BASE);
	SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_RXFF);
	Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

__interrupt void DSP_SCIA_TX_ISR(void)
{
	SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXFF);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

//
// End of file
//

