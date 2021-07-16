/*
 * PhoenixonControls_TUNING.h
 *
 *  Created on: 2019. 9. 19.
 *      Author: HW Team
 */

#ifndef PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_TUNING_H_
#define PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_TUNING_H_

typedef enum
{
    kOk,
    kWrongParam,
    kOperationFail,
    kNotFound
} TuneResult;

typedef enum
{
    kNone,
    kCurrent,
    kVoltage,
    kVoltage_ref
} VITuneMode;

typedef enum
{
    kOver,
    kUnder
} MarginType;

typedef struct
{
    float32 adc_sum;
    float32 adc_array[1024];
    float32 adc_average;
    float32 verify_charge_v_adc[CALIBRATION_VOLATAGE_MAX_ARRAY_LENGTH];
    float32 verify_discharge_v_adc[CALIBRATION_VOLATAGE_MAX_ARRAY_LENGTH];
    float32 verify_charge_v_ref[CALIBRATION_VOLATAGE_MAX_ARRAY_LENGTH];
    float32 verify_discharge_v_ref[CALIBRATION_VOLATAGE_MAX_ARRAY_LENGTH];
    float32 charge_v_ref;
    float32 discharge_v_ref;
    float32 verify_charge_i_adc[CALIBRATION_CURRENT_MAX_ARRAY_LENGTH];
    float32 verify_charge_low_i_adc[CALIBRATION_CURRENT_MAX_ARRAY_LENGTH];
    float32 verify_discharge_i_adc[CALIBRATION_CURRENT_MAX_ARRAY_LENGTH];
    float32 verify_discharge_low_i_adc[CALIBRATION_CURRENT_MAX_ARRAY_LENGTH];
    VITuneMode mode;
    Uint16 command;
    float32 cv_voltage;
    float32 start;
    float32 end;
    float32 force_coarse_cur;
    float32 force_fine_cur;
    Uint16 force_stop;
    Uint16 force_loop_stop;
    Uint16 index;
    float32 slope;
    float32 slope_gain;
    float32 slope_offset;
    float32 zero_offset;
    Uint16 time;
    Uint16 time_end;
    Uint16 time_flag;
    Uint32 loop;
    Uint32 loop_temp;
    Uint32 loop_count;
    TuneResult result;
} VITuneProperties;

typedef struct
{
    Uint16 force_stop;
    Uint16 command;

    float32 tunning_cur;
    E_CONTROL_OPERATION_MODE_SELECT tunning_mode;
    MarginType margin_type;
    float32 overshoot_margin;

    Uint16 start_cur;
    Uint16 end_cur;
    Uint16 rate_cur;

    float32 sensing_new_cur;
    float32 sensing_max_cur;

    float32 find_res;
    float32 now_res;
    float32 start_res;
    float32 rate_res;

    Uint16 find_flag;
    Uint16 end_flag;
    Uint16 tunning_time_flag;

    Uint16 tunning_time;
    Uint16 rising_time;

    float32 verify_cur;
    float32 verify_res;
    E_CONTROL_OPERATION_MODE_SELECT verify_mode;

    Uint32 verify_time;
    Uint16 verify_time_flag;
    Uint32 verify_end_time;

    float32 initial_res;

    Uint16 linear_switching_time_flag;
    Uint16 linear_switching_time;

    Uint16 linear_end_time;

    float32 linear_res_offset;
    float32 linear_res;
    float32 linear_gain;

    Uint16 linear_apply_count;

    float32 sensing_new_vol;
    float32 sensing_max_vol;

    float32 vol_rate;
    float32 gate_offset;
    Uint16 gate_mode;
    TuneResult result;
} SlopeTuneProperties;

extern SlopeTuneProperties g_slope_tune;
extern VITuneProperties g_vi_tune;

void InitVIADCTuneProperties(void);
void RunVIADCTune(void);

void InitSlopeTuneProperties(void);
void RunSlopTune(void);

#endif /* PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_TUNING_H_ */
