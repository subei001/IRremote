#include "stm32f10x.h"
#include "All_head.h"
#include "stdint.h"
#include "All_define.h"


#define  Write_Addr  0x70 	 // slave  address = 70H
#define  Read_Addr   0x71    //Read_Addr = Write_Addr+1 

#define SDA_1_ZLG7290()   (GPIOB->BSRR = 0x00000080)
#define SDA_0_ZLG7290()   (GPIOB->BSRR = 0x00800000)
#define SCL_1_ZLG7290()   (GPIOB->BSRR = 0x00000040)
#define SCL_0_ZLG7290()   (GPIOB->BSRR = 0x00400000)

/*---------------------------I2C操作----------------------------------------------------
对zlg7290的操作时，1.要按照手册的时序写
				   2.时钟比51的打很多，所以要延时更长写
				   3.在等待应答和读数据时，注意IO的配置，是输入还是输出
				   4.本次是模拟I2C操作，所以不要IO对应SDA,SCL设置为复用开漏输出，而是设置为通用推挽输出；
				     只有在使用固件库里面的I2C时，才要设为复用开漏输出（GPIO_Mode_AF_OD）																   */
/*****************************************************************************
**函数名称:delay_IIC
**功能描述:I2C_ZLG7290专用延时
**输入参数:n
**输出参数:无
********************************************************************************/
void _delay_us(unsigned int n)	  //1us*n
{
    unsigned int i,j;
	for(i=0; i<n; i++)
	   for(j=0; j<5; j++);
}

/***********************************************
  * 函数名：Start_ZLG7290
  * 功  能：启动ZLG7290
  * 子函数：无
  * 变  量：无
*************************************************/
void Start_ZLG7290(void)
{
	SDA_1_ZLG7290();
	SCL_1_ZLG7290();
	_delay_us(100);	 //100us
	SDA_0_ZLG7290();
	_delay_us(100);
	SCL_0_ZLG7290();
	_delay_us(100);
}


/***********************************************
  * 函数名：Stop_ZLG7290
  * 功  能：关闭ZLG7290
  * 子函数：无
  * 变  量：无
*************************************************/
void Stop_ZLG7290(void)
{
	SCL_1_ZLG7290();
	SDA_0_ZLG7290();
	_delay_us(100);
	SDA_1_ZLG7290();
	_delay_us(100);
	SCL_0_ZLG7290();
	_delay_us(100);
}


/***********************************************************************
函数：I2C_ZLG7209_Ack()
功能：读取从机应答位
返回：
	0：从机应答
	1：从机非应答
说明：
	从机在收到每个字节的数据后，要产生应答位
	从机在收到最后1个字节的数据后，一般要产生非应答位
*************************************************************************/
void I2C_ZLG7209_Ack(unsigned char a)
{
  if(a==0){SDA_0_ZLG7290();}
  else {SDA_1_ZLG7290();}
  _delay_us(50);//50微秒
  
  SCL_1_ZLG7290();
  _delay_us(50);//50微秒

  SCL_0_ZLG7290();
  _delay_us(50);//50微秒
  SDA_1_ZLG7290();
}


/***********************************************
  * 函数名：WriteByte_ZLG7290
  * 功  能：向ZLG7290写一个字节
  * 说  明：IIC先写高位
  * 子函数：无
  * 变  量：data，i
*************************************************/
void WriteByte_ZLG7290(unsigned int data)	//先发高位
{
	unsigned int i;
	SCL_0_ZLG7290();	//时钟线为低电平时,数据线可以改变
	for(i=0;i<8;i++)
	{		
		if(data & 0X80)   
		{
			SDA_1_ZLG7290();
		}
		else           
		{
			SDA_0_ZLG7290();
		}
		_delay_us(50);
		SCL_1_ZLG7290();
		_delay_us(100);
		SCL_0_ZLG7290();
		data<<=1;
		_delay_us(50);
	}
	SDA_1_ZLG7290();
	_delay_us(50);
	SCL_1_ZLG7290();
	_delay_us(50);

//	GPIOB->CRL&=0X0FFFFFFF;//先保存其他位不变	//因为不知如配置GPIO_Speed，所以测试不成功
//	GPIOB->CRL|=0X8000000;//PB7浮空输入
	GPIO_ZLG7290_Config_PB7_InPut();//设置sda为输入上拉
	while( GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) );	//等待应答
//	GPIO_ZLG7290_Config_OutPut();//设置sda为输出
	GPIOB->CRL&=0X0FFFFFFF;//先保存其他位不变		   //用上面那句也可以
	GPIOB->CRL|=0X30000000;//PB7 通用推挽输出模式 50MHz

	SCL_0_ZLG7290();
	_delay_us(50);
}

/***********************************************
  * 函数名：ReadByte_ZLG7290
  * 功  能：向ZLG7290读一个字节
  * 说  明：IIC先读高位
  * 子函数：无
  * 变  量：a
*************************************************/
unsigned int ReadByte_ZLG7290(void) //先读高位
{
	unsigned int i,a=0;

	SDA_1_ZLG7290(); //在读取数据之前，要把SDA拉高
	_delay_us(50);

	GPIO_ZLG7290_Config_PB7_InPut();//设置sda为输入上拉
//	GPIOB->CRL&=0X0FFFFFFF;//先保存其他位不变
//	GPIOB->CRL|=0X8000000;//PB7浮空输入

	for(i=0;i<8;i++)
	{		
		a<<=1;
		SCL_1_ZLG7290();
		_delay_us(50);
				
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7))
		{
			a|=0x01;
		} 
		SCL_0_ZLG7290();
		_delay_us(50);
	}

	GPIO_ZLG7290_Config_OutPut();//设置sda为输出
	GPIOB->CRL&=0X0FFFFFFF;//先保存其他位不变
	GPIOB->CRL|=0X30000000;//PB7 通用推挽输出模式 50MHz

	SCL_0_ZLG7290();
	_delay_us(50);

	return(a);	
}

/***************************************************************
  * 函数名：Read_ZLG7290
  * 功  能：读24c64
  * 说  明： 启动总线→写器件地址→写单元地址→启动总线→写入要读出的器件地址→
             读数据→． ． ．→关闭总线
  * 子函数：
  * 变  量：
******************************************************************/
unsigned int Read_ZLG7290(void)
{
	unsigned int key=0; //读取键值
	
	Start_ZLG7290();              //启动总线

	WriteByte_ZLG7290(Write_Addr);//写从器件地址 
	//I2C_ZLG7209_Ack(0);          //发送应答	//加入后，键盘不能读取（卡死）
	WriteByte_ZLG7290(0x01);     //写从器件寄存器
	//I2C_ZLG7209_Ack(0);          //发送应答
 
	Start_ZLG7290();             //重新启动总线
	WriteByte_ZLG7290(Read_Addr);//读从器件地址
	//I2C_ZLG7209_Ack(0);         //发送应答
	
	key =ReadByte_ZLG7290();    //读键值
	I2C_ZLG7209_Ack(1);//非应答信号   

	Stop_ZLG7290();          	//关闭总线
								
	return(key);
} 	

/*----------------------ZLG7290_I2C键盘键值处理及显示---------------------*/

/*******************************************************************
**函数名： Key_change_ZLG7290(void)                                *
**功  能： 按键值处理 ,把读出来的按键值转换成成 1--16 十进制       *
**子函数:  Read_ZLG7290()                                          *
**变  量： Key_，Key_Value                                         *
********************************************************************/
void treat_zlg7290(void)
{
	unsigned char Key_;
	Key_ = Read_ZLG7290();

	switch(Key_)
	{
		case 1 :  Write_IRdata(0x00);
				  LCD_write_num_string(0x28,0x90,"send data 0x00",Red,Green);break;
		case 2 :  Write_IRdata(0x01);
				  LCD_write_num_string(0x28,0x90,"send data 0x01",Red,Green);break;
		case 3 :  Write_IRdata(0x02);
				  LCD_write_num_string(0x28,0x90,"send data 0x02",Red,Green);break;
		case 4 :  Write_IRdata(0x03);
				  LCD_write_num_string(0x28,0x90,"send data 0x03",Red,Green);break;
		
		case 9 :  Write_IRdata(0x04);
				  LCD_write_num_string(0x28,0x90,"send data 0x04",Red,Green);break;
		case 10:  Write_IRdata(0x05);
				  LCD_write_num_string(0x28,0x90,"send data 0x05",Red,Green);break;
		case 11:  Write_IRdata(0x06);
				  LCD_write_num_string(0x28,0x90,"send data 0x06",Red,Green);break;
		case 12:  Write_IRdata(0x07);
				  LCD_write_num_string(0x28,0x90,"send data 0x07",Red,Green);break;
	
		case 17:  Write_IRdata(0x08);
				  LCD_write_num_string(0x28,0x90,"send data 0x08",Red,Green);break;
		case 18:  Write_IRdata(0x09);
				  LCD_write_num_string(0x28,0x90,"send data 0x09",Red,Green);break;
		case 19:  Write_IRdata(0x0a);
				  LCD_write_num_string(0x28,0x90,"send data 0x0a",Red,Green);break;
		case 20:  Write_IRdata(0x0b);
		   		  LCD_write_num_string(0x28,0x90,"send data 0x0b",Red,Green);break;

		case 25:  Write_IRdata(0x0c);
				  LCD_write_num_string(0x28,0x90,"send data 0x0c",Red,Green);break;
		case 26:  Write_IRdata(0x0d);
				  LCD_write_num_string(0x28,0x90,"send data 0x0d",Red,Green);break;
		case 27:  Write_IRdata(0x0e);
				  LCD_write_num_string(0x28,0x90,"send data 0x0e",Red,Green);break;
		case 28:  Write_IRdata(0x0f);
				  LCD_write_num_string(0x28,0x90,"send data 0x0f",Red,Green);break;
		//case 28: GPIO_WriteBit(GPIOD, GPIO_Pin_10, (BitAction)(1-GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_10)));break; 
	   default :break;
	}

}
