#include "bsp_adc.h"

//__IO uint16_t ADC_ConvertedValue[NOFCHANEL]={0,0,0,0};
__IO uint16_t ADC_ConvertedValue[1]={0};
/**
  * @brief  ADC GPIO 初始化
  * @param  无
  * @retval 无
  */
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 打开 ADC IO端口时钟
	ADC_GPIO_APBxClock_FUN ( ADC_GPIO_CLK, ENABLE );
	
	// 配置 ADC IO 引脚模式
	GPIO_InitStructure.GPIO_Pin = ADC_PIN2|
																		ADC_PIN3|
																		ADC_PIN4|
																		ADC_PIN5;
															
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	// 初始化 ADC IO
	GPIO_Init(ADC_PORT, &GPIO_InitStructure);				
}

/**
  * @brief  配置ADC工作模式
  * @param  无
  * @retval 无
  */
static void ADCx_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	// 打开DMA时钟
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK, ENABLE);
	// 打开ADC时钟
	ADC_APBxClock_FUN ( ADC_CLK, ENABLE );
	
	// 复位DMA控制器
	DMA_DeInit(ADC_DMA_CHANNEL);
	
	// 配置 DMA 初始化结构体
	// 外设基址为：ADC 数据寄存器地址
	DMA_InitStructure.DMA_PeripheralBaseAddr = ( u32 ) ( & ( ADC_x->DR ) );
	
	// 存储器地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC_ConvertedValue;
	
	// 数据源来自外设
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	
	// 缓冲区大小，应该等于数据目的地的大小
	DMA_InitStructure.DMA_BufferSize = 4;//传输多少个
	
	// 外设寄存器只有一个，地址不用递增
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

	// 存储器地址递增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable; 
	
	// 外设数据大小为半字，即两个字节
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	
	// 内存数据大小也为半字，跟外设数据大小相同
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	
	// 循环传输模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	

	// DMA 传输通道优先级为高，当使用一个DMA通道时，优先级设置不影响
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	
	// 禁止存储器到存储器模式，因为是从外设到存储器
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	// 初始化DMA
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);
	
	// 使能 DMA 通道
	DMA_Cmd(ADC_DMA_CHANNEL , ENABLE);
	
	
	//开启DMA中断
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);
	NVIC_InitStruct.NVIC_IRQChannel=DMA1_Channel1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0X01;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0X01;
	NVIC_Init(&NVIC_InitStruct);
	
	// ADC 模式配置
	// 只使用一个ADC，属于单模式
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	
	// 扫描模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE ; 

	// 连续转换模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;

	// 不用外部触发转换，软件开启即可
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
//	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
	// 转换结果右对齐
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	
	// 转换通道个数
	ADC_InitStructure.ADC_NbrOfChannel = NOFCHANEL;	
		
	// 初始化ADC
	ADC_Init(ADC_x, &ADC_InitStructure);
	
	// 配置ADC时钟Ｎ狿CLK2的8分频，即9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
	
	// 配置ADC 通道的转换顺序和采样时间
	//ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL2, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL3, 1, ADC_SampleTime_55Cycles5);//采样周期  13.5个转换周期
//	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL4, 3, ADC_SampleTime_55Cycles5);
//	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL5, 4, ADC_SampleTime_55Cycles5);
	
	// 使能ADC DMA 请求
	ADC_DMACmd(ADC_x, ENABLE);
	
	// 开启ADC ，并开始转换
	ADC_Cmd(ADC_x, ENABLE);
	
	// 初始化ADC 校准寄存器  
	ADC_ResetCalibration(ADC_x);
	// 等待校准寄存器初始化完成
	while(ADC_GetResetCalibrationStatus(ADC_x));
	
	// ADC开始校准
	ADC_StartCalibration(ADC_x);
	// 等待校准完成
	while(ADC_GetCalibrationStatus(ADC_x));
	
	// 由于没有采用外部触发，所以使用软件触发ADC转换 
	ADC_ExternalTrigConvCmd(ADC_x,ENABLE);		
	//ADC_SoftwareStartConvCmd(ADC_x,ENABLE);
}


/********
定时器初始化
************/
void TIM1_INIT(u16 Preoid,u16 Pre)
{
	TIM_TimeBaseInitTypeDef TIM1_Init_Structure;
	TIM_OCInitTypeDef TIM_OCInit_Stru;
	//打开定时器1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	//复位TIM1
	//TIM_DeInit(TIM1);
	TIM1_Init_Structure.TIM_ClockDivision=TIM_CKD_DIV1;//先保留这个参数
	TIM1_Init_Structure.TIM_CounterMode=TIM_CounterMode_Up;//计数模式
	TIM1_Init_Structure.TIM_Period=Preoid-1;//周期
	TIM1_Init_Structure.TIM_Prescaler=Pre-1;//分频系数
	TIM_TimeBaseInit(TIM1,&TIM1_Init_Structure);
	//产生CC1事件
	TIM_OCInit_Stru.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInit_Stru.TIM_OCPolarity=TIM_OCPolarity_Low;
	TIM_OCInit_Stru.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInit_Stru.TIM_Pulse=Preoid/2;
	TIM_OC1Init(TIM1,&TIM_OCInit_Stru);
	TIM_Cmd(TIM1,ENABLE);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);				//?????
	TIM_UpdateDisableConfig(TIM1, DISABLE);
	TIM_CtrlPWMOutputs(TIM1,ENABLE);

}
/**
  * @brief  ADC初始化
  * @param  无
  * @retval 无
  */
void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_Mode_Config();
	TIM1_INIT(10000,720);
}
/*********************************************END OF FILE**********************/



