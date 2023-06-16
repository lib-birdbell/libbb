//defined for ioctl
/************************************************/
/*  IOCTL CMD									*/
/************************************************/
#define KICC_LCD_MODE_SET			1
#define KICC_LCDSETCURRENT_XY		2
#define KICC_LCDGETCURRENT_XY		3
#define KICC_LCDBCKLIGHT_ON			4
#define KICC_LCDBCKLIGHT_OFF		5
#define KICC_LCDCONTRAST			6
#define KICC_LCDBCKLIGHT_TIME		7
#define KICC_LCD_FONT8_MODE			8
#define KICC_LCD_FONT16_MODE		9
#define KICC_LCD_RST_MODE			10
#define KICC_LCD_BL_BRIGHT			11
#define KICC_LCD_REFLESH			12
#define KICC_LCD_CLEAR				13
#define KICC_LCD_OFFSET				14
#define KICC_LCD_SLEEP_MODE			15

#define KICC_LCD_INST_DATA			0
#define KICC_LCD_CMD_DATA			1
#define KICC_LCD_FONT8				0
#define KICC_LCD_FONT16				1

/************************************************/
/*  PARAMETER									*/
/************************************************/
#define LCD_RST_LOW             0x00
#define LCD_RST_HIGH            0x01
#define LCD_BACKLIGHT_ON        0x01
#define LCD_BACKLIGHT_OFF       0x00
