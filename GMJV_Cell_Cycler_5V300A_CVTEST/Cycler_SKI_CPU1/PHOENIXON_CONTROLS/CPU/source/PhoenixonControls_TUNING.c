/*
 * PhoenixonControls_TUNING.c
 *
 *  Created on: 2019. 9. 19.
 *      Author: HW Team
 */

//
// Included Files
//
#include "../../../common/include/F28x_Project.h"
#include "../../../common/include/driverlib.h"
#include "../../../common/include/device.h"

extern S_OPERATION_FILE s_Operation;

extern float32 IntegrationChargeSlopGain[OPERATION_INTEGRATION_SLOP_ARRAY_LENGTH ];
extern float32 IntegrationChargeSlopOffset[OPERATION_INTEGRATION_SLOP_ARRAY_LENGTH ];
extern float32 IntegrationDischargeSlopGain[OPERATION_INTEGRATION_SLOP_ARRAY_LENGTH ];
extern float32 IntegrationDischargeSlopOffset[OPERATION_INTEGRATION_SLOP_ARRAY_LENGTH ];

VITuneProperties g_vi_tune;          //Voltage & Current Calibration Properties Value
SlopeTuneProperties g_slope_tune;    //Slope Tuning Properties Value

static Uint16 TuneVADC                  (S_OPERATION_FILE *sp_OP, Uint16 voltage, E_CONTROL_OPERATION_MODE_SELECT mode);
static Uint16 VerifyVADC                (S_OPERATION_FILE *sp_OP, Uint16 voltage, E_CONTROL_OPERATION_MODE_SELECT mode);
static Uint16 TuneIADC                  (S_OPERATION_FILE *sp_OP, float32 current, E_CONTROL_OPERATION_MODE_SELECT mode);
static Uint16 TuneVREF                  (S_OPERATION_FILE *sp_OP, float32 current, E_CONTROL_OPERATION_MODE_SELECT mode, float32 cv_voltage);
static Uint16 TuneLowIADC               (S_OPERATION_FILE *sp_OP, float32 current, E_CONTROL_OPERATION_MODE_SELECT mode);
static Uint16 TuneForceIADC             (S_OPERATION_FILE *sp_OP, float32 current, E_CONTROL_OPERATION_MODE_SELECT mode);
static Uint16 VerifyIADC                (S_OPERATION_FILE *sp_OP, float32 current, E_CONTROL_OPERATION_MODE_SELECT mode);
static Uint16 VerifyLowIADC             (S_OPERATION_FILE *sp_OP, float32 current, E_CONTROL_OPERATION_MODE_SELECT mode);
static void ProcessVADCTune             (void);
static void ProcessVREFTune             (void);
static void ProcessIADCTune             (void);
static void InsertionSort               (float *Data, int Number);
static void ProcessSlopTune             (void);
static Uint16 TuneSlop                  (S_OPERATION_FILE *sp_OP, float32 current, E_CONTROL_OPERATION_MODE_SELECT mode, MarginType tpye);
static Uint16 VerifySlop                (S_OPERATION_FILE *sp_OP, float32 current, float32 resistance, E_CONTROL_OPERATION_MODE_SELECT mode);
static Uint16 VerifyLinearStepSlop      (S_OPERATION_FILE *sp_OP, float32 current, float32 initial_resistance, E_CONTROL_OPERATION_MODE_SELECT mode);
static void ControlInitial              (S_OPERATION_FILE *sp_OP, E_CONTROL_OPERATION_MODE_SELECT mode);
static void ControlGateOffset           (S_OPERATION_FILE *sp_OP, float32 current, E_CONTROL_OPERATION_MODE_SELECT mode);
static void ControlCurrentDacForITune   (S_OPERATION_FILE *sp_OP, float32 current);
static void ControlCurrentDac           (S_OPERATION_FILE *sp_OP, float32 current);
static void ControlVoltageDac           (S_OPERATION_FILE *sp_OP, E_CONTROL_OPERATION_MODE_SELECT mode);

void InitVIADCTuneProperties(void)
{
    Uint16 i;

    g_vi_tune.adc_sum = 0.0;

    for (i = 0; i < 1024; i++)
        g_vi_tune.adc_array[i] = 0.0;

    g_vi_tune.adc_average = 0.0;

    for (i = 0; i < CALIBRATION_VOLATAGE_MAX_ARRAY_LENGTH ; i++) {
        g_vi_tune.verify_charge_v_adc[i] = 0.0;
        g_vi_tune.verify_discharge_v_adc[i] = 0.0;
    }

    for (i = 0; i < CALIBRATION_CURRENT_MAX_ARRAY_LENGTH ; i++) {
        g_vi_tune.verify_charge_i_adc[i] = 0.0;
        g_vi_tune.verify_discharge_i_adc[i] = 0.0;
    }

    for (i = 0; i < CALIBRATION_LOW_CURRENT_MAX_ARRAY_LENGTH ; i++) {
        g_vi_tune.verify_charge_low_i_adc[i] = 0.0;
        g_vi_tune.verify_discharge_low_i_adc[i] = 0.0;
    }

    g_vi_tune.mode = kNone;
    g_vi_tune.command = 100;
    g_vi_tune.start = 0.0;
    g_vi_tune.end = 0.0;
    g_vi_tune.force_coarse_cur = 0.0;
    g_vi_tune.force_fine_cur = 0.0;
    g_vi_tune.force_stop = 0;
    g_vi_tune.force_loop_stop = 0;
    g_vi_tune.index = 0;
    g_vi_tune.slope = AD5293_MAX_F_VLAUE;
    g_vi_tune.slope_gain = 1000.0;
    g_vi_tune.slope_offset = AD5293_MAX_F_VLAUE;
    g_vi_tune.zero_offset = 0.0;
    g_vi_tune.time = 0;
    g_vi_tune.time_end = 200; //100us * 200 = 20ms
    g_vi_tune.time_flag = 0;
    g_vi_tune.loop = 5000;     //Must be greater than 1000
    g_vi_tune.loop_temp = 1000;
    g_vi_tune.loop_count = 0;
    g_vi_tune.result = kOk;
}

void RunVIADCTune(void)
{
    while(1){
        ProcessVADCTune();
        ProcessVREFTune();
        ProcessIADCTune();
    }
}

void ProcessVADCTune(void)
{
    if (g_vi_tune.mode == kVoltage) {
        switch (g_vi_tune.command)
        {
            case 0:  //0V Calibration & Verify
                TuneVADC(&s_Operation, 0, CONTROL_CHARGE);
                VerifyVADC(&s_Operation, 0, CONTROL_CHARGE);
                TuneVADC(&s_Operation, 0, CONTROL_DISCHARGE);
                VerifyVADC(&s_Operation, 0, CONTROL_DISCHARGE);
                g_vi_tune.command = 100;
                break;
            case 1:  //1V Calibration & Verify
                TuneVADC(&s_Operation, 1, CONTROL_CHARGE);
                VerifyVADC(&s_Operation, 1, CONTROL_CHARGE);
                TuneVADC(&s_Operation, 1, CONTROL_DISCHARGE);
                VerifyVADC(&s_Operation, 1, CONTROL_DISCHARGE);
                g_vi_tune.command = 100;
                break;
            case 2:  //2V Calibration & Verify
                TuneVADC(&s_Operation, 2, CONTROL_CHARGE);
                VerifyVADC(&s_Operation, 2, CONTROL_CHARGE);
                TuneVADC(&s_Operation, 2, CONTROL_DISCHARGE);
                VerifyVADC(&s_Operation, 2, CONTROL_DISCHARGE);
                g_vi_tune.command = 100;
                break;
            case 3:  //3V Calibration & Verify
                TuneVADC(&s_Operation, 3, CONTROL_CHARGE);
                VerifyVADC(&s_Operation, 3, CONTROL_CHARGE);
                TuneVADC(&s_Operation, 3, CONTROL_DISCHARGE);
                VerifyVADC(&s_Operation, 3, CONTROL_DISCHARGE);
                g_vi_tune.command = 100;
                break;
            case 4:  //4V Calibration & Verify
                TuneVADC(&s_Operation, 4, CONTROL_CHARGE);
                VerifyVADC(&s_Operation, 4, CONTROL_CHARGE);
                TuneVADC(&s_Operation, 4, CONTROL_DISCHARGE);
                VerifyVADC(&s_Operation, 4, CONTROL_DISCHARGE);
                g_vi_tune.command = 100;
                break;
            case 5:  //5V Calibration & Verify
                TuneVADC(&s_Operation, 5, CONTROL_CHARGE);
                VerifyVADC(&s_Operation, 5, CONTROL_CHARGE);
                TuneVADC(&s_Operation, 5, CONTROL_DISCHARGE);
                VerifyVADC(&s_Operation, 5, CONTROL_DISCHARGE);
                g_vi_tune.command = 100;
                break;
            case 50:    //Alarm reset
                OPERATION_AlramReset(&s_Operation);
                g_vi_tune.command = 100;
                break;
            case 51:    //Calibration value reset
                InitVIADCTuneProperties();
                g_vi_tune.command = 100;
                break;
            default:
                break;
        }
        AD7608_Conversion();
        OPERATION_Sensing(&s_Operation);
        if (OPERATION_Alram(&s_Operation)) {
            OPERATION_Stop(&s_Operation);
            s_Operation.C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
        }
    }
}

void ProcessVREFTune(void)
{
    if (g_vi_tune.mode == kCurrent) {
            switch (g_vi_tune.command)
            {
            //charge
            case 1: //charge 1 value find
                TuneVREF(&s_Operation, g_vi_tune.start, CONTROL_CHARGE, g_vi_tune.cv_voltage);
                g_vi_tune.command = 100;
                break;
            case 2: //charge 1 value check
                //verify code
//                VerifyIADC(&s_Operation, g_vi_tune.start, CONTROL_CHARGE);
                g_vi_tune.command = 100;
                break;
            case 3: //discharge 1 value find
                TuneVREF(&s_Operation, g_vi_tune.start, CONTROL_DISCHARGE, g_vi_tune.cv_voltage);
                g_vi_tune.command = 100;
                break;
            case 4: //discharge 1 value check
//                VerifyIADC(&s_Operation, g_vi_tune.start, CONTROL_DISCHARGE);
                g_vi_tune.command = 100;
                break;
            }
            AD7608_Conversion();
            OPERATION_Sensing(&s_Operation);
            if (OPERATION_Alram(&s_Operation))
            {
                OPERATION_Stop(&s_Operation);
                s_Operation.C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
            }
        }
}

void ProcessIADCTune(void)
{
    float32 i;

    if (g_vi_tune.mode == kCurrent) {
        switch (g_vi_tune.command)
        {
#if 0
            //Low Range
            case 1: //charge 1 low value find
                TuneLowIADC(&s_Operation, g_vi_tune.start, CONTROL_CHARGE);
                g_vi_tune.command = 100;
                break;
            case 2: //charge 1 low value check
                VerifyLowIADC(&s_Operation, g_vi_tune.start, CONTROL_CHARGE);
                g_vi_tune.command = 100;
                break;
            case 3: //discharge 1 low value find
                TuneLowIADC(&s_Operation, g_vi_tune.start, CONTROL_DISCHARGE);
                g_vi_tune.command = 100;
                break;
            case 4: //discharge 1 low value check
                VerifyLowIADC(&s_Operation, g_vi_tune.start, CONTROL_DISCHARGE);
                g_vi_tune.command = 100;
                break;
#endif
            //Original Range
            case 10: //charge 1 value find
                TuneIADC(&s_Operation, g_vi_tune.start, CONTROL_CHARGE);
                g_vi_tune.command = 100;
                break;
            case 11: //charge 1 value check
                VerifyIADC(&s_Operation, g_vi_tune.start, CONTROL_CHARGE);
                g_vi_tune.command = 100;
                break;
            case 12: //discharge 1 value find
                TuneIADC(&s_Operation, g_vi_tune.start, CONTROL_DISCHARGE);
                g_vi_tune.command = 100;
                break;
            case 13: //discharge 1 value check
                VerifyIADC(&s_Operation, g_vi_tune.start, CONTROL_DISCHARGE);
                g_vi_tune.command = 100;
                break;
            //For Test
            case 20:
                TuneForceIADC(&s_Operation, g_vi_tune.start, CONTROL_CHARGE);
                g_vi_tune.command = 100;
                break;
            case 21:
                TuneForceIADC(&s_Operation, g_vi_tune.start, CONTROL_DISCHARGE);
                g_vi_tune.command = 100;
                break;
#if 0
            //Low Range find loop
            case 30:
                for (i = g_vi_tune.start; i <= g_vi_tune.end; i += 1.0) {
                    TuneLowIADC(&s_Operation, i, CONTROL_CHARGE);
                    DELAY_US(100000);
                    TuneLowIADC(&s_Operation, i, CONTROL_DISCHARGE);
                    DELAY_US(100000);
                    AD7608_Conversion();
                    OPERATION_Sensing(&s_Operation);
                    if (OPERATION_Alram(&s_Operation) || g_vi_tune.force_loop_stop == 1) {
                        g_vi_tune.force_loop_stop = 0;
                        OPERATION_Stop(&s_Operation);
                        s_Operation.C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
                        break;
                    }
                }
                g_vi_tune.command = 100;
                break;
            //Low Range verify loop
            case 31:
                 for (i = g_vi_tune.start; i <= g_vi_tune.end; i += 1.0) {
                     VerifyLowIADC(&s_Operation, i, CONTROL_CHARGE);
                     DELAY_US(100000);
                     VerifyLowIADC(&s_Operation, i, CONTROL_DISCHARGE);
                     DELAY_US(100000);
                     AD7608_Conversion();
                     OPERATION_Sensing(&s_Operation);
                     if (OPERATION_Alram(&s_Operation) || g_vi_tune.force_loop_stop == 1) {
                         g_vi_tune.force_loop_stop = 0;
                         OPERATION_Stop(&s_Operation);
                         s_Operation.C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
                         break;
                     }
                 }
                 g_vi_tune.command = 100;
                 break;
#endif
            //Range loop
            case 40:
                for (i = g_vi_tune.start; i <= g_vi_tune.end; i += 10.0) {
                    TuneIADC(&s_Operation, i, CONTROL_CHARGE);
                    DELAY_US(100000);
                    TuneIADC(&s_Operation, i, CONTROL_DISCHARGE);
                    DELAY_US(100000);
                    AD7608_Conversion();
                    OPERATION_Sensing(&s_Operation);
                    if (OPERATION_Alram(&s_Operation) || g_vi_tune.force_loop_stop == 1) {
                        g_vi_tune.force_loop_stop = 0;
                        OPERATION_Stop(&s_Operation);
                        s_Operation.C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
                        break;
                    }
                }
                g_vi_tune.command = 100;
                break;
            //Range verify loop
            case 41:
                for (i = g_vi_tune.start; i <= g_vi_tune.end; i += 10.0) {
                    VerifyIADC(&s_Operation, i, CONTROL_CHARGE);
                    DELAY_US(100000);
                    VerifyIADC(&s_Operation, i, CONTROL_DISCHARGE);
                    DELAY_US(100000);
                    AD7608_Conversion();
                    OPERATION_Sensing(&s_Operation);
                    if (OPERATION_Alram(&s_Operation) || g_vi_tune.force_loop_stop == 1) {
                        g_vi_tune.force_loop_stop = 0;
                        OPERATION_Stop(&s_Operation);
                        s_Operation.C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
                        break;
                    }
                }
                g_vi_tune.command = 100;
                break;
                //Range loop
            case 45:
                for (i = g_vi_tune.start; i <= g_vi_tune.end; i += 50.0) {
                    TuneIADC(&s_Operation, i, CONTROL_CHARGE);
                    DELAY_US(100000);
                    TuneIADC(&s_Operation, i, CONTROL_DISCHARGE);
                    DELAY_US(100000);
                    AD7608_Conversion();
                    OPERATION_Sensing(&s_Operation);
                    if (OPERATION_Alram(&s_Operation) || g_vi_tune.force_loop_stop == 1) {
                        g_vi_tune.force_loop_stop = 0;
                        OPERATION_Stop(&s_Operation);
                        s_Operation.C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
                        break;
                    }
                }
                g_vi_tune.command = 100;
                break;
            case 50:    //Alarm reset
                OPERATION_AlramReset (&s_Operation);
                g_vi_tune.command = 100;
                break;
            case 51:    //Calibration value reset
                InitVIADCTuneProperties();
                g_vi_tune.command = 100;
                break;
            default:
                break;
        }
        AD7608_Conversion();
        OPERATION_Sensing(&s_Operation);
        if (OPERATION_Alram(&s_Operation)) {
            OPERATION_Stop(&s_Operation);
            s_Operation.C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
        }
    }
}

Uint16 TuneVADC(S_OPERATION_FILE *sp_OP, Uint16 voltage, E_CONTROL_OPERATION_MODE_SELECT mode)
{
    Uint32 i, j = 0;

    OPERATION_Stop(sp_OP);
    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);

    if (voltage > 5) return g_vi_tune.result = kWrongParam;
    if (mode != CONTROL_CHARGE && mode != CONTROL_DISCHARGE) return g_vi_tune.result = kWrongParam;

    g_vi_tune.loop_temp = g_vi_tune.loop / 1000;
    if (g_vi_tune.loop / g_vi_tune.loop_temp < 1000) {
        return g_vi_tune.result = kWrongParam;
    }

    if (mode == CONTROL_CHARGE) {
        sp_OP->C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_CHARGE);
    } else {
        sp_OP->C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_DISCHARGE);
    }

    for (i = 0; i < g_vi_tune.loop; i++) {

        g_vi_tune.loop_count = i;

        AD7608_Conversion();
        OPERATION_Sensing(sp_OP);

        if (OPERATION_Alram(sp_OP)|| g_vi_tune.force_stop == 1) {
            g_vi_tune.force_stop = 0;
            OPERATION_Stop(sp_OP);
            sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
            return g_vi_tune.result = kOperationFail;
        }

        if ((i % g_vi_tune.loop_temp) == 0) {
            g_vi_tune.adc_array[j++] = ABS_F(sp_OP->s_ADCData.SENSING_BatteryVoltage);
        }
    }

    OPERATION_Stop(sp_OP);
    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);

    InsertionSort(g_vi_tune.adc_array, 999);

    g_vi_tune.adc_sum = 0.0;
    j = 0;

    for (i = 350; i < 750; i++, j++) {
        g_vi_tune.adc_sum += g_vi_tune.adc_array[i];
    }

    g_vi_tune.adc_average = g_vi_tune.adc_sum / (float) (j);

    if (mode == CONTROL_CHARGE) {
        VoltageCharge_ADC[voltage] = g_vi_tune.adc_average;
    } else {
        VoltageDischarge_ADC[voltage] = g_vi_tune.adc_average;
    }

    return g_vi_tune.result = kOk;
}

Uint16 VerifyVADC(S_OPERATION_FILE *sp_OP, Uint16 voltage, E_CONTROL_OPERATION_MODE_SELECT mode)
{
    Uint32 i, j = 0;

    OPERATION_Stop(sp_OP);
    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);

    if (voltage > 5) return g_vi_tune.result = kWrongParam;
    if (mode != CONTROL_CHARGE && mode != CONTROL_DISCHARGE) return g_vi_tune.result = kWrongParam;

    g_vi_tune.loop_temp = g_vi_tune.loop / 1000;
    if (g_vi_tune.loop / g_vi_tune.loop_temp < 1000) {
        return g_vi_tune.result = kWrongParam;
    }

    if(mode == CONTROL_CHARGE) {
        sp_OP->COMMAND = COM_CHARGE;
        sp_OP->C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_CHARGE);
    } else {
        sp_OP->COMMAND = COM_DISCHARGE;
        sp_OP->C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_DISCHARGE);
    }

    for (i = 0; i < g_vi_tune.loop; i++) {

        g_vi_tune.loop_count = i;

        AD7608_Conversion();
        OPERATION_Sensing(sp_OP);

        if (OPERATION_Alram(sp_OP) || g_vi_tune.force_stop == 1) {
            g_vi_tune.force_stop = 0;
            OPERATION_Stop(sp_OP);
            sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
            sp_OP->COMMAND = COM_NONE;
            return g_vi_tune.result = kOperationFail;
        }

        if ((i % g_vi_tune.loop) == 0) {
            g_vi_tune.adc_array[j++] = ABS_F(sp_OP->s_OldADCData.SENSING_BatteryVoltage);
        }
    }

    OPERATION_Stop(sp_OP);
    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
    sp_OP->COMMAND = COM_NONE;

    InsertionSort(g_vi_tune.adc_array, 999);

    g_vi_tune.adc_sum = 0.0;
    j = 0;

    for (i = 350; i < 750; i++, j++) {
        g_vi_tune.adc_sum += g_vi_tune.adc_array[i];
    }

    g_vi_tune.adc_average = g_vi_tune.adc_sum / (float) (j);

    if (mode == CONTROL_CHARGE) {
        g_vi_tune.verify_charge_v_adc[voltage] = g_vi_tune.adc_average;
    } else {
        g_vi_tune.verify_discharge_v_adc[voltage] = g_vi_tune.adc_average;
    }

    return g_vi_tune.result = kOk;
}

Uint16 TuneVREF(S_OPERATION_FILE *sp_OP, float32 current, E_CONTROL_OPERATION_MODE_SELECT mode, float32 cv_voltage)
{
    Uint32 i, j = 0;
    float32 v_offset = 0.0;

    OPERATION_Stop(sp_OP);
    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);

    if (current > 300.0) return g_vi_tune.result = kWrongParam;
    if(abs(g_vi_tune.cv_voltage - abs(sp_OP->s_OldADCData.SENSING_BatteryVoltage)) > 1) return g_vi_tune.result = kWrongParam;

    if (mode != CONTROL_CHARGE && mode != CONTROL_DISCHARGE) return g_vi_tune.result = kWrongParam;

    g_vi_tune.loop_temp = g_vi_tune.loop / 1000;

    if (g_vi_tune.loop / g_vi_tune.loop_temp < 1000) {
        return g_vi_tune.result = kWrongParam;
    }

    // cv_voltage ?? ?????????? ??????/?????? ???? ???? ?????? ?????? kWrongParam
    if (mode == CONTROL_CHARGE) {
        if(g_vi_tune.cv_voltage < abs(sp_OP->s_OldADCData.SENSING_BatteryVoltage)) return g_vi_tune.result = kWrongParam;
        if(g_vi_tune.cv_voltage > 4.5 ) return g_vi_tune.result = kWrongParam;


        sp_OP->COMMAND = COM_CHARGE;
        sp_OP->C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_CHARGE);
    } else {
        if(g_vi_tune.cv_voltage > abs(sp_OP->s_OldADCData.SENSING_BatteryVoltage)) return g_vi_tune.result = kWrongParam;
        if(g_vi_tune.cv_voltage < 2.7 ) return g_vi_tune.result = kWrongParam;

        sp_OP->COMMAND = COM_DISCHARGE;
        sp_OP->C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_DISCHARGE);
    }

    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_ON);

    //slope initial
    if (current < 10.0) {
        g_vi_tune.index = (Uint16) (current);
    } else {
        g_vi_tune.index = (Uint16) (current / 10);
        g_vi_tune.index += 9;
    }

    if (mode == CONTROL_CHARGE) {
        g_vi_tune.slope = IntegrationChargeSlopOffset[g_vi_tune.index];
        g_vi_tune.slope_gain = IntegrationChargeSlopGain[g_vi_tune.index];
        g_vi_tune.slope_offset = IntegrationChargeSlopOffset[g_vi_tune.index];

    } else {
        g_vi_tune.slope = IntegrationDischargeSlopOffset[g_vi_tune.index];
        g_vi_tune.slope_gain = IntegrationDischargeSlopGain[g_vi_tune.index];
        g_vi_tune.slope_offset = IntegrationDischargeSlopOffset[g_vi_tune.index];
    }

    ControlInitial(sp_OP, mode);                                            //1. Initial
#if 0
    sp_OP->C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, g_vi_tune.slope);  //2. Slope change
#else
    g_vi_tune.slope = AD5293_MAX_F_VLAUE;
    sp_OP->C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, g_vi_tune.slope);  //2. Slope change
#endif
    ControlGateOffset(sp_OP, current, mode);                                //3. Gate Offset
    ControlCurrentDac(sp_OP, current);                                      //4. Current DAC ON

    // voltage DAC control
    if (mode == CONTROL_CHARGE) {
         sp_OP->C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_CHARGE);
         sp_OP->C_VRefDAC = CONTROL_VoltageReferenceDAC(g_vi_tune.cv_voltage);
         sp_OP->C_GateOutputAMP = CONTROL_GateOutputAMP(CONTROL_ALL, CONTROL_CHARGE);
         sp_OP->C_GateOutputMUX = CONTROL_GateOutputMUX(CONTROL_ALL, CONTROL_CHARGE);
         sp_OP->C_IntSlopMUX = CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_PM);

     } else {
         sp_OP->C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_DISCHARGE);
         sp_OP->C_VRefDAC = CONTROL_VoltageReferenceDAC((g_vi_tune.cv_voltage * (float) (-1.0)));
         sp_OP->C_GateOutputAMP = CONTROL_GateOutputAMP(CONTROL_ALL, CONTROL_DISCHARGE);
         sp_OP->C_GateOutputMUX = CONTROL_GateOutputMUX(CONTROL_ALL, CONTROL_DISCHARGE);
         sp_OP->C_IntSlopMUX = CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_PM);
     }


#if 0
    g_vi_tune.time_end = 200;
    g_vi_tune.time_flag = 1;

    //wait 20ms, Timer 1 start
    while(g_vi_tune.time_flag == 1) {
        AD7608_Conversion();
        OPERATION_Sensing(sp_OP);
        if (OPERATION_Alram(sp_OP) || g_vi_tune.force_stop == 1) {
            OPERATION_Stop(sp_OP);
            sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
            g_vi_tune.force_stop = 0;
            g_vi_tune.time_flag = 0;
            return g_vi_tune.result = kOperationFail;
        }
    }
#else
    while (1) {
        AD7608_Conversion();
        OPERATION_Sensing(sp_OP);
        if (OPERATION_Alram(sp_OP) || g_vi_tune.force_stop == 1) {
            g_vi_tune.force_stop = 0;
            OPERATION_Stop(sp_OP);
            sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
            return g_vi_tune.result = kOperationFail;
        }

        if( mode == CONTROL_CHARGE )
        {
            if(ABS_F(sp_OP->s_OldADCData.SENSING_BatteryVoltage) > g_vi_tune.cv_voltage - OPERATION_VOLTAGE_TOLERANCE)
            {
                if(ABS_F(sp_OP->s_ADCData.SENSING_Current) < (current * 0.9))
                {
                    break;
                }
            }
        }
        else
        {
            if(ABS_F(sp_OP->s_OldADCData.SENSING_BatteryVoltage) < g_vi_tune.cv_voltage + OPERATION_VOLTAGE_TOLERANCE)
            {
                if(ABS_F(sp_OP->s_ADCData.SENSING_Current) < (current * 0.9))
                {
                    break;
                }
            }
        }
    }
    DELAY_US(10000);
#endif

    for (i = 0; i < g_vi_tune.loop; i++) {

        g_vi_tune.loop_count = i;

        AD7608_Conversion();
        OPERATION_Sensing(sp_OP);

        if (OPERATION_Alram(sp_OP) || g_vi_tune.force_stop == 1) {
            g_vi_tune.force_stop = 0;
            OPERATION_Stop(sp_OP);
            sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
            return g_vi_tune.result = kOperationFail;
        }

        if ((i % g_vi_tune.loop_temp) == 0) {
            g_vi_tune.adc_array[j++] = ABS_F(sp_OP->s_OldADCData.SENSING_BatteryVoltage);
        }
    }

    OPERATION_Stop(sp_OP);
    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);

    InsertionSort(g_vi_tune.adc_array, 999);

    g_vi_tune.adc_sum = 0.0;
    j = 0;

    for (i = 350; i < 750; i++, j++) {
        g_vi_tune.adc_sum += g_vi_tune.adc_array[i];
    }

    g_vi_tune.adc_average = g_vi_tune.adc_sum / (float) (j);


    // save Voltage_ref
    if (mode == CONTROL_CHARGE) {
        g_vi_tune.charge_v_ref = g_vi_tune.adc_average;

        v_offset = g_vi_tune.cv_voltage - g_vi_tune.adc_average;

        for( i = 1; i< CALIBRATION_VOLATAGE_MAX_ARRAY_LENGTH; i++)
        {
            VoltageCharge_REF[i] = i - v_offset;
        }


    } else {
        g_vi_tune.discharge_v_ref = g_vi_tune.adc_average;

        v_offset = g_vi_tune.cv_voltage - g_vi_tune.adc_average;

        for( i = 1; i< CALIBRATION_VOLATAGE_MAX_ARRAY_LENGTH; i++)
        {
            VoltageDischarge_REF[i] = i - v_offset;
        }

    }



    return g_vi_tune.result = kOk;
}

Uint16 TuneIADC(S_OPERATION_FILE *sp_OP, float32 current, E_CONTROL_OPERATION_MODE_SELECT mode)
{
    Uint32 i, j = 0;
    Uint16 index;

    OPERATION_Stop(sp_OP);
    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);

    if (current > 300.0) return g_vi_tune.result = kWrongParam;
    if (mode != CONTROL_CHARGE && mode != CONTROL_DISCHARGE) return g_vi_tune.result = kWrongParam;

    g_vi_tune.loop_temp = g_vi_tune.loop / 1000;
    if (g_vi_tune.loop / g_vi_tune.loop_temp < 1000) {
        return g_vi_tune.result = kWrongParam;
    }

    if (mode == CONTROL_CHARGE) {
        sp_OP->C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_CHARGE);
    } else {
        sp_OP->C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_DISCHARGE);
    }

    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_ON);

    //slope initial
    if (current < 10.0) {
        g_vi_tune.index = (Uint16) (current);
    } else {
        g_vi_tune.index = (Uint16) (current / 10);
        g_vi_tune.index += 9;
    }

    if (mode == CONTROL_CHARGE) {
        g_vi_tune.slope = IntegrationChargeSlopOffset[g_vi_tune.index];
        g_vi_tune.slope_gain = IntegrationChargeSlopGain[g_vi_tune.index];
        g_vi_tune.slope_offset = IntegrationChargeSlopOffset[g_vi_tune.index];

    } else {
        g_vi_tune.slope = IntegrationDischargeSlopOffset[g_vi_tune.index];
        g_vi_tune.slope_gain = IntegrationDischargeSlopGain[g_vi_tune.index];
        g_vi_tune.slope_offset = IntegrationDischargeSlopOffset[g_vi_tune.index];
    }

    ControlInitial(sp_OP, mode);                                            //1. Initial
#if 0
    sp_OP->C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, g_vi_tune.slope);  //2. Slope change
#else
    g_vi_tune.slope = AD5293_MAX_F_VLAUE;
    sp_OP->C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, g_vi_tune.slope);  //2. Slope change
#endif
    ControlGateOffset(sp_OP, current, mode);                                //3. Gate Offset
    ControlCurrentDac(sp_OP, current);                                      //4. Current DAC ON
    ControlVoltageDac(sp_OP, mode);                                         //5. Voltage DAC ON

#if 0
    g_vi_tune.time_end = 200;
    g_vi_tune.time_flag = 1;

    //wait 20ms, Timer 1 start
    while(g_vi_tune.time_flag == 1) {
        AD7608_Conversion();
        OPERATION_Sensing(sp_OP);
        if (OPERATION_Alram(sp_OP) || g_vi_tune.force_stop == 1) {
            OPERATION_Stop(sp_OP);
            sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
            g_vi_tune.force_stop = 0;
            g_vi_tune.time_flag = 0;
            return g_vi_tune.result = kOperationFail;
        }
    }
#else
    while (1) {
        AD7608_Conversion();
        OPERATION_Sensing(sp_OP);
        if (OPERATION_Alram(sp_OP) || g_vi_tune.force_stop == 1) {
            g_vi_tune.force_stop = 0;
            OPERATION_Stop(sp_OP);
            sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
            return g_vi_tune.result = kOperationFail;
        }

        if(ABS_F(sp_OP->s_ADCData.SENSING_Current) > (current * 0.9)) {
            break;
        }
    }
    DELAY_US(10000);
#endif

    for (i = 0; i < g_vi_tune.loop; i++) {

        g_vi_tune.loop_count = i;

        AD7608_Conversion();
        OPERATION_Sensing(sp_OP);

        if (OPERATION_Alram(sp_OP) || g_vi_tune.force_stop == 1) {
            g_vi_tune.force_stop = 0;
            OPERATION_Stop(sp_OP);
            sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
            return g_vi_tune.result = kOperationFail;
        }

        if ((i % g_vi_tune.loop_temp) == 0) {
            g_vi_tune.adc_array[j++] = ABS_F(sp_OP->s_ADCData.SENSING_Current);
        }
    }

    OPERATION_Stop(sp_OP);
    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);

    InsertionSort(g_vi_tune.adc_array, 999);

    g_vi_tune.adc_sum = 0.0;
    j = 0;

    for (i = 350; i < 750; i++, j++) {
        g_vi_tune.adc_sum += g_vi_tune.adc_array[i];
    }

    g_vi_tune.adc_average = g_vi_tune.adc_sum / (float) (j);


    if(g_vi_tune.command == 45){
           g_vi_tune.index = (Uint16) (current / 50);
    }else{
        index = (Uint16)(current / 10);
    }

    if (mode == CONTROL_CHARGE) {
        CurrentCharge_ADC[index] = g_vi_tune.adc_average;
    } else {
        CurrentDischarge_ADC[index] = g_vi_tune.adc_average;
    }

    return g_vi_tune.result = kOk;
}

Uint16 TuneLowIADC(S_OPERATION_FILE *sp_OP, float32 current, E_CONTROL_OPERATION_MODE_SELECT mode)
{
    Uint32 i, j = 0;
    Uint16 index;

    OPERATION_Stop(sp_OP);
    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);

    if (current > 11.0) return g_vi_tune.result = kWrongParam;
    if (mode != CONTROL_CHARGE && mode != CONTROL_DISCHARGE) return g_vi_tune.result = kWrongParam;

    g_vi_tune.loop_temp = g_vi_tune.loop / 1000;
    if (g_vi_tune.loop / g_vi_tune.loop_temp < 1000) {
        return g_vi_tune.result = kWrongParam;
    }

    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_ON);

    //slope initial
    if (current < 10.0) {
        g_vi_tune.index = (Uint16) (current);
    } else {
        g_vi_tune.index = (Uint16) (current / 10);
        g_vi_tune.index += 9;
    }

    if (mode == CONTROL_CHARGE) {
        g_vi_tune.slope = IntegrationChargeSlopOffset[g_vi_tune.index];
        g_vi_tune.slope_gain = IntegrationChargeSlopGain[g_vi_tune.index];
        g_vi_tune.slope_offset = IntegrationChargeSlopOffset[g_vi_tune.index];

    } else {
        g_vi_tune.slope = IntegrationDischargeSlopOffset[g_vi_tune.index];
        g_vi_tune.slope_gain = IntegrationDischargeSlopGain[g_vi_tune.index];
        g_vi_tune.slope_offset = IntegrationDischargeSlopOffset[g_vi_tune.index];
    }

    ControlInitial(sp_OP, mode);                                            //1. Initial
    sp_OP->C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, g_vi_tune.slope);  //2. Slope change
    ControlGateOffset(sp_OP, current, mode);                                //3. Gate Offset
    ControlCurrentDac(sp_OP, current);                                      //4. Current DAC ON
    ControlVoltageDac(sp_OP, mode);                                         //5. Voltage DAC ON

    g_vi_tune.time_end = 200;
    g_vi_tune.time_flag = 1;

    //wait 20ms, Timer 1 start
    while(g_vi_tune.time_flag == 1) {
        AD7608_Conversion();
        OPERATION_Sensing(sp_OP);
        if (OPERATION_Alram(sp_OP) || g_vi_tune.force_stop == 1) {
            OPERATION_Stop(sp_OP);
            sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
            g_vi_tune.force_stop = 0;
            g_vi_tune.time_flag = 0;
            return g_vi_tune.result = kOperationFail;
        }
    }

    for (i = 0; i < g_vi_tune.loop; i++) {

        g_vi_tune.loop_count = i;
        AD7608_Conversion();
        OPERATION_Sensing(sp_OP);

        if (OPERATION_Alram(sp_OP) || g_vi_tune.force_stop == 1) {
            OPERATION_Stop(sp_OP);
            sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
            g_vi_tune.force_stop = 0;
            return g_vi_tune.result = kOperationFail;
        }

        if ((i % g_vi_tune.loop_temp) == 0) {
            g_vi_tune.adc_array[j++] = ABS_F(sp_OP->s_ADCData.SENSING_Current);
        }
    }

    OPERATION_Stop(sp_OP);
    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);

    InsertionSort(g_vi_tune.adc_array, 999);

    g_vi_tune.adc_sum = 0.0;
    j = 0;

    for (i = 350; i < 750; i++, j++) {
        g_vi_tune.adc_sum += g_vi_tune.adc_array[i];
    }

    g_vi_tune.adc_average = g_vi_tune.adc_sum / (float) (j);

    index = (Uint16)(current);

    if (mode == CONTROL_CHARGE) {
        //CurrentCharge_Low_ADC[index] = g_vi_tune.adc_average;
    } else {
        //CurrentDischarge_Low_ADC[index] = g_vi_tune.adc_average;
    }

    return g_vi_tune.result = kOk;
}

Uint16 TuneForceIADC(S_OPERATION_FILE *sp_OP, float32 current, E_CONTROL_OPERATION_MODE_SELECT mode)
{
    OPERATION_Stop(sp_OP);
    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);

    if (current > 300.0) return g_vi_tune.result = kWrongParam;
    if (mode != CONTROL_CHARGE && mode != CONTROL_DISCHARGE) return g_vi_tune.result = kWrongParam;

    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_ON);

    ControlInitial(sp_OP, mode);                                            //1. Initial
    //sp_OP->C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(g_vi_tune.slope);  //2. Slope change
    ControlGateOffset(sp_OP, current, mode);                                //3. Gate Offset
    //ControlCurrentDac(sp_OP, current);                                    //4. Current DAC ON
    ControlVoltageDac(sp_OP, mode);                                         //5. Voltage DAC ON

    g_vi_tune.force_coarse_cur = 0.0;
    g_vi_tune.force_fine_cur = 0.0;

    while (1) {
        AD7608_Conversion();
        OPERATION_Sensing(sp_OP);

        if (OPERATION_Alram(sp_OP) || g_vi_tune.force_stop == 1) {
            OPERATION_Stop(sp_OP);
            sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
            g_vi_tune.force_stop = 0;
            return g_vi_tune.result = kOperationFail;
        }
        sp_OP->C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, g_vi_tune.slope);
        sp_OP->C_IRefDAC = CONTROL_CurrentReferenceDAC(CONTROL_ALL, g_vi_tune.force_coarse_cur);
        sp_OP->C_FINEIRefDAC = CONTROL_FINECurrentReferenceDAC(CONTROL_ALL, g_vi_tune.force_fine_cur);
    }
}

Uint16 VerifyIADC(S_OPERATION_FILE *sp_OP, float32 current, E_CONTROL_OPERATION_MODE_SELECT mode)
{
    Uint32 i, j = 0;
    Uint32 Temp32;
    Uint16 index;

    OPERATION_Stop(sp_OP);
    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);

    if (current > 300.0) return g_vi_tune.result = kWrongParam;
    if (mode != CONTROL_CHARGE && mode != CONTROL_DISCHARGE) return g_vi_tune.result = kWrongParam;

    g_vi_tune.loop_temp = g_vi_tune.loop / 1000;
    if (g_vi_tune.loop / g_vi_tune.loop_temp < 1000) {
        return g_vi_tune.result = kWrongParam;
    }

    if (mode == CONTROL_CHARGE) {
        sp_OP->COMMAND = COM_CHARGE;
    } else {
        sp_OP->COMMAND = COM_DISCHARGE;
    }

    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_ON);

    //slope initial
    if (current < 10.0) {
        g_vi_tune.index = (Uint16) (current);
    } else {
        g_vi_tune.index = (Uint16) (current / 10);
        g_vi_tune.index += 9;
    }

    if (mode == CONTROL_CHARGE) {
        g_vi_tune.slope = IntegrationChargeSlopOffset[g_vi_tune.index];
        g_vi_tune.slope_gain = IntegrationChargeSlopGain[g_vi_tune.index];
        g_vi_tune.slope_offset = IntegrationChargeSlopOffset[g_vi_tune.index];

    } else {
        g_vi_tune.slope = IntegrationDischargeSlopOffset[g_vi_tune.index];
        g_vi_tune.slope_gain = IntegrationDischargeSlopGain[g_vi_tune.index];
        g_vi_tune.slope_offset = IntegrationDischargeSlopOffset[g_vi_tune.index];
    }

    ControlInitial(sp_OP, mode);                                            //1. Initial
#if 0
    sp_OP->C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, g_vi_tune.slope);  //2. Slope change
#else
    g_vi_tune.slope = AD5293_MAX_F_VLAUE;
    sp_OP->C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, g_vi_tune.slope);  //2. Slope change
#endif
    ControlGateOffset(sp_OP, current, mode);                                //3. Gate Offset

    if (mode == CONTROL_CHARGE) {
        Temp32 = (Uint32) (CALIBRATION_ConversionREF(current,
                                                     CurrentCharge_REF,
                                                     CALIBRATION_CURRENT_MIN_VALUE,
                                                     CALIBRATION_CURRENT_UNIT_VALUE,
                                                     CALIBRATION_CURRENT_MAX_ARRAY_LENGTH) * 1000);
    } else {
        Temp32 = (Uint32) (CALIBRATION_ConversionREF(current,
                                                     CurrentDischarge_REF,
                                                     CALIBRATION_CURRENT_MIN_VALUE,
                                                     CALIBRATION_CURRENT_UNIT_VALUE,
                                                     CALIBRATION_CURRENT_MAX_ARRAY_LENGTH) * 1000);
    }

    ControlCurrentDacForITune(sp_OP, Temp32);                               //4. Current DAC ON
    ControlVoltageDac(sp_OP, mode);                                         //5. Voltage DAC ON

#if 0
    g_vi_tune.time_end = 200;
    g_vi_tune.time_flag = 1;

    //wait 20ms, Timer 1 start
    while (g_vi_tune.time_flag == 1) {
        AD7608_Conversion();
        OPERATION_Sensing(sp_OP);

        if (OPERATION_Alram(sp_OP) || g_vi_tune.force_stop == 1) {
            OPERATION_Stop(sp_OP);
            sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
            sp_OP->COMMAND = COM_NONE;
            g_vi_tune.force_stop = 0;
            g_vi_tune.time_flag = 0;
            return g_vi_tune.result = kOperationFail;
        }
    }
#else
    while (1) {
        AD7608_Conversion();
        OPERATION_Sensing(sp_OP);

        if (OPERATION_Alram(sp_OP) || g_vi_tune.force_stop == 1) {
            g_vi_tune.force_stop = 0;
            OPERATION_Stop(sp_OP);
            sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
            sp_OP->COMMAND = COM_NONE;
            return g_vi_tune.result = kOperationFail;
        }

        if (ABS_F(sp_OP->s_ADCData.SENSING_Current) > (current * 0.9)) {
            break;
        }
    }
    DELAY_US(10000);
#endif

    for (i = 0; i < g_vi_tune.loop; i++) {

        g_vi_tune.loop_count = i;

        AD7608_Conversion();
        OPERATION_Sensing(sp_OP);

        if (OPERATION_Alram(sp_OP) || g_vi_tune.force_stop == 1) {
            OPERATION_Stop(sp_OP);
            sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
            sp_OP->COMMAND = COM_NONE;
            g_vi_tune.force_stop = 0;
            return g_vi_tune.result = kOperationFail;
        }

        if ((i % g_vi_tune.loop_temp) == 0) {
            g_vi_tune.adc_array[j++] = ABS_F(sp_OP->s_OldADCData.SENSING_Current);
        }
    }

    OPERATION_Stop(sp_OP);
    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
    sp_OP->COMMAND = COM_NONE;

    InsertionSort(g_vi_tune.adc_array, 999);

    g_vi_tune.adc_sum = 0.0;
    j = 0;

    for (i = 350; i < 750; i++, j++) {
        g_vi_tune.adc_sum += g_vi_tune.adc_array[i];
    }

    g_vi_tune.adc_average = g_vi_tune.adc_sum / (float) (j);

    index = (Uint16)(current / 10);

    if (mode == CONTROL_CHARGE) {
        g_vi_tune.verify_charge_i_adc[index] = g_vi_tune.adc_average;
    } else {
        g_vi_tune.verify_discharge_i_adc[index] = g_vi_tune.adc_average;
    }

    return g_vi_tune.result = kOk;
}

Uint16 VerifyLowIADC(S_OPERATION_FILE *sp_OP, float32 current, E_CONTROL_OPERATION_MODE_SELECT mode)
{
    Uint32 i, j = 0;
    Uint32 Temp32;
    Uint16 index;

    OPERATION_Stop(sp_OP);
    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);

    if (current > 11.0) return g_vi_tune.result = kWrongParam;
    if (mode != CONTROL_CHARGE && mode != CONTROL_DISCHARGE) return g_vi_tune.result = kWrongParam;

    g_vi_tune.loop_temp = g_vi_tune.loop / 1000;
    if (g_vi_tune.loop / g_vi_tune.loop_temp < 1000) {
        return g_vi_tune.result = kWrongParam;
    }

    if (mode == CONTROL_CHARGE) {
        sp_OP->COMMAND = COM_CHARGE;
    } else {
        sp_OP->COMMAND = COM_DISCHARGE;
    }

    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_ON);

    //slope initial
    if (current < 10.0) {
        g_vi_tune.index = (Uint16) (current);
    } else {
        g_vi_tune.index = (Uint16) (current / 10);
        g_vi_tune.index += 9;
    }

    if (mode == CONTROL_CHARGE) {
        g_vi_tune.slope = IntegrationChargeSlopOffset[g_vi_tune.index];
        g_vi_tune.slope_gain = IntegrationChargeSlopGain[g_vi_tune.index];
        g_vi_tune.slope_offset = IntegrationChargeSlopOffset[g_vi_tune.index];

    } else {
        g_vi_tune.slope = IntegrationDischargeSlopOffset[g_vi_tune.index];
        g_vi_tune.slope_gain = IntegrationDischargeSlopGain[g_vi_tune.index];
        g_vi_tune.slope_offset = IntegrationDischargeSlopOffset[g_vi_tune.index];
    }

    ControlInitial(sp_OP, mode);                                            //1. Initial
    sp_OP->C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, g_vi_tune.slope);  //2. Slope change
    ControlGateOffset(sp_OP, current, mode);                                //3. Gate Offset

    if (mode == CONTROL_CHARGE) {
//        Temp32 = (Uint32) (CALIBRATION_ConversionREF(current,
//                                                     CurrentCharge_Low_REF,
//                                                     CALIBRATION_LOW_CURRENT_MIN_VALUE,
//                                                     CALIBRATION_LOW_CURRENT_UNIT_VALUE,
//                                                     CALIBRATION_LOW_CURRENT_MAX_ARRAY_LENGTH) * 1000);
    } else {
//        Temp32 = (Uint32) (CALIBRATION_ConversionREF(current,
//                                                     CurrentDischarge_Low_REF,
//                                                     CALIBRATION_LOW_CURRENT_MIN_VALUE,
//                                                     CALIBRATION_LOW_CURRENT_UNIT_VALUE,
//                                                     CALIBRATION_LOW_CURRENT_MAX_ARRAY_LENGTH) * 1000);
    }

    ControlCurrentDacForITune(sp_OP, Temp32);                               //4. Current DAC ON
    ControlVoltageDac(sp_OP, mode);                                         //5. Voltage DAC ON

    g_vi_tune.time_end = 200;
    g_vi_tune.time_flag = 1;

    //wait 20ms, Timer 1 start
    while (g_vi_tune.time_flag == 1) {
        AD7608_Conversion();
        OPERATION_Sensing(sp_OP);

        if (OPERATION_Alram(sp_OP) || g_vi_tune.force_stop == 1) {
            OPERATION_Stop(sp_OP);
            sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
            sp_OP->COMMAND = COM_NONE;
            g_vi_tune.force_stop = 0;
            g_vi_tune.time_flag = 0;
            return g_vi_tune.result = kOperationFail;
        }
    }

    g_vi_tune.loop_temp = g_vi_tune.loop / 1000;
    if (g_vi_tune.loop / g_vi_tune.loop_temp < 1000) {
        return g_vi_tune.result = kWrongParam;
    }

    for (i = 0; i < g_vi_tune.loop; i++) {

        g_vi_tune.loop_count = i;
        AD7608_Conversion();
        OPERATION_Sensing(sp_OP);

        if (OPERATION_Alram(sp_OP) || g_vi_tune.force_stop == 1) {
            OPERATION_Stop(sp_OP);
            sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
            sp_OP->COMMAND = COM_NONE;
            g_vi_tune.force_stop = 0;
            return g_vi_tune.result = kOperationFail;
        }

        if ((i % g_vi_tune.loop_temp) == 0) {
            g_vi_tune.adc_array[j++] = ABS_F(sp_OP->s_OldADCData.SENSING_Current);
        }
    }

    OPERATION_Stop(sp_OP);
    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
    sp_OP->COMMAND = COM_NONE;

    InsertionSort(g_vi_tune.adc_array, 999);

    g_vi_tune.adc_sum = 0.0;
    j = 0;

    for (i = 350; i < 750; i++, j++) {
        g_vi_tune.adc_sum += g_vi_tune.adc_array[i];
    }

    g_vi_tune.adc_average = g_vi_tune.adc_sum / (float) (j);

    index = (Uint16)(current);

    if (mode == CONTROL_CHARGE) {
        g_vi_tune.verify_charge_low_i_adc[index] = g_vi_tune.adc_average;
    } else {
        g_vi_tune.verify_discharge_low_i_adc[index] = g_vi_tune.adc_average;
    }

    return g_vi_tune.result = kOk;
}

void InsertionSort(float *Data, int Number)
{
    int i, j;
    float Key;

    for (i = 1; i <= Number; i++) {
        Key = Data[i];
        j = i - 1;
        while ((j >= 0) && (Data[j] > Key)) {
            Data[j + 1] = Data[j];
            j--;
        }
        Data[j + 1] = Key;
    }
}

void InitSlopeTuneProperties(void)
{
    g_slope_tune.force_stop = 0;
    g_slope_tune.command = 100;

    g_slope_tune.tunning_cur = 0.0;
    g_slope_tune.tunning_mode = CONTROL_OPERATION_INIT;
    g_slope_tune.margin_type = kOver;
    g_slope_tune.overshoot_margin = 0.01;   //1%

    g_slope_tune.start_cur = 0;
    g_slope_tune.end_cur = 0;
    g_slope_tune.rate_cur = 0;

    g_slope_tune.sensing_new_cur = 0.0;
    g_slope_tune.sensing_max_cur = 0.0;

    g_slope_tune.find_res = 0.0;
    g_slope_tune.start_res = AD5293_MAX_F_VLAUE;    //Max kOhm
    g_slope_tune.rate_res = 100.0;                  //100 Ohm

    g_slope_tune.find_flag = 0;
    g_slope_tune.end_flag = 0;
    g_slope_tune.tunning_time_flag = 0;

    g_slope_tune.tunning_time = 0;
    g_slope_tune.rising_time = 100;             //10ms

    g_slope_tune.verify_cur = 0.0;
    g_slope_tune.verify_res = AD5293_MAX_F_VLAUE;
    g_slope_tune.verify_mode = CONTROL_OPERATION_INIT;

    g_slope_tune.verify_time = 0;
    g_slope_tune.verify_time_flag = 0;
    g_slope_tune.verify_end_time = 10000;       //1s

    g_slope_tune.initial_res = AD5293_MAX_F_VLAUE;

    g_slope_tune.linear_switching_time_flag = 1;
    g_slope_tune.linear_switching_time = 0;

    g_slope_tune.linear_end_time = 50;
    g_slope_tune.linear_res_offset = AD5293_MAX_F_VLAUE;
    g_slope_tune.linear_res = AD5293_MAX_F_VLAUE;
    g_slope_tune.linear_gain = 1000.0;
    g_slope_tune.linear_apply_count = 0;

    g_slope_tune.sensing_new_vol = 0.0;
    g_slope_tune.sensing_max_vol = 0.0;
    g_slope_tune.vol_rate = 0.0;
    g_slope_tune.gate_offset = 0.0;
    g_slope_tune.gate_mode = 0;

    g_slope_tune.result = kNotFound;
}

void RunSlopTune(void)
{
    while(1){
        ProcessSlopTune();
    }
}

void ProcessSlopTune(void)
{
    Uint16 i = 0;

    switch (g_slope_tune.command)
    {
        case 1:     //find only one
            TuneSlop(&s_Operation, g_slope_tune.tunning_cur, g_slope_tune.tunning_mode, g_slope_tune.margin_type);
            g_slope_tune.command = 100;
            break;
        case 2:     //find discharge user range
            for (i = g_slope_tune.start_cur; i <= g_slope_tune.end_cur; i += g_slope_tune.rate_cur) {
                TuneSlop(&s_Operation, (float) i, CONTROL_DISCHARGE, g_slope_tune.margin_type);
                AD7608_Conversion();
                OPERATION_Sensing(&s_Operation);
                if (OPERATION_Alram(&s_Operation)|| g_slope_tune.force_stop) {
                    OPERATION_Stop(&s_Operation);
                    g_slope_tune.force_stop = 0;
                    s_Operation.C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
                    break;
                }
            }
            g_slope_tune.command = 100;
            break;
        case 3:     //find charge user range
            for (i = g_slope_tune.start_cur; i <= g_slope_tune.end_cur; i += g_slope_tune.rate_cur) {
                TuneSlop(&s_Operation, (float) i, CONTROL_CHARGE, g_slope_tune.margin_type);
                AD7608_Conversion();
                OPERATION_Sensing(&s_Operation);
                if (OPERATION_Alram(&s_Operation)|| g_slope_tune.force_stop) {
                    OPERATION_Stop(&s_Operation);
                    g_slope_tune.force_stop = 0;
                    s_Operation.C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
                    break;
                }
            }
            g_slope_tune.command = 100;
            break;
        case 4:     //find charge-discharge user range
            for (i = g_slope_tune.start_cur; i <= g_slope_tune.end_cur; i += g_slope_tune.rate_cur) {
                TuneSlop(&s_Operation, (float) i, CONTROL_CHARGE, g_slope_tune.margin_type);
                TuneSlop(&s_Operation, (float) i, CONTROL_DISCHARGE, g_slope_tune.margin_type);
                AD7608_Conversion();
                OPERATION_Sensing(&s_Operation);
                if (OPERATION_Alram(&s_Operation)|| g_slope_tune.force_stop) {
                    OPERATION_Stop(&s_Operation);
                    g_slope_tune.force_stop = 0;
                    s_Operation.C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
                    break;
                }
            }
            g_slope_tune.command = 100;
            break;
        case 8:     //verify one slope
            VerifySlop(&s_Operation, g_slope_tune.verify_cur, g_slope_tune.verify_res, g_slope_tune.verify_mode);
            g_slope_tune.command = 100;
            break;
        case 10:     //verify Linear slope
            VerifyLinearStepSlop(&s_Operation, g_slope_tune.verify_cur, g_slope_tune.linear_res_offset, g_slope_tune.verify_mode);
            g_slope_tune.command = 100;
            break;
        case 50:    //Alarm reset
            OPERATION_AlramReset(&s_Operation);
            g_slope_tune.command = 100;
            break;
        case 51:    //Calibration value reset
            InitSlopeTuneProperties();
            g_slope_tune.command = 100;
            break;
        default:
            break;
    }
    AD7608_Conversion();
    OPERATION_Sensing(&s_Operation);
    if (OPERATION_Alram(&s_Operation)) {
        OPERATION_Stop(&s_Operation);
        s_Operation.C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);
    }
}

Uint16 TuneSlop(S_OPERATION_FILE *sp_OP, float32 current, E_CONTROL_OPERATION_MODE_SELECT mode, MarginType type)
{
    float32 resistance = g_slope_tune.start_res;
    Uint16 table_index = 0;
    float32 target_current;

    OPERATION_Stop(sp_OP);
    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);

    //Check Param
    if (current > 300.0) return g_slope_tune.result = kWrongParam;
    if (mode != CONTROL_CHARGE && mode != CONTROL_DISCHARGE) return g_slope_tune.result = kWrongParam;

    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_ON);

    g_slope_tune.end_flag = 0;
    g_slope_tune.result = kNotFound;

    while ((g_slope_tune.end_flag == 0) && (resistance >= 0.0)) {

        ControlInitial(sp_OP, mode);                                        //1. Initial
        if(mode == CONTROL_DISCHARGE) {
            DELAY_US(100000);
        }

        sp_OP->C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, resistance);   //2. Slope change
        ControlGateOffset(sp_OP, current, mode);                            //3. Gate Offset
        ControlCurrentDac(sp_OP, current);                                  //4. Current DAC ON
        ControlVoltageDac(sp_OP, mode);                                     //5. Voltage DAC ON

        if(type == kOver) {
            target_current = current + (current * g_slope_tune.overshoot_margin);
        } else {
            target_current = current - (current * g_slope_tune.overshoot_margin);
        }

        g_slope_tune.now_res = resistance;
        g_slope_tune.tunning_time_flag = 0;
        g_slope_tune.find_flag = 0;
        g_slope_tune.sensing_new_cur = 0.0;
        g_slope_tune.sensing_max_cur = 0.0;

        while ((g_slope_tune.end_flag == 0) && (g_slope_tune.tunning_time_flag == 0) && (g_slope_tune.find_flag == 0)) {
            AD7608_Conversion();
            OPERATION_Sensing(sp_OP);

            if (mode == CONTROL_CHARGE) g_slope_tune.sensing_new_cur = sp_OP->s_ADCData.SENSING_Current;
            else g_slope_tune.sensing_new_cur = sp_OP->s_ADCData.SENSING_Current * (float) (-1.0);


            //Update MAX Current
            if (g_slope_tune.sensing_new_cur > g_slope_tune.sensing_max_cur) {
                g_slope_tune.sensing_max_cur = g_slope_tune.sensing_new_cur;
            }

            //Check Overshoot Margin(default : 1%)
            if (g_slope_tune.sensing_max_cur > target_current) {

                g_slope_tune.end_flag = 1;
                g_slope_tune.find_flag = 1;

                g_slope_tune.find_res = resistance;
                g_slope_tune.result = kOk;
            }

            //Check Alarm
            if (OPERATION_Alram(sp_OP) || g_slope_tune.force_stop) {
                g_slope_tune.end_flag = 1;
                g_slope_tune.force_stop = 0;
                g_slope_tune.result = kOperationFail;
            }
        }

        resistance -= g_slope_tune.rate_res;
    }

    OPERATION_Stop(sp_OP);
    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);

    if (current < 10.0) {
        table_index = (Uint16) (current);
    } else {
        table_index = (Uint16) (current / 10);
        table_index += 9;
    }

    if(g_slope_tune.result == kOk) {
        if (mode == CONTROL_CHARGE) {
            IntegrationChargeSlopOffset[table_index] = g_slope_tune.find_res;
        } else {
            IntegrationDischargeSlopOffset[table_index] = g_slope_tune.find_res;
        }
    }

    return g_slope_tune.result;
}

Uint16 VerifySlop(S_OPERATION_FILE *sp_OP, float32 current, float32 resistance, E_CONTROL_OPERATION_MODE_SELECT mode)
{
    OPERATION_Stop(sp_OP);
    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);

    //Check Param
    if (current > 300.0) return g_slope_tune.result = kWrongParam;
    if (mode != CONTROL_CHARGE && mode != CONTROL_DISCHARGE) return g_slope_tune.result = kWrongParam;

    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_ON);

    g_slope_tune.result = kOk;

    ControlInitial(sp_OP, mode);                                        //1. Initial
    sp_OP->C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, resistance);   //2. Slope change
    ControlGateOffset(sp_OP, current, mode);                            //3. Gate Offset
    ControlCurrentDac(sp_OP, current);                                  //4. Current DAC ON
    ControlVoltageDac(sp_OP, mode);                                     //5. Voltage DAC ON

    g_slope_tune.verify_time_flag = 0;
    g_slope_tune.sensing_new_cur = 0.0;
    g_slope_tune.end_flag = 0;

    while ((g_slope_tune.end_flag == 0) && (g_slope_tune.verify_time_flag == 0)) {
        AD7608_Conversion();
        OPERATION_Sensing(sp_OP);

        //Sensing current
        if (mode == CONTROL_CHARGE) g_slope_tune.sensing_new_cur = sp_OP->s_ADCData.SENSING_Current;
        else g_slope_tune.sensing_new_cur = sp_OP->s_ADCData.SENSING_Current * (float) (-1.0);

        //Check Alram
        if (OPERATION_Alram(sp_OP) || g_slope_tune.force_stop) {
            g_slope_tune.end_flag = 1;
            g_slope_tune.force_stop = 0;
            g_slope_tune.result = kOperationFail;
        }
    }

    OPERATION_Stop(sp_OP);
    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);

    return g_slope_tune.result;
}

Uint16 VerifyLinearStepSlop(S_OPERATION_FILE *sp_OP, float32 current, float32 initial_resistance, E_CONTROL_OPERATION_MODE_SELECT mode)
{
    float voltage_temp = 0.0;

    OPERATION_Stop(sp_OP);
    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);

    //Check Param
    if (current > 300.0) return g_slope_tune.result = kWrongParam;
    if (mode != CONTROL_CHARGE && mode != CONTROL_DISCHARGE) return g_slope_tune.result = kWrongParam;

    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_ON);

    g_slope_tune.result = kOk;

    if (mode == CONTROL_CHARGE) {
        g_slope_tune.linear_end_time = 200;
    }else {
        g_slope_tune.linear_end_time = 200;
    }

    AD7608_Conversion();
    OPERATION_Sensing(sp_OP);

    voltage_temp = ABS_F(sp_OP->s_ADCData.SENSING_BatteryVoltage);

    ControlInitial(sp_OP, mode);                                                //1. Initial
    DELAY_US(10000);
    sp_OP->C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, initial_resistance);   //2. Slope change
    ControlGateOffset(sp_OP, current, mode);                                    //3. Gate Offset
    ControlCurrentDac(sp_OP, current);                                          //4. Current DAC ON
    ControlVoltageDac(sp_OP, mode);                                             //5. Voltage DAC ON

    g_slope_tune.verify_time_flag = 0;
    g_slope_tune.sensing_new_cur = 0.0;
    g_slope_tune.sensing_new_vol = 0.0;
    g_slope_tune.sensing_max_vol = 0.0;
    g_slope_tune.vol_rate = 0.0;
    g_slope_tune.end_flag = 0;

    g_slope_tune.linear_switching_time_flag = 0;
    g_slope_tune.linear_res = initial_resistance;

    while ((g_slope_tune.end_flag == 0) && (g_slope_tune.verify_time_flag == 0)) {
        AD7608_Conversion();
        OPERATION_Sensing(sp_OP);

        //Sensing current
        if (mode == CONTROL_CHARGE) g_slope_tune.sensing_new_cur = sp_OP->s_ADCData.SENSING_Current;
        else g_slope_tune.sensing_new_cur = sp_OP->s_ADCData.SENSING_Current * (float) (-1.0);

        g_slope_tune.sensing_new_vol = ABS_F(sp_OP->s_ADCData.SENSING_BatteryVoltage);

        //Update MAX Voltage
        if (g_slope_tune.sensing_new_vol > g_slope_tune.sensing_max_vol) {
            g_slope_tune.sensing_max_vol = g_slope_tune.sensing_new_vol;
        }

        //Check Alram
        if (OPERATION_Alram(sp_OP) || g_slope_tune.force_stop) {
            g_slope_tune.end_flag = 1;
            g_slope_tune.force_stop = 0;
            g_slope_tune.result = kOperationFail;
        }
    }

    g_slope_tune.vol_rate = g_slope_tune.sensing_max_vol - voltage_temp;

    OPERATION_Stop(sp_OP);
    sp_OP->C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);

    return g_slope_tune.result;
}

void ControlInitial(S_OPERATION_FILE *sp_OP, E_CONTROL_OPERATION_MODE_SELECT mode)
{
    sp_OP->C_VRefDAC = CONTROL_VoltageReferenceDAC((float) (0.0));
    sp_OP->C_IRefDAC = CONTROL_CurrentReferenceDAC(CONTROL_ALL, (float) (0.0));
    sp_OP->C_FINEIRefDAC = CONTROL_FINECurrentReferenceDAC(CONTROL_ALL, (float) (0.0));
    sp_OP->C_GateOffsetDAC = CONTROL_GateOffsetDAC((float) (0.0));

    sp_OP->C_GateOutputAMP = CONTROL_GateOutputAMP(CONTROL_ALL, CONTROL_OPERATION_INIT);
    sp_OP->C_GateOutputMUX = CONTROL_GateOutputMUX(CONTROL_ALL, CONTROL_OPERATION_INIT);
    sp_OP->C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_CHARGE);
#if PCB_VERSION != 220
#if (FET == 200)
        (*sp_OP).C_IntSlopCapMUX = CONTROL_IntegrationSlopCapMUX(CONTROL_CAP_100nF);
#elif (FET == 240)
        (*sp_OP).C_IntSlopCapMUX = CONTROL_IntegrationSlopCapMUX(CONTROL_CAP_220nF);
#endif
#endif
    sp_OP->C_IntSlopMUX = CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_NONE);
    sp_OP->C_IntResetMUX = CONTROL_IntegrationResetMUX(CONTROL_ALL, CONTROL_ON);
    if (mode == CONTROL_CHARGE) {
        DELAY_US(OPERATION_INTEGRATION_RESET_TIME_US_CHA);
    } else {
        DELAY_US(OPERATION_INTEGRATION_RESET_TIME_US_DIS);
    }
    sp_OP->C_IntResetMUX = CONTROL_IntegrationResetMUX(CONTROL_ALL, CONTROL_OFF);
}

void ControlGateOffset(S_OPERATION_FILE *sp_OP, float32 current, E_CONTROL_OPERATION_MODE_SELECT mode)
{
    float32 gate_offset = 0.0;
    float32 current_temp = 0.0;
    float32 gain = 0.0;
    float32 bat_rate = 0.0;

#if (SLOPE_TUNE == 1)
    if (g_slope_tune.gate_mode == 0) {
        if (current > 300.0) {
            current_temp = 300.0;
        } else {
            current_temp = current;
        }

        if (mode == CONTROL_CHARGE) {
#if PCB_VERSION == 220
            gate_offset = (OPERATION_GATEOFFSET_VOLT_GAIN_CHA / current_temp) + OPERATION_GATEOFFSET_VOLT_OFFSET_CHA;
#else
            //gate_offset = (OPERATION_GATEOFFSET_VOLT_GAIN_CHA / current) + OPERATION_GATEOFFSET_VOLT_OFFSET_CHA;

            if (current_temp < 12.0) {
                gate_offset = 7.0;
            } else {
                gain = 0.008;
                bat_rate = 1.8;
//                if(current_temp < 50.0){
//                    bat_rate = 1.0;
//                } else {
//                    bat_rate = 1.8;
//                }
                gate_offset = (float) ((300.0 - current_temp) * gain) + ABS_F((*sp_OP).s_OldADCData.SENSING_BatteryVoltage * bat_rate);
            }

#endif
        } else {
#if PCB_VERSION == 220
            gate_offset = (OPERATION_GATEOFFSET_VOLT_GAIN_DIS / current_temp) + OPERATION_GATEOFFSET_VOLT_OFFSET_DIS;
#else
            //gate_offset = (OPERATION_GATEOFFSET_VOLT_GAIN_DIS / current) + OPERATION_GATEOFFSET_VOLT_OFFSET_DIS;

            if (current_temp < 10.0) {
                gate_offset = 3.0;
            } else {
                gain = 0.006;
                bat_rate = 1.6;
//                if(current_temp < 50.0) {
//                    bat_rate = 1.0;
//                } else {
//                    bat_rate = 2.0;
//                }
                gate_offset = (float) ((300.0 - current_temp) * gain) + ((5.0 - ABS_F((*sp_OP).s_OldADCData.SENSING_BatteryVoltage)) * bat_rate);
            }

#endif
        }
        sp_OP->C_GateOffsetDAC = CONTROL_GateOffsetDAC(gate_offset);
    } else {
        sp_OP->C_GateOffsetDAC = CONTROL_GateOffsetDAC(g_slope_tune.gate_offset);
    }
#endif
#if (VI_ADC_TUNE == 1)
    if (current > 300.0) {
        current_temp = 300.0;
    } else {
        current_temp = current;
    }

    if (mode == CONTROL_CHARGE) {
        //gate_offset = (OPERATION_GATEOFFSET_VOLT_GAIN_CHA / current) + OPERATION_GATEOFFSET_VOLT_OFFSET_CHA;
        if (current_temp < 1.0) {
            gate_offset = 6.0;
        } else {
            gate_offset = (float) ((300.0 - current_temp) * 0.008) + ABS_F((*sp_OP).s_OldADCData.SENSING_BatteryVoltage);
        }
    } else {
        //gate_offset = (OPERATION_GATEOFFSET_VOLT_GAIN_DIS / current) + OPERATION_GATEOFFSET_VOLT_OFFSET_DIS;
        if (current_temp < 10.0) {
            gate_offset = 4.0;
        } else {
            gate_offset = (float) ((300.0 - current_temp) * 0.006) + ((5.0 - ABS_F((*sp_OP).s_OldADCData.SENSING_BatteryVoltage)) * 0.5);
        }
    }

    sp_OP->C_GateOffsetDAC = CONTROL_GateOffsetDAC(gate_offset);
#endif
}

void ControlCurrentDacForITune(S_OPERATION_FILE *sp_OP, float32 current)
{
    Uint32 Temp32 = 0;
    Uint32 FineTemp32 = 0;

    float32 CoarseTotal = 0.0;
    float32 Coarse = 0.0;

    float32 Fine = 0.0;
    float32 FineTotal = 0.0;

    Temp32 = (Uint32)current;

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
}

void ControlCurrentDac(S_OPERATION_FILE *sp_OP, float32 current)
{
    Uint32 Temp32 = 0;
    Uint32 FineTemp32 = 0;

    float32 CoarseTotal = 0.0;
    float32 Coarse = 0.0;

    float32 Fine = 0.0;
    float32 FineTotal = 0.0;

    Temp32 = (Uint32) (current * 1000.0);

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
}

void ControlVoltageDac(S_OPERATION_FILE *sp_OP, E_CONTROL_OPERATION_MODE_SELECT mode)
{
    if (mode == CONTROL_CHARGE) {
        sp_OP->C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_CHARGE);
        sp_OP->C_VRefDAC = CONTROL_VoltageReferenceDAC(OPERATION_LIMIT_MAX_VOLTAGE);
        sp_OP->C_GateOutputAMP = CONTROL_GateOutputAMP(CONTROL_ALL, CONTROL_CHARGE);
        sp_OP->C_GateOutputMUX = CONTROL_GateOutputMUX(CONTROL_ALL, CONTROL_CHARGE);
        sp_OP->C_IntSlopMUX = CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_PM);

    } else {
        sp_OP->C_OperationMUX = CONTROL_OperationMUX(CONTROL_ALL, CONTROL_DISCHARGE);
        sp_OP->C_VRefDAC = CONTROL_VoltageReferenceDAC((OPERATION_LIMIT_MIN_VOLTAGE * (float) (-1.0)));
        sp_OP->C_GateOutputAMP = CONTROL_GateOutputAMP(CONTROL_ALL, CONTROL_DISCHARGE);
        sp_OP->C_GateOutputMUX = CONTROL_GateOutputMUX(CONTROL_ALL, CONTROL_DISCHARGE);
        sp_OP->C_IntSlopMUX = CONTROL_IntegrationSlopMUX(CONTROL_ALL, CONTROL_PM);
    }
}
