/*
 * PhoenixonControls_TIMER.h
 *
 *  Created on: 2018. 5. 21.
 *      Author: BGKim
 */

#ifndef PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_TIMER_H_
#define PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_TIMER_H_

#define DSP_TIMER0_TIME 100000 	//500ms = 500000 * 1us
#define DSP_TIMER1_TIME 1000000 //1s = 1000000 * 1us
#define DSP_TIMER2_TIME 1000000 //1s = 1000000 * 1us

void DSP_TimerInit(void);
void DSP_Configimer(Uint32 Timer, float Frequency, float Period);
__interrupt void DSP_Timer0_ISR(void);
__interrupt void DSP_Timer1_ISR(void);
__interrupt void DSP_Timer2_ISR(void);
#endif /* PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_TIMER_H_ */
