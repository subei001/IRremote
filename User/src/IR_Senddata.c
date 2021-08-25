#include "All_head.h"

volatile unsigned char irdata1,irdata2; //16Ϊ��ַ��ǰ��λ�ͺ��λ,��Ϊʶ�������Ƿ���ȷ
unsigned int endcount; //�����������������ʱ��
extern unsigned int count;	 //�������൱�ڷ�����ʱ��
volatile unsigned char IR_flag = 0;  //���ⷢ���־


/*******************************************************
ÿ����0.56ms��38KHz���ز�����һ�������Լ21����������
					0: 0.56ms/0.565ms 
					1: 0.56ms/1.685ms
********************************************************/
void Send_IRdata(unsigned char x_irdata)
{
	unsigned char i;   //ѭ����λ����
	unsigned char irdata;  //���ݼĴ�
	
	irdata = x_irdata;
	for(i=0;i<8;i++)
	{
		//�ȷ���0.56ms��38KHz���Ⲩ����������0.56ms�ĵ͵�ƽ��
		count = 0;
		endcount = 42;	//0.56ms / 13.15us = 42 (42 / 2 = 21��������0.56ms���ز��а���21���ز�����)
		IR_flag = 1; //���ͱ�־
		while(count < endcount);

		if(irdata&0x01) //�ж����λ��0����1��0����ʱ(1.125ms-0.56ms=0.565ms)��1����ʱ(2.25ms-0.56ms=1.685ms)��
		{
			
			endcount = 43;	  //1.685ms / 13.15us = 128
		}
		else endcount = 128;	  //0.565ms / 13.15us = 43

		count = 0;
		IR_flag = 0;	//�����־λ��Ϊ�´���׼��
		while(count < endcount);

		irdata = irdata >> 1;
	}		  

}
/*******************************************************************
�������ݷ��� �� ������������һ֡�뺬��һ�������롢8λ�û��롢8λ�û����룬8λ�������롢8λ�����ݷ���
				���У����������9ms�ĸߵ�ƽ��4.5ms�ĵ͵�ƽ
				A logical ��1�� takes 2.25ms 
				A logical ��0�� is only half of that, being 1.125ms .

				�͵�ƽ�����ز���IR_flag = 1����
				�ߵ�ƽ�������ز���IR_flag = 0��,����������0����

				����ʱ���ȷ���λ���ٷ���λ
*******************************************************************/
void Write_IRdata(unsigned char p_irdata)   
{	
	irdata1 = 3;   //���û��룩�Լ�����
	irdata2 = 252;	//���û����룩

	//����9ms�������루�ߵ�ƽ��
	
	endcount = 684;	 //9ms / 13.15us = 684
	count = 0;
	IR_flag = 1; //���ͱ�־(����38K�ز�)
	while(count < endcount);

	//����4.5ms��������Ľ����루�͵�ƽ��
	count = 0;
	endcount = 342;	 //4.5ms / 13.15us = 342 
	IR_flag = 0; //���ͱ�־������0��
	while(count < endcount);
	
	Send_IRdata(irdata1);       //����16λ��ַ��ǰ��λ��8λ�û��룩
    Send_IRdata(irdata2);      //����16λ��ַ�ĺ��λ��8λ�û����룩
	Send_IRdata(p_irdata);	  //����8λ��������
	Send_IRdata(~p_irdata); //����8λ�����ݷ���
	Send_IRdata(0x66);	//�����루Ҳ���ڱ��

	
	//�����0.56ms��38KHz�����ز�����������0.56ms�ĵ͵�ƽ
	count = 0;
	endcount = 42;	//0.56ms / 13.15us = 42 (42 / 2 = 21��������0.56ms���ز��а���21���ز�����)
	IR_flag = 1; //���ͱ�־
	while(count < endcount);
	IR_flag = 0; 			
}	
