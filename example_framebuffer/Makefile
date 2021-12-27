CCDDDDDD=arm-linux-gnueabihf-gcc

default:
	$(CROSS_COMPILE)gcc -c main.c
	$(CROSS_COMPILE)gcc main.o -o lcd_test.out

clean:
	rm -rf *.o
	rm -rf *.out
