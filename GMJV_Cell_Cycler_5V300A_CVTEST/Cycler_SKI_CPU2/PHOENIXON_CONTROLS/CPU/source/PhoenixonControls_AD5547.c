/*
 * PhoenixonControls_AD5547.c
 *
 *  Created on: 2018. 5. 20.
 *      Author: BGKim
 */
#include "../../../common/include/F28x_Project.h"
#include "../../../common/include/driverlib.h"
#include "../../../common/include/device.h"

#pragma CODE_SECTION(AD5547_OutputSelect, ".TI.ramfunc");
#pragma CODE_SECTION(AD5547_Write, ".TI.ramfunc");

void AD5547_Init(void)
{
	AD5547_Reset();
}

Uint16 AD5547_Reset(void)
{
	AD5547_VOLTAGE_N_RS_SET;
	AD5547_CURRENT_N_RS_SET;
	AD5547_FINE_CURRENT_N_RS_SET;
	asm(" RPT #10 || NOP");//60ns, Delay : 60ns * (10 + 2) = 60ns Max 200
	AD5547_VOLTAGE_N_RS_CLEAR;
	AD5547_CURRENT_N_RS_CLEAR;
	AD5547_FINE_CURRENT_N_RS_CLEAR;
	AD5547_OutputSelect(AD5547_NON_OUTPUT);

	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 AD5547_OutputSelect(E_AD5547_OUTPUT_SELECT e_Port)
{
	static E_AD5547_OUTPUT_SELECT e_Port_Old = 	AD5547_NON_OUTPUT;

	if(e_Port_Old != e_Port)
	switch(e_Port)
	{
	case AD5547_A:
		AD5547_A0_CLEAR;
		AD5547_A1_CLEAR;
		break;
	case AD5547_NON_OUTPUT:
		AD5547_A0_SET;
		AD5547_A1_CLEAR;
		break;
	case AD5547_ALL_OUTPUT:
		AD5547_A0_CLEAR;
		AD5547_A1_SET;
		break;
	case AD5547_B:
		AD5547_A0_SET;
		AD5547_A1_SET;
		break;
	default : break;
	}
	e_Port_Old = e_Port;

	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 AD5547_Write(E_AD5547_CHANNEL_SELECT e_Channel, Uint16 ui_Data)
{
	switch(e_Channel)
	{
	case AD5547_VOLTAGE:
		AD5547_OutputSelect(AD5547_A);
		AD5547_VOTAGE_WIRTE_DATA(ui_Data);
		AD5547_VOTAGE_LDAC;
		break;

	case AD5547_INTEGRATION_VOLTAGE:
		AD5547_OutputSelect(AD5547_B);
		AD5547_VOTAGE_WIRTE_DATA(ui_Data);
		AD5547_VOTAGE_LDAC;
		break;

	case AD5547_CURRENT_MASTER:
		AD5547_OutputSelect(AD5547_A);
		AD5547_CURRENT_WIRTE_DATA(ui_Data);
		AD5547_CURRENT_LDAC;
		break;

	case AD5547_CURRENT_SLAVE:
		AD5547_OutputSelect(AD5547_B);
		AD5547_CURRENT_WIRTE_DATA(ui_Data);
		AD5547_CURRENT_LDAC;
		break;

	case AD5547_FINE_CURRENT_MASTER:
		AD5547_OutputSelect(AD5547_A);
		AD5547_FINE_CURRENT_WIRTE_DATA(ui_Data);
		AD5547_FINE_CURRENT_LDAC;
		break;

	case AD5547_FINE_CURRENT_SLAVE:
		AD5547_OutputSelect(AD5547_B);
		AD5547_FINE_CURRENT_WIRTE_DATA(ui_Data);
		AD5547_FINE_CURRENT_LDAC;
		break;

	case AD5547_ALL_CHANNEL:
		AD5547_OutputSelect(AD5547_ALL_OUTPUT);
		AD5547_CURRENT_WIRTE_DATA(ui_Data);
		AD5547_VOTAGE_WIRTE_DATA(ui_Data);
		AD5547_FINE_CURRENT_WIRTE_DATA(ui_Data);
		AD5547_VOTAGE_LDAC;
		AD5547_CURRENT_LDAC;
		AD5547_FINE_CURRENT_LDAC;
		break;
	default : break;
	}
	DELAY_US(1);//AD5547   Output Voltage Settling Time Max 0.5us

	return(SUCCESS); //SUCCESS Or FAIL
}

//
// End of file
//

