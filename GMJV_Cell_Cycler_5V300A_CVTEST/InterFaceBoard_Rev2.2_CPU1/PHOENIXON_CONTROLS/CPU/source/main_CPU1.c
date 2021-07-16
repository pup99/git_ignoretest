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
volatile S_COMMUNICATION_GSRAM_CPU1 s_CPU_1_GSRAM[COMMUNICATION_CHANNEL];
volatile S_COMMUNICATION_GSRAM_CPU2 s_CPU_2_GSRAM[COMMUNICATION_CHANNEL];
volatile Uint16 ui_UpdateflagCPU1 = 0;
volatile Uint16 ui_UpdateflagCPU2 = 0;
volatile Uint16 ui_Updateflag = 0;

volatile Uint16 SCIATxBuff[DSP_SCIA_TX_MAX_BUFFER_LENGTH] 	= {0,};
volatile Uint16 SCIARxBuff[DSP_SCIA_RX_MAX_BUFFER_LENGTH]	= {0,};

#pragma DATA_SECTION(s_CPU_1_GSRAM,"SHARERAMGS15");
#pragma DATA_SECTION(s_CPU_2_GSRAM,"SHARERAMGS4");
#pragma DATA_SECTION(ui_UpdateflagCPU1,"SHARERAMGS15");
#pragma DATA_SECTION(ui_UpdateflagCPU2,"SHARERAMGS4");

#pragma DATA_SECTION(SCIARxBuff,"SHARERAMGS4");
#pragma DATA_SECTION(SCIATxBuff,"SHARERAMGS4");

extern volatile S_COMMUNICATION_DRAM *s_SDRAMp[];

extern uint16 *ETHERNET_RxBuffer;
extern uint16 *ETHERNET_TxBuffer;

#ifdef _DEBUG
Uint16 DEBUG_ = 0;

#endif//_DEBUG END

//#pragma CODE_SECTION(printf, "ramfuncs");

void main(void)
{
    //
    // Initialize device clock and peripherals
    //
    Device_init();
    DSP_EmifInit();
    //
    // GPIO35 is the LED pin.
    //
    GPIO_setPinConfig(GPIO_35_GPIO35);
	GPIO_setDirectionMode(35, GPIO_DIR_MODE_OUT);

    //
    // GPIO28 is the SCI Rx pin.
    //
    GPIO_setMasterCore(9, GPIO_CORE_CPU2);
    GPIO_setPinConfig(GPIO_9_SCIRXDA);
    GPIO_setDirectionMode(9, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(9, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(9, GPIO_QUAL_ASYNC);

    //
    // GPIO29 is the SCI Tx pin.
    //
    GPIO_setMasterCore(8, GPIO_CORE_CPU2);
    GPIO_setPinConfig(GPIO_8_SCITXDA);
    GPIO_setDirectionMode(8, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(8, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(8, GPIO_QUAL_ASYNC);

    SysCtl_selectCPUForPeripheral(SYSCTL_CPUSEL5_SCI, 1, SYSCTL_CPUSEL_CPU2);

    DSP_TimerInit();
    ETHERNET_Init();
    COMMUNICATION_INIT();

#ifdef _DEBUG
#endif
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
    //Interrupt_register(INT_TIMER1, &DSP_Timer1_ISR);
    //Interrupt_register(INT_TIMER2, &DSP_Timer2_ISR);
    //
    // To ensure precise timing, use write-only instructions to write to the
    // entire register. Therefore, if any of the configuration bits are changed
    // in configCPUTimer and initCPUTimers, the below settings must also
    // be updated.
    //
    CPUTimer_enableInterrupt(CPUTIMER0_BASE);
    //CPUTimer_enableInterrupt(CPUTIMER1_BASE);
    //CPUTimer_enableInterrupt(CPUTIMER2_BASE);

    //
    // Enables CPU int1, int13, and int14 which are connected to CPU-Timer 0,
    // CPU-Timer 1, and CPU-Timer 2 respectively.
    // Enable TINT0 in the PIE: Group 1 interrupt 7
    //
    Interrupt_enable(INT_TIMER0);
    //Interrupt_enable(INT_TIMER1);
    //Interrupt_enable(INT_TIMER2);
    //
    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    //
    while(SDRAM_Init());

#ifdef _STANDALONE
    IPCBootCPU2(C1C2_BROM_BOOTMODE_BOOT_FROM_FLASH);
#endif
    EINT;
    ERTM;
    //
    // Starts CPU-Timer 0, CPU-Timer 1, and CPU-Timer 2.
    //
    CPUTimer_startTimer(CPUTIMER0_BASE);
    //CPUTimer_startTimer(CPUTIMER1_BASE);
    //CPUTimer_startTimer(CPUTIMER2_BASE);

    //
    // Loop Forever
    //
    for(;;)
    {

		if(ui_UpdateflagCPU1 != ui_UpdateflagCPU2)
		{
			COMMUNICATION_Read(s_SDRAMp, s_CPU_1_GSRAM, s_CPU_2_GSRAM);
			COMMUNICATION_Write(s_SDRAMp, s_CPU_1_GSRAM, s_CPU_2_GSRAM);
//			ETHERNET_TCPs(0, COMMUNICATION_ETHERNET_PORT, 0, ETHERNET_RxBuffer, ETHERNET_TxBuffer, s_SDRAMp, s_CPU_1_GSRAM, s_CPU_2_GSRAM);
			ui_Updateflag = SET;
			ui_UpdateflagCPU1 = ui_UpdateflagCPU2;
		}
		ETHERNET_TCPs(0, COMMUNICATION_ETHERNET_PORT, 0, ETHERNET_RxBuffer, ETHERNET_TxBuffer, s_SDRAMp, s_CPU_1_GSRAM, s_CPU_2_GSRAM);


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

