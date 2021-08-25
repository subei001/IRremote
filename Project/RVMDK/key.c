#include "key.h"
#include "All_head.h"
#include "Sys.h"

#define KEY_PORT GPIOC

void Matrixkey_Init(void)	  //矩阵键盘初始化函数；PC0~PC3设为输出模式，PC4~PC7设为输入模式
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOG, ENABLE);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	   /*输出38KHzIO配置*/	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	
	
	//灯测试来检测矩阵键盘
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;			 //板上LED编号 D2
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOG, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;			 //板上LED编号 D5
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);

}

unsigned char Get_KeyValue(void)
{
	u8 temp_key;
	unsigned char  key;
	GPIO_Write(KEY_PORT, 0xfe);
	temp_key = GPIO_ReadInputData(KEY_PORT);
	temp_key = temp_key & 0xf0;     //11110000
	if(temp_key != 0xf0)
	{
		Delay_ms(10);
		temp_key = GPIO_ReadInputData(KEY_PORT);
		temp_key = temp_key & 0xf0;
		if(temp_key != 0xf0)
		{
			temp_key = GPIO_ReadInputData(KEY_PORT);
			switch(temp_key)
			{
				case 0xee: key = '1';  break;
				case 0xde: key = '2';  break;
				case 0xbe: key = '3';  break;
				case 0x7e: key = 'A';  break;
			}	  
			while(temp_key != 0xf0)		  //等待按键释放
			{
				temp_key = GPIO_ReadInputData(KEY_PORT);
				temp_key = temp_key&0xf0;
			}
		}	
	}    /* end of if(temp_key!=0xf0)*/

	GPIO_Write(KEY_PORT,0xfd);
	temp_key = GPIO_ReadInputData(KEY_PORT);
	temp_key=temp_key&0xf0;     //11110000
	if(temp_key!=0xf0)
	{
		Delay_ms(10);
		temp_key = GPIO_ReadInputData(KEY_PORT);
		temp_key=temp_key&0xf0;
		if(temp_key!=0xf0)
		{
			temp_key = GPIO_ReadInputData(KEY_PORT);
			switch(temp_key)
			{
				case 0xed: key='4';  break;
				case 0xdd: key='5';  break;
				case 0xbd: key='6';  break;
				case 0x7d: key='B';  break;
			}	  
			while(temp_key!=0xf0)		  //等待按键释放
			{
				temp_key = GPIO_ReadInputData(KEY_PORT);
				temp_key=temp_key&0xf0;
			}
		}	
	}    /* end of if(temp_key!=0xf0)*/

	GPIO_Write(KEY_PORT,0xfb);
	temp_key = GPIO_ReadInputData(KEY_PORT);
	temp_key=temp_key&0xf0;     //11110000
	if(temp_key!=0xf0)
	{
		Delay_ms(10);
		temp_key = GPIO_ReadInputData(KEY_PORT);
		temp_key=temp_key&0xf0;
		if(temp_key!=0xf0)
		{
			temp_key = GPIO_ReadInputData(KEY_PORT);
			switch(temp_key)
			{
				case 0xeb: key='7';  break;
				case 0xdb: key='8';  break;
				case 0xbb: key='9';  break;
				case 0x7b: key='C';  break;
			}	  
			while(temp_key!=0xf0)		  //等待按键释放
			{
				temp_key = GPIO_ReadInputData(KEY_PORT);
				temp_key=temp_key&0xf0;
			}
		}	
	}    /* end of if(temp_key!=0xf0)*/

	GPIO_Write(KEY_PORT,0xf7);
	temp_key = GPIO_ReadInputData(KEY_PORT);
	temp_key=temp_key&0xf0;     //11110000
	if(temp_key!=0xf0)
	{
		Delay_ms(10);
		temp_key = GPIO_ReadInputData(KEY_PORT);
		temp_key=temp_key&0xf0;
		if(temp_key!=0xf0)
		{
			temp_key = GPIO_ReadInputData(KEY_PORT);
			switch(temp_key)
			{
				case 0xe7: key='*';  break;
				case 0xd7: key='0';  break;
				case 0xb7: key='#';  break;
				case 0x77: key='D';  break;
			}	  
			while(temp_key!=0xf0)		  //等待按键释放
			{
				temp_key = GPIO_ReadInputData(KEY_PORT);
				temp_key=temp_key&0xf0;
			}
		}	
	}    /* end of if(temp_key!=0xf0)*/
	return key;
}


void treat_key_value(unsigned char key_value)
{
	
	switch(key_value)
	{
		case '1':  Write_IRdata(0x00);GPIO_SetBits(GPIOD, GPIO_Pin_13);		GPIO_SetBits(GPIOG, GPIO_Pin_14);break;
		case '2':  Write_IRdata(0x01);GPIO_ResetBits(GPIOD, GPIO_Pin_13);		GPIO_ResetBits(GPIOG, GPIO_Pin_14);break;
		case '3':  Write_IRdata(0x02);GPIO_SetBits(GPIOD, GPIO_Pin_13);		GPIO_SetBits(GPIOG, GPIO_Pin_14);break;
		case 'A':  Write_IRdata(0x03);GPIO_ResetBits(GPIOD, GPIO_Pin_13);		GPIO_ResetBits(GPIOG, GPIO_Pin_14);break;

		case '4':  Write_IRdata(0x04);GPIO_SetBits(GPIOD, GPIO_Pin_13);		GPIO_SetBits(GPIOG, GPIO_Pin_14);break;
		case '5':  Write_IRdata(0x05);GPIO_ResetBits(GPIOD, GPIO_Pin_13);		GPIO_ResetBits(GPIOG, GPIO_Pin_14);break;
		case '6':  Write_IRdata(0x06);GPIO_SetBits(GPIOD, GPIO_Pin_13);		GPIO_SetBits(GPIOG, GPIO_Pin_14);break;
		case 'B':  Write_IRdata(0x07);GPIO_SetBits(GPIOD, GPIO_Pin_13);		GPIO_SetBits(GPIOG, GPIO_Pin_14);break;

		case '7':  Write_IRdata(0x08);GPIO_ResetBits(GPIOD, GPIO_Pin_13);		GPIO_ResetBits(GPIOG, GPIO_Pin_14);break;
		case '8':  Write_IRdata(0x09);GPIO_SetBits(GPIOD, GPIO_Pin_13);		GPIO_SetBits(GPIOG, GPIO_Pin_14);break;
		case '9':  Write_IRdata(0x0a);GPIO_ResetBits(GPIOD, GPIO_Pin_13);		GPIO_ResetBits(GPIOG, GPIO_Pin_14);break;
		case 'C':  Write_IRdata(0x0b);GPIO_SetBits(GPIOD, GPIO_Pin_13);		GPIO_SetBits(GPIOG, GPIO_Pin_14);break;

		case '*':  Write_IRdata(0x0c);GPIO_ResetBits(GPIOD, GPIO_Pin_13);		GPIO_ResetBits(GPIOG, GPIO_Pin_14);break;
		case '0':  Write_IRdata(0x0d);GPIO_SetBits(GPIOD, GPIO_Pin_13);		GPIO_SetBits(GPIOG, GPIO_Pin_14);break;
		case '#':  Write_IRdata(0x0e);GPIO_ResetBits(GPIOD, GPIO_Pin_13);		GPIO_ResetBits(GPIOG, GPIO_Pin_14);break;
		case 'D':  Write_IRdata(0x0f);GPIO_SetBits(GPIOD, GPIO_Pin_13);		GPIO_SetBits(GPIOG, GPIO_Pin_14);break;

	   default :break;
	}

}

//void Matrxkey_Scan(void)    /*  矩阵键盘扫描  */
//{
//	u8 Key_Value;
//	Key_Value = Get_KeyValue();
//	switch (Key_Value)
//	{
//		case 0:							    
//			{
//				printf("  key0  ");
//				Delay_ms(100);
//			} break;
//		case 1:							   
//			{
//				printf("  key1  ");
//				Delay_ms(100);
//			} break;
//		case 2:							    
//			{
//				printf("  key2  ");
//				Delay_ms(100);
//			} break;
//		case 3:							    
//			{
//				printf("  key3  ");
//				Delay_ms(100);
//			} break;
//		case 4:							    
//			{
//				printf("  key4  ");
//				Delay_ms(100);
//			} break;
//		case 5:							    
//			{
//				printf("  key5  ");
//				Delay_ms(100);
//			} break;
//		case 6:							    
//			{
//				printf("  key6  ");
//				Delay_ms(100);
//			} break;
//		case 7:							    
//			{
//				printf("  key7  ");
//				Delay_ms(100);
//			} break;
//		case 8:							   
//			{
//				printf("  key8  ");
//				Delay_ms(100);
//			} break;
//		case 9:							    
//			{
//				printf("  key9  ");
//				Delay_ms(100);
//			} break;
//		case 10:							   
//			{
//				printf("  key10  ");
//				Delay_ms(100);
//			} break;
//		case 11:							 
//			{
//				printf("  key14  ");
//				Delay_ms(100);
//			} break;
//		case 12:							   
//			{
//				printf("  key12  ");
//				Delay_ms(100);
//			} break;
//		case 13:							   
//			{
//				printf("  key13  ");
//				Delay_ms(100);
//			} break;
//		case 14:							   
//			{
//				printf("  key14  ");
//				Delay_ms(100);
//			} break;
//		case 15:							 
//			{
//				printf("  key15  ");
//				Delay_ms(100);
//			} break;
//		default:	break;					
//	}	 /* end of switch (Key_Value) */		
//}  /* end of void Matrxkey_Scan(void) */


