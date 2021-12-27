#!/bin/sh
FILE_NAME=zImage
if [ -f ./$FILE_NAME ];then
	echo "update..."
	flash_erase /dev/mtd7 0 0
	nandwrite -p /dev/mtd7 $FILE_NAME
	echo "complete!"
else
	echo "file not found"
fi
