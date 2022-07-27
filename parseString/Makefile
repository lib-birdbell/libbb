CROSS_GCC	:= arm-linux-gcc
OUT_FILE	:= parseStringTest

all:
	gcc -c -Wall -Werror -fpic parseString.c
	gcc -shared -o libParseString.so parseString.o
	#gcc -L/home/sjlee/test/libbb/parseString -Wall -o $(OUT_FILE).out main.c -lParseString
	gcc -L/home/sjlee/test/libbb/parseString -Wl,-rpath=./ -Wall -o $(OUT_FILE).out main.c -lParseString

	#gcc -c main.c
	#gcc -L/home/sjlee/test/libbb/parseString/parseString -Wl,-rpath,/home/sjlee/test/libbb/parseString -o $(OUT_FILE).out main.c -lParseString
	#gcc -o $(OUT_FILE).out main.o libParseString.so -ldl
	#gcc -o $(OUT_FILE).out main.o -ldl

arm:
	$(CROSS_GCC) -c parseString.c main.c
	$(CROSS_GCC) -o $(OUT_FILE).out main.o parseString.o
	#cp -f $(OUT_FILE).out ~/goToLinux.so/


clean:
	rm	-rf *.o
	rm	-rf *.out
