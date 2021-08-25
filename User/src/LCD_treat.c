#include "stdint.h"
#include "All_define.h"
#include "num_code.h"

void LCD_write_num(uint16_t x,uint16_t y,uint8_t str,unsigned int color,unsigned int xcolor)//д�ַ�
{
    uint16_t xpos = x;
    uint16_t ypos = y;

    unsigned char avl,i,n;

    for(i=0; i<16; i++) //16�У���--�����ַ���
    {
        avl= (num_code[str-32][i]);
        lcd_SetCursor(xpos,ypos);
        ypos++;
        rw_data_prepare();
        for(n=0; n<8; n++) //8�У���--�����ַ���
        {
            if(avl&0x80) write_data(color);	  //��Ϊ0x80---8λ����8�ж�Ӧ��
            else write_data(xcolor);
            avl<<=1;
        }
    }
}

void LCD_write_num_string(uint16_t x,uint16_t y,char *s,unsigned int color,unsigned int xcolor)//Ӣ���ַ�����ʾ
{
	while (*s)
    {
        LCD_write_num(x,y,*s,color,xcolor);
        s++; //��s+=1��+1����Ϊһ���ַ�Ϊ1���ֽڡ�����ָ���1Ϊ����8λ��
        x += 8;
    }
}
