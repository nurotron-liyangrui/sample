################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../.metadata/.plugins/org.eclipse.cdt.make.core/specs.c 

C_DEPS += \
./.metadata/.plugins/org.eclipse.cdt.make.core/specs.d 

C_OBJS += \
./.metadata/.plugins/org.eclipse.cdt.make.core/specs.o 


# Each subdirectory must supply rules for building sources it contributes
.metadata/.plugins/org.eclipse.cdt.make.core/%.o: ../.metadata/.plugins/org.eclipse.cdt.make.core/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Compiler'
	arm-none-eabi-gcc -c -DSK4_CID=102 -I. -IC:\PROGRA~2\ONSEMI~1\EZAIRO~1/include/cm3 -IC:\PROGRA~2\ONSEMI~1\EZAIRO~1/arm_tools/arm-none-eabi/include -IC:\PROGRA~2\ONSEMI~1\EZAIRO~1/arm_tools/lib/gcc/arm-none-eabi/4.5.2/include -O0 -g -Wall -mcpu=cortex-m3 -mthumb -msoft-float -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


