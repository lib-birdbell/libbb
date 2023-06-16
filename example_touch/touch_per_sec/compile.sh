#!/bin/sh
PATH=`readlink -f ../../toolchain/arm-linux-gnueabihf-7.3`/bin/:$PATH
#PATH=`readlink -f ../tools/`:$PATH

make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
#make ARCH=arm CROSS_COMPILE=/home/sjlee/MS-204/src/buildroot/buildroot-2021.05/output/host/bin/arm-linux-
