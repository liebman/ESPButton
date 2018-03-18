################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Config.cpp \
../ESPButton.cpp \
../Logger.cpp 

LINK_OBJ += \
./Config.cpp.o \
./ESPButton.cpp.o \
./Logger.cpp.o 

CPP_DEPS += \
./Config.cpp.d \
./ESPButton.cpp.d \
./Logger.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
Config.cpp.o: ../Config.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/Users/chris.l/Arduino/hardware/esp8266/tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-g++" -D__ets__ -DICACHE_FLASH -U__STRICT_ANSI__ "-I/Users/chris.l/Arduino/hardware/esp8266/tools/sdk/include" "-I/Users/chris.l/Arduino/hardware/esp8266/tools/sdk/lwip2/include" "-I/Users/chris.l/Arduino/hardware/esp8266/tools/sdk/libc/xtensa-lx106-elf/include" "-I/Users/chris.l/git/ESPButton/ESPDebug/core" -c -Wall -Wextra -Os -g -mlongcalls -mtext-section-literals -fno-exceptions -fno-rtti -falign-functions=4 -std=c++11 -ffunction-sections -fdata-sections -DF_CPU=80000000L -DLWIP_OPEN_SRC -DDEBUG_ESP_PORT=Serial -DARDUINO=10802 -DARDUINO_ESP8266_NODEMCU -DARDUINO_ARCH_ESP8266 '-DARDUINO_BOARD="ESP8266_NODEMCU"' -DESP8266  -I"/Users/chris.l/Arduino/hardware/esp8266/cores/esp8266" -I"/Users/chris.l/Arduino/hardware/esp8266/variants/nodemcu" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/DNSServer" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/DNSServer/src" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/EEPROM" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/ESP8266WebServer" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/ESP8266WebServer/src" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/ESP8266WiFi" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/ESP8266WiFi/src" -I"/Users/chris.l/Arduino/libraries/WiFiManager" -I"/Users/chris.l/Arduino/libraries/RGBLED" -I"/Users/chris.l/Arduino/libraries/RGBLED/src" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/Ticker" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/ESP8266HTTPClient" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/ESP8266HTTPClient/src" -I"/Users/chris.l/Arduino/libraries/gdb" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"
	@echo 'Finished building: $<'
	@echo ' '

ESPButton.cpp.o: ../ESPButton.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/Users/chris.l/Arduino/hardware/esp8266/tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-g++" -D__ets__ -DICACHE_FLASH -U__STRICT_ANSI__ "-I/Users/chris.l/Arduino/hardware/esp8266/tools/sdk/include" "-I/Users/chris.l/Arduino/hardware/esp8266/tools/sdk/lwip2/include" "-I/Users/chris.l/Arduino/hardware/esp8266/tools/sdk/libc/xtensa-lx106-elf/include" "-I/Users/chris.l/git/ESPButton/ESPDebug/core" -c -Wall -Wextra -Os -g -mlongcalls -mtext-section-literals -fno-exceptions -fno-rtti -falign-functions=4 -std=c++11 -ffunction-sections -fdata-sections -DF_CPU=80000000L -DLWIP_OPEN_SRC -DDEBUG_ESP_PORT=Serial -DARDUINO=10802 -DARDUINO_ESP8266_NODEMCU -DARDUINO_ARCH_ESP8266 '-DARDUINO_BOARD="ESP8266_NODEMCU"' -DESP8266  -I"/Users/chris.l/Arduino/hardware/esp8266/cores/esp8266" -I"/Users/chris.l/Arduino/hardware/esp8266/variants/nodemcu" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/DNSServer" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/DNSServer/src" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/EEPROM" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/ESP8266WebServer" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/ESP8266WebServer/src" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/ESP8266WiFi" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/ESP8266WiFi/src" -I"/Users/chris.l/Arduino/libraries/WiFiManager" -I"/Users/chris.l/Arduino/libraries/RGBLED" -I"/Users/chris.l/Arduino/libraries/RGBLED/src" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/Ticker" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/ESP8266HTTPClient" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/ESP8266HTTPClient/src" -I"/Users/chris.l/Arduino/libraries/gdb" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"
	@echo 'Finished building: $<'
	@echo ' '

Logger.cpp.o: ../Logger.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/Users/chris.l/Arduino/hardware/esp8266/tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-g++" -D__ets__ -DICACHE_FLASH -U__STRICT_ANSI__ "-I/Users/chris.l/Arduino/hardware/esp8266/tools/sdk/include" "-I/Users/chris.l/Arduino/hardware/esp8266/tools/sdk/lwip2/include" "-I/Users/chris.l/Arduino/hardware/esp8266/tools/sdk/libc/xtensa-lx106-elf/include" "-I/Users/chris.l/git/ESPButton/ESPDebug/core" -c -Wall -Wextra -Os -g -mlongcalls -mtext-section-literals -fno-exceptions -fno-rtti -falign-functions=4 -std=c++11 -ffunction-sections -fdata-sections -DF_CPU=80000000L -DLWIP_OPEN_SRC -DDEBUG_ESP_PORT=Serial -DARDUINO=10802 -DARDUINO_ESP8266_NODEMCU -DARDUINO_ARCH_ESP8266 '-DARDUINO_BOARD="ESP8266_NODEMCU"' -DESP8266  -I"/Users/chris.l/Arduino/hardware/esp8266/cores/esp8266" -I"/Users/chris.l/Arduino/hardware/esp8266/variants/nodemcu" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/DNSServer" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/DNSServer/src" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/EEPROM" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/ESP8266WebServer" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/ESP8266WebServer/src" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/ESP8266WiFi" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/ESP8266WiFi/src" -I"/Users/chris.l/Arduino/libraries/WiFiManager" -I"/Users/chris.l/Arduino/libraries/RGBLED" -I"/Users/chris.l/Arduino/libraries/RGBLED/src" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/Ticker" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/ESP8266HTTPClient" -I"/Users/chris.l/Arduino/hardware/esp8266/libraries/ESP8266HTTPClient/src" -I"/Users/chris.l/Arduino/libraries/gdb" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"
	@echo 'Finished building: $<'
	@echo ' '


