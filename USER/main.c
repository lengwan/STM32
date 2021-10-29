#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "smg.h"
#include "oled.h"

/************************************************
 ALIENTEKս��STM32������ʵ��1
 �����ʵ�� 
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
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
	delay_init(	);	    //��ʱ������ʼ��	
	LED_Init();		  	//��ʼ����LED���ӵ�Ӳ���ӿ�
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
		while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8)temp;  
}