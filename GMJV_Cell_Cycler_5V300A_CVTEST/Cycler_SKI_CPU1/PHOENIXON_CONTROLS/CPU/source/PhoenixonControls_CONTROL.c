/*
 * PhoenixonControls_CONTROL.c
 *
 *  Created on: 2018. 5. 20.
 *      Author: BGKim
 */
#include "../../../common/include/F28x_Project.h"
#include "../../../common/include/driverlib.h"
#include "../../../common/include/device.h"

#pragma CODE_SECTION(CONTROL_VoltageRELAY, ".TI.ramfunc");
#pragma CODE_SECTION(CONTROL_CurrentRELAY, ".TI.ramfunc");
#pragma CODE_SECTION(CONTROL_DIORELAY, ".TI.ramfunc");
#pragma CODE_SECTION(CONTROL_OperationMUX, ".TI.ramfunc");
#pragma CODE_SECTION(CONTROL_IntegrationResetMUX, ".TI.ramfunc");
#pragma CODE_SECTION(CONTROL_IntegrationSlopMUX, ".TI.ramfunc");
#pragma CODE_SECTION(CONTROL_IntegrationSlopPMDAC, ".TI.ramfunc");
#pragma CODE_SECTION(CONTROL_IntegrationSlopCapMUX, ".TI.ramfunc");
#pragma CODE_SECTION(CONTROL_GateOffsetDAC, ".TI.ramfunc");
#pragma CODE_SECTION(CONTROL_GateOutputMUX, ".TI.ramfunc");
#pragma CODE_SECTION(CONTROL_GateOutputAMP, ".TI.ramfunc");
#pragma CODE_SECTION(CONTROL_VoltageReferenceDAC, ".TI.ramfunc");
#pragma CODE_SECTION(CONTROL_CurrentReferenceDAC, ".TI.ramfunc");
#pragma CODE_SECTION(CONTROL_FINECurrentReferenceDAC, ".TI.ramfunc");

void CONTROL_Init(void)
{

}

S_CONTROL_VRELAY CONTROL_VoltageRELAY(E_CONTROL_COMMAND e_Command)
{
    volatile static S_CONTROL_VRELAY s_Data = { CONTROL_OFF, CONTROL_COMMAND_INIT };

    s_Data.e_Command = e_Command;

    if (s_Data.e_OldCommand != s_Data.e_Command) {
        if (s_Data.e_Command == CONTROL_ON) {
            DSP_GPIO56_O_P_VOLATAGE_RELAY_SET()
            DSP_GPIO57_O_N_VOLATAGE_RELAY_SET()
            DELAY_US(10000);
        } else {
            DSP_GPIO56_O_P_VOLATAGE_RELAY_CLEAR()
            DSP_GPIO57_O_N_VOLATAGE_RELAY_CLEAR()
            DELAY_US(5000);
        }

        s_Data.e_OldCommand = s_Data.e_Command;
    }

    return (s_Data);  //SUCCESS Or FAIL
}

S_CONTROL_RELAY CONTROL_CurrentRELAY(E_CONTROL_CHANNEL_SELECT e_Channel, E_CONTROL_COMMAND e_Command)
{
    volatile static S_CONTROL_RELAY s_Data = { CONTROL_ALL, CONTROL_OFF, CONTROL_COMMAND_INIT };

    s_Data.e_Command = e_Command;

    if (s_Data.e_OldCommand != s_Data.e_Command) {
        s_Data.e_Channel = e_Channel;

        switch (s_Data.e_Channel)
        {
            case CONTROL_MASTER:
                if (s_Data.e_Command == CONTROL_ON) DSP_GPIO99_O_N_RLYON_0_SET()
                else DSP_GPIO99_O_N_RLYON_0_CLEAR()
                break;
            case CONTROL_SLAVE:
                if (s_Data.e_Command == CONTROL_ON) DSP_GPIO94_O_N_RLYON_1_SET()
                else DSP_GPIO94_O_N_RLYON_1_CLEAR()
                break;
            case CONTROL_ALL:
            default:
                if (s_Data.e_Command == CONTROL_ON) {
                    DSP_GPIO99_O_N_RLYON_0_SET()
                    DSP_GPIO94_O_N_RLYON_1_SET()
                } else {
                    DSP_GPIO99_O_N_RLYON_0_CLEAR()
                    DSP_GPIO94_O_N_RLYON_1_CLEAR()
                }
                break;
        }

        DELAY_US(10000);
        s_Data.e_OldCommand = s_Data.e_Command;
    }

    return (s_Data);  //SUCCESS Or FAIL
}

S_CONTROL_DIO CONTROL_DIORELAY(E_CONTROL_DIO_CHANNEL_SELECT e_Channel, E_CONTROL_COMMAND e_Command)
{
    volatile static S_CONTROL_DIO s_Data = { CONTROL_DIO_ALL, CONTROL_OFF, CONTROL_COMMAND_INIT };

    s_Data.e_Command = e_Command;

    if (s_Data.e_OldCommand != s_Data.e_Command) {
        s_Data.e_Channel = e_Channel;

        switch (s_Data.e_Channel)
        {
            case CONTROL_DIO_00:
                if (s_Data.e_Command == CONTROL_ON) DSP_GPIO55_O_DIO_PORT_0_SET()
                else DSP_GPIO55_O_DIO_PORT_0_CLEAR()
                break;
            case CONTROL_DIO_01:
                if (s_Data.e_Command == CONTROL_ON) DSP_GPIO54_O_DIO_PORT_1_SET()
                else DSP_GPIO54_O_DIO_PORT_1_CLEAR()
                break;
            case CONTROL_DIO_ALL:
            default:
                if (s_Data.e_Command == CONTROL_ON) {
                    DSP_GPIO55_O_DIO_PORT_0_SET()
                    DSP_GPIO54_O_DIO_PORT_1_SET()
                } else {
                    DSP_GPIO55_O_DIO_PORT_0_CLEAR()
                    DSP_GPIO54_O_DIO_PORT_1_CLEAR()
                }
                break;
        }

        DELAY_US(10000);
        s_Data.e_OldCommand = s_Data.e_Command;
    }

    return (s_Data);  //SUCCESS Or FAIL
}

S_CONTROL_MUX CONTROL_OperationMUX(E_CONTROL_CHANNEL_SELECT e_Channel, E_CONTROL_OPERATION_MODE_SELECT e_Mode)
{
    volatile static S_CONTROL_MUX s_Data = { CONTROL_ALL, CONTROL_REST, CONTROL_OPERATION_INIT };

    s_Data.e_Mode = e_Mode;

    if (s_Data.e_OldMode != s_Data.e_Mode) {
        s_Data.e_Channel = e_Channel;

        switch (s_Data.e_Channel)
        {
            case CONTROL_MASTER:
                if (s_Data.e_Mode == CONTROL_CHARGE) {
                    DSP_GPIO04_O_OPMODE_MASTER_SEL0_CLEAR()
                    DSP_GPIO05_O_OPMODE_MASTER_SEL1_CLEAR()
                    DSP_GPIO06_O_OPMODE_SLAVE_SEL0_CLEAR()
                    DSP_GPIO07_O_OPMODE_SLAVE_SEL1_CLEAR()
                    DELAY_US(1);
                    DSP_GPIO04_O_OPMODE_MASTER_SEL0_SET()
                } else if (s_Data.e_Mode == CONTROL_DISCHARGE) {
                    DSP_GPIO04_O_OPMODE_MASTER_SEL0_CLEAR()
                    DSP_GPIO05_O_OPMODE_MASTER_SEL1_CLEAR()
                    DSP_GPIO06_O_OPMODE_SLAVE_SEL0_CLEAR()
                    DSP_GPIO07_O_OPMODE_SLAVE_SEL1_CLEAR()
                    DELAY_US(1);
                    DSP_GPIO05_O_OPMODE_MASTER_SEL1_SET()
                } else {
                    DSP_GPIO04_O_OPMODE_MASTER_SEL0_CLEAR()
                    DSP_GPIO05_O_OPMODE_MASTER_SEL1_CLEAR()
                    DSP_GPIO06_O_OPMODE_SLAVE_SEL0_CLEAR()
                    DSP_GPIO07_O_OPMODE_SLAVE_SEL1_CLEAR()
                }
                break;
            case CONTROL_SLAVE:
                if (s_Data.e_Mode == CONTROL_CHARGE) {
                    DSP_GPIO04_O_OPMODE_MASTER_SEL0_CLEAR()
                    DSP_GPIO05_O_OPMODE_MASTER_SEL1_CLEAR()
                    DSP_GPIO06_O_OPMODE_SLAVE_SEL0_CLEAR()
                    DSP_GPIO07_O_OPMODE_SLAVE_SEL1_CLEAR()
                    DELAY_US(1);
                    DSP_GPIO06_O_OPMODE_SLAVE_SEL0_SET()
                } else if (s_Data.e_Mode == CONTROL_DISCHARGE) {
                    DSP_GPIO04_O_OPMODE_MASTER_SEL0_CLEAR()
                    DSP_GPIO05_O_OPMODE_MASTER_SEL1_CLEAR()
                    DSP_GPIO06_O_OPMODE_SLAVE_SEL0_CLEAR()
                    DSP_GPIO07_O_OPMODE_SLAVE_SEL1_CLEAR()
                    DELAY_US(1);
                    DSP_GPIO07_O_OPMODE_SLAVE_SEL1_SET()
                } else {
                    DSP_GPIO04_O_OPMODE_MASTER_SEL0_CLEAR()
                    DSP_GPIO05_O_OPMODE_MASTER_SEL1_CLEAR()
                    DSP_GPIO06_O_OPMODE_SLAVE_SEL0_CLEAR()
                    DSP_GPIO07_O_OPMODE_SLAVE_SEL1_CLEAR()
                }
                break;
            case CONTROL_ALL:
            default:
                if (s_Data.e_Mode == CONTROL_CHARGE) {
                    DSP_GPIO04_O_OPMODE_MASTER_SEL0_CLEAR()
                    DSP_GPIO05_O_OPMODE_MASTER_SEL1_CLEAR()
                    DSP_GPIO06_O_OPMODE_SLAVE_SEL0_CLEAR()
                    DSP_GPIO07_O_OPMODE_SLAVE_SEL1_CLEAR()
                    DELAY_US(1);
                    DSP_GPIO04_O_OPMODE_MASTER_SEL0_SET()
                    DSP_GPIO06_O_OPMODE_SLAVE_SEL0_SET()
                } else if (s_Data.e_Mode == CONTROL_DISCHARGE) {
                    DSP_GPIO04_O_OPMODE_MASTER_SEL0_CLEAR()
                    DSP_GPIO05_O_OPMODE_MASTER_SEL1_CLEAR()
                    DSP_GPIO06_O_OPMODE_SLAVE_SEL0_CLEAR()
                    DSP_GPIO07_O_OPMODE_SLAVE_SEL1_CLEAR()
                    DELAY_US(1);
                    DSP_GPIO05_O_OPMODE_MASTER_SEL1_SET()
                    DSP_GPIO07_O_OPMODE_SLAVE_SEL1_SET()
                } else {
                    DSP_GPIO04_O_OPMODE_MASTER_SEL0_CLEAR()
                    DSP_GPIO05_O_OPMODE_MASTER_SEL1_CLEAR()
                    DSP_GPIO06_O_OPMODE_SLAVE_SEL0_CLEAR()
                    DSP_GPIO07_O_OPMODE_SLAVE_SEL1_CLEAR()
                }
                break;
        }
        DELAY_US(1);
        s_Data.e_OldMode = s_Data.e_Mode;
    }

    return (s_Data);  //SUCCESS Or FAIL
}

S_CONTROL_INTEGRATION_RESET_MUX CONTROL_IntegrationResetMUX(E_CONTROL_CHANNEL_SELECT e_Channel, E_CONTROL_COMMAND e_Command)
{
    volatile static S_CONTROL_INTEGRATION_RESET_MUX s_Data = { CONTROL_ALL, CONTROL_OFF, CONTROL_COMMAND_INIT };

    s_Data.e_Command = e_Command;

    if (s_Data.e_OldCommand != s_Data.e_Command) {
        s_Data.e_Channel = e_Channel;

        switch (s_Data.e_Channel)
        {
            case CONTROL_MASTER:
                if (s_Data.e_Command == CONTROL_ON) DSP_GPIO10_O_INTG_RESET_SEL0_SET()
                else DSP_GPIO10_O_INTG_RESET_SEL0_CLEAR()
                break;
            case CONTROL_SLAVE:
                if (s_Data.e_Command == CONTROL_ON) DSP_GPIO11_O_INTG_RESET_SEL1_SET()
                else DSP_GPIO11_O_INTG_RESET_SEL1_CLEAR()
                break;
            case CONTROL_ALL:
            default:
                if (s_Data.e_Command == CONTROL_ON) {
                    DSP_GPIO10_O_INTG_RESET_SEL0_SET()
                    DSP_GPIO11_O_INTG_RESET_SEL1_SET()
                } else {
                    DSP_GPIO10_O_INTG_RESET_SEL0_CLEAR()
                    DSP_GPIO11_O_INTG_RESET_SEL1_CLEAR()
                }
                break;
        }
        DELAY_US(1);
        s_Data.e_OldCommand = s_Data.e_Command;
    }

    return (s_Data);  //SUCCESS Or FAIL
}

S_CONTROL_INTEGRATION_SOLP_MUX CONTROL_IntegrationSlopMUX(E_CONTROL_CHANNEL_SELECT e_Channel, E_CONTROL_SLOP_SELECT e_Slop)
{
    volatile static S_CONTROL_INTEGRATION_SOLP_MUX s_Data = { CONTROL_ALL, CONTROL_GND, CONTROL_SLOP_INIT };

    s_Data.e_Slop = e_Slop;

    if (s_Data.e_OldSlop != s_Data.e_Slop) {
        s_Data.e_Channel = e_Channel;

        switch (s_Data.e_Channel)
        {
            case CONTROL_MASTER:
                if (s_Data.e_Slop & 0x1) DSP_GPIO08_O_INTG_MASTER_SEL0_SET()
                else DSP_GPIO08_O_INTG_MASTER_SEL0_CLEAR()

                if (s_Data.e_Slop & 0x2) DSP_GPIO09_O_INTG_MASTER_SEL1_SET()
                else DSP_GPIO09_O_INTG_MASTER_SEL1_CLEAR()

                if (s_Data.e_Slop & 0x4) DSP_GPIO50_O_INTG_MASTER_SEL2_SET()
                else DSP_GPIO50_O_INTG_MASTER_SEL2_CLEAR()
                break;
            case CONTROL_SLAVE:
                if (s_Data.e_Slop & 0x1) DSP_GPIO12_O_INTG_SLAVE_SEL0_SET()
                else DSP_GPIO12_O_INTG_SLAVE_SEL0_CLEAR()

                if (s_Data.e_Slop & 0x2) DSP_GPIO13_O_INTG_SLAVE_SEL1_SET()
                else DSP_GPIO13_O_INTG_SLAVE_SEL1_CLEAR()

                if (s_Data.e_Slop & 0x4) DSP_GPIO51_O_INTG_SLAVE_SEL2_SET()
                else DSP_GPIO51_O_INTG_SLAVE_SEL2_CLEAR()
                break;
            case CONTROL_ALL:
            default:
                //Master
                if (s_Data.e_Slop & 0x1) DSP_GPIO08_O_INTG_MASTER_SEL0_SET()
                else DSP_GPIO08_O_INTG_MASTER_SEL0_CLEAR()

                if (s_Data.e_Slop & 0x2) DSP_GPIO09_O_INTG_MASTER_SEL1_SET()
                else DSP_GPIO09_O_INTG_MASTER_SEL1_CLEAR()

                if (s_Data.e_Slop & 0x4) DSP_GPIO50_O_INTG_MASTER_SEL2_SET()
                else DSP_GPIO50_O_INTG_MASTER_SEL2_CLEAR()

                //Slave
                if (s_Data.e_Slop & 0x1) DSP_GPIO12_O_INTG_SLAVE_SEL0_SET()
                else DSP_GPIO12_O_INTG_SLAVE_SEL0_CLEAR()

                if (s_Data.e_Slop & 0x2) DSP_GPIO13_O_INTG_SLAVE_SEL1_SET()
                else DSP_GPIO13_O_INTG_SLAVE_SEL1_CLEAR()

                if (s_Data.e_Slop & 0x4) DSP_GPIO51_O_INTG_SLAVE_SEL2_SET()
                else DSP_GPIO51_O_INTG_SLAVE_SEL2_CLEAR()
                break;
        }

        s_Data.e_OldSlop = s_Data.e_Slop;
    }

	return(s_Data); //SUCCESS Or FAIL
}

S_CONTROL_DAC CONTROL_IntegrationSlopPMDAC(E_CONTROL_CHANNEL_SELECT e_Channel, float32 f_PotentialValue)
{
    volatile static S_CONTROL_DAC s_Data = { CONTROL_ALL, CONTROL_ALL, 10000.0, 0.0 };

    Uint16 ui_Value = 0;
    Uint32 ul_Temp = 0;

    s_Data.f_Value = f_PotentialValue;
    s_Data.e_Channel = e_Channel;

    if (s_Data.f_Value != s_Data.f_OldValue || s_Data.e_OldChannel != s_Data.e_Channel) {
        ul_Temp = (Uint32) (s_Data.f_Value / (float32) AD5293_RESOLUTION);
        if (ul_Temp >= AD5293_DIGIT) ul_Temp = AD5293_MAX_VLAUE;
        ui_Value = (Uint16) ((~ul_Temp) & 0x000003FF);

        s_Data.e_Channel = e_Channel;

        switch (s_Data.e_Channel)
        {
            case CONTROL_MASTER:
                AD5293_Write(AD5293_MASTER, AD5293_COMMMAND_WRITE_DATA, ui_Value);
                AD5293_Write(AD5293_SLAVE, AD5293_COMMMAND_WRITE_DATA, 0);
                break;
            case CONTROL_SLAVE:
                AD5293_Write(AD5293_MASTER, AD5293_COMMMAND_WRITE_DATA, 0);
                AD5293_Write(AD5293_SLAVE, AD5293_COMMMAND_WRITE_DATA, ui_Value);
                break;
            case CONTROL_ALL:
            default:
                AD5293_Write(AD5293_MASTER, AD5293_COMMMAND_WRITE_DATA, ui_Value);
                AD5293_Write(AD5293_SLAVE, AD5293_COMMMAND_WRITE_DATA, ui_Value);
                break;
        }
        s_Data.f_OldValue = s_Data.f_Value;
        s_Data.e_OldChannel = s_Data.e_Channel;
    }

    return (s_Data);  //SUCCESS Or FAIL
}

S_CONTROL_INTEGRATION_SOLP_CAP_MUX CONTROL_IntegrationSlopCapMUX(E_CONTROL_SLOP_CAP_SELECT e_Slop_Cap)
{
    volatile static S_CONTROL_INTEGRATION_SOLP_CAP_MUX s_Data = { (E_CONTROL_SLOP_CAP_SELECT) 0xFFFF, (E_CONTROL_SLOP_CAP_SELECT) 0xFFFF };

    s_Data.e_Slop_Cap = e_Slop_Cap;
#if 1
    if(s_Data.e_Slop_Cap != s_Data.e_OldSlop_Cap) {
        switch (s_Data.e_Slop_Cap)
        {
            case CONTROL_CAP_GND:   //S1, 000
                DSP_GPIO35_O_INTG_CAP_SEL0_CLEAR()
                DSP_GPIO36_O_INTG_CAP_SEL1_CLEAR()
                DSP_GPIO38_O_INTG_CAP_SEL2_CLEAR()
                break;
            case CONTROL_CAP_OPEN:  //S2, 001
                DSP_GPIO35_O_INTG_CAP_SEL0_SET()
                DSP_GPIO36_O_INTG_CAP_SEL1_CLEAR()
                DSP_GPIO38_O_INTG_CAP_SEL2_CLEAR()
                break;
            case CONTROL_CAP_33nF:  //S3, 010
                DSP_GPIO35_O_INTG_CAP_SEL0_CLEAR()
                DSP_GPIO36_O_INTG_CAP_SEL1_SET()
                DSP_GPIO38_O_INTG_CAP_SEL2_CLEAR()
                break;
            case CONTROL_CAP_47nF:  //S4, 011
                DSP_GPIO35_O_INTG_CAP_SEL0_SET()
                DSP_GPIO36_O_INTG_CAP_SEL1_SET()
                DSP_GPIO38_O_INTG_CAP_SEL2_CLEAR()
                break;
            case CONTROL_CAP_68nF:  //S5, 100
                DSP_GPIO35_O_INTG_CAP_SEL0_CLEAR()
                DSP_GPIO36_O_INTG_CAP_SEL1_CLEAR()
                DSP_GPIO38_O_INTG_CAP_SEL2_SET()
                break;
            case CONTROL_CAP_100nF: //S6, 101
                DSP_GPIO35_O_INTG_CAP_SEL0_SET()
                DSP_GPIO36_O_INTG_CAP_SEL1_CLEAR()
                DSP_GPIO38_O_INTG_CAP_SEL2_SET()
                break;
            case CONTROL_CAP_220nF: //S7, 110
                DSP_GPIO35_O_INTG_CAP_SEL0_CLEAR()
                DSP_GPIO36_O_INTG_CAP_SEL1_SET()
                DSP_GPIO38_O_INTG_CAP_SEL2_SET()
                break;
        }
        DELAY_US(1);
        s_Data.e_OldSlop_Cap = s_Data.e_Slop_Cap;
    }
#endif

    return (s_Data);
}

S_CONTROL_VDAC CONTROL_GateOffsetDAC(float32 f_VolatageValue)
{
    volatile static S_CONTROL_VDAC s_Data = { 0.0, -1.0 };

    Uint16 ui_Temp = 0;
    float32 f_Temp = 0.0;

    s_Data.f_Value = f_VolatageValue;

    if (s_Data.f_OldValue != s_Data.f_Value) {
        f_Temp = s_Data.f_Value * CONTROL_INTEGRATION_VOLTAGE_GAIN;
#if (PCB_VERSION == 220)
        ui_Temp = (unsigned int) ((f_Temp / AD5547_BIPOLAR_GAIN ) + AD5547_ZERO_VOLTAGE_BIPOLAR );
#elif (PCB_VERSION == 230) || (PCB_VERSION == 231)
        ui_Temp = (unsigned int) ((f_Temp / AD5547_BIPOLAR_GAIN2 ) + AD5547_ZERO_VOLTAGE_BIPOLAR );
#endif

        AD5547_Write(AD5547_INTEGRATION_VOLTAGE, ui_Temp);

        s_Data.f_OldValue = s_Data.f_Value;
    }
    return (s_Data);  //SUCCESS Or FAIL
}

S_CONTROL_MUX CONTROL_GateOutputMUX(E_CONTROL_CHANNEL_SELECT e_Channel, E_CONTROL_OPERATION_MODE_SELECT e_Mode)
{
    volatile static S_CONTROL_MUX s_Data = { CONTROL_ALL, CONTROL_REST, CONTROL_OPERATION_INIT };

    s_Data.e_Mode = e_Mode;

    if (s_Data.e_OldMode != s_Data.e_Mode) {
        s_Data.e_Channel = e_Channel;

        switch (s_Data.e_Channel)
        {
            case CONTROL_MASTER:
                if (s_Data.e_Mode == CONTROL_CHARGE) {
                    DSP_GPIO00_O_OUTPUT_MASTER_CHARGE_CLEAR()
                    DSP_GPIO01_O_OUTPUT_MASTER_DISCHARGE_CLEAR()
                    DSP_GPIO02_O_OUTPUT_SLAVE_CHARGE_CLEAR()
                    DSP_GPIO03_O_OUTPUT_SLAVE_DISCHARGE_CLEAR()
                    DELAY_US(1);
                    DSP_GPIO00_O_OUTPUT_MASTER_CHARGE_SET()
                } else if (s_Data.e_Mode == CONTROL_DISCHARGE) {
                    DSP_GPIO00_O_OUTPUT_MASTER_CHARGE_CLEAR()
                    DSP_GPIO01_O_OUTPUT_MASTER_DISCHARGE_CLEAR()
                    DSP_GPIO02_O_OUTPUT_SLAVE_CHARGE_CLEAR()
                    DSP_GPIO03_O_OUTPUT_SLAVE_DISCHARGE_CLEAR()
                    DELAY_US(1);
                    DSP_GPIO01_O_OUTPUT_MASTER_DISCHARGE_SET()
                } else {
                    DSP_GPIO00_O_OUTPUT_MASTER_CHARGE_CLEAR()
                    DSP_GPIO01_O_OUTPUT_MASTER_DISCHARGE_CLEAR()
                    DSP_GPIO02_O_OUTPUT_SLAVE_CHARGE_CLEAR()
                    DSP_GPIO03_O_OUTPUT_SLAVE_DISCHARGE_CLEAR()
                }
                break;
            case CONTROL_SLAVE:
                if (s_Data.e_Mode == CONTROL_CHARGE) {
                    DSP_GPIO00_O_OUTPUT_MASTER_CHARGE_CLEAR()
                    DSP_GPIO01_O_OUTPUT_MASTER_DISCHARGE_CLEAR()
                    DSP_GPIO02_O_OUTPUT_SLAVE_CHARGE_CLEAR()
                    DSP_GPIO03_O_OUTPUT_SLAVE_DISCHARGE_CLEAR()
                    DELAY_US(1);
                    DSP_GPIO02_O_OUTPUT_SLAVE_CHARGE_SET()
                } else if (s_Data.e_Mode == CONTROL_DISCHARGE) {
                    DSP_GPIO00_O_OUTPUT_MASTER_CHARGE_CLEAR()
                    DSP_GPIO01_O_OUTPUT_MASTER_DISCHARGE_CLEAR()
                    DSP_GPIO02_O_OUTPUT_SLAVE_CHARGE_CLEAR()
                    DSP_GPIO03_O_OUTPUT_SLAVE_DISCHARGE_CLEAR()
                    DELAY_US(1);
                    DSP_GPIO03_O_OUTPUT_SLAVE_DISCHARGE_SET()
                } else {
                    DSP_GPIO00_O_OUTPUT_MASTER_CHARGE_CLEAR()
                    DSP_GPIO01_O_OUTPUT_MASTER_DISCHARGE_CLEAR()
                    DSP_GPIO02_O_OUTPUT_SLAVE_CHARGE_CLEAR()
                    DSP_GPIO03_O_OUTPUT_SLAVE_DISCHARGE_CLEAR()
                }
                break;
            case CONTROL_ALL:
            default:
                if (s_Data.e_Mode == CONTROL_CHARGE) {
                    DSP_GPIO00_O_OUTPUT_MASTER_CHARGE_CLEAR()
                    DSP_GPIO01_O_OUTPUT_MASTER_DISCHARGE_CLEAR()
                    DSP_GPIO02_O_OUTPUT_SLAVE_CHARGE_CLEAR()
                    DSP_GPIO03_O_OUTPUT_SLAVE_DISCHARGE_CLEAR()
                    DELAY_US(1);
                    DSP_GPIO00_O_OUTPUT_MASTER_CHARGE_SET()
                    DSP_GPIO02_O_OUTPUT_SLAVE_CHARGE_SET()
                } else if (s_Data.e_Mode == CONTROL_DISCHARGE) {
                    DSP_GPIO00_O_OUTPUT_MASTER_CHARGE_CLEAR()
                    DSP_GPIO01_O_OUTPUT_MASTER_DISCHARGE_CLEAR()
                    DSP_GPIO02_O_OUTPUT_SLAVE_CHARGE_CLEAR()
                    DSP_GPIO03_O_OUTPUT_SLAVE_DISCHARGE_CLEAR()
                    DELAY_US(1);
                    DSP_GPIO01_O_OUTPUT_MASTER_DISCHARGE_SET()
                    DSP_GPIO03_O_OUTPUT_SLAVE_DISCHARGE_SET()
                } else {
                    DSP_GPIO00_O_OUTPUT_MASTER_CHARGE_CLEAR()
                    DSP_GPIO01_O_OUTPUT_MASTER_DISCHARGE_CLEAR()
                    DSP_GPIO02_O_OUTPUT_SLAVE_CHARGE_CLEAR()
                    DSP_GPIO03_O_OUTPUT_SLAVE_DISCHARGE_CLEAR()
                }
                break;
        }

        DELAY_US(1);
        s_Data.e_OldMode = s_Data.e_Mode;
    }

    return (s_Data);  //SUCCESS Or FAIL
}

S_CONTROL_MUX CONTROL_GateOutputAMP(E_CONTROL_CHANNEL_SELECT e_Channel, E_CONTROL_OPERATION_MODE_SELECT e_Mode)
{
    volatile static S_CONTROL_MUX s_Data = { CONTROL_ALL, CONTROL_REST, CONTROL_OPERATION_INIT };

    s_Data.e_Mode = e_Mode;

    if (s_Data.e_OldMode != s_Data.e_Mode) {
        s_Data.e_Channel = e_Channel;

        switch (s_Data.e_Channel)
        {
            case CONTROL_MASTER:
                if (s_Data.e_Mode == CONTROL_CHARGE) {
                    DSP_GPIO42_O_N_MASTER_DIS_EN1_CLEAR()
                    DSP_GPIO43_O_N_MASTER_DIS_EN0_CLEAR()
                    DSP_GPIO46_O_N_SLAVE_DIS_EN1_CLEAR()
                    DSP_GPIO47_O_N_SLAVE_DIS_EN0_CLEAR()
                    DELAY_US(5);
                    DSP_GPIO44_O_N_MASTER_CHG_EN1_SET()
                    DSP_GPIO45_O_N_MASTER_CHG_EN0_SET()
                } else if (s_Data.e_Mode == CONTROL_DISCHARGE) {
                    DSP_GPIO44_O_N_MASTER_CHG_EN1_CLEAR()
                    DSP_GPIO45_O_N_MASTER_CHG_EN0_CLEAR()
                    DSP_GPIO48_O_N_SLAVE_CHG_EN1_CLEAR()
                    DSP_GPIO49_O_N_SLAVE_CHG_EN0_CLEAR()
                    DELAY_US(5);
                    DSP_GPIO42_O_N_MASTER_DIS_EN1_SET()
                    DSP_GPIO43_O_N_MASTER_DIS_EN0_SET()
                } else {
                    DSP_GPIO44_O_N_MASTER_CHG_EN1_CLEAR()
                    DSP_GPIO45_O_N_MASTER_CHG_EN0_CLEAR()
                    DSP_GPIO42_O_N_MASTER_DIS_EN1_CLEAR()
                    DSP_GPIO43_O_N_MASTER_DIS_EN0_CLEAR()
                }
                break;
            case CONTROL_SLAVE:
                if (s_Data.e_Mode == CONTROL_CHARGE) {
                    DSP_GPIO42_O_N_MASTER_DIS_EN1_CLEAR()
                    DSP_GPIO43_O_N_MASTER_DIS_EN0_CLEAR()
                    DSP_GPIO46_O_N_SLAVE_DIS_EN1_CLEAR()
                    DSP_GPIO47_O_N_SLAVE_DIS_EN0_CLEAR()
                    DELAY_US(5);
                    DSP_GPIO48_O_N_SLAVE_CHG_EN1_SET()
                    DSP_GPIO49_O_N_SLAVE_CHG_EN0_SET()
                } else if (s_Data.e_Mode == CONTROL_DISCHARGE) {
                    DSP_GPIO44_O_N_MASTER_CHG_EN1_CLEAR()
                    DSP_GPIO45_O_N_MASTER_CHG_EN0_CLEAR()
                    DSP_GPIO48_O_N_SLAVE_CHG_EN1_CLEAR()
                    DSP_GPIO49_O_N_SLAVE_CHG_EN0_CLEAR()
                    DELAY_US(5);
                    DSP_GPIO46_O_N_SLAVE_DIS_EN1_SET()
                    DSP_GPIO47_O_N_SLAVE_DIS_EN0_SET()
                } else {
                    DSP_GPIO48_O_N_SLAVE_CHG_EN1_CLEAR()
                    DSP_GPIO49_O_N_SLAVE_CHG_EN0_CLEAR()
                    DSP_GPIO46_O_N_SLAVE_DIS_EN1_CLEAR()
                    DSP_GPIO47_O_N_SLAVE_DIS_EN0_CLEAR()
                }
                break;
            case CONTROL_ALL:
            default:
                if (s_Data.e_Mode == CONTROL_CHARGE) {
                    DSP_GPIO42_O_N_MASTER_DIS_EN1_CLEAR()
                    DSP_GPIO43_O_N_MASTER_DIS_EN0_CLEAR()
                    DSP_GPIO46_O_N_SLAVE_DIS_EN1_CLEAR()
                    DSP_GPIO47_O_N_SLAVE_DIS_EN0_CLEAR()
                    DELAY_US(5);
                    DSP_GPIO44_O_N_MASTER_CHG_EN1_SET()
                    DSP_GPIO45_O_N_MASTER_CHG_EN0_SET()
                    DSP_GPIO48_O_N_SLAVE_CHG_EN1_SET()
                    DSP_GPIO49_O_N_SLAVE_CHG_EN0_SET()
                } else if (s_Data.e_Mode == CONTROL_DISCHARGE) {
                    DSP_GPIO44_O_N_MASTER_CHG_EN1_CLEAR()
                    DSP_GPIO45_O_N_MASTER_CHG_EN0_CLEAR()
                    DSP_GPIO48_O_N_SLAVE_CHG_EN1_CLEAR()
                    DSP_GPIO49_O_N_SLAVE_CHG_EN0_CLEAR()
                    DELAY_US(5);
                    DSP_GPIO42_O_N_MASTER_DIS_EN1_SET()
                    DSP_GPIO43_O_N_MASTER_DIS_EN0_SET()
                    DSP_GPIO46_O_N_SLAVE_DIS_EN1_SET()
                    DSP_GPIO47_O_N_SLAVE_DIS_EN0_SET()
                } else {
                    DSP_GPIO44_O_N_MASTER_CHG_EN1_CLEAR()
                    DSP_GPIO45_O_N_MASTER_CHG_EN0_CLEAR()
                    DSP_GPIO42_O_N_MASTER_DIS_EN1_CLEAR()
                    DSP_GPIO43_O_N_MASTER_DIS_EN0_CLEAR()
                    DSP_GPIO48_O_N_SLAVE_CHG_EN1_CLEAR()
                    DSP_GPIO49_O_N_SLAVE_CHG_EN0_CLEAR()
                    DSP_GPIO46_O_N_SLAVE_DIS_EN1_CLEAR()
                    DSP_GPIO47_O_N_SLAVE_DIS_EN0_CLEAR()
                }
                break;
        }

        DELAY_US(1);
        s_Data.e_OldMode = s_Data.e_Mode;
    }

    return (s_Data);  //SUCCESS Or FAIL
}

S_CONTROL_VDAC CONTROL_VoltageReferenceDAC(float32 f_VolatageValue)
{
    volatile static S_CONTROL_VDAC s_Data = { 0.0, -1.0 };

    Uint16 ui_Temp = 0;
    float32 f_Temp = 0.0;

    s_Data.f_Value = f_VolatageValue;

    if (s_Data.f_OldValue != s_Data.f_Value) {
        f_Temp = s_Data.f_Value * CONTROL_VOLTAGE_GAIN;
        ui_Temp = (unsigned int) ((f_Temp / AD5547_BIPOLAR_GAIN ) + AD5547_ZERO_VOLTAGE_BIPOLAR );

        AD5547_Write(AD5547_VOLTAGE, ui_Temp);

        s_Data.f_OldValue = s_Data.f_Value;
    }
    return (s_Data);  //SUCCESS Or FAIL
}

S_CONTROL_DAC CONTROL_CurrentReferenceDAC(E_CONTROL_CHANNEL_SELECT e_Channel, float32 f_CurrentValue)
{
    volatile static S_CONTROL_DAC s_Data = { CONTROL_ALL, CONTROL_ALL, 0.0, -1.0 };

    Uint16 ui_Temp = 0;
    float32 f_Temp = 0.0;

    s_Data.f_Value = f_CurrentValue;
    s_Data.e_Channel = e_Channel;

    if (s_Data.f_OldValue != s_Data.f_Value || s_Data.e_OldChannel != s_Data.e_Channel) {
        f_Temp = s_Data.f_Value * CONTROL_CURRENT_GAIN;
        if (f_Temp <= 0.0) f_Temp = 0.0;
        ui_Temp = (unsigned int) ((f_Temp / AD5547_UNIPOLAR_GAIN ) + AD5547_ZERO_VOLTAGE_UNIPOLAR );

        s_Data.e_Channel = e_Channel;

        switch(s_Data.e_Channel) {
            case CONTROL_MASTER:
                AD5547_Write(AD5547_CURRENT_MASTER, ui_Temp);
                AD5547_Write(AD5547_CURRENT_SLAVE, AD5547_ZERO_VOLTAGE_UNIPOLAR);
                break;
            case CONTROL_SLAVE:
                AD5547_Write(AD5547_CURRENT_MASTER, AD5547_ZERO_VOLTAGE_UNIPOLAR);
                AD5547_Write(AD5547_CURRENT_SLAVE, ui_Temp);
                break;
            case CONTROL_ALL:
            default:
                AD5547_Write(AD5547_CURRENT_MASTER, ui_Temp);
                AD5547_Write(AD5547_CURRENT_SLAVE, ui_Temp);
                break;
        }

        s_Data.f_OldValue = s_Data.f_Value;
        s_Data.e_OldChannel = s_Data.e_Channel;
    }

	return(s_Data); //SUCCESS Or FAIL
}

S_CONTROL_DAC CONTROL_FINECurrentReferenceDAC(E_CONTROL_CHANNEL_SELECT e_Channel, float32 f_CurrentValue)
{
    volatile static S_CONTROL_DAC s_Data = { CONTROL_ALL, CONTROL_ALL, 0.0, -1.0 };

    Uint16 ui_Temp = 0;
    float32 f_Temp = 0.0;

    s_Data.f_Value = f_CurrentValue;
    s_Data.e_Channel = e_Channel;

    if (s_Data.f_OldValue != s_Data.f_Value || s_Data.e_OldChannel != s_Data.e_Channel) {
        f_Temp = s_Data.f_Value * CONTROL_FINE_CURRENT_GAIN;
        ui_Temp = (unsigned int) ((f_Temp / AD5547_BIPOLAR_GAIN ) + AD5547_ZERO_VOLTAGE_BIPOLAR );

        s_Data.e_Channel = e_Channel;

        switch (s_Data.e_Channel)
        {
            case CONTROL_MASTER:
                AD5547_Write(AD5547_FINE_CURRENT_MASTER, ui_Temp);
                AD5547_Write(AD5547_FINE_CURRENT_SLAVE, AD5547_ZERO_VOLTAGE_BIPOLAR);
                break;
            case CONTROL_SLAVE:
                AD5547_Write(AD5547_FINE_CURRENT_MASTER, AD5547_ZERO_VOLTAGE_BIPOLAR);
                AD5547_Write(AD5547_FINE_CURRENT_SLAVE, ui_Temp);
                break;
            case CONTROL_ALL:
            default:
                AD5547_Write(AD5547_FINE_CURRENT_MASTER, ui_Temp);
                AD5547_Write(AD5547_FINE_CURRENT_SLAVE, ui_Temp);
                break;
        }

        s_Data.f_OldValue = s_Data.f_Value;
        s_Data.e_OldChannel = s_Data.e_Channel;
    }

    return (s_Data);  //SUCCESS Or FAIL
}
//
// End of file
//

