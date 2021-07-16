/*
 * PhoenixonControls_SENSING.c
 *
 *  Created on: 2018. 5. 20.
 *      Author: BGKim
 */
#include "../../../common/include/F28x_Project.h"
#include "../../../common/include/driverlib.h"
#include "../../../common/include/device.h"

#pragma CODE_SECTION(SENSING_Emergency, ".TI.ramfunc");
#pragma CODE_SECTION(SENSING_ADC, ".TI.ramfunc");
#pragma CODE_SECTION(SENSING_ADCResultData, ".TI.ramfunc");

extern S_OPERATION_FILE s_Operation;

void SENSING_Init(void)
{

}

E_SENSING_EMERGENCY SENSING_Emergency(void)
{
	E_SENSING_EMERGENCY e_SensingEmergency = SENSING_EMERGENCY_OFF;

	e_SensingEmergency = (E_SENSING_EMERGENCY)DSP_GPIO65_I_EMERGENCY_IN();

	return (e_SensingEmergency);
}
Uint16 SENSING_ADC(S_SENSING_ADC_RESULT_ARRAY_DATA *s_ResultData)
{
    AD7608_Read(AD7608_10_VOLTAGE, AD7608_64, &(*s_ResultData).ExternalADC[0]);
	DSP_ADCRead(&(*s_ResultData).ADC[0]);

    return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 SENSING_ADCResultData(S_SENSING_ADC_RESULT_DATA *s_ResultData)
{
	S_SENSING_ADC_RESULT_ARRAY_DATA s_Data;

	SENSING_ADC(&s_Data);

	(*s_ResultData).SENSING_BatteryVoltage 					= s_Data.ExternalADC[SENSING_BATTERY_VOLTAGE] * SENSING_VOLTAGE_GAIN;
	(*s_ResultData).SENSING_MASTERCurrentA					= s_Data.ExternalADC[SENSING_MASTER_CURRENT_A] * SENSING_CURRENT_GAIN;
	(*s_ResultData).SENSING_MASTERCurrentB					= s_Data.ExternalADC[SENSING_MASTER_CURRENT_B] * SENSING_CURRENT_GAIN;
	(*s_ResultData).SENSING_SLAVECurrent_A					= s_Data.ExternalADC[SENSING_SLAVE_CURRENT_A] * SENSING_CURRENT_GAIN;
	(*s_ResultData).SENSING_SLAVECurrent_B					= s_Data.ExternalADC[SENSING_SLAVE_CURRENT_B] * SENSING_CURRENT_GAIN;
	(*s_ResultData).SENSING_ReferenceVoltage				= s_Data.ExternalADC[SENSING_REFERENCE_VOLTAGE] * SENSING_VOLTAGE_GAIN;
	(*s_ResultData).SENSING_MasterReferenceFineCurrent		= s_Data.ExternalADC[SENSING_MASTER_REFERENCE_FINE_CURRENT] * SENSING_FINE_CURRENT_GAIN;
	(*s_ResultData).SENSING_SlaveReferenceFineCurrent		= s_Data.ExternalADC[SENSING_SLAVE_REFERENCE_FINE_CURRENT] * SENSING_FINE_CURRENT_GAIN;
	(*s_ResultData).SENSING_Negative15VPower				= s_Data.ADC[SENSING_NEGATIVE_15V_POWER] * SENSING_NEGATIVE_15V_POWER_GAIN;
	(*s_ResultData).SENSING_MasterReferenceCurrent			= s_Data.ADC[SENSING_MASTER_REFERENCE_CURRENT] * SENSING_MASTER_REFERENCE_CURRENT_GAIN;
	(*s_ResultData).SENSING_SlaveReferenceCurrent			= s_Data.ADC[SENSING_SLAVE_REFERENCE_CURRENT] * SENSING_SLAVE_REFERENCE_CURRENT_GAIN;
	(*s_ResultData).SENSING_Temperature						= s_Data.ADC[SENSING_TEMPERATURE] * SENSING_TEMPERATURE_GAIN;
	(*s_ResultData).SENSING_InnerBatteryVoltage				= s_Data.ADC[SENSING_GROUND_POWER] * SENSING_GROUND_POWER_GAIN;
	(*s_ResultData).SENSING_Negative02VPower				= s_Data.ADC[SENSING_NEGATIVE_02V_POWER] * SENSING_NEGATIVE_02V_POWER_GAIN;
	(*s_ResultData).SENSING_Positive07VPower				= s_Data.ADC[SENSING_POSITIVE_07V_POWER] * SENSING_POSITIVE_07V_POWER_GAIN;
	(*s_ResultData).SENSING_Positive_05V_POWER				= s_Data.ADC[SENSING_POSITIVE_05V_POWER] * SENSING_POSITIVE_05V_POWER_GAIN;
	(*s_ResultData).SENSING_Positive_15V_POWER				= s_Data.ADC[SENSING_POSITIVE_15V_POWER] * SENSING_POSITIVE_15V_POWER_GAIN;
	(*s_ResultData).SENSING_CurrentMaster 					= (*s_ResultData).SENSING_MASTERCurrentA + (*s_ResultData).SENSING_MASTERCurrentB;
	(*s_ResultData).SENSING_CurrentSlave  					= (*s_ResultData).SENSING_SLAVECurrent_A + (*s_ResultData).SENSING_SLAVECurrent_B;
	(*s_ResultData).SENSING_Current                         = (*s_ResultData).SENSING_CurrentMaster + (*s_ResultData).SENSING_CurrentSlave;

	return(SUCCESS); //SUCCESS Or FAIL
}

//
// End of file
//

