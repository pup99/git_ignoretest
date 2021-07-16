/*
 * PhoenixonControls_CONTROL.h
 *
 *  Created on: 2018. 5. 20.
 *      Author: BGKim
 */

#ifndef PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_CONTROL_H_
#define PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_CONTROL_H_

#define CONTROL_SHUNT						(float)(0.001)
#define CONTROL_RESISTANCE_VALUE    		(float)(1800.0)
#define CONTROL_CURRETN_AD8226_GAIN    		(float)(((float)(49400.0) / CONTROL_RESISTANCE_VALUE) + (float)(1.0))

#define CONTROL_VOLTAGE_GAIN				(float)(1.0)
#define CONTROL_INTEGRATION_VOLTAGE_GAIN	(float)(1.0)
#define CONTROL_CURRENT_GAIN				(float)(CONTROL_CURRETN_AD8226_GAIN * CONTROL_SHUNT)
#define CONTROL_FINE_CURRENT_GAIN			(float)(CONTROL_CURRENT_GAIN * (float)(10.0))//Max 50mV 갖는 전류값은?

typedef enum
{
	CONTROL_OFF,
	CONTROL_ON,
	CONTROL_COMMAND_INIT,
}E_CONTROL_COMMAND;

typedef enum
{
	CONTROL_REST,
	CONTROL_CHARGE,
	CONTROL_DISCHARGE,
	CONTROL_OPERATION_INIT,
}E_CONTROL_OPERATION_MODE_SELECT;

typedef enum
{
	CONTROL_MASTER,
	CONTROL_SLAVE,
	CONTROL_ALL,
	CONTROL_CHANNEL_INIT,
}E_CONTROL_CHANNEL_SELECT;

typedef enum
{
	CONTROL_DIO_00,
	CONTROL_DIO_01,
	CONTROL_DIO_ALL,
	CONTROL_DIO_INIT,
}E_CONTROL_DIO_CHANNEL_SELECT;

typedef enum
{
	CONTROL_GND,
	CONTROL_PM,
	CONTROL_NONE,
	CONTROL_SLOW,
	CONTROL_NOUMAL,
	CONTROL_FAST,
	CONTROL_VERY_FAST,
	CONTROL_VERY_FASTEST,
	CONTROL_SLOP_INIT,
}E_CONTROL_SLOP_SELECT;

typedef struct
{
	E_CONTROL_COMMAND e_Command;
	E_CONTROL_COMMAND e_OldCommand;
}S_CONTROL_VRELAY;

typedef struct
{
	E_CONTROL_CHANNEL_SELECT e_Channel;
	E_CONTROL_COMMAND e_Command[2];
	E_CONTROL_COMMAND e_OldCommand[2];
}S_CONTROL_RELAY;

typedef struct
{
	float32 f_Value;
	float32 f_OldValue;
}S_CONTROL_VDAC;

typedef struct
{
	E_CONTROL_CHANNEL_SELECT e_Channel;
	float32 f_Value[2];
	float32 f_OldValue[2];
}S_CONTROL_DAC;

typedef struct
{
	E_CONTROL_CHANNEL_SELECT e_Channel;
	E_CONTROL_OPERATION_MODE_SELECT e_Mode[2];
	E_CONTROL_OPERATION_MODE_SELECT e_OldMode[2];
}S_CONTROL_MUX;

typedef struct
{
	E_CONTROL_CHANNEL_SELECT e_Channel;
	E_CONTROL_COMMAND e_Command[2];
	E_CONTROL_COMMAND e_OldCommand[2];
}S_CONTROL_INTEGRATION_RESET_MUX;

typedef struct
{
	E_CONTROL_CHANNEL_SELECT e_Channel;
	E_CONTROL_SLOP_SELECT e_Slop[2];
	E_CONTROL_SLOP_SELECT e_OldSlop[2];

}S_CONTROL_INTEGRATION_SOLP_MUX;

typedef struct
{
	E_CONTROL_DIO_CHANNEL_SELECT e_Channel;
	E_CONTROL_COMMAND e_Command[2];
	E_CONTROL_COMMAND e_OldCommand[2];
}S_CONTROL_DIO;

void CONTROL_Init(void);
S_CONTROL_VRELAY CONTROL_VoltageRELAY(E_CONTROL_COMMAND e_Command);
S_CONTROL_RELAY CONTROL_CurrentRELAY(E_CONTROL_CHANNEL_SELECT e_Channel, E_CONTROL_COMMAND e_Command);
S_CONTROL_DIO CONTROL_DIORELAY(E_CONTROL_DIO_CHANNEL_SELECT e_Channel, E_CONTROL_COMMAND e_Command);

S_CONTROL_MUX CONTROL_OperationMUX(E_CONTROL_CHANNEL_SELECT e_Channel, E_CONTROL_OPERATION_MODE_SELECT e_Mode);

S_CONTROL_INTEGRATION_RESET_MUX CONTROL_IntegrationResetMUX(E_CONTROL_CHANNEL_SELECT e_Channel, E_CONTROL_COMMAND e_Command);
S_CONTROL_INTEGRATION_SOLP_MUX CONTROL_IntegrationSlopMUX(E_CONTROL_CHANNEL_SELECT e_Channel, E_CONTROL_SLOP_SELECT e_Slop);
S_CONTROL_DAC CONTROL_IntegrationSlopPMDAC(E_CONTROL_CHANNEL_SELECT e_Channel, float32 f_PotentialValue);
S_CONTROL_VDAC CONTROL_GateOffsetDAC(float32 f_VolatageValue);

S_CONTROL_MUX CONTROL_GateOutputMUX(E_CONTROL_CHANNEL_SELECT e_Channel, E_CONTROL_OPERATION_MODE_SELECT e_Mode);
S_CONTROL_MUX CONTROL_GateOutputAMP(E_CONTROL_CHANNEL_SELECT e_Channel, E_CONTROL_OPERATION_MODE_SELECT e_Mode);

S_CONTROL_VDAC CONTROL_VoltageReferenceDAC(float32 f_VolatageValue);
S_CONTROL_DAC CONTROL_CurrentReferenceDAC(E_CONTROL_CHANNEL_SELECT e_Channel, float32 f_CurrentValue);
S_CONTROL_DAC CONTROL_FINECurrentReferenceDAC(E_CONTROL_CHANNEL_SELECT e_Channel, float32 f_CurrentValue);

//void CONTROL_Init(void);
//Uint16 CONTROL_VoltageRELAY(S_CONTROL_VRELAY *s_Data);
//Uint16 CONTROL_CurrentRELAY(S_CONTROL_RELAY *s_Data);
//Uint16 CONTROL_DIORELAY(S_CONTROL_DIO *s_Data);
//
//Uint16 CONTROL_OperationMUX(S_CONTROL_MUX *s_Data);
//
//Uint16 CONTROL_IntegrationResetMUX(S_CONTROL_INTEGRATION_RESET_MUX *s_Data);
//Uint16 CONTROL_IntegrationSlopMUX(S_CONTROL_INTEGRATION_SOLP_MUX *s_Data);
//Uint16 CONTROL_IntegrationSlopPMDAC(S_CONTROL_DAC *s_Data);
//Uint16 CONTROL_GateOffsetDAC(S_CONTROL_VDAC *s_Data);
//
//Uint16 CONTROL_GateOutputMUX(S_CONTROL_MUX *s_Data);
//Uint16 CONTROL_GateOutputAMP(S_CONTROL_MUX *s_Data);
//
//Uint16 CONTROL_VoltageReferenceDAC(S_CONTROL_VDAC *s_Data);
//Uint16 CONTROL_CurrentReferenceDAC(S_CONTROL_DAC *s_Data);
//Uint16 CONTROL_FINECurrentReferenceDAC(S_CONTROL_DAC *s_Data);

#endif /* PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_CONTROL_H_ */
