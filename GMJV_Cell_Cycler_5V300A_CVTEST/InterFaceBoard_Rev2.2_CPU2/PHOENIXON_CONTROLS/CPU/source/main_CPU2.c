/*
 * main.c
 *
 *  Created on: 2018. 5. 16.
 *      Author: BGKim
 */

#include "../../../common/include/F28x_Project.h"
#include "../../../common/include/driverlib.h"
#include "../../../common/include/device.h"

//
// PhoenixonControls_COMMUNICATION.c
//
volatile S_COMMUNICATION_GSRAM_CPU1 s_CPU_1_GSRAM[COMMUNICATION_CHANNEL];
volatile S_COMMUNICATION_GSRAM_CPU2 s_CPU_2_GSRAM[COMMUNICATION_CHANNEL];
volatile Uint16 ui_UpdateflagCPU1 = 0;
volatile Uint16 ui_UpdateflagCPU2 = 0;

volatile Uint16 SCIATxBuff[DSP_SCIA_TX_MAX_BUFFER_LENGTH] 	= {0,};
volatile Uint16 SCIARxBuff[DSP_SCIA_RX_MAX_BUFFER_LENGTH]	= {0,};

#pragma DATA_SECTION(s_CPU_1_GSRAM,"SHARERAMGS15");
#pragma DATA_SECTION(s_CPU_2_GSRAM,"SHARERAMGS4");
#pragma DATA_SECTION(ui_UpdateflagCPU1,"SHARERAMGS15");
#pragma DATA_SECTION(ui_UpdateflagCPU2,"SHARERAMGS4");

#pragma DATA_SECTION(SCIARxBuff,"SHARERAMGS4");
#pragma DATA_SECTION(SCIATxBuff,"SHARERAMGS4");

//End

//
// PhoenixonControls_SCI.c
//
extern volatile Uint16 SCIARxCount;
extern volatile Uint16 SCIATxCount;
extern volatile Uint16 SCIARxChannel;
extern volatile Uint16 SCIATxChannel;
extern volatile Uint16 SCIATxData[]; //Int 형으로 데이타 버퍼임.
extern volatile Uint16 SCIARxData[];
extern volatile Uint16 SCIATxBuff[]; //char 형으로 통신 버퍼용임.
extern volatile Uint16 SCIARxBuff[];
extern volatile S_DSP_SCIA_RINGBUFFER_FILE SCIARxRing;
extern volatile S_DSP_SCIA_RINGBUFFER_FILE SCIATxRing;
extern volatile E_DSP_SCIA_FLAG e_SCIA_RxState;
extern volatile E_DSP_SCIA_FLAG e_SCIA_TxState;
extern volatile Uint16 ui_TimerFlag100ms;
//End

#ifdef _DEBUG
Uint16 DEBUG_S_COMMUNICATION_GSRAM_CPU1     = sizeof(S_COMMUNICATION_GSRAM_CPU1);
Uint16 DEBUG_S_COMMUNICATION_GSRAM_CPU2     = sizeof(S_COMMUNICATION_GSRAM_CPU2);
Uint16 DEBUG_DSP_SCIA_TX_DATA_LENGTH		= DSP_SCIA_TX_DATA_LENGTH;
Uint16 DEBUG_DSP_SCIA_RX_DATA_LENGTH		= DSP_SCIA_RX_DATA_LENGTH;
Uint16 DEBUG_DSP_SCIA_TX_TOTAL_LENGTH		= DSP_SCIA_TX_TOTAL_LENGTH;
Uint16 DEBUG_DSP_SCIA_RX_TOTAL_LENGTH		= DSP_SCIA_RX_TOTAL_LENGTH;
Uint16 DEBUG_DSP_SCIA_TX_DUMY_LENGTH		= DSP_SCIA_TX_DUMY_LENGTH;
Uint16 DEBUG_DSP_SCIA_RX_DUMY_LENGTH		= DSP_SCIA_RX_DUMY_LENGTH;
Uint16 DEBUG_DSP_SCIA_TX_MAX_DATA_LENGTH	= DSP_SCIA_TX_MAX_DATA_LENGTH;
Uint16 DEBUG_DSP_SCIA_RX_MAX_DATA_LENGTH 	= DSP_SCIA_RX_MAX_DATA_LENGTH;
Uint16 DEBUG_DSP_SCIA_TX_MAX_BUFFER_LENGTH 	= DSP_SCIA_TX_MAX_BUFFER_LENGTH;
Uint16 DEBUG_DSP_SCIA_RX_MAX_BUFFER_LENGTH	= DSP_SCIA_RX_MAX_BUFFER_LENGTH;
Uint32 DEBUG_SCI_ERROR_COUNT[COMMUNICATION_CHANNEL] = {0,};
#endif //_DEBUG

void main(void) {
	Uint16 ui_i = 0;
	Uint32 ul_TimeOut = 0;
	E_DSP_SCIA_FLAG e_Flag = SCIA_INIT;
    //
    // Initialize device clock and peripherals
    //
    Device_init();
    DSP_TimerInit();
    DSP_SCIA_Init();
    //
    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    //
    Interrupt_initModule();

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();

    //
    // ISRs
    //
    Interrupt_register(INT_TIMER0, &DSP_Timer0_ISR);
    Interrupt_register(INT_TIMER1, &DSP_Timer1_ISR);
    Interrupt_register(INT_TIMER2, &DSP_Timer2_ISR);
    Interrupt_register(INT_SCIA_RX, &DSP_SCIA_RX_ISR);
    Interrupt_register(INT_SCIA_TX, &DSP_SCIA_TX_ISR);
    //
    // To ensure precise timing, use write-only instructions to write to the
    // entire register. Therefore, if any of the configuration bits are changed
    // in configCPUTimer and initCPUTimers, the below settings must also
    // be updated.
    //
    CPUTimer_enableInterrupt(CPUTIMER0_BASE);
    CPUTimer_enableInterrupt(CPUTIMER1_BASE);
    CPUTimer_enableInterrupt(CPUTIMER2_BASE);

    //
    // Enables CPU int1, int13, and int14 which are connected to CPU-Timer 0,
    // CPU-Timer 1, and CPU-Timer 2 respectively.
    // Enable TINT0 in the PIE: Group 1 interrupt 7
    //
    Interrupt_enable(INT_TIMER0);
    Interrupt_enable(INT_TIMER1);
    Interrupt_enable(INT_TIMER2);
    Interrupt_enable(INT_SCIA_RX);
    Interrupt_enable(INT_SCIA_TX);
    //
    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    //
    EINT;
    ERTM;

    //
    // Starts CPU-Timer 0, CPU-Timer 1, and CPU-Timer 2.
    //
    CPUTimer_startTimer(CPUTIMER0_BASE);
    CPUTimer_startTimer(CPUTIMER1_BASE);
    CPUTimer_startTimer(CPUTIMER2_BASE);

    //
    // Loop Forever
    //

    for(;;)
    {
    	if(ui_TimerFlag100ms)
    	{
    		COMMUNICATION_RS422A_TxRunSync(SCIATxBuff, s_CPU_1_GSRAM, 0x000F);//전채널
    		DELAY_US(500);

    		for(ui_i = 0; ui_i < COMMUNICATION_CHANNEL; ui_i++)
        	{
    			DELAY_US(500);
    			COMMUNICATION_RS422A_TxDataCall(SCIATxBuff, s_CPU_1_GSRAM, ui_i);
    			ul_TimeOut = 25000;
            	while(--ul_TimeOut)
            	{
            		e_Flag = DSP_SCIA_RxUpDate(SCIARxData, SCIARxBuff, &SCIARxCount, &SCIARxChannel);

            		if(e_Flag == SCIA_SUCCESS)
            		{
            			if(ui_i == SCIARxChannel)
            			COMMUNICATION_RS422A_RxUnPacket(s_CPU_2_GSRAM, &(SCIARxData[DSP_SCIA_HEADER_LENGTH]), SCIARxChannel);
                   		ui_UpdateflagCPU2++; //Update Flag
            			//ToDo : Jump일땐 어떻게 하지?
            			//if(s_CPU_1_GSRAM[ui_i].CPU1_Jump == s_CPU_2_GSRAM[ui_i].CPU2_Jump)
            			while(ui_UpdateflagCPU2 != ui_UpdateflagCPU1);
            			COMMUNICATION_RS422A_TxStepSend(SCIATxBuff, s_CPU_1_GSRAM, ui_i);
            			break;
            		}
            	}
#ifdef _DEBUG
            	if(!ul_TimeOut)
            	{
            		if(DEBUG_SCI_ERROR_COUNT[ui_i] != 0xFFFFFFFF)DEBUG_SCI_ERROR_COUNT[ui_i]++;
            	}
            	//DELAY_US(500000);
#endif //_DEBUG
        	}
        	ui_TimerFlag100ms = CLEAR;
    	}
    	if((SCI_RXSTATUS_OVERRUN | SCI_RXSTATUS_PARITY | SCI_RXSTATUS_ERROR | SCI_RXSTATUS_FRAMING) & SCI_getRxStatus(SCIA_BASE))
    	{
    		SCI_performSoftwareReset(SCIA_BASE);
    	}

#ifdef _DEBUG
    	PHOENIXON_CONTROLS_Funtion();
#endif//_DEBUG END
    }
}
void PHOENIXON_CONTROLS_Funtion(void)
{

}

//
// End of file
//

