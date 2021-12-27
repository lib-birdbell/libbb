#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#include <fcntl.h>


#define	SERIAL_DEV	"/dev/ttyS0"
#define	BUF_SIZE	256


int main(int argc, char** argv){
	int fd;
	int ret;
	char buf[BUF_SIZE] = "test";

	struct termios newtio;

	// Open
	fd = open(SERIAL_DEV, O_RDWR | O_NOCTTY | O_NONBLOCK);

	// Init
	memset( &newtio, 0, sizeof(newtio) );

	newtio.c_cflag = B115200;
	newtio.c_cflag |= CS8;
	newtio.c_cflag |= CLOCAL;
	newtio.c_cflag |= CREAD;
	newtio.c_iflag = 0;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 1;

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &newtio );

	// Write
	ret = write(fd, buf, 4);
	printf("write %d\n, ret");

	// Read
	ret = read(fd, buf, 4);
	printf("read %d\n, ret");

	// Close
	close( fd );

	return 0;
}
