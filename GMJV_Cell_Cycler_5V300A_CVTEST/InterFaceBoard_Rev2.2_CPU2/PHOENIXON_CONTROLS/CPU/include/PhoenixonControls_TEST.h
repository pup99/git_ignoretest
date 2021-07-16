/*
 * PhoenixonControls_TEST.h
 *
 *  Created on: 2018. 5. 20.
 *      Author: BGKim
 */

#ifndef PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_TEST_H_
#define PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_TEST_H_
#ifdef _DEBUG

//#define DEBUG_DATA 1
//typedef enum
//{
//	DEBUG_EMERGENCY_ON,
//	DEBUG_EMERGENCY_OFF,
//}E_DEBUG_;

typedef struct
{
	Uint32 IntegrationResetDelayTime;
	//...More
}S_DEBUG_DATA;

typedef struct
{
	E_CONTROL_COMMAND e_VoltageRELAYCommand;
	E_CONTROL_CHANNEL_SELECT e_CurrentRELAYChannel;
	E_CONTROL_COMMAND e_CurrentRELAYCommand;
	E_CONTROL_DIO_CHANNEL_SELECT e_DIORELAYChannel;
	E_CONTROL_COMMAND e_DIORELAYCommand;
	E_CONTROL_CHANNEL_SELECT e_OperationMUXChannel;
	E_CONTROL_OPERATION_MODE_SELECT e_OperationMUXMode;
	E_CONTROL_CHANNEL_SELECT e_IntegrationResetMUXChannel;
	E_CONTROL_COMMAND e_IntegrationResetMUXCommand;
	E_CONTROL_CHANNEL_SELECT e_IntegrationSlopMUXChannel;
	E_CONTROL_SLOP_SELECT e_IntegrationSlopMUXSlop;
	E_CONTROL_CHANNEL_SELECT e_IntegrationSlopPMChannel;
	float32 f_IntegrationSlopPMPotentialValue;
	float32 f_IntegrationOffsetDACVolatageValue;
	E_CONTROL_CHANNEL_SELECT e_GateOutputMUXChannel;
	E_CONTROL_OPERATION_MODE_SELECT e_GateOutputMUXMode;
	E_CONTROL_CHANNEL_SELECT e_GateOutputAMPChannel;
	E_CONTROL_OPERATION_MODE_SELECT e_GateOutputAMPMode;
	float32 f_VoltageReferenceDACVolatageValue;
	E_CONTROL_CHANNEL_SELECT e_CurrentReferenceDACChannel;
	float32 f_CurrentReferenceDACCurrentValue;
	E_CONTROL_CHANNEL_SELECT e_FINECurrentReferenceDACChannel;
	float32 f_FINECurrentReferenceDACCurrentValue;
	//...More
}S_DEBUG_CONTROL;

typedef struct
{
	E_CONTROL_OPERATION_MODE_SELECT ui_Flag;
	float32 f_Volatge;
	float32	f_Current;
	float32	f_FineCurrent;
	S_DEBUG_DATA Data;
	S_DEBUG_CONTROL Control;
}S_DEBUG_FILE;


void DEBUG_Inint(S_DEBUG_FILE *s_Data);
void DEBUG_UpDateState(S_OPERATION_FILE s_OP, S_DEBUG_FILE *sp_Data);
S_OPERATION_FILE DEBUG_Operation(S_DEBUG_FILE *s_Data);

#endif
#endif /* PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_TEST_H_ */

#ifdef _DEBUG
#endif
