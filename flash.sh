#!/bin/bash

if [ -z "$1" ]; then
   PORT=/dev/tty.usbserial
else
   PORT=$1
fi

/Volumes/ESPTools/esp-open-sdk/esptool/esptool.py --port $PORT  write_flash 0x00000 firmware/0x00000.bin 0x40000 firmware/0x40000.bin 0x12000 firmware/webpages.espfs

