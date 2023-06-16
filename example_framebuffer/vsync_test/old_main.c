#include <linux/ioctl.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <fcntl.h>  
#include "kicc_lcd_cmd.h"
#include "eng6x8.h"

#define COLOR_RED	0xF800
#define COLOR_YELLO	0x07E0
#define	COLOR_BLUE	0x001F
#define COLOR_WHITE	0xFFFF
#define COLOR_BLACK	0x0000

#define	COLOR_FONT	COLOR_BLUE
#define COLOR_BACK	COLOR_WHITE

#define X_MAX	240	// x colume
#define Y_MAX	320 //y page

typedef unsigned long	u32;
typedef unsigned short	u16;
typedef unsigned char	u8;

typedef struct bmp_data_struct
{
//	unsigned int width;
//	unsigned int height;
//	unsigned int size;
//	unsigned int *data;
	u16	width;
	u16	height;
	u32 size;
//	u16	*data;
}bmp_data_str;

struct lcd_position_struct
{
	unsigned int sx_pos;
	unsigned int sy_pos;
	unsigned int ex_pos;
	unsigned int ey_pos;

}__attribute__((packed))lcd_position;

struct lcd_offset_struct
{
	unsigned int x;
	unsigned int y;

}__attribute__((packed))lcd_offset;

static inline void lcd_putc_xy (u16 x, u16 y, u8  c);
static void	lcd_drawchars  (u16 x, u16 y, u8 *str, int count);

void cmd_lcd_position(u16 sx, u16 sy, u16 ex, u16 ey);
void cmd_help(void);
unsigned int bmp_load(char *file_name);
int fd_fd;
u16	lcd_data[240*320];



int main(int argc, char *argv[])
{
	int i,j;
	int fd;
	FILE *pic_header;
//	unsigned int lcd_data[240*320];
	
	unsigned int cmd,value;
	struct bmp_data_struct bmp_data;

	cmd = 0;
	value = 0;

	if(argc < 2)
	{
		printf("usage : ./app_lcd cmd value\n");
		printf("ex : ./app_lcd 01 300\n");
		printf("ex : ./app_lcd bmp file_name\n");

		exit(0);
	}

	fd = open("/dev/kicc_lcd",O_RDWR|O_NDELAY);//,O_WRONLY|O_NDELAY);
	if(fd < 0) 
	{
		printf("device open error\n");
		exit(0);
	}
	fd_fd = fd;

	if(strcmp(argv[1],"bmp") == 0)
	{
		printf("bmp load\n");

		bmp_load(argv[2]);
//		memcpy(&bmp_data,(u16 *)bmp_load(argv[2]),sizeof(bmp_data));
		memcpy(&bmp_data,(unsigned int *)bmp_load(argv[2]),sizeof(bmp_data));
		printf("lcd_data size : %d\n",sizeof(lcd_data));
		write(fd,lcd_data,bmp_data.size);

	}

	if(strcmp(argv[1],"header") == 0)
	{
		pic_header= fopen("kicc_logo.h","wr"); 
			
//		bmp_load(argv[2]);
		memcpy(&bmp_data,(unsigned int *)bmp_load(argv[2]),sizeof(bmp_data));

		fprintf(pic_header,"#define BMP_LOGO_WIDTH		%d\n",bmp_data.width);
		fprintf(pic_header,"#define BMP_LOGO_HEIGHT		%d\n",bmp_data.height);
		fprintf(pic_header,"\n");

		fprintf(pic_header,"u16 LOGOIMAGE[]={");

		printf("size : [%08d] [%4d]\n",bmp_data.size,bmp_data.size/8);

		for(j=0;j<(bmp_data.size/2);j++)
		{
			if((j%240)==0) fprintf(pic_header,"\n/*line: [%03d]********************************************************/",(j/240));
			if((j%8)==0) fprintf(pic_header,"\n/*%08X*/  ",j*16);

//			for(i=0;i<8;i++)
//			{
			fprintf(pic_header,"0x%04X, ",lcd_data[j]);
//			}

		}
		fprintf(pic_header,"\n");
		fprintf(pic_header,"};\n");

		fclose(pic_header);

	}

	if(strcmp(argv[1],"cmd") == 0)
	{
		printf("cmd mode\n");
		if(argc < 3){
			printf("cmd error\n");
			exit(0);
		}
			sscanf(argv[2], "%xl", &cmd);
			sscanf(argv[3], "%xl", &value);
			ioctl(fd,cmd,&value);
	}

	if(strcmp(argv[1],"position") == 0)
	{
		printf("position set(sx sy ex ey)\n");
		
		sscanf(argv[2], "%dl", &lcd_position.sx_pos);
		sscanf(argv[3], "%dl", &lcd_position.sy_pos);
		sscanf(argv[4], "%dl", &lcd_position.ex_pos);
		sscanf(argv[5], "%dl", &lcd_position.ey_pos);
		ioctl(fd,2,&lcd_position);
	}


	if(strcmp(argv[1],"clear") == 0)
	{
		printf("clear\n");
		
		sscanf(argv[2], "%xl", &value);

		ioctl(fd,13,&value);
	}

	if(strcmp(argv[1],"size") == 0)
	{
		printf("size int : [%d]\n",sizeof(int));
		printf("size long : [%d]\n",sizeof(long));
		printf("size char : [%d]\n",sizeof(char));
		printf("size double : [%d]\n",sizeof(double));

	}

	if(strcmp(argv[1],"offset") == 0)
	{
		sscanf(argv[2], "%dl", &lcd_offset.x);
		sscanf(argv[3], "%dl", &lcd_offset.y);

		ioctl(fd,14,&lcd_offset);

	}

	if(strcmp(argv[1],"read") == 0)
	{
		printf("LCD Buffer\n");

		read(fd,lcd_data,1024);

		for(i=0;i<512;i++)
		{
			if((i%8) ==0 )
			{
				printf("\n");
				printf("%04d : ",i/8);
			}
			printf("%08x ",lcd_data[i]);
		}
		printf("\n");
	}



//test

	if(strcmp(argv[1],"sleep") == 0)
	{

		while(1)
		{

			for(i=0;i<7;i++)
			{
				lcd_position.sx_pos = 0;
				lcd_position.sy_pos = 47*i;

				lcd_position.ex_pos = 239;
				lcd_position.ey_pos = 319;
				ioctl(fd,2,&lcd_position);

//				memcpy(&bmp_data,(u16 *)bmp_load("kicc_ci_240.bmp"),sizeof(bmp_data));
//				write(fd,bmp_data.data,bmp_data.size);
				sleep(1);
			}
			value = 0xffff;
			ioctl(fd,13,&value);
			sleep(1);
			system("echo 1 > /proc/sys/pm/sleep");
		}
	}

	close(fd);

	return 0;
}

void cmd_help(void)
{
	printf("usage : ./app_lcd cmd value\n");

	printf("ex : ./app_lcd 01 300\n");
	printf("ex : ./app_lcd bmp file_name\n");
}

unsigned int bmp_load(char *file_name)
{
	int i,j;
	unsigned int *result;

	u16 data[240*320];
	u16 rev_data[240*320];

	struct bmp_data_struct bmp_data;

	struct struct_bmp_file{
		u8	bmp_magic[2];
		u32	bmp_size;
		u16	dummy0;
		u16	dummy1;
		u32	bmp_data_offset;
		u32 bmp_header_offset;
		u32	bmp_xpixel;
		u32	bmp_ypixel;
		u16	bmp_color_plane;
		u16	bmp_color_depth;
		u32	dummp2[6];
	}__attribute__((packed)) kicc_log;

	FILE *pic_file;

	pic_file = fopen(file_name,"r");

	fread(&kicc_log,1,sizeof(kicc_log),pic_file);

	printf("BMP Magic   : [%c%c]\n",kicc_log.bmp_magic[0],kicc_log.bmp_magic[1]);
	printf("File Size   : [%9d]byte\n",kicc_log.bmp_size);
	printf("Data Offset : [%9x]\n",kicc_log.bmp_data_offset);
	printf("BMP info    : [%3d]x[%3d] \n",kicc_log.bmp_xpixel,kicc_log.bmp_ypixel);
	printf("BMP plasne  : [%9d]\n",kicc_log.bmp_color_plane);
	printf("BMP depth   : [%9d]\n",kicc_log.bmp_color_depth);
	printf("BMP Size    : [%9d]\n", kicc_log.bmp_xpixel*kicc_log.bmp_ypixel*(kicc_log.bmp_color_depth/8));

	fseek(pic_file,kicc_log.bmp_data_offset,SEEK_SET);
//	fread(rev_data,2,(kicc_log.bmp_xpixel * kicc_log.bmp_ypixel),pic_file);
	fread(rev_data,2,kicc_log.bmp_size,pic_file);
	fclose(pic_file);

	for(i=0;i<kicc_log.bmp_ypixel;i++)
	{
//		memcpy(&data[i*X_MAX],&rev_data[(kicc_log.bmp_ypixel-1-i)*X_MAX],(kicc_log.bmp_xpixel*kicc_log.bmp_color_depth/8));
		memcpy(&data[i*X_MAX],&rev_data[((kicc_log.bmp_ypixel-1)-i)*(X_MAX)],X_MAX*2);
		if(i==0)  printf("[%03d] t : [%04d] d : [%04d]\n",i,i*X_MAX,(kicc_log.bmp_ypixel-1-i)*X_MAX);
		if(i==318)printf("[%03d] t : [%04d] d : [%04d]\n",i,i*X_MAX,(kicc_log.bmp_ypixel-1-i)*X_MAX);
		if(i==317)
		{
			printf("[%03d] t : [%04d] d : [%04d]\n",i,i*X_MAX,(kicc_log.bmp_ypixel-1-i)*X_MAX);
			for(j=0;j<10;j++)
			{
				printf("%03d : data[%08X][%04X] rev_data[%08X][%04X]\n"
				,j,(i*X_MAX+j),data[i*X_MAX+j],(i*X_MAX,(kicc_log.bmp_ypixel-1-i)*X_MAX+j),rev_data[i*X_MAX,(kicc_log.bmp_ypixel-1-i)*X_MAX+j]);
			}

		}
	}
//	printf("bmp : [%03d][%03d]\n",kicc_log.bmp_ypixel,i);
				
	bmp_data.width = kicc_log.bmp_xpixel;
	bmp_data.height = kicc_log.bmp_ypixel;
	bmp_data.size = kicc_log.bmp_xpixel*kicc_log.bmp_ypixel*(kicc_log.bmp_color_depth/8);
//	bmp_data.data = (u16 *)data;
	memcpy(lcd_data,data,sizeof(data));

	result =(unsigned int)&bmp_data;
	return result;
//	return 0;
}

void cmd_lcd_position(u16 sx, u16 sy, u16 ex, u16 ey)
{
	lcd_position.sx_pos = sx;
	lcd_position.sy_pos = sy;

	lcd_position.sx_pos = ex;
	lcd_position.sy_pos = ey;

	ioctl(fd_fd,2,&lcd_position);
}
