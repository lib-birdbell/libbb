#include <stdio.h>
//#include <stdlib.h>
//#include <stdint.h>
//#include <string.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>



int main(int argc, char** argv){
	int fd;
	unsigned char keys[128];
	struct input_event ev;
	int count = 0;
	int x_cal, y_cal;
	int x, y;
	//const int cal[] = {-5730, -16, 20025636, -116, -6805, 24777744, 65536, 240, 320, 0};
	//const int cal[] = {-1, 0, 4096, 0, -1, 4096, 1, 240, 320, 0};
	//const int cal[] = {-1, 0, 3450, 0, -1, 3615, 65, 240, 320, 0};
	const int cal[] = {-99790, 0, 344275500, 0, -86258, 311822670, 65536, 240, 320, 0};

	fd = open("/dev/input/event1", O_RDONLY);

	while(1){
		if(read(fd, &ev, sizeof(struct input_event)) < 0){
		}else{
			if(ev.type == 3){
				if(ev.code == 0){
					//printf("x=%d, x_cal=%d\n", ev.value, x_cal);
					x = ev.value;
				}else if(ev.code == 1){
					y = ev.value;
					x_cal = (x*cal[0] + y*cal[1] + cal[2])/cal[6];
					y_cal = (x*cal[3] + y*cal[4] + cal[5])/cal[6];
					printf("x=%d, y=%d, cx=%d, cy=%d\n", x, y, x_cal, y_cal);
				}else{
					continue;
				}
				//printf("type=%d,code=%d,value=%d\n", ev.type, ev.code, ev.value);
				count++;
			}
		}
		if(count > 10){
			break;
		}
	}

	close(fd);

	return 0;
}
