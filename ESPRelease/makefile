################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -f

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include libraries/WiFiManager/subdir.mk
-include libraries/Ticker/subdir.mk
-include libraries/RGBLED/src/subdir.mk
-include libraries/ESP8266WiFi/src/subdir.mk
-include libraries/ESP8266WebServer/src/subdir.mk
-include libraries/ESP8266HTTPClient/src/subdir.mk
-include libraries/EEPROM/subdir.mk
-include libraries/DNSServer/src/subdir.mk
-include core/core/umm_malloc/subdir.mk
-include core/core/spiffs/subdir.mk
-include core/core/libb64/subdir.mk
-include core/core/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
AR += \
/Users/chris.l/git/ESPButton/ESPRelease/arduino.ar \

ELF += \
ESPButton.elf \

HEX += \
ESPButton.hex \


# All Target
all: ESPButton

# Tool invocations
ESPButton: $(HEX)
	@echo 'Building target: $@'
	@echo 'Printing size:'
	"/Users/chris.l/Arduino/hardware/esp8266/tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-size" -A "/Users/chris.l/git/ESPButton/ESPRelease/ESPButton.elf"
	@echo 'Finished building target: $@'
	@echo ' '

/Users/chris.l/git/ESPButton/ESPRelease/arduino.ar: $(AR_OBJ)
	@echo 'Starting archiver'
	"/Users/chris.l/Arduino/hardware/esp8266/tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-ar" cru "/Users/chris.l/git/ESPButton/ESPRelease/arduino.ar"    /Users/chris.l/git/ESPButton/ESPRelease/arduino.ar $(AR_OBJ)
	@echo 'Finished building: $@'
	@echo ' '

ESPButton.elf: $(LINK_OBJ) $(AR)
	@echo 'Starting combiner'
	"/Users/chris.l/Arduino/hardware/esp8266/tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-gcc" -g -Wall -Wextra -Os -nostdlib -Wl,--no-check-sections -u call_user_start -u _printf_float -u _scanf_float -Wl,-static "-L/Users/chris.l/Arduino/hardware/esp8266/tools/sdk/lib" "-L/Users/chris.l/Arduino/hardware/esp8266/tools/sdk/ld" "-L/Users/chris.l/Arduino/hardware/esp8266/tools/sdk/libc/xtensa-lx106-elf/lib" "-Teagle.flash.4m1m.ld" -Wl,--gc-sections -Wl,-wrap,system_restart_local -Wl,-wrap,spi_flash_read -o "/Users/chris.l/git/ESPButton/ESPRelease/ESPButton.elf" -Wl,--start-group   $(LINK_OBJ)  "/Users/chris.l/git/ESPButton/ESPRelease/arduino.ar" -lhal -lphy -lpp -lnet80211 -llwip2 -lwpa -lcrypto -lmain -lwps -laxtls -lespnow -lsmartconfig -lairkiss -lmesh -lwpa2 -lstdc++ -lm -lc -lgcc -Wl,--end-group "-L/Users/chris.l/git/ESPButton/ESPRelease" /Users/chris.l/git/ESPButton/ESPRelease/arduino.ar
	@echo 'Finished building: $@'
	@echo ' '

ESPButton.hex: $(ELF)
	@echo 'Create eeprom image'
	"/Users/chris.l/Arduino/hardware/esp8266/tools/esptool/esptool" -eo "/Users/chris.l/Arduino/hardware/esp8266/bootloaders/eboot/eboot.elf" -bo "/Users/chris.l/git/ESPButton/ESPRelease/ESPButton.bin" -bm dio -bf 40 -bz 4M -bs .text -bp 4096 -ec -eo "/Users/chris.l/git/ESPButton/ESPRelease/ESPButton.elf" -bs .irom0.text -bs .text -bs .data -bs .rodata -bc -ec
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C_UPPER_DEPS)$(PDE_DEPS)$(C_DEPS)$(AR)$(CC_DEPS)$(AR_OBJ)$(C++_DEPS)$(LINK_OBJ)$(CXX_DEPS)$(ASM_DEPS)$(HEX)$(INO_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(ELF)$(CPP_DEPS)
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
