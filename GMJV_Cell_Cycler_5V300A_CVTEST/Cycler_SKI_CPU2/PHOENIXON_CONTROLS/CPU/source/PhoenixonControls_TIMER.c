/*
 * PhoenixonControls_TIMER.c
 *
 *  Created on: 2018. 5. 21.
 *      Author: BGKim
 */
#include "../../../common/include/F28x_Project.h"
#include "../../../common/include/driverlib.h"
#include "../../../common/include/device.h"

#pragma CODE_SECTION(DSP_Timer0_ISR, ".TI.ramfunc");
#pragma CODE_SECTION(DSP_Timer1_ISR, ".TI.ramfunc");
#pragma CODE_SECTION(DSP_Timer2_ISR, ".TI.ramfunc");

void DSP_TimerInit(void)
{
    //
    // Initialize timer period to maximum
    //
    CPUTimer_setPeriod(CPUTIMER0_BASE, 0xFFFFFFFF);
    CPUTimer_setPeriod(CPUTIMER1_BASE, 0xFFFFFFFF);
    CPUTimer_setPeriod(CPUTIMER2_BASE, 0xFFFFFFFF);

    //
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT)
    //
    CPUTimer_setPreScaler(CPUTIMER0_BASE, 0);
    CPUTimer_setPreScaler(CPUTIMER1_BASE, 0);
    CPUTimer_setPreScaler(CPUTIMER2_BASE, 0);

    //
    // Make sure timer is stopped
    //
    CPUTimer_stopTimer(CPUTIMER0_BASE);
    CPUTimer_stopTimer(CPUTIMER1_BASE);
    CPUTimer_stopTimer(CPUTIMER2_BASE);

    //
    // Reload all counter register with period value
    //
    CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER1_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER2_BASE);

    DSP_Configimer(CPUTIMER0_BASE, DEVICE_SYSCLK_FREQ, DSP_TIMER0_TIME);
    DSP_Configimer(CPUTIMER1_BASE, DEVICE_SYSCLK_FREQ, DSP_TIMER1_TIME);
    DSP_Configimer(CPUTIMER2_BASE, DEVICE_SYSCLK_FREQ, DSP_TIMER2_TIME);

}

void DSP_Configimer(Uint32 Timer, float Frequency, float Period)
{
    Uint32 ul_Temp;

    //
    // Initialize timer period:
    //
    ul_Temp = (Uint32)(Frequency / 1000000 * Period);
    CPUTimer_setPeriod(Timer, ul_Temp);

    //
    // Set pre-scale counter to divide by 1 (SYSCLKOUT):
    //
    CPUTimer_setPreScaler(Timer, 0);

    //
    // Initializes timer control register. The timer is stopped, reloaded,
    // free run disabled, and interrupt enabled.
    // Additionally, the free and soft bits are set
    //
    CPUTimer_stopTimer(Timer);
    CPUTimer_reloadTimerCounter(Timer);
    CPUTimer_setEmulationMode(Timer, CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
    CPUTimer_enableInterrupt(Timer);

}

__interrupt void DSP_Timer0_ISR(void)
{
	GpioDataRegs.GPETOGGLE.bit.GPIO133 = 1;
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
__interrupt void DSP_Timer1_ISR(void)
{

}
__interrupt void DSP_Timer2_ISR(void)
{

}
//
// End of file
//


