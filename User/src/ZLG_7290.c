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

/*---------------------------I2C����----------------------------------------------------
��zlg7290�Ĳ���ʱ��1.Ҫ�����ֲ��ʱ��д
				   2.ʱ�ӱ�51�Ĵ�ܶ࣬����Ҫ��ʱ����д
				   3.�ڵȴ�Ӧ��Ͷ�����ʱ��ע��IO�����ã������뻹�����
				   4.������ģ��I2C���������Բ�ҪIO��ӦSDA,SCL����Ϊ���ÿ�©�������������Ϊͨ�����������
				     ֻ����ʹ�ù̼��������I2Cʱ����Ҫ��Ϊ���ÿ�©�����GPIO_Mode_AF_OD��																   */
/*****************************************************************************
**��������:delay_IIC
**��������:I2C_ZLG7290ר����ʱ
**�������:n
**�������:��
********************************************************************************/
void _delay_us(unsigned int n)	  //1us*n
{
    unsigned int i,j;
	for(i=0; i<n; i++)
	   for(j=0; j<5; j++);
}

/***********************************************
  * ��������Start_ZLG7290
  * ��  �ܣ�����ZLG7290
  * �Ӻ�������
  * ��  ������
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
  * ��������Stop_ZLG7290
  * ��  �ܣ��ر�ZLG7290
  * �Ӻ�������
  * ��  ������
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
������I2C_ZLG7209_Ack()
���ܣ���ȡ�ӻ�Ӧ��λ
���أ�
	0���ӻ�Ӧ��
	1���ӻ���Ӧ��
˵����
	�ӻ����յ�ÿ���ֽڵ����ݺ�Ҫ����Ӧ��λ
	�ӻ����յ����1���ֽڵ����ݺ�һ��Ҫ������Ӧ��λ
*************************************************************************/
void I2C_ZLG7209_Ack(unsigned char a)
{
  if(a==0){SDA_0_ZLG7290();}
  else {SDA_1_ZLG7290();}
  _delay_us(50);//50΢��
  
  SCL_1_ZLG7290();
  _delay_us(50);//50΢��

  SCL_0_ZLG7290();
  _delay_us(50);//50΢��
  SDA_1_ZLG7290();
}


/***********************************************
  * ��������WriteByte_ZLG7290
  * ��  �ܣ���ZLG7290дһ���ֽ�
  * ˵  ����IIC��д��λ
  * �Ӻ�������
  * ��  ����data��i
*************************************************/
void WriteByte_ZLG7290(unsigned int data)	//�ȷ���λ
{
	unsigned int i;
	SCL_0_ZLG7290();	//ʱ����Ϊ�͵�ƽʱ,�����߿��Ըı�
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

//	GPIOB->CRL&=0X0FFFFFFF;//�ȱ�������λ����	//��Ϊ��֪������GPIO_Speed�����Բ��Բ��ɹ�
//	GPIOB->CRL|=0X8000000;//PB7��������
	GPIO_ZLG7290_Config_PB7_InPut();//����sdaΪ��������
	while( GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) );	//�ȴ�Ӧ��
//	GPIO_ZLG7290_Config_OutPut();//����sdaΪ���
	GPIOB->CRL&=0X0FFFFFFF;//�ȱ�������λ����		   //�������Ǿ�Ҳ����
	GPIOB->CRL|=0X30000000;//PB7 ͨ���������ģʽ 50MHz

	SCL_0_ZLG7290();
	_delay_us(50);
}

/***********************************************
  * ��������ReadByte_ZLG7290
  * ��  �ܣ���ZLG7290��һ���ֽ�
  * ˵  ����IIC�ȶ���λ
  * �Ӻ�������
  * ��  ����a
*************************************************/
unsigned int ReadByte_ZLG7290(void) //�ȶ���λ
{
	unsigned int i,a=0;

	SDA_1_ZLG7290(); //�ڶ�ȡ����֮ǰ��Ҫ��SDA����
	_delay_us(50);

	GPIO_ZLG7290_Config_PB7_InPut();//����sdaΪ��������
//	GPIOB->CRL&=0X0FFFFFFF;//�ȱ�������λ����
//	GPIOB->CRL|=0X8000000;//PB7��������

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

	GPIO_ZLG7290_Config_OutPut();//����sdaΪ���
	GPIOB->CRL&=0X0FFFFFFF;//�ȱ�������λ����
	GPIOB->CRL|=0X30000000;//PB7 ͨ���������ģʽ 50MHz

	SCL_0_ZLG7290();
	_delay_us(50);

	return(a);	
}

/***************************************************************
  * ��������Read_ZLG7290
  * ��  �ܣ���24c64
  * ˵  ���� �������ߡ�д������ַ��д��Ԫ��ַ���������ߡ�д��Ҫ������������ַ��
             �����ݡ��� �� �����ر�����
  * �Ӻ�����
  * ��  ����
******************************************************************/
unsigned int Read_ZLG7290(void)
{
	unsigned int key=0; //��ȡ��ֵ
	
	Start_ZLG7290();              //��������

	WriteByte_ZLG7290(Write_Addr);//д��������ַ 
	//I2C_ZLG7209_Ack(0);          //����Ӧ��	//����󣬼��̲��ܶ�ȡ��������
	WriteByte_ZLG7290(0x01);     //д�������Ĵ���
	//I2C_ZLG7209_Ack(0);          //����Ӧ��
 
	Start_ZLG7290();             //������������
	WriteByte_ZLG7290(Read_Addr);//����������ַ
	//I2C_ZLG7209_Ack(0);         //����Ӧ��
	
	key =ReadByte_ZLG7290();    //����ֵ
	I2C_ZLG7209_Ack(1);//��Ӧ���ź�   

	Stop_ZLG7290();          	//�ر�����
								
	return(key);
} 	

/*----------------------ZLG7290_I2C���̼�ֵ������ʾ---------------------*/

/*******************************************************************
**�������� Key_change_ZLG7290(void)                                *
**��  �ܣ� ����ֵ���� ,�Ѷ������İ���ֵת���ɳ� 1--16 ʮ����       *
**�Ӻ���:  Read_ZLG7290()                                          *
**��  ���� Key_��Key_Value                                         *
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
