#include "sys.h"
#include "delay.h"
#include "myusart.h"

 int main(void)
 {
	char i=0;
	 char flag=0;
	delay_init();	 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
   //延时函数初始化	
	uart_init();
	Usart2_Pass();
	Usart3_Face();
	Rt_buf2.C_state=0;
	Rt_buf1.C_state=0;
	Rt_buf3.C_state=0;
	while(1)
	{
		if(Rt_buf1.C_state==1)
		{
			Send1_string(Rt_buf1.RT_Buffer);
			Send2_string(Rt_buf1.RT_Buffer);
			Send3_string(Rt_buf1.RT_Buffer);
			Usart1_Data_Init();

		}
		if(Rt_buf2.C_state==1)
		{
			Send1_string(Rt_buf2.RT_Buffer);
			Usart2_Data_Init();
		}
		if(Rt_buf3.C_state==1)
		{
			Send1_string(Rt_buf3.RT_Buffer);
			Usart3_Data_Init();
		}
		
	}

 }



