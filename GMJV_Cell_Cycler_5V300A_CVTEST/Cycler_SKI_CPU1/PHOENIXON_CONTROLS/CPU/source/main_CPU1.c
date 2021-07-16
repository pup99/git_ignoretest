/*
 * main.c
 *
 *  Created on: 2018. 5. 16.
 *      Author: BGKim
 */

#include "../../../common/include/F28x_Project.h"
#include "../../../common/include/driverlib.h"
#include "../../../common/include/device.h"
#include "../../../common/include/F2837xD_Ipc_drivers.h"

//
// FPU header file to access memcpy_fast_far().
//
//#include "../include/PhoenixonControls_FPU.h"

//
// PhoenixonControls_COMMUNICATION.c
//
volatile S_COMMUNICATION_GSRAM_CPU1 GSRAM_STPE[COMMUNICATION_TX_DEVICE_BANK];
volatile S_COMMUNICATION_GSRAM_CPU2 GSRAM_DATA[COMMUNICATION_TX_DEVICE_BANK];

volatile Uint32 GSRAM_DATA_BANK_INDEX_CPU1 = 0;
volatile Uint32 GSRAM_DATA_BANK_INDEX_CPU2 = 0;

volatile Uint32 SYNC_SIGNAL_CPU1 = 0;
volatile Uint32 SYNC_SIGNAL_CPU2 = 0;

volatile Uint32 ui_UpdateflagCPU1 = 0;
volatile Uint32 ui_UpdateflagCPU2 = 0;

volatile Uint32 ui_UpdateSTEPCPU1 = 0;
volatile Uint32 ui_UpdateSTEPCPU2 = 0;

volatile Uint32 ui_UpdateDATACPU1 = 0;
volatile Uint32 ui_UpdateDATACPU2 = 0;

volatile Uint16 SCIATxBuff[DSP_SCIA_TX_MAX_BUFFER_LENGTH] 	= {0,};
volatile Uint16 SCIARxBuff[DSP_SCIA_RX_MAX_BUFFER_LENGTH]	= {0,};

#pragma DATA_SECTION(GSRAM_DATA,"SHARE_GSRAM_CPU1");
#pragma DATA_SECTION(GSRAM_DATA_BANK_INDEX_CPU1,"SHARE_GSRAM_CPU1");
#pragma DATA_SECTION(SYNC_SIGNAL_CPU1,"SHARE_GSRAM_CPU1");
#pragma DATA_SECTION(ui_UpdateflagCPU1,"SHARE_GSRAM_CPU1");
#pragma DATA_SECTION(ui_UpdateSTEPCPU1,"SHARE_GSRAM_CPU1");
#pragma DATA_SECTION(ui_UpdateDATACPU1,"SHARE_GSRAM_CPU1");

#pragma DATA_SECTION(GSRAM_STPE,"SHARE_GSRAM_CPU2");
#pragma DATA_SECTION(GSRAM_DATA_BANK_INDEX_CPU2,"SHARE_GSRAM_CPU2");
#pragma DATA_SECTION(SYNC_SIGNAL_CPU2,"SHARE_GSRAM_CPU2");
#pragma DATA_SECTION(ui_UpdateflagCPU2,"SHARE_GSRAM_CPU2");
#pragma DATA_SECTION(ui_UpdateSTEPCPU2,"SHARE_GSRAM_CPU2");
#pragma DATA_SECTION(ui_UpdateDATACPU2,"SHARE_GSRAM_CPU2");
#pragma DATA_SECTION(SCIATxBuff,"SHARE_GSRAM_CPU2");
#pragma DATA_SECTION(SCIARxBuff,"SHARE_GSRAM_CPU2");
//End

extern volatile S_COMMUNICATION_TX s_DATA;
extern volatile S_COMMUNICATION_RX_JUMP s_JUMP;
extern volatile S_COMMUNICATION_RX_COMMAD s_COMMAND;
extern volatile S_COMMUNICATION_RX_START s_START;
extern volatile S_COMMUNICATION_RX_PAUSE s_PAUSE;

extern volatile S_COMMUNICATION_TX_END   s_SENSING_END;
extern volatile S_COMMUNICATION_TX_ALARM s_SENSING_ALARM;

S_OPERATION_FILE s_Operation;

uint16_t git_ignore_test;
uint16_t git_ignore_test2;
uint16_t git_ignore_test3;
uint16_t git_ignore_test4;

//#pragma CODE_SECTION(printf, "ramfuncs");

void main(void)
{
    //
    // Initialize device clock and peripherals
    //
    Device_init();
    DSP_GpioInit();
    DSP_EmifInit();
    DSP_SpiInit();
    DSP_ADCInit();
    DSP_TimerInit();

    AD5547_Init();
    AD7608_Init();
    AD5293_Init();
//    CALIBRATION_Init();
    CONTROL_Init();
    OPERATION_Init(&s_Operation);

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

    //
    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    //
#ifdef _STANDALONE
    IPCBootCPU2(C1C2_BROM_BOOTMODE_BOOT_FROM_FLASH);
#endif
    EINT;
    ERTM;

#if (SLOPE_TUNE == 1)
    Interrupt_register(INT_TIMER1, &DSP_Timer1_ISR);
    CPUTimer_enableInterrupt(CPUTIMER1_BASE);
    Interrupt_enable(INT_TIMER1);
    CPUTimer_startTimer(CPUTIMER1_BASE);

    InitSlopeTuneProperties();
    RunSlopTune();
#endif
#if (VI_ADC_TUNE == 1)
    Interrupt_register(INT_TIMER1, &DSP_Timer1_ISR);
    CPUTimer_enableInterrupt(CPUTIMER1_BASE);
    Interrupt_enable(INT_TIMER1);
    CPUTimer_startTimer(CPUTIMER1_BASE);

    InitVIADCTuneProperties();
    RunVIADCTune();
#endif

    //
    // Starts CPU-Timer 0, CPU-Timer 1, and CPU-Timer 2.
    //
    CPUTimer_startTimer(CPUTIMER0_BASE);

    //
    // Loop Forever
    //

    for(;;)
    {
    	if(ui_UpdateSTEPCPU1 != ui_UpdateSTEPCPU2 && s_Operation.VARIABLE_StartFlag == CLEAR)
		{
            s_Operation.VARIABLE_UpdateData++;
            COMMUNICATION_STEPRead(&s_Operation, GSRAM_STPE, GSRAM_DATA, 0);
			ui_UpdateSTEPCPU1 = ui_UpdateSTEPCPU2;
		}
    }
}

void PHOENIXON_CONTROLS_Funtion(void)
{

}

//
// End of file
//

