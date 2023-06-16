#include <stdio.h>
#include <stdlib.h>
//#include <stdint.h>
//#include <string.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>

#include <pthread.h>

#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>



#define	DATA_COUNT	4096
struct xyz_data {
	int x, y, z;
	int seq;
};

struct thread_data{
	struct xyz_data xyz[DATA_COUNT];
	int nCount;
	int nStep;
	int isThreadEnd;
};



void* thread_routine(void *arg){
	struct thread_data *tData = (struct thread_data*)arg;
	pthread_t tid;

	//
	//printf("thread_routine() nCount = %d\n", tData->nCount);
	while(tData->isThreadEnd == 0){
		sleep(1);
		tData->nStep++;
	}
}

void ConvertX(int* x);
void ConvertY(int* y);
void ConvertXY(int* x, int* y);
void DrawXY(unsigned char *fbp, int x, int y);
void DrawLine(unsigned char *fbp, int x, int y, int ex, int ey);
void DrawLineExact(unsigned char *fbp, int x, int y, int ex, int ey);
void DrawPoint(unsigned char *fbp, int x, int y, int size);
void bresenham(unsigned char *fbp, int x, int y, int ex, int ey);
void average10(int *xsum, int *ysum, int *x, int *y);



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
	int fb_fd;
	unsigned char *fb;
	unsigned char *fbp;
	int x_old, y_old;
	int x_sum[10], y_sum[10];
	int xa[235] = {
499,
524,
524,
531,
543,
545,
563,
582,
595,
608,
601,
588,
614,
601,
614,
633,
649,
640,
633,
652,
655,
652,
662,
665,
665,
683,
666,
665,
651,
678,
671,
681,
676,
681,
665,
673,
676,
678,
676,
671,
671,
678,
665,
672,
678,
673,
673,
671,
684,
672,
676,
675,
665,
652,
665,
652,
666,
665,
668,
684,
665,
665,
673,
668,
677,
678,
675,
675,
678,
678,
691,
678,
665,
663,
662,
639,
627,
614,
614,
607,
601,
595,
593,
563,
579,
563,
561,
543,
542,
543,
527,
512,
521,
499,
499,
495,
499,
479,
471,
459,
454,
450,
437,
443,
422,
420,
412,
403,
392,
383,
379,
371,
361,
354,
345,
335,
332,
321,
307,
300,
289,
281,
268,
265,
248,
241,
226,
218,
205,
204,
193,
192,
185,
178,
177,
179,
169,
163,
163,
160,
164,
168,
168,
168,
165,
164,
167,
171,
168,
172,
176,
181,
179,
182,
182,
183,
185,
185,
187,
185,
185,
187,
192,
190,
191,
192,
192,
194,
195,
197,
191,
198,
197,
197,
195,
204,
200,
200,
199,
195,
204,
201,
200,
214,
217,
224,
230,
246,
255,
255,
272,
281,
291,
302,
310,
310,
331,
344,
347,
352,
348,
364,
367,
375,
380,
387,
393,
406,
407,
418,
426,
433,
438,
446,
447,
454,
473,
473,
486,
495,
512,
512,
515,
524,
543,
543,
550,
556,
556,
572,
577,
588,
598,
607,
614
	};
	int ya[235] = {
             205,
	     195,
	     203,
	     201,
	     198,
	     185,
	     191,
	     180,
	     186,
	     175,
	     175,
	     184,
	     182,
	     183,
	     164,
	     170,
	     177,
	     166,
	     179,
	     159,
	     162,
	     159,
	     164,
	     159,
	     159,
	     168,
	     159,
	     159,
	     172,
	     159,
	     160,
	     163,
	     169,
	     167,
	     162,
	     171,
	     166,
	     171,
	     181,
	     194,
	     201,
	     209,
	     211,
	     222,
	     230,
	     233,
	     243,
	     260,
	     270,
	     269,
	     272,
	     286,
	     294,
	     297,
	     310,
	     314,
	     315,
	     323,
	     324,
	     334,
	     331,
	     340,
	     340,
	     348,
	     350,
	     347,
	     345,
	     345,
	     346,
	     345,
	     348,
	     346,
	     342,
	     337,
	     334,
	     332,
	     329,
	     323,
	     319,
	     318,
	     313,
	     303,
	     300,
	     297,
	     295,
	     290,
	     287,
	     286,
	     280,
	     285,
	     279,
	     270,
	     264,
	     268,
	     267,
	     256,
	     255,
	     257,
	     251,
	     244,
	     233,
	     239,
	     230,
	     227,
	     222,
	     217,
	     222,
	     221,
	     217,
	     214,
	     201,
	     209,
	     207,
	     195,
	     206,
	     187,
	     192,
	     179,
	     185,
	     181,
	     167,
	     163,
	     170,
	     163,
	     167,
	     163,
	     158,
	     153,
	     147,
	     139,
	     147,
	     141,
	     139,
	     135,
	     140,
	     137,
	     126,
	     120,
	     135,
	     127,
	     128,
	     137,
	     131,
	     143,
	     131,
	     149,
	     159,
	     175,
	     178,
	     191,
	     203,
	     207,
	     217,
	     220,
	     232,
	     248,
	     259,
	     268,
	     274,
	     283,
	     291,
	     304,
	     310,
	     314,
	     318,
	     321,
	     324,
	     327,
	     332,
	     331,
	     334,
	     341,
	     340,
	     346,
	     344,
	     351,
	     346,
	     348,
	     348,
	     349,
	     347,
	     347,
	     346,
	     342,
	     338,
	     342,
	     338,
	     329,
	     326,
	     318,
	     313,
	     309,
	     307,
	     296,
	     294,
	     284,
	     279,
	     278,
	     271,
	     273,
	     268,
	     263,
	     257,
	     252,
	     247,
	     245,
	     244,
	     239,
	     234,
	     241,
	     230,
	     228,
	     222,
	     223,
	     217,
	     221,
	     215,
	     207,
	     203,
	     207,
	     199,
	     197,
	     203,
	     199,
	     195,
	     193,
	     190,
	     180,
	     175,
	     170,
	     174,
	     177,
	     175,
	     163,
	     161
	};

	// Buffer memory allocation
	fb = (unsigned char*)malloc((size_t)800*480*3);
	if(!fb){
		printf("malloc failed! %d\n", fb);
		exit(1);
	}

	//fd = open("/dev/input/event1", O_RDONLY);
	fd = open("/dev/input/event0", O_RDONLY);

	printf("APP : fd=%d bytes\n", fd);
	printf("APP : sizeof(struct input_event)=%d bytes\n", sizeof(struct input_event));

	x = -1;
	y = -1;
	p = -1;

	tData.nCount = 0;
	tData.nStep = 0;
	tData.isThreadEnd = 0;

	// Create thread
	pthread_create(&thread, NULL, thread_routine, (void*)&tData);

	// Open frame buffer
	fb_fd = open("/dev/fb0", O_RDWR);
	if(fb_fd < 0){
		printf("fb0 open failed! %d\n", fb_fd);
		close(fd);
		exit(1);
	}

	fbp = (unsigned char*)mmap(0, 800*480*3, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
	if(fbp == MAP_FAILED){
		perror("Error : mmap\r\n");
		goto EXIT;
	}

	// Make filed white
	for(i=0;i<800*480*3;i+=3){
		fbp[i+0] = 0xFF;// Blue
		fbp[i+1] = 0xFF;// Green
		fbp[i+2] = 0xFF;// Red
	}

#if 0
	//DrawLineExact(fbp, 230, 338, 246, 329);
	//DrawLineExact(fbp, 246, 329, 255, 326);
	//DrawPoint(fbp, 245, 330, 1);
	
	for(i=1;i<235;i++){
		//DrawLineExact(fbp, xa[i-1], ya[i-1], xa[i], ya[i]);
		bresenham(fbp, xa[i-1], ya[i-1], xa[i], ya[i]);
	}
#endif

#if 1
	while(count < 1500){
		ret = read(fd, &ev, sizeof(struct input_event));
		if(ret){
			if(ev.type == 3){
				//printf("APP : read count = %d\n", x);
				//printf("APP : ev.type=%d,", ev.type);
				if(ev.code == 0){
					if(x == -1){
					x = ev.value;
					ConvertX(&x);
					x_cal = x;
					}else{
					//printf("APP : x = %d, ", ev.value);
					//x_cal = x;	//// Backup
					x = ev.value;
					ConvertX(&x);
					}
				}else if(ev.code == 1){
					if(y == -1){
					y = ev.value;
					ConvertY(&y);
					y_cal = y;
					}else{
					//printf("APP : y = %d, ", ev.value);
					//y_cal = y;	//// Backup
					y = ev.value;
					ConvertY(&y);
					//DrawLine(fbp, x_cal, y_cal, x, y);
					//DrawLineExact(fbp, x_cal, y_cal, x, y);
					//bresenham(fbp, x_cal, y_cal, x, y);
				tData.xyz[count].x = x;
				tData.xyz[count].y = y;
				tData.xyz[count].z = p;
				tData.xyz[count].seq = tData.nStep;
				count++;
				tData.nCount = count;
					// Make average
					if(count%10 == 0){
						x_sum[count%10] = x;
						y_sum[count%10] = y;
						average10(x_sum, y_sum, &x, &y);
						bresenham(fbp, x_cal, y_cal, x, y);
						x_cal = x;
						y_cal = y;
					}else{
						x_sum[count%10] = x;
						y_sum[count%10] = y;
					}
					}
				}else if(ev.code == 24){
					//printf("APP : p = %d, ", ev.value);
					p = ev.value;
				}
				//printf("APP : ev.code=%d,", ev.code);
				//printf("APP : ev.value=%d\n", ev.value);
				//printf("\n");
				//ConvertXY(&x, &y);
				//if(y != -1 && x != -1){
					//DrawLine(fbp, x_cal, y_cal, x, y);
				//}
				//printf("APP : x=%d, y=%d\n", x, y);
			}
		}
		if(count > 1500){
			break;
		}
	}
#endif

	tData.isThreadEnd = 1;

	munmap(fbp, 800*480*3);
EXIT:
	close(fb_fd);
	close(fd);
	free(fb);

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

	sleep(2);

	return 0;
}



void ConvertX(int* x){
	int ConvX = 0;

	if(*x < 300){
		ConvX = 0;
	}else if(*x > 3800){
		ConvX = 798;
	}else{
		ConvX = *x / 5;
	}

	*x = ConvX;
}



void ConvertY(int* y){
	int ConvY = 0;

	if(*y < 600){
		ConvY = 478;
	}else if(*y > 3500){
		ConvY = 0;
	}else{
		ConvY = 478 - (*y / 8);
	}

	*y = ConvY;
}



void ConvertXY(int* x, int* y){
	int ConvX = 0, ConvY = 0;

	if(*x < 300){
		ConvX = 0;
	}else if(*x > 3800){
		ConvX = 797;
	}else{
		ConvX = *x / 5;
	}

	if(*y < 600){
		ConvY = 477;
	}else if(*y > 3500){
		ConvY = 0;
	}else{
		ConvY = 477 - (*y / 8);
	}

	*x = ConvX;*y = ConvY;
}



void DrawXY(unsigned char *fbp, int x, int y){
	int pos = (y*800*3) + (x*3);

	fbp[pos+0] = 0x00;
	fbp[pos+1] = 0x00;
	fbp[pos+2] = 0xFF;
	fbp[pos+3] = 0x00;
	fbp[pos+4] = 0x00;
	fbp[pos+5] = 0xFF;
	fbp[pos+6] = 0x00;
	fbp[pos+7] = 0x00;
	fbp[pos+8] = 0xFF;

	pos += 800*3;// Next line

	fbp[pos+0] = 0x00;
	fbp[pos+1] = 0x00;
	fbp[pos+2] = 0xFF;
	fbp[pos+3] = 0x00;
	fbp[pos+4] = 0x00;
	fbp[pos+5] = 0xFF;
	fbp[pos+6] = 0x00;
	fbp[pos+7] = 0x00;
	fbp[pos+8] = 0xFF;

	pos += 800*3;// Next line

	fbp[pos+0] = 0x00;
	fbp[pos+1] = 0x00;
	fbp[pos+2] = 0xFF;
	fbp[pos+3] = 0x00;
	fbp[pos+4] = 0x00;
	fbp[pos+5] = 0xFF;
	fbp[pos+6] = 0x00;
	fbp[pos+7] = 0x00;
	fbp[pos+8] = 0xFF;
}



void DrawLine(unsigned char *fbp, int x, int y, int ex, int ey){
	int pos = (y*800*3) + (x*3);
	int i, j, k;
	int dx, dy, dirx, diry, diff, diff_abs;

	fbp[pos+0] = 0x00;
	fbp[pos+1] = 0x00;
	fbp[pos+2] = 0xFF;/*
	fbp[pos+3] = 0x00;
	fbp[pos+4] = 0x00;
	fbp[pos+5] = 0xFF;
	fbp[pos+6] = 0x00;
	fbp[pos+7] = 0x00;
	fbp[pos+8] = 0xFF;*/

	if(x == ex && y == ey){
	}else if(x == ex){
		if(y < ex){
			for(i=0;i<(ey-y);i++){
				pos = pos + (800*3);
				fbp[pos+0] = 0x00;
				fbp[pos+1] = 0x00;
				fbp[pos+2] = 0xFF;
			}
		}else{
			for(i=0;i<(y-ey);i++){
				pos = pos - (800*3);
				fbp[pos+0] = 0x00;
				fbp[pos+1] = 0x00;
				fbp[pos+2] = 0xFF;
			}
		}
	}else if(y == ey){
		if(x < ex){
			for(i=0;i<(ex-x)*3;i+=3){
				pos = pos + 3;
				fbp[pos+0] = 0x00;
				fbp[pos+1] = 0x00;
				fbp[pos+2] = 0xFF;
			}
		}else{
			for(i=0;i<(x-ex)*3;i+=3){
				pos = pos - 3;
				fbp[pos+0] = 0x00;
				fbp[pos+1] = 0x00;
				fbp[pos+2] = 0xFF;
			}
		}
	}else{
		if(x < ex){
			dx = ex - x;
			dirx = 1;
		}else{
			dx = x - ex;
			dirx = -1;
		}
		if(y < ey){
			dy = ey - y;
			diry = 1;
		}else{
			dy = y - ey;
			diry = -1;
		}

		//
		if(dx > dy){
			diff = dx / dy;
			dirx = dirx*diff;
			diff_abs = dy;
		}else{
			diff = dy / dx;
			diry = diry*diff;
			diff_abs = dx;
		}

		for(i=0;i<diff_abs;i++){
			if(dirx > 0){
				for(j=0;j<dirx;j++){
					pos = pos + 3;
					fbp[pos+0] = 0x00;
					fbp[pos+1] = 0x00;
					fbp[pos+2] = 0xFF;
				}
			}else{
				for(j=0;j>dirx;j--){
					pos = pos - 3;
					fbp[pos+0] = 0x00;
					fbp[pos+1] = 0x00;
					fbp[pos+2] = 0xFF;
				}
			}
			if(diry > 0){
				for(j=0;j<diry;j++){
					pos = pos + (800*3);
					fbp[pos+0] = 0x00;
					fbp[pos+1] = 0x00;
					fbp[pos+2] = 0xFF;
				}
			}else{
				for(j=0;j>diry;j--){
					pos = pos - (800*3);
					fbp[pos+0] = 0x00;
					fbp[pos+1] = 0x00;
					fbp[pos+2] = 0xFF;
				}
			}
		}
	}
}



void DrawLineExact(unsigned char *fbp, int x, int y, int ex, int ey){
	int pos = (y*800*3) + (x*3);
	int yy = y;
	int i, j, k;
	int dx, dy, dirx, diry, diff, diff_abs;

	fbp[pos+0] = 0x00;
	fbp[pos+1] = 0x00;
	fbp[pos+2] = 0xFF;

	if(x == ex && y == ey){
	}else if(x == ex){
		if(y<ey)	dy=ey-y;
		else		dy=y-ey;
		printf("y=%d,ey=%d:(x=%d)%d\n", y, ey, x, dy);
		if(y < ey){
			for(i=0;i<(ey-y);i++){
				pos = pos + (800*3);
				if(ey-y == 6){
				fbp[pos+0] = 0x00;
				fbp[pos+1] = 0xFF;
				fbp[pos+2] = 0x00;
				}else{
				fbp[pos+0] = 0x00;
				fbp[pos+1] = 0xFF;
				fbp[pos+2] = 0x00;
				}
			}
		}else{
			for(i=0;i<(y-ey);i++){
				pos = pos - (800*3);
				if(y-ey == 6){
					printf("=x=%d,y=%d\n", x, y);
				fbp[pos+0] = 0xFF;
				fbp[pos+1] = 0x00;
				fbp[pos+2] = 0x00;
				}else{
				fbp[pos+0] = 0xFF;
				fbp[pos+1] = 0x00;
				fbp[pos+2] = 0x00;
				}
			}
		}
	}else if(y == ey){
		if(x < ex){
			for(i=0;i<(ex-x)*3;i+=3){
				pos = pos + 3;
				fbp[pos+0] = 0x00;
				fbp[pos+1] = 0x00;
				fbp[pos+2] = 0xFF;
			}
		}else{
			for(i=0;i<(x-ex)*3;i+=3){
				pos = pos - 3;
				fbp[pos+0] = 0x00;
				fbp[pos+1] = 0x00;
				fbp[pos+2] = 0xFF;
			}
		}
	}else{
		if(x < ex){
			dx = ex - x;
			dirx = 1;
		}else{
			dx = x - ex;
			dirx = -1;
		}
		if(y < ey){
			dy = ey - y;
			diry = 1;
		}else{
			dy = y - ey;
			diry = -1;
		}
if(ex == 246)	printf("ex=%d, dx=%d, ey=%d, dy=%d\n", ex, dx, ey, dy);
		//
		if(dx == dy){
			if(x < ex){
				if(y < ey){	// X++ Y++
					for(i=0;i<dx*3;i+=3){
						pos = pos + (800*3);
						pos = pos + 3;
						fbp[pos+0] = 0x00;
						fbp[pos+1] = 0x00;
						fbp[pos+2] = 0xFF;
					}
				}else{		// X++ Y--
					for(i=0;i<dx*3;i+=3){
						pos = pos - (800*3);
						pos = pos + 3;
						fbp[pos+0] = 0x00;
						fbp[pos+1] = 0x00;
						fbp[pos+2] = 0xFF;
					}
				}
			}else{
				if(y < ey){	// X-- Y++
					for(i=0;i<dx*3;i+=3){
						pos = pos + (800*3);
						pos = pos - 3;
						fbp[pos+0] = 0x00;
						fbp[pos+1] = 0x00;
						fbp[pos+2] = 0xFF;
					}
				}else{		// X-- Y--
					for(i=0;i<dx*3;i+=3){
						pos = pos - (800*3);
						pos = pos - 3;
						fbp[pos+0] = 0x00;
						fbp[pos+1] = 0x00;
						fbp[pos+2] = 0xFF;
					}
				}
			}
		}else if(dx > dy){
			diff = dx / dy;
			dirx = dirx*diff;
			diff_abs = dy;

			if(x < ex){
				if(y < ey){	// X++ Y++
					for(i=0;i<dx*3;i+=3){
						if(((i/3)%diff)+1 == diff){
							pos = pos + (800*3);
						}
						pos = pos + 3;
						fbp[pos+0] = 0x00;
						fbp[pos+1] = 0x00;
						fbp[pos+2] = 0xFF;
					}
				}else{		// X++ Y--
					for(i=0;i<dx*3;i+=3){
						if(ex==246)	printf("diff=%d, i=%d\n", diff, i);
						if(((i/3)%diff)+1 == diff){
							pos = pos - (800*3);
						}
						pos = pos + 3;
						fbp[pos+0] = 0x00;
						fbp[pos+1] = 0x00;
						fbp[pos+2] = 0xFF;
					}
				}
			}else{
				if(y < ey){	// X-- Y++
					for(i=0;i<dx*3;i+=3){
						if(((i/3)%diff)+1 == diff){
							pos = pos + (800*3);
						}
						pos = pos - 3;
						fbp[pos+0] = 0x00;
						fbp[pos+1] = 0x00;
						fbp[pos+2] = 0xFF;
					}
				}else{		// X-- Y--
					for(i=0;i<dx*3;i+=3){
						if(((i/3)%diff)+1 == diff){
							pos = pos - (800*3);
						}
						pos = pos - 3;
						fbp[pos+0] = 0x00;
						fbp[pos+1] = 0x00;
						fbp[pos+2] = 0xFF;
					}
				}
			}
		}else{
			diff = dy / dx;
			diry = diry*diff;
			diff_abs = dx;

			if(y < ey){
				if(x < ex){	// X++ Y++
					for(i=0;i<dy;i++){
						pos = pos + (800*3);
						fbp[pos+0] = 0x00;
						fbp[pos+1] = 0x00;
						fbp[pos+2] = 0xFF;
						if((i%diff)+1 == diff){
							pos = pos + 3;
						}
					}
				}else{		// X-- Y++
					for(i=0;i<dy;i++){
						pos = pos + (800*3);
						fbp[pos+0] = 0x00;
						fbp[pos+1] = 0x00;
						fbp[pos+2] = 0xFF;
						if((i%diff)+1 == diff){
							pos = pos - 3;
						}
					}
				}
			}else{
				if(x < ex){	// X++ Y--
					for(i=0;i<dy;i++){
						pos = pos - (800*3);
						fbp[pos+0] = 0x00;
						fbp[pos+1] = 0x00;
						fbp[pos+2] = 0xFF;
						if((i%diff)+1 == diff){
							pos = pos + 3;
						}
					}
				}else{		// X-- Y--
					for(i=0;i<dy;i++){
						pos = pos - (800*3);
						fbp[pos+0] = 0x00;
						fbp[pos+1] = 0x00;
						fbp[pos+2] = 0xFF;
						if((i%diff)+1 == diff){
							pos = pos - 3;
						}
					}
				}
			}
		}
	}
}



void	DrawPoint(unsigned char *fbp, int x, int y, int size){
	int pos = (y*800*3) + (x*3);

	//for(i=0;i<size;i++){
		fbp[pos+0] = 0x00;
		fbp[pos+1] = 0x00;
		fbp[pos+2] = 0xFF;
#if 0
		fbp[pos+3] = 0x00;
		fbp[pos+4] = 0x00;
		fbp[pos+5] = 0x00;

		pos = pos + (800*3);

		fbp[pos+0] = 0x00;
		fbp[pos+1] = 0x00;
		fbp[pos+2] = 0x00;
		fbp[pos+3] = 0x00;
		fbp[pos+4] = 0x00;
		fbp[pos+5] = 0x00;
#endif
	//}
}



void bresenham(unsigned char *fbp, int x0, int y0, int x1, int y1){
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = (dx > dy ? dx : -dy) / 2;

	while(DrawPoint(fbp, x0, y0, 1), x0 != x1 || y0 != y1){
		int e2 = err;
		if(e2 > -dx) { err -= dy; x0 += sx; }
		if(e2 < dy) { err += dx; y0 += sy; }
	}
}



void average10(int *xsum, int *ysum, int *x, int *y){
	int i, j;
	int xx = 0, yy = 0;
	int temp;

	// Sort X
	for(i=0;i<10;i++){
		for(j=i;j<9;j++){
			if(xsum[j] > xsum[j+1]){
				temp = xsum[j];
				xsum[j] = xsum[j+1];
				xsum[j+1] = temp;
			}
		}
	}

	// Sort Y
	for(i=0;i<10;i++){
		for(j=i;j<9;j++){
			if(ysum[j] > ysum[j+1]){
				temp = ysum[j];
				ysum[j] = ysum[j+1];
				ysum[j+1] = temp;
			}
		}
	}
#if 0
	printf("X");
	for(i=0;i<10;i++){
		printf(",%d", xsum[i]);
	}
	printf("\nY");
	for(i=0;i<10;i++){
		printf(",%d", ysum[i]);
	}
	printf("\n");
#endif

	for(i=1;i<9;i++){
		xx = xx + xsum[i];
		yy = yy + ysum[i];
	}
	//printf("xsum=%d, ysum=%d\n", xx, yy);

	*x = xx/8;
	*y = yy/8;
}
