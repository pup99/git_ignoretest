/*
 * PhoenixonControls_COMMON.h
 *
 *  Created on: 2018. 5. 20.
 *      Author: BGKim
 */

#ifndef PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_COMMON_H_
#define PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_COMMON_H_

#define DEV_LOG
#define DEV_SCHEDULE_CYCLE
#define DEV_SCHEDULE_SYNC

#define DEV_SCHEDULE_WH
/*
 * old : 220
 * SKI : 230,   100kohm,     Vint triple amp
 * SKI : 231,    50kohm,     Vint triple amp
 */
#define PCB_VERSION 231

/*
 * FET Old : 200 (IXTN200N10L2)
 * FET New : 240 (IXTN240N075L2)
 */
//#define FET 240
#define FET 200

//#define Current_Spec 200
#define Current_Spec 300

#define VI_ADC_TUNE         0 //for calibration,  Enable : 1, Disable = 0
#define SLOPE_TUNE          0 //for Integrator slope tuning,  Enable : 1, Disable = 0

#if (VI_ADC_TUNE == 1) && (SLOPE_TUNE == 1)
#error "Please select only one from ADC Calibration and SLOPE Tuning."
#endif

#define SET 1
#define CLEAR 0

#define ADDR32(A)			(*((volatile ulong *)(A)))
#define ADDR16(A)			(*((volatile ushort *)(A)))
#define ADDR8(A)			(*((volatile uchar *)(A)))

// Byte swapping.
#define SWAP8(A)		(A)
#define SWAP16(A)		((((A << 8 ) & 0xFF00)) | ((A >> 8)& 0x00FF))
#define SWAP32(A)		((((A<<24)&0xFF000000)) | (((A<<8)&0x00FF0000)) | (((A>>8)&0x0000FF00)) | (((A>>24)&0x000000FF)))

#define NEWLINE					"\n"

#define ABS_F(x) ((x) > 0 ? (x) : ((x) * (float)(-1.0)))

#define MAX_16BIT_COUNT (65535)
#define MAX_32BIT_COUNT	(4294967295)

#define INCREASE_16BIT(x) {if(x < MAX_16BIT_COUNT) x += 1;}
#define INCREASE_32BIT(x) {if(x < MAX_32BIT_COUNT) x += 1;}

#define DECREASE_16BIT(x) {if(x) x -= 1;}
#define DECREASE_32BIT(x) {if(x) x -= 1;}
#endif /* PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_COMMON_H_ */
