/*
 * PhoenixonControls_TIMER.c
 *
 *  Created on: 2018. 5. 21.
 *      Author: BGKim
 */
#include "../../../common/include/F28x_Project.h"
#include "../../../common/include/driverlib.h"
#include "../../../common/include/device.h"

extern volatile S_COMMUNICATION_GSRAM_CPU1 GSRAM_STPE[];
extern volatile S_COMMUNICATION_GSRAM_CPU2 GSRAM_DATA[];

extern volatile Uint32 GSRAM_DATA_BANK_INDEX_CPU1;
extern volatile Uint32 GSRAM_DATA_BANK_INDEX_CPU2;

extern volatile Uint32 SYNC_SIGNAL_CPU1;
extern volatile Uint32 SYNC_SIGNAL_CPU2;

extern volatile Uint32 ui_UpdateflagCPU1;
extern volatile Uint32 ui_UpdateflagCPU2;

extern volatile Uint32 ui_UpdateSTEPCPU1;
extern volatile Uint32 ui_UpdateSTEPCPU2;

extern volatile Uint32 ui_UpdateDATACPU1;
extern volatile Uint32 ui_UpdateDATACPU2;

extern volatile S_COMMUNICATION_TX_DATA s_DATA[];
extern volatile S_COMMUNICATION_RX_JUMP s_JUMP;
extern volatile S_COMMUNICATION_RX_COMMAD s_COMMAND;
extern volatile S_COMMUNICATION_RX_START s_START;
extern volatile S_COMMUNICATION_RX_PAUSE s_PAUSE;

extern volatile S_COMMUNICATION_TX_END   s_SENSING_END;
extern volatile S_COMMUNICATION_TX_ALARM s_SENSING_ALARM;

extern S_OPERATION_FILE s_Operation;

#pragma CODE_SECTION(DSP_Timer0_ISR, ".TI.ramfunc");
#pragma CODE_SECTION(DSP_Timer1_ISR, ".TI.ramfunc");
#pragma CODE_SECTION(DSP_Timer2_ISR, ".TI.ramfunc");

#define SYNC_DURATION               9
#define SYNC_OVERTIME               300000
#define SYNC_RANGE                  200
#define SYNC_OVERTIME_MID           (SYNC_OVERTIME - SYNC_RANGE)        //29800
#define SYNC_OVERTIME_LOW           (SYNC_OVERTIME_MID - SYNC_RANGE)    //29600

#define TIME_PERIOD_1MS             200000
#define SYNC_TIMEPERIOD_HIGH_LIMIT  (TIME_PERIOD_1MS + 1000)            //201000
#define SYNC_TIMEPERIOD_LOW_LIMIT   (TIME_PERIOD_1MS - 1000)            //199000

float32 debug_res_table[256] = { 0.0, };
float32 debug_cur_table[256] = { 0.0, };
float32 debug_weight_table[256] = { 0.0, };

volatile Uint16 FlagPause = CLEAR;
volatile E_OPERATION_COM_SELECT SaveCOMMAND = COM_NONE;
volatile Uint16 SYNC = OPERATION_END_CONDITION_NONE;
volatile Uint16 SYNC_STEP_OLD = 0;

void DSP_TimerInit(void)
{
    //
    // Initialize timer period to maximum
    //
    CPUTimer_setPeriod(CPUTIMER0_BASE, 0xFFFFFFFF);
    CPUTimer_setPeriod(CPUTIMER1_BASE, 0xFFFFFFFF);
    CPUTimer_setPeriod(CPUTIMER2_BASE, 0xFFFFFFFF);

    //
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT)
    //
    CPUTimer_setPreScaler(CPUTIMER0_BASE, 0);
    CPUTimer_setPreScaler(CPUTIMER1_BASE, 0);
    CPUTimer_setPreScaler(CPUTIMER2_BASE, 0);

    //
    // Make sure timer is stopped
    //
    CPUTimer_stopTimer(CPUTIMER0_BASE);
    CPUTimer_stopTimer(CPUTIMER1_BASE);
    CPUTimer_stopTimer(CPUTIMER2_BASE);

    //
    // Reload all counter register with period value
    //
    CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER1_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER2_BASE);

    DSP_Configimer(CPUTIMER0_BASE, DEVICE_SYSCLK_FREQ, DSP_TIMER0_TIME);
    DSP_Configimer(CPUTIMER1_BASE, DEVICE_SYSCLK_FREQ, DSP_TIMER1_TIME);
    DSP_Configimer(CPUTIMER2_BASE, DEVICE_SYSCLK_FREQ, DSP_TIMER2_TIME);

}

void DSP_Configimer(Uint32 Timer, float Frequency, float Period)
{
    Uint32 ul_Temp;

    //
    // Initialize timer period:
    //
    ul_Temp = (Uint32)(Frequency / 1000000 * Period);
    CPUTimer_setPeriod(Timer, ul_Temp);

    //
    // Set pre-scale counter to divide by 1 (SYSCLKOUT):
    //
    CPUTimer_setPreScaler(Timer, 0);

    //
    // Initializes timer control register. The timer is stopped, reloaded,
    // free run disabled, and interrupt enabled.
    // Additionally, the free and soft bits are set
    //
    CPUTimer_stopTimer(Timer);
    CPUTimer_reloadTimerCounter(Timer);
    CPUTimer_setEmulationMode(Timer, CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
    CPUTimer_enableInterrupt(Timer);

}

__interrupt void DSP_Timer0_ISR(void)
{
	static float32 VoltageAVG = 0.0;
	static float32 CurrentAVG = 0.0;
	static float32 WattAVG = 0.0;
    static Uint32 AVGCounter = 0;

    static Uint16 UpdateFlagStepStart = CLEAR;
    static Uint16 UpdateFlagStepEnd = CLEAR;

    Uint32 TimeOut = SYNC_OVERTIME;

    Uint16 DataCount = 0;
    Uint16 Bank = (GSRAM_DATA_BANK_INDEX_CPU1 + 1) % COMMUNICATION_TX_DEVICE_BANK;
    static E_OPERATION_END_CONDITION EndConditionState = OPERATION_END_CONDITION_NONE;
    static E_OPERATION_END_CONDITION SaveEndConditionState = OPERATION_END_CONDITION_NONE;

    static Uint16 TimerCount = 0;
    static Uint32 Timer_Period = TIME_PERIOD_1MS;

    DataCount = (s_Operation.VARIABLE_TimeSpanResult % COMMUNICATION_TX_DATA_LENGTH);

    OPERATION_Sensing(&s_Operation);
    OPERATION_ModeCheck(&s_Operation);
    OPERATION_Alram(&s_Operation);

    COMMUNICATION_COMMANDRead(&s_Operation, GSRAM_STPE, GSRAM_DATA);

    if(COMMUNICATION_STARTRead(&s_Operation, GSRAM_STPE, GSRAM_DATA))
    {
    	DataCount = CLEAR;
        TimerCount = CLEAR;
    }

    VoltageAVG += s_Operation.VARIABLE_VoltageResult;
    CurrentAVG += s_Operation.VARIABLE_CurrentResult;
    WattAVG += s_Operation.VARIABLE_PowerResult;
    AVGCounter++;

    if(DataCount >= COMMUNICATION_MAX_LENGTH_TX_DATA - 1)
    {
        s_Operation.VARIABLE_EndConditionVoltageValueAVG =  VoltageAVG / (float32)(AVGCounter);
        s_Operation.VARIABLE_EndConditionCurrentValueAVG =  CurrentAVG / (float32)(AVGCounter);
        s_Operation.VARIABLE_EndConditionWattValueAVG =     WattAVG    / (float32)(AVGCounter);


        if(UpdateFlagStepStart == CLEAR && EndConditionState == OPERATION_END_CONDITION_UPDATE_END)
            EndConditionState = OPERATION_EndConditionCheck(&s_Operation);
        else
        {
            EndConditionState = OPERATION_END_CONDITION_NONE_OP;
        }

        VoltageAVG = 0.0;
        CurrentAVG = 0.0;
        WattAVG    = 0.0;
        AVGCounter = 0;
    }

    COMMUNICATION_DATAWrite(GSRAM_STPE, &(s_DATA[Bank].DATA[DataCount]), &s_Operation);

    if(DataCount >= COMMUNICATION_MAX_LENGTH_TX_DATA - 1)
    {
        if (TimerCount < SYNC_DURATION)
            TimerCount++;
        else
            TimerCount = 0;

        while(TimeOut--)
        {
            if(SYNC_SIGNAL_CPU1 !=  SYNC_SIGNAL_CPU2)
            {
                SYNC_SIGNAL_CPU1 = SYNC_SIGNAL_CPU2;
                break;
            }

            CPUTimer_stopTimer(CPUTIMER0_BASE);
            DELAY_US(1);
        }

        CPUTimer_resumeTimer(CPUTIMER0_BASE);

        //Auto timer-synchronization every 1 seconds
        if(TimerCount >= SYNC_DURATION && TimeOut > 0)
        {
            //Keep Timeout from 1 us to 400 us
            if (TimeOut >= (SYNC_OVERTIME - 1) || TimeOut <= SYNC_OVERTIME_LOW)
            {
                if (TimeOut >= (SYNC_OVERTIME - 1))
                    Timer_Period = Timer_Period - ((TimeOut - SYNC_OVERTIME_MID) * 2);
                else
                    Timer_Period = Timer_Period + ((SYNC_OVERTIME_MID - TimeOut) * 2);

                if (Timer_Period < SYNC_TIMEPERIOD_LOW_LIMIT || Timer_Period > SYNC_TIMEPERIOD_HIGH_LIMIT)
                    Timer_Period = TIME_PERIOD_1MS;

                CPUTimer_setPeriod(CPUTIMER0_BASE, Timer_Period);
            }
        }
        //End 2019-01-11

#ifdef DEV_SCHEDULE_SYNC

#if 1
        //end sync
        if (s_Operation.VARIABLE_StartFlag == SET && FlagPause == CLEAR && SYNC == OPERATION_END_CONDITION_NONE) {
            if (OPERATION_END_CONDITION_NONE < EndConditionState && EndConditionState < OPERATION_END_CONDITION_GOTO) {

                if (s_Operation.SYNC_END_PAUSE == 1) {
                        SaveEndConditionState = EndConditionState;
                        SYNC = OPERATION_END_CONDITION_SYNC_END_PAUSE;
                }
            }
        }
#endif

#if 1
        if (SYNC != OPERATION_END_CONDITION_NONE) {
            if (FlagPause == CLEAR) {
                SaveCOMMAND = s_Operation.COMMAND;
            }
            s_Operation.COMMAND = COM_REST;
            FlagPause = SET;
            EndConditionState = OPERATION_END_CONDITION_NONE_OP;

            s_DATA[Bank].DATA[DataCount].END_CONDITION_STATE = (Uint16) (SYNC);
        }
#endif
#endif
        //Jump 구간
        if(GSRAM_DATA[0].CPU2_Jump != GSRAM_STPE[0].CPU1_Jump &&
           s_Operation.VARIABLE_StartFlag == SET)//짬푸할때는 엔드 조건이 0이어야 합니다.
        {
            if(GSRAM_STPE[0].CPU1_STEP[0].STEP_INDEX == GSRAM_STPE[0].CPU1_JUMP.STEP_INDEX)
            {
                COMMUNICATION_STEPRead(&s_Operation, GSRAM_STPE, GSRAM_DATA, 0);

                //DL 2019-07-09
                //Same cycle
                if ((s_Operation.CYCLE_INDEX_OLD == s_Operation.CYCLE_INDEX) && (s_Operation.STEP_INDEX < s_Operation.STEP_INDEX_OLD))
                {
                     s_Operation.VARIABLE_CYCLE_LOOP_COUNT++;
                }
                //End DL 2019-07-09

                GSRAM_DATA[0].CPU2_Jump = GSRAM_STPE[0].CPU1_Jump;
            }
            else
            {
                s_Operation.STEP_INDEX = GSRAM_STPE[0].CPU1_JUMP.STEP_INDEX;
            }
            EndConditionState = OPERATION_END_CONDITION_NONE_OP;

            //DL 2019-06-09 (JUMP ECHO)
            s_DATA[Bank].DATA[DataCount].END_CONDITION_STATE = (Uint16)OPERATION_END_CONDITION_NONE_OP;

        }//Pause 구간
        else if(GSRAM_DATA[0].CPU2_Pause != GSRAM_STPE[0].CPU1_Pause)
        {
            switch(GSRAM_STPE[0].CPU1_PAUSE.COMMAND)
            {
            case OPERATION_PAUSE_STOP:
#ifdef DEV_SCHEDULE_SYNC
                SYNC = OPERATION_END_CONDITION_NONE;
                SaveEndConditionState = OPERATION_END_CONDITION_NONE;
#endif
                SaveCOMMAND = COM_NONE;
                s_DATA[Bank].DATA[DataCount].END_CONDITION_STATE = (Uint16)(OPERATION_END_CONDITION_PAUSE);    //DL 2019-07-14
                break;

            case OPERATION_PAUSE_PAUSE:
                if(FlagPause == CLEAR)SaveCOMMAND = s_Operation.COMMAND;
                s_Operation.COMMAND = COM_REST;
                FlagPause = SET;
                EndConditionState = OPERATION_END_CONDITION_NONE_OP;
                s_DATA[Bank].DATA[DataCount].END_CONDITION_STATE = (Uint16)(OPERATION_END_CONDITION_PAUSE);    //DL 2019-07-14
                break;

            case OPERATION_PAUSE_COMPLETE:
#ifdef DEV_SCHEDULE_SYNC
                SYNC = OPERATION_END_CONDITION_NONE;
                SaveEndConditionState = OPERATION_END_CONDITION_NONE;
#endif
                SaveCOMMAND = COM_NONE;
                FlagPause = CLEAR;
                s_Operation.STEP_INDEX = INDEX_STEP_END;
                EndConditionState = OPERATION_END_CONDITION_NONE_OP;
                GSRAM_DATA[0].CPU2_Pause = GSRAM_STPE[0].CPU1_Pause;
                s_Operation.VARIABLE_ALARM_UpDateFlag = CLEAR;      //DL 2019-07-15
                s_DATA[Bank].DATA[DataCount].END_CONDITION_STATE = (Uint16)OPERATION_END_CONDITION_UPDATE_END;    //DL 2019-07-14
                break;

            case OPERATION_PAUSE_RESTART:
                s_Operation.COMMAND = SaveCOMMAND;
                SaveCOMMAND = COM_NONE;
                FlagPause = CLEAR;

#ifdef DEV_SCHEDULE_SYNC
                if (SYNC != OPERATION_END_CONDITION_NONE) {

                    if (SYNC == OPERATION_END_CONDITION_SYNC_END_PAUSE ) {
                        EndConditionState = SaveEndConditionState;
                        SaveEndConditionState = OPERATION_END_CONDITION_NONE;
                    } else {
                        EndConditionState = OPERATION_END_CONDITION_NONE;
                    }

                    SYNC = OPERATION_END_CONDITION_NONE;
                } else {
                    EndConditionState = OPERATION_END_CONDITION_NONE;
                }
#else
                EndConditionState = OPERATION_END_CONDITION_NONE;
#endif

                GSRAM_DATA[0].CPU2_Pause = GSRAM_STPE[0].CPU1_Pause;
                s_DATA[Bank].DATA[DataCount].END_CONDITION_STATE = (Uint16)(OPERATION_END_CONDITION_PAUSE);    //DL 2019-07-14
                break;
            }
        }
        if(s_Operation.VARIABLE_ALARM_Flag)
        {
            s_Operation.OPERATIN = OP_DEFAULT;
            s_Operation.STEP_INDEX = INDEX_STEP_END;
            EndConditionState = OPERATION_END_CONDITION_NONE_OP;
            s_DATA[Bank].DATA[DataCount].END_CONDITION_STATE = (Uint16)(OPERATION_END_CONDITION_ALARM);    //DL 2019-07-11 (SW engineer request)
        }

        if(s_Operation.VARIABLE_StartFlag == SET && FlagPause == CLEAR)
        {

            UpdateFlagStepStart = SET;
            //End DL 2019-01-11

            if(UpdateFlagStepEnd == SET)
            {
                COMMUNICATION_STEPRead(&s_Operation, &GSRAM_STPE[0], &GSRAM_DATA[0], (Uint16)EndConditionState);
                EndConditionState = OPERATION_END_CONDITION_UPDATE_END;
                UpdateFlagStepEnd = CLEAR;
            }

        }
        else if(FlagPause != SET)
        {
            s_Operation.STEP_INDEX = OPERATION_END_CONDITION_NONE;
        }

        COMMUNICATION_ENDWrite(GSRAM_STPE, GSRAM_DATA, &s_Operation);

        COMMUNICATION_ALARMWrite(GSRAM_STPE, GSRAM_DATA, &s_Operation);

        s_DATA[Bank].Header         = HEADER_DATA;
        s_DATA[Bank].DataCount      = COMMUNICATION_INTEGER_LENGTH_DATA * 2;
        s_DATA[Bank].ChannelIndex   = (1 << COMMUNICATION_CHANNEL_NUMBER);

        GSRAM_DATA[0].CPU2_DATA= s_DATA[Bank];

        GSRAM_DATA[0].CPU2_DataBank = Bank;
        GSRAM_DATA[0].CPU2_StepIndex= s_Operation.STEP_INDEX;

        GSRAM_DATA[0].CPU2_Data     = Bank;

        GSRAM_DATA_BANK_INDEX_CPU1++;
        GSRAM_DATA_BANK_INDEX_CPU1 %= COMMUNICATION_TX_DEVICE_BANK;
        ui_UpdateDATACPU1++;

        //DL 2019-01-11
        if(s_Operation.STEP_INDEX == INDEX_STEP_END)
        {
            OPERATION_Stop(&s_Operation);
            s_Operation.C_IRELAY = CONTROL_CurrentRELAY(CONTROL_ALL, CONTROL_OFF);

#ifdef DEV_SCHEDULE_SYNC
            SYNC = OPERATION_END_CONDITION_NONE;
            SYNC_STEP_OLD = 0;
            SaveEndConditionState = OPERATION_END_CONDITION_NONE;
            FlagPause = CLEAR;
#endif
        }
        //End 2019-01-11
    }
    OPERATION_Run(&s_Operation);

    s_Operation.VARIABLE_TimeSpanResult++;


    if (s_Operation.VARIABLE_StartFlag == SET && FlagPause == CLEAR)
    {
        if (UpdateFlagStepStart == SET)
        {
            if (ui_UpdateSTEPCPU1 != ui_UpdateSTEPCPU2)
            {

                s_Operation.VARIABLE_UpdateData++;
                if (s_Operation.STEP_INDEX == GSRAM_STPE[0].CPU1_STEP[0].STEP_INDEX)
                {
                    COMMUNICATION_STEPRead(&s_Operation, GSRAM_STPE, GSRAM_DATA, 0);
                    UpdateFlagStepStart = CLEAR;
                    UpdateFlagStepEnd = SET;
                }
                ui_UpdateSTEPCPU1 = ui_UpdateSTEPCPU2;
            }
        }
    }
    if (s_Operation.VARIABLE_StartFlag == SET && FlagPause == SET)
    {
        if (ui_UpdateSTEPCPU1 != ui_UpdateSTEPCPU2)
        {
            s_Operation.VARIABLE_UpdateData++;
            ui_UpdateSTEPCPU1 = ui_UpdateSTEPCPU2;
        }
    }

    AD7608_Conversion();   //DL 2019-07-01

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

__interrupt void DSP_Timer1_ISR(void)
{
#if (VI_ADC_TUNE == 1)
    float adc7608[8] = { 0.0, };
    float current = 0.0;
    Uint32 res_temp = 0;

    if (g_vi_tune.time_flag == 1) {
        g_vi_tune.time++;

        AD7608_Conversion();
        AD7608_Read(AD7608_10_VOLTAGE, AD7608_NON, adc7608);
        current = (adc7608[1] * SENSING_CURRENT_GAIN ) + (adc7608[2] * SENSING_CURRENT_GAIN ) + (adc7608[3] * SENSING_CURRENT_GAIN ) + (adc7608[4] * SENSING_CURRENT_GAIN );

        debug_cur_table[g_vi_tune.time] = ABS_F(current);
        debug_weight_table[g_vi_tune.time] = (float) ((float) g_vi_tune.time_end / (float) ((g_vi_tune.time_end - 1) + g_vi_tune.time));

        g_vi_tune.slope = (ABS_F(current) * (g_vi_tune.slope_gain * debug_weight_table[g_vi_tune.time])) + g_vi_tune.slope_offset;

        res_temp = (Uint32) g_vi_tune.slope;
        res_temp = res_temp - (res_temp % AD5293_RESOLUTION);

        debug_res_table[g_vi_tune.time] = g_vi_tune.slope = (float) res_temp;

        s_Operation.C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, g_vi_tune.slope);

        if (g_vi_tune.time >= g_vi_tune.time_end) {
            g_vi_tune.time_flag = 0;
        }
    } else {
        g_vi_tune.time = 0;
    }
#endif

#if (SLOPE_TUNE == 1)
    float adc7608[8] = {0.0,};
    float current = 0.0;
    Uint32 res_temp = 0;

    //Rising & Falling time check
    if (g_slope_tune.tunning_time_flag == 0) {
        g_slope_tune.tunning_time++;
        if (g_slope_tune.tunning_time >= g_slope_tune.rising_time) {
            g_slope_tune.tunning_time_flag = 1;
        }
    } else {
        g_slope_tune.tunning_time = 0;
    }

    //Running time check for verify resistance
    if (g_slope_tune.verify_time_flag == 0) {
        g_slope_tune.verify_time++;
        if (g_slope_tune.verify_time >= g_slope_tune.verify_end_time) {
            g_slope_tune.verify_time_flag = 1;
        }
    } else {
        g_slope_tune.verify_time = 0;
    }

#if 1
    if (g_slope_tune.linear_switching_time_flag == 0) {

        AD7608_Conversion();
        AD7608_Read(AD7608_10_VOLTAGE, AD7608_NON, adc7608);
        current = (adc7608[1] * SENSING_CURRENT_GAIN ) + (adc7608[2] * SENSING_CURRENT_GAIN ) + (adc7608[3] * SENSING_CURRENT_GAIN ) + (adc7608[4] * SENSING_CURRENT_GAIN );

#if 1   //Time base gain rate change

        g_slope_tune.linear_switching_time++;
        if (g_slope_tune.linear_apply_count != 0) {
            if ((g_slope_tune.linear_switching_time % g_slope_tune.linear_apply_count) == 0) {
                debug_cur_table[g_slope_tune.linear_switching_time] = ABS_F(current);
                debug_weight_table[g_slope_tune.linear_switching_time] = (float) ((float) g_slope_tune.linear_end_time / (float) ((g_slope_tune.linear_end_time - 1) + g_slope_tune.linear_switching_time));

                //g_slope_tune.linear_res = (ABS_F(current) * (g_slope_tune.linear_gain)) + g_slope_tune.linear_res_offset;
                g_slope_tune.linear_res = (ABS_F(current) * (g_slope_tune.linear_gain * debug_weight_table[g_slope_tune.linear_switching_time])) + g_slope_tune.linear_res_offset;

                res_temp = (Uint32) g_slope_tune.linear_res;
                res_temp = res_temp - (res_temp % AD5293_RESOLUTION);

                debug_res_table[g_slope_tune.linear_switching_time] = g_slope_tune.linear_res = (float) res_temp;

                s_Operation.C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, g_slope_tune.linear_res);
            }
        } else {
            debug_cur_table[g_slope_tune.linear_switching_time] = ABS_F(current);
            debug_weight_table[g_slope_tune.linear_switching_time] = (float) ((float) g_slope_tune.linear_end_time / (float) ((g_slope_tune.linear_end_time - 1) + g_slope_tune.linear_switching_time));

            //g_slope_tune.linear_res = (ABS_F(current) * (g_slope_tune.linear_gain)) + g_slope_tune.linear_res_offset;
            g_slope_tune.linear_res = (ABS_F(current) * (g_slope_tune.linear_gain * debug_weight_table[g_slope_tune.linear_switching_time])) + g_slope_tune.linear_res_offset;

            res_temp = (Uint32) g_slope_tune.linear_res;
            res_temp = res_temp - (res_temp % AD5293_RESOLUTION);

            debug_res_table[g_slope_tune.linear_switching_time] = g_slope_tune.linear_res = (float) res_temp;

            s_Operation.C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, g_slope_tune.linear_res);
        }
        if (g_slope_tune.linear_switching_time >= g_slope_tune.linear_end_time) {
            g_slope_tune.linear_switching_time_flag = 1;
        }
    } else {
        g_slope_tune.linear_switching_time = 0;
    }
#endif
#endif
#endif
}

__interrupt void DSP_Timer2_ISR(void)
{
    float adc7608[8] = { 0.0, };
    float gain_weight = 1.0;
    float current = 0.0;
    Uint32 slop_temp = 0;

    if (s_Operation.s_Slop.set == SET) {

        AD7608_Conversion();
        AD7608_Read(AD7608_10_VOLTAGE, AD7608_NON, adc7608);

        debug_cur_table[s_Operation.s_Slop.time] = current = ABS_F((adc7608[1] * SENSING_CURRENT_GAIN ) + (adc7608[2] * SENSING_CURRENT_GAIN ) + (adc7608[3] * SENSING_CURRENT_GAIN ) + (adc7608[4] * SENSING_CURRENT_GAIN ));
        //current = ABS_F((adc7608[1] * SENSING_CURRENT_GAIN ) + (adc7608[2] * SENSING_CURRENT_GAIN ) + (adc7608[3] * SENSING_CURRENT_GAIN ) + (adc7608[4] * SENSING_CURRENT_GAIN ));

        if (s_Operation.VARIABLE_CurrentRef >= 10.0) {
            if (current < 1.0) {
                return;
            }
        }

        s_Operation.s_Slop.time++;

        debug_weight_table[s_Operation.s_Slop.time] = gain_weight = (float) ((float) (s_Operation.s_Slop.end) / (float) (s_Operation.s_Slop.end + s_Operation.s_Slop.time));
        //gain_weight = (float) ((float) (s_Operation.s_Slop.end) / (float) (s_Operation.s_Slop.end + s_Operation.s_Slop.time));

        s_Operation.s_Slop.res = (current * (s_Operation.s_Slop.gain * gain_weight)) + s_Operation.s_Slop.offset;

        slop_temp = (Uint32) s_Operation.s_Slop.res;
        slop_temp = slop_temp - (slop_temp % AD5293_RESOLUTION);
        debug_res_table[s_Operation.s_Slop.time] = s_Operation.s_Slop.res = (float) slop_temp;
        s_Operation.s_Slop.res = (float) slop_temp;

        s_Operation.C_IntSlopPMDAC = CONTROL_IntegrationSlopPMDAC(CONTROL_ALL, s_Operation.s_Slop.res);

        if (s_Operation.s_Slop.time >= s_Operation.s_Slop.end) {
            s_Operation.s_Slop.set = CLEAR;
            s_Operation.s_Slop.time = 0;
            AD7608_OverSamplingSelect(AD7608_64);
            CPUTimer_stopTimer(CPUTIMER2_BASE);
        }
    }
}

//
// End of file
//


