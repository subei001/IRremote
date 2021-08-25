/********************************************************************************
  * @�ļ�����   ���ⷢ��/Project/main.c 
  * @�ļ����ܣ� ������38K�ķ�����Ϊ���ⷢ����ز� ����ɷ���
  * @��ɵ�λ�� ����ʦ����ѧ���ӹ���ѧԺ12������2��
  * @����  ��   ������

*********************************************************************************/

/* Includes --------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "All_head.h"
#include "stdint.h"
#include "All_define.h"
#include "sys.h"
#include "key.h"

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	 //��ʱ����ʼ���ṹ
	   
volatile unsigned int ZLG_flag;  //	���̰��±�־


/*************************************************************************************
						 		��������
**************************************************************************************/
/*��ʱ���������η��ͼ��ʱ��*/
void delay(unsigned int num)
{
	while(num--);	
}


/*��IOʱ��*/
void RCC_Configuration(void)
{
   /* TIM3 clock enable */
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
 
   /* GPIO_x clock enable */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB , ENABLE);
                          
}


/*IO����*/
void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 
     
    /*LEDָʾ�Ƽ����38KHzIO����*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

//   	/*zlg7290�����ж���������*/
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_Init(GPIOC, &GPIO_InitStructure);

//	/*ģ��I2C�����ߺ�ʱ���� PB6 SCL and PB7 SDA */  
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //�̼�I2CЭ������ø��ÿ�©���GPIO_Mode_AF_OD����ģ��I2C��GPIO_Mode_Out_PP�������ܲ�������
//	GPIO_Init(GPIOB, &GPIO_InitStructure);

}

/*****************************************************************************
**��������:GPIO_ZLG7290_Config_InPut
**��������:����PB7Ϊ����
**�������:��
**�������:��
**************************************************************************/
/*void GPIO_ZLG7290_Config_PB7_InPut(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_7;//ʹ��5~7
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      //��������/�����������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //ʱ��Ϊ50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);                //ָ��ṹ���еı���
}
*/
/*****************************************************************************
**��������:GPIO_ZLG7290_Config_OutPut
**��������:����PB6.7Ϊ���
**�������:��
**�������:��
**************************************************************************/
/*void GPIO_ZLG7290_Config_OutPut(void)//pb6=scl;  pb7=sda 
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6 |GPIO_Pin_7;//ʹ��6~7
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //ʱ��Ϊ50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);                //ָ��ṹ���еı���
}

*/
/**
   * @brief  Configure the nested vectored interrupt controller.
   * @param  None
   * @retval None
   Ƕ���ж�����
*/
void NVIC_Configuration(void)
{
   NVIC_InitTypeDef NVIC_InitStructure;
 
   /* Enable the TIM2 gloabal Interrupt */
   NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);

   /* Enable the EXTI13 Interrupt */
 /*  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;	//ע��9-5����15-10���ж����Ƶı�ʾ
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
*/
   
}


/**********************************************************************
Frequency=(TIM3CLK(72M)/(Prescaler+1)) /(Period+1) ��������������� 
��TIM3CLK/(Prescaler+1)=ʱ��Ƶ�ʣ� ��ʱ��Ƶ��/(Period+1)=�������Ƶ��
��ΪTIM2~4��ʱ��Դ��APB1 ����PCLK1 ( APB1 ��ӦPCLK1 )	TIM2~4CLK = 36 MHz
PCLK1 = APB1 = HCLK/2 = SYSCLK/2 = 36MHZ (36,000,000 HZ)
����ע�⣺��Ƶ�����Զ��������ǡ�72MHZ����
ֻ��TIM_Period��TIM_Prescaler��ֵ�Ż�Ӱ�죨����ʱ�ӣ����Ƶ��
***********************************************************************/
/*��ʱ��TIM3����*/
void TIM_Configuration(void)  
{
	/* Time base configuration */ 
   TIM_TimeBaseStructure.TIM_Period = 235; //���ڣ���������ֵ��������»��жϣ�����ʱʱ�䵽��
   TIM_TimeBaseStructure.TIM_Prescaler = 3; //������������ΪTIM3ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
   			//���Ǽ������ڵı�Ƶ���������൱�ڵ��ڼ������ڣ���ʹPeriod��������߼������ȣ���ֵԽ��Ƶ��ԽС������Խ��
			/*  ����ʱ�� = 72M / (3+1) / (235+1) = 	76KHz(��13.15us��һ�ζ�ʱ���ж�)
				����һ�����ڵ�Ƶ�� = 76 / 2 = 38KHz */		       // yinggai shi yizhi zai fangwen
   TIM_TimeBaseStructure.TIM_ClockDivision = 0;	//ʱ�ӷָ�
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���
   TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //������ʼ��
	
	/* Prescaler configuration */
   //TIM_PrescalerConfig(TIM3, 7199, TIM_PSCReloadMode_Immediate);  //�൱��TIM_Prescaler��ֵ 

   TIM_ITConfig(TIM3, TIM_IT_Update , ENABLE );  //��ͨ���жϣ��ж���Ҫ���д��루����������NVIC�жϼ��ɣ�
   //Ҳ����TIM_ITConfig(TIM3, TIM_IT_CC1 , ENABLE ); //Ҳ��������ͨ���������ж��Ӻ���ҲҪ��Ӧ�ı�
   /* TIM3 enable counter */
   TIM_Cmd(TIM3, ENABLE);  //������ʱ��3
	TIM_SetCompare1(TIM3,49);  //iֵ������ȡ235����ΪARR���ֵ��235
}

/*void EXTI_Configuration(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	//ѡ��GPIO�ܽ������ⲿ�ж���·
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);
	 
	EXTI_InitStructure.EXTI_Line = EXTI_Line13;  //ע�⣺ѡ�õ�IO���Ƕ��٣�����Line����
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE; 
	EXTI_Init(&EXTI_InitStructure);
}
*/




u8 reverse_bit8(u8 x)
{
	x = ((x & 0x55) << 1) | ((x & 0xAA) >> 1);
	x = ((x & 0x33) << 2) | ((x & 0xCC) >> 2);
	return (x << 4) | (x >> 4);
}
/**
  * @brief  ������.
  * @param  None
  * @retval : None
  */

int main(void)
{	
	unsigned char key_value;
	ZLG_flag = 0;
	/* Setup STM32 system (clock, PLL and Flash configuration) */
	SystemInit();

	/* Add your application code here	*/
	RCC_Configuration();  //����TIM3ʱ��
	RCC_Config();    //--����ϵͳ��ƵΪ72MHz
//	Matrixkey_Init();//������̺ͺ��ⷢ���
	NVIC_Configuration(); //TIM3�ж�����
  GPIO_Configuration();  /*IO����*/
	TIM_Configuration();  /*��ʱ��TIM3����*/
	
//	GPIO_ResetBits(GPIOD,GPIO_Pin_8);
//	D_value=GPIO_ReadOutputDataBit(GPIOD,GPIO_Pin_8);

//	EXTI_Configuration();

	
//		GPIO_ResetBits(GPIOD, GPIO_Pin_13);
//		GPIO_ResetBits(GPIOG, GPIO_Pin_14);
	
	while (1)
	{		
		
		  Write_IRdata(0x12);
			Delay_ms(200);
//		  delay(60000);
				
//				key_value=Get_KeyValue();	
 // 		  treat_key_value(key_value);
//			  delay(60000);
				
			

//		if(ZLG_flag == 1)
//		{
//			ZLG_flag=0;
//			treat_zlg7290();
			//delay(60000); //��ʱ���������η��ͼ��ʱ��
//		}
	}
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
