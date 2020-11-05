################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include build/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: o2.axf

# Tool invocations
o2.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m3 -mthumb -T "o2.ld" -Xlinker --gc-sections -Xlinker -Map="o2.map" --specs=nano.specs -o o2.axf "./gpio_constants.o" "./o2.o" "./sys-tick_constants.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: o2.hex'
	arm-none-eabi-objcopy -O ihex "o2.axf" "o2.hex"
	@echo ' '

	@echo 'Building bin file: o2.bin'
	arm-none-eabi-objcopy -O binary "o2.axf" "o2.bin"
	@echo ' '

	@echo 'Building s37 file: o2.s37'
	arm-none-eabi-objcopy -O srec "o2.axf" "o2.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "o2.axf" -A
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) o2.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
