#include "All_head.h"

volatile unsigned char irdata1,irdata2; //16为地址的前八位和后八位,作为识别数据是否正确
unsigned int endcount; //计数结束（即完成延时）
extern unsigned int count;	 //计数（相当于发送延时）
volatile unsigned char IR_flag = 0;  //红外发射标志


/*******************************************************
每发送0.56ms的38KHz的载波里面一般包括大约21个脉冲周期
					0: 0.56ms/0.565ms 
					1: 0.56ms/1.685ms
********************************************************/
void Send_IRdata(unsigned char x_irdata)
{
	unsigned char i;   //循环移位计数
	unsigned char irdata;  //数据寄存
	
	irdata = x_irdata;
	for(i=0;i<8;i++)
	{
		//先发送0.56ms的38KHz红外波（即编码中0.56ms的低电平）
		count = 0;
		endcount = 42;	//0.56ms / 13.15us = 42 (42 / 2 = 21，即发送0.56ms的载波中包括21个载波周期)
		IR_flag = 1; //发送标志
		while(count < endcount);

		if(irdata&0x01) //判断最低位是0还是1（0则延时(1.125ms-0.56ms=0.565ms)；1则延时(2.25ms-0.56ms=1.685ms)）
		{
			
			endcount = 43;	  //1.685ms / 13.15us = 128
		}
		else endcount = 128;	  //0.565ms / 13.15us = 43

		count = 0;
		IR_flag = 0;	//清除标志位，为下次做准备
		while(count < endcount);

		irdata = irdata >> 1;
	}		  

}
/*******************************************************************
红外数据发送 ： 发射端所发射的一帧码含有一个引导码、8位用户码、8位用户反码，8位键数据码、8位键数据反码
				其中，引导码包括9ms的高电平和4.5ms的低电平
				A logical “1” takes 2.25ms 
				A logical “0” is only half of that, being 1.125ms .

				低电平发送载波（IR_flag = 1）；
				高电平不发送载波（IR_flag = 0）,但仍有数据0发送

				发送时，先发低位，再发高位
*******************************************************************/
void Write_IRdata(unsigned char p_irdata)   
{	
	irdata1 = 3;   //（用户码）自己定义
	irdata2 = 252;	//（用户反码）

	//发送9ms的引导码（高电平）
	
	endcount = 684;	 //9ms / 13.15us = 684
	count = 0;
	IR_flag = 1; //发送标志(发送38K载波)
	while(count < endcount);

	//发送4.5ms的引导码的结束码（低电平）
	count = 0;
	endcount = 342;	 //4.5ms / 13.15us = 342 
	IR_flag = 0; //发送标志（发送0）
	while(count < endcount);
	
	Send_IRdata(irdata1);       //发送16位地址的前八位（8位用户码）
    Send_IRdata(irdata2);      //发送16位地址的后八位（8位用户反码）
	Send_IRdata(p_irdata);	  //发送8位键数据码
	Send_IRdata(~p_irdata); //发送8位键数据反码
	Send_IRdata(0x66);	//结束码（也用于辨别）

	
	//最后发送0.56ms的38KHz红外载波（即编码中0.56ms的低电平
	count = 0;
	endcount = 42;	//0.56ms / 13.15us = 42 (42 / 2 = 21，即发送0.56ms的载波中包括21个载波周期)
	IR_flag = 1; //发送标志
	while(count < endcount);
	IR_flag = 0; 			
}	
