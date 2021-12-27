CROSS_GCC	:= arm-linux-gnueabihf-gcc

all: default

default:
	$(CROSS_GCC) -o serialtest main.c

clean:
	rm	-rf *.ko
	rm	-rf *.mod.*
	rm	-rf .*.cmd
	rm	-rf *.o
	rm	-rf .tmp_versions
	rm	-rf Module.markers  
	rm	-rf Module.symvers
	rm	-rf modules.order

# export ARCH=arm
# export CROSS_COMPILE=/opt/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/arm-none-linux-gnueabi-
