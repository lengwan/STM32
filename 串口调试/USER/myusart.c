#include "myusart.h"
#include <sys.h>
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	  	 
extern char open_flag;
extern int	mt_times;
RT_buf Rt_buf3,Rt_buf2,Rt_buf1;
//u8 TT_Sum=0;

void Usart3_Data_Init(void)
{
    Rt_buf3.state=Rt_buf3.C_state=0;
    Rt_buf3.Data_Sum=0;
		Rt_buf3.first=0;
}
void Usart2_Data_Init(void)
{
    Rt_buf2.state=Rt_buf2.C_state=0;
    Rt_buf2.Data_Sum=0;
		Rt_buf2.first=0;
}
//????
void Usart1_Data_Init(void)
{
    Rt_buf1.state=Rt_buf1.C_state=0;
    Rt_buf1.Data_Sum=0;
		Rt_buf1.first=0;
}
void Usart3_Data_Recieve_Over()
{
   Rt_buf3.RT_Buffer[Rt_buf3.Data_Sum++]='\0';
    Rt_buf3.state=1;
	Rt_buf3.C_state=1;
	//TIM_Cmd(TIM6,DISABLE);
}
void Usart2_Data_Recieve_Over()
{
   Rt_buf2.RT_Buffer[Rt_buf2.Data_Sum++]='\0';
    Rt_buf2.state=1;
	Rt_buf2.C_state=1;
	//TIM_Cmd(TIM6,DISABLE);
}
void Usart1_Data_Recieve_Over()
{
   Rt_buf1.RT_Buffer[Rt_buf1.Data_Sum++]='\0';
    Rt_buf1.state=1;
	Rt_buf1.C_state=1;
	//TIM_Cmd(TIM6,DISABLE);
}
//????
void Usart3_Data_Recieve(char temp)
{
    if(Rt_buf3.Data_Sum>=MAXSIZE)//??
    {
        Usart3_Data_Recieve_Over();
        return;
    }
    Rt_buf3.RT_Buffer[Rt_buf3.Data_Sum++]=temp;
}
void Usart2_Data_Recieve(char temp)
{
    if(Rt_buf2.Data_Sum>=MAXSIZE)//??
    {
        Usart2_Data_Recieve_Over();
        return;
    }
    Rt_buf2.RT_Buffer[Rt_buf2.Data_Sum++]=temp;
}
void Usart1_Data_Recieve(char temp)
{
    if(Rt_buf1.Data_Sum>=MAXSIZE)//??
    {
        Usart1_Data_Recieve_Over();
        return;
    }
    Rt_buf1.RT_Buffer[Rt_buf1.Data_Sum++]=temp;
}
/*********��������ʼ��***********/
void BaseTim6Init()
{//������ʱ��TIM6��TIM7ʱ��ԴֻΪ�ڲ�84M�����Ը���Ԥ��Ƶ���Զ���װ��ֵ������ʱ��
	//������ʱ���У�����ģʽ�ǹ̶������ϼ��������Բ���Ҫ����
	//���������Ϊ�ظ���������Ϊ�߼���ʱ��׼��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef     NVIC_InitStructure;
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE );
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;//����Ƶ
	TIM_TimeBaseStructure.TIM_Period=700-1;
	TIM_TimeBaseStructure.TIM_Prescaler=7200-1;
TIM_TimeBaseInit(TIM6 ,&TIM_TimeBaseStructure);
//������ʱ���жϿ����ж�ǰ�ȶ�ʱ�������жϱ�־λ
	TIM_ClearFlag(TIM6,TIM_IT_Update);//�����жϱ�־���
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);//�ж�����
	//��ʱ��ʹ��
	TIM_Cmd(TIM6,ENABLE);
	//�ж����ȼ����úͱ�д�жϷ�����
	NVIC_InitStructure.NVIC_IRQChannel=TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStructure);
	//TIM_Cmd(TIM6,DISABLE);
	//printf("ʱ��6\n");
}
void BaseTim7Init()
{//������ʱ��TIM6��TIM7ʱ��ԴֻΪ�ڲ�84M�����Ը���Ԥ��Ƶ���Զ���װ��ֵ������ʱ��
	//������ʱ���У�����ģʽ�ǹ̶������ϼ��������Բ���Ҫ����
	//���������Ϊ�ظ���������Ϊ�߼���ʱ��׼��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef     NVIC_InitStructure;
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE );
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;//����Ƶ
	TIM_TimeBaseStructure.TIM_Period=700-1;
	TIM_TimeBaseStructure.TIM_Prescaler=7200-1;
TIM_TimeBaseInit(TIM7 ,&TIM_TimeBaseStructure);
//������ʱ���жϿ����ж�ǰ�ȶ�ʱ�������жϱ�־λ
	TIM_ClearFlag(TIM7,TIM_IT_Update);//�����жϱ�־���
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);//�ж�����
	//��ʱ��ʹ��
	TIM_Cmd(TIM7,ENABLE);
	//�ж����ȼ����úͱ�д�жϷ�����
	NVIC_InitStructure.NVIC_IRQChannel=TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStructure);
	//TIM_Cmd(TIM6,DISABLE);
	//printf("ʱ��6\n");
}
void BaseTim4Init()
{//������ʱ��TIM6��TIM7ʱ��ԴֻΪ�ڲ�84M�����Ը���Ԥ��Ƶ���Զ���װ��ֵ������ʱ��
	//������ʱ���У�����ģʽ�ǹ̶������ϼ��������Բ���Ҫ����
	//���������Ϊ�ظ���������Ϊ�߼���ʱ��׼��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef     NVIC_InitStructure;
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE );
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;//����Ƶ
	TIM_TimeBaseStructure.TIM_Period=1000-1;
	TIM_TimeBaseStructure.TIM_Prescaler=72-1;
TIM_TimeBaseInit(TIM4 ,&TIM_TimeBaseStructure);
//������ʱ���жϿ����ж�ǰ�ȶ�ʱ�������жϱ�־λ
	TIM_ClearFlag(TIM4,TIM_IT_Update);//�����жϱ�־���
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);//�ж�����
	//��ʱ��ʹ��
	TIM_Cmd(TIM4,ENABLE);
	//�ж����ȼ����úͱ�д�жϷ�����
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStructure);
//	TIM_Cmd(TIM4,DISABLE);
	//printf("ʱ��6\n");
}
void Usart3_Face()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��USART1��GPIOAʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	//USART3_TX   GPIOB.10
	USART_DeInit(USART3);  //��λ����3

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART3_RX	  GPIOB.11��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOA.10  

	USART_InitStructure.USART_BaudRate = 9600;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART3, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART3, ENABLE);   
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	BaseTim6Init();
	TIM_Cmd(TIM6,DISABLE);
	//printf("ds8");
}
/************���ڶ���ʼ��  PA2 PA3******************************/
void Usart2_Pass()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	//USART2_TX   GPIOA.2
		 	//USART_DeInit(USART2);  //��λ����3

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
  
  //USART3_RX	  GPIOB.11��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  
RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,ENABLE);//????2
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,DISABLE);//????
	USART_InitStructure.USART_BaudRate = 115200;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART2, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART2, ENABLE);   
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	BaseTim7Init();
	TIM_Cmd(TIM7,DISABLE);
}
void uart_init(){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = 115200;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 
	BaseTim4Init();
	TIM_Cmd(TIM4,DISABLE);



}
void USART3_IRQHandler(void)
{
	u8 Res;
	//printf("ds");
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//���յ�����
	{	 
		//printf("ok");	
		Res =USART_ReceiveData(USART3);//(USART1->DR);	//��ȡ���յ�������
		if(Rt_buf3.state==0)
		{
			TIM_SetCounter(TIM6,0);
			if(Rt_buf3.first==0)
			{
				TIM_Cmd(TIM6,ENABLE);
				Rt_buf3.first=1;
			}
		}
		Usart3_Data_Recieve(Res);
	}		
USART_ClearITPendingBit(USART3, USART_IT_RXNE);

}	
void USART2_IRQHandler(void)
{
	u8 Res;
	//printf("ds");
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//���յ�����
	{	 
		//printf("ok");	
		Res =USART_ReceiveData(USART2);//(USART1->DR);	//��ȡ���յ�������
		if(Rt_buf2.state==0)
		{
			TIM_SetCounter(TIM7,0);
			if(Rt_buf2.first==0)
			{
				TIM_Cmd(TIM7,ENABLE);
				Rt_buf2.first=1;
			}
		}
		Usart2_Data_Recieve(Res);
	}	
	if(USART_GetFlagStatus(USART2,USART_FLAG_ORE) == SET)
    {
       USART_ReceiveData(USART2);
        USART_ClearFlag(USART2,USART_FLAG_ORE);
   }	
USART_ClearITPendingBit(USART2, USART_IT_RXNE);

}
void USART1_IRQHandler(void)
{
	u8 Res;
	//printf("ds");
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//���յ�����
	{	 
		//printf("ok");	
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
		if(Rt_buf1.state==0)
		{
			TIM_SetCounter(TIM4,0);
			if(Rt_buf1.first==0)
			{
				TIM_Cmd(TIM4,ENABLE);
				Rt_buf1.first=1;
			}
		}
		Usart1_Data_Recieve(Res);
	}	
	if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET)
    {
       USART_ReceiveData(USART1);
        USART_ClearFlag(USART1,USART_FLAG_ORE);
   }	
USART_ClearITPendingBit(USART1, USART_IT_RXNE);

}	

//����3,printf ����
//ȷ��һ�η������ݲ�����USART3_MAX_SEND_LEN�ֽ�
/*void u3_printf(char* fmt,...)  
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF);		//�˴η������ݵĳ���
	for(j=0;j<i;j++)							//ѭ����������
	{
	  while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
		USART_SendData(USART3,USART3_TX_BUF[j]); 
	} 
}*/
  void USART3_Sned_Char(u8 temp)        
 {  
      USART_SendData(USART3,(u8)temp);      
      while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);  
  }
   void USART2_Sned_Char(u8 temp)        
 {  
      USART_SendData(USART2,(u8)temp);      
      while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);  
  }
 void Send3_string(char *s)
{
		while(*s)
		USART3_Sned_Char(*s++);
}
 void Send2_string(char *s)
{
		while(*s)
		USART2_Sned_Char(*s++);
}
    void USART1_Sned_Char(u8 temp)        
 {  
      USART_SendData(USART1,(u8)temp);      
      while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);  
  }
  void Send1_string(char *s)
{
		while(*s)
		USART1_Sned_Char(*s++);
}
void TIM6_IRQHandler()
{
	while(TIM_GetITStatus(TIM6,TIM_IT_Update)!=RESET)
	{
    Usart3_Data_Recieve_Over();
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);  //���TIM7�����жϱ�־    
		TIM_Cmd(TIM6, DISABLE);  //�ر�TIM7 
	}
}
void TIM7_IRQHandler()
{
	while(TIM_GetITStatus(TIM7,TIM_IT_Update)!=RESET)
	{
    Usart2_Data_Recieve_Over();
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update);  //���TIM7�����жϱ�־    
		TIM_Cmd(TIM7, DISABLE);  //�ر�TIM7 
	}
}

void TIM4_IRQHandler()
{
	while(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)
	{
    Usart1_Data_Recieve_Over();
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  //���TIM7�����жϱ�־    
		TIM_Cmd(TIM4, DISABLE);  //�ر�TIM7 
	}
}


