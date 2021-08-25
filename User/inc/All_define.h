#include "stm32f10x.h"
#include "stdint.h"

/* LCD color */
#define White            0xFFFF
#define Black            0x0000
#define Grey             0xF7DE
#define Blue             0x001F
#define Blue2            0x051F	  //浅蓝
#define Red              0xF800
#define Magenta          0xF81F	  //粉红
#define Green            0x07E0
#define Cyan             0x7FFF	  //蓝绿
#define Yellow           0xFFE0
#define	blue_l     		 0x07ff//浅蓝
#define zise      		 0xf81f//紫
#define chase     		 0xd5b1//茶色
#define hese       	     0xa285//褐色
#define tianlanse 		 0x05ff//天蓝色
#define jialanse  		 0x435c//皇家蓝
#define bohese     		 0xf5ff//薄荷色
#define huise      		 0x0000//灰色
#define yinbaise   		 0xc618//银白色 
#define qianhuise  		 0x8410//浅灰色
#define chengse    		 0xfbea//橙色

#define LCD_WIDTH       320                 /* Screen Width (in pixels)           */
#define LCD_HEIGHT      240                 /* Screen Hight (in pixels)           */

extern void count_treat(void);
//各种外设初始化配置
extern void Init_Config(void);
//LCD初始化
extern void lcd_Initializtion(void);
//设置坐标
extern void lcd_SetCursor(unsigned int x,unsigned int y);
//获取LCD的ID
extern unsigned int lcd_getdeviceid(void);
//清屏
extern void lcd_clear(unsigned short Color);
extern void rw_data_prepare(void);
extern void write_data(unsigned short data );

extern void LCD_write_num_string(uint16_t x,uint16_t y,char *s,unsigned int color,unsigned int xcolor);//英文字符串显示
