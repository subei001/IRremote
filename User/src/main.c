/********************************************************************************
  * @文件名：   红外发射/Project/main.c 
  * @文件功能： ⑴制作38K的方波作为红外发射的载波 ⑵完成发射
  * @完成单位： 广西师范大学电子工程学院12级电信2班
  * @作者  ：   龙保任

*********************************************************************************/

/* Includes --------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "All_head.h"
#include "stdint.h"
#include "All_define.h"
#include "sys.h"
#include "key.h"

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	 //定时器初始化结构
	   
volatile unsigned int ZLG_flag;  //	键盘按下标志


/*************************************************************************************
						 		函数部分
**************************************************************************************/
/*延时，满足两次发送间的时间*/
void delay(unsigned int num)
{
	while(num--);	
}


/*开IO时钟*/
void RCC_Configuration(void)
{
   /* TIM3 clock enable */
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
 
   /* GPIO_x clock enable */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB , ENABLE);
                          
}


/*IO配置*/
void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 
     
    /*LED指示灯及输出38KHzIO配置*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

//   	/*zlg7290按键中断输入配置*/
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_Init(GPIOC, &GPIO_InitStructure);

//	/*模拟I2C数据线和时钟线 PB6 SCL and PB7 SDA */  
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //固件I2C协议必须用复用开漏输出GPIO_Mode_AF_OD，而模拟I2C用GPIO_Mode_Out_PP，否则不能不出脉冲
//	GPIO_Init(GPIOB, &GPIO_InitStructure);

}

/*****************************************************************************
**函数名称:GPIO_ZLG7290_Config_InPut
**功能描述:配置PB7为输入
**输入参数:无
**输出参数:无
**************************************************************************/
/*void GPIO_ZLG7290_Config_PB7_InPut(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_7;//使能5~7
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      //输入上拉/浮空输入均可
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //时钟为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);                //指向结构体中的变量
}
*/
/*****************************************************************************
**函数名称:GPIO_ZLG7290_Config_OutPut
**功能描述:配置PB6.7为输出
**输入参数:无
**输出参数:无
**************************************************************************/
/*void GPIO_ZLG7290_Config_OutPut(void)//pb6=scl;  pb7=sda 
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6 |GPIO_Pin_7;//使能6~7
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //时钟为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);                //指向结构体中的变量
}

*/
/**
   * @brief  Configure the nested vectored interrupt controller.
   * @param  None
   * @retval None
   嵌套中断配置
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
 /*  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;	//注意9-5，和15-10的中断名称的表示
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
*/
   
}


/**********************************************************************
Frequency=(TIM3CLK(72M)/(Prescaler+1)) /(Period+1) 由以下两部分组成 
①TIM3CLK/(Prescaler+1)=时钟频率； ②时钟频率/(Period+1)=输出计数频率
因为TIM2~4的时钟源是APB1 即是PCLK1 ( APB1 对应PCLK1 )	TIM2~4CLK = 36 MHz
PCLK1 = APB1 = HCLK/2 = SYSCLK/2 = 36MHZ (36,000,000 HZ)
但是注意：倍频器会自动倍，即是【72MHZ】！
只有TIM_Period和TIM_Prescaler的值才会影响（计数时钟）输出频率
***********************************************************************/
/*定时器TIM3配置*/
void TIM_Configuration(void)  
{
	/* Time base configuration */ 
   TIM_TimeBaseStructure.TIM_Period = 235; //周期（计数到该值后产生更新或中断，即定时时间到）
   TIM_TimeBaseStructure.TIM_Prescaler = 3; //设置了用来作为TIM3时钟频率除数的预分频值
   			//这是计数周期的倍频计数器，相当于调节计数周期，可使Period尽量大，提高计数精度（该值越大，频率越小，周期越大）
			/*  计数时钟 = 72M / (3+1) / (235+1) = 	76KHz(即13.15us进一次定时器中断)
				所以一个周期的频率 = 76 / 2 = 38KHz */		       // yinggai shi yizhi zai fangwen
   TIM_TimeBaseStructure.TIM_ClockDivision = 0;	//时钟分割
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
   TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //基本初始化
	
	/* Prescaler configuration */
   //TIM_PrescalerConfig(TIM3, 7199, TIM_PSCReloadMode_Immediate);  //相当于TIM_Prescaler的值 

   TIM_ITConfig(TIM3, TIM_IT_Update , ENABLE );  //打开通道中断，中断需要这行代码（另外再配置NVIC中断即可）
   //也可用TIM_ITConfig(TIM3, TIM_IT_CC1 , ENABLE ); //也可用其他通道，但是中断子函数也要相应改变
   /* TIM3 enable counter */
   TIM_Cmd(TIM3, ENABLE);  //启动定时器3
	TIM_SetCompare1(TIM3,49);  //i值最大可以取235，因为ARR最大值是235
}

/*void EXTI_Configuration(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	//选择GPIO管脚用作外部中断线路
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);
	 
	EXTI_InitStructure.EXTI_Line = EXTI_Line13;  //注意：选用的IO口是多少，就用Line多少
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
  * @brief  主函数.
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
	RCC_Configuration();  //开启TIM3时钟
	RCC_Config();    //--配置系统主频为72MHz
//	Matrixkey_Init();//矩阵键盘和红外发射端
	NVIC_Configuration(); //TIM3中断配置
  GPIO_Configuration();  /*IO配置*/
	TIM_Configuration();  /*定时器TIM3配置*/
	
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
			//delay(60000); //延时，满足两次发送间的时间
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
