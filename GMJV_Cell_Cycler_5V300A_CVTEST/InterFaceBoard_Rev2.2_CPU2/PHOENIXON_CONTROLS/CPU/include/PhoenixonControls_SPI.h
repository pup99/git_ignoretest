/*
 * PhoenixonControls_SPI.h
 *
 *  Created on: 2018. 5. 20.
 *      Author: BGKim
 */

#ifndef PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_SPI_H_
#define PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_SPI_H_

#define SPIB_BAUD_RATE 10E06 //10Mbps
#define SPIB_SPIBRR ((Uint16)(((float32)(DEVICE_LSPCLK_FREQ)/(float32)(SPIA_BAUD_RATE)) - (float32)(1))) & 0X7F
//SPI Baud Rate = LSPCLK / (SPIBRR + 1)
//SPIBRR = (LSPCLK / SPI Baud Rate) - 1
//SPIBRR 7BIT REGISTER

void DSP_SpiInit(void);
Uint16 DSP_SpiWrite(Uint16 Data);
Uint16 DSP_SpiRead(Uint16 *Data);
#endif /* PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_SPI_H_ */
