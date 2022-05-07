#ifndef __MAIN_H
#define	__MAIN_H
#include "stm32f10x.h"   // 相当于51单片机中的  #include <reg51.h>
/**************引用头文件****************/
#include "bsp_usart.h"
#include "delay.h"
#include "mysd.h"
#include "ff.h"			/* Declarations of FatFs API */
#include "diskio.h"		/* Declarations of disk I/O functions*/
#include "malloc.h"
#include "bsp_adc.h"
#include "bsp_rtc.h"
#include "bsp_ili9341_lcd.h"



/****************变量***********************/
FATFS fs;													/* FatFs文件系统对象 */
FIL fnew;													/* 文件对象 */
FRESULT res_sd,res_flash;                /* 文件操作结果 */
/*时间结构体，默认时间2000-01-01 00:00:00*/
struct rtc_time systmtime=
{
0,0,0,1,1,2000,0
};

#endif



