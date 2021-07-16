/*
 * PhoenixonControls_OPERATION.c
 *
 *  Created on: 2018. 5. 20.
 *      Author: BGKim
 */
#include "../../../common/include/F28x_Project.h"
#include "../../../common/include/driverlib.h"
#include "../../../common/include/device.h"

#ifdef _DEBUG
extern S_DEBUG_FILE s_DebugFile;
#endif

S_OPERATION_FILE OPERATION_Init(void)
{
	S_OPERATION_FILE s_OP;
	s_OP.C_VRELAY = CONTROL_VoltageRELAY(CONTROL_OFF);
	s_OP.C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
	s_OP.C_DIORELAY = CONTROL_DIORELAY(CONTROL_DIO_ALL, CONTROL_OFF);

	s_OP.C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_REST);

	s_OP.C_IntResetMUX = CONTROL_IntegrationResetMUX(CONTROL_ALL, CONTROL_OFF);
	s_OP.C_IntSlopMUX = CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_GND);
	s_OP.C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, 10000);
	s_OP.C_GateOffsetDAC = CONTROL_GateOffsetDAC(0.0);

	s_OP.C_GateOutputMUX = CONTROL_GateOutputMUX(CONTROL_ALL, CONTROL_REST);
	s_OP.C_GateOutputAMP = CONTROL_GateOutputAMP(CONTROL_ALL, CONTROL_REST);

	s_OP.C_VRefDAC = CONTROL_VoltageReferenceDAC(0.0);
	s_OP.C_IRefDAC = CONTROL_CurrentReferenceDAC(CONTROL_ALL, 0.0);
	s_OP.C_FINEIRefDAC = CONTROL_FINECurrentReferenceDAC(CONTROL_ALL, 0.0);

	return(s_OP);//SUCCESS Or FAIL
//	(*s_OP).ui_UpdateData = 0; //ipc 사용
//
//	(*s_OP).f_Voltage = 0.0;
//	(*s_OP).f_Current = 0.0;
//	(*s_OP).f_Power = 0.0;
//	(*s_OP).e_Cell = OPEARTION_NORMAL;
//	(*s_OP).e_Operation = OPERATION_MODE_STOP;
//
//	//
//	//	Uint16 CONTROL_VoltageRELAY(S_CONTROL_VRELAY *s_Data);
//	//	-(*s_OP).C_VRELAY.e_Command
//	//   0=CONTROL_OFF, 1=CONTROL_ON, 2=CONTROL_COMMAND_INIT
//	(*s_OP).C_VRELAY.e_Command = CONTROL_OFF;
//	(*s_OP).C_VRELAY.e_OldCommand = CONTROL_COMMAND_INIT;
//	//
//	//	Uint16 CONTROL_CurrentRELAY(S_CONTROL_RELAY *s_Data);
//	//	-(*s_OP).C_IRELAY.e_Channel :
//	//   0=CONTROL_MASTER, 1=CONTROL_SLAVE, 2=CONTROL_ALL, 3=CONTROL_CHANNEL_INIT
//	//  -C_IRELAY.e_Command:
//	//   0=CONTROL_OFF, 1=CONTROL_ON, 3=CONTROL_COMMAND_INIT
//	(*s_OP).C_IRELAY.e_Channel = CONTROL_ALL;
//	(*s_OP).C_IRELAY.e_Command[CONTROL_MASTER] = CONTROL_OFF;
//	(*s_OP).C_IRELAY.e_Command[CONTROL_SLAVE] = CONTROL_OFF;
//	(*s_OP).C_IRELAY.e_OldCommand[CONTROL_MASTER] = CONTROL_COMMAND_INIT;
//	(*s_OP).C_IRELAY.e_OldCommand[CONTROL_SLAVE] = CONTROL_COMMAND_INIT;
//	//
//	//	Uint16 CONTROL_DIORELAY(S_CONTROL_DIO *s_Data);
//	//  -(*s_OP).C_DIORELAY.e_Channel
//	//  -(*s_OP).C_DIORELAY.e_Command
//	//   0=CONTROL_OFF, 1=CONTROL_ON, 3=CONTROL_COMMAND_INIT
//	(*s_OP).C_DIORELAY.e_Channel = CONTROL_DIO_ALL;//0=CONTROL_DIO_00, 1=CONTROL_DIO_01, 2=CONTROL_DIO_ALL, 3=CONTROL_DIO_INIT
//	(*s_OP).C_DIORELAY.e_Command[CONTROL_MASTER] = CONTROL_OFF;//0=CONTROL_OFF, 1=CONTROL_ON, 3=CONTROL_COMMAND_INIT
//	(*s_OP).C_DIORELAY.e_Command[CONTROL_SLAVE] = CONTROL_OFF;
//	(*s_OP).C_DIORELAY.e_OldCommand[CONTROL_MASTER] = CONTROL_COMMAND_INIT;
//	(*s_OP).C_DIORELAY.e_OldCommand[CONTROL_SLAVE] = CONTROL_COMMAND_INIT;
//	//
//	//	Uint16 CONTROL_OperationMUX(S_CONTROL_MUX *s_Data);
//	//  -(*s_OP).C_OperationMUX.e_Channel
//	//  -(*s_OP).C_OperationMUX.e_Mode
//	(*s_OP).C_OperationMUX.e_Channel = CONTROL_ALL;
//	(*s_OP).C_OperationMUX.e_Mode[CONTROL_MASTER] = CONTROL_REST;
//	(*s_OP).C_OperationMUX.e_Mode[CONTROL_SLAVE] = CONTROL_REST;
//	(*s_OP).C_OperationMUX.e_OldMode[CONTROL_MASTER] = CONTROL_OPERATION_INIT;
//	(*s_OP).C_OperationMUX.e_OldMode[CONTROL_SLAVE] = CONTROL_OPERATION_INIT;
//	//
//	//	Uint16 CONTROL_IntegrationResetMUX(S_CONTROL_INTEGRATION_RESET_MUX *s_Data);
//	//  -(*s_OP).C_IntResetMUX.e_Channel
//	//  -(*s_OP).C_IntResetMUX.e_Command
//	//   0=CONTROL_OFF, 1=CONTROL_ON, 3=CONTROL_COMMAND_INIT
//	(*s_OP).C_IntResetMUX.e_Channel = CONTROL_ALL;
//	(*s_OP).C_IntResetMUX.e_Command[CONTROL_MASTER] = CONTROL_OFF;
//	(*s_OP).C_IntResetMUX.e_Command[CONTROL_SLAVE] = CONTROL_OFF;
//	//
//	//	Uint16 CONTROL_IntegrationSlopMUX(S_CONTROL_INTEGRATION_SOLP_MUX *s_Data);
//	//  -(*s_OP).C_IntSlopMUX.e_Channel
//	//  -(*s_OP).C_IntSlopMUX.e_Slop
//	//
//	(*s_OP).C_IntSlopMUX.e_Channel = CONTROL_ALL;
//	(*s_OP).C_IntSlopMUX.e_Slop[CONTROL_MASTER] = CONTROL_GND;
//	(*s_OP).C_IntSlopMUX.e_Slop[CONTROL_SLAVE] = CONTROL_GND;
//	(*s_OP).C_IntSlopMUX.e_OldSlop[CONTROL_MASTER] = CONTROL_SLOP_INIT;
//	(*s_OP).C_IntSlopMUX.e_OldSlop[CONTROL_SLAVE] = CONTROL_SLOP_INIT;
//	//
//	//	Uint16 CONTROL_IntegrationSlopPMDAC(S_CONTROL_DAC *s_Data);
//	//	-(*s_OP).C_IntSlopPMDAC.e_Channel
//	(*s_OP).C_IntSlopPMDAC.e_Channel = CONTROL_ALL;
//	(*s_OP).C_IntSlopPMDAC.f_Value[CONTROL_MASTER] = 10000;
//	(*s_OP).C_IntSlopPMDAC.f_Value[CONTROL_SLAVE] = 10000;
//	(*s_OP).C_IntSlopPMDAC.f_OldValue[CONTROL_MASTER] = -1.0;
//	(*s_OP).C_IntSlopPMDAC.f_OldValue[CONTROL_SLAVE] = -1.0;
//	//
//	//	Uint16 CONTROL_GateOffsetDAC(S_CONTROL_VDAC *s_Data);
//	//
//	(*s_OP).C_GateOffsetDAC.f_Value = 0.0;
//	(*s_OP).C_GateOffsetDAC.f_Value = 0.0;
//	(*s_OP).C_GateOffsetDAC.f_OldValue = -1.0;
//	(*s_OP).C_GateOffsetDAC.f_OldValue = -1.0;
//	//
//	//	Uint16 CONTROL_GateOutputMUX(S_CONTROL_MUX *s_Data);
//	//  -(*s_OP).C_GateOutputMUX.e_Channel
//	//  -(*s_OP).C_GateOutputMUX.e_Mode
//	(*s_OP).C_GateOutputMUX.e_Channel = CONTROL_ALL;
//	(*s_OP).C_GateOutputMUX.e_Mode[CONTROL_MASTER] = CONTROL_REST;
//	(*s_OP).C_GateOutputMUX.e_Mode[CONTROL_SLAVE] = CONTROL_REST;
//	(*s_OP).C_GateOutputMUX.e_OldMode[CONTROL_MASTER] = CONTROL_OPERATION_INIT;
//	(*s_OP).C_GateOutputMUX.e_OldMode[CONTROL_SLAVE] = CONTROL_OPERATION_INIT;
//	//
//	//	Uint16 CONTROL_GateOutputAMP(S_CONTROL_MUX *s_Data);
//	//  -(*s_OP).C_GateOutputAMP.e_Channel
//	//  -(*s_OP).C_GateOutputAMP.e_Mode
//	(*s_OP).C_GateOutputAMP.e_Channel = CONTROL_ALL;
//	(*s_OP).C_GateOutputAMP.e_Mode[CONTROL_MASTER] = CONTROL_REST;
//	(*s_OP).C_GateOutputAMP.e_Mode[CONTROL_SLAVE] = CONTROL_REST;
//	(*s_OP).C_GateOutputAMP.e_OldMode[CONTROL_MASTER] = CONTROL_OPERATION_INIT;
//	(*s_OP).C_GateOutputAMP.e_OldMode[CONTROL_SLAVE] = CONTROL_OPERATION_INIT;
//	//
//	//	Uint16 CONTROL_VoltageReferenceDAC(S_CONTROL_VDAC *s_Data);
//	//
//	(*s_OP).C_VRefDAC.f_Value = 0.0;
//	(*s_OP).C_VRefDAC.f_OldValue = -1.0;
//	//
//	//	Uint16 CONTROL_CurrentReferenceDAC(S_CONTROL_DAC *s_Data);
//	//  -(*s_OP).C_IRefDAC.e_Channel
//	(*s_OP).C_IRefDAC.e_Channel = CONTROL_ALL;
//	(*s_OP).C_IRefDAC.f_Value[CONTROL_MASTER] = 0.0;
//	(*s_OP).C_IRefDAC.f_Value[CONTROL_SLAVE] = 0.0;
//	(*s_OP).C_IRefDAC.f_OldValue[CONTROL_MASTER] = -1.0;
//	(*s_OP).C_IRefDAC.f_OldValue[CONTROL_SLAVE] = -1.0;
//	//
//	//	Uint16 CONTROL_FINECurrentReferenceDAC(S_CONTROL_DAC *s_Data);
//	//  -(*s_OP).C_FINEIRefDAC.e_Channel
//	//   0=CONTROL_MASTER, 1=CONTROL_SLAVE, 2=CONTROL_ALL, 3=CONTROL_CHANNEL_INIT
//	(*s_OP).C_FINEIRefDAC.e_Channel = CONTROL_ALL;
//	(*s_OP).C_FINEIRefDAC.f_Value[CONTROL_MASTER] = 0.0;
//	(*s_OP).C_FINEIRefDAC.f_Value[CONTROL_SLAVE] = 0.0;
//	(*s_OP).C_FINEIRefDAC.f_OldValue[CONTROL_MASTER] = -1.0;
//	(*s_OP).C_FINEIRefDAC.f_OldValue[CONTROL_SLAVE] = -1.0;
//
//
//	(*s_OP).s_OldADCData = (*s_OP).s_ADCData;
}

S_OPERATION_FILE OPERATION_STATE(void)
{
	S_OPERATION_FILE s_OP;
	s_OP.C_VRELAY = CONTROL_VoltageRELAY(CONTROL_OFF);
	s_OP.C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
	s_OP.C_DIORELAY = CONTROL_DIORELAY(CONTROL_DIO_ALL, CONTROL_OFF);

	s_OP.C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_REST);

	s_OP.C_IntResetMUX = CONTROL_IntegrationResetMUX(CONTROL_ALL, CONTROL_OFF);
	s_OP.C_IntSlopMUX = CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_GND);
	s_OP.C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, 10000);
	s_OP.C_GateOffsetDAC = CONTROL_GateOffsetDAC(0.0);

	s_OP.C_GateOutputMUX = CONTROL_GateOutputMUX(CONTROL_ALL, CONTROL_REST);
	s_OP.C_GateOutputAMP = CONTROL_GateOutputAMP(CONTROL_ALL, CONTROL_REST);

	s_OP.C_VRefDAC = CONTROL_VoltageReferenceDAC(0.0);
	s_OP.C_IRefDAC = CONTROL_CurrentReferenceDAC(CONTROL_ALL, 0.0);
	s_OP.C_FINEIRefDAC = CONTROL_FINECurrentReferenceDAC(CONTROL_ALL, 0.0);

	return(s_OP);
}

S_OPERATION_FILE OPERATION_Run(void)
{
	S_OPERATION_FILE s_OP;
//	CONTROL_VoltageRELAY(&(*s_OP).C_VRELAY);
//	CONTROL_CurrentRELAY(&(*s_OP).C_IRELAY);
//	CONTROL_DIORELAY(&(*s_OP).C_DIORELAY);
//
//	CONTROL_OperationMUX(&(*s_OP).C_OperationMUX);
//	CONTROL_IntegrationResetMUX(&(*s_OP).C_IntResetMUX);
//	CONTROL_IntegrationSlopMUX(&(*s_OP).C_IntSlopMUX);
//	CONTROL_IntegrationSlopPMDAC(&(*s_OP).C_IntSlopPMDAC);
//
//	CONTROL_GateOutputMUX(&(*s_OP).C_GateOutputMUX);
//	CONTROL_GateOutputAMP(&(*s_OP).C_GateOutputAMP);
//	CONTROL_GateOffsetDAC(&(*s_OP).C_GateOffsetDAC);
//
//	CONTROL_VoltageReferenceDAC(&(*s_OP).C_VRefDAC);
//
//	CONTROL_CurrentReferenceDAC(&(*s_OP).C_IRefDAC);
//	CONTROL_FINECurrentReferenceDAC(&(*s_OP).C_FINEIRefDAC);
//
//    SENSING_ADCResultData(&(*s_OP).s_ADCData);
//
//    (*s_OP).s_OldADCData =(*s_OP).s_ADCData;

//	CONTROL_VoltageRELAY(&(*s_OP).C_VRELAY);
//	CONTROL_CurrentRELAY(&(*s_OP).C_IRELAY);
//	CONTROL_DIORELAY(&(*s_OP).C_DIORELAY);
//
//	CONTROL_OperationMUX(&(*s_OP).C_OperationMUX);
//	CONTROL_IntegrationResetMUX(&(*s_OP).C_IntResetMUX);
//	CONTROL_IntegrationSlopMUX(&(*s_OP).C_IntSlopMUX);
//	CONTROL_IntegrationSlopPMDAC(&(*s_OP).C_IntSlopPMDAC);
//
//	CONTROL_GateOutputMUX(&(*s_OP).C_GateOutputMUX);
//	CONTROL_GateOutputAMP(&(*s_OP).C_GateOutputAMP);
//	CONTROL_GateOffsetDAC(&(*s_OP).C_GateOffsetDAC);
//
//	CONTROL_VoltageReferenceDAC(&(*s_OP).C_VRefDAC);
//
//	CONTROL_CurrentReferenceDAC(&(*s_OP).C_IRefDAC);
//	CONTROL_FINECurrentReferenceDAC(&(*s_OP).C_FINEIRefDAC);
//
//    SENSING_ADCResultData(&(*s_OP).s_ADCData);
//
//    (*s_OP).s_OldADCData =(*s_OP).s_ADCData;

	return(s_OP);//SUCCESS Or FAIL
}

S_OPERATION_FILE OPERATION_CellCheck(void)
{
	S_OPERATION_FILE s_OP;
//	OPERATION_Charge(s_OP);
//	DELAY_US(1000);//검사조건
//	OPERATION_Stop(s_OP);
	return(s_OP);//SUCCESS Or FAIL
}

S_OPERATION_FILE OPERATION_Start(void)
{
	S_OPERATION_FILE s_OP;
//	CONTROL_GateOutputMUX(CONTROL_ALL, CONTROL_REST);
//	CONTROL_GateOutputAMP(CONTROL_ALL, CONTROL_REST);
//
//	CONTROL_VoltageRELAY(CONTROL_ON);
//	CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
//
//	CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_GND);
//	CONTROL_IntegrationResetMUX(CONs_OPONTROL_OFF);
//
//	OPERATION_CellCheck(s_OP);
//	if((*s_OP).e_Cell_STATE != OPEARTION_NORMAL)
//	{
//		OPERATION_Stop(s_OP);
//	}
	return(s_OP);//SUCCESS Or FAIL
}

S_OPERATION_FILE OPERATION_Stop(void)
{
//STOP START
	S_OPERATION_FILE s_OP;
//Step 1. Rest all of Mux
	s_OP.C_GateOutputAMP = CONTROL_GateOutputAMP(CONTROL_ALL, CONTROL_REST);
	s_OP.C_GateOutputMUX = CONTROL_GateOutputMUX(CONTROL_ALL, CONTROL_REST);
	s_OP.C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_REST);
//Step 2. Zero of Reference All
	s_OP.C_VRefDAC = CONTROL_VoltageReferenceDAC(0.0);
	s_OP.C_IRefDAC = CONTROL_CurrentReferenceDAC(CONTROL_ALL, 0.0);
	s_OP.C_GateOffsetDAC = CONTROL_GateOffsetDAC(0.0);
//Step 3.Off of Relay All
	s_OP.C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
	s_OP.C_VRELAY = CONTROL_VoltageRELAY(CONTROL_OFF);
//Step 3.Reset of Integration All
	s_OP.C_IntSlopMUX = CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_NONE); //CONTROL_NONE Or CONTROL_GND
	s_OP.C_IntResetMUX = CONTROL_IntegrationResetMUX(CONTROL_ALL, CONTROL_ON);
#ifdef _DEBUG
	DELAY_US(s_DebugFile.Data.IntegrationResetDelayTime);
#endif
#ifndef _DEBUG
	DELAY_US(5);//Test Setting
#endif
	s_OP.C_IntResetMUX = CONTROL_IntegrationResetMUX(CONTROL_ALL, CONTROL_OFF);
//Step 4.Reset of Zero
	s_OP.C_FINEIRefDAC = CONTROL_FINECurrentReferenceDAC(CONTROL_ALL, 0.0);
	s_OP.C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, 10000);
//STOP END
    SENSING_ADCResultData(&s_OP.s_ADCData);
	return(s_OP);
}
//	(*s_OP).ui_UpdateData = 0; //ipc 사용
//
//	(*s_OP).f_Voltage = 0.0;
//	(*s_OP).f_Current = 0.0;
//	(*s_OP).f_Power = 0.0;
//	(*s_OP).e_Cell = OPEARTION_NORMAL;
//	(*s_OP).e_Operation = OPERATION_MODE_STOP;
//
//	//
//	//	S_OPERATION_FILE CONTROL_VoltageRELAY(S_CONTROL_VRELAY *s_Data);
//	//	-(*s_OP).C_VRELAY.e_Command
//	//   0=CONTROL_OFF, 1=CONTROL_ON, 2=CONTROL_COMMAND_INIT
//	(*s_OP).C_VRELAY.e_Command = CONTROL_OFF;
//	(*s_OP).C_VRELAY.e_OldCommand = CONTROL_COMMAND_INIT;
//	//
//	//	Uint16 CONTROL_CurrentRELAY(S_CONTROL_RELAY *s_Data);
//	//	-(*s_OP).C_IRELAY.e_Channel :
//	//   0=CONTROL_MASTER, 1=CONTROL_SLAVE, 2=CONTROL_ALL, 3=CONTROL_CHANNEL_INIT
//	//  -C_IRELAY.e_Command:
//	//   0=CONTROL_OFF, 1=CONTROL_ON, 3=CONTROL_COMMAND_INIT
//	(*s_OP).C_IRELAY.e_Channel = CONTROL_ALL;
//	(*s_OP).C_IRELAY.e_Command[CONTROL_MASTER] = CONTROL_OFF;
//	(*s_OP).C_IRELAY.e_Command[CONTROL_SLAVE] = CONTROL_OFF;
//	(*s_OP).C_IRELAY.e_OldCommand[CONTROL_MASTER] = CONTROL_COMMAND_INIT;
//	(*s_OP).C_IRELAY.e_OldCommand[CONTROL_SLAVE] = CONTROL_COMMAND_INIT;
//	//
//	//	Uint16 CONTROL_DIORELAY(S_CONTROL_DIO *s_Data);
//	//  -(*s_OP).C_DIORELAY.e_Channel
//	//  -(*s_OP).C_DIORELAY.e_Command
//	//   0=CONTROL_OFF, 1=CONTROL_ON, 3=CONTROL_COMMAND_INIT
//	(*s_OP).C_DIORELAY.e_Channel = CONTROL_DIO_ALL;//0=CONTROL_DIO_00, 1=CONTROL_DIO_01, 2=CONTROL_DIO_ALL, 3=CONTROL_DIO_INIT
//	(*s_OP).C_DIORELAY.e_Command[CONTROL_MASTER] = CONTROL_OFF;//0=CONTROL_OFF, 1=CONTROL_ON, 3=CONTROL_COMMAND_INIT
//	(*s_OP).C_DIORELAY.e_Command[CONTROL_SLAVE] = CONTROL_OFF;
//	(*s_OP).C_DIORELAY.e_OldCommand[CONTROL_MASTER] = CONTROL_COMMAND_INIT;
//	(*s_OP).C_DIORELAY.e_OldCommand[CONTROL_SLAVE] = CONTROL_COMMAND_INIT;
//	//
//	//	Uint16 CONTROL_OperationMUX(S_CONTROL_MUX *s_Data);
//	//  -(*s_OP).C_OperationMUX.e_Channel
//	//  -(*s_OP).C_OperationMUX.e_Mode
//	(*s_OP).C_OperationMUX.e_Channel = CONTROL_ALL;
//	(*s_OP).C_OperationMUX.e_Mode[CONTROL_MASTER] = CONTROL_REST;
//	(*s_OP).C_OperationMUX.e_Mode[CONTROL_SLAVE] = CONTROL_REST;
//	(*s_OP).C_OperationMUX.e_OldMode[CONTROL_MASTER] = CONTROL_OPERATION_INIT;
//	(*s_OP).C_OperationMUX.e_OldMode[CONTROL_SLAVE] = CONTROL_OPERATION_INIT;
//	//
//	//	Uint16 CONTROL_IntegrationResetMUX(S_CONTROL_INTEGRATION_RESET_MUX *s_Data);
//	//  -(*s_OP).C_IntResetMUX.e_Channel
//	//  -(*s_OP).C_IntResetMUX.e_Command
//	//   0=CONTROL_OFF, 1=CONTROL_ON, 3=CONTROL_COMMAND_INIT
//	(*s_OP).C_IntResetMUX.e_Channel = CONTROL_ALL;
//	(*s_OP).C_IntResetMUX.e_Command[CONTROL_MASTER] = CONTROL_OFF;
//	(*s_OP).C_IntResetMUX.e_Command[CONTROL_SLAVE] = CONTROL_OFF;
//	(*s_OP).C_IntResetMUX.e_OldCommand[CONTROL_MASTER] = CONTROL_COMMAND_INIT;
//	(*s_OP).C_IntResetMUX.e_OldCommand[CONTROL_SLAVE] = CONTROL_COMMAND_INIT;
//	//
//	//	Uint16 CONTROL_IntegrationSlopMUX(S_CONTROL_INTEGRATION_SOLP_MUX *s_Data);
//	//  -(*s_OP).C_IntSlopMUX.e_Channel
//	//  -(*s_OP).C_IntSlopMUX.e_Slop
//	//
//	(*s_OP).C_IntSlopMUX.e_Channel = CONTROL_ALL;
//	(*s_OP).C_IntSlopMUX.e_Slop[CONTROL_MASTER] = CONTROL_GND;
//	(*s_OP).C_IntSlopMUX.e_Slop[CONTROL_SLAVE] = CONTROL_GND;
//	(*s_OP).C_IntSlopMUX.e_OldSlop[CONTROL_MASTER] = CONTROL_SLOP_INIT;
//	(*s_OP).C_IntSlopMUX.e_OldSlop[CONTROL_SLAVE] = CONTROL_SLOP_INIT;
//	//
//	//	Uint16 CONTROL_IntegrationSlopPMDAC(S_CONTROL_DAC *s_Data);
//	//	-(*s_OP).C_IntSlopPMDAC.e_Channel
//	(*s_OP).C_IntSlopPMDAC.e_Channel = CONTROL_ALL;
//	(*s_OP).C_IntSlopPMDAC.f_Value[CONTROL_MASTER] = 10000;
//	(*s_OP).C_IntSlopPMDAC.f_Value[CONTROL_SLAVE] = 10000;
//	(*s_OP).C_IntSlopPMDAC.f_OldValue[CONTROL_MASTER] = -1.0;
//	(*s_OP).C_IntSlopPMDAC.f_OldValue[CONTROL_SLAVE] = -1.0;
//	//
//	//	Uint16 CONTROL_GateOffsetDAC(S_CONTROL_VDAC *s_Data);
//	//
//	(*s_OP).C_GateOffsetDAC.f_Value = 0.0;
//	(*s_OP).C_GateOffsetDAC.f_Value = 0.0;
//	(*s_OP).C_GateOffsetDAC.f_OldValue = -1.0;
//	(*s_OP).C_GateOffsetDAC.f_OldValue = -1.0;
//	//
//	//	Uint16 CONTROL_GateOutputMUX(S_CONTROL_MUX *s_Data);
//	//  -(*s_OP).C_GateOutputMUX.e_Channel
//	//  -(*s_OP).C_GateOutputMUX.e_Mode
//	(*s_OP).C_GateOutputMUX.e_Channel = CONTROL_ALL;
//	(*s_OP).C_GateOutputMUX.e_Mode[CONTROL_MASTER] = CONTROL_REST;
//	(*s_OP).C_GateOutputMUX.e_Mode[CONTROL_SLAVE] = CONTROL_REST;
//	(*s_OP).C_GateOutputMUX.e_OldMode[CONTROL_MASTER] = CONTROL_OPERATION_INIT;
//	(*s_OP).C_GateOutputMUX.e_OldMode[CONTROL_SLAVE] = CONTROL_OPERATION_INIT;
//	//
//	//	Uint16 CONTROL_GateOutputAMP(S_CONTROL_MUX *s_Data);
//	//  -(*s_OP).C_GateOutputAMP.e_Channel
//	//  -(*s_OP).C_GateOutputAMP.e_Mode
//	(*s_OP).C_GateOutputAMP.e_Channel = CONTROL_ALL;
//	(*s_OP).C_GateOutputAMP.e_Mode[CONTROL_MASTER] = CONTROL_REST;
//	(*s_OP).C_GateOutputAMP.e_Mode[CONTROL_SLAVE] = CONTROL_REST;
//	(*s_OP).C_GateOutputAMP.e_OldMode[CONTROL_MASTER] = CONTROL_OPERATION_INIT;
//	(*s_OP).C_GateOutputAMP.e_OldMode[CONTROL_SLAVE] = CONTROL_OPERATION_INIT;
//	//
//	//	Uint16 CONTROL_VoltageReferenceDAC(S_CONTROL_VDAC *s_Data);
//	//
//	(*s_OP).C_VRefDAC.f_Value = 0.0;
//	(*s_OP).C_VRefDAC.f_OldValue = -1.0;
//	//
//	//	Uint16 CONTROL_CurrentReferenceDAC(S_CONTROL_DAC *s_Data);
//	//  -(*s_OP).C_IRefDAC.e_Channel
//	(*s_OP).C_IRefDAC.e_Channel = CONTROL_ALL;
//	(*s_OP).C_IRefDAC.f_Value[CONTROL_MASTER] = 0.0;
//	(*s_OP).C_IRefDAC.f_Value[CONTROL_SLAVE] = 0.0;
//	(*s_OP).C_IRefDAC.f_OldValue[CONTROL_MASTER] = -1.0;
//	(*s_OP).C_IRefDAC.f_OldValue[CONTROL_SLAVE] = -1.0;
//	//
//	//	Uint16 CONTROL_FINECurrentReferenceDAC(S_CONTROL_DAC *s_Data);
//	//  -(*s_OP).C_FINEIRefDAC.e_Channel
//	//   0=CONTROL_MASTER, 1=CONTROL_SLAVE, 2=CONTROL_ALL, 3=CONTROL_CHANNEL_INIT
//	(*s_OP).C_FINEIRefDAC.e_Channel = CONTROL_ALL;
//	(*s_OP).C_FINEIRefDAC.f_Value[CONTROL_MASTER] = 0.0;
//	(*s_OP).C_FINEIRefDAC.f_Value[CONTROL_SLAVE] = 0.0;
//	(*s_OP).C_FINEIRefDAC.f_OldValue[CONTROL_MASTER] = -1.0;
//	(*s_OP).C_FINEIRefDAC.f_OldValue[CONTROL_SLAVE] = -1.0;
//
//
//	(*s_OP).s_OldADCData = (*s_OP).s_ADCData;
//	return(SUCCESS);//SUCCESS Or FAIL
//}
S_OPERATION_FILE OPERATION_Alram(void)
{
	S_OPERATION_FILE s_OP;
	return(s_OP);//SUCCESS Or FAIL
}

S_OPERATION_FILE OPERATION_Emergency(void)
{
	S_OPERATION_FILE s_OP;
	return(s_OP);//SUCCESS Or FAIL
}

S_OPERATION_FILE OPERATION_Reset(void)
{
	S_OPERATION_FILE s_OP;
	return(s_OP);//SUCCESS Or FAIL
}

S_OPERATION_FILE OPERATION_Charge(void)
{
	S_OPERATION_FILE sp_OP;

	sp_OP.C_IntSlopMUX = CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_NONE);
	sp_OP.C_IntResetMUX = CONTROL_IntegrationResetMUX(CONTROL_ALL, CONTROL_ON);
	DELAY_US(1000);
	sp_OP.C_IntResetMUX = CONTROL_IntegrationResetMUX(CONTROL_ALL, CONTROL_OFF);
	sp_OP.C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, 10000);

	sp_OP.C_VRELAY = CONTROL_VoltageRELAY(CONTROL_ON);
	sp_OP.C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_ON);

	sp_OP.C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_CHARGE);

	sp_OP.C_GateOffsetDAC = CONTROL_GateOffsetDAC(0.0);
	sp_OP.C_VRefDAC = CONTROL_VoltageReferenceDAC(3.7);
	sp_OP.C_IRefDAC = CONTROL_CurrentReferenceDAC(CONTROL_ALL, 1.0);
//	sp_OP.C_FINEIRefDAC = CONTROL_FINECurrentReferenceDAC(CONTROL_ALL, 0.0);

	sp_OP.C_GateOutputMUX = CONTROL_GateOutputMUX(CONTROL_ALL, CONTROL_CHARGE);
	sp_OP.C_GateOutputAMP = CONTROL_GateOutputAMP(CONTROL_ALL, CONTROL_CHARGE);
	sp_OP.C_IntSlopMUX = CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_PM);

	return(sp_OP);//SUCCESS Or FAIL
//	OPERATION_ChannelControl(sp_OP);
//
//	CONTROL_VoltageRELAY(CONTROL_OFF);
//	CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
//	CONTROL_DIORELAY(CONTROL_DIO_ALL, CONTROL_OFF);
//
//	CONTROL_OperationMUX(CONTROL_ALL, CONTROL_REST);
//
//	CONTROL_IntegrationResetMUX(CONTROL_ALL,  CONTROL_OFF);
//	CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_GND);
//	CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, 10000.0);
//	CONTROL_GateOffsetDAC(0.0);
//
//	CONTROL_GateOutputMUX(CONTROL_ALL, CONTROL_REST);
//	CONTROL_GateOutputAMP(CONTROL_ALL, CONTROL_REST);
//
//	(*sp_OP).Old.f_VoltageRefDAC = CONTROL_VoltageReferenceDAC((*sp_OP).f_VoltageReferenceDAC_DATA);
//
//	(*sp_OP).Old.f_CurrentRefDAC[(*sp_OP).Old.e_ChargeMode] = CONTROL_CurrentReferenceDAC(CONTROL_ALL,
//															  (*sp_OP).New.f_CurrentRefDAC[(*sp_OP).Old.e_ChargeMode]);
//
//	(*sp_OP).Old.FINECurrentReferenceDAC_STATE = CONTROL_FINECurrentReferenceDAC(CONTROL_ALL,
//												   (*sp_OP).FINECurrentReferenceDAC_DATA[(*sp_OP).e_Mode_STATE]);

//	return(SUCCESS);//SUCCESS Or FAIL
}
S_OPERATION_FILE OPERATION_Discharge(void)
{
	S_OPERATION_FILE sp_OP;

	sp_OP.C_IntSlopMUX = CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_NONE);
	sp_OP.C_IntResetMUX = CONTROL_IntegrationResetMUX(CONTROL_ALL, CONTROL_ON);
	DELAY_US(1000);
	sp_OP.C_IntResetMUX = CONTROL_IntegrationResetMUX(CONTROL_ALL, CONTROL_OFF);
	sp_OP.C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, 10000);

	sp_OP.C_VRELAY = CONTROL_VoltageRELAY(CONTROL_ON);
	sp_OP.C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_ON);

	sp_OP.C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_DISCHARGE);

	sp_OP.C_GateOffsetDAC = CONTROL_GateOffsetDAC(0.0);
	sp_OP.C_VRefDAC = CONTROL_VoltageReferenceDAC(3.7);
	sp_OP.C_IRefDAC = CONTROL_CurrentReferenceDAC(CONTROL_ALL, 1.0);
//	sp_OP.C_FINEIRefDAC = CONTROL_FINECurrentReferenceDAC(CONTROL_ALL, 0.0);

	sp_OP.C_GateOutputMUX = CONTROL_GateOutputMUX(CONTROL_ALL, CONTROL_DISCHARGE);
	sp_OP.C_GateOutputAMP = CONTROL_GateOutputAMP(CONTROL_ALL, CONTROL_DISCHARGE);
	sp_OP.C_IntSlopMUX = CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_PM);

	return(sp_OP);//SUCCESS Or FAIL
//	OPERATION_ChannelControl(sp_OP);
//
//	CONTROL_VoltageRELAY(CONTROL_OFF);
//	CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
//	CONTROL_DIORELAY(CONTROL_DIO_ALL, CONTROL_OFF);
//
//	CONTROL_OperationMUX(CONTROL_ALL, CONTROL_REST);
//
//	CONTROL_IntegrationResetMUX(CONTROL_ALL,  CONTROL_OFF);
//	CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_GND);
//	CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, 10000.0);
//	CONTROL_GateOffsetDAC(0.0);
//
//	CONTROL_GateOutputMUX(CONTROL_ALL, CONTROL_REST);
//	CONTROL_GateOutputAMP(CONTROL_ALL, CONTROL_REST);
//
//	(*sp_OP).Old.f_VoltageRefDAC = CONTROL_VoltageReferenceDAC((*sp_OP).f_VoltageReferenceDAC_DATA);
//
//	(*sp_OP).Old.f_CurrentRefDAC[(*sp_OP).Old.e_ChargeMode] = CONTROL_CurrentReferenceDAC(CONTROL_ALL,
//															  (*sp_OP).New.f_CurrentRefDAC[(*sp_OP).Old.e_ChargeMode]);
//
//	(*sp_OP).Old.FINECurrentReferenceDAC_STATE = CONTROL_FINECurrentReferenceDAC(CONTROL_ALL,
//												   (*sp_OP).FINECurrentReferenceDAC_DATA[(*sp_OP).e_Mode_STATE]);

//	return(SUCCESS);//SUCCESS Or FAIL
}
S_OPERATION_FILE OPERATION_CC(void)
{
	S_OPERATION_FILE s_OP;
	return(s_OP);//SUCCESS Or FAIL
}

S_OPERATION_FILE OPERATION_CV(void)
{
	S_OPERATION_FILE s_OP;
	return(s_OP);//SUCCESS Or FAIL
}

S_OPERATION_FILE OPERATION_CP(void)
{
	S_OPERATION_FILE s_OP;
	return(s_OP);//SUCCESS Or FAIL
}

S_OPERATION_FILE OPERATION_Nomal(void)
{
	S_OPERATION_FILE s_OP;
	return(s_OP);//SUCCESS Or FAIL
}

S_OPERATION_FILE OPERATION_Pattern(void)
{
	S_OPERATION_FILE s_OP;
	return(s_OP);//SUCCESS Or FAIL
}

S_OPERATION_FILE OPERATION_Parallel(void)
{
	S_OPERATION_FILE s_OP;
	return(s_OP);//SUCCESS Or FAIL
}

S_OPERATION_FILE OPERATION_Sensing(void)
{
	S_OPERATION_FILE s_OP;
	return(s_OP);//SUCCESS Or FAIL
}

S_OPERATION_FILE OPERATION_ChannelControl(void)
{
	S_OPERATION_FILE s_OP;
	return(s_OP);//SUCCESS Or FAIL
}

S_OPERATION_FILE OPERATION_IntegrationSlopControl(void)
{
	S_OPERATION_FILE s_OP;
	return(s_OP);//SUCCESS Or FAIL
}

S_OPERATION_FILE OPERATION_GateOffsetControl(void)
{
	S_OPERATION_FILE s_OP;
	return(s_OP);//SUCCESS Or FAIL
}

S_OPERATION_FILE OPEARTION_CurrentFineControl(void)
{
	S_OPERATION_FILE s_OP;
	return(s_OP);//SUCCESS Or FAIL
}

//
// End of file
//

