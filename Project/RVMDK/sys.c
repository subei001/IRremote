
#include "stm32f10x.h"
#include "Sys.h"
	 
static u8  fac_us=0;//us��ʱ������
static u16 fac_ms=0;//ms��ʱ������

//��ʼ���ӳٺ���
//SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8
//SYSCLK:ϵͳʱ��
void Delay_init(u8 SYSCLK)
{
	SysTick->CTRL&=0xfffffffb;//bit2���,ѡ���ⲿʱ��  HCLK/8
	fac_us=SYSCLK/8;		    
	fac_ms=(u16)fac_us*1000;
}	
							    
//��ʱnms
//ע��nms�ķ�Χ
//SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK��λΪHz,nms��λΪms
//��72M������,nms<=1864 

void Delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;           //��ռ�����
	SysTick->CTRL=0x01 ;          //��ʼ����  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����	  	    
}  
 
//��ʱnus
//nusΪҪ��ʱ��us��.		    							  
void Delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL=0x01 ;      //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����	 
}

void RCC_Config(void)
{
  ErrorStatus HSEStartUpStatus;                    //�����ⲿ���پ�������״̬ö�ٱ���
  RCC_DeInit();                                    //��λRCC�ⲿ�豸�Ĵ�����Ĭ��ֵ
  RCC_HSEConfig(RCC_HSE_ON);                       //���ⲿ���پ���
  HSEStartUpStatus = RCC_WaitForHSEStartUp();      //�ȴ��ⲿ����ʱ��׼����
  if(HSEStartUpStatus == SUCCESS)                  //�ⲿ����ʱ���Ѿ�׼���
  {
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); //����FLASHԤ�����幦�ܣ�����FLASH�Ķ�ȡ�����г����б�����÷�.λ�ã�RCC��ʼ���Ӻ������棬ʱ������֮��
    FLASH_SetLatency(FLASH_Latency_2);                    //flash��������ʱ
      	
    RCC_HCLKConfig(RCC_SYSCLK_Div1);               //����AHB(HCLK)ʱ�ӵ���==SYSCLK
    RCC_PCLK2Config(RCC_HCLK_Div1);                //����APB2(PCLK2)��==AHBʱ��
    RCC_PCLK1Config(RCC_HCLK_Div2);                //����APB1(PCLK1)��==AHB1/2ʱ��
         
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);  //����PLLʱ�� == �ⲿ���پ���ʱ�� * 9 = 72MHz
    RCC_PLLCmd(ENABLE);                                   //ʹ��PLLʱ��
   
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)    //�ȴ�PLLʱ�Ӿ���
    {
    }
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);            //����ϵͳʱ�� = PLLʱ��
    while(RCC_GetSYSCLKSource() != 0x08)                  //���PLLʱ���Ƿ���Ϊϵͳʱ��
    {
    }
  }  
  Delay_init(72);  
}

void NVIC_Config(void)
{
//		EXTI_InitTypeDef EXTI_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

//		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	             //����2����ռ���ȼ�0-3�������ȼ�0-3				 
/*---------------------�����ⲿ�ж�PD2����ռ���ȼ�1�������ȼ�1----------------------------------------*/
//		GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource2);	
//
//		EXTI_InitStructure.EXTI_Line = EXTI_Line2;      		     //--�ⲿ�ж���,ʹ�õ�0��
//		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;          //--�ж�ģʽ
//		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;      //--�½��ش���ģʽ
//		EXTI_InitStructure.EXTI_LineCmd = DISABLE;              	     //--���ж�
//	    EXTI_Init(&EXTI_InitStructure); 						     //--д���Ӧ�Ĵ�����
//
//		NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn; 			 //--��0������,����ʹ��0ͨ��
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;    //--�����ж����ȼ�Ϊ2
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			 //--���������ȼ�Ϊ1
//		NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;				 //--����ͨ��
//		NVIC_Init(&NVIC_InitStructure); 							 //--д���Ӧ�Ĵ�����  
///*---------------------�����ⲿ�ж�PC12����ռ���ȼ�0�������ȼ�0----------------------------------------*/		
//		GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource12);
//
//		EXTI_InitStructure.EXTI_Line = EXTI_Line12;      		     //--�ⲿ�ж���,ʹ�õ�1��
//		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;          //--�ж�ģʽ
//		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;      //--�½��ش���ģʽ
//		EXTI_InitStructure.EXTI_LineCmd = DISABLE;              	     //--���ж�
//	    EXTI_Init(&EXTI_InitStructure); 						     //--д���Ӧ�Ĵ�����
//
//		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn; 			 //--��һ������,����ʹ��1ͨ��
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;    //--�����ж����ȼ�Ϊ0
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			 //--���������ȼ�Ϊ0
//		NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;				 //--����ͨ��
//		NVIC_Init(&NVIC_InitStructure); 							 //--д���Ӧ�Ĵ�����
/*---------------------���ö�ʱ��2�жϣ���ռ���ȼ�1�������ȼ�1----------------------------------------*/
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;    //ѡ��ΪTIM2ͨ��
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   //������ռ���ȼ�Ϊ2
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;    //���ô����ȼ�Ϊ0
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    //ʹ��TIM2�ж�
		NVIC_Init(&NVIC_InitStructure);

}






































