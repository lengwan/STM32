#ifndef __MDELAY1_H
#define __MDELAY1_H 			   
#include "stm32f10x.h"   // 相当于51单片机中的  #include <reg51.h>
	 
void delay_init(void);
void delay_ms(u16 nms);
void delay_us(u32 nus);

#endif





























