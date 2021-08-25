#include "stdint.h"
#include "All_define.h"
#include "num_code.h"

void LCD_write_num(uint16_t x,uint16_t y,uint8_t str,unsigned int color,unsigned int xcolor)//写字符
{
    uint16_t xpos = x;
    uint16_t ypos = y;

    unsigned char avl,i,n;

    for(i=0; i<16; i++) //16行（高--正看字符）
    {
        avl= (num_code[str-32][i]);
        lcd_SetCursor(xpos,ypos);
        ypos++;
        rw_data_prepare();
        for(n=0; n<8; n++) //8列（宽--正看字符）
        {
            if(avl&0x80) write_data(color);	  //因为0x80---8位（与8列对应）
            else write_data(xcolor);
            avl<<=1;
        }
    }
}

void LCD_write_num_string(uint16_t x,uint16_t y,char *s,unsigned int color,unsigned int xcolor)//英文字符串显示
{
	while (*s)
    {
        LCD_write_num(x,y,*s,color,xcolor);
        s++; //即s+=1；+1是因为一个字符为1个字节——（指针加1为跳过8位）
        x += 8;
    }
}
