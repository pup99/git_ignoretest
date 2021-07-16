/*
 * PhoenixonControls_EMIF.c
 *
 *  Created on: 2018. 5. 20.
 *      Author: BGKim
 */
#include "../../../common/include/F28x_Project.h"
#include "../../../common/include/driverlib.h"
#include "../../../common/include/device.h"

#pragma CODE_SECTION(DSP_EmifRead, ".TI.ramfunc");
#pragma CODE_SECTION(DSP_EmifWrite, ".TI.ramfunc");

extern void setup_emif1_pinmux_async_16bit(Uint16 cpu_sel);
extern void setup_emif1_pinmux_sdram_16bit(Uint16 cpu_sel);

void DSP_EmifInit(void)
{
        EMIF_SyncConfig sdConfig;
        EMIF_SyncTimingParams tParam;
	    EMIF_AsyncTimingParams tparam;

	    setup_emif1_pinmux_async_16bit(GPIO_MUX_CPU1);
	    setup_emif1_pinmux_sdram_16bit(GPIO_MUX_CPU1);

//	    Emif1Initialize();
//EMIF Setting Sync
	    //
	    // Configure to run EMIF1 on half Rate. (EMIF1CLK = CPU1SYSCLK/2)
	    //
	    SysCtl_setEMIF1ClockDivider(SYSCTL_EMIF1CLK_DIV_2);

	    //
	    // Grab EMIF1 For CPU1.
	    //
	    EMIF_selectMaster(EMIF1CONFIG_BASE, EMIF_MASTER_CPU1_G);

	    //
	    // Disable Access Protection. (CPU_FETCH/CPU_WR/DMA_WR)
	    //
	    EMIF_setAccessProtection(EMIF1CONFIG_BASE, 0x0);

	    //
	    // Commit the configuration related to protection. Till this bit remains
	    // set content of EMIF1ACCPROT0 register can't be changed.
	    //
	    EMIF_commitAccessConfig(EMIF1CONFIG_BASE);

	    //
	    // Lock the configuration so that EMIF1COMMIT register can't be changed
	    // any more.
	    //
	    EMIF_lockAccessConfig(EMIF1CONFIG_BASE);

	    // Configure SDRAM control registers. Needs to be
	    // programmed based on SDRAM Data-Sheet. For this example:
	    // T_RFC = 60ns = 0x6; T_RP  = 18ns = 0x1;
	    // T_RCD = 18ns = 0x1; T_WR  = 1CLK + 6 ns = 0x1;
	    // T_RAS = 42ns = 0x4; T_RC  = 60ns = 0x6;
	    // T_RRD = 12ns = 0x1.
	    //
//	    tParam.tRfc = 0x6U;
//	    tParam.tRp  = 0x1U;
//	    tParam.tRcd = 0x1U;
//	    tParam.tWr  = 0x1U;
//	    tParam.tRas = 0x4U;
//	    tParam.tRc  = 0x6U;
//	    tParam.tRrd = 0x1U;

	    tParam.tRfc = 0x1FU;
	    tParam.tRp  = 0x7U;
	    tParam.tRcd = 0x7U;
	    tParam.tWr  = 0x7U;
	    tParam.tRas = 0xFU;
	    tParam.tRc  = 0xFU;
	    tParam.tRrd = 0x7U;

	    EMIF_setSyncTimingParams(EMIF1_BASE, &tParam);

	    //
	    // Configure Self Refresh exit timing.
	    // Txsr = 70ns = 0x7.
	    //
	    EMIF_setSyncSelfRefreshExitTmng(EMIF1_BASE, 0x7U);
//	    EMIF_setSyncSelfRefreshExitTmng(EMIF1_BASE, 0x1FU);

	    //
	    // Configure Refresh Rate.
	    // Tref = 64ms for 8192 ROW, RR = 64000*100(Tfrq)/8192 = 781.25 (0x30E).
	    //
	    EMIF_setSyncRefreshRate(EMIF1_BASE, 781);
//	    EMIF_setSyncRefreshRate(EMIF1_BASE, 0x30E);

	    //
	    // Configure SDRAM parameters. PAGESIZE=2 (1024 elements per ROW),
	    // IBANK = 2 (4 BANK), CL = 3, NM = 1 (16bit).
	    //
//	    sdConfig.casLatency = EMIF_SYNC_CAS_LAT_3;
//	    sdConfig.iBank = EMIF_SYNC_BANK_4;
//	    sdConfig.narrowMode = EMIF_SYNC_NARROW_MODE_TRUE;
//	    sdConfig.pageSize = EMIF_SYNC_COLUMN_WIDTH_10;

	    sdConfig.casLatency = EMIF_SYNC_CAS_LAT_2;
	    sdConfig.iBank = EMIF_SYNC_BANK_4;
	    sdConfig.narrowMode = EMIF_SYNC_NARROW_MODE_TRUE;
	    sdConfig.pageSize = EMIF_SYNC_COLUMN_WIDTH_11;
	    EMIF_setSyncMemoryConfig(EMIF1_BASE, &sdConfig);
//EMIF Setting Async

	    //
	    // Configures Normal Asynchronous Mode of Operation.
	    //
	    EMIF_setAsyncMode(EMIF1_BASE, EMIF_ASYNC_CS2_OFFSET,
	                      EMIF_ASYNC_NORMAL_MODE);

	    //
	    // Disables Extended Wait Mode.
	    //
	    EMIF_disableAsyncExtendedWait(EMIF1_BASE, EMIF_ASYNC_CS2_OFFSET);

	    //
	    // Configure EMIF1 Data Bus Width.
	    //
	    EMIF_setAsyncDataBusWidth(EMIF1_BASE, EMIF_ASYNC_CS2_OFFSET,
	                              EMIF_ASYNC_DATA_WIDTH_16);

	    //
	    // Configure the access timing for CS2 space.
	    //
	    tparam.rSetup = 0;
	    tparam.rStrobe = 3;
	    tparam.rHold = 0;
	    tparam.turnArnd = 0;
	    tparam.wSetup = 0;
	    tparam.wStrobe = 1;
	    tparam.wHold = 0;

//	    tparam.rSetup 	= 16;		//Max 16
//	    tparam.rStrobe 	= 6;	    //Max 64
//	    tparam.rHold 	= 4;    	//Max 8
//	    tparam.turnArnd = 4;    	//Max 4
//	    tparam.wSetup 	= 15;      	//Max 16
//	    tparam.wStrobe 	= 10;    	//Max 64
//	    tparam.wHold 	= 4;       	//Max 8


	    EMIF_setAsyncTimingParams(EMIF1_BASE, EMIF_ASYNC_CS2_OFFSET, &tparam);

}
void setupEMIF1PinmuxAsync16Bit()
{
    Uint16 i;
    Uint16 cpu_sel = 0;

    for (i=28; i<=52; i++)
    {
        if ((i != 42) && (i != 43))
        {
            GPIO_SetupPinMux(i,cpu_sel,2);
        }
    }
    for (i=63; i<=87; i++)
    {
        if (i != 84)
        {
            GPIO_SetupPinMux(i,cpu_sel,2);
        }
    }

    GPIO_SetupPinMux(88,cpu_sel,3);
    GPIO_SetupPinMux(89,cpu_sel,3);
    GPIO_SetupPinMux(90,cpu_sel,3);
    GPIO_SetupPinMux(91,cpu_sel,3);
    GPIO_SetupPinMux(92,cpu_sel,3);
    GPIO_SetupPinMux(93,cpu_sel,3);
    GPIO_SetupPinMux(94,cpu_sel,2);

    //
    //setup async mode and enable pull-ups for Data pins
    //
    for (i=69; i<=85; i++)
    {
        if (i != 84)
        {
            GPIO_SetupPinOptions(i,0,0x31); // GPIO_ASYNC||GPIO_PULLUP
        }
    }
}

//
// setup_emif1_pinmux_async_32bit - Setup pinmux for 32bit async
//
void setupEMIF1PinmuxSync16Bit()
{
    Uint16 i;
    Uint16 cpu_sel = 0;
    for (i=28; i<=87; i++)
    {
        if ((i != 42) && (i != 43) && (i != 84))
        {
            GPIO_SetupPinMux(i,cpu_sel,2);
        }
    }

    GPIO_SetupPinMux(88,cpu_sel,3);
    GPIO_SetupPinMux(89,cpu_sel,3);
    GPIO_SetupPinMux(90,cpu_sel,3);
    GPIO_SetupPinMux(91,cpu_sel,3);
    GPIO_SetupPinMux(92,cpu_sel,3);
    GPIO_SetupPinMux(93,cpu_sel,3);
    GPIO_SetupPinMux(94,cpu_sel,2);

    //
    //setup async mode for Data pins
    //
    for (i=53; i<=85; i++)
    {
        if (i != 84)
        {
            GPIO_SetupPinOptions(i,0,0x31);
        }
    }
}
//
// Setup EMIF1 Pinmux Sync 16Bit - This function configures pins for 16bit
// Synchronous EMIF1.
//
//void setupEMIF1PinmuxSync16Bit(void)
//{
//    uint16_t i;
//
//    GPIO_setPinConfig(GPIO_29_EM1SDCKE);
//    GPIO_setPinConfig(GPIO_30_EM1CLK);
//    GPIO_setPinConfig(GPIO_31_EM1WEN);
//    GPIO_setPinConfig(GPIO_32_EM1CS0N);
//    GPIO_setPinConfig(GPIO_33_EM1RNW);
//    GPIO_setPinConfig(GPIO_34_EM1CS2N);
//    GPIO_setPinConfig(GPIO_35_EM1CS3N);
//    GPIO_setPinConfig(GPIO_36_EM1WAIT1);
//    GPIO_setPinConfig(GPIO_37_EM1OEN);
//
//    //
//    // Selecting address lines.
//    //
//    GPIO_setPinConfig(GPIO_38_EM1A0);
//    GPIO_setPinConfig(GPIO_39_EM1A1);
//    GPIO_setPinConfig(GPIO_40_EM1A2);
//    GPIO_setPinConfig(GPIO_41_EM1A3);
//    GPIO_setPinConfig(GPIO_44_EM1A4);
//    GPIO_setPinConfig(GPIO_45_EM1A5);
//    GPIO_setPinConfig(GPIO_46_EM1A6);
//    GPIO_setPinConfig(GPIO_47_EM1A7);
//    GPIO_setPinConfig(GPIO_48_EM1A8);
//    GPIO_setPinConfig(GPIO_49_EM1A9);
//    GPIO_setPinConfig(GPIO_50_EM1A10);
//    GPIO_setPinConfig(GPIO_51_EM1A11);
//    GPIO_setPinConfig(GPIO_52_EM1A12);
//
//    //
//    // Selecting Data Lines.
//    //
//    GPIO_setPinConfig(GPIO_69_EM1D15);
//    GPIO_setPinConfig(GPIO_70_EM1D14);
//    GPIO_setPinConfig(GPIO_71_EM1D13);
//    GPIO_setPinConfig(GPIO_72_EM1D12);
//    GPIO_setPinConfig(GPIO_73_EM1D11);
//    GPIO_setPinConfig(GPIO_74_EM1D10);
//    GPIO_setPinConfig(GPIO_75_EM1D9);
//    GPIO_setPinConfig(GPIO_76_EM1D8);
//    GPIO_setPinConfig(GPIO_77_EM1D7);
//    GPIO_setPinConfig(GPIO_78_EM1D6);
//    GPIO_setPinConfig(GPIO_79_EM1D5);
//    GPIO_setPinConfig(GPIO_80_EM1D4);
//    GPIO_setPinConfig(GPIO_81_EM1D3);
//    GPIO_setPinConfig(GPIO_82_EM1D2);
//    GPIO_setPinConfig(GPIO_83_EM1D1);
//    GPIO_setPinConfig(GPIO_85_EM1D0);
//
//    //
//    // Selecting RAS & CAS.
//    //
//    GPIO_setPinConfig(GPIO_86_EM1CAS);
//    GPIO_setPinConfig(GPIO_87_EM1RAS);
//
//    //
//    // Selecting DQM and Bank Select Lines.
//    //
//    GPIO_setPinConfig(GPIO_88_EM1DQM0);
//    GPIO_setPinConfig(GPIO_89_EM1DQM1);
//    GPIO_setPinConfig(GPIO_90_EM1DQM2);
//    GPIO_setPinConfig(GPIO_91_EM1DQM3);
//    GPIO_setPinConfig(GPIO_92_EM1BA1);
//    GPIO_setPinConfig(GPIO_93_EM1BA0);
//
//    //
//    // Configure Data pins for Async mode.
//    //
//    for(i = 69;i <= 85;i++)
//    {
//        if(i != 84)
//        {
//            GPIO_setPadConfig(i, GPIO_PIN_TYPE_PULLUP);
//            GPIO_setQualificationMode(i, GPIO_QUAL_ASYNC);
//        }
//    }
//
//    for(i = 88; i <= 91; i++)
//    {
//        GPIO_setPadConfig(i, GPIO_PIN_TYPE_PULLUP);
//        GPIO_setQualificationMode(i, GPIO_QUAL_ASYNC);
//    }
//
//}
////
//// Setup EMIF1 Pinmux Async 16Bit - This function configures pins for 16 bit
//// Asynchronous EMIF1.
////
//void setupEMIF1PinmuxAsync16Bit(void)
//{
//    uint16_t i;
//
//    GPIO_setPinConfig(GPIO_28_EM1CS4N);
//    GPIO_setPinConfig(GPIO_29_EM1SDCKE);
//    GPIO_setPinConfig(GPIO_30_EM1CLK);
//    GPIO_setPinConfig(GPIO_31_EM1WEN);
//    GPIO_setPinConfig(GPIO_32_EM1CS0N);
//    GPIO_setPinConfig(GPIO_33_EM1RNW);
//    GPIO_setPinConfig(GPIO_34_EM1CS2N);
//    GPIO_setPinConfig(GPIO_35_EM1CS3N);
//    GPIO_setPinConfig(GPIO_36_EM1WAIT1);
//    GPIO_setPinConfig(GPIO_37_EM1OEN);
//
//    //
//    // Selecting address lines.
//    //
//    GPIO_setPinConfig(GPIO_38_EM1A0);
//    GPIO_setPinConfig(GPIO_39_EM1A1);
//    GPIO_setPinConfig(GPIO_40_EM1A2);
//    GPIO_setPinConfig(GPIO_41_EM1A3);
//    GPIO_setPinConfig(GPIO_44_EM1A4);
//    GPIO_setPinConfig(GPIO_45_EM1A5);
//    GPIO_setPinConfig(GPIO_46_EM1A6);
//    GPIO_setPinConfig(GPIO_47_EM1A7);
//    GPIO_setPinConfig(GPIO_48_EM1A8);
//    GPIO_setPinConfig(GPIO_49_EM1A9);
//    GPIO_setPinConfig(GPIO_50_EM1A10);
//    GPIO_setPinConfig(GPIO_51_EM1A11);
//    GPIO_setPinConfig(GPIO_52_EM1A12);
//    GPIO_setPinConfig(GPIO_86_EM1A13);
//    GPIO_setPinConfig(GPIO_87_EM1A14);
//
//    //
//    // Selecting data lines.
//    //
//    GPIO_setPinConfig(GPIO_63_EM1D21);
//    GPIO_setPinConfig(GPIO_64_EM1D20);
//    GPIO_setPinConfig(GPIO_65_EM1D19);
//    GPIO_setPinConfig(GPIO_66_EM1D18);
//    GPIO_setPinConfig(GPIO_67_EM1D17);
//    GPIO_setPinConfig(GPIO_68_EM1D16);
//    GPIO_setPinConfig(GPIO_69_EM1D15);
//    GPIO_setPinConfig(GPIO_70_EM1D14);
//    GPIO_setPinConfig(GPIO_71_EM1D13);
//    GPIO_setPinConfig(GPIO_72_EM1D12);
//    GPIO_setPinConfig(GPIO_73_EM1D11);
//    GPIO_setPinConfig(GPIO_74_EM1D10);
//    GPIO_setPinConfig(GPIO_75_EM1D9);
//    GPIO_setPinConfig(GPIO_76_EM1D8);
//    GPIO_setPinConfig(GPIO_77_EM1D7);
//    GPIO_setPinConfig(GPIO_78_EM1D6);
//    GPIO_setPinConfig(GPIO_79_EM1D5);
//    GPIO_setPinConfig(GPIO_80_EM1D4);
//    GPIO_setPinConfig(GPIO_81_EM1D3);
//    GPIO_setPinConfig(GPIO_82_EM1D2);
//    GPIO_setPinConfig(GPIO_83_EM1D1);
//    GPIO_setPinConfig(GPIO_85_EM1D0);
//
//    //
//    // Setting DQM and Bank Select lines.
//    //
//    GPIO_setPinConfig(GPIO_88_EM1DQM0);
//    GPIO_setPinConfig(GPIO_89_EM1DQM1);
//    GPIO_setPinConfig(GPIO_90_EM1DQM2);
//    GPIO_setPinConfig(GPIO_91_EM1DQM3);
//    GPIO_setPinConfig(GPIO_92_EM1BA1);
//    GPIO_setPinConfig(GPIO_93_EM1BA0);
//    GPIO_setPinConfig(GPIO_94_EM1A21);
//
//    //
//    // Setup async mode and enable pull-ups for Data pins.
//    //
//    for(i=69; i<=85;i++)
//    {
//        if(i != 84)
//        {
//            GPIO_setPadConfig(i, GPIO_PIN_TYPE_PULLUP);
//            GPIO_setQualificationMode(i, GPIO_QUAL_ASYNC);
//        }
//    }
// }

Uint16 DSP_EmifRead(Uint32 ul_Address, Uint16* ui_Data)
{

	if(ul_Address < EMIF_CS2_BASE_ADDRESS ||
	   ul_Address > EMIF_CS2_END_ADDRESS)
	{
		return(FAIL); //SUCCESS Or FAIL
	}

	*ui_Data = *((Uint32 *)(ul_Address));

	return(SUCCESS); //SUCCESS Or FAIL
}

Uint16 DSP_EmifWrite(Uint32 ul_Address, Uint16 ui_Data)
{

	if(ul_Address < EMIF_CS2_BASE_ADDRESS ||
	   ul_Address > EMIF_CS2_END_ADDRESS)
	{
		return(FAIL); //SUCCESS Or FAIL
	}

	(*(Uint16 *)(ul_Address)) = ui_Data;
	return(SUCCESS);
}

//
// End of file
//

