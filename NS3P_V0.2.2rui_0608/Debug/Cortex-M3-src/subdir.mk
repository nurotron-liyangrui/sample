################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Cortex-M3-src/initialize.c \
../Cortex-M3-src/interrupt.c \
../Cortex-M3-src/main.c \
../Cortex-M3-src/memory.c \
../Cortex-M3-src/pcm.c \
../Cortex-M3-src/variable.c 

C_DEPS += \
./Cortex-M3-src/initialize.d \
./Cortex-M3-src/interrupt.d \
./Cortex-M3-src/main.d \
./Cortex-M3-src/memory.d \
./Cortex-M3-src/pcm.d \
./Cortex-M3-src/variable.d 

C_OBJS += \
./Cortex-M3-src/initialize.o \
./Cortex-M3-src/interrupt.o \
./Cortex-M3-src/main.o \
./Cortex-M3-src/memory.o \
./Cortex-M3-src/pcm.o \
./Cortex-M3-src/variable.o 


# Each subdirectory must supply rules for building sources it contributes
Cortex-M3-src/%.o: ../Cortex-M3-src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Compiler'
	arm-none-eabi-gcc -c -DSK4_CID=102 -I. -IC:\PROGRA~2\ONSEMI~1\EZAIRO~1/include/cm3 -IC:\PROGRA~2\ONSEMI~1\EZAIRO~1/arm_tools/arm-none-eabi/include -IC:\PROGRA~2\ONSEMI~1\EZAIRO~1/arm_tools/lib/gcc/arm-none-eabi/4.5.2/include -O0 -g -Wall -mcpu=cortex-m3 -mthumb -msoft-float -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


