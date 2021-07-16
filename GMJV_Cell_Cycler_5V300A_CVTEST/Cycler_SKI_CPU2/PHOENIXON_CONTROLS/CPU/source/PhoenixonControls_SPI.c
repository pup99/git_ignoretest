/*
 * PhoenixonControls_SPI.C
 *
 *  Created on: 2018. 5. 20.
 *      Author: BGKim
 */
#include "../../../common/include/F28x_Project.h"
#include "../../../common/include/driverlib.h"
#include "../../../common/include/device.h"

#pragma CODE_SECTION(DSP_SpiWrite, ".TI.ramfunc");
#pragma CODE_SECTION(DSP_SpiRead, ".TI.ramfunc");

void DSP_SpiInit(void)
{
    //
    // Must put SPI into reset before configuring it
    //
    SPI_disableModule(SPIB_BASE);

    //
    // SPI configuration. Use a 1MHz SPICLK and 16-bit word size.
    //
    SPI_setConfig(SPIB_BASE, DEVICE_LSPCLK_FREQ, SPI_PROT_POL0PHA1,
                  SPI_MODE_MASTER, SPIB_BAUD_RATE, 16);
//    SPI_enableLoopback(SPIB_BASE);
    SPI_setEmulationMode(SPIB_BASE, SPI_EMULATION_STOP_AFTER_TRANSMIT);

    //
    // Configuration complete. Enable the module.
    //
    SPI_enableModule(SPIB_BASE);
}

Uint16 DSP_SpiWrite(Uint16 Data)
{
    SPI_writeDataNonBlocking(SPIB_BASE, Data);
    return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 DSP_SpiRead(Uint16 *Data)
{
	*Data = SPI_readDataBlockingNonFIFO(SPIB_BASE);
	return(SUCCESS); //SUCCESS Or FAIL
}
//
// End of file
//

