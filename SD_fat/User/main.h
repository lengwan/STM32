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
#define MAX_FILE_SIZE 100
#define SD_CHECK_ERROR_NUM 1

/***********�ṹ�嶨��*****************/
typedef struct
{
	u32 times;//��д����
	long long  block_num;//��ַ
}SD_BLOCK;

typedef struct
{
	u8 buf[512];
}NUM_BUF;

typedef struct
{
	u32 speed;//1S�ڶ�д����
	u8 test_times;//�����ٶ�ʱ��
	SD_BLOCK test_blc;
	NUM_BUF mshu_nuf[2];
	u8 read_buf[512];
	u8 err_data[SD_CHECK_ERROR_NUM];
	u16 err_index[SD_CHECK_ERROR_NUM];
	u8 now_err;
	u8 write_flag;
}SD_TEST;














/***********************************��������************************/

void sd_read_speed_test(void);
void sdfat_init(u8 res_flash);
static FRESULT Get_Dev_Infor(void);
static FRESULT Creat_File(char *name_budder,char *excel_top_name);//�ļ�������ͷ
//static FRESULT Tran_ADC_V(void);
void  ADC_Caiji_Mode(void);
static FRESULT Write_One_shuju(char *name_budder, char *str);
void save_log(char *str,char time_flag,char err_flg,char state_flag);
u8 Xie_Du_Test(u8 mode,u8 jiaoyan_flag);





/*******************��������ѡ��*******************/
#define TEST_SD_MODE 1

#if TEST_SD_MODE
void  SD_Test_Init(u32 blr,u32 times,u8 sec);
u8  Sd_Data_jiaoyan(char mode);

#endif

#endif



