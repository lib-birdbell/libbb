#!/bin/sh
argc=$#
argv0=$0
argv1=$1
argv2=$2
argv3=$3
argv4=$4
argv5=$5
if [ $argc -lt 5 ]
then
	echo "HOW TO USE : merge.sh [filesize(Kbyte)] [1st file] [1st file address] [2nd file] [2nd file address]"
	echo "This program merge 2 files. Space was filled with 0xFF."
	echo "size, address recognize as hex.(Add 0x)"
	exit 0
fi
dd if=/dev/zero ibs=1k count=$argv1 | tr "\000" "\377" >outfile.bin
dd if=./$argv2 of=./outfile.bin conv=notrunc ibs=1 seek=$(($argv3))
dd if=./$argv4 of=./outfile.bin conv=notrunc ibs=1 seek=$(($argv5))
echo $argc
echo $argv0 $argv1 $argv2 $argv3 $argv4 $argv5
echo $(($argv3))
