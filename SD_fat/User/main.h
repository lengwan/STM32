#ifndef __MAIN_H
#define	__MAIN_H
#include "stm32f10x.h"  
/**************����ͷ�ļ�****************/
#include "bsp_usart.h"
#include "delay.h"
#include "mysd.h"
#include "ff.h"			/* Declarations of FatFs API */
#include "diskio.h"		/* Declarations of disk I/O functions*/
#include "malloc.h"
#include "bsp_adc.h"
#include "bsp_rtc.h"
#include "bsp_ili9341_lcd.h"
#include "string.h"
#include "mytime.h"




#define GET_ADC_VCC_MODE 0  //ADC��ѹ�ɼ�ģʽ
#define TEST_SD_NUM_MODE 1
/****************����***********************/
FATFS fs;													/* FatFs�ļ�ϵͳ���� */
FIL fnew;													/* �ļ����� */
FRESULT res_sd,res_flash;                /* �ļ�������� */
/*ʱ��ṹ�壬Ĭ��ʱ��2000-01-01 00:00:00*/
struct rtc_time systmtime=
{
0,0,0,1,1,2000,0
};
uint32_t BJ_TimeVar;





//��ʱ��ʱ��
char Ding_Con_time=0,Ding_Set_time=0;




#if TEST_SD_NUM_MODE

#define ERROR_NUM 1


typedef struct
{
	u32 times;//��д����
	long long  block_num;//��ַ
}SD_BLOCK;

typedef struct
{
	u8 buf[512];
}shu_nuf;

typedef struct
{
	u32 speed;//1S�ڶ�д����
	u8 test_times;//�����ٶ�ʱ��
	SD_BLOCK test_blc;
	shu_nuf mshu_nuf[2];
	u8 read_buf[512];
	
	
	u8 err_data[ERROR_NUM];
	u16 err_index[ERROR_NUM];
	
	u8 now_err;
	u8 write_flag;
	
	
}SD_TEST;
SD_TEST my_sd_data;



void  SD_Test_Init(u32 blr,u32 times,u8 sec);
u8  Sd_Data_jiaoyan(char mode);



#endif

#endif



