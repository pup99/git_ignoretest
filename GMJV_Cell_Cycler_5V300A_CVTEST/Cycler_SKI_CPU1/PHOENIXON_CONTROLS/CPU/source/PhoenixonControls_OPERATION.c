/*
 * PhoenixonControls_OPERATION.c
 *
 *  Created on: 2018. 5. 20.
 *      Author: BGKim
 */
#include "../../../common/include/F28x_Project.h"
#include "../../../common/include/driverlib.h"
#include "../../../common/include/device.h"
#include "math.h"

#ifdef _DEBUG_PRINTF_CALL
#include "stdio.h"
#endif

#pragma CODE_SECTION(OPERATION_Run, ".TI.ramfunc");
#pragma CODE_SECTION(OPERATION_ModeCheck, ".TI.ramfunc");
#pragma CODE_SECTION(OPERATION_EndConditionCheck, ".TI.ramfunc");
#pragma CODE_SECTION(OPERATION_CellCheck, ".TI.ramfunc");
#pragma CODE_SECTION(OPERATION_Start, ".TI.ramfunc");
#pragma CODE_SECTION(OPERATION_Stop, ".TI.ramfunc");
#pragma CODE_SECTION(OPERATION_Alram, ".TI.ramfunc");
#pragma CODE_SECTION(OPERATION_AlramReset, ".TI.ramfunc");
#pragma CODE_SECTION(OPERATION_Emergency, ".TI.ramfunc");
#pragma CODE_SECTION(OPERATION_ValueReset, ".TI.ramfunc");
#pragma CODE_SECTION(OPERATION_Reset, ".TI.ramfunc");
#pragma CODE_SECTION(OPERATION_Rest, ".TI.ramfunc");
#pragma CODE_SECTION(OPERATION_Charge, ".TI.ramfunc");
#pragma CODE_SECTION(OPERATION_Sensing, ".TI.ramfunc");
#pragma CODE_SECTION(OPERATION_VoltageControl, ".TI.ramfunc");
#pragma CODE_SECTION(OPERATION_IntegrationSlopSelect, ".TI.ramfunc");
#pragma CODE_SECTION(OPERATION_IntegrationSlopControl, ".TI.ramfunc");
#pragma CODE_SECTION(OPERATION_GateOffsetControl, ".TI.ramfunc");
#pragma CODE_SECTION(OPEARTION_CurrentControl, ".TI.ramfunc");

extern float32 VoltageCharge_REF[];
extern float32 VoltageDischarge_REF[];
extern float32 VoltageCharge_ADC[];
extern float32 VoltageDischarge_ADC[];

extern float32 CurrentCharge_REF[];
extern float32 CurrentDischarge_REF[];
extern float32 CurrentCharge_ADC[];
extern float32 CurrentDischarge_ADC[];

#if (FET == 200)
float32 IntegrationChargeSlopOffset[OPERATION_INTEGRATION_SLOP_ARRAY_LENGTH ] = {
       500   ,//0    A
       1000   ,//1    A
       1500   ,//2    A
       2000   ,//3    A
       2500   ,//4    A
       2500   ,//5    A
       3000   ,//6    A
       3000   ,//7    A
       3500   ,//8    A
       4000   ,//9    A
       6000   ,//10   A
       6000   ,//20   A
       6500   ,//30   A
       6500   ,//40   A
       7000   ,//50   A
       7000   ,//60   A
       7500   ,//70   A
       7700   ,//80   A
       7800   ,//90   A
       8000   ,//100  A
       8200   ,//110  A
       7000   ,//120  A
       7000   ,//130  A
       7000   ,//140  A
       7000   ,//150  A
       7500   ,//160  A
       8000   ,//170  A
       7000   ,//180  A
       7500   ,//190  A
       8000   ,//200  A
       8000   ,//210  A
       7000   ,//220  A
       6000   ,//230  A
       6000   ,//240  A
       6000   ,//250  A
       5000   ,//260  A
       5000   ,//270  A
       5000   ,//280  A
       4500   ,//290  A
       4500    //300  A
};

float32 IntegrationChargeSlopGain[OPERATION_INTEGRATION_SLOP_ARRAY_LENGTH ] = {
       100 ,   //0    A
       100 ,   //1    A
       100 ,   //2    A
       100 ,   //3    A
       100 ,   //4    A
       100 ,   //5    A
       100 ,   //6    A
       100 ,   //7    A
       100 ,   //8    A
       100 ,   //9    A
       200 ,   //10   A
       200 ,   //20   A
       350 ,   //30   A
       350 ,   //40   A
       350 ,   //50   A
       400 ,   //60   A
       400 ,   //70   A
       400 ,   //80   A
       400 ,   //90   A
       400 ,   //100  A
       400 ,   //110  A
       350 ,   //120  A
       350 ,   //130  A
       350 ,   //140  A
       370 ,   //150  A
       400 ,   //160  A
       400 ,   //170  A
       350 ,   //180  A
       350 ,   //190  A
       350 ,   //200  A
       300 ,   //210  A
       250 ,   //220  A
       250 ,   //230  A
       270 ,   //240  A
       270 ,   //250  A
       270 ,   //260  A
       270 ,   //270  A
       270 ,   //280  A
       270 ,   //290  A
       270     //300  A
};

float32 IntegrationDischargeSlopOffset[OPERATION_INTEGRATION_SLOP_ARRAY_LENGTH ] = {
       500    ,  //0    A
       1000    ,  //1    A
       1300    ,  //2    A
       1500    ,  //3    A
       2000    ,  //4    A
       2500    ,  //5    A
       3000    ,  //6    A
       3500    ,  //7    A
       4000    ,  //8    A
       4500    ,  //9    A
       5500    ,  //10   A
       6000    ,  //20   A
       7000    ,  //30   A
       7500    ,  //40   A
       8200    , //50   A
       9500    , //60   A
       11000    , //70   A
       12000    ,  //80   A
       13000    , //90   A
       14000    , //100  A
       15000    , //110  A
       16500    , //120  A
       18000    , //130  A
       20000    , //140  A
       21000    , //150  A
       22000    , //160  A
       24000    , //170  A
       25000    , //180  A
       26500    , //190  A
       28000    , //200  A
       30500    , //210  A
       32500    , //220  A
       35000    , //230  A
       37000    , //240  A
       39000    , //250  A
       43000    , //260  A
       45000    , //270  A
       50000    , //280  A
       50000    , //290  A
       50000      //300  A
};

float32 IntegrationDischargeSlopGain[OPERATION_INTEGRATION_SLOP_ARRAY_LENGTH ] = {
      100 ,    //0    A
      100 ,    //1    A
      100 ,    //2    A
      100 ,    //3    A
      100 ,    //4    A
      100 ,    //5    A
      100 ,    //6    A
      100 ,    //7    A
      100 ,    //8    A
      100 ,    //9    A
      100 ,    //10   A
      100 ,    //20   A
      100 ,    //30   A
      100 ,    //40   A
      100 ,    //50   A
      100 ,    //60   A
      100 ,    //70   A
      150 ,    //80   A
      150 ,    //90   A
      200 ,    //100  A
      200 ,    //110  A
      200 ,    //120  A
      200 ,    //130  A
      200 ,    //140  A
      200 ,    //150  A
      200 ,    //160  A
      200 ,    //170  A
      200 ,    //180  A
      200 ,    //190  A
      200 ,    //200  A
      200 ,    //210  A
      200 ,    //220  A
      200 ,    //230  A
      200 ,    //240  A
      200 ,    //250  A
      200 ,    //260  A
      200 ,    //270  A
      200 ,    //280  A
      200 ,    //290  A
      200      //300  A
};

#elif (FET == 240)
float32 IntegrationChargeSlopOffset[OPERATION_INTEGRATION_SLOP_ARRAY_LENGTH ] = {
300  ,//0    A
300   ,//1    A
400   ,//2    A
500   ,//3    A
500   ,//4    A
500   ,//5    A
500   ,//6    A
600   ,//7    A
600   ,//8    A
1000   ,//9    A
1500   ,//10   A
1700   ,//20   A
1800   ,//30   A
2200   ,//40   A
2400   ,//50   A
2600   ,//60   A
3000   ,//70   A
3200   ,//80   A
3400   ,//90   A
3400   ,//100  A
3400   ,//110  A
3400   ,//120  A
3400   ,//130  A
3400   ,//140  A
3000   ,//150  A
       2800   ,//160  A
       2800   ,//170  A
       2800   ,//180  A
       2500   ,//190  A
       2000   ,//200  A
       2000   ,//210  A
       2000   ,//220  A
       2000   ,//230  A
       2000   ,//240  A
       1800   ,//250  A
       1800   ,//260  A
       1800   ,//270  A
       1700   ,//280  A
       1700   ,//290  A
       1700    //300  A
};

float32 IntegrationChargeSlopGain[OPERATION_INTEGRATION_SLOP_ARRAY_LENGTH ] = {
       100 ,   //0    A
       100 ,   //1    A
       100 ,   //2    A
       100 ,   //3    A
       100 ,   //4    A
       100 ,   //5    A
       100 ,   //6    A
       100 ,   //7    A
       100 ,   //8    A
       100 ,   //9    A
       100 ,   //10   A
       100 ,   //20   A
       100 ,   //30   A
       100 ,   //40   A
       130 ,   //50   A
       130 ,   //60   A
       130 ,   //70   A
       130 ,   //80   A
       130 ,   //90   A
       130 ,   //100  A
       130 ,   //110  A
       130 ,   //120  A
       130 ,   //130  A
       130 ,   //140  A
       130 ,   //150  A
       130 ,   //160  A
       130 ,   //170  A
       130 ,   //180  A
       130 ,   //190  A
       130 ,   //200  A
       130 ,   //210  A
       130 ,   //220  A
       130 ,   //230  A
       130 ,   //240  A
       130 ,   //250  A
       130 ,   //260  A
       130 ,   //270  A
       100 ,   //280  A
       100 ,   //290  A
       100     //300  A
};

float32 IntegrationDischargeSlopOffset[OPERATION_INTEGRATION_SLOP_ARRAY_LENGTH ] = {
       150  ,  //0    A
       150   ,  //1    A
       300   ,  //2    A
       400   ,  //3    A
       500   ,  //4    A
       600   ,  //5    A
       700   ,  //6    A
       800   ,  //7    A
       900   ,  //8    A
       1000   ,  //9    A
       1000   ,  //10   A
       1500   ,  //20   A
       2000   ,  //30   A
       2500   ,  //40   A
       3500   , //50   A
       4500   , //60   A
       5000    , //70   A
       6000    ,  //80   A
       7000    , //90   A
       7800    , //100  A
       8000    , //110  A
       8500    , //120  A
       8500    , //130  A
       9000    , //140  A
       9000    , //150  A
       9500    , //160  A
       10000    , //170  A
       11000    , //180  A
       11500    , //190  A
       12000    , //200  A
       13000    , //210  A
       13000    , //220  A
       13000    , //230  A
       13000    , //240  A
       14000    , //250  A
       14000    , //260  A
       14000    , //270  A
       14000    , //280  A
       14000    , //290  A
       14000      //300  A
};

float32 IntegrationDischargeSlopGain[OPERATION_INTEGRATION_SLOP_ARRAY_LENGTH ] = {
      100 ,    //0    A
      100 ,    //1    A
      100 ,    //2    A
      100 ,    //3    A
      100 ,    //4    A
      100 ,    //5    A
      100 ,    //6    A
      100 ,    //7    A
      100 ,    //8    A
      100 ,    //9    A
      100 ,    //10   A
      100 ,    //20   A
      150 ,    //30   A
      150 ,    //40   A
      150 ,    //50   A
      150 ,    //60   A
      150 ,    //70   A
      150 ,    //80   A
      150 ,    //90   A
      150 ,    //100  A
      150 ,    //110  A
      150 ,    //120  A
      150 ,    //130  A
      150 ,    //140  A
      150 ,    //150  A
      180 ,    //160  A
      180 ,    //170  A
      180 ,    //180  A
      180 ,    //190  A
      150 ,    //200  A
      150 ,    //210  A
      130 ,    //220  A
      120 ,    //230  A
      120 ,    //240  A
      120 ,    //250  A
      120 ,    //260  A
      100 ,    //270  A
      100 ,    //280  A
      100 ,    //290  A
      100      //300  A
};
#endif

Uint16 OPERATION_Init(S_OPERATION_FILE *sp_OP)
{
    Uint16 ui_i = 0;

    OPERATION_Reset(sp_OP);
    DELAY_US(1000000);//Time of 1s
    for(ui_i = 0; ui_i < 10; ui_i++)
    SENSING_ADCResultData(&(*sp_OP).s_ADCData);

    return(SUCCESS);//SUCCESS Or FAIL
}

Uint16 OPERATION_Run(S_OPERATION_FILE *sp_OP)
{
    E_OPERATION_COM_SELECT Command = COM_NONE;

    if ((*sp_OP).DIRECT_COMMAND != COM_NONE) {
        Command = (*sp_OP).DIRECT_COMMAND;
        if ((*sp_OP).VARIABLE_COMMAND_OLD == Command) (*sp_OP).DIRECT_COMMAND = COM_NONE;
        Command = (*sp_OP).DIRECT_COMMAND;
    } else {
        if ((*sp_OP).VARIABLE_ALARM_Flag == CLEAR) Command = (*sp_OP).COMMAND;
        else Command = COM_NONE;
    }

    switch (Command)
    {
        case COM_NONE:
            OPERATION_Stop(sp_OP);
            break;
        case COM_REST:
            OPERATION_Rest(sp_OP);
            break;
        case COM_CHARGE:
        case COM_DISCHARGE:
            OPERATION_Charge(sp_OP);
            break;
        case COM_ALRAM_CLEAR:
            OPERATION_AlramReset(sp_OP);
            break;
        case COM_INPUT_RELAY_OFF:
            OPERATION_Stop(sp_OP);
            (*sp_OP).VARIABLE_StartFlag = CLEAR;
            (*sp_OP).C_VRELAY = CONTROL_VoltageRELAY(CONTROL_OFF);
            break;
        case COM_INPUT_RELAY_ON:
            (*sp_OP).C_VRELAY = CONTROL_VoltageRELAY(CONTROL_ON);
            break;
        case COM_OUTPUT_RELAY_OFF:
            OPERATION_Stop(sp_OP);
            (*sp_OP).VARIABLE_StartFlag = CLEAR;
            (*sp_OP).C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
            break;
        case COM_OUTPUT_RELAY_ON:
            (*sp_OP).C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_ON);
            break;
        case COM_ALL_RELAY_OFF:
            OPERATION_Stop(sp_OP);
            (*sp_OP).VARIABLE_StartFlag = CLEAR;
            (*sp_OP).C_VRELAY = CONTROL_VoltageRELAY(CONTROL_OFF);
            (*sp_OP).C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
            break;
        case COM_ALRAM_REQUEST:
            (*sp_OP).VARIABLE_ALARM_RequestFlag++;
            break;
        case COM_MODE_PARALLEL:
            break;
        case COM_MODE_NORMAL:
            break;
    }
    (*sp_OP).VARIABLE_COMMAND_OLD = Command;

    return (SUCCESS);  //SUCCESS Or FAIL
}


Uint16 OPERATION_CellCheck(S_OPERATION_FILE *sp_OP)
{
    if(ALARM_BatteryFaultCheck(sp_OP))
        return(FAIL);

    if(ALARM_BatteryNoneCheck(sp_OP))
        return(FAIL);

    return(SUCCESS);//SUCCESS Or FAIL
}

E_OPERATION_OP_SEND_SELECT OPERATION_ModeCheck(S_OPERATION_FILE *sp_OP)
{
    E_OPERATION_OP_SEND_SELECT Mode = OP_SEND_DEFAULT;
    static Uint16 CC_To_CV_Change_Flag = 0;

    static Uint16 SettingTime = 0;
    static E_OPERATION_COM_SELECT Flag = COM_NONE;
    static Uint16 Index = 0;

    float current_new = 0.0;
    static Uint16 sensing_stable_count = 0;
    static Uint16 error_time = 0;
    float ref_current = 0.0;

    if ((*sp_OP).OPERATIN == OP_CHARGE_CP || (*sp_OP).OPERATIN == OP_DISCHARGE_CP || (*sp_OP).OPERATIN == OP_CHARGE_CPCV || (*sp_OP).OPERATIN == OP_DISCHARGE_CPCV) {
        (*sp_OP).VARIABLE_PowerRef = (float) ((*sp_OP).VARIABLE_CurrentRef / ABS_F((*sp_OP).VARIABLE_VoltageResult));
        //CP kW -> W
        //(*sp_OP).VARIABLE_PowerRef *= (float) (1000.0);

        ref_current = (*sp_OP).VARIABLE_PowerRef;
        if(ref_current > (*sp_OP).CP_CURRENT_LIMIT && (*sp_OP).CP_CURRENT_LIMIT != 0){
            ref_current = (*sp_OP).CP_CURRENT_LIMIT;
        }
    } else if (sp_OP->OPERATIN == OP_CHARGE_CR || sp_OP->OPERATIN == OP_DISCHARGE_CR) {
        (*sp_OP).VARIABLE_ResistanceRef = (float) (ABS_F((*sp_OP).VARIABLE_VoltageResult) / (*sp_OP).VARIABLE_CurrentRef);

        ref_current = (*sp_OP).VARIABLE_ResistanceRef;
    } else {
        ref_current = (*sp_OP).VARIABLE_CurrentRef;
    }
#if (Current_Spec == 200)
    if (ref_current > 200.0) {
        ref_current = 200.0;
    }
#elif (Current_Spec == 300)
    if (ref_current > 300.0) {
        ref_current = 300.0;
    }
#endif
    switch ((*sp_OP).COMMAND)
    {
        case COM_CHARGE:
            if (Flag != COM_CHARGE || Index != (*sp_OP).STEP_INDEX) {
                Flag = COM_CHARGE;
                Index = (*sp_OP).STEP_INDEX;
                SettingTime = 0;

                CC_To_CV_Change_Flag = CLEAR;
                sensing_stable_count = 0;
                error_time = 0;
            }
            if (sensing_stable_count >= 3) {
                if ((ABS_F((*sp_OP).VARIABLE_VoltageResult)) >= ((*sp_OP).VARIABLE_VoltageRef - OPERATION_VOLTAGE_TOLERANCE )) {
                    if (CC_To_CV_Change_Flag == CLEAR) {

                        current_new = ABS_F((*sp_OP).s_OldADCData.SENSING_Current);

                        //case 1. Target strange
                        if (current_new < 0.06) {
                            error_time++;
                            if (error_time >= 3) {
                                CC_To_CV_Change_Flag = SET;
                            }
                        }   else {
                            error_time = 0;
                        }

                        //case 2. Current decrease
                        //DL 2019-08-12, 0.02 -> 0.1
                        if ((ref_current - 0.1) > current_new) {

                            SettingTime++;

                            if (SettingTime >= OPERATION_MODE_VOLTAGE_CHECK_TIME) {
                                CC_To_CV_Change_Flag = SET;
                            }
                        } else {
                            SettingTime = 0;
                        }
                    }
                }   else {
                    CC_To_CV_Change_Flag = 0;
                    SettingTime = 0;
                    error_time = 0;
                }
            }
            else {
                sensing_stable_count++;
            }
        break;

        case COM_DISCHARGE:
            if (Flag != COM_DISCHARGE || Index != (*sp_OP).STEP_INDEX) {
                Flag = COM_DISCHARGE;
                Index = (*sp_OP).STEP_INDEX;
                SettingTime = 0;

                CC_To_CV_Change_Flag = CLEAR;
                sensing_stable_count = 0;
                error_time = 0;
            }
            if (sensing_stable_count >= 3) {
                if ((ABS_F((*sp_OP).VARIABLE_VoltageResult)) <= ((*sp_OP).VARIABLE_VoltageRef + OPERATION_VOLTAGE_TOLERANCE )) {
                    if (CC_To_CV_Change_Flag == CLEAR) {

                        current_new = ABS_F((*sp_OP).s_OldADCData.SENSING_Current);

                        //case 1. Target strange
                        if (current_new < 0.06) {
                            error_time++;
                            if (error_time >= 3) {
                                CC_To_CV_Change_Flag = SET;
                            }
                        } else
                        {
                            error_time = 0;
                        }

                        //case 2. Current decrease
                        if ((ref_current - 0.02) > current_new) {

                            SettingTime++;

                            if (SettingTime >= OPERATION_MODE_VOLTAGE_CHECK_TIME) {
                                CC_To_CV_Change_Flag = SET;
                            }
                        } else
                        {
                            SettingTime = 0;
                        }
                    }
                } else
                {
                    CC_To_CV_Change_Flag = 0;
                    SettingTime = 0;
                    error_time = 0;
                }
            } else
            {
                sensing_stable_count++;
            }
            break;

        default:
            Flag = COM_NONE;
            Index = (*sp_OP).STEP_INDEX;
            SettingTime = 0;

            CC_To_CV_Change_Flag = CLEAR;
            sensing_stable_count = 0;
            error_time = 0;
            break;
    }

    if ((*sp_OP).C_VRELAY.e_OldCommand == CONTROL_ON) Mode = OP_SEND_INPUT_ON;
    if ((*sp_OP).C_IRELAY.e_OldCommand == CONTROL_ON || (*sp_OP).C_IRELAY.e_OldCommand == CONTROL_ON) Mode = OP_SEND_OUTPUT_ON;

    switch ((*sp_OP).OPERATIN)
    {
        case OP_DEFAULT:
            Mode = OP_SEND_DEFAULT;
            break;
        case OP_CHARGE_CC:
            Mode = OP_SEND_CHARGE_CC;
            break;
        case OP_CHARGE_CCCV:
            if (CC_To_CV_Change_Flag == SET) Mode = OP_SEND_CHARGE_CV;
            else Mode = OP_SEND_CHARGE_CC;
            break;
        case OP_DISCHARGE_CC:
            Mode = OP_SEND_DISCHARGE_CC;
            break;
        case OP_DISCHARGE_CCCV:
            if (CC_To_CV_Change_Flag == SET) Mode = OP_SEND_DISCHARGE_CV;
            else Mode = OP_SEND_DISCHARGE_CC;
            break;
        case OP_CHARGE_CP:
            Mode = OP_SEND_CHARGE_CP;
            break;
        case OP_DISCHARGE_CP:
            Mode = OP_SEND_DISCHARGE_CP;
            break;
        case OP_CHARGE_CPCV:
            if (CC_To_CV_Change_Flag == SET) Mode = OP_SEND_CHARGE_CV;
            else Mode = OP_SEND_CHARGE_CP;
            break;
        case OP_DISCHARGE_CPCV:
            if (CC_To_CV_Change_Flag == SET) Mode = OP_SEND_DISCHARGE_CV;
            else Mode = OP_SEND_DISCHARGE_CP;
            break;
        case OP_CHARGE_CR:
            Mode = OP_SEND_CHARGE_CR;
            break;
        case OP_DISCHARGE_CR:
            Mode = OP_SEND_DISCHARGE_CR;
            break;
    }
    (*sp_OP).VARIABLE_OPERATIN = Mode;

    return (Mode);                        //SUCCESS Or FAIL
}

E_OPERATION_END_CONDITION OPERATION_EndConditionCheck_LOOP(S_OPERATION_FILE *sp_OP)
{
    if ((*sp_OP).CYCLE_LOOP_COUNT) {
        (*sp_OP).VARIABLE_CYCLE_LOOP_COUNT++;

        if ((*sp_OP).VARIABLE_CYCLE_LOOP_COUNT >= (*sp_OP).CYCLE_LOOP_COUNT) {
            (*sp_OP).VARIABLE_CYCLE_LOOP_COUNT = CLEAR;
            return (OPERATION_END_CONDITION_GOTO);
        } else {
            return (OPERATION_END_CONDITION_RETRUN);
        }
    }
    return (OPERATION_END_CONDITION_NONE);					//SUCCESS Or FAIL
}

E_OPERATION_END_CONDITION OPERATION_EndConditionCheck_VOLTAGE(S_OPERATION_FILE *sp_OP)
{
    static Uint16 SettingTime = 0;
    static Uint16 Index = 0;

    float EndVoltagConditionValue = 0.0;

    static Uint16 cycle_SettingTime = 0;
    static Uint16 cycle_Setting_LowV_Time = 0;
    static Uint16 old_cycle = 0;

    if (Index != (*sp_OP).STEP_INDEX) {
        Index = (*sp_OP).STEP_INDEX;
        SettingTime = 0;
    }

    if (old_cycle != (*sp_OP).CYCLE_INDEX) {
        old_cycle = (*sp_OP).CYCLE_INDEX;
        cycle_SettingTime = 0;
        cycle_Setting_LowV_Time = 0;
    }

    if ((*sp_OP).END_VOLTAGE_CONDITION) {
        EndVoltagConditionValue = (float) ((*sp_OP).END_VOLTAGE_CONDITION);

        switch ((*sp_OP).COMMAND)
        {
            case COM_REST:
                if ((ABS_F((*sp_OP).VARIABLE_EndConditionVoltageValueAVG)) > EndVoltagConditionValue) {
                        return (OPERATION_END_CONDITION_VOLTAGE);
                }
                break;

            case COM_CHARGE:
                if ((ABS_F((*sp_OP).VARIABLE_EndConditionVoltageValueAVG)) > EndVoltagConditionValue) {
                    if (SettingTime++ >= OPERATION_CHARGE_VOLTAGE_CHECK_TIME) {
                        SettingTime = 0;
                        return (OPERATION_END_CONDITION_VOLTAGE);
                    }
                } else {
                    SettingTime = 0;
                }
                break;

            case COM_DISCHARGE:
                if ((ABS_F((*sp_OP).VARIABLE_EndConditionVoltageValueAVG)) < EndVoltagConditionValue) {
                    if (SettingTime++ >= OPERATION_DISCHARGE_VOLTAGE_CHECK_TIME) {
                        SettingTime = 0;
                        return (OPERATION_END_CONDITION_VOLTAGE);
                    }
                } else {
                    SettingTime = 0;
                }
                break;
        }
    }

#ifdef DEV_SCHEDULE_CYCLE
    //Cycle end condition check
    //High Volatge limit
    if ((*sp_OP).CYCLE_END_VOLTAGE_CONDITION) {
        EndVoltagConditionValue = (float) ((*sp_OP).CYCLE_END_VOLTAGE_CONDITION);

        switch ((*sp_OP).COMMAND)
        {
            case COM_REST:
                if ((ABS_F((*sp_OP).VARIABLE_EndConditionVoltageValueAVG)) > EndVoltagConditionValue) {
                    return (OPERATION_CYCLE_END_CONDITION_VOLTAGE);
                }
                break;

            case COM_CHARGE:
                if ((ABS_F((*sp_OP).VARIABLE_EndConditionVoltageValueAVG)) > EndVoltagConditionValue) {
                    if (cycle_SettingTime++ >= OPERATION_CHARGE_VOLTAGE_CHECK_TIME) {
                        cycle_SettingTime = 0;
                        return (OPERATION_CYCLE_END_CONDITION_VOLTAGE);
                    }
                } else {
                    cycle_SettingTime = 0;
                }
                break;

            case COM_DISCHARGE:
                if ((ABS_F((*sp_OP).VARIABLE_EndConditionVoltageValueAVG)) > EndVoltagConditionValue) {
                    if (cycle_SettingTime++ >= OPERATION_DISCHARGE_VOLTAGE_CHECK_TIME) {
                        cycle_SettingTime = 0;
                        return (OPERATION_CYCLE_END_CONDITION_VOLTAGE);
                    }
                } else {
                    cycle_SettingTime = 0;
                }
                break;
        }
    }

    //Low Volatge limit
    if ((*sp_OP).CYCLE_END_LOW_VOLTAGE_CONDITION) {
        EndVoltagConditionValue = (float) ((*sp_OP).CYCLE_END_LOW_VOLTAGE_CONDITION);

        switch ((*sp_OP).COMMAND)
        {
            case COM_REST:
                if ((ABS_F((*sp_OP).VARIABLE_EndConditionVoltageValueAVG)) < EndVoltagConditionValue) {
                    return (OPERATION_CYCLE_END_CONDITION_LOW_VOLTAGE);
                }
                break;

            case COM_CHARGE:
                if ((ABS_F((*sp_OP).VARIABLE_EndConditionVoltageValueAVG)) < EndVoltagConditionValue) {
                    if (cycle_Setting_LowV_Time++ >= OPERATION_CHARGE_VOLTAGE_CHECK_TIME) {
                        cycle_Setting_LowV_Time = 0;
                        return (OPERATION_CYCLE_END_CONDITION_LOW_VOLTAGE);
                    }
                } else {
                    cycle_Setting_LowV_Time = 0;
                }
                break;

            case COM_DISCHARGE:
                if ((ABS_F((*sp_OP).VARIABLE_EndConditionVoltageValueAVG)) < EndVoltagConditionValue) {
                    if (cycle_Setting_LowV_Time++ >= OPERATION_DISCHARGE_VOLTAGE_CHECK_TIME) {
                        cycle_Setting_LowV_Time = 0;
                        return (OPERATION_CYCLE_END_CONDITION_LOW_VOLTAGE);
                    }
                } else {
                    cycle_Setting_LowV_Time = 0;
                }
                break;
        }
    }

#endif

    return (OPERATION_END_CONDITION_NONE);
}

E_OPERATION_END_CONDITION OPERATION_EndConditionCheck_TIME(S_OPERATION_FILE *sp_OP)
{
    static Uint32 SettingTime = 0;
    static Uint16 Index = 0;

    static Uint32 cycle_SettingTime = 0;
    static Uint16 old_cycle = 0;

    if (old_cycle != (*sp_OP).CYCLE_INDEX) {
        old_cycle = (*sp_OP).CYCLE_INDEX;
        cycle_SettingTime = 0;
    }

    if ((*sp_OP).END_TIME_CONDITION) {
        if (Index != (*sp_OP).STEP_INDEX) {
            Index = (*sp_OP).STEP_INDEX;
            SettingTime = 0;
        }
        if (((*sp_OP).VARIABLE_TimeSpanResult % COMMUNICATION_TX_DATA_LENGTH) >= COMMUNICATION_TX_DATA_LENGTH - 1) SettingTime++;

        if (SettingTime >= ((*sp_OP).END_TIME_CONDITION)) {
            (*sp_OP).VARIABLE_EndConditionTime = SettingTime;
            SettingTime = 0;
            return (OPERATION_END_CONDITION_TIME);
        }
    } else {
        Index = (*sp_OP).STEP_INDEX;
        SettingTime = 0;
    }

#ifdef DEV_SCHEDULE_CYCLE
    //Cycle end condition check
    if ((*sp_OP).CYCLE_END_TIME_CONDITION) {
        if (((*sp_OP).VARIABLE_TimeSpanResult % COMMUNICATION_TX_DATA_LENGTH) >= COMMUNICATION_TX_DATA_LENGTH - 1) cycle_SettingTime++;

        if (cycle_SettingTime >= ((*sp_OP).CYCLE_END_TIME_CONDITION)) {
            (*sp_OP).VARIABLE_CycleEndConditionTime = cycle_SettingTime;
            cycle_SettingTime = 0;
            return (OPERATION_CYCLE_END_CONDITION_TIME);
        }
    }
#endif

    return (OPERATION_END_CONDITION_NONE);
}

E_OPERATION_END_CONDITION OPERATION_EndConditionCheck_CURRENT(S_OPERATION_FILE *sp_OP)
{
    static Uint16 RiseFallTime = 0;
    static Uint16 SettingTime = 0;
    static Uint16 Index = 0;
    float EndCurrentConditionValue = 0.0;

    if (Index != (*sp_OP).STEP_INDEX) {
        Index = (*sp_OP).STEP_INDEX;
        RiseFallTime = 0;
        SettingTime = 0;
    }

    if ((*sp_OP).END_CURRENT_CONDITION) {
        EndCurrentConditionValue = (float) ((*sp_OP).END_CURRENT_CONDITION);

        if (((*sp_OP).VARIABLE_TimeSpanResult & 0x1) != (RiseFallTime & 0x1)) RiseFallTime += 1;

        switch ((*sp_OP).VARIABLE_OPERATIN)
        {
            case OP_SEND_CHARGE_CV:
            case OP_SEND_DISCHARGE_CV:
                if (((ABS_F((*sp_OP).VARIABLE_EndConditionCurrentValueAVG)) < EndCurrentConditionValue && (ABS_F((*sp_OP).VARIABLE_EndConditionCurrentValueAVG)) >= EndCurrentConditionValue - 0.005) || (ABS_F((*sp_OP).VARIABLE_EndConditionCurrentValueAVG)) < EndCurrentConditionValue - 0.010) {
                    if (SettingTime++ >= OPERATION_CURRENT_CHECK_TIME) {
                        SettingTime = 0;
                        return (OPERATION_END_CONDITION_CURRENT);
                    }
                }
                break;
        }
    }

    return (OPERATION_END_CONDITION_NONE);
}

E_OPERATION_END_CONDITION OPERATION_EndConditionCheck_CAPACITY(S_OPERATION_FILE *sp_OP)
{
    float EndCapacityConditionValue = 0.0;

    if ((*sp_OP).END_CAPACITY_CONDITION) {
        EndCapacityConditionValue = (float) ((*sp_OP).END_CAPACITY_CONDITION);

        switch ((*sp_OP).COMMAND)
        {
            case COM_CHARGE:
            case COM_DISCHARGE:
                if ((ABS_F((*sp_OP).VARIABLE_CapacityResult)) >= (EndCapacityConditionValue)) {
                    return (OPERATION_END_CONDITION_CAPACITY);
                }
                break;
        }
    }

#ifdef DEV_SCHEDULE_CYCLE
    //Cycle end condition check
    if ((*sp_OP).CYCLE_END_CAPACITY_CONDITION) {
        EndCapacityConditionValue = (float) ((*sp_OP).CYCLE_END_CAPACITY_CONDITION);

        switch ((*sp_OP).COMMAND)
        {
            case COM_CHARGE:
            case COM_DISCHARGE:
                if ((ABS_F((*sp_OP).VARIABLE_CYCLE_CapacityResult)) >= (EndCapacityConditionValue)) {
                    return (OPERATION_CYCLE_END_CONDITION_CAPACITY);
                }
                break;
        }
    }
#endif

    return (OPERATION_END_CONDITION_NONE);
}

E_OPERATION_END_CONDITION OPERATION_EndConditionCheck_WATT(S_OPERATION_FILE *sp_OP)
{
    static Uint16 RiseFallTime = 0;
    static Uint16 SettingTime = 0;
    static Uint16 Index = 0;
    float EndWattConditionValue = 0.0;

    static Uint16 cycle_RiseFallTime = 0;
    static Uint16 cycle_SettingTime = 0;
    static Uint16 cycle = 0;

    if (Index != (*sp_OP).STEP_INDEX) {
        Index = (*sp_OP).STEP_INDEX;
        RiseFallTime = 0;
        SettingTime = 0;
    }

    if (cycle != (*sp_OP).CYCLE_INDEX) {
        cycle = (*sp_OP).CYCLE_INDEX;
        cycle_RiseFallTime = 0;
        cycle_SettingTime = 0;
    }

    if ((*sp_OP).END_WATT_CONDITION) {
        EndWattConditionValue = (float) ((*sp_OP).END_WATT_CONDITION);

        if (((*sp_OP).VARIABLE_TimeSpanResult & 0xA) != (RiseFallTime & 0xA)) RiseFallTime += 1;

        switch ((*sp_OP).COMMAND)
        {
            case COM_CHARGE:
            case COM_DISCHARGE:
                if ((ABS_F((*sp_OP).VARIABLE_PowerResult_Wh)) >= EndWattConditionValue) {
                    if (++SettingTime >= OPERATION_POWER_CHECK_TIME) {
                        SettingTime = 0;
                        return (OPERATION_END_CONDITION_WATT);
                    }
                }
                break;
        }
    }

#ifdef DEV_SCHEDULE_CYCLE
    //Cycle end condition check
    if ((*sp_OP).CYCLE_END_WATT_CONDITION) {
        EndWattConditionValue = (float) ((*sp_OP).CYCLE_END_WATT_CONDITION);

        if (((*sp_OP).VARIABLE_TimeSpanResult & 0xA) != (cycle_RiseFallTime & 0xA)) cycle_RiseFallTime += 1;

        switch ((*sp_OP).COMMAND)
        {
            case COM_CHARGE:
            case COM_DISCHARGE:
                if ((ABS_F((*sp_OP).VARIABLE_CYCLE_PowerResult_Wh)) >= EndWattConditionValue) {
                    if (++cycle_SettingTime >= OPERATION_POWER_CHECK_TIME) {
                        cycle_SettingTime = 0;
                        return (OPERATION_CYCLE_END_CONDITION_WATT);
                    }
                }
                break;
        }
    }
#endif

    return (OPERATION_END_CONDITION_NONE);
}

E_OPERATION_END_CONDITION OPERATION_EndConditionCheck(S_OPERATION_FILE *sp_OP)
{
    static Uint32 ValueTemp = 0.0;
    static Uint16 ReaultTemp = 0;
    static Uint16 Flag = CLEAR;

    E_OPERATION_END_CONDITION EndCondition = OPERATION_END_CONDITION_NONE;
    E_OPERATION_END_CONDITION EndConditionTemp = OPERATION_END_CONDITION_NONE;

    EndConditionTemp = OPERATION_EndConditionCheck_CURRENT(sp_OP);
    if (EndConditionTemp) EndCondition = EndConditionTemp;

    EndConditionTemp = OPERATION_EndConditionCheck_CAPACITY(sp_OP);
    if (EndConditionTemp && !EndCondition) EndCondition = EndConditionTemp;

    EndConditionTemp = OPERATION_EndConditionCheck_WATT(sp_OP);
    if (EndConditionTemp && !EndCondition) EndCondition = EndConditionTemp;

    EndConditionTemp = OPERATION_EndConditionCheck_VOLTAGE(sp_OP);
    if (EndConditionTemp && !EndCondition) EndCondition = EndConditionTemp;

    EndConditionTemp = OPERATION_EndConditionCheck_TIME(sp_OP);
    if (EndConditionTemp && !EndCondition) EndCondition = EndConditionTemp;

    if (EndCondition && Flag == CLEAR) {
        Flag = SET;
        ReaultTemp = (EndCondition);  //1[ Time ], 2[ Ampere ], 3[ Capacity ], 4[ Watt ], 5[ Voltage ]

        switch (EndCondition)
        {
            case OPERATION_END_CONDITION_TIME:
                ValueTemp = (Uint32) ((*sp_OP).VARIABLE_EndConditionTime & 0xFFFFFFFF);
                break;
            case OPERATION_END_CONDITION_VOLTAGE:
                ValueTemp = (Uint32) (ABS_F((*sp_OP).VARIABLE_VoltageResult) * 10000.0);
                break;
            case OPERATION_END_CONDITION_CURRENT:
                ValueTemp = (Uint32) (ABS_F((*sp_OP).VARIABLE_CurrentResult) * 100000.0);
                break;
            case OPERATION_END_CONDITION_CAPACITY:
                ValueTemp = (Uint32) (ABS_F((*sp_OP).VARIABLE_CapacityResult) * 10000.0);
                break;
#ifdef DEV_SCHEDULE_WH
            case OPERATION_END_CONDITION_WATT:
                ValueTemp = (Uint32) (ABS_F((*sp_OP).VARIABLE_PowerResult_Wh) * 1000.0);
                break;
#endif

#ifdef DEV_SCHEDULE_CYCLE
            case OPERATION_CYCLE_END_CONDITION_VOLTAGE:
                ValueTemp = (Uint32) (ABS_F((*sp_OP).VARIABLE_VoltageResult) * 10000.0);
                break;
            case OPERATION_CYCLE_END_CONDITION_TIME:
                ValueTemp = (Uint32) ((*sp_OP).VARIABLE_CycleEndConditionTime & 0xFFFFFFFF);
                break;
            case OPERATION_CYCLE_END_CONDITION_CAPACITY:
                ValueTemp = (Uint32) (ABS_F((*sp_OP).VARIABLE_CYCLE_CapacityResult) * 10000.0);
                break;
#ifdef DEV_SCHEDULE_WH
            case OPERATION_CYCLE_END_CONDITION_WATT:
                ValueTemp = (Uint32) (ABS_F((*sp_OP).VARIABLE_CYCLE_PowerResult_Wh) * 1000.0);
                break;
#endif
            case OPERATION_CYCLE_END_CONDITION_LOW_VOLTAGE:
                ValueTemp = (Uint32) (ABS_F((*sp_OP).VARIABLE_VoltageResult) * 10000.0);
                break;
#endif
        }
    }

    if (((*sp_OP).VARIABLE_TimeSpanResult % COMMUNICATION_TX_DATA_LENGTH) >= COMMUNICATION_MAX_LENGTH_TX_DATA - 1) {
#if 0
        (*sp_OP).VARIABLE_EndConditionResult = ReaultTemp;  //1[ Time ], 2[ Ampere ], 3[ Capacity ], 4[ Watt ], 5[ Voltage ]
        (*sp_OP).VARIABLE_EndConditionValue = ValueTemp;    //1[ Time ], 2[ Ampere ], 3[ Capacity ], 4[ Watt ], 5[ Voltage ]

        Flag = CLEAR;
        ReaultTemp = CLEAR;
        ValueTemp = CLEAR;

        if (EndCondition) {
            EndConditionTemp = OPERATION_EndConditionCheck_LOOP(sp_OP);
            if (EndConditionTemp) EndCondition = EndConditionTemp;
        }
#else
        if (EndCondition) {
            if ((*sp_OP).CYCLE_LOOP_COUNT) {
                (*sp_OP).VARIABLE_CYCLE_LOOP_COUNT++;

                if ((*sp_OP).VARIABLE_CYCLE_LOOP_COUNT >= (*sp_OP).CYCLE_LOOP_COUNT) {
                    (*sp_OP).VARIABLE_CYCLE_LOOP_COUNT = CLEAR;

                    EndCondition = OPERATION_END_CONDITION_GOTO;
                } else {
                    EndCondition = OPERATION_END_CONDITION_RETRUN;
                }
            }

            if(ReaultTemp) {
                switch (ReaultTemp)
                {
                    case OPERATION_END_CONDITION_TIME:
                    if (((*sp_OP).STEP_INDEX + 1) != (*sp_OP).END_TIME_INDEX) {
                        EndCondition = (E_OPERATION_END_CONDITION) ReaultTemp;
                    }
                    break;
                    case OPERATION_END_CONDITION_VOLTAGE:
                    if (((*sp_OP).STEP_INDEX + 1) != (*sp_OP).END_VOLTAGE_INDEX) {
                        EndCondition = (E_OPERATION_END_CONDITION) ReaultTemp;
                    }
                    break;
                    case OPERATION_END_CONDITION_CURRENT:
                    if (((*sp_OP).STEP_INDEX + 1) != (*sp_OP).END_CURRENT_INDEX) {
                        EndCondition = (E_OPERATION_END_CONDITION) ReaultTemp;
                    }
                    break;
                    case OPERATION_END_CONDITION_CAPACITY:
                    if (((*sp_OP).STEP_INDEX + 1) != (*sp_OP).END_CAPACITY_INDEX) {
                        EndCondition = (E_OPERATION_END_CONDITION) ReaultTemp;
                    }
                    break;
                    case OPERATION_END_CONDITION_WATT:
                    if (((*sp_OP).STEP_INDEX + 1) != (*sp_OP).END_WATT_INDEX) {
                        EndCondition = (E_OPERATION_END_CONDITION) ReaultTemp;
                    }
                    break;
                }
            }
        }

        (*sp_OP).VARIABLE_EndConditionResult = ReaultTemp;   //1[ Time ], 2[ Ampere ], 3[ Capacity ], 4[ Watt ], 5[ Voltage ]
        (*sp_OP).VARIABLE_EndConditionValue = ValueTemp;     //1[ Time ], 2[ Ampere ], 3[ Capacity ], 4[ Watt ], 5[ Voltage ]

        Flag = CLEAR;
        ReaultTemp = CLEAR;
        ValueTemp = CLEAR;
#endif
    }

    return (EndCondition);    //SUCCESS Or FAIL
}

Uint16 OPERATION_Start(S_OPERATION_FILE *sp_OP)
{
    return (OPERATION_CellCheck(sp_OP));//SUCCESS Or FAIL
}

Uint16 OPERATION_Stop(S_OPERATION_FILE *sp_OP)
{
    //STOP START
    //Step 1. Rest all of Mux
    (*sp_OP).C_GateOutputAMP = CONTROL_GateOutputAMP(CONTROL_ALL, CONTROL_REST);
    (*sp_OP).C_GateOutputMUX = CONTROL_GateOutputMUX(CONTROL_ALL, CONTROL_REST);
    (*sp_OP).C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_CHARGE);

    //Step 2. Zero of Reference All
    (*sp_OP).C_VRefDAC = CONTROL_VoltageReferenceDAC(0.0);
    (*sp_OP).C_IRefDAC = CONTROL_CurrentReferenceDAC(CONTROL_ALL, 0.0);
    (*sp_OP).C_GateOffsetDAC = CONTROL_GateOffsetDAC(0.0);

    //Step 3.Off of Relay All
    if ((*sp_OP).COMMAND != COM_INPUT_RELAY_OFF) {
        (*sp_OP).C_VRELAY = CONTROL_VoltageRELAY(CONTROL_ON);
    }
    if ((*sp_OP).VARIABLE_StartFlag == CLEAR) {
        (*sp_OP).C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
    }

    //Step 4.Reset of Integration All
#if PCB_VERSION != 220
#if (FET == 200)
        (*sp_OP).C_IntSlopCapMUX = CONTROL_IntegrationSlopCapMUX(CONTROL_CAP_100nF);
#elif (FET == 240)
        (*sp_OP).C_IntSlopCapMUX = CONTROL_IntegrationSlopCapMUX(CONTROL_CAP_220nF);
#endif
#endif
    (*sp_OP).C_IntSlopMUX = CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_NONE);  //CONTROL_NONE Or CONTROL_GND
    (*sp_OP).C_IntResetMUX = CONTROL_IntegrationResetMUX(CONTROL_ALL, CONTROL_ON);
    DELAY_US(OPERATION_INTEGRATION_RESET_TIME_US_REST);  //Test Setting
    (*sp_OP).C_IntResetMUX = CONTROL_IntegrationResetMUX(CONTROL_ALL, CONTROL_OFF);

    //Step 5.Reset of Zero
    (*sp_OP).C_FINEIRefDAC = CONTROL_FINECurrentReferenceDAC(CONTROL_ALL, 0.0);
    (*sp_OP).C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, AD5293_MAX_F_VLAUE);
    //STOP END

    sp_OP->s_Slop.set = CLEAR;
    CPUTimer_stopTimer(CPUTIMER2_BASE);
    AD7608_OverSamplingSelect(AD7608_64);

    return (SUCCESS);
}

Uint16 OPERATION_AlramReset(S_OPERATION_FILE *sp_OP)
{
    (*sp_OP).VARIABLE_ALARM_DC.all = 0x0000;
    (*sp_OP).VARIABLE_ALARM_FAULT.all = 0x0000;
    (*sp_OP).VARIABLE_ALARM_GRID.all = 0x0000;
    (*sp_OP).VARIABLE_ALARM_WARING.all = 0x0000;
    (*sp_OP).VARIABLE_ALARM_Flag = CLEAR;
    (*sp_OP).VARIABLE_ALARM_UpDateFlag = CLEAR;
    return (SUCCESS);  //SUCCESS Or FAIL
}
Uint16 OPERATION_Alram(S_OPERATION_FILE *sp_OP)
{
    OPERATION_Emergency(sp_OP);
#if (VI_ADC_TUNE == 1)
    (*sp_OP).VARIABLE_ALARM_Flag |= ALARM_PowerConditionCheck(sp_OP);

    if(g_vi_tune.mode != kVoltage) {
        if (ABS_F((*sp_OP).s_OldADCData.SENSING_BatteryVoltage) < 0.02 ||
                ABS_F((*sp_OP).s_OldADCData.SENSING_BatteryVoltage) > 4.6) {
            (*sp_OP).VARIABLE_ALARM_Flag |= (*sp_OP).VARIABLE_ALARM_WARING.bit.VoltageFault = SET;
        }
    }

    if (ABS_F((*sp_OP).s_OldADCData.SENSING_Current) > ALARM_OVER_CURRENT_LIMIT) {
        (*sp_OP).VARIABLE_ALARM_Flag |= (*sp_OP).VARIABLE_ALARM_WARING.bit.CurrentFault = SET;
    }

    (*sp_OP).VARIABLE_ALARM_Flag |= ALARM_HeatConditionCheck(sp_OP);

    (*sp_OP).VARIABLE_ALARM_Flag |= (*sp_OP).VARIABLE_ALARM_WARING.bit.EMOSwitchOn;
#elif (SLOPE_TUNE == 1)
    (*sp_OP).VARIABLE_ALARM_Flag |= ALARM_PowerConditionCheck(sp_OP);

    if (ABS_F((*sp_OP).s_OldADCData.SENSING_BatteryVoltage) < 0.02 ||
            ABS_F((*sp_OP).s_OldADCData.SENSING_BatteryVoltage) > 4.6) {
        (*sp_OP).VARIABLE_ALARM_Flag |= (*sp_OP).VARIABLE_ALARM_WARING.bit.VoltageFault = SET;
    }

    if (ABS_F((*sp_OP).s_OldADCData.SENSING_Current) > ALARM_OVER_CURRENT_LIMIT) {
        (*sp_OP).VARIABLE_ALARM_Flag |= (*sp_OP).VARIABLE_ALARM_WARING.bit.CurrentFault = SET;
    }

    (*sp_OP).VARIABLE_ALARM_Flag |= ALARM_HeatConditionCheck(sp_OP);

    (*sp_OP).VARIABLE_ALARM_Flag |= (*sp_OP).VARIABLE_ALARM_WARING.bit.EMOSwitchOn;
#else
    (*sp_OP).VARIABLE_ALARM_Flag |= ALARM_PowerConditionCheck(sp_OP);
    (*sp_OP).VARIABLE_ALARM_Flag |= ALARM_LineConditionCheck(sp_OP);
    (*sp_OP).VARIABLE_ALARM_Flag |= ALARM_CurrentConditionCheck(sp_OP);
    (*sp_OP).VARIABLE_ALARM_Flag |= ALARM_VoltageConditionCheck(sp_OP);
    (*sp_OP).VARIABLE_ALARM_Flag |= ALARM_HeatConditionCheck(sp_OP);
    (*sp_OP).VARIABLE_ALARM_Flag |= ALARM_CommunicationConditionCheck(sp_OP);

    (*sp_OP).VARIABLE_ALARM_Flag |= ALARM_CC_Fault(sp_OP);
    (*sp_OP).VARIABLE_ALARM_Flag |= ALARM_CV_Fault(sp_OP);

    (*sp_OP).VARIABLE_ALARM_Flag |= (*sp_OP).VARIABLE_ALARM_GRID.bit.BatteryFault;
    (*sp_OP).VARIABLE_ALARM_Flag |= (*sp_OP).VARIABLE_ALARM_WARING.bit.EMOSwitchOn;
    (*sp_OP).VARIABLE_ALARM_Flag |= (*sp_OP).VARIABLE_ALARM_WARING.bit.BatteryNone;
    (*sp_OP).VARIABLE_ALARM_Flag |= (*sp_OP).VARIABLE_ALARM_WARING.bit.SensingConnector;
    (*sp_OP).VARIABLE_ALARM_Flag |= (*sp_OP).VARIABLE_ALARM_WARING.bit.CurrentConnector;
#endif
    return((*sp_OP).VARIABLE_ALARM_Flag);//SUCCESS Or FAIL
}

Uint16 OPERATION_Emergency(S_OPERATION_FILE *sp_OP)
{
    if (SENSING_Emergency() == SENSING_EMERGENCY_ON) {
        (*sp_OP).VARIABLE_ALARM_WARING.bit.EMOSwitchOn = SET;
    } else {
        (*sp_OP).VARIABLE_ALARM_WARING.bit.EMOSwitchOn = CLEAR;
    }

    return ((*sp_OP).VARIABLE_ALARM_WARING.bit.EMOSwitchOn);  //SUCCESS Or FAIL
}

Uint16 OPERATION_ValueReset(S_OPERATION_FILE *sp_OP)
{
    (*sp_OP).VARIABLE_UpdateData = CLEAR;
    (*sp_OP).VARIABLE_UpdateData_OLD = CLEAR;
    (*sp_OP).VARIABLE_UpdateAlramTime = CLEAR;

    (*sp_OP).VARIABLE_StartFlag = CLEAR;
    (*sp_OP).VARIABLE_VoltageRef = 0.0;
    (*sp_OP).VARIABLE_CurrentRef = 0.0;
    (*sp_OP).VARIABLE_PowerRef = 0.0;
    (*sp_OP).VARIABLE_VoltageResult = 0.0;
    (*sp_OP).VARIABLE_CurrentResult = 0.0;
    (*sp_OP).VARIABLE_PowerResult = 0.0;
#ifdef DEV_SCHEDULE_WH
    (*sp_OP).VARIABLE_PowerResult_Wh = 0.0;
#endif
    (*sp_OP).VARIABLE_CapacityResult = 0.0;
    (*sp_OP).VARIABLE_TimeSpanResult = 0.0;
    (*sp_OP).VARIABLE_EndConditionResult = CLEAR;

    (*sp_OP).VARIABLE_EndConditionTime = CLEAR;
    (*sp_OP).VARIABLE_EndConditionResult = CLEAR;
    (*sp_OP).VARIABLE_EndConditionValue = CLEAR;

    (*sp_OP).VARIABLE_ALARM_Flag = CLEAR;
    (*sp_OP).VARIABLE_ALARM_RequestFlag = CLEAR;
    (*sp_OP).VARIABLE_ALARM_UpDateFlag = CLEAR;
    (*sp_OP).VARIABLE_ALARM_WARING.all = CLEAR;
    (*sp_OP).VARIABLE_ALARM_GRID.all = CLEAR;
    (*sp_OP).VARIABLE_ALARM_DC.all = CLEAR;
    (*sp_OP).VARIABLE_ALARM_FAULT.all = CLEAR;

    (*sp_OP).CV_MovingData[0] = CLEAR;
    (*sp_OP).CC_MovingData[0] = CLEAR;

    (*sp_OP).COMMAND = COM_NONE;
    (*sp_OP).VARIABLE_COMMAND_OLD = COM_NONE;
    (*sp_OP).DIRECT_COMMAND = COM_NONE;
    (*sp_OP).MODE = MODE_NONE;
    (*sp_OP).VARIABLE_MODE_OLD = MODE_NONE;
    (*sp_OP).OPERATIN = OP_DEFAULT;
    (*sp_OP).VARIABLE_OPERATIN = OP_SEND_DEFAULT;
    (*sp_OP).SAMPLING_TIME = CLEAR;
    (*sp_OP).SEND_TIME = CLEAR;
    (*sp_OP).STEP_INDEX = CLEAR;
    (*sp_OP).VARIABLE_STEP_INDEX = CLEAR;
    (*sp_OP).CYCLE_LOOP_COUNT = CLEAR;
    (*sp_OP).VARIABLE_CYCLE_LOOP_COUNT = CLEAR;
    (*sp_OP).GOTO_INDEX = CLEAR;
    (*sp_OP).RETURN_INDEX = CLEAR;
    (*sp_OP).END_TIME_INDEX = CLEAR;
    (*sp_OP).END_VOLTAGE_INDEX = CLEAR;
    (*sp_OP).END_CURRENT_INDEX = CLEAR;
    (*sp_OP).END_CAPACITY_INDEX = CLEAR;
    (*sp_OP).END_WATT_INDEX = CLEAR;
    (*sp_OP).END_TIME_CONDITION = CLEAR;
    (*sp_OP).END_VOLTAGE_CONDITION = CLEAR;
    (*sp_OP).END_CURRENT_CONDITION = CLEAR;
    (*sp_OP).END_WATT_CONDITION = CLEAR;
    (*sp_OP).END_CAPACITY_CONDITION = CLEAR;
    (*sp_OP).CYCLE_INDEX = CLEAR;

    (*sp_OP).s_Slop.mode = OP_CC;
    (*sp_OP).s_Slop.time = 0;
    (*sp_OP).s_Slop.end = 200;
    (*sp_OP).s_Slop.set = CLEAR;
    (*sp_OP).s_Slop.gain = 1000.0;
    (*sp_OP).s_Slop.offset = 50000.0;
    (*sp_OP).s_Slop.res = 50000.0;

    return(SUCCESS);//SUCCESS Or FAIL
}

Uint16 OPERATION_Reset(S_OPERATION_FILE *sp_OP)
{
    OPERATION_ValueReset(sp_OP);
    OPERATION_AlramReset(sp_OP);

    (*sp_OP).C_VRELAY = CONTROL_VoltageRELAY(CONTROL_ON);
    (*sp_OP).C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
    (*sp_OP).C_DIORELAY = CONTROL_DIORELAY(CONTROL_DIO_ALL, CONTROL_OFF);

    (*sp_OP).C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_CHARGE);

    //Step 4.Reset of Integration All
#if PCB_VERSION != 220
#if (FET == 200)
        (*sp_OP).C_IntSlopCapMUX = CONTROL_IntegrationSlopCapMUX(CONTROL_CAP_100nF);
#elif (FET == 240)
        (*sp_OP).C_IntSlopCapMUX = CONTROL_IntegrationSlopCapMUX(CONTROL_CAP_220nF);
#endif
#endif
    (*sp_OP).C_IntSlopMUX = CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_NONE);  //CONTROL_NONE Or CONTROL_GND
    (*sp_OP).C_IntResetMUX = CONTROL_IntegrationResetMUX(CONTROL_ALL, CONTROL_ON);
    DELAY_US(200);  //Test Setting
    (*sp_OP).C_IntResetMUX = CONTROL_IntegrationResetMUX(CONTROL_ALL, CONTROL_OFF);

    (*sp_OP).C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, AD5293_MAX_F_VLAUE);
    (*sp_OP).C_GateOffsetDAC = CONTROL_GateOffsetDAC(0.0);

    (*sp_OP).C_GateOutputMUX = CONTROL_GateOutputMUX(CONTROL_ALL, CONTROL_REST);
    (*sp_OP).C_GateOutputAMP = CONTROL_GateOutputAMP(CONTROL_ALL, CONTROL_REST);

    (*sp_OP).C_VRefDAC = CONTROL_VoltageReferenceDAC(0.0);
    (*sp_OP).C_IRefDAC = CONTROL_CurrentReferenceDAC(CONTROL_ALL, 0.0);
    (*sp_OP).C_FINEIRefDAC = CONTROL_FINECurrentReferenceDAC(CONTROL_ALL, 0.0);

    sp_OP->s_Slop.set = CLEAR;
    CPUTimer_stopTimer(CPUTIMER2_BASE);
    AD7608_OverSamplingSelect(AD7608_64);

    return (SUCCESS);  //SUCCESS Or FAIL
}

Uint16 OPERATION_Rest(S_OPERATION_FILE *sp_OP)
{
        //STOP START
        //Step 1. Rest all of Mux

    if ((*sp_OP).VARIABLE_COMMAND_OLD != (*sp_OP).COMMAND) {
        (*sp_OP).C_GateOutputAMP = CONTROL_GateOutputAMP(CONTROL_ALL, CONTROL_REST);
        (*sp_OP).C_GateOutputMUX = CONTROL_GateOutputMUX(CONTROL_ALL, CONTROL_REST);
        (*sp_OP).C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_CHARGE);  //볼데이지 검출을 위해
        //Step 2. Zero of Reference All
        (*sp_OP).C_VRefDAC = CONTROL_VoltageReferenceDAC(0.0);
        (*sp_OP).C_IRefDAC = CONTROL_CurrentReferenceDAC(CONTROL_ALL, 0.0);
        (*sp_OP).C_GateOffsetDAC = CONTROL_GateOffsetDAC(0.0);
        //Step 3.Off of Relay All
//      (*sp_OP).C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_ON);
//      (*sp_OP).C_VRELAY = CONTROL_VoltageRELAY(CONTROL_ON);
        //Step 3.Reset of Integration All

        //Step 4.Reset of Integration All
#if PCB_VERSION != 220
#if (FET == 200)
        (*sp_OP).C_IntSlopCapMUX = CONTROL_IntegrationSlopCapMUX(CONTROL_CAP_100nF);
#elif (FET == 240)
        (*sp_OP).C_IntSlopCapMUX = CONTROL_IntegrationSlopCapMUX(CONTROL_CAP_220nF);
#endif
#endif
        (*sp_OP).C_IntSlopMUX = CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_NONE);  //CONTROL_NONE Or CONTROL_GND
        (*sp_OP).C_IntResetMUX = CONTROL_IntegrationResetMUX(CONTROL_ALL, CONTROL_ON);

        DELAY_US(OPERATION_INTEGRATION_RESET_TIME_US_REST);  //Test Setting

        (*sp_OP).C_IntResetMUX = CONTROL_IntegrationResetMUX(CONTROL_ALL, CONTROL_OFF);
        //Step 4.Reset of Zero
        (*sp_OP).C_FINEIRefDAC = CONTROL_FINECurrentReferenceDAC(CONTROL_ALL, 0.0);
        (*sp_OP).C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, AD5293_MAX_F_VLAUE);
        //STOP END

        sp_OP->s_Slop.set = CLEAR;
        CPUTimer_stopTimer(CPUTIMER2_BASE);
        AD7608_OverSamplingSelect(AD7608_64);
    }

    return (SUCCESS);
}

Uint16 OPERATION_Charge(S_OPERATION_FILE *sp_OP)
{
    E_OPERATION_COM_SELECT Command = COM_NONE;
    E_CONTROL_OPERATION_MODE_SELECT e_Flag = CONTROL_REST;

    Command = (*sp_OP).COMMAND;

    if(Command == COM_CHARGE)e_Flag = CONTROL_CHARGE;
    else e_Flag = CONTROL_DISCHARGE;

    if ((*sp_OP).VARIABLE_COMMAND_OLD != Command) {

        // 대전류 충전 혹은 대전류 방전 후 방전, 혹은 충전을 바로 진행하면 반대 방향으로 오버슛이 생김
        // 따라서 위와 같은 경우를 대비하기 위해 0으로 초기화를 한 후 잠시으 딜레이를 주어서 해결
        (*sp_OP).s_Slop.set = CLEAR;

        (*sp_OP).C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, (float) 1000.0);

        (*sp_OP).C_GateOffsetDAC = CONTROL_GateOffsetDAC((float32) (0.0));
        (*sp_OP).C_IRefDAC = CONTROL_CurrentReferenceDAC(CONTROL_ALL, (float32) (0.0));
        (*sp_OP).C_FINEIRefDAC = CONTROL_FINECurrentReferenceDAC(CONTROL_ALL, (float32) (0.0));

        DELAY_US(200);

        (*sp_OP).C_VRefDAC = CONTROL_VoltageReferenceDAC((float32) 0.0);

        (*sp_OP).C_GateOutputAMP = CONTROL_GateOutputAMP(CONTROL_ALL, CONTROL_OPERATION_INIT);
        (*sp_OP).C_GateOutputMUX = CONTROL_GateOutputMUX(CONTROL_ALL, CONTROL_OPERATION_INIT);
        (*sp_OP).C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_OPERATION_INIT);
#if PCB_VERSION != 220
#if (FET == 200)
        (*sp_OP).C_IntSlopCapMUX = CONTROL_IntegrationSlopCapMUX(CONTROL_CAP_100nF);
#elif (FET == 240)
        (*sp_OP).C_IntSlopCapMUX = CONTROL_IntegrationSlopCapMUX(CONTROL_CAP_220nF);
#endif
#endif
        (*sp_OP).C_IntSlopMUX = CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_NONE);  //CONTROL_NONE Or CONTROL_GND
        (*sp_OP).C_IntResetMUX = CONTROL_IntegrationResetMUX(CONTROL_ALL, CONTROL_ON);

        if (Command == COM_CHARGE) DELAY_US(OPERATION_INTEGRATION_RESET_TIME_US_CHA);
        else DELAY_US(OPERATION_INTEGRATION_RESET_TIME_US_DIS);

        (*sp_OP).C_IntResetMUX = CONTROL_IntegrationResetMUX(CONTROL_ALL, CONTROL_OFF);
    }

    OPERATION_IntegrationSlopControl(sp_OP);

    if ((*sp_OP).VARIABLE_COMMAND_OLD != Command) {
        OPERATION_GateOffsetControl(sp_OP);

        (*sp_OP).C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, e_Flag);

        DELAY_US(500);

        if (Command == COM_CHARGE) {
            (*sp_OP).C_VRefDAC = CONTROL_VoltageReferenceDAC(OPERATION_LIMIT_MAX_VOLTAGE);
        } else {
            (*sp_OP).C_VRefDAC = CONTROL_VoltageReferenceDAC((OPERATION_LIMIT_MIN_VOLTAGE * (float) (-1.0)));
        }

        (*sp_OP).C_GateOutputMUX = CONTROL_GateOutputMUX(CONTROL_ALL, e_Flag);
        (*sp_OP).C_GateOutputAMP = CONTROL_GateOutputAMP(CONTROL_ALL, e_Flag);
        (*sp_OP).C_IntSlopMUX = CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_PM);
    }

    OPEARTION_CurrentControl(sp_OP);
    OPERATION_VoltageControl(sp_OP);

    return(SUCCESS);//SUCCESS Or FAIL
}

Uint16 OPERATION_Sensing(S_OPERATION_FILE *sp_OP)
{
    float32 Current = 0.0;
    static Uint16 Index = 0;
    static Uint16 old_cycle = 0;

    if ((*sp_OP).OPERATIN == OP_CHARGE_CP || (*sp_OP).OPERATIN == OP_DISCHARGE_CP || (*sp_OP).OPERATIN == OP_CHARGE_CPCV || (*sp_OP).OPERATIN == OP_DISCHARGE_CPCV) {
        (*sp_OP).VARIABLE_PowerRef = (float) ((*sp_OP).VARIABLE_CurrentRef / ABS_F((*sp_OP).VARIABLE_VoltageResult));
        //CP kW -> W
        //(*sp_OP).VARIABLE_PowerRef *= (float) (1000.0);

        Current = (*sp_OP).VARIABLE_PowerRef;
    } else if (sp_OP->OPERATIN == OP_CHARGE_CR || sp_OP->OPERATIN == OP_DISCHARGE_CR) {
        (*sp_OP).VARIABLE_ResistanceRef = (float) (ABS_F((*sp_OP).VARIABLE_VoltageResult) / (*sp_OP).VARIABLE_CurrentRef);

        Current = (*sp_OP).VARIABLE_ResistanceRef;
    } else {
        Current = (*sp_OP).VARIABLE_CurrentRef;
    }


#if (Current_Spec == 200)
    if (Current > 200.0) {
        Current = 200.0;
    }
#elif (Current_Spec == 300)
    if (Current > 300.0) {
        Current = 300.0;
    }
#endif
    SENSING_ADCResultData(&(*sp_OP).s_ADCData);

    (*sp_OP).s_OldADCData = (*sp_OP).s_ADCData;

    if ((*sp_OP).COMMAND == COM_DISCHARGE) {
        (*sp_OP).s_OldADCData.SENSING_BatteryVoltage = CALIBRATION_ConversionVoltageADC((*sp_OP).s_OldADCData.SENSING_BatteryVoltage * (float) (-1.0),
                                                                                        VoltageDischarge_ADC,
                                                                                        CALIBRATION_VOLATAGE_MIN_VALUE,
                                                                                        CALIBRATION_VOLATAGE_UNIT_VALUE,
                                                                                        CALIBRATION_VOLATAGE_MAX_ARRAY_LENGTH);
    } else {
        (*sp_OP).s_OldADCData.SENSING_BatteryVoltage = CALIBRATION_ConversionVoltageADC((*sp_OP).s_OldADCData.SENSING_BatteryVoltage * (float) (-1.0),
                                                                                        VoltageCharge_ADC,
                                                                                        CALIBRATION_VOLATAGE_MIN_VALUE,
                                                                                        CALIBRATION_VOLATAGE_UNIT_VALUE,
                                                                                        CALIBRATION_VOLATAGE_MAX_ARRAY_LENGTH);
        (*sp_OP).s_OldADCData.SENSING_BatteryVoltage *= (float) (-1.0);
    }

    if ((*sp_OP).COMMAND == COM_DISCHARGE) {
        (*sp_OP).s_OldADCData.SENSING_Current = CALIBRATION_ConversionCurrentADC((*sp_OP).s_OldADCData.SENSING_Current * (float) (-1.0),
                                                                                 CurrentDischarge_ADC,
                                                                                 CurrentDischarge_REF,
                                                                                 CALIBRATION_CURRENT_MIN_VALUE,
                                                                                 CALIBRATION_CURRENT_UNIT_VALUE,
                                                                                 CALIBRATION_CURRENT_MAX_ARRAY_LENGTH);

        (*sp_OP).s_OldADCData.SENSING_Current *= (float) (-1.0);
    } else {
        (*sp_OP).s_OldADCData.SENSING_Current = CALIBRATION_ConversionCurrentADC((*sp_OP).s_OldADCData.SENSING_Current,
                                                                                 CurrentCharge_ADC,
                                                                                 CurrentCharge_REF,
                                                                                 CALIBRATION_CURRENT_MIN_VALUE,
                                                                                 CALIBRATION_CURRENT_UNIT_VALUE,
                                                                                 CALIBRATION_CURRENT_MAX_ARRAY_LENGTH);
    }

    (*sp_OP).VARIABLE_VoltageResult = (*sp_OP).s_OldADCData.SENSING_BatteryVoltage;

    if ((*sp_OP).COMMAND == COM_CHARGE || (*sp_OP).COMMAND == COM_DISCHARGE) {
        (*sp_OP).VARIABLE_CurrentResult = (*sp_OP).s_OldADCData.SENSING_Current;
    } else {
        (*sp_OP).VARIABLE_CurrentResult = 0.0;
    }

    (*sp_OP).VARIABLE_PowerResult = ABS_F((*sp_OP).s_OldADCData.SENSING_BatteryVoltage) * ABS_F((*sp_OP).VARIABLE_CurrentResult);

    if (Index != (*sp_OP).STEP_INDEX) {
        Index = (*sp_OP).STEP_INDEX;
        (*sp_OP).VARIABLE_CapacityResult = CLEAR;
#ifdef DEV_SCHEDULE_WH
        (*sp_OP).VARIABLE_PowerResult_Wh = CLEAR;
#endif
    }

#ifdef DEV_SCHEDULE_CYCLE
    if(old_cycle != (*sp_OP).CYCLE_INDEX) {
        old_cycle = (*sp_OP).CYCLE_INDEX;
#ifdef DEV_SCHEDULE_WH
        (*sp_OP).VARIABLE_CYCLE_PowerResult_Wh = CLEAR;
#endif
        (*sp_OP).VARIABLE_CYCLE_CapacityResult = CLEAR;
        (*sp_OP).VARIABLE_CYCLE_Charge_CapacityResult = CLEAR;
        (*sp_OP).VARIABLE_CYCLE_Discharge_CapacityResult = CLEAR;
    }
#endif

#ifdef DEV_SCHEDULE_WH
    (*sp_OP).VARIABLE_PowerResult_Wh += (ABS_F((*sp_OP).VARIABLE_PowerResult) * (float) (1.0 / 3600.0) * (float) (0.001));  // * (float)(0.001) = Pwoer/1ms이므로 Pwoer/1h 환산 하려고 한것임.
    (*sp_OP).VARIABLE_CYCLE_PowerResult_Wh += (ABS_F((*sp_OP).VARIABLE_PowerResult) * (float) (1.0 / 3600.0) * (float) (0.001));  // * (float)(0.001) = Pwoer/1ms이므로 Pwoer/1h 환산 하려고 한것임.
#endif
    (*sp_OP).VARIABLE_CapacityResult += (ABS_F((*sp_OP).VARIABLE_CurrentResult) * (float) (1.0 / 3600.0) * (float) (0.001));  // * (float)(0.001) = Pwoer/1ms이므로 Pwoer/1h 환산 하려고 한것임.
    (*sp_OP).VARIABLE_CYCLE_CapacityResult += (ABS_F((*sp_OP).VARIABLE_CurrentResult) * (float) (1.0 / 3600.0) * (float) (0.001));  // * (float)(0.001) = Pwoer/1ms이므로 Pwoer/1h 환산 하려고 한것임.

    if ((*sp_OP).COMMAND == COM_CHARGE) {
        //Cycle capacity charge
        (*sp_OP).VARIABLE_CYCLE_Charge_CapacityResult += (*sp_OP).VARIABLE_CYCLE_CapacityResult;
    } else if ((*sp_OP).COMMAND == COM_DISCHARGE) {
        //Cycle capacity discharge
        (*sp_OP).VARIABLE_CYCLE_Discharge_CapacityResult += (*sp_OP).VARIABLE_CYCLE_CapacityResult;
    }

    return (SUCCESS);  //SUCCESS Or FAIL
}

Uint16 OPERATION_VoltageControl(S_OPERATION_FILE *sp_OP)
{
    float32 RefVoltage = 0.0;
    static Uint16 Timer = 30;  //30ms
    float32 NewVoltage = 0.0;
    static float Voltage = 0.0;
    static Uint16 Index = 0;

    NewVoltage = (*sp_OP).VARIABLE_VoltageRef;

    if ((*sp_OP).COMMAND != (*sp_OP).VARIABLE_COMMAND_OLD) {
        Timer = 30;
    } else {
        if (Timer) Timer--;
        else Timer = CLEAR;
    }

    if ((!(Timer) && NewVoltage != Voltage) || (*sp_OP).COMMAND != (*sp_OP).VARIABLE_COMMAND_OLD || Index != (*sp_OP).STEP_INDEX) {

        Index = (*sp_OP).STEP_INDEX;

        switch ((*sp_OP).COMMAND)
        {
            case COM_CHARGE:
                if ((*sp_OP).OPERATIN == OP_CHARGE_CC) {
                    (*sp_OP).C_VRefDAC = CONTROL_VoltageReferenceDAC(OPERATION_LIMIT_MAX_VOLTAGE);
                } else if ((*sp_OP).OPERATIN == OP_CHARGE_CCCV || (*sp_OP).OPERATIN == OP_CHARGE_CPCV) {
                    RefVoltage = CALIBRATION_ConversionREF((*sp_OP).VARIABLE_VoltageRef,
                                                           VoltageCharge_REF,
                                                           CALIBRATION_VOLATAGE_MIN_VALUE,
                                                           CALIBRATION_VOLATAGE_UNIT_VALUE,
                                                           CALIBRATION_VOLATAGE_MAX_ARRAY_LENGTH);
                    (*sp_OP).C_VRefDAC = CONTROL_VoltageReferenceDAC(RefVoltage);
                }
                Voltage = (*sp_OP).VARIABLE_VoltageRef;
                break;

            case COM_DISCHARGE:
                if ((*sp_OP).OPERATIN == OP_DISCHARGE_CC) {
                    (*sp_OP).C_VRefDAC = CONTROL_VoltageReferenceDAC(OPERATION_LIMIT_MIN_VOLTAGE); //if apply * (float) (-1.0), converts from the OPERATION_LIMIT_MIN_VOLTAGE to CV.
                } else if ((*sp_OP).OPERATIN == OP_DISCHARGE_CCCV || (*sp_OP).OPERATIN == OP_DISCHARGE_CPCV) {
                    RefVoltage = CALIBRATION_ConversionREF((*sp_OP).VARIABLE_VoltageRef,
                                                           VoltageDischarge_REF,
                                                           CALIBRATION_VOLATAGE_MIN_VALUE,
                                                           CALIBRATION_VOLATAGE_UNIT_VALUE,
                                                           CALIBRATION_VOLATAGE_MAX_ARRAY_LENGTH);
                    (*sp_OP).C_VRefDAC = CONTROL_VoltageReferenceDAC(RefVoltage * (float) (-1.0));
                }
                Voltage = (*sp_OP).VARIABLE_VoltageRef;
                break;
        }
    }

    return (SUCCESS);  //SUCCESS Or FAIL
}

Uint16 OPERATION_IntegrationSlopSelect(float32 Current)
{
    Uint16 SelectAddress = 0;

    if (Current < 10.0) {
        SelectAddress = (Uint16) (Current);
    } else {
        SelectAddress = (Uint16) (Current / 10);
        SelectAddress += 9;
    }

    if (SelectAddress >= OPERATION_INTEGRATION_SLOP_ARRAY_LENGTH) {
        SelectAddress = (OPERATION_INTEGRATION_SLOP_ARRAY_LENGTH - 1);
    }

    return (SelectAddress);
}

Uint16 OPERATION_IntegrationSlopControl(S_OPERATION_FILE *sp_OP)
{
    float32 new_current_ref = 0.0;
    static float32 old_current_ref = 0.0;
    Uint32 temp32 = 0;

    Uint16 slop_index = 0;

    float32 sensing_current = 0.0;

    static float32 new_slop = AD5293_MAX_F_VLAUE;
    static float32 old_slop = AD5293_MAX_F_VLAUE;
    Uint32 slop_temp = 0;

    float32 adc7608[8] = { 0.0, };
    float32 sensing_bat_voltage = 0.0;

    if ((*sp_OP).OPERATIN == OP_CHARGE_CP || (*sp_OP).OPERATIN == OP_DISCHARGE_CP || (*sp_OP).OPERATIN == OP_CHARGE_CPCV || (*sp_OP).OPERATIN == OP_DISCHARGE_CPCV) {
        (*sp_OP).VARIABLE_PowerRef = (float) ((*sp_OP).VARIABLE_CurrentRef / ABS_F((*sp_OP).VARIABLE_VoltageResult));
        //CP kW -> W
        //(*sp_OP).VARIABLE_PowerRef *= (float) (1000.0);

        new_current_ref = (*sp_OP).VARIABLE_PowerRef;
        sp_OP->s_Slop.mode = OP_CP;

    } else if (sp_OP->OPERATIN == OP_CHARGE_CR || sp_OP->OPERATIN == OP_DISCHARGE_CR) {
        (*sp_OP).VARIABLE_ResistanceRef = (float) (ABS_F((*sp_OP).VARIABLE_VoltageResult) / (*sp_OP).VARIABLE_CurrentRef);

        new_current_ref = (*sp_OP).VARIABLE_ResistanceRef;
        sp_OP->s_Slop.mode = OP_CR;
    } else {
        new_current_ref = (*sp_OP).VARIABLE_CurrentRef;
        sp_OP->s_Slop.mode = OP_CC;
    }

    if (new_current_ref > 300.0) {
        new_current_ref = 300.0;
    }

    temp32 = (Uint32) (new_current_ref * 1000);
    new_current_ref = (float) (temp32) * (float) (0.001);

    if (sp_OP->s_Slop.set == CLEAR && (*sp_OP).COMMAND != (*sp_OP).VARIABLE_COMMAND_OLD || old_current_ref != new_current_ref) {

        slop_index = OPERATION_IntegrationSlopSelect(new_current_ref);

        sp_OP->s_Slop.set = SET;

        if (sp_OP->s_Slop.mode == OP_CC) {
#if 1
            sensing_current = ABS_F((*sp_OP).s_OldADCData.SENSING_Current);
            sensing_bat_voltage = ABS_F((*sp_OP).s_OldADCData.SENSING_BatteryVoltage);
#else
            AD7608_Conversion();
            AD7608_Read(AD7608_10_VOLTAGE, AD7608_NON, adc7608);

            sensing_bat_voltage = ABS_F((*sp_OP).s_OldADCData.SENSING_BatteryVoltage);
            sensing_current = ABS_F((adc7608[1] * SENSING_CURRENT_GAIN ) + (adc7608[2] * SENSING_CURRENT_GAIN ) + (adc7608[3] * SENSING_CURRENT_GAIN ) + (adc7608[4] * SENSING_CURRENT_GAIN ));
#endif
            sp_OP->s_Slop.time = 0;
            sp_OP->s_Slop.end = 200;

            if ((*sp_OP).COMMAND == COM_CHARGE) {
                sp_OP->s_Slop.offset = IntegrationChargeSlopOffset[slop_index];
                sp_OP->s_Slop.gain = IntegrationChargeSlopGain[slop_index];

#if 1
                //case 1. battery status exception handling
                // low gain = fast
                // high gain = slow
                sp_OP->s_Slop.offset *= (1.0 - ((sensing_bat_voltage - 3.5) / 5.0));
                sp_OP->s_Slop.gain *= (1.0 - ((sensing_bat_voltage - 3.5) / 5.0));

#endif
#if 0
                //case 2. before operation exception handling
                //same condition(charge -> charge, up)
                if ((*sp_OP).VARIABLE_COMMAND_OLD == COM_CHARGE) {
                    if (old_current_ref > 50.0)
                    {
                        sp_OP->s_Slop.offset *= 0.75;
                        sp_OP->s_Slop.gain *= 0.75;
                    }
                } else if ((*sp_OP).VARIABLE_COMMAND_OLD == COM_DISCHARGE) {  //different condition(discharge -> charge, switching)
                    if (new_current_ref >= 250.0) {
                        sp_OP->s_Slop.offset *= 1.3;
                        sp_OP->s_Slop.gain *= 1.3;
                    }
                }
#endif
            } else {
                sp_OP->s_Slop.offset = IntegrationDischargeSlopOffset[slop_index];
                sp_OP->s_Slop.gain = IntegrationDischargeSlopGain[slop_index];

#if 1
                //case 1. battery status exception handling
                sp_OP->s_Slop.offset *= (1.0 + ((sensing_bat_voltage - 3.5) / 5.0));
                sp_OP->s_Slop.gain *= (1.0 + ((sensing_bat_voltage - 3.5) / 5.0));

#endif
            }

            //new_slop = sp_OP->s_Slop.offset;
            new_slop = (sensing_current * sp_OP->s_Slop.gain) + sp_OP->s_Slop.offset;

            AD7608_OverSamplingSelect(AD7608_NON);
            CPUTimer_reloadTimerCounter(CPUTIMER2_BASE);
            CPUTimer_startTimer(CPUTIMER2_BASE);
        } else {
            CPUTimer_stopTimer(CPUTIMER2_BASE);
            sp_OP->s_Slop.time = 0;
            AD7608_OverSamplingSelect(AD7608_64);
            CPUTimer_stopTimer(CPUTIMER2_BASE);
        }

        old_current_ref = new_current_ref;
    }

    if (sp_OP->s_Slop.mode == OP_CP || sp_OP->s_Slop.mode == OP_CR) {
        if (sp_OP->s_Slop.set == SET) {
            sensing_current = ABS_F((*sp_OP).s_OldADCData.SENSING_Current);

            switch ((*sp_OP).COMMAND)
            {
                case COM_CHARGE:
                    new_slop = (sensing_current * IntegrationChargeSlopGain[slop_index]) + IntegrationChargeSlopOffset[slop_index];
                    break;
                case COM_DISCHARGE:
                    new_slop = (sensing_current * IntegrationDischargeSlopGain[slop_index]) + IntegrationDischargeSlopOffset[slop_index];
                    break;
            }

            if (sensing_current >= new_current_ref * 0.8) {
                sp_OP->s_Slop.set = CLEAR;
                switch ((*sp_OP).COMMAND)
                {
                    case COM_CHARGE:
                        new_slop = (new_current_ref * IntegrationChargeSlopGain[slop_index]) + IntegrationChargeSlopOffset[slop_index];
                        break;
                    case COM_DISCHARGE:
                        new_slop = (new_current_ref * IntegrationDischargeSlopGain[slop_index]) + IntegrationDischargeSlopOffset[slop_index];
                        break;
                }
            }
        }
    }

    slop_temp = (Uint32) new_slop;
    slop_temp = slop_temp - (slop_temp % AD5293_RESOLUTION);
    new_slop = (float) slop_temp;

    if (new_slop > AD5293_MAX_F_VLAUE) new_slop = AD5293_MAX_F_VLAUE;

    if (old_slop != new_slop || (*sp_OP).COMMAND != (*sp_OP).VARIABLE_COMMAND_OLD) {
        old_slop = new_slop;
        (*sp_OP).C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, old_slop);
    }

    return (SUCCESS);  //SUCCESS Or FAIL
}

Uint16 OPERATION_GateOffsetControl(S_OPERATION_FILE *sp_OP)
{
    float32 TotalCurrent = 0.0;
    float32 Value = 0.0;
    float32 gain = 0.0;
    float32 bat_rate = 0.0;

    float sensing_bat_voltage = 0.0;

    if ((*sp_OP).OPERATIN == OP_CHARGE_CP || (*sp_OP).OPERATIN == OP_DISCHARGE_CP || (*sp_OP).OPERATIN == OP_CHARGE_CPCV || (*sp_OP).OPERATIN == OP_DISCHARGE_CPCV) {
        (*sp_OP).VARIABLE_PowerRef = (float) ((*sp_OP).VARIABLE_CurrentRef / ABS_F((*sp_OP).VARIABLE_VoltageResult));
        //CP kW -> W
        //(*sp_OP).VARIABLE_PowerRef *= (float) (1000.0);

        TotalCurrent = (*sp_OP).VARIABLE_PowerRef;
    } else if (sp_OP->OPERATIN == OP_CHARGE_CR || sp_OP->OPERATIN == OP_DISCHARGE_CR) {
        (*sp_OP).VARIABLE_ResistanceRef = (float) (ABS_F((*sp_OP).VARIABLE_VoltageResult) / (*sp_OP).VARIABLE_CurrentRef);

        TotalCurrent = (*sp_OP).VARIABLE_ResistanceRef;
    } else {
        TotalCurrent = (*sp_OP).VARIABLE_CurrentRef;
    }


#if (Current_Spec == 200)
    if (TotalCurrent > 200.0) {
        TotalCurrent = 200.0;
    }
#elif (Current_Spec == 300)
    if (TotalCurrent > 300.0) {
        TotalCurrent = 300.0;
    }
#endif
    sensing_bat_voltage = ABS_F(sp_OP->s_OldADCData.SENSING_BatteryVoltage);

    switch ((*sp_OP).COMMAND)
    {
        case COM_CHARGE:
#if PCB_VERSION == 220
            Value = (OPERATION_GATEOFFSET_VOLT_GAIN_CHA / TotalCurrent) + OPERATION_GATEOFFSET_VOLT_OFFSET_CHA;
#else
            if (TotalCurrent < 12.0) {
                Value = 0.0;
            } else {
                if(TotalCurrent <= 120.0) {
                    gain = 0.005;
                } else {
                    gain = 0.008;
                }

                bat_rate = 1.8;
//                if(TotalCurrent < 50.0) {
//                    bat_rate = 1.0;
//                } else {
//                    bat_rate = 1.8;
//                }

                //If overshoot occurs in a specific current range, reduce the gain(0.008)
                //ex) -> if(TotalCurrent > xxx.x) 0.008 -> 0.007;

                //exception handling before step discharge
                if ((*sp_OP).VARIABLE_COMMAND_OLD == COM_DISCHARGE) {
                    //ToDo need to tuning
                    //gain -= 0.001;
                    //bat_rate -= 0.1;

                }
//                Value = (float) ((300.0 - TotalCurrent) * gain) + (sensing_bat_voltage * bat_rate);
                  Value =  0.0;
            }
#endif
            break;
        case COM_DISCHARGE:
#if PCB_VERSION == 220
            Value = (OPERATION_GATEOFFSET_VOLT_GAIN_DIS / TotalCurrent) + OPERATION_GATEOFFSET_VOLT_OFFSET_DIS;
#else
            if (TotalCurrent < 10.0) {
//                Value = 4.0;
//                Value = 3.0;
                Value = 0.0;
            } else {
                gain = 0.006;

                bat_rate = 1.6;
//                if(TotalCurrent < 50.0) {
////                    bat_rate = 1.5;
//                    bat_rate = 1.0;
//                } else {
//                    bat_rate = 2.0;
//                }

                //If undershoot occurs in a specific current range, reduce the bat_rate or gain(0.006)
                //ex) -> if(TotalCurrent > xxx.x) discharge_bat_rate = 0.3;

                //exception handling before step charge
                if ((*sp_OP).VARIABLE_COMMAND_OLD == COM_CHARGE) {
                    //ToDo need to tuning
                    //gain -= 0.001;
                    //bat_rate -= 0.1;

                }
                //Value = (float) ((300.0 - TotalCurrent) * gain) + ((5.0 - sensing_bat_voltage) * bat_rate);
                //Value = (OPERATION_GATEOFFSET_VOLT_GAIN_DIS / TotalCurrent) + 1;
                Value = 0.0;
            }
#endif
            break;
    }

    (*sp_OP).C_GateOffsetDAC = CONTROL_GateOffsetDAC(Value);

    return (SUCCESS);                    //SUCCESS Or FAIL
}

Uint16 OPEARTION_CurrentControl(S_OPERATION_FILE *sp_OP)
{
    float32 NewCurrent = 0.0;
    static float32 OldCurrent = 0.0;

    Uint32 Temp32 = 0;
    Uint32 FineTemp32 = 0;

    float32 CoarseTotal = 0.0;
    float32 Coarse = 0.0;

    float32 Fine = 0.0;
    float32 FineTotal = 0.0;

    if ((*sp_OP).OPERATIN == OP_CHARGE_CP || (*sp_OP).OPERATIN == OP_DISCHARGE_CP || (*sp_OP).OPERATIN == OP_CHARGE_CPCV || (*sp_OP).OPERATIN == OP_DISCHARGE_CPCV) {
        (*sp_OP).VARIABLE_PowerRef = (float) ((*sp_OP).VARIABLE_CurrentRef / ABS_F((*sp_OP).VARIABLE_VoltageResult));
        //CP kW -> W
        //(*sp_OP).VARIABLE_PowerRef *= (float) (1000.0);

        NewCurrent = (*sp_OP).VARIABLE_PowerRef;
        if(NewCurrent > (*sp_OP).CP_CURRENT_LIMIT && (*sp_OP).CP_CURRENT_LIMIT != 0){
            NewCurrent = (*sp_OP).CP_CURRENT_LIMIT;
        }
    } else if (sp_OP->OPERATIN == OP_CHARGE_CR || sp_OP->OPERATIN == OP_DISCHARGE_CR) {
        (*sp_OP).VARIABLE_ResistanceRef = (float) (ABS_F((*sp_OP).VARIABLE_VoltageResult) / (*sp_OP).VARIABLE_CurrentRef);

        NewCurrent = (*sp_OP).VARIABLE_ResistanceRef;
    } else {
        NewCurrent = (*sp_OP).VARIABLE_CurrentRef;
    }
#if (Current_Spec == 200)
    if (NewCurrent > 200.0) {
        NewCurrent = 200.0;
    }
#elif (Current_Spec == 300)
    if (NewCurrent > 300.0) {
        NewCurrent = 300.0;
    }
#endif


    if (OldCurrent != NewCurrent || (*sp_OP).COMMAND != (*sp_OP).VARIABLE_COMMAND_OLD) {
        if ((*sp_OP).COMMAND == COM_DISCHARGE) {
            Temp32 = (Uint32) (CALIBRATION_ConversionREF(NewCurrent,
                                                         CurrentDischarge_REF,
                                                         CALIBRATION_CURRENT_MIN_VALUE,
                                                         CALIBRATION_CURRENT_UNIT_VALUE,
                                                         CALIBRATION_CURRENT_MAX_ARRAY_LENGTH) * 1000);
        } else {
            Temp32 = (Uint32) (CALIBRATION_ConversionREF(NewCurrent,
                                                         CurrentCharge_REF,
                                                         CALIBRATION_CURRENT_MIN_VALUE,
                                                         CALIBRATION_CURRENT_UNIT_VALUE,
                                                         CALIBRATION_CURRENT_MAX_ARRAY_LENGTH) * 1000);
        }

        if (Temp32 <= OPERATION_CURRENT_FINE_CONTROL_RANGE) {
            FineTotal = (float) Temp32 * (float) (0.001);
            Fine = (FineTotal * (float) (0.5));

            CoarseTotal = 0.0;
            Coarse = 0.0;

            (*sp_OP).C_IRefDAC = CONTROL_CurrentReferenceDAC(CONTROL_ALL, Coarse);
            (*sp_OP).C_FINEIRefDAC = CONTROL_FINECurrentReferenceDAC(CONTROL_ALL, Fine);
        } else {
            Temp32 = Temp32 - OPERATION_CURRENT_FINE_OFFSET;
             FineTemp32 = (((Temp32 >> 1) % OPERATION_CURRENT_MOD ) << 1) + OPERATION_CURRENT_FINE_OFFSET;
             Temp32 = Temp32 - FineTemp32 + OPERATION_CURRENT_FINE_OFFSET;

             FineTotal = FineTemp32 * (float) (0.001);
             Fine = (FineTotal * (float) (0.5));

             CoarseTotal = (float) (Temp32) * (float) (0.001);
             Coarse = (CoarseTotal * (float) (0.5));

             (*sp_OP).C_IRefDAC = CONTROL_CurrentReferenceDAC(CONTROL_ALL, Coarse);
             (*sp_OP).C_FINEIRefDAC = CONTROL_FINECurrentReferenceDAC(CONTROL_ALL, Fine);
        }

        OldCurrent = NewCurrent;
    }

    return (SUCCESS);                    //SUCCESS Or FAIL
}

//
// End of file
//

