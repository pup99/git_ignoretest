/*
 * PhoenixonControls_ALARM.c
 *
 *  Created on: 2018. 5. 20.
 *      Author: BGKim
 */
#include "../../../common/include/F28x_Project.h"
#include "../../../common/include/driverlib.h"
#include "../../../common/include/device.h"

#pragma CODE_SECTION(ALARM_VoltageFault, ".TI.ramfunc");
#pragma CODE_SECTION(ALARM_CurrentFault, ".TI.ramfunc");
#pragma CODE_SECTION(ALARM_VolatageInnerFault, ".TI.ramfunc");
#pragma CODE_SECTION(ALARM_VoltageReferenceFault, ".TI.ramfunc");
#pragma CODE_SECTION(ALARM_CurrentReferenceFault, ".TI.ramfunc");
#pragma CODE_SECTION(ALARM_N02VPowerFault, ".TI.ramfunc");
#pragma CODE_SECTION(ALARM_P07VPowerFault, ".TI.ramfunc");
#pragma CODE_SECTION(ALARM_P05VPowerFault, ".TI.ramfunc");
#pragma CODE_SECTION(ALARM_P15VPowerFault, ".TI.ramfunc");
#pragma CODE_SECTION(ALARM_N15VPowerFault, ".TI.ramfunc");
#pragma CODE_SECTION(ALARM_TemperatureFault, ".TI.ramfunc");
#pragma CODE_SECTION(ALARM_PowerConditionCheck, ".TI.ramfunc");
#pragma CODE_SECTION(ALARM_LineConditionCheck, ".TI.ramfunc");
#pragma CODE_SECTION(ALARM_CurrentConditionCheck, ".TI.ramfunc");
#pragma CODE_SECTION(ALARM_VoltageConditionCheck, ".TI.ramfunc");
#pragma CODE_SECTION(ALARM_HeatConditionCheck, ".TI.ramfunc");
#pragma CODE_SECTION(ALARM_CommunicationConditionCheck, ".TI.ramfunc");
#pragma CODE_SECTION(ALARM_BatteryConditionCheck, ".TI.ramfunc");
#pragma CODE_SECTION(ALARM_BatteryFaultCheck, ".TI.ramfunc");
#pragma CODE_SECTION(ALARM_BatteryNoneCheck, ".TI.ramfunc");
#pragma CODE_SECTION(ALARM_Emergency, ".TI.ramfunc");

Uint16 ALARM_VoltageFault(S_OPERATION_FILE* sp_OP)
{
	//전압값이 부호가 다를때 알람 발생
	static Uint16 CheckTime = 0;
	static Uint16 SetTime = 0;
	static Uint16 CommandFlagOld = 0;

	if(((*sp_OP).VARIABLE_TimeSpanResult & 0xA) == (CheckTime & 0xA))CheckTime += 1 ;

	if((*sp_OP).COMMAND == COMMAND_REST)
	{
		if((*sp_OP).s_OldADCData.SENSING_BatteryVoltage > ALARM_VOLTAGE_LIMIT)
		{
			if(SetTime == CLEAR)CheckTime = CLEAR;
			SetTime++;
		}
	}
	else if((*sp_OP).COMMAND == COMMAND_CHARGE)
	{	//충전시 전압 음수, 전류는 양수 측정됨
		if((*sp_OP).s_OldADCData.SENSING_BatteryVoltage > ALARM_VOLTAGE_LIMIT)// 역전압 측정
		{
			if(SetTime == CLEAR)CheckTime = CLEAR;
			SetTime++;
		}
	}
	else if((*sp_OP).COMMAND == COMMAND_DISCHARGE)
	{	//방전시 전압 양수, 전류는 음수 측정됨
		if((*sp_OP).s_OldADCData.SENSING_BatteryVoltage < (ALARM_VOLTAGE_LIMIT * (float)(-1.0))) // 역전압 측정
		{
			if(SetTime == CLEAR)CheckTime = CLEAR;
			SetTime++;
		}
	}

    if(ABS_F((*sp_OP).s_OldADCData.SENSING_BatteryVoltage) >= ALARM_OVER_VOLTAGE_LIMIT) // 초과전압 측정
    {
        if(SetTime == CLEAR)CheckTime = CLEAR;
        SetTime++;
    }

	if(CheckTime >= ALARM_VOLTAGE_TIME)
	{
		CheckTime = CLEAR;
		SetTime   = CLEAR;
	}

	if(CommandFlagOld != (*sp_OP).COMMAND)
	{
		CheckTime = CLEAR;
		SetTime   = CLEAR;
	}
	CommandFlagOld = (*sp_OP).COMMAND;

	if(SetTime >= (ALARM_VOLTAGE_TIME / 2))
	{
		SetTime = CLEAR;
		//SetTime = (ALARM_VOLTAGE_TIME / 2);
		(*sp_OP).VARIABLE_ALARM_WARING.bit.VoltageFault = SET;
		//return (SET);
	}

	return((*sp_OP).VARIABLE_ALARM_WARING.bit.VoltageFault); //SET Or CLEAR
}

Uint16 ALARM_CurrentFault(S_OPERATION_FILE* sp_OP)
{
	static Uint16 CheckTime = 0;
	static Uint16 SetTime = 0;
	static Uint16 CommandFlagOld = 0;
	float32 CurrentRefTemp = 0;

    if ((*sp_OP).OPERATIN == OP_CHARGE_CP || (*sp_OP).OPERATIN == OP_DISCHARGE_CP || (*sp_OP).OPERATIN == OP_CHARGE_CPCV || (*sp_OP).OPERATIN == OP_DISCHARGE_CPCV) {
        CurrentRefTemp = (*sp_OP).VARIABLE_PowerRef;
    } else if (sp_OP->OPERATIN == OP_CHARGE_CR || sp_OP->OPERATIN == OP_DISCHARGE_CR) {
        CurrentRefTemp = (*sp_OP).VARIABLE_ResistanceRef;
    } else {
        CurrentRefTemp = (*sp_OP).VARIABLE_CurrentRef;
    }

	if(((*sp_OP).VARIABLE_TimeSpanResult & 0xA) == (CheckTime & 0xA))CheckTime += 1 ;

	if((*sp_OP).COMMAND == COMMAND_REST)
	{
		if(ABS_F((*sp_OP).s_OldADCData.SENSING_Current) > ALARM_CURRENT_LIMIT)
		{
			if(SetTime == CLEAR)CheckTime = CLEAR;
			SetTime++;
		}
	}
	else if((*sp_OP).COMMAND == COMMAND_CHARGE)
	{	//충전시 전압 음수, 전류는 양수 측정됨
		if((*sp_OP).s_OldADCData.SENSING_Current > (CurrentRefTemp + ALARM_CURRENT_LIMIT) ||//초과전류 측정
		   (*sp_OP).s_OldADCData.SENSING_Current < (ALARM_CURRENT_LIMIT * (float)(-1.0)))//역전류 측정
		{
			if(SetTime == CLEAR)CheckTime = CLEAR;
			SetTime++;
		}
	}
	else if((*sp_OP).COMMAND == COMMAND_DISCHARGE)
	{	//방전시 전압 양수, 전류는 음수 측정됨
		if((*sp_OP).s_OldADCData.SENSING_Current < ((CurrentRefTemp + ALARM_CURRENT_LIMIT) * (float)(-1.0)) ||//초과전류 측정
		   (*sp_OP).s_OldADCData.SENSING_Current > ALARM_CURRENT_LIMIT)//역전류 측정
		{
			if(SetTime == CLEAR)CheckTime = CLEAR;
			SetTime++;
		}
	}

	if(CheckTime >= ALARM_CURRENT_TIME)
	{
		CheckTime = CLEAR;
		SetTime   = CLEAR;
	}

	if(CommandFlagOld != (*sp_OP).COMMAND)
	{
		CheckTime = CLEAR;
		SetTime   = CLEAR;
	}
	CommandFlagOld = (*sp_OP).COMMAND;

	if(SetTime >= (ALARM_CURRENT_TIME / 2))
	{
        SetTime = CLEAR;
//	    SetTime = (ALARM_CURRENT_TIME / 2);
		(*sp_OP).VARIABLE_ALARM_WARING.bit.CurrentFault = SET;
//		return (SET);
	}

	return((*sp_OP).VARIABLE_ALARM_WARING.bit.CurrentFault); //SET Or CLEAR
}

Uint16 ALARM_VolatageInnerFault(S_OPERATION_FILE* sp_OP)
{
	static Uint16 CheckTime = 0;
	static Uint16 SetTime = 0;
	static Uint16 CommandFlagOld = 0;

	if(((*sp_OP).VARIABLE_TimeSpanResult & 0xA) == (CheckTime & 0xA))CheckTime += 1 ;

	if((*sp_OP).COMMAND == COMMAND_REST)
	{	//휴지시 전압 음수, 전류는 양수 측정됨
		if((*sp_OP).s_OldADCData.SENSING_BatteryVoltage > (((*sp_OP).s_OldADCData.SENSING_InnerBatteryVoltage * (float)(-0.1)) + ALARM_VOLTAGE_LIMIT) &&//초과전압 측정
		   (*sp_OP).s_OldADCData.SENSING_BatteryVoltage < (((*sp_OP).s_OldADCData.SENSING_InnerBatteryVoltage * (float)(-0.1)) - ALARM_VOLTAGE_LIMIT))//미만전압 측정
		{
			if(SetTime == CLEAR)CheckTime = CLEAR;
			SetTime++;
		}
	}
	else if((*sp_OP).COMMAND == COMMAND_CHARGE)
	{	//충전시 전압 음수, 전류는 양수 측정됨
		if((*sp_OP).s_OldADCData.SENSING_BatteryVoltage > (((*sp_OP).s_OldADCData.SENSING_InnerBatteryVoltage * (float)(-0.1)) + ALARM_VOLTAGE_LIMIT) &&//초과전압 측정
		   (*sp_OP).s_OldADCData.SENSING_BatteryVoltage < (((*sp_OP).s_OldADCData.SENSING_InnerBatteryVoltage * (float)(-0.1)) - ALARM_VOLTAGE_LIMIT))//미만전압 측정
		{
			if(SetTime == CLEAR)CheckTime = CLEAR;
			SetTime++;
		}
	}
	else if((*sp_OP).COMMAND == COMMAND_DISCHARGE)
	{	//방전시 전압 양수, 전류는 음수 측정됨
		if((*sp_OP).s_OldADCData.SENSING_BatteryVoltage > ((*sp_OP).s_OldADCData.SENSING_InnerBatteryVoltage + ALARM_VOLTAGE_LIMIT) &&//초과전압 측정
		   (*sp_OP).s_OldADCData.SENSING_BatteryVoltage < ((*sp_OP).s_OldADCData.SENSING_InnerBatteryVoltage - ALARM_VOLTAGE_LIMIT))//미만전압 측정
		{
			if(SetTime == CLEAR)CheckTime = CLEAR;
			SetTime++;
		}
	}

	if(CheckTime >= ALARM_INNER_VOLTAGE_TIME)
	{
		CheckTime = CLEAR;
		SetTime   = CLEAR;
	}

	if(CommandFlagOld != (*sp_OP).COMMAND)
	{
		CheckTime = CLEAR;
		SetTime   = CLEAR;
	}
	CommandFlagOld = (*sp_OP).COMMAND;

	if(SetTime >= (ALARM_INNER_VOLTAGE_TIME / 2))
	{
	    SetTime = CLEAR;
//		SetTime = (ALARM_INNER_VOLTAGE_TIME / 2);
		(*sp_OP).VARIABLE_ALARM_GRID.bit.LineFault = SET;
//		return (SET);
	}

	return((*sp_OP).VARIABLE_ALARM_GRID.bit.LineFault); //SET Or CLEAR
}

Uint16 ALARM_VoltageReferenceFault(S_OPERATION_FILE* sp_OP)
{
	return(CLEAR); //SET Or CLEAR
}

Uint16 ALARM_CurrentReferenceFault(S_OPERATION_FILE* sp_OP)
{
	return(CLEAR); //SET Or CLEAR
}

Uint16 ALARM_CC_Fault(S_OPERATION_FILE* sp_OP)
{
	//배열로 무빙하고 무비 첫 데이타 더할때 마지막데이타 빼주고그값이CC때 항상 유지가 안되면 펄트
	//이건 발진으로 확인 할 수 있다네 ㅋㅋ
	static Uint16 MovingCounter = 0;
	static Uint16 MovingStartFlag = 0;
	static float32 DataSum = 0.0;
	float32	DataNew = 0.0;
	float32 DtatResult = 0.0;
	float32 CurrentRefTemp = 0;

	static Uint16 CheckTime = 0;
	static Uint16 SetTime = 0;
	static Uint16 CommandFlagOld = 0;

    if ((*sp_OP).OPERATIN == OP_CHARGE_CP || (*sp_OP).OPERATIN == OP_DISCHARGE_CP || (*sp_OP).OPERATIN == OP_CHARGE_CPCV || (*sp_OP).OPERATIN == OP_DISCHARGE_CPCV) {
        CurrentRefTemp = (*sp_OP).VARIABLE_PowerRef;
    } else if (sp_OP->OPERATIN == OP_CHARGE_CR || sp_OP->OPERATIN == OP_DISCHARGE_CR) {
        CurrentRefTemp = (*sp_OP).VARIABLE_ResistanceRef;
    } else {
        CurrentRefTemp = (*sp_OP).VARIABLE_CurrentRef;
    }

	if(((*sp_OP).VARIABLE_TimeSpanResult & 0xA) == (CheckTime & 0xA))CheckTime += 1 ;

	if((*sp_OP).VARIABLE_OPERATIN == OPERATION_SEND_CHARGE_CC	    ||
	   (*sp_OP).VARIABLE_OPERATIN == OPERATION_SEND_DISCHARGE_CC	||
	   (*sp_OP).VARIABLE_OPERATIN == OPERATION_SEND_CHARGE_CC_CP	||
	   (*sp_OP).VARIABLE_OPERATIN == OPERATION_SEND_DISCHARGE_CC_CP)
	{
		//이동평균 시작
		DataNew = (*sp_OP).s_OldADCData.SENSING_Current;
//		(*sp_OP).CC_MovingData[MovingCounter] = DataNew;
		DataSum +=DataNew;

		if(++MovingCounter >= (ALARM_CC_TIME / 4) - 1)
		{
			MovingCounter = CLEAR;
			MovingStartFlag = SET;
		}
		if(MovingStartFlag == SET)
		{
			DtatResult = (float)((float)(DataSum) / (float)(ALARM_CC_TIME / 4));
			MovingCounter = (ALARM_CC_TIME / 4);
			DataSum -= (*sp_OP).CC_MovingData[MovingCounter];//이동평균 마지막값 제외
		}
		//이동평균 끝
		if((*sp_OP).COMMAND == COMMAND_CHARGE)
		{	//충전시 전압 마이너스, 전류는 플러스 센싱됨
			if(DtatResult > (CurrentRefTemp + ALARM_CURRENT_LIMIT) ||//초과전류 체크
			   DtatResult < (CurrentRefTemp - ALARM_CURRENT_LIMIT))//미만전류 체크
			{
				if(SetTime == CLEAR)CheckTime = CLEAR;
				SetTime++;
			}
		}
		else if((*sp_OP).COMMAND == COMMAND_DISCHARGE)
		{	//방전시 전압 마이너스, 전류는 플러스 센싱됨
			if(DtatResult < ((CurrentRefTemp + ALARM_CURRENT_LIMIT) * (float)(-1.0)) ||//초과전류 체크
			   DtatResult > ((CurrentRefTemp - ALARM_CURRENT_LIMIT) * (float)(-1.0)))//미만전류 체크
			{
				if(SetTime == CLEAR)CheckTime = CLEAR;
				SetTime++;
			}
		}
	}

	if(CheckTime >= ALARM_CC_TIME)
	{
		CheckTime = CLEAR;
		SetTime   = CLEAR;
	}

	if(CommandFlagOld != (*sp_OP).COMMAND)
	{
		CheckTime = CLEAR;
		SetTime   = CLEAR;
	}
	CommandFlagOld = (*sp_OP).COMMAND;

	if(SetTime >= (ALARM_CC_TIME / 2))
	{
	    SetTime = CLEAR;
//		SetTime = (ALARM_CC_TIME / 2);
//		(*sp_OP).VARIABLE_ALARM_GRID.bit.CCFault = SET;
//		return (SET);
	}
	return((*sp_OP).VARIABLE_ALARM_GRID.bit.CCFault); //SET Or CLEAR
}

Uint16 ALARM_CV_Fault(S_OPERATION_FILE* sp_OP)
{
	//배열로 무빙하고 무비 첫 데이타 더할때 마지막데이타 빼주고그값이CC때 항상 유지가 안되면 펄트
	//이건 발진으로 확인 할 수 있다네 ㅋㅋ
	static Uint16 MovingCounter = 0;
	static Uint16 MovingStartFlag = 0;
	static float32 DataSum = 0.0;
	float32	DataNew = 0.0;
	float32 DtatResult = 0.0;
	float32 VoltageRefTemp = 0;

	static Uint16 CheckTime = 0;
	static Uint16 SetTime = 0;
	static Uint16 CommandFlagOld = 0;

	VoltageRefTemp = (*sp_OP).VARIABLE_VoltageRef;

	if(((*sp_OP).VARIABLE_TimeSpanResult & 0xA) == (CheckTime & 0xA))CheckTime += 1 ;

	if((*sp_OP).VARIABLE_OPERATIN == OPERATION_SEND_CHARGE_CV	    ||
	   (*sp_OP).VARIABLE_OPERATIN == OPERATION_SEND_DISCHARGE_CV	||
	   (*sp_OP).VARIABLE_OPERATIN == OPERATION_SEND_CHARGE_CV_CP	||
	   (*sp_OP).VARIABLE_OPERATIN == OPERATION_SEND_DISCHARGE_CV_CP)
	{
		//이동평균 시작
		DataNew = (*sp_OP).s_OldADCData.SENSING_BatteryVoltage;
//		(*sp_OP).CC_MovingData[MovingCounter] = DataNew;
		DataSum +=DataNew;

		if(++MovingCounter >= (ALARM_CV_TIME / 4) - 1)
		{
			MovingCounter = CLEAR;
			MovingStartFlag = SET;
		}
		if(MovingStartFlag == SET)
		{
			DtatResult = (float)((float)(DataSum) / (float)(ALARM_CV_TIME / 4));
			MovingCounter = (ALARM_CV_TIME / 4);
			DataSum -= (*sp_OP).CC_MovingData[MovingCounter];//이동평균 마지막값 제외
		}
		//이동평균 끝
		if((*sp_OP).COMMAND == COMMAND_CHARGE)
		{	//충전시 전압 음수, 전류는 양수 센싱됨
			if(DtatResult < ((VoltageRefTemp + ALARM_VOLTAGE_LIMIT) * (float)(-1.0)) ||//초과전압 체크
			   DtatResult > ((VoltageRefTemp - ALARM_VOLTAGE_LIMIT) * (float)(-1.0)))//미만전압 체크
			{
				if(SetTime == CLEAR)CheckTime = CLEAR;
				SetTime++;
			}
		}
		else if((*sp_OP).COMMAND == COMMAND_DISCHARGE)
		{	//방전시 전압 양수, 전류는 음수 센싱됨
			if(DtatResult > (VoltageRefTemp + ALARM_VOLTAGE_LIMIT) ||//초과전압 체크
			   DtatResult < (VoltageRefTemp - ALARM_VOLTAGE_LIMIT))//미만전압 체크
			{
				if(SetTime == CLEAR)CheckTime = CLEAR;
				SetTime++;
			}
		}
	}

	if(CheckTime >= ALARM_CV_TIME)
	{
		CheckTime = CLEAR;
		SetTime   = CLEAR;
	}


	if(CommandFlagOld != (*sp_OP).COMMAND)
	{
		CheckTime = CLEAR;
		SetTime   = CLEAR;
	}
	CommandFlagOld = (*sp_OP).COMMAND;

	if(SetTime >= (ALARM_CV_TIME / 2))
	{
	    SetTime = CLEAR;
//		SetTime = (ALARM_CV_TIME / 2);
//		(*sp_OP).VARIABLE_ALARM_WARING.bit.CVFault = SET;
//		return (SET);
	}
	return((*sp_OP).VARIABLE_ALARM_WARING.bit.CVFault); //SET Or CLEAR
}

Uint16 ALARM_N02VPowerFault(S_OPERATION_FILE* sp_OP)
{
	static Uint16 CheckTime = 0;
	static Uint16 SetTime = 0;

	if(((*sp_OP).VARIABLE_TimeSpanResult & 0xA) == (CheckTime & 0xA))CheckTime += 1 ;

	if((*sp_OP).s_OldADCData.SENSING_Negative02VPower < ALARM_NEGATIVE_02V_POWER_MIN ||
	   (*sp_OP).s_OldADCData.SENSING_Negative02VPower > ALARM_NEGATIVE_02V_POWER_MAX)
	{
		if(SetTime == CLEAR)CheckTime = CLEAR;
		SetTime++;
	}
	else
	{
		if(CheckTime >= ALARM_NEGATIVE_02V_POWER_TIME)
		{
			CheckTime = CLEAR;
			SetTime   = CLEAR;
		}
	}

	if(SetTime >= (ALARM_NEGATIVE_02V_POWER_TIME / 2))
	{
	    SetTime = CLEAR;
//		SetTime = (ALARM_NEGATIVE_02V_POWER_TIME / 2);
		return (SET);
	}

	return(CLEAR); //SET Or CLEAR
}

Uint16 ALARM_P07VPowerFault(S_OPERATION_FILE* sp_OP)
{
	static Uint16 CheckTime = 0;
	static Uint16 SetTime = 0;

	if(((*sp_OP).VARIABLE_TimeSpanResult & 0xA) != (CheckTime & 0xA))CheckTime += 1 ;

	if((*sp_OP).s_OldADCData.SENSING_Positive07VPower < ALARM_POSITIVE_07V_POWER_MIN ||
	   (*sp_OP).s_OldADCData.SENSING_Positive07VPower > ALARM_POSITIVE_07V_POWER_MAX)
	{
		if(SetTime == CLEAR)CheckTime = CLEAR;
		SetTime++;
	}

	if(CheckTime >= ALARM_POSITIVE_07V_POWER_TIME)
	{
		CheckTime = CLEAR;
		SetTime   = CLEAR;
	}

	if(SetTime >= (ALARM_POSITIVE_07V_POWER_TIME / 2))
	{
	    SetTime = CLEAR;
//		SetTime = (ALARM_POSITIVE_07V_POWER_TIME / 2);
		return (SET);
	}

	return(CLEAR); //SET Or CLEAR
}

Uint16 ALARM_P05VPowerFault(S_OPERATION_FILE* sp_OP)
{
	static Uint16 CheckTime = 0;
	static Uint16 SetTime = 0;

	if(((*sp_OP).VARIABLE_TimeSpanResult & 0xA) != (CheckTime & 0xA))CheckTime += 1 ;

	if((*sp_OP).s_OldADCData.SENSING_Positive_05V_POWER < ALARM_POSITIVE_05V_POWER_MIN ||
	   (*sp_OP).s_OldADCData.SENSING_Positive_05V_POWER > ALARM_POSITIVE_05V_POWER_MAX)
	{
		if(SetTime == CLEAR)CheckTime = CLEAR;
		SetTime++;
	}

	if(CheckTime >= ALARM_POSITIVE_05V_POWER_TIME)
	{
		CheckTime = CLEAR;
		SetTime   = CLEAR;
	}

	if(SetTime >= (ALARM_POSITIVE_05V_POWER_TIME / 2))
	{
	    SetTime = CLEAR;
//		SetTime = (ALARM_POSITIVE_05V_POWER_TIME / 2);
		return (SET);
	}

	return(CLEAR); //SET Or CLEAR
}

Uint16 ALARM_P15VPowerFault(S_OPERATION_FILE* sp_OP)
{
	static Uint16 CheckTime = 0;
	static Uint16 SetTime = 0;

	if(((*sp_OP).VARIABLE_TimeSpanResult & 0xA) != (CheckTime & 0xA))CheckTime += 1 ;

	if((*sp_OP).s_OldADCData.SENSING_Positive_15V_POWER < ALARM_POSITIVE_15V_POWER_MIN ||
	   (*sp_OP).s_OldADCData.SENSING_Positive_15V_POWER > ALARM_POSITIVE_15V_POWER_MAX)
	{
		if(SetTime == CLEAR)CheckTime = CLEAR;
		SetTime++;
	}

	if(CheckTime >= ALARM_POSITIVE_15V_POWER_TIME)
	{
		CheckTime = CLEAR;
		SetTime   = CLEAR;
	}

	if(SetTime >= (ALARM_POSITIVE_15V_POWER_TIME / 2))
	{
	    SetTime = CLEAR;
//		SetTime = (ALARM_POSITIVE_15V_POWER_TIME / 2);
		return (SET);
	}

	return(CLEAR); //SET Or CLEAR
}

Uint16 ALARM_N15VPowerFault(S_OPERATION_FILE* sp_OP)
{
	static Uint16 CheckTime = 0;
	static Uint16 SetTime = 0;

	if(((*sp_OP).VARIABLE_TimeSpanResult & 0xA) != (CheckTime & 0xA))CheckTime += 1 ;

	if((*sp_OP).s_OldADCData.SENSING_Negative15VPower < ALARM_NEGATIVE_15V_POWER_MIN ||
	   (*sp_OP).s_OldADCData.SENSING_Negative15VPower > ALARM_NEGATIVE_15V_POWER_MAX)
	{
		if(SetTime == CLEAR)CheckTime = CLEAR;
		SetTime++;
	}

	if(CheckTime >= ALARM_NEGATIVE_15V_POWER_TIME)
	{
		CheckTime = CLEAR;
		SetTime   = CLEAR;
	}

	if(SetTime >= (ALARM_NEGATIVE_15V_POWER_TIME / 2))
	{
	    SetTime = CLEAR;
		//SetTime = (ALARM_NEGATIVE_15V_POWER_TIME / 2);
		return (SET);
	}

	return(CLEAR); //SET Or CLEAR
}

Uint16 ALARM_TemperatureFault(S_OPERATION_FILE* sp_OP)
{
	static Uint16 CheckTime = 0;
	static Uint16 SetTime = 0;

	if(((*sp_OP).VARIABLE_TimeSpanResult & 0xA) != (CheckTime & 0xA))CheckTime += 1 ;

	if((*sp_OP).s_OldADCData.SENSING_Temperature > ALARM_TEMPERATURE_LIMIT)
	{
		if(SetTime == CLEAR)CheckTime = CLEAR;
		SetTime++;
	}

	if(CheckTime >= ALARM_TEMPERATURE_TIME)
	{
		CheckTime = CLEAR;
		SetTime   = CLEAR;
	}

	if(SetTime >= (ALARM_TEMPERATURE_TIME / 2))
	{
	    SetTime = CLEAR;
//		SetTime = (ALARM_TEMPERATURE_TIME / 2);
		return (SET);
	}

	return(CLEAR); //SET Or CLEAR
}

Uint16 ALARM_PowerConditionCheck(S_OPERATION_FILE* sp_OP)
{
	Uint16 CheckAlarm = 0;
	CheckAlarm |= ALARM_N02VPowerFault(sp_OP);
	CheckAlarm |= ALARM_P07VPowerFault(sp_OP);
	CheckAlarm |= ALARM_P05VPowerFault(sp_OP);
	CheckAlarm |= ALARM_P15VPowerFault(sp_OP);
	CheckAlarm |= ALARM_N15VPowerFault(sp_OP);
	if(CheckAlarm)(*sp_OP).VARIABLE_ALARM_WARING.bit.PowerFault = SET;
	return((*sp_OP).VARIABLE_ALARM_WARING.bit.PowerFault); //SUCCESS Or FAIL
}

Uint16 ALARM_LineConditionCheck(S_OPERATION_FILE* sp_OP)
{
	Uint16 CheckAlarm = 0;
	CheckAlarm |= ALARM_VolatageInnerFault(sp_OP);
	if(CheckAlarm)(*sp_OP).VARIABLE_ALARM_GRID.bit.LineFault = SET;
	return((*sp_OP).VARIABLE_ALARM_GRID.bit.LineFault); //SUCCESS Or FAIL
}

Uint16 ALARM_CurrentConditionCheck(S_OPERATION_FILE* sp_OP)
{
	Uint16 CheckAlarm = 0;
	CheckAlarm |= ALARM_CurrentFault(sp_OP);
	CheckAlarm |= ALARM_CurrentReferenceFault(sp_OP);
	if(CheckAlarm)(*sp_OP).VARIABLE_ALARM_WARING.bit.CurrentFault = SET;
	return((*sp_OP).VARIABLE_ALARM_WARING.bit.CurrentFault); //SUCCESS Or FAIL
}

Uint16 ALARM_VoltageConditionCheck(S_OPERATION_FILE* sp_OP)
{
	Uint16 CheckAlarm = 0;

	CheckAlarm |= ALARM_VoltageFault(sp_OP);
	CheckAlarm |= ALARM_VoltageReferenceFault(sp_OP);
	if(CheckAlarm)(*sp_OP).VARIABLE_ALARM_WARING.bit.VoltageFault = SET;
	return((*sp_OP).VARIABLE_ALARM_WARING.bit.VoltageFault); //SUCCESS Or FAIL
}

Uint16 ALARM_HeatConditionCheck(S_OPERATION_FILE* sp_OP)
{
	Uint16 CheckAlarm = 0;
	CheckAlarm |= ALARM_TemperatureFault(sp_OP);
	if(CheckAlarm)(*sp_OP).VARIABLE_ALARM_GRID.bit.OverHeatFault = SET;
	return((*sp_OP).VARIABLE_ALARM_GRID.bit.OverHeatFault); //SUCCESS Or FAIL
}

Uint16 ALARM_CommunicationConditionCheck(S_OPERATION_FILE* sp_OP)
{
	Uint16 UpdateFlag = 0;

	UpdateFlag = (*sp_OP).VARIABLE_UpdateData;

	if((*sp_OP).VARIABLE_UpdateData_OLD == (*sp_OP).VARIABLE_UpdateData)
		(*sp_OP).VARIABLE_UpdateAlramTime++;// =CheckTime++;
	else (*sp_OP).VARIABLE_UpdateAlramTime = CLEAR;

	if((*sp_OP).VARIABLE_UpdateAlramTime >= ALARM_COMMUINCATION_TIME)
		(*sp_OP).VARIABLE_ALARM_DC.bit.CommunicationFault = SET;

	(*sp_OP).VARIABLE_UpdateData_OLD = UpdateFlag;
	return((*sp_OP).VARIABLE_ALARM_DC.bit.CommunicationFault); //SUCCESS Or FAIL
}

Uint16 ALARM_BatteryConditionCheck(S_OPERATION_FILE *sp_OP)
{
	Uint16 CheckAlarm = 0;

	if((*sp_OP).s_OldADCData.SENSING_BatteryVoltage > ALARM_BATTERY_VOLTAGE_LIMIT)
	{
		//ToDo : 버터리 역삽
		(*sp_OP).VARIABLE_ALARM_GRID.bit.BatteryFault = SET;
	}

	if(ABS_F((*sp_OP).s_OldADCData.SENSING_Current) < ALARM_BATTERY_CURRENT_LIMIT)//충전때 부호가 어떻게 되나요?
	{
		//ToDo : 버터리 없음
		(*sp_OP).VARIABLE_ALARM_WARING.bit.BatteryNone = SET;
	}

	CheckAlarm |= (*sp_OP).VARIABLE_ALARM_GRID.bit.BatteryFault;
	CheckAlarm |= (*sp_OP).VARIABLE_ALARM_WARING.bit.BatteryNone;

	return(CheckAlarm); //SUCCESS Or FAIL
}

Uint16 ALARM_BatteryFaultCheck(S_OPERATION_FILE *sp_OP)
{
    Uint16 ui_i = 0;

    for (ui_i = 0; ui_i < 10; ui_i++) {
        AD7608_Conversion();
        OPERATION_Sensing(sp_OP);
    }

    if ((*sp_OP).s_OldADCData.SENSING_BatteryVoltage > ALARM_BATTERY_VOLTAGE_LIMIT) {
        (*sp_OP).VARIABLE_ALARM_GRID.bit.BatteryFault = SET;
    }

    return ((*sp_OP).VARIABLE_ALARM_GRID.bit.BatteryFault);  //SUCCESS Or FAIL
}

//Uint16 ALARM_BatteryNoneCheck(S_OPERATION_FILE *sp_OP)
//{
//    Uint16 ui_i = 0;
//
//    (*sp_OP).C_IntSlopMUX = CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_NONE);
//    (*sp_OP).C_IntResetMUX = CONTROL_IntegrationResetMUX(CONTROL_ALL, CONTROL_ON);
//    DELAY_US(OPERATION_INTEGRATION_RESET_TIME_US_CHA);
//    (*sp_OP).C_IntResetMUX = CONTROL_IntegrationResetMUX(CONTROL_ALL, CONTROL_OFF);
//    (*sp_OP).C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, 10000);//전류값에 따라 설정
//
//    (*sp_OP).C_VRELAY = CONTROL_VoltageRELAY(CONTROL_ON);
//    (*sp_OP).C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_ON);
//
//    (*sp_OP).C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_CHARGE);
//
//    (*sp_OP).C_GateOffsetDAC = CONTROL_GateOffsetDAC(0.0);//베터리 전압에 따라 조정
//    (*sp_OP).C_VRefDAC = CONTROL_VoltageReferenceDAC(OPERATION_LIMIT_MAX_VOLTAGE);
//    (*sp_OP).C_IRefDAC = CONTROL_CurrentReferenceDAC(CONTROL_ALL, 1.0);
//    (*sp_OP).C_FINEIRefDAC = CONTROL_FINECurrentReferenceDAC(CONTROL_ALL, 0.0);
//
//    (*sp_OP).C_GateOutputMUX = CONTROL_GateOutputMUX(CONTROL_ALL, CONTROL_CHARGE);
//    (*sp_OP).C_GateOutputAMP = CONTROL_GateOutputAMP(CONTROL_ALL, CONTROL_CHARGE);
//    (*sp_OP).C_IntSlopMUX = CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_PM);
//    DELAY_US(1000000);
//
//    for(ui_i = 0; ui_i < 10; ui_i++)//1s
//    {
//        AD7608_Conversion();   //DL 2019-07-01
//        OPERATION_Sensing(sp_OP);
//    }
//    OPERATION_Stop(sp_OP);
//
//	if(ABS_F((*sp_OP).s_ADCData.SENSING_Current) < ALARM_BATTERY_CURRENT_LIMIT)//충전때 부호가 어떻게 되나요?
//	{
//		//ToDo : 버터리 없음
//		(*sp_OP).VARIABLE_ALARM_WARING.bit.BatteryNone = SET;
//	}
//
//	return((*sp_OP).VARIABLE_ALARM_WARING.bit.BatteryNone); //SUCCESS Or FAIL
//}


Uint16 ALARM_BatteryNoneCheck(S_OPERATION_FILE *sp_OP)
{
    Uint16 ui_i = 0;

    if ((*sp_OP).VARIABLE_ALARM_GRID.bit.BatteryFault == CLEAR) {

#if PCB_VERSION != 220
#if (FET == 200)
        (*sp_OP).C_IntSlopCapMUX = CONTROL_IntegrationSlopCapMUX(CONTROL_CAP_100nF);
#elif (FET == 240)
        (*sp_OP).C_IntSlopCapMUX = CONTROL_IntegrationSlopCapMUX(CONTROL_CAP_220nF);
#endif
#endif
        (*sp_OP).C_IntSlopMUX = CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_NONE);
        (*sp_OP).C_IntResetMUX = CONTROL_IntegrationResetMUX(CONTROL_ALL, CONTROL_ON);
        DELAY_US(OPERATION_INTEGRATION_RESET_TIME_US_CHA);
        (*sp_OP).C_IntResetMUX = CONTROL_IntegrationResetMUX(CONTROL_ALL, CONTROL_OFF);
        (*sp_OP).C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, 1000);  //전류값에 따라 설정

        (*sp_OP).C_VRELAY = CONTROL_VoltageRELAY(CONTROL_ON);
        (*sp_OP).C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_ON);

        (*sp_OP).C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_CHARGE);

        (*sp_OP).C_GateOffsetDAC = CONTROL_GateOffsetDAC(0.0);  //베터리 전압에 따라 조정
        (*sp_OP).C_VRefDAC = CONTROL_VoltageReferenceDAC(OPERATION_LIMIT_MAX_VOLTAGE);
        (*sp_OP).C_IRefDAC = CONTROL_CurrentReferenceDAC(CONTROL_ALL, 0.0);
        (*sp_OP).C_FINEIRefDAC = CONTROL_FINECurrentReferenceDAC(CONTROL_ALL, 1.0);

        (*sp_OP).C_GateOutputMUX = CONTROL_GateOutputMUX(CONTROL_ALL, CONTROL_CHARGE);
        (*sp_OP).C_GateOutputAMP = CONTROL_GateOutputAMP(CONTROL_ALL, CONTROL_CHARGE);
        (*sp_OP).C_IntSlopMUX = CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_PM);
        DELAY_US(1000000);

        for (ui_i = 0; ui_i < 10; ui_i++)  //1s
                {
            AD7608_Conversion();   //DL 2019-07-01
            OPERATION_Sensing(sp_OP);
        }
        OPERATION_Stop(sp_OP);

        if (ABS_F((*sp_OP).s_ADCData.SENSING_Current) < ALARM_BATTERY_CURRENT_LIMIT) {
            (*sp_OP).VARIABLE_ALARM_WARING.bit.BatteryNone = SET;
        }
    }

    return ((*sp_OP).VARIABLE_ALARM_WARING.bit.BatteryNone);  //SUCCESS Or FAIL
}

Uint16 ALARM_Emergency(S_OPERATION_FILE *sp_OP)
{
	return((*sp_OP).VARIABLE_ALARM_WARING.bit.EMOSwitchOn); //SUCCESS Or FAIL
}

//
// End of file
//


