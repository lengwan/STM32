#ifndef __MDELAY_H
#define __MDELAY_H 			   
#include "stm32f10x.h"   // �൱��51��Ƭ���е�  #include <reg51.h>
	 
void delay_init(void);
void delay_ms(u16 nms);
void delay_us(u32 nus);

#endif





























