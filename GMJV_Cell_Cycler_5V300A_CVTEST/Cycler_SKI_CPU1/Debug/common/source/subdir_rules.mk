################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
common/source/%.obj: ../common/source/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_18.12.2.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --fp_mode=relaxed --include_path="C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_18.12.2.LTS/include" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/GMJV_Cell_Cycler_5V300A_CVTEST/Cycler_SKI_CPU1/PHOENIXON_CONTROLS/CPU/include" --advice:performance=all --define=CPU1 --define=_STANDALONE --define=_FLASH --define=_DUAL_HEADERS --define=_DEBUG --define=DEVICE_CPU1 --undefine=INTERFACE_CPU1 --undefine=_DEBUG_PRINTF_CALL --undefine=DEVICE_CPU2 --undefine=INTERFACE_CPU2 -g --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="common/source/$(basename $(<F)).d_raw" --obj_directory="common/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

common/source/%.obj: ../common/source/%.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_18.12.2.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --fp_mode=relaxed --include_path="C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_18.12.2.LTS/include" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/GMJV_Cell_Cycler_5V300A_CVTEST/Cycler_SKI_CPU1/PHOENIXON_CONTROLS/CPU/include" --advice:performance=all --define=CPU1 --define=_STANDALONE --define=_FLASH --define=_DUAL_HEADERS --define=_DEBUG --define=DEVICE_CPU1 --undefine=INTERFACE_CPU1 --undefine=_DEBUG_PRINTF_CALL --undefine=DEVICE_CPU2 --undefine=INTERFACE_CPU2 -g --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="common/source/$(basename $(<F)).d_raw" --obj_directory="common/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

