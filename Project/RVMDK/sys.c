
#include "stm32f10x.h"
#include "Sys.h"
	 
static u8  fac_us=0;//us延时倍乘数
static u16 fac_ms=0;//ms延时倍乘数

//初始化延迟函数
//SYSTICK的时钟固定为HCLK时钟的1/8
//SYSCLK:系统时钟
void Delay_init(u8 SYSCLK)
{
	SysTick->CTRL&=0xfffffffb;//bit2清空,选择外部时钟  HCLK/8
	fac_us=SYSCLK/8;		    
	fac_ms=(u16)fac_us*1000;
}	
							    
//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对72M条件下,nms<=1864 

void Delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;           //清空计数器
	SysTick->CTRL=0x01 ;          //开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL=0x00;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器	  	    
}  
 
//延时nus
//nus为要延时的us数.		    							  
void Delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; //时间加载	  		 
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL=0x01 ;      //开始倒数 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL=0x00;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器	 
}

void RCC_Config(void)
{
  ErrorStatus HSEStartUpStatus;                    //定义外部高速晶体启动状态枚举变量
  RCC_DeInit();                                    //复位RCC外部设备寄存器到默认值
  RCC_HSEConfig(RCC_HSE_ON);                       //打开外部高速晶振
  HSEStartUpStatus = RCC_WaitForHSEStartUp();      //等待外部高速时钟准备好
  if(HSEStartUpStatus == SUCCESS)                  //外部高速时钟已经准别好
  {
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); //开启FLASH预读缓冲功能，加速FLASH的读取。所有程序中必须的用法.位置：RCC初始化子函数里面，时钟起振之后
    FLASH_SetLatency(FLASH_Latency_2);                    //flash操作的延时
      	
    RCC_HCLKConfig(RCC_SYSCLK_Div1);               //配置AHB(HCLK)时钟等于==SYSCLK
    RCC_PCLK2Config(RCC_HCLK_Div1);                //配置APB2(PCLK2)钟==AHB时钟
    RCC_PCLK1Config(RCC_HCLK_Div2);                //配置APB1(PCLK1)钟==AHB1/2时钟
         
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);  //配置PLL时钟 == 外部高速晶体时钟 * 9 = 72MHz
    RCC_PLLCmd(ENABLE);                                   //使能PLL时钟
   
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)    //等待PLL时钟就绪
    {
    }
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);            //配置系统时钟 = PLL时钟
    while(RCC_GetSYSCLKSource() != 0x08)                  //检查PLL时钟是否作为系统时钟
    {
    }
  }  
  Delay_init(72);  
}

void NVIC_Config(void)
{
//		EXTI_InitTypeDef EXTI_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

//		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	             //分组2：先占优先级0-3，从优先级0-3				 
/*---------------------配置外部中断PD2，先占优先级1，从优先级1----------------------------------------*/
//		GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource2);	
//
//		EXTI_InitStructure.EXTI_Line = EXTI_Line2;      		     //--外部中断线,使用第0根
//		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;          //--中断模式
//		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;      //--下降沿触发模式
//		EXTI_InitStructure.EXTI_LineCmd = DISABLE;              	     //--打开中断
//	    EXTI_Init(&EXTI_InitStructure); 						     //--写入对应寄存器中
//
//		NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn; 			 //--第0个引脚,所以使用0通道
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;    //--设置中断优先级为2
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			 //--设置子优先级为1
//		NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;				 //--启用通道
//		NVIC_Init(&NVIC_InitStructure); 							 //--写入对应寄存器中  
///*---------------------配置外部中断PC12，先占优先级0，从优先级0----------------------------------------*/		
//		GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource12);
//
//		EXTI_InitStructure.EXTI_Line = EXTI_Line12;      		     //--外部中断线,使用第1根
//		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;          //--中断模式
//		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;      //--下降沿触发模式
//		EXTI_InitStructure.EXTI_LineCmd = DISABLE;              	     //--打开中断
//	    EXTI_Init(&EXTI_InitStructure); 						     //--写入对应寄存器中
//
//		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn; 			 //--第一个引脚,所以使用1通道
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;    //--设置中断优先级为0
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			 //--设置子优先级为0
//		NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;				 //--启用通道
//		NVIC_Init(&NVIC_InitStructure); 							 //--写入对应寄存器中
/*---------------------配置定时器2中断，先占优先级1，从优先级1----------------------------------------*/
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;    //选择为TIM2通道
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   //设置抢占优先级为2
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;    //设置从优先级为0
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    //使能TIM2中断
		NVIC_Init(&NVIC_InitStructure);

}






































