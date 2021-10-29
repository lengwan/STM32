#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "smg.h"
#include "oled.h"

/************************************************
 ALIENTEK战舰STM32开发板实验1
 跑马灯实验 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/


union float_temp
{
	float f_data;
	u8 u_temp[4]; 
};
union float_temp send_buf;
void Send_char(char temp);

 int main(void)
 {	
  char  xieyi[4]={0x03,0xfc,0xfc,0x03};
	char i=0;
	delay_init(	);	    //延时函数初始化	
	LED_Init();		  	//初始化与LED连接的硬件接口
	uart_init(115200);
	send_buf.f_data=0.01;
	while(1)
	{
		 LED0=!LED0;
		Send_char(xieyi[0]);
		Send_char(xieyi[1]);
		send_buf.f_data+=0.001;
		if(send_buf.f_data>5)
			send_buf.f_data=0;
		for (i=0;i<4;i++)
		Send_char(send_buf.u_temp[i]);
//		printf("%d",i);
		Send_char(xieyi[2]);
		Send_char(xieyi[3]);
//		printf("%x",xieyi[1]);
//		delay_ms(100);

	}
 }

void Send_char(char temp)
{
		while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8)temp;  
}