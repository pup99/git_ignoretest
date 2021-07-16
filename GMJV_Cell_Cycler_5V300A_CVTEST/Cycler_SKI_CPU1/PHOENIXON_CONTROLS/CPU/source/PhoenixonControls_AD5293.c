/*
 * PhoenixonControls_AD5293.c
 *
 *  Created on: 2018. 5. 20.
 *      Author: BGKim
 */
#include "../../../common/include/F28x_Project.h"
#include "../../../common/include/driverlib.h"
#include "../../../common/include/device.h"

#pragma CODE_SECTION(AD5293_Write, ".TI.ramfunc");
#pragma CODE_SECTION(AD5293_Read, ".TI.ramfunc");
#pragma CODE_SECTION(AD5293_BusyCheck, ".TI.ramfunc");

void AD5293_Init(void)
{
	AD5293_Reset(AD5293_MASTER);
	AD5293_Reset(AD5293_MASTER);
	AD5293_Reset(AD5293_MASTER);
	AD5293_Reset(AD5293_SLAVE);
	AD5293_Reset(AD5293_SLAVE);
	AD5293_Reset(AD5293_SLAVE);
	asm(" RPT #10 || NOP");//60ns, Delay : 60ns * (10 + 2) = 60ns Max 200
	AD5293_Write(AD5293_MASTER, AD5293_COMMMAND_WRITE_CONTROL, AD5293_CONTROL_REGISTER_D1);
	AD5293_Write(AD5293_MASTER, AD5293_COMMMAND_WRITE_CONTROL, AD5293_CONTROL_REGISTER_D1);
	AD5293_Write(AD5293_MASTER, AD5293_COMMMAND_WRITE_CONTROL, AD5293_CONTROL_REGISTER_D1);
	AD5293_Write(AD5293_SLAVE, AD5293_COMMMAND_WRITE_CONTROL, AD5293_CONTROL_REGISTER_D1);
	AD5293_Write(AD5293_SLAVE, AD5293_COMMMAND_WRITE_CONTROL, AD5293_CONTROL_REGISTER_D1);
	AD5293_Write(AD5293_SLAVE, AD5293_COMMMAND_WRITE_CONTROL, AD5293_CONTROL_REGISTER_D1);

}
void AD5293_Reset(E_AD5293_CHANNEL_SELECT Channel)
{
	if(Channel == AD5293_MASTER)
	{
		AD5293_MASTER_RESET_SET
	}
	else
	{
		AD5293_SLAVE_RESET_SET
	}
	asm(" RPT #10 || NOP");//60ns, Delay : 60ns * (10 + 2) = 60ns Max 200

	AD5293_MASTER_RESET_CLEAR
	AD5293_SLAVE_RESET_CLEAR
}

Uint16 AD5293_BusyCheck(E_AD5293_CHANNEL_SELECT Channel)
{
	Uint32 ul_TimeOut = 200000;

	while(ul_TimeOut--)
	{
		if(Channel == AD5293_MASTER)
		{
			if(AD5293_MASTER_RADY)break;
		}
		else
		{
			if(AD5293_SLAVE_RADY)break;
		}
	}
	if(ul_TimeOut)return(SUCCESS); //SUCCESS Or FAIL
	else return(FAIL); //SUCCESS Or FAIL
}

Uint16 AD5293_Write(E_AD5293_CHANNEL_SELECT Channel, Uint16 Command, Uint16 Data)
{
	Uint16 ui_i = 0;
	Uint16 ui_Temp = 0;
//	Uint16 ui_ReadTemp = 0;

	ui_Temp = ((Command & 0xF) << 10) + (Data & 0x3FF);

	for(ui_i = 0; ui_i < 3; ui_i++)
	{
		AD5293_BusyCheck(Channel);
        DELAY_US(1);
		if(Channel == AD5293_MASTER)AD5293_MASTER_CS_SET
		else AD5293_SLAVE_CS_SET
		DELAY_US(1);
		AD5293_WRITE(ui_Temp);
		DELAY_US(20);

		if(Channel == AD5293_MASTER)AD5293_MASTER_CS_CLEAR
		else AD5293_SLAVE_CS_CLEAR
        DELAY_US(10);
		asm(" RPT #10 || NOP");//60ns, Delay : 60ns * (10 + 2) = 60ns Max 200

//		AD5293_READ(&ui_ReadTemp);
	}
//	DELAY_US(10);

//	if(ui_ReadTemp == ui_Temp)return(SUCCESS); //SUCCESS Or FAIL
//	else return(FAIL); //SUCCESS Or FAIL
//	DELAY_US(5);//AD5293   VW Settling Time
				//		   Code = full scale, R-normal mode 750 ns
				//		   Code = full scale, R-perf mode 2.5 µs
				//		   Code = half scale, R-normal mode 2.5 µs
				//		   Code = half scale, R-perf mode 5 µs
	return(SUCCESS); //SUCCESS Or FAIL
}
Uint16 AD5293_Read(E_AD5293_CHANNEL_SELECT Channel, Uint16 *Command, Uint16 *Data)
{
	//To do
	return(SUCCESS); //SUCCESS Or FAIL
}

//
// End of file
//
