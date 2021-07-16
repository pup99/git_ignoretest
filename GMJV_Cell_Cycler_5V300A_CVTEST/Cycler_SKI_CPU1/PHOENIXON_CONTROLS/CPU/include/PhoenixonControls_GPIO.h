/*
 * PhoenixonControls_GPIO.h
 *
 *  Created on: 2018. 5. 19.
 *      Author: BGKim
 */

#ifndef PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_GPIO_H_
#define PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_GPIO_H_

#define DSP_GPIO00_O_OUTPUT_MASTER_CHARGE_SET()					{GpioDataRegs.GPASET.bit.GPIO0 = 1;}
#define DSP_GPIO01_O_OUTPUT_MASTER_DISCHARGE_SET()				{GpioDataRegs.GPASET.bit.GPIO1 = 1;}
#define DSP_GPIO02_O_OUTPUT_SLAVE_CHARGE_SET()					{GpioDataRegs.GPASET.bit.GPIO2 = 1;}
#define DSP_GPIO03_O_OUTPUT_SLAVE_DISCHARGE_SET()				{GpioDataRegs.GPASET.bit.GPIO3 = 1;}

#define DSP_GPIO00_O_OUTPUT_MASTER_CHARGE_CLEAR()				{GpioDataRegs.GPACLEAR.bit.GPIO0 = 1;}
#define DSP_GPIO01_O_OUTPUT_MASTER_DISCHARGE_CLEAR()			{GpioDataRegs.GPACLEAR.bit.GPIO1 = 1;}
#define DSP_GPIO02_O_OUTPUT_SLAVE_CHARGE_CLEAR()				{GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;}
#define DSP_GPIO03_O_OUTPUT_SLAVE_DISCHARGE_CLEAR()				{GpioDataRegs.GPACLEAR.bit.GPIO3 = 1;}



#define DSP_GPIO04_O_OPMODE_MASTER_SEL0_SET()					{GpioDataRegs.GPASET.bit.GPIO4 = 1;}
#define DSP_GPIO05_O_OPMODE_MASTER_SEL1_SET()					{GpioDataRegs.GPASET.bit.GPIO5 = 1;}
#define DSP_GPIO06_O_OPMODE_SLAVE_SEL0_SET()					{GpioDataRegs.GPASET.bit.GPIO6 = 1;}
#define DSP_GPIO07_O_OPMODE_SLAVE_SEL1_SET()					{GpioDataRegs.GPASET.bit.GPIO7 = 1;}

#define DSP_GPIO04_O_OPMODE_MASTER_SEL0_CLEAR()					{GpioDataRegs.GPACLEAR.bit.GPIO4 = 1;}
#define DSP_GPIO05_O_OPMODE_MASTER_SEL1_CLEAR()					{GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;}
#define DSP_GPIO06_O_OPMODE_SLAVE_SEL0_CLEAR()					{GpioDataRegs.GPACLEAR.bit.GPIO6 = 1;}
#define DSP_GPIO07_O_OPMODE_SLAVE_SEL1_CLEAR()					{GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;}



#define DSP_GPIO08_O_INTG_MASTER_SEL0_SET()						{GpioDataRegs.GPASET.bit.GPIO8 = 1;}
#define DSP_GPIO09_O_INTG_MASTER_SEL1_SET()						{GpioDataRegs.GPASET.bit.GPIO9 = 1;}
#define DSP_GPIO50_O_INTG_MASTER_SEL2_SET()						{GpioDataRegs.GPBSET.bit.GPIO50 = 1;}
#define DSP_GPIO10_O_INTG_RESET_SEL0_SET()						{GpioDataRegs.GPASET.bit.GPIO10 = 1;}
#define DSP_GPIO11_O_INTG_RESET_SEL1_SET()						{GpioDataRegs.GPASET.bit.GPIO11 = 1;}
#define DSP_GPIO12_O_INTG_SLAVE_SEL0_SET()						{GpioDataRegs.GPASET.bit.GPIO12 = 1;}
#define DSP_GPIO13_O_INTG_SLAVE_SEL1_SET()						{GpioDataRegs.GPASET.bit.GPIO13 = 1;}
#define DSP_GPIO51_O_INTG_SLAVE_SEL2_SET()						{GpioDataRegs.GPBSET.bit.GPIO51 = 1;}
#define DSP_GPIO52_O_INTG_CHARGE_SEL0_SET()						{GpioDataRegs.GPBSET.bit.GPIO52 = 1;}
#define DSP_GPIO53_O_INTG_CHARGE_SEL1_SET()						{GpioDataRegs.GPBSET.bit.GPIO53 = 1;}

#define DSP_GPIO35_O_INTG_CAP_SEL0_SET()                        {GpioDataRegs.GPBSET.bit.GPIO35 = 1;}
#define DSP_GPIO36_O_INTG_CAP_SEL1_SET()                        {GpioDataRegs.GPBSET.bit.GPIO36 = 1;}
#define DSP_GPIO38_O_INTG_CAP_SEL2_SET()                        {GpioDataRegs.GPBSET.bit.GPIO38 = 1;}

#define DSP_GPIO08_O_INTG_MASTER_SEL0_CLEAR()					{GpioDataRegs.GPACLEAR.bit.GPIO8 = 1;}
#define DSP_GPIO09_O_INTG_MASTER_SEL1_CLEAR()					{GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;}
#define DSP_GPIO50_O_INTG_MASTER_SEL2_CLEAR()					{GpioDataRegs.GPBCLEAR.bit.GPIO50 = 1;}
#define DSP_GPIO10_O_INTG_RESET_SEL0_CLEAR()					{GpioDataRegs.GPACLEAR.bit.GPIO10 = 1;}
#define DSP_GPIO11_O_INTG_RESET_SEL1_CLEAR()					{GpioDataRegs.GPACLEAR.bit.GPIO11 = 1;}
#define DSP_GPIO12_O_INTG_SLAVE_SEL0_CLEAR()					{GpioDataRegs.GPACLEAR.bit.GPIO12 = 1;}
#define DSP_GPIO13_O_INTG_SLAVE_SEL1_CLEAR()					{GpioDataRegs.GPACLEAR.bit.GPIO13 = 1;}
#define DSP_GPIO51_O_INTG_SLAVE_SEL2_CLEAR()					{GpioDataRegs.GPBCLEAR.bit.GPIO51 = 1;}
#define DSP_GPIO52_O_INTG_CHARGE_SEL0_CLEAR()					{GpioDataRegs.GPBCLEAR.bit.GPIO52 = 1;}
#define DSP_GPIO53_O_INTG_CHARGE_SEL1_CLEAR()					{GpioDataRegs.GPBCLEAR.bit.GPIO53 = 1;}

#define DSP_GPIO35_O_INTG_CAP_SEL0_CLEAR()                      {GpioDataRegs.GPBCLEAR.bit.GPIO35 = 1;}
#define DSP_GPIO36_O_INTG_CAP_SEL1_CLEAR()                      {GpioDataRegs.GPBCLEAR.bit.GPIO36 = 1;}
#define DSP_GPIO38_O_INTG_CAP_SEL2_CLEAR()                      {GpioDataRegs.GPBCLEAR.bit.GPIO38 = 1;}

#define DSP_GPIO14_I_MASTER_RDAC_RDY()							GpioDataRegs.GPADAT.bit.GPIO14



#define DSP_GPIO15_I_SLAVE_RDAC_RDY()							GpioDataRegs.GPADAT.bit.GPIO15



#define DSP_GPIO19_I_ENABLE_IO_1()								GpioDataRegs.GPADAT.bit.GPIO19
#define DSP_GPIO19_O_ENABLE_IO_1_SET()							{GpioDataRegs.GPASET.bit.GPIO19 = 1;}
#define DSP_GPIO19_O_ENABLE_IO_1_CLEAR()						{GpioDataRegs.GPACLEAR.bit.GPIO19 = 1;}
#define DSP_GPIO19_O_ENABLE_IO_1_TOGGLE()						{GpioDataRegs.GPATOGGLE.bit.GPIO19 = 1;}





#define DSP_GPIO20_I_ENABLE_IO_0()								GpioDataRegs.GPADAT.bit.GPIO20
#define DSP_GPIO20_O_ENABLE_IO_0_SET()							{GpioDataRegs.GPASET.bit.GPIO20 = 1;}
#define DSP_GPIO20_O_ENABLE_IO_0_CLEAR()						{GpioDataRegs.GPACLEAR.bit.GPIO20 = 1;}
#define DSP_GPIO20_O_ENABLE_IO_0_TOGGLE()						{GpioDataRegs.GPATOGGLE.bit.GPIO20 = 1;}




#define DSP_GPIO32_I_EXT_SYNCH_SLAVE_IN()						GpioDataRegs.GPBDAT.bit.GPIO32

#define DSP_GPIO33_O_EXT_SYNCH_MASTER_SET()						{GpioDataRegs.GPBSET.bit.GPIO33 = 1;}
#define DSP_GPIO33_O_EXT_SYNCH_MASTER_CLEAR()					{GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;}




#define DSP_GPIO42_O_N_MASTER_DIS_EN1_SET()						{GpioDataRegs.GPBCLEAR.bit.GPIO42 = 1;}
#define DSP_GPIO43_O_N_MASTER_DIS_EN0_SET()						{GpioDataRegs.GPBCLEAR.bit.GPIO43 = 1;}
#define DSP_GPIO44_O_N_MASTER_CHG_EN1_SET()						{GpioDataRegs.GPBCLEAR.bit.GPIO44 = 1;}
#define DSP_GPIO45_O_N_MASTER_CHG_EN0_SET()						{GpioDataRegs.GPBCLEAR.bit.GPIO45 = 1;}
#define DSP_GPIO46_O_N_SLAVE_DIS_EN1_SET()						{GpioDataRegs.GPBCLEAR.bit.GPIO46 = 1;}
#define DSP_GPIO47_O_N_SLAVE_DIS_EN0_SET()						{GpioDataRegs.GPBCLEAR.bit.GPIO47 = 1;}
#define DSP_GPIO48_O_N_SLAVE_CHG_EN1_SET()						{GpioDataRegs.GPBCLEAR.bit.GPIO48 = 1;}
#define DSP_GPIO49_O_N_SLAVE_CHG_EN0_SET()						{GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;}

#define DSP_GPIO42_O_N_MASTER_DIS_EN1_CLEAR()					{GpioDataRegs.GPBSET.bit.GPIO42 = 1;}
#define DSP_GPIO43_O_N_MASTER_DIS_EN0_CLEAR()					{GpioDataRegs.GPBSET.bit.GPIO43 = 1;}
#define DSP_GPIO44_O_N_MASTER_CHG_EN1_CLEAR()					{GpioDataRegs.GPBSET.bit.GPIO44 = 1;}
#define DSP_GPIO45_O_N_MASTER_CHG_EN0_CLEAR()					{GpioDataRegs.GPBSET.bit.GPIO45 = 1;}
#define DSP_GPIO46_O_N_SLAVE_DIS_EN1_CLEAR()					{GpioDataRegs.GPBSET.bit.GPIO46 = 1;}
#define DSP_GPIO47_O_N_SLAVE_DIS_EN0_CLEAR()					{GpioDataRegs.GPBSET.bit.GPIO47 = 1;}
#define DSP_GPIO48_O_N_SLAVE_CHG_EN1_CLEAR()					{GpioDataRegs.GPBSET.bit.GPIO48 = 1;}
#define DSP_GPIO49_O_N_SLAVE_CHG_EN0_CLEAR()					{GpioDataRegs.GPBSET.bit.GPIO49 = 1;}




#define DSP_GPIO54_O_DIO_PORT_1_SET()							{GpioDataRegs.GPBSET.bit.GPIO54 = 1;}
#define DSP_GPIO54_O_DIO_PORT_1_CLEAR()							{GpioDataRegs.GPBCLEAR.bit.GPIO54 = 1;}

#define DSP_GPIO55_O_DIO_PORT_0_SET()							{GpioDataRegs.GPBSET.bit.GPIO55 = 1;}
#define DSP_GPIO55_O_DIO_PORT_0_CLEAR()							{GpioDataRegs.GPBCLEAR.bit.GPIO55 = 1;}




#define DSP_GPIO56_O_P_VOLATAGE_RELAY_SET()						{GpioDataRegs.GPBSET.bit.GPIO56 = 1;}
#define DSP_GPIO56_O_P_VOLATAGE_RELAY_CLEAR()					{GpioDataRegs.GPBCLEAR.bit.GPIO56 = 1;}

#define DSP_GPIO57_O_N_VOLATAGE_RELAY_SET()						{GpioDataRegs.GPBSET.bit.GPIO57 = 1;}
#define DSP_GPIO57_O_N_VOLATAGE_RELAY_CLEAR()					{GpioDataRegs.GPBCLEAR.bit.GPIO57 = 1;}

#define DSP_GPIO58_O_ADC_OS0_SET()								{GpioDataRegs.GPBSET.bit.GPIO58 = 1;}
#define DSP_GPIO59_O_ADC_OS1_SET()								{GpioDataRegs.GPBSET.bit.GPIO59 = 1;}
#define DSP_GPIO60_O_ADC_OS2_SET()								{GpioDataRegs.GPBSET.bit.GPIO60 = 1;}

#define DSP_GPIO58_O_ADC_OS0_CLEAR()							{GpioDataRegs.GPBCLEAR.bit.GPIO58 = 1;}
#define DSP_GPIO59_O_ADC_OS1_CLEAR()							{GpioDataRegs.GPBCLEAR.bit.GPIO59 = 1;}
#define DSP_GPIO60_O_ADC_OS2_CLEAR()							{GpioDataRegs.GPBCLEAR.bit.GPIO60 = 1;}

#define DSP_GPIO61_I_ADC_BUSY()									GpioDataRegs.GPBDAT.bit.GPIO61

#define DSP_GPIO62_O_ADC_N_CS_SET()								{GpioDataRegs.GPBCLEAR.bit.GPIO62 = 1;}
#define DSP_GPIO62_O_ADC_N_CS_CLEAR()							{GpioDataRegs.GPBSET.bit.GPIO62 = 1;}

#define DSP_GPIO63_I_ADC_FRSTD()								GpioDataRegs.GPBDAT.bit.GPIO63

#define DSP_GPIO64_O_ADC_RESET_SET()							{GpioDataRegs.GPCSET.bit.GPIO64 = 1;}
#define DSP_GPIO64_O_ADC_RESET_CLEAR()							{GpioDataRegs.GPCCLEAR.bit.GPIO64 = 1;}

#define DSP_GPIO88_O_ADC_RANGE_SET()							{GpioDataRegs.GPCSET.bit.GPIO88 = 1;}
#define DSP_GPIO88_O_ADC_RANGE_CLEAR()							{GpioDataRegs.GPCCLEAR.bit.GPIO88 = 1;}



#define DSP_GPIO65_I_EMERGENCY_IN()								GpioDataRegs.GPCDAT.bit.GPIO65



#define DSP_GPIO66_O_DAC_A0_SET()								{GpioDataRegs.GPCSET.bit.GPIO66 = 1;}
#define DSP_GPIO67_O_DAC_A1_SET()								{GpioDataRegs.GPCSET.bit.GPIO67 = 1;}

#define DSP_GPIO66_O_DAC_A0_CLEAR()								{GpioDataRegs.GPCCLEAR.bit.GPIO66 = 1;}
#define DSP_GPIO67_O_DAC_A1_CLEAR()								{GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1;}

#define DSP_GPIO68_O_DAC_FINE_I_N_RESET_SET()					{GpioDataRegs.GPCCLEAR.bit.GPIO68 = 1;}
#define DSP_GPIO86_O_DAC_V_N_RESET_SET()						{GpioDataRegs.GPCCLEAR.bit.GPIO86 = 1;}
#define DSP_GPIO87_O_DAC_I_N_RESET_SET()						{GpioDataRegs.GPCCLEAR.bit.GPIO87 = 1;}

#define DSP_GPIO68_O_DAC_FINE_I_N_RESET_CLEAR()					{GpioDataRegs.GPCSET.bit.GPIO68 = 1;}
#define DSP_GPIO86_O_DAC_V_N_RESET_CLEAR()						{GpioDataRegs.GPCSET.bit.GPIO86 = 1;}
#define DSP_GPIO87_O_DAC_I_N_RESET_CLEAR()						{GpioDataRegs.GPCSET.bit.GPIO87 = 1;}



#define DSP_GPIO89_O_MASTER_RDAC_N_RESET_SET()					{GpioDataRegs.GPCCLEAR.bit.GPIO89 = 1;}
#define DSP_GPIO90_O_MASTER_RDAC_N_CS_SET()						{GpioDataRegs.GPCCLEAR.bit.GPIO90 = 1;}
#define DSP_GPIO91_O_SLAVE_RDAC_N_RESET_SET()					{GpioDataRegs.GPCCLEAR.bit.GPIO91 = 1;}
#define DSP_GPIO93_O_SLAVE_RDAC_N_CS_SET()						{GpioDataRegs.GPCCLEAR.bit.GPIO93 = 1;}

#define DSP_GPIO89_O_MASTER_RDAC_N_RESET_CLEAR()				{GpioDataRegs.GPCSET.bit.GPIO89 = 1;}
#define DSP_GPIO90_O_MASTER_RDAC_N_CS_CLEAR()					{GpioDataRegs.GPCSET.bit.GPIO90 = 1;}
#define DSP_GPIO91_O_SLAVE_RDAC_N_RESET_CLEAR()					{GpioDataRegs.GPCSET.bit.GPIO91 = 1;}
#define DSP_GPIO93_O_SLAVE_RDAC_N_CS_CLEAR()					{GpioDataRegs.GPCSET.bit.GPIO93 = 1;}



#define DSP_GPIO94_O_N_RLYON_1_SET()							{GpioDataRegs.GPCSET.bit.GPIO94 = 1;}
#define DSP_GPIO99_O_N_RLYON_0_SET()							{GpioDataRegs.GPDSET.bit.GPIO99 = 1;}

#define DSP_GPIO94_O_N_RLYON_1_CLEAR()							{GpioDataRegs.GPCCLEAR.bit.GPIO94 = 1;}
#define DSP_GPIO99_O_N_RLYON_0_CLEAR()							{GpioDataRegs.GPDCLEAR.bit.GPIO99 = 1;}



#define DSP_GPIO133_O_DSP_STATUS_SET()							{GpioDataRegs.GPESET.bit.GPIO133 = 1;}
#define DSP_GPIO133_O_DSP_STATUS_CLEAR()						{GpioDataRegs.GPECLEAR.bit.GPIO133 = 1;}
#define DSP_GPIO133_O_DSP_STATUS_TOGGLE()						{GpioDataRegs.GPETOGGLE.bit.GPIO133 = 1;}

#define DSP_GPIO00_O_OUTPUT_MASTER_CHARGE_DATA 					GpioDataRegs.GPADAT.bit.GPIO0
#define DSP_GPIO01_O_OUTPUT_MASTER_DISCHARGE_DATA 				GpioDataRegs.GPADAT.bit.GPIO1
#define DSP_GPIO02_O_OUTPUT_SLAVE_CHARGE_DATA 					GpioDataRegs.GPADAT.bit.GPIO2
#define DSP_GPIO03_O_OUTPUT_SLAVE_DISCHARGE_DATA 				GpioDataRegs.GPADAT.bit.GPIO3
#define DSP_GPIO04_O_OPMODE_MASTER_SEL0_DATA 					GpioDataRegs.GPADAT.bit.GPIO4
#define DSP_GPIO05_O_OPMODE_MASTER_SEL1_DATA 					GpioDataRegs.GPADAT.bit.GPIO5
#define DSP_GPIO06_O_OPMODE_SLAVE_SEL0_DATA 					GpioDataRegs.GPADAT.bit.GPIO6
#define DSP_GPIO07_O_OPMODE_SLAVE_SEL1_DATA 					GpioDataRegs.GPADAT.bit.GPIO7
#define DSP_GPIO08_O_INTG_MASTER_SEL0_DATA 						GpioDataRegs.GPADAT.bit.GPIO8
#define DSP_GPIO09_O_INTG_MASTER_SEL1_DATA 						GpioDataRegs.GPADAT.bit.GPIO9
#define DSP_GPIO50_O_INTG_MASTER_SEL2_DATA 						GpioDataRegs.GPBDAT.bit.GPIO50
#define DSP_GPIO10_O_INTG_RESET_SEL0_DATA 						GpioDataRegs.GPADAT.bit.GPIO10
#define DSP_GPIO11_O_INTG_RESET_SEL1_DATA 						GpioDataRegs.GPADAT.bit.GPIO11
#define DSP_GPIO12_O_INTG_SLAVE_SEL0_DATA 						GpioDataRegs.GPADAT.bit.GPIO12
#define DSP_GPIO13_O_INTG_SLAVE_SEL1_DATA 						GpioDataRegs.GPADAT.bit.GPIO13
#define DSP_GPIO51_O_INTG_SLAVE_SEL2_DATA 						GpioDataRegs.GPBDAT.bit.GPIO51
#define DSP_GPIO52_O_INTG_CHARGE_SEL0_DATA 						GpioDataRegs.GPBDAT.bit.GPIO52
#define DSP_GPIO53_O_INTG_CHARGE_SEL1_DATA 						GpioDataRegs.GPBDAT.bit.GPIO53
#define DSP_GPIO33_O_EXT_SYNCH_MASTER_DATA 						GpioDataRegs.GPBDAT.bit.GPIO33
#define DSP_GPIO42_O_N_MASTER_DIS_EN1_DATA 						GpioDataRegs.GPBDAT.bit.GPIO42
#define DSP_GPIO43_O_N_MASTER_DIS_EN0_DATA 						GpioDataRegs.GPBDAT.bit.GPIO43
#define DSP_GPIO44_O_N_MASTER_CHG_EN1_DATA 						GpioDataRegs.GPBDAT.bit.GPIO44
#define DSP_GPIO45_O_N_MASTER_CHG_EN0_DATA 						GpioDataRegs.GPBDAT.bit.GPIO45
#define DSP_GPIO46_O_N_SLAVE_DIS_EN1_DATA 						GpioDataRegs.GPBDAT.bit.GPIO46
#define DSP_GPIO47_O_N_SLAVE_DIS_EN0_DATA 						GpioDataRegs.GPBDAT.bit.GPIO47
#define DSP_GPIO48_O_N_SLAVE_CHG_EN1_DATA 						GpioDataRegs.GPBDAT.bit.GPIO48
#define DSP_GPIO49_O_N_SLAVE_CHG_EN0_DATA 						GpioDataRegs.GPBDAT.bit.GPIO49
#define DSP_GPIO54_O_DIO_PORT_1_DATA 							GpioDataRegs.GPBDAT.bit.GPIO54
#define DSP_GPIO55_O_DIO_PORT_0_DATA 							GpioDataRegs.GPBDAT.bit.GPIO55
#define DSP_GPIO56_O_P_VOLATAGE_RELAY_DATA 						GpioDataRegs.GPBDAT.bit.GPIO56
#define DSP_GPIO57_O_N_VOLATAGE_RELAY_DATA 						GpioDataRegs.GPBDAT.bit.GPIO57
#define DSP_GPIO58_O_ADC_OS0_DATA 								GpioDataRegs.GPBDAT.bit.GPIO58
#define DSP_GPIO59_O_ADC_OS1_DATA 								GpioDataRegs.GPBDAT.bit.GPIO59
#define DSP_GPIO60_O_ADC_OS2_DATA 								GpioDataRegs.GPBDAT.bit.GPIO60
#define DSP_GPIO61_I_ADC_BUSY_DATA 								GpioDataRegs.GPBDAT.bit.GPIO61
#define DSP_GPIO62_O_ADC_N_CS_DATA 								GpioDataRegs.GPBDAT.bit.GPIO62
#define DSP_GPIO63_I_ADC_FRSTD_DATA 							GpioDataRegs.GPBDAT.bit.GPIO63
#define DSP_GPIO64_O_ADC_RESET_DATA 							GpioDataRegs.GPCDAT.bit.GPIO64
#define DSP_GPIO88_O_ADC_RANGE_DATA 							GpioDataRegs.GPCDAT.bit.GPIO88
#define DSP_GPIO65_I_EMERGENCY_IN_DATA 							GpioDataRegs.GPCDAT.bit.GPIO65
#define DSP_GPIO66_O_DAC_A0_DATA 								GpioDataRegs.GPCDAT.bit.GPIO66
#define DSP_GPIO67_O_DAC_A1_DATA 								GpioDataRegs.GPCDAT.bit.GPIO67
#define DSP_GPIO68_O_DAC_FINE_I_N_RESET_DATA 					GpioDataRegs.GPCDAT.bit.GPIO68
#define DSP_GPIO86_O_DAC_V_N_RESET_DATA 						GpioDataRegs.GPCDAT.bit.GPIO86
#define DSP_GPIO87_O_DAC_I_N_RESET_DATA 						GpioDataRegs.GPCDAT.bit.GPIO87
#define DSP_GPIO89_O_MASTER_RDAC_N_RESET_DATA 					GpioDataRegs.GPCDAT.bit.GPIO89
#define DSP_GPIO90_O_MASTER_RDAC_N_CS_DATA 						GpioDataRegs.GPCDAT.bit.GPIO90
#define DSP_GPIO91_O_SLAVE_RDAC_N_RESET_DATA 					GpioDataRegs.GPCDAT.bit.GPIO91
#define DSP_GPIO93_O_SLAVE_RDAC_N_CS_DATA 						GpioDataRegs.GPCDAT.bit.GPIO93
#define DSP_GPIO94_O_N_RLYON_1_DATA 							GpioDataRegs.GPCDAT.bit.GPIO94
#define DSP_GPIO99_O_N_RLYON_0_DATA 							GpioDataRegs.GPDDAT.bit.GPIO99
#define DSP_GPIO133_O_DSP_STATUS_DATA 							GpioDataRegs.GPEDAT.bit.GPIO133



void DSP_GpioInit(void);


#endif /* PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_GPIO_H_ */
