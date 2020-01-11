#include "stm32f10x.h"
#include "stm32f10x_it.h"
//#include "led.h"
#include "key.h"
#include "usart.h"
#include "delay.h"
#include "lcd.h"
#include "ov7670.h"
#include "string.h"
#include "bsp_esp8266.h"
////
#include "discern.h"

//#define  WIFI  
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
vu8 cur_status=0;
vu8 LED_flag=0;
//vu32 a=0;
//vu32 b=0;
vu16 AA=0,BB=0;
vu16 color=0;
vu16 color_save=0;//����һ�����ص�ֵ
vu8 R=0,G=0,B=0;//��ɫ����
vu8 TableChangePoint_240[240];//�����240��
vu8 Max_ChangePoint_240=0,Min_ChangePoint_240=0,Max_bChangePoint=0,Min_bChangePoint=0;//���������ʼ��ĩ����,��������ʼ��ĩ����
vu8 a_Continue=0,b_Continue=0;//��¼�ݡ�����ͻ����������
vu8 flag_aMax=0;//ĩֵ���±�־
vu8 Max_aChangePoint_reset=0,Min_aChangePoint_reset=0;//�������������
vu16 Length_card=0,Width_card=0;//���Ƶĳ��Ϳ�
vu8 Max_aChangePoint_reset_1=0,Min_aChangePoint_reset_1=0;//�����ϴε�����
vu8 flag_MaxMinCompare=0;//Max_aChangePoint_reset_1��Max_aChangePoint_reset�ı�־
vu8 TableChangePoint_320[320];//���������320��
float V=0.00,S=0.00,H=0.00;//����HSVֵ
vu16 Min_blue=0;
vu16 Max_blue=0;//���峵����ɫ����ĺ������ֵ����Сֵ
vu16 k1=0,kk1=0,k2=0,kk2=0,k3=0,kk3=0,k4=0,kk4=0,k5=0,kk5=0,k6=0,kk6=0,k7=0,kk7=0,k8=0,kk8=0;//�˸��ַ��߽�
extern vu8 Table[6300];//�����ַ��� ��10+26��*150 = 5400 �ֽ�
extern vu8 talble_0[150];//�ַ�3,������
extern vu8 table_yu[32];//����
extern vu8 table_min[32];//����
extern vu8 table_lu[32];//³��
extern vu8 table_zhe[32];//����
extern vu8 table_shan[32];//����
extern vu8 table_cuan[32];//����
vu8 R_a=0,G_a=0,B_a=0;//��ֵ

vu8 table_picture[150];//���屣��ͼƬ������
vu8 table_char[36]={0,1,2,3,4,5,6,7,8,9,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',};
vu8 table_char_char[36]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',};
	vu8 table_card[5][8]={	//����5�����ƵĶ�ά����
{0,0,0,0,0,0,0,0},		//���һλ����ʱ��
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
};
vu8 tim3_num=0;//TIM3���Ӽ�ʱ

vu8 table_cardMeasure[7];//�����ĳ��ƽ��
void Show_Card(vu8 i);//��ʾ�ڼ��鳵��
void Show_Title();//��ʾ����

void MYRCC_DeInit(void)//��λ������������
{										 
	NVIC_InitTypeDef NVIC_InitStructure;	
	RCC->APB1RSTR = 0x00000000;//��λ����			 
	RCC->APB2RSTR = 0x00000000; 
	  
	RCC->AHBENR = 0x00000014;  //˯��ģʽ�����SRAMʱ��ʹ��.�����ر�.	  
	RCC->APB2ENR = 0x00000000; //����ʱ�ӹر�.			   
	RCC->APB1ENR = 0x00000000;   
	RCC->CR |= 0x00000001;     //ʹ���ڲ�����ʱ��HSION	 															 
	RCC->CFGR &= 0xF8FF0000;   //��λSW[1:0],HPRE[3:0],PPRE1[2:0],PPRE2[2:0],ADCPRE[1:0],MCO[2:0]					 
	RCC->CR &= 0xFEF6FFFF;     //��λHSEON,CSSON,PLLON
	RCC->CR &= 0xFFFBFFFF;     //��λHSEBYP	   	  
	RCC->CFGR &= 0xFF80FFFF;   //��λPLLSRC, PLLXTPRE, PLLMUL[3:0] and USBPRE 
	RCC->CIR = 0x00000000;     //�ر������ж�
	/* Enable the TIM3 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}
void Stm32_Clock_Init(vu8 PLL)//ϵͳʱ�ӳ�ʼ������  pll:ѡ��ı�Ƶ������2��ʼ�����ֵΪ16	
{
	unsigned char temp=0;   
	MYRCC_DeInit();		  //��λ������������
	RCC->CR|=0x00010000;  //�ⲿ����ʱ��ʹ��HSEON
	while(!(RCC->CR>>17));//�ȴ��ⲿʱ�Ӿ���
	RCC->CFGR=0X00000400; //APB1=DIV2;APB2=DIV1;AHB=DIV1;
	PLL-=2;//����2����λ
	RCC->CFGR|=PLL<<18;   //����PLLֵ 2~16
	RCC->CFGR|=1<<16;	  //PLLSRC ON 
	FLASH->ACR|=0x32;	  //FLASH 2����ʱ����

	RCC->CR|=0x01000000;  //PLLON
	while(!(RCC->CR>>25));//�ȴ�PLL����
	RCC->CFGR|=0x00000002;//PLL��Ϊϵͳʱ��	 
	while(temp!=0x02)     //�ȴ�PLL��Ϊϵͳʱ�����óɹ�
	{   
		temp=RCC->CFGR>>2;
		temp&=0x03;
	}    
}

//void LED()//LEDָʾ��-��ʾ
//{
//	GPIO_WriteBit(LED1_GPIO_PORT, LED1_GPIO_PIN,LED_flag>>7);
//	LED_flag=~LED_flag;
//}
void Data_LCD_Display()//������ʾ
{
	vu16 a=0,b=0;
	LCD_SetWindows(0,0,320,240);//������ʾ����  
	GPIO_WriteBit(LCD_RS_PORT, LCD_RS_PIN,1);//��־������д��

	while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)==1);
	GPIO_WriteBit(FIFO_WRST_PORT, FIFO_WRST_PIN, 0);
	GPIO_WriteBit(FIFO_WRST_PORT, FIFO_WRST_PIN, 1);
	GPIO_WriteBit(FIFO_WR_PORT, FIFO_WR_PIN, 1);
	while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)==0);
	while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)==1);
	GPIO_WriteBit(FIFO_WR_PORT, FIFO_WR_PIN, 0);
	
	FIFO_Reset_Read_Addr(); 
 
	
	for (a=0;a<240;a++)
	{
		for(b=0;b<320;b++)
		{
			GPIOC->BRR =1<<4;
			AA=GPIOA->IDR;						
			GPIOC->BSRR =1<<4;
			
			GPIOC->BRR =1<<4;
			BB=GPIOA->IDR&0x00ff;								
			GPIOC->BSRR =1<<4;	
			
			color=(AA<<8)|BB;
			
			LCD_DATA_PORT->ODR = color;

			GPIOC->BRR =1<<11;
			GPIOC->BSRR =1<<11;
		}							
	}	 
}

void StringToPicture()//����->ͼƬ
{
	vu16 a=0,b=0,e=0,num1=0;
	
	for(a=0;a<50;a++)//50��
	{
		for(b=0;b<24;b++)//24��
		{
			if(talble_0[b/8+a*3]&(1<<(7-b%8)))
			{
				num1=0xffff;
			}
			else
			{
				num1=0x0000;
			}
			LCD_DrawPoint(b+296,a+191,num1);//����
		}				
	}	
}


void Data_LCD_ColorChange_Test()//����ͷɨ�����
{
	vu16 a=0,b=0;
	
	for(a=0;a<240;a++)//����������������������
	{
		TableChangePoint_240[a]=0;
	}
	Min_blue=320;//��ʼ����¼��ɫ���������ֵ
	Max_blue=0;
	
	LCD_SetWindows(0,0,320,240);//������ʾ����
	GPIO_WriteBit(LCD_RS_PORT, LCD_RS_PIN,1);//��־������д��

	while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)==1);
	GPIO_WriteBit(FIFO_WRST_PORT, FIFO_WRST_PIN, 0);
	GPIO_WriteBit(FIFO_WRST_PORT, FIFO_WRST_PIN, 1);
	GPIO_WriteBit(FIFO_WR_PORT, FIFO_WR_PIN, 1);
	while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)==0);
	while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)==1);
	GPIO_WriteBit(FIFO_WR_PORT, FIFO_WR_PIN, 0);
	
	FIFO_Reset_Read_Addr(); 
 
	
	for (a=0;a<240;a++)
	{
		for(b=0;b<320;b++)
		{
			GPIOC->BRR =1<<4;
			AA=GPIOA->IDR;						
			GPIOC->BSRR =1<<4;
			
			GPIOC->BRR =1<<4;
			BB=GPIOA->IDR&0x00ff;								
			GPIOC->BSRR =1<<4;	
			
			color=(AA<<8)|BB;

			R=color>>11;
			G=(color>>5)&0x3f;
			B=color&0x1f;
			
			if((R>R_a) && (G>=G_a) && (B>=B_a))//��ֵ��,����ֵ��25.55.25���Ϻ�����ֵ��21,47,21��
			{
				color=0xffff;
			}
			else
			{
				color=0x0000;
			}
			
			if(color!=color_save)//�����
			{
				TableChangePoint_240[a]++;		//������������+1
			}
			color_save=color;//��������ֵ������һ���жϺͱȽ�
			
			color=(AA<<8)|BB;//��ԭɫ��
					
			LCD_DATA_PORT->ODR = color;
			GPIOC->BRR =1<<11;
			GPIOC->BSRR =1<<11;
		}
	}
	ChangePoint_Show_240();//240�����������ʾ
	ChangePoint_Analysis_240();	//��������  ��ó��Ƹ߶�
}

void Data_LCD_ColorChange()//����ͷɨ��
{
	vu8 i=0;

	Data_LCD_ColorChange_Test();
	//����flag_MaxMinCompare��Min_ChangePoint_240��Max_ChangePoint_240
	if(flag_MaxMinCompare==1)//�����ɸѡ�ɹ�  ���߶Ⱥ���
	{
			BEEP_ON;
			delayms(500);
			BEEP_OFF;
		//������ʾ�������ұ߽�
		 ChangePoint_Analysis_Blue();//320��ɫ�������,���ö�ȡ���أ��ý��Min_blue,Max_blue  �����������ұ߽�
		if(Min_blue>Max_blue) 
			flag_MaxMinCompare=0;//���к������ж�1
		if((Min_blue>290)||(Max_blue>290)) 
			flag_MaxMinCompare=0;//���к������ж�2
	}
	if(flag_MaxMinCompare==1)//�����ɸѡ�ɹ�  ���ұ߽��ȡ�ɹ� ���Һ���
	{
		ChangePoint_Analysis_320();//��ɫ�����У�320��������,��ã�TableChangePoint_320[b]��� ���ұ߽��ڶ�ֵ��
		ChangePoint_Show_320();//320�����������ʾ
		i=SegmentationChar(); 
		
		if(i==8)//�ַ��ָ�,���طָ���ַ������������жϺϷ���
		{
			ZhiFuShiBie();//�ַ�ʶ��	
		}
		else
		{
			LCD_Fill(0x6666);//��������ʾMeasure Faill
			LCD_ShowChar(8*1,200,'M',0);
			LCD_ShowChar(8*2,200,'e',0);
			LCD_ShowChar(8*3,200,'a',0);
			LCD_ShowChar(8*4,200,'s',0);
			LCD_ShowChar(8*5,200,'u',0);
			LCD_ShowChar(8*6,200,'r',0);
			LCD_ShowChar(8*7,200,'e',0);
			
			LCD_ShowChar(8*9,200,'F',0);
			LCD_ShowChar(8*10,200,'a',0);
			LCD_ShowChar(8*11,200,'i',0);
			LCD_ShowChar(8*12,200,'l',0);
			LCD_ShowChar(8*13,200,'l',0);

			delay_ms(800);
		}
	}
}

//��ʱ��3�жϷ������	 
void TIM3_IRQHandler(void)
{ 		    		  			    
	if(TIM3->SR&0X0001)//����ж�
	{
//		LED();
		if(tim3_num==60)
		{
			if(table_card[0][0]!=0)//��1���ʱ
			{
				table_card[0][7]++;
			}		
			if(table_card[1][0]!=0)//��2���ʱ
			{
				table_card[1][7]++;
			}
			if(table_card[2][0]!=0)//��3���ʱ
			{
				table_card[2][7]++;
			}	
			if(table_card[3][0]!=0)//��4���ʱ
			{
				table_card[3][7]++;
			}	
			if(table_card[4][0]!=0)//��5���ʱ
			{
				table_card[4][7]++;
			}
			tim3_num=0;
		}		
	}			
	tim3_num++;
	TIM3->SR&=~(1<<0);//����жϱ�־λ 	    
}
void TIM3_Configuration(void)
	{
	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	/* ---------------------------------------------------------------
	TIM3CLK ��PCLK1=36MHz
	TIM3CLK = 36 MHz, Prescaler = 7200, TIM3 counter clock = 5K,���ı�һ��Ϊ5K,���ھ�Ϊ10K
	--------------------------------------------------------------- */
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 2000; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������10000Ϊ1000ms
	TIM_TimeBaseStructure.TIM_Prescaler =(12800-1); //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	/* Enables the Update event for TIM3 */
	//TIM_UpdateDisableConfig(TIM3,ENABLE); 	//ʹ�� TIM3 �����¼� 
	
	/* TIM IT enable */
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM3, //TIM2
		TIM_IT_Update  |  //TIM �ж�Դ
		TIM_IT_Trigger,   //TIM �����ж�Դ 
		ENABLE  //ʹ��
		);
	
	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����
}

int main(void)
{  
	unsigned int num=0;
 
	Stm32_Clock_Init(16);//��ʼ��ʱ��
//	Led_init();			//��ʼ�� LED	
	Lcd_Gpio_Init();
	LCD_Init();	
	Key_init();	//��ʼ�� KEY1 PA8
	OV7670_Gpio_Init();//OV7670���ų�ʼ�������ڴ��ڳ�ʼ��ǰ��
	GPIO_WriteBit(FIFO_OE_PORT, FIFO_OE_PIN, 0);
	//USART1_init();//��ʼ������	
	ESP8266_Init();      //8266��ʼ�� ������115200
#ifdef WIFI	
	ESP8266_AT_Test (); //ATָ�����
	ESP8266_Net_Mode_Choose (STA_AP); //����ģʽ 
	ESP8266_Enable_MultipleId ( ENABLE );  //����Ϊ������ģʽ
	ESP8266_StartOrShutServer(ENABLE,"8080","100"); //����������ģʽ���˿�8080
#endif 
	TIM3_Configuration();//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms  
	LCD_Fill(0x6666);		
	printf("Welecom\r\n");
	while(!Sensor_init());
	
	LCD_Fill(0xF800);
	delayms(100);
	num=2;
//��ֵ����ֵ	
	R_a=24;
	G_a=53;
	B_a=24;

	while(1)
	{
			if(num<=1)
			{
				Data_LCD_ColorChange();//���Ʋⶨ
			}
			if(num>1)
			{
				Data_LCD_ColorChange_Test();//����ͷɨ�����
				LCD_ShowNum(30,220,21-num,2);//
				while(GPIO_ReadInputDataBit(KEY1_PORT,KEY1_PIN)==0)
				{
					LCD_Fill(0x00);//����
					Show_Title();//��ʾ����
					Show_Card(0);//��ʾ�ڼ��鳵��
					Show_Card(1);
					Show_Card(2);
					Show_Card(3);
					Show_Card(4);
					delay_ms(5000);
				}
			}
		if(num==20)
		{
			 num=0;
		}
			 num++;
	}
 
}


