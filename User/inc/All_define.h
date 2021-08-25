#include "stm32f10x.h"
#include "stdint.h"

/* LCD color */
#define White            0xFFFF
#define Black            0x0000
#define Grey             0xF7DE
#define Blue             0x001F
#define Blue2            0x051F	  //ǳ��
#define Red              0xF800
#define Magenta          0xF81F	  //�ۺ�
#define Green            0x07E0
#define Cyan             0x7FFF	  //����
#define Yellow           0xFFE0
#define	blue_l     		 0x07ff//ǳ��
#define zise      		 0xf81f//��
#define chase     		 0xd5b1//��ɫ
#define hese       	     0xa285//��ɫ
#define tianlanse 		 0x05ff//����ɫ
#define jialanse  		 0x435c//�ʼ���
#define bohese     		 0xf5ff//����ɫ
#define huise      		 0x0000//��ɫ
#define yinbaise   		 0xc618//����ɫ 
#define qianhuise  		 0x8410//ǳ��ɫ
#define chengse    		 0xfbea//��ɫ

#define LCD_WIDTH       320                 /* Screen Width (in pixels)           */
#define LCD_HEIGHT      240                 /* Screen Hight (in pixels)           */

extern void count_treat(void);
//���������ʼ������
extern void Init_Config(void);
//LCD��ʼ��
extern void lcd_Initializtion(void);
//��������
extern void lcd_SetCursor(unsigned int x,unsigned int y);
//��ȡLCD��ID
extern unsigned int lcd_getdeviceid(void);
//����
extern void lcd_clear(unsigned short Color);
extern void rw_data_prepare(void);
extern void write_data(unsigned short data );

extern void LCD_write_num_string(uint16_t x,uint16_t y,char *s,unsigned int color,unsigned int xcolor);//Ӣ���ַ�����ʾ
