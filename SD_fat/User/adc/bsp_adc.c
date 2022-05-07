#include "bsp_adc.h"

//__IO uint16_t ADC_ConvertedValue[NOFCHANEL]={0,0,0,0};
__IO uint16_t ADC_ConvertedValue[1]={0};
/**
  * @brief  ADC GPIO ��ʼ��
  * @param  ��
  * @retval ��
  */
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// �� ADC IO�˿�ʱ��
	ADC_GPIO_APBxClock_FUN ( ADC_GPIO_CLK, ENABLE );
	
	// ���� ADC IO ����ģʽ
	GPIO_InitStructure.GPIO_Pin = ADC_PIN2|
																		ADC_PIN3|
																		ADC_PIN4|
																		ADC_PIN5;
															
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	// ��ʼ�� ADC IO
	GPIO_Init(ADC_PORT, &GPIO_InitStructure);				
}

/**
  * @brief  ����ADC����ģʽ
  * @param  ��
  * @retval ��
  */
static void ADCx_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	// ��DMAʱ��
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK, ENABLE);
	// ��ADCʱ��
	ADC_APBxClock_FUN ( ADC_CLK, ENABLE );
	
	// ��λDMA������
	DMA_DeInit(ADC_DMA_CHANNEL);
	
	// ���� DMA ��ʼ���ṹ��
	// �����ַΪ��ADC ���ݼĴ�����ַ
	DMA_InitStructure.DMA_PeripheralBaseAddr = ( u32 ) ( & ( ADC_x->DR ) );
	
	// �洢����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC_ConvertedValue;
	
	// ����Դ��������
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	
	// ��������С��Ӧ�õ�������Ŀ�ĵصĴ�С
	DMA_InitStructure.DMA_BufferSize = 4;//������ٸ�
	
	// ����Ĵ���ֻ��һ������ַ���õ���
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

	// �洢����ַ����
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable; 
	
	// �������ݴ�СΪ���֣��������ֽ�
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	
	// �ڴ����ݴ�СҲΪ���֣����������ݴ�С��ͬ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	
	// ѭ������ģʽ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	

	// DMA ����ͨ�����ȼ�Ϊ�ߣ���ʹ��һ��DMAͨ��ʱ�����ȼ����ò�Ӱ��
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	
	// ��ֹ�洢�����洢��ģʽ����Ϊ�Ǵ����赽�洢��
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	// ��ʼ��DMA
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);
	
	// ʹ�� DMA ͨ��
	DMA_Cmd(ADC_DMA_CHANNEL , ENABLE);
	
	
	//����DMA�ж�
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);
	NVIC_InitStruct.NVIC_IRQChannel=DMA1_Channel1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0X01;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0X01;
	NVIC_Init(&NVIC_InitStruct);
	
	// ADC ģʽ����
	// ֻʹ��һ��ADC�����ڵ�ģʽ
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	
	// ɨ��ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE ; 

	// ����ת��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;

	// �����ⲿ����ת���������������
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
//	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
	// ת������Ҷ���
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	
	// ת��ͨ������
	ADC_InitStructure.ADC_NbrOfChannel = NOFCHANEL;	
		
	// ��ʼ��ADC
	ADC_Init(ADC_x, &ADC_InitStructure);
	
	// ����ADCʱ�ӣΪPCLK2��8��Ƶ����9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
	
	// ����ADC ͨ����ת��˳��Ͳ���ʱ��
	//ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL2, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL3, 1, ADC_SampleTime_55Cycles5);//��������  13.5��ת������
//	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL4, 3, ADC_SampleTime_55Cycles5);
//	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL5, 4, ADC_SampleTime_55Cycles5);
	
	// ʹ��ADC DMA ����
	ADC_DMACmd(ADC_x, ENABLE);
	
	// ����ADC ������ʼת��
	ADC_Cmd(ADC_x, ENABLE);
	
	// ��ʼ��ADC У׼�Ĵ���  
	ADC_ResetCalibration(ADC_x);
	// �ȴ�У׼�Ĵ�����ʼ�����
	while(ADC_GetResetCalibrationStatus(ADC_x));
	
	// ADC��ʼУ׼
	ADC_StartCalibration(ADC_x);
	// �ȴ�У׼���
	while(ADC_GetCalibrationStatus(ADC_x));
	
	// ����û�в����ⲿ����������ʹ���������ADCת�� 
	ADC_ExternalTrigConvCmd(ADC_x,ENABLE);		
	//ADC_SoftwareStartConvCmd(ADC_x,ENABLE);
}


/********
��ʱ����ʼ��
************/
void TIM1_INIT(u16 Preoid,u16 Pre)
{
	TIM_TimeBaseInitTypeDef TIM1_Init_Structure;
	TIM_OCInitTypeDef TIM_OCInit_Stru;
	//�򿪶�ʱ��1��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	//��λTIM1
	//TIM_DeInit(TIM1);
	TIM1_Init_Structure.TIM_ClockDivision=TIM_CKD_DIV1;//�ȱ����������
	TIM1_Init_Structure.TIM_CounterMode=TIM_CounterMode_Up;//����ģʽ
	TIM1_Init_Structure.TIM_Period=Preoid-1;//����
	TIM1_Init_Structure.TIM_Prescaler=Pre-1;//��Ƶϵ��
	TIM_TimeBaseInit(TIM1,&TIM1_Init_Structure);
	//����CC1�¼�
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
  * @brief  ADC��ʼ��
  * @param  ��
  * @retval ��
  */
void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_Mode_Config();
	TIM1_INIT(10000,720);
}
/*********************************************END OF FILE**********************/



