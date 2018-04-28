################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
FreeRTOS/portable/MemMang/heap_2.obj: ../FreeRTOS/portable/MemMang/heap_2.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --include_path="C:/Users/vipra/Apes2/FreeRTOS_tiva_demo/FreeRTOS/portable/CCS/ARM_CM4F" --include_path="C:/Users/vipra/Apes2/FreeRTOS_tiva_demo" --include_path="C:/Users/vipra/Apes2/FreeRTOS_tiva_demo/driverlib" --include_path="C:/Users/vipra/Apes2/FreeRTOS_tiva_demo/FreeRTOS/include" --include_path="C:/Users/vipra/Apes2/FreeRTOS_tiva_demo/FreeRTOS/portable/CCS/ARM_CM4F" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA1 --define=PART_TM4C1294NCPDT --define=DEBUG --display_error_number --diag_wrap=off --diag_warning=225 --preproc_with_compile --preproc_dependency="FreeRTOS/portable/MemMang/heap_2.d_raw" --obj_directory="FreeRTOS/portable/MemMang" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


