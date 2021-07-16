/*
 * PhoenixonControls_AD7608.c
 *
 *  Created on: 2018. 5. 20.
 *      Author: BGKim
 */
#include "../../../common/include/F28x_Project.h"
#include "../../../common/include/driverlib.h"
#include "../../../common/include/device.h"

#pragma CODE_SECTION(AD7608_Conversion, ".TI.ramfunc");
#pragma CODE_SECTION(AD7608_FastDataCheck, ".TI.ramfunc");
#pragma CODE_SECTION(AD7608_BusyCheck, ".TI.ramfunc");
#pragma CODE_SECTION(AD7608_OverSamplingSelect, ".TI.ramfunc");
#pragma CODE_SECTION(AD7608_VoltageRangeSelect, ".TI.ramfunc");
#pragma CODE_SECTION(AD7608_Read, ".TI.ramfunc");

void AD7608_Init(void)
{
	AD7608_Reset();
}

void AD7608_Reset(void)
{
	AD7608_RESET_SET
	asm(" RPT #10 || NOP");//60ns, Delay : 5ns * (10 + 2) = 60ns Max 200
	AD7608_RESET_CLEAR
}

Uint16 AD7608_BusyCheck(void)
{
	Uint32 ul_TimeOut = 200000;

	asm(" RPT #10 || NOP");//60ns, Delay : 5ns * (10 + 2) = 60ns Max 200
	while(ul_TimeOut--)
	{
		if(!AD7608_BUSY)break;
	}
	if(ul_TimeOut)return(SUCCESS); //SUCCESS Or FAIL
	else return(FAIL); //SUCCESS Or FAIL
}

Uint16 AD7608_Conversion(void)
{
    if(AD7608_BusyCheck())
    {
        AD7608_CONVERSION
    }

    return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 AD7608_FastDataCheck(void)
{
	Uint32 ul_TimeOut = 200000;

	asm(" RPT #4 || NOP");//30ns, Delay : 5ns * (4 + 2) = 60ns Max 200
	while(ul_TimeOut--)
	{
		if(AD7608_FRSTD)break;
	}
	if(ul_TimeOut)return(SUCCESS); //SUCCESS Or FAIL
	else return(FAIL); //SUCCESS Or FAIL
}

Uint16 AD7608_OverSamplingSelect(E_AD7608_OVER_SAMPLING_SELECT e_OverSampling)
{
	volatile static E_AD7608_OVER_SAMPLING_SELECT e_OverSampling_Old = (E_AD7608_OVER_SAMPLING_SELECT)0xFFFF;

	if(e_OverSampling_Old != e_OverSampling)
	if(AD7608_BusyCheck())
	{
		if(e_OverSampling & 0x1)AD7608_OS0_SET
		else AD7608_OS0_CLEAR

		if(e_OverSampling & 0x2)AD7608_OS1_SET
		else AD7608_OS1_CLEAR

		if(e_OverSampling & 0x4)AD7608_OS2_SET
		else AD7608_OS2_CLEAR

		e_OverSampling_Old = e_OverSampling;
	}
	else
	{
		return(FAIL); //SUCCESS Or FAIL
	}
	asm(" RPT #10 || NOP");//60ns, Delay : 60ns * (10 + 2) = 60ns Max 200
	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 AD7608_VoltageRangeSelect(E_AD7608_VOLTAGE_RANGE_SELECT e_Voltage)
{
	volatile static E_AD7608_VOLTAGE_RANGE_SELECT e_Voltage_Old = (E_AD7608_VOLTAGE_RANGE_SELECT)0xFFFF;

	if(e_Voltage_Old != e_Voltage)
	if(AD7608_BusyCheck())
	{
		if(e_Voltage == AD7608_05_VOLTAGE)AD7608_RANGE_CLEAR
		else if(e_Voltage == AD7608_10_VOLTAGE)AD7608_RANGE_SET
		else AD7608_RANGE_CLEAR

		asm(" RPT #10 || NOP");//60ns, Delay : 60ns * (10 + 2) = 60ns Max 200
		e_Voltage_Old = e_Voltage;
	}
	else
	{
		return(FAIL); //SUCCESS Or FAIL
	}

	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 AD7608_Read(E_AD7608_VOLTAGE_RANGE_SELECT e_Voltage, E_AD7608_OVER_SAMPLING_SELECT e_OverSampling, float32 *Data)
{
	Uint16 ui_i = 0;
	Uint16 ui_HighByte = 0;
	Uint16 ui_LowByte = 0;
	int32 l_Temp = 0;

	AD7608_VoltageRangeSelect(e_Voltage);
	//AD7608_OverSamplingSelect(e_OverSampling);

	if(AD7608_BusyCheck())  //DL 2019-07-01
	{
		AD7608_N_CS_SET
		for(ui_i = 0; ui_i < 8; ui_i++)//Read Data
		{
			AD7608_READ_DATA(&ui_HighByte)
			AD7608_READ_DATA(&ui_LowByte)
			l_Temp = (int32)(ui_HighByte);
			l_Temp <<= 2;

			ui_LowByte >>= 14;
			ui_LowByte &= 0x03;
			l_Temp |= ui_LowByte;

			if(l_Temp & 0x20000)l_Temp |= 0xFFFC0000;

			Data[ui_i] = (float32)(l_Temp)
			*(float32)(e_Voltage == AD7608_10_VOLTAGE ? AD7608_GAIN_10V : AD7608_GAIN_5V);
		}
		AD7608_N_CS_CLEAR
	}
	else
	{
		return(FAIL); //SUCCESS Or FAIL
	}

	return(SUCCESS); //SUCCESS Or FAIL
}
//
// End of file
//

