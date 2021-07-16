/*
 * PhoenixonControls_ADC.c
 *
 *  Created on: 2018. 5. 20.
 *      Author: BGKim
 */
#include "../../../common/include/F28x_Project.h"
#include "../../../common/include/driverlib.h"
#include "../../../common/include/device.h"

#pragma CODE_SECTION(DSP_ADCRead, ".TI.ramfunc");
void DSP_ADCInit(void)
{
    //
    // Set ADCCLK divider to /4
    //
    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_8_0);
    ADC_setPrescaler(ADCB_BASE, ADC_CLK_DIV_8_0);
    ADC_setPrescaler(ADCC_BASE, ADC_CLK_DIV_8_0);
    //
    // Set resolution and signal mode (see #defines above) and load
    // corresponding trims.
    //
    ADC_setMode(ADCA_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);
    ADC_setMode(ADCB_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);
    ADC_setMode(ADCC_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);
    //
    // Set pulse positions to late
    //
    ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);
    ADC_setInterruptPulseMode(ADCB_BASE, ADC_PULSE_END_OF_CONV);
    ADC_setInterruptPulseMode(ADCC_BASE, ADC_PULSE_END_OF_CONV);
    //
    // Power up the ADCs and then delay for 1 ms
    //
    ADC_enableConverter(ADCA_BASE);
    ADC_enableConverter(ADCB_BASE);
    ADC_enableConverter(ADCC_BASE);

    DEVICE_DELAY_US(1000);

    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN2, 511);//adc window max 512
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN3, 511);
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER2, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN4, 511);
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER3, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN5, 511);

    ADC_setInterruptSource(ADCA_BASE, ADC_INT_NUMBER3, ADC_SOC_NUMBER3);
    ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER3);
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER3);


    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN0, 511);
	ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN2, 511);
	ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER2, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN3, 511);

	ADC_setInterruptSource(ADCB_BASE, ADC_INT_NUMBER2, ADC_SOC_NUMBER2);
	ADC_enableInterrupt(ADCB_BASE, ADC_INT_NUMBER2);
	ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER2);


    ADC_setupSOC(ADCC_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN2, 511);
	ADC_setupSOC(ADCC_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN3, 511);


	ADC_setInterruptSource(ADCC_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER1);
	ADC_enableInterrupt(ADCC_BASE, ADC_INT_NUMBER1);
	ADC_clearInterruptStatus(ADCC_BASE, ADC_INT_NUMBER1);


}

Uint16 DSP_ADCRead(float32 *Data)
{
	Uint16	ui_i = 0;
	Uint16 ui_ResultData[9] = {0,};
	int32 l_TimeOut = 200000;
	//
    // Convert, wait for completion, and store results
    //
    ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER0);
    ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER1);
    ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER2);
    ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER3);

    ADC_forceSOC(ADCB_BASE, ADC_SOC_NUMBER0);
    ADC_forceSOC(ADCB_BASE, ADC_SOC_NUMBER1);
    ADC_forceSOC(ADCB_BASE, ADC_SOC_NUMBER2);

    ADC_forceSOC(ADCC_BASE, ADC_SOC_NUMBER0);
    ADC_forceSOC(ADCC_BASE, ADC_SOC_NUMBER1);

    l_TimeOut = 200000;
    while(ADC_getInterruptStatus(ADCA_BASE, ADC_INT_NUMBER3) == false)
    {
    	if(l_TimeOut-- <= 0)break;
    }
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER3);
    l_TimeOut = 200000;

    while(ADC_getInterruptStatus(ADCB_BASE, ADC_INT_NUMBER2) == false)
    {
    	if(l_TimeOut-- <= 0)break;
    }
    ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER2);
    l_TimeOut = 200000;

    while(ADC_getInterruptStatus(ADCC_BASE, ADC_INT_NUMBER1) == false)
    {
    	if(l_TimeOut-- <= 0)break;
    }
    ADC_clearInterruptStatus(ADCC_BASE, ADC_INT_NUMBER1);

    //
    // Store results
    //
    ui_ResultData[0] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);
    ui_ResultData[1] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER1);
    ui_ResultData[2] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER2);
    ui_ResultData[3] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER3);

    ui_ResultData[4] = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER0);
    ui_ResultData[5] = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER1);
    ui_ResultData[6] = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER2);

    ui_ResultData[7] = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER0);
    ui_ResultData[8] = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER1);

    for(ui_i = 0; ui_i < 9; ui_i++)Data[ui_i] = ui_ResultData[ui_i] * DSP_ADC_GAIN;
    //
    // Software breakpoint. At this point, conversion results are stored in
    // adcAResult0, adcAResult1, adcBResult0, and adcBResult1.
    //
    // Hit run again to get updated conversions.
    //
    return(SUCCESS); //SUCCESS Or FAIL
}

//
// End of file
//

