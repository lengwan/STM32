#ifndef MY__USART_H
#define MY__USART_H
#include "stdio.h"	
#include "sys.h" 
#define MAXSIZE 1000
#define DUO PAout(7)

typedef struct
{
   char state;
	 char C_state;//用于主函数处理数据
	 char first;
   u16 Data_Sum;
   char RT_Buffer[MAXSIZE];
}RT_buf;

extern RT_buf Rt_buf3,Rt_buf2,Rt_buf1;
void Usart3_Face(void);
void Send3_string(char *s);
void Usart3_Data_Init(void);
void USART3_Sned_Char(u8 temp);   



void Usart2_Pass(void);
void Send2_string(char *s);
void Usart2_Data_Init(void);
void USART2_Sned_Char(u8 temp); 


void uart_init(void);
void Send1_string(char *s);
void Usart1_Data_Init(void);
void USART1_Sned_Char(u8 temp);

#endif





