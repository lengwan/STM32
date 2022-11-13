#include "mytime.h"



//定时器初始化 基本定时器
void Time_Base_Dingshi_Mode(u16 per,u16 pre)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	TIM_TimeBaseInitStruct.TIM_ClockDivision=0;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=per;
	TIM_TimeBaseInitStruct.TIM_Prescaler=pre;
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStruct);
	TIM_Cmd(TIM6,DISABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel=TIM6_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_InitStruct);
	TIM_ClearFlag(TIM6,TIM_IT_Update);
	TIM_ITConfig(TIM6,TIM_IT_Update,DISABLE);
}











