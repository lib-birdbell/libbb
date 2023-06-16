#include <stdio.h>
//#include <stdlib.h>
//#include <stdint.h>
//#include <string.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>

#include <pthread.h>



#define	DATA_COUNT	2048
struct xyz_data {
	int x, y, z;
	int seq;
};

struct thread_data{
	struct xyz_data xyz[DATA_COUNT];
	int nCount;
	int nStep;
};



void* thread_routine(void *arg){
	struct thread_data *tData = (struct thread_data*)arg;
	pthread_t tid;

	//
	//printf("thread_routine() nCount = %d\n", tData->nCount);
	sleep(1);
	tData->nStep++;
	sleep(1);
	tData->nStep++;
	sleep(1);
	tData->nStep++;
	sleep(1);
	tData->nStep++;
	sleep(1);
	tData->nStep++;
}



int main(int argc, char** argv){
	int fd;
	unsigned char keys[128];
	struct input_event ev;
	int count = 0;
	int x_cal, y_cal;
	int x, y, p;
	int ret, i, comp;
	//const int cal[] = {-5730, -16, 20025636, -116, -6805, 24777744, 65536, 240, 320, 0};
	//const int cal[] = {-1, 0, 4096, 0, -1, 4096, 1, 240, 320, 0};
	//const int cal[] = {-1, 0, 3450, 0, -1, 3615, 65, 240, 320, 0};
	const int cal[] = {-99790, 0, 344275500, 0, -86258, 311822670, 65536, 240, 320, 0};
	pthread_t thread;
	struct thread_data tData;

	//fd = open("/dev/input/event1", O_RDONLY);
	fd = open("/dev/input/event0", O_RDONLY);

	printf("APP : fd=%d bytes\n", fd);
	printf("APP : sizeof(struct input_event)=%d bytes\n", sizeof(struct input_event));

	x = -1;
	y = -1;
	p = -1;

	tData.nCount = 0;
	tData.nStep = 0;

	// Create thread
	pthread_create(&thread, NULL, thread_routine, (void*)&tData);

	//while(1){
	while(count < 300){
		ret = read(fd, &ev, sizeof(struct input_event));
		if(ret){
			if(ev.type == 3){
				//printf("APP : read count = %d\n", x);
				//printf("APP : ev.type=%d,", ev.type);
				if(ev.code == 0){
					//printf("APP : x = %d, ", ev.value);
					x = ev.value;
				}else if(ev.code == 1){
					//printf("APP : y = %d, ", ev.value);
					y = ev.value;
				}else if(ev.code == 24){
					//printf("APP : p = %d, ", ev.value);
					p = ev.value;
				}
				//printf("APP : ev.code=%d,", ev.code);
				//printf("APP : ev.value=%d\n", ev.value);
				//printf("\n");
				tData.xyz[count].x = x;
				tData.xyz[count].y = y;
				tData.xyz[count].z = p;
				tData.xyz[count].seq = tData.nStep;
				count++;
				tData.nCount = count;
			}
#if 0
			if(x != -1 && y != -1 && p != -1){
				printf("APP : X = %d, Y = %d, P = %d\n", x, y, p);
				x = -1;
				y = -1;
				p = -1;
			}
#endif
		}
		if(count > 300){
			break;
		}
	}

	close(fd);

	for(i=0;i<tData.nCount;i++){
		printf("%04d:x=%d,y=%d,z=%d\n", tData.xyz[i].seq, tData.xyz[i].x, tData.xyz[i].y, tData.xyz[i].z);
	}

	printf("TPS\n");
	count = 0;
	comp = 0;
	for(i=0;i<tData.nCount;i++){
		if(tData.xyz[i].seq != comp){
			printf("%04d:%d\n", comp, count);
			count = 0;
			comp++;
		}else{
			count++;
		}
	}

	printf("APP : program end!\n");

	return 0;
}
