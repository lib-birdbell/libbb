#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <sys/time.h>


#define XRES	240
#define YRES	320

void makeScreen16bit(unsigned short *p, int x, int y);

// http://webnautes.tistory.com/635
int main(int argc, char* argvp[]){
	int framebuffer_fd = 0;
	struct fb_var_screeninfo framebuffer_variable_screeninfo;
	struct fb_fix_screeninfo framebuffer_fixed_screeninfo;
	unsigned short *pBuf;
	int dummy = 0;
	int ret = 0;
	int i = 0;
	struct timeval tv;
	unsigned long time_us_start, time_us_end;
	
	// Time init
	gettimeofday(&tv, NULL);

	framebuffer_fd = open("/dev/fb0", O_RDWR);
	if( framebuffer_fd < 0){
		perror("Error : cannot open framebuffer device\n");
		exit(1);
	}

	if(ioctl(framebuffer_fd, FBIOGET_VSCREENINFO, &framebuffer_variable_screeninfo)){
		perror("Error : reading variable screen infomation 1\n");
		exit(1);
	}
	printf(" %d x %d , %d bpp\n", framebuffer_variable_screeninfo.xres, framebuffer_variable_screeninfo.yres, framebuffer_variable_screeninfo.bits_per_pixel);


	//framebuffer_variable_screeninfo.bits_per_pixel=32;
	framebuffer_variable_screeninfo.bits_per_pixel=16;
	if(ioctl(framebuffer_fd, FBIOPUT_VSCREENINFO, &framebuffer_variable_screeninfo)){
		perror("Error : writing variable screen infomation 2\n");
		exit(1);
	}

	if(ioctl(framebuffer_fd, FBIOGET_FSCREENINFO, &framebuffer_fixed_screeninfo)){
		perror("Error : reading variable screen infomation 3\n");
		exit(1);
	}


	printf("framebuffer Display information\n");
	printf(" %d x %d , %d bpp\n", framebuffer_variable_screeninfo.xres, framebuffer_variable_screeninfo.yres, framebuffer_variable_screeninfo.bits_per_pixel);

	int width = framebuffer_variable_screeninfo.xres;
	int height = framebuffer_variable_screeninfo.yres;
	int bpp = framebuffer_variable_screeninfo.bits_per_pixel/8;
	int xoffset = framebuffer_variable_screeninfo.xoffset;
	int yoffset = framebuffer_variable_screeninfo.yoffset;


	long int screensize = width * height * bpp;

	// Buffer allocation
	pBuf = (unsigned short*)malloc(screensize);
	if(!pBuf){
		perror("Error : malloc failed!\n");
		close(framebuffer_fd);
		exit(1);
	}

	// Set Buffer
	makeScreen16bit(pBuf, width, height);

	printf("Screen size=%d(%d x %d x %d)\n", screensize, width, height, bpp);
	char *framebuffer_pointer = (char*)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, framebuffer_fd, 0);

	if(framebuffer_pointer == MAP_FAILED){
		perror("Error : mmap\n");
		exit(1);
	}else{
		int x = 0, y = 0;
		printf("xoffset = %d, yoffset = %d\n", xoffset, yoffset);
		printf("line_length = %d\n", framebuffer_fixed_screeninfo.line_length);
		printf("screensize = %d\n", screensize);
		printf("framebuffer_pointer=%p\n", framebuffer_pointer);
		//height = 240;
	}

	gettimeofday(&tv, NULL);
	time_us_start = 1000000 * tv.tv_sec + tv.tv_usec;
	printf("start time=%d\n", time_us_start);
	for(i=0;i<60*10;i++){
		ret = ioctl(framebuffer_fd, FBIO_WAITFORVSYNC, &dummy);
		//ret = ioctl(framebuffer_fd, FBIOBLANK, &dummy);
		//framebuffer_variable_screeninfo.xoffset = 0;
		//framebuffer_variable_screeninfo.yoffset = 0;
		//framebuffer_variable_screeninfo.activate = FB_ACTIVATE_VBL;
		//ret = ioctl(framebuffer_fd, FBIOPAN_DISPLAY, &framebuffer_variable_screeninfo);
		if(ret < 0){
			printf("ioctl ret=%d\n", ret);
		}
	}
	gettimeofday(&tv, NULL);
	time_us_end = 1000000 * tv.tv_sec + tv.tv_usec;
	printf("end time=%d(%d)\n", time_us_end, time_us_end - time_us_start);

	sleep(1);
	munmap(framebuffer_pointer, screensize);
	close(framebuffer_fd);

	free(pBuf);

	printf("program end\n");
	
	return 0;
}



/*
 * Function name	: makeScreen
 * Remarks		: Fill Black/RGB color
 */
void makeScreen16bit(unsigned short *p, int width, int height){
	int i;
	int screensize = width*height;
	int nDevide = height/4;

	for(i=0;i<screensize;i++){
		if(i<(width*nDevide)){
			p[i] = 0xFFFF;	// BLACK
		} else if(i<(width*nDevide*2)){
			p[i] = 0xF800;	// RED
		} else if(i<(width*nDevide*3)){
			p[i] = 0x07E0;	// GREEN
		} else{
			p[i] = 0x008F;	// BLUE
		}
	}
}
