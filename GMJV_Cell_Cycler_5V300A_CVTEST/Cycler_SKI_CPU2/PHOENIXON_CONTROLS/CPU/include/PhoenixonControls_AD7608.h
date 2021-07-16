/*
 * PhoenixonControls_AD7608.h
 *
 *  Created on: 2018. 5. 20.
 *      Author: BGKim
 */

#ifndef PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_AD7608_H_
#define PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_AD7608_H_

#define AD7608_GAIN_5V 	  	(float)(38.14697266E-6)// Ex) 5(VIN) = 131072( 5V Sampling Code) * 38.14697266E-6( 5V/Sampling bit)
#define AD7608_GAIN_10V	  	(float)(76.29394531E-6)// Ex)10(VIN) = 131072(10V Sampling Code) * 76.29394531E-6(10V/Sampling bit)

#define AD7608_OS0_SET		DSP_GPIO58_O_ADC_OS0_SET()
#define AD7608_OS1_SET		DSP_GPIO59_O_ADC_OS1_SET()
#define AD7608_OS2_SET		DSP_GPIO60_O_ADC_OS2_SET()

#define AD7608_OS0_CLEAR	DSP_GPIO58_O_ADC_OS0_CLEAR()
#define AD7608_OS1_CLEAR	DSP_GPIO59_O_ADC_OS1_CLEAR()
#define AD7608_OS2_CLEAR	DSP_GPIO60_O_ADC_OS2_CLEAR()

#define AD7608_BUSY			DSP_GPIO61_I_ADC_BUSY()

#define AD7608_N_CS_SET		DSP_GPIO62_O_ADC_N_CS_SET()
#define AD7608_N_CS_CLEAR	DSP_GPIO62_O_ADC_N_CS_CLEAR()

#define AD7608_FRSTD		DSP_GPIO63_I_ADC_FRSTD()

#define AD7608_RESET_SET	DSP_GPIO64_O_ADC_RESET_SET()
#define AD7608_RESET_CLEAR	DSP_GPIO64_O_ADC_RESET_CLEAR()

#define AD7608_RANGE_SET	DSP_GPIO88_O_ADC_RANGE_SET()
#define AD7608_RANGE_CLEAR	DSP_GPIO88_O_ADC_RANGE_CLEAR()

#define AD7608_CONVERSION_ADDRESS (Uint32)(0x100011)	//y1 = (Uint16)(0x110001) ADC CNVST
#define AD7608_READ_DATA_ADDRESS  (Uint32)(0x100010)	//y0 = (Uint16)(0x110000) ADC nXRD
#define AD7608_CONVERSION	{DSP_EmifWrite(AD7608_CONVERSION_ADDRESS, 0xFFFF);}
#define AD7608_READ_DATA(X)	{DSP_EmifRead(AD7608_READ_DATA_ADDRESS, X);}

typedef enum
{
	AD7608_NON,
	AD7608_02,
	AD7608_04,
	AD7608_08,
	AD7608_16,
	AD7608_32,
	AD7608_64
}E_AD7608_OVER_SAMPLING_SELECT;

typedef enum
{
	AD7608_05_VOLTAGE,
	AD7608_10_VOLTAGE,
}E_AD7608_VOLTAGE_RANGE_SELECT;

void AD7608_Init(void);
void AD7608_Reset(void);
Uint16 AD7608_Conversion(void);
Uint16 AD7608_FastDataCheck(void);
Uint16 AD7608_BusyCheck(void);
Uint16 AD7608_OverSamplingSelect(E_AD7608_OVER_SAMPLING_SELECT e_OverSampling);
Uint16 AD7608_VoltageRangeSelect(E_AD7608_VOLTAGE_RANGE_SELECT e_Voltage);
Uint16 AD7608_Read(E_AD7608_VOLTAGE_RANGE_SELECT e_Voltage, E_AD7608_OVER_SAMPLING_SELECT e_OverSampling, float32 *Data);

#endif /* PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_AD7608_H_ */
