################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
hal/a2d.obj: ../hal/a2d.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ProgramFiles/ti_ccs/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me -Ooff --opt_for_speed=5 --include_path="C:/ProgramFiles/ti_ccs/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/hal" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/nonessential" --include_path="C:/Projects/rLoop/workspace_ccs/LGU" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/halcogen_includes" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/fapi_includes" -g --c99 --c++03 --diag_wrap=off --diag_warning=225 --display_error_number --abi=eabi --enum_type=packed --preproc_with_compile --preproc_dependency="hal/a2d.d" --obj_directory="hal" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

hal/gpin.obj: ../hal/gpin.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ProgramFiles/ti_ccs/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me -Ooff --opt_for_speed=5 --include_path="C:/ProgramFiles/ti_ccs/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/hal" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/nonessential" --include_path="C:/Projects/rLoop/workspace_ccs/LGU" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/halcogen_includes" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/fapi_includes" -g --c99 --c++03 --diag_wrap=off --diag_warning=225 --display_error_number --abi=eabi --enum_type=packed --preproc_with_compile --preproc_dependency="hal/gpin.d" --obj_directory="hal" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

hal/pwm.obj: ../hal/pwm.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ProgramFiles/ti_ccs/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me -Ooff --opt_for_speed=5 --include_path="C:/ProgramFiles/ti_ccs/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/hal" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/nonessential" --include_path="C:/Projects/rLoop/workspace_ccs/LGU" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/halcogen_includes" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/fapi_includes" -g --c99 --c++03 --diag_wrap=off --diag_warning=225 --display_error_number --abi=eabi --enum_type=packed --preproc_with_compile --preproc_dependency="hal/pwm.d" --obj_directory="hal" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

hal/ser.obj: ../hal/ser.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ProgramFiles/ti_ccs/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me -Ooff --opt_for_speed=5 --include_path="C:/ProgramFiles/ti_ccs/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/hal" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/nonessential" --include_path="C:/Projects/rLoop/workspace_ccs/LGU" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/halcogen_includes" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/fapi_includes" -g --c99 --c++03 --diag_wrap=off --diag_warning=225 --display_error_number --abi=eabi --enum_type=packed --preproc_with_compile --preproc_dependency="hal/ser.d" --obj_directory="hal" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

hal/systmr.obj: ../hal/systmr.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ProgramFiles/ti_ccs/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me -Ooff --opt_for_speed=5 --include_path="C:/ProgramFiles/ti_ccs/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/hal" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/nonessential" --include_path="C:/Projects/rLoop/workspace_ccs/LGU" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/halcogen_includes" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/fapi_includes" -g --c99 --c++03 --diag_wrap=off --diag_warning=225 --display_error_number --abi=eabi --enum_type=packed --preproc_with_compile --preproc_dependency="hal/systmr.d" --obj_directory="hal" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

hal/term_hal.obj: ../hal/term_hal.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ProgramFiles/ti_ccs/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me -Ooff --opt_for_speed=5 --include_path="C:/ProgramFiles/ti_ccs/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/hal" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/nonessential" --include_path="C:/Projects/rLoop/workspace_ccs/LGU" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/halcogen_includes" --include_path="C:/Projects/rLoop/workspace_ccs/LGU/fapi_includes" -g --c99 --c++03 --diag_wrap=off --diag_warning=225 --display_error_number --abi=eabi --enum_type=packed --preproc_with_compile --preproc_dependency="hal/term_hal.d" --obj_directory="hal" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


