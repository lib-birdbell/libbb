#!/bin/sh
FILE_NAME=u-boot.bin
if [ -f ./$FILE_NAME ];then
	echo "update..."
	flash_erase /dev/mtd1 0 0
	nandwrite -p /dev/mtd1 $FILE_NAME
	echo "complete!"
else
	echo "file not found"
fi
