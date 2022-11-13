#include <stm32f4xx.h>
#include "SPI_MSD_Driver.h"
#include "ff.h"
#include <user.h>
#include <stdio.h>
#include <string.h>
FATFS fs;         /* Work area (file system object) for logical drive */
FIL fsrc1,fsrc2,fsrc3,fsrc4, fsrc;       /* file objects */   
FRESULT resf,resm,res1,res2,res3,res4;
UINT br1,br2,br3,br4,br;
//uint32_t   fadc[4];
const uint32_t Sine12bit[32] = {
                      2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
                      3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909, 
                      599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};	
BYTE spi3Buff1[1024];
BYTE spi3Buff2[1024];
BYTE *spi3Buff[2]={spi3Buff1,spi3Buff2};
BYTE read_buf[530*4];
 BYTE y2cflag=0;
 int i1=0,i2=0,i3=0,i4=0,i5=0;
struct record{ FIL fsrc1,fsrc2,fsrc3,fsrc4;
	 BYTE y2cflag;
	int i1,i2,i3,i4,i5;
}record;
//FlagStatus DMAtcflag=SET;
char* fname1[]={"f1.txt","f2.txt","f3.txt","f4.txt","f5.txt"};
char* fname2[]={"ff1.txt","ff2.txt","ff3.txt","ff4.txt","ff5.txt"};
void gpioconfig(void);
void EXTIconfig(void);
void iwdgconfig(void);
void tim2config(void);
void dacconfig(void);
void usartconfig(void);
void spi2config(void);
void spi3config(void);
void initfilestatus(void);
int main()
{
	gpioconfig();//GPIO初始化；
	EXTIconfig();
 
	tim2config();//定时器初始化 ,ADCclock输出
  dacconfig();//DAC初始化；

	spi2config();//SPI初始化；
  usartconfig();//串口初始化；
  spi3config();	
	iwdgconfig();//独立看门狗初始化
	
  resf=f_mkfs(0, 0, 0);
	while(resf!= FR_OK) {};
  resm=f_mount(0,&fs);
	while(resm!= FR_OK) {};	
//res3=f_open(&fsrc1,fname1[1],FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
//	res4=f_open(&fsrc2,"ff1.txt",FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
	DAC_DMACmd(DAC_Channel_1, ENABLE);
	//adc();
f_open(&fsrc,"record.txt",FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
	f_read(&fsrc,&record, sizeof(record), &br);	
	initfilestatus();
		
	while(1)
	{
		if(y2cflag==0)
    {
			if(fsrc2.fs == 0)
			{
				res2=f_open(&fsrc2,fname1[i2],FA_OPEN_EXISTING|FA_READ);
				f_lseek (&fsrc2, record.fsrc2.fptr);
			}
			if(res2== FR_OK&&i1!=i2)
			{
			f_read(&fsrc2 ,read_buf,sizeof(read_buf),&br2);
				//zhizhenyiwei  
			//yasuo;
			if(f src3.fs == 0)
			{
				res3=f_open(&fsrc3,fname2[i3],FA_CREATE_ALWAYS  |FA_WRITE);
				f_lseek (&fsrc3, record.fsrc3.fptr);
			}
			if(res3== FR_OK)
			{
			f_write(&fsrc3, read_buf, sizeof(read_buf), &br3);
			}
			if(br2<sizeof(read_buf)) 
					{
						f_close(&fsrc2);
						f_unlink(fname1[i2]);
						i2++;
					}
			if(i2==5) i2=0;
			if(fsrc3.fptr>FFSIZE)
			{
				f_close(&fsrc3);
				i3++;
				if(i3==5) i3=0;
			}
			}
			
		}
		else
		{
			if(fsrc3.fs!=0)
      {
			f_close(&fsrc3);
		  i3=0;
			}
			if(fsrc4.fs == 0)
			{
			res4=f_open(&fsrc4,fname2[i4],FA_READ);
			f_lseek (&fsrc4, record.fsrc4.fptr);
			if(res4==FR_OK)
			{
			f_read(&fsrc4 ,spi3Buff[i5],sizeof(spi3Buff[i5]),&br4);
			}
			}
			if(res4==FR_OK)
			{ 
			DMA_InitTypeDef DMA_InitStructure;
			DMA_InitStructure.DMA_BufferSize = br4;
			DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)spi3Buff[i5] ;
      DMA_Init(DMA1_Stream7, &DMA_InitStructure);
			DMA_Cmd(DMA1_Stream7,DISABLE);
			DMA_Cmd(DMA1_Stream7,ENABLE);
			SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);
			if(br4<sizeof(spi3Buff[i5])) 
					{
						f_close(&fsrc4);
						f_unlink(fname1[i4]);
						i4++;
					}			
      i5++;
			if(i5==2) i5=0;
			if(fsrc4.fs !=0)
			{
			f_read(&fsrc4 ,spi3Buff[i5],sizeof(spi3Buff[i5]),&br4);
			}			
			
			while(DMA_GetFlagStatus(DMA1_Stream7,DMA_FLAG_TCIF7 )==RESET);
			DMA_ClearFlag(DMA1_Stream7,DMA_FLAG_TCIF7 );
			}
			else
			{
				i4=0;
				y2cflag=0;
				USART_SendData( USART1, Transferover);
			}
		}
		f_close(&fsrc);
		f_open(&fsrc,"record.txt",FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
		f_write(&fsrc,&record, sizeof(record), &br);
		
	  IWDG_ReloadCounter();//刷新看门狗    
 }
	
}

// si lu kongzhi
void gpioconfig()
{
	//控制信号输出端口设置
	 GPIO_InitTypeDef        GPIO_InitStructure;
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
}
 
//caiyang zhunbeihao
void EXTIconfig()
{
	EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

  /* Enable GPIOA clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  /* Configure PA0 pin as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Connect EXTI Line0 to PA0 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);

  /* Configure EXTI Line0 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  /* Enable and set EXTI Line0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

//kanmengou
void iwdgconfig()
{
	/* Get the LSI frequency:  TIM5 is used to measure the LSI frequency */
  //uwLsiFreq = GetLSIFrequency();
   RCC_LSICmd(ENABLE);
  /* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
     dis
	persion) */
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

  /* IWDG counter clock: LSI/32 */
  IWDG_SetPrescaler(IWDG_Prescaler_32);

  /* Set counter reload value to obtain 250ms IWDG TimeOut.
     IWDG counter clock Frequency = LsiFreq/32
     Counter Reload Value = 250ms/IWDG counter clock period
                          = 0.25s / (32/LsiFreq)
                          = LsiFreq/(32 * 4)
                          = LsiFreq/128
   */   
  IWDG_SetReload(1000);

  /* Reload IWDG counter */
  IWDG_ReloadCounter();

  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();
}

//dac chufa
void tim2config()
{
	 TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
//	 TIM_OCInitTypeDef  TIM_OCInitStructure;
//	 uint16_t CCR1_Val = 333;
	 uint16_t PrescalerValue = 0;
//	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  /* Time base configuration */
	PrescalerValue = (uint16_t) (SystemCoreClock / 8000000) - 1;
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 665;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  //TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
  
	//TIM_TimeBaseStructure.TIM_Period = 665;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  /* TIM2 TRGO selection */
  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
  
  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);
//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
//  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
//  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
//  TIM_OC1Init(TIM4, &TIM_OCInitStructure);
//  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable); 
//	 TIM_ARRPreloadConfig(TIM4, ENABLE);

//  /* TIM3 enable counter */
//  TIM_Cmd(TIM4, ENABLE);
	 
}

//DAC dmashuchu saomiao
void dacconfig()
 {
	 	GPIO_InitTypeDef GPIO_InitStructure;
	  DAC_InitTypeDef            DAC_InitStructure;
    DMA_InitTypeDef            DMA_InitStructure;
	  NVIC_InitTypeDef           NVIC_InitStructure;
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
  /* DAC Periph clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 
	   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
     GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
     GPIO_Init(GPIOA, &GPIO_InitStructure);
	        DAC_DeInit(); 
	        DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;
          DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
          DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

          DMA_DeInit(DMA1_Stream5); 
	        DMA_InitStructure.DMA_Channel = DMA_Channel_7;  
          DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R1_ADDRESS;
          DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&Sine12bit;
          DMA_InitStructure.DMA_BufferSize = 1024;
          DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
          DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
          DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
          DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
          DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
          DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
          DMA_InitStructure.DMA_Priority = DMA_Priority_High;
          DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
          DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
          DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
          DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
          DMA_Init(DMA1_Stream5, &DMA_InitStructure); 
						NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
					 NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
           NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
           NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
           NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
           NVIC_Init(&NVIC_InitStructure);  
					/* Enable DMA1 Channel3 */
          DMA_Cmd(DMA1_Stream5, ENABLE);
          DMA_ITConfig(DMA1_Stream5, DMA_IT_TC|DMA_IT_HT, ENABLE);
            /* DAC Channel2 Init */
          DAC_Init(DAC_Channel_1, &DAC_InitStructure);

          /* Enable DAC Channel2 */
          DAC_Cmd(DAC_Channel_1, ENABLE);

          /* Enable DMA for DAC Channel2 */
        //  DAC_DMACmd(DAC_Channel_1, ENABLE);
}

//422
void usartconfig()
{
	USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	GPIO_PinAFConfig(GPIOA , GPIO_PinSource9, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA,  GPIO_PinSource10,  GPIO_AF_USART1);
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9 ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA , &GPIO_InitStructure);
	USART_OverSampling8Cmd(USART1, ENABLE); 
	
	USART_InitStructure.USART_BaudRate = 5250000;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);
  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable USART */
  USART_Cmd(USART1, ENABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

void spi2config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 , ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC , ENABLE); 

	GPIO_DeInit(GPIOB);
	GPIO_DeInit(GPIOC);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_SPI2);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF_SPI2);    
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_SPI2);
	

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

  /* SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* SPI  MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2;
  GPIO_Init(GPIOC, &GPIO_InitStructure);  

  /* SPI  MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
	SPI_I2S_DeInit(SPI2);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);
	
	SPI_Cmd(SPI2, ENABLE);
	
  
}

void spi3config()
{

	GPIO_InitTypeDef GPIO_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3 , ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC , ENABLE); 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	GPIO_DeInit(GPIOC);

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SPI3);    
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

  /* SPI SCK pin configuration */
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	/* SPI  MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;
  GPIO_Init(GPIOC, &GPIO_InitStructure);  

  /* SPI  MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	

	SPI_I2S_DeInit(SPI3);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Init(SPI3, &SPI_InitStructure);
	SPI_Cmd(SPI3, ENABLE);
	DMA_DeInit( DMA1_Stream7);
  
  
  
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
  DMA_InitStructure.DMA_FIFOThreshold =  DMA_FIFOThreshold_HalfFull ;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte  ;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(SPI3->DR)) ;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium ;
  /* Configure TX DMA */
  DMA_InitStructure.DMA_Channel = DMA_Channel_0 ;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;
  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)spi3Buff1 ;
  DMA_Init(DMA1_Stream7, &DMA_InitStructure);
	DMA_Cmd(DMA1_Stream7,ENABLE);
//	SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Tx, ENABLE);

//  /* Enable SPI DMA RX Requsts */
//  SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Rx, ENABLE);
  
}

void initfilestatus()
{
	fsrc1=record.fsrc1;
	fsrc2=record.fsrc2;
	fsrc3=record.fsrc3;
	fsrc4=record.fsrc4;
	y2cflag=record.y2cflag;
	i1=record.i1;
	i2=record.i2;
	i3=record.i3;
	i4=record.i4;
	i5=record.i5;
	
}
