/*
 * PhoenixonControls_TEST.c
 *
 *  Created on: 2018. 5. 20.
 *      Author: BGKim
 */
#include "../../../common/include/F28x_Project.h"
#include "../../../common/include/driverlib.h"
#include "../../../common/include/device.h"
#ifdef _DEBUG

S_DEBUG_FILE s_DebugFile;

void DEBUG_Inint(S_DEBUG_FILE *sp_Data)
{
	//DATA
	(*sp_Data).Data.IntegrationResetDelayTime = 10;

	//CONTROL
	(*sp_Data).Control.e_VoltageRELAYCommand = CONTROL_OFF;
	(*sp_Data).Control.e_CurrentRELAYChannel = CONTROL_ALL;
	(*sp_Data).Control.e_CurrentRELAYCommand = CONTROL_OFF;
	(*sp_Data).Control.e_DIORELAYChannel = CONTROL_DIO_ALL;
	(*sp_Data).Control.e_DIORELAYCommand = CONTROL_OFF;
	(*sp_Data).Control.e_OperationMUXChannel = CONTROL_ALL;
	(*sp_Data).Control.e_OperationMUXMode = CONTROL_REST;
	(*sp_Data).Control.e_IntegrationResetMUXChannel = CONTROL_ALL;
	(*sp_Data).Control.e_IntegrationResetMUXCommand = CONTROL_OFF;
	(*sp_Data).Control.e_IntegrationSlopMUXChannel = CONTROL_ALL;
	(*sp_Data).Control.e_IntegrationSlopMUXSlop = CONTROL_GND;
	(*sp_Data).Control.e_IntegrationSlopPMChannel = CONTROL_ALL;
	(*sp_Data).Control.f_IntegrationSlopPMPotentialValue = 10000;
	(*sp_Data).Control.f_IntegrationOffsetDACVolatageValue = 0.0;
	(*sp_Data).Control.e_GateOutputMUXChannel = CONTROL_ALL;
	(*sp_Data).Control.e_GateOutputMUXMode = CONTROL_REST;
	(*sp_Data).Control.e_GateOutputAMPChannel = CONTROL_ALL;
	(*sp_Data).Control.e_GateOutputAMPMode = CONTROL_REST;
	(*sp_Data).Control.f_VoltageReferenceDACVolatageValue = 0.0;
	(*sp_Data).Control.e_CurrentReferenceDACChannel = CONTROL_ALL;
	(*sp_Data).Control.f_CurrentReferenceDACCurrentValue = 0.0;
	(*sp_Data).Control.e_FINECurrentReferenceDACChannel = CONTROL_ALL;
	(*sp_Data).Control.f_FINECurrentReferenceDACCurrentValue = 0.0;
}

S_OPERATION_FILE DEBUG_Operation(S_DEBUG_FILE *sp_Data)
{
	S_OPERATION_FILE s_OP;
    s_OP.C_VRELAY = CONTROL_VoltageRELAY((*sp_Data).Control.e_VoltageRELAYCommand);
    s_OP.C_IRELAY = CONTROL_CurrentRELAY((*sp_Data).Control.e_CurrentRELAYChannel, (*sp_Data).Control.e_CurrentRELAYCommand);
    s_OP.C_DIORELAY = CONTROL_DIORELAY((*sp_Data).Control.e_DIORELAYChannel, (*sp_Data).Control.e_DIORELAYCommand);
    s_OP.C_OperationMUX = CONTROL_OperationMUX((*sp_Data).Control.e_OperationMUXChannel, (*sp_Data).Control.e_OperationMUXMode);
    s_OP.C_IntResetMUX = CONTROL_IntegrationResetMUX((*sp_Data).Control.e_IntegrationResetMUXChannel, (*sp_Data).Control.e_IntegrationResetMUXCommand);
    s_OP.C_IntSlopMUX = CONTROL_IntegrationSlopMUX((*sp_Data).Control.e_IntegrationSlopMUXChannel, (*sp_Data).Control.e_IntegrationSlopMUXSlop);
    s_OP.C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC((*sp_Data).Control.e_IntegrationSlopPMChannel, (*sp_Data).Control.f_IntegrationSlopPMPotentialValue);
    s_OP.C_GateOffsetDAC = CONTROL_GateOffsetDAC((*sp_Data).Control.f_IntegrationOffsetDACVolatageValue);
    s_OP.C_GateOutputMUX = CONTROL_GateOutputMUX((*sp_Data).Control.e_GateOutputMUXChannel, (*sp_Data).Control.e_GateOutputMUXMode);
    s_OP.C_GateOutputAMP = CONTROL_GateOutputAMP((*sp_Data).Control.e_GateOutputAMPChannel, (*sp_Data).Control.e_GateOutputAMPMode);
    s_OP.C_VRefDAC = CONTROL_VoltageReferenceDAC((*sp_Data).Control.f_VoltageReferenceDACVolatageValue);
    s_OP.C_IRefDAC = CONTROL_CurrentReferenceDAC((*sp_Data).Control.e_CurrentReferenceDACChannel, (*sp_Data).Control.f_CurrentReferenceDACCurrentValue);
    s_OP.C_FINEIRefDAC = CONTROL_FINECurrentReferenceDAC((*sp_Data).Control.e_FINECurrentReferenceDACChannel, (*sp_Data).Control.f_FINECurrentReferenceDACCurrentValue);
    SENSING_ADCResultData(&s_OP.s_ADCData);
    return(s_OP);
}
//#define UART_PRINTF

#ifdef UART_PRINTF
int fputc(int _c, register FILE *_fp);
int fputs(const char *_ptr, register FILE *_fp);
#endif

#ifdef UART_PRINTF
int fputc(int _c, register FILE *_fp)
{
//  while(!(UCA1IFG&UCTXIFG));
//  UCA1TXBUF = (unsigned char) _c;

  return((unsigned char)_c);
}

int fputs(const char *_ptr, register FILE *_fp)
{
  unsigned int i, len;

  len = strlen(_ptr);

  for(i=0 ; i<len ; i++)
  {
//    while(!(UCA1IFG&UCTXIFG));
//    UCA1TXBUF = (unsigned char) _ptr[i];
  }

  return len;
}
#endif

//memcpy(void *s1, const void *s2, size_t n);
//
//매개변수
//
//s1
//  새 버퍼입니다.
//s2
//  복사할 버퍼입니다.
//n
//  복사할 문자 수입니다.


#endif

//
// End of file
//

