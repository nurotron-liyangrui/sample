################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CFS_SRCS += \
../CM3.cfs \
../app.cfs \
../app_init.cfs \
../app_isr.cfs \
../app_memory.cfs 

HCT_SRCS += \
../microcode.hct 

CFS_OBJS += \
./CM3.o \
./app.o \
./app_init.o \
./app_isr.o \
./app_memory.o \
./microcode.o 

HCT_DEPS += \
./microcode.dep 

CFS_DEPS += \
./CM3.dep \
./app.dep \
./app_init.dep \
./app_isr.dep \
./app_memory.dep 


# Each subdirectory must supply rules for building sources it contributes
%.o %.orc: ../%.cfs $(HCT_DEPS:.dep=.cfi)
	@echo 'Building file: $<'
	@echo 'Invoking: CFCC Assembler'
	dpp -IC:\PROGRA~2\ONSEMI~1\EZAIRO~1/include/cfx -I. -DSK4_CID=102 -MG -MT $@ $< $(addsuffix .dep,$(basename $@))
	cfcc -c -bSK4 --toolchain=gnu -g -IC:\PROGRA~2\ONSEMI~1\EZAIRO~1/include/cfx -I. -DSK4_CID=102 --lst --strictnop -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CM3.orc: CM3.o

app.orc: app.o

app_init.orc: app_init.o

app_isr.orc: app_isr.o

app_memory.orc: app_memory.o

%.cfi %.o: ../%.hct
	@echo 'Building file: $<'
	@echo 'Invoking: HCT Compiler'
	dpp -IC:\PROGRA~2\ONSEMI~1\EZAIRO~1/include/cfx -MG -MT $@ $< $(addsuffix .dep,$(basename $@))
	hct -bsk4c102 --toolchain=gnu -g -s -LC:\PROGRA~2\ONSEMI~1\EZAIRO~1/lib/cfx -IC:\PROGRA~2\ONSEMI~1\EZAIRO~1/include/cfx -lstdhear2.hcl "$<" -o "$(@:.cfi=.o)" -i "$(@:.o=.cfi)"
	@echo 'Finished building: $<'
	@echo ' '

microcode.o: microcode.cfi


