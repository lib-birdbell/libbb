#!/bin/sh
FILE_NAME=at91-sama5d27_ms204.dtb
if [ -f ./$FILE_NAME ];then
	echo "update..."
	flash_erase /dev/mtd5 0 0
	nandwrite -p /dev/mtd5 $FILE_NAME
	echo "complete!"
else
	echo "file not found"
fi
