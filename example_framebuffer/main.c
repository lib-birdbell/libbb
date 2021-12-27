#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>		// ioctl
#include <linux/ioctl.h>	// FBIO_WAITFORVSYNC
#include <errno.h>		// error
#include <linux/fb.h>
#include <sys/mman.h>



// http://webnautes.tistory.com/635
int main(int argc, char* argvp[]){
	int fb_fd = 0;
	struct fb_var_screeninfo framebuffer_variable_screeninfo;
	struct fb_fix_screeninfo framebuffer_fixed_screeninfo;

	fb_fd = open("/dev/fb0", O_RDWR);
	if( fb_fd < 0){
		perror("Error : cannot open framebuffer device\r\n");
		exit(1);
	}

	if(ioctl(fb_fd, FBIOGET_VSCREENINFO, &framebuffer_variable_screeninfo)){
		perror("Error : reading variable screen infomation 1\r\n");
		exit(1);
	}

/*
	framebuffer_variable_screeninfo.bits_per_pixel=32;
	if(ioctl(fb_fd, FBIOPUT_VSCREENINFO, &framebuffer_variable_screeninfo)){
		perror("Error : writing variable screen infomation 2\r\n");
		exit(1);
	}
*/
	if(ioctl(fb_fd, FBIOGET_FSCREENINFO, &framebuffer_fixed_screeninfo)){
		perror("Error : reading variable screen infomation 3\r\n");
		exit(1);
	}

	printf("framebuffer Display information\r\n");
	printf(" %d x %d , %d bpp\r\n", framebuffer_variable_screeninfo.xres, framebuffer_variable_screeninfo.yres, framebuffer_variable_screeninfo.bits_per_pixel);

	int width = framebuffer_variable_screeninfo.xres;
	int height = framebuffer_variable_screeninfo.yres;
	int bpp = framebuffer_variable_screeninfo.bits_per_pixel/8;
	int xoffset = framebuffer_variable_screeninfo.xoffset;
	int yoffset = framebuffer_variable_screeninfo.yoffset;


	long int screensize = width * height * bpp;

	printf("%d = %d x %d x %d\r\n", screensize, width, height, bpp);
	char *framebuffer_pointer = (char*)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);

	if(framebuffer_pointer == MAP_FAILED){
		perror("Error : mmap\r\n");
		exit(1);
	}else{
		int x = 0, y = 0;
		printf("xoffset = %d, yoffset = %d\r\n", xoffset, yoffset);
		printf("line_length = %d\r\n", framebuffer_fixed_screeninfo.line_length);
		height = 240;

		for(x=0;x<240*320;x++){//
		//for(x=0;x<screensize;x++){
			framebuffer_pointer[x] = 0xFF;
			//framebuffer_pointer[x] = 0x00;
		}

		for(x=240*320;x<240*320*2;x++){
			framebuffer_pointer[x] = 0x00;
		}

#if 0
		for(y=0;y<height;y++)
		for(x=0;x<width;x++){
			unsigned int pixel_offset = (y+yoffset)*framebuffer_fixed_screeninfo.line_length*2 + (x+xoffset)*bpp;

			if(bpp == 4){
				if(x<=width*1/3){
					framebuffer_pointer[pixel_offset]=255;//B
					framebuffer_pointer[pixel_offset+1]=0;//G
					framebuffer_pointer[pixel_offset+2]=0;//R
					framebuffer_pointer[pixel_offset+3]=0;//A
				}
				if(x>width*1/3 && x<=width*2/3){
					framebuffer_pointer[pixel_offset]=0;//B
					framebuffer_pointer[pixel_offset+1]=255;//G
					framebuffer_pointer[pixel_offset+2]=0;//R
					framebuffer_pointer[pixel_offset+3]=0;//A
				}
				if(x>width*2/3){
					framebuffer_pointer[pixel_offset]=0;//B
					framebuffer_pointer[pixel_offset+1]=0;//G
					framebuffer_pointer[pixel_offset+2]=255;//R
					framebuffer_pointer[pixel_offset+3]=0;//A
				}
			}else if(bpp == 3){
				if(x<120){
					framebuffer_pointer[pixel_offset]=0;//B
					framebuffer_pointer[pixel_offset+1]=0;//G
					framebuffer_pointer[pixel_offset+2]=0;//R
				}else{
					framebuffer_pointer[pixel_offset]=255;//B
					framebuffer_pointer[pixel_offset+1]=255;//G
					framebuffer_pointer[pixel_offset+2]=255;//R
				}
			}else{
				printf(".");
			}
		}
#endif
	}
#if 0
	int ret;
	unsigned int zero;
	ret = ioctl(fb_fd, FBIO_WAITFORVSYNC, &zero);
	printf("FBIO_WAITFORVSYNC=%d\n", ret);
	if(ret < 0){
		printf("%s\n", strerror(errno));
	}
#endif

	munmap(framebuffer_pointer, screensize);
	close(fb_fd);

	printf("program end 3\r\n");
	sleep(1);
	printf("program end 2\r\n");
	sleep(1);
	printf("program end 1\r\n");
	sleep(1);
	printf("program end 0\r\n");
	
	return 0;
}
