#!/bin/sh
set -x 
mkspiffs -c data -p 256 -b 8192 -s 1028096 Release/ESPButton.spiffs.bin
../hardware/esp8266com/esp8266/tools/esptool/esptool.py --before no_reset --after soft_reset --baud 115200 --port /dev/cu.SLAB_USBtoUART write_flash  --flash_mode dio 0x300000 Release/ESPButton.spiffs.bin

