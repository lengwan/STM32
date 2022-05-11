#include "main.h"
#define MAX_FILE_SIZE 100
extern __IO uint16_t ADC_ConvertedValue[NOFCHANEL];
float ADC_ConvertedValueLocal[NOFCHANEL]; 
int name_i=0;
int tra_i=0;
int tra_j=0;
u8 stop_flag=1;
char write_data[100]="ADC1,ADC2,ADC3,ADC4\r\n";
char name_budder[20];
u8 *temp_adr=SDIO_DATA_BUFFER;

char Adc_Dma_Flag=0;



void Fat_init(u8 res_flash);
static FRESULT Get_Dev_Infor(void);
static FRESULT Creat_File(char *name_budder,char *excel_top_name);//�ļ�������ͷ
static FRESULT Tran_ADC_V(void);
void  ADC_Caiji_Mode(void);
static FRESULT Write_One_shuju(char *name_budder, char *str);
void save_log(char *str,char mode);
void save_state_log(void);

void save_error_log(void);
u8 Xie_Du_Test(u8 mode,u8 jiaoyan_flag);
u32 Du_cishu=0;


int main(void)
{
	//�����޶�
	u8 res=0,flag;
	u8 cehc_flg=0,open_check=0;
	delay_init();
	USART_Config();
	RTC_NVIC_Config();
	RTC_CheckAndConfig(&systmtime);
	BJ_TimeVar =RTC_GetCounter() + TIME_ZOOM;
	to_tm(BJ_TimeVar, &systmtime);/*�Ѷ�ʱ����ֵת��Ϊ����ʱ��*/	
	
	Time_Base_Dingshi_Mode(7200-1,10000-1);
	
	
	ILI9341_Init ();         //LCD ��ʼ��
	ILI9341_GramScan ( 6 );
	LCD_SetFont(&Font16x24);
	LCD_SetColors(RED,BLACK);

  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */
	/********��ʾ�ַ���ʾ��*******/
  ILI9341_DispStringLine_EN(LINE(0),"   LCD    ");
	//delay_ms(100);
#if GET_ADC_VCC_MODE
	ADCx_Init();
#endif

	
//			/* ����RTC���ж����ȼ� */

	
	 //LCD_ClearLine(LINE(4));/* ����������� */

	fs.drv=0;
	res=f_mount(&fs,"0:",1);
	ILI9341_DispStringLine_EN(LINE(1),"TEST SD");
	Fat_init(res);
	LCD_ClearLine(LINE(1));
	ILI9341_DispStringLine_EN(LINE(1),"SD OK");
	Get_Dev_Infor();
#if GET_ADC_VCC_MODE
	//��ʼ���ļ�   ADC_VCC_HZ
	ILI9341_DispStringLine_EN(LINE(1),"ADC RECIEVE MODE");
	sprintf(name_budder,"%s","ADC_VCC_10HZ.csv");
	sprintf(excel_top_name,"%s","ADC\r\n");
	Creat_File(name_budder,excel_top_name);
#endif


/************����SD��д����*****************/
#if TEST_SD_NUM_MODE
	ILI9341_DispStringLine_EN(LINE(2),"SD TEST MODE");
	sprintf(name_budder,"%s","SD_test.txt");
	sprintf(write_data,"\r\n%d-%d-%d  %d:%d:%d\r\n",systmtime.tm_year,systmtime.tm_mon,systmtime.tm_mday,systmtime.tm_hour,systmtime.tm_min,systmtime.tm_sec);
	//Creat_File(name_budder,excel_top_name);


	SD_Test_Init(8000,50000,5);//3000�飬0�ο�ʼ��5s
#endif



#if TEST_SD_NUM_MODE
	while(stop_flag)
	{
		if(flag==0)
			flag=1;
		else
			flag=0;
		cehc_flg++;
		if(cehc_flg>20)
		{
			cehc_flg=0;
			open_check=1;
			
		}
		stop_flag=Xie_Du_Test(flag,open_check);
		open_check=0;
		if(my_sd_data.test_blc.times%10000==0)
		{
			save_state_log();
		}
	}
	ILI9341_DispStringLine_EN(LINE(5),"SD TEST OVER!");


#endif


while(1)
	{
#if GET_ADC_VCC_MODE
		ADC_Caiji_Mode();//ADC�ɼ�����
#endif
		

		
		
		
		
		

			
	}
}








static FRESULT Get_Dev_Infor()
{
	DWORD fre_clust, fre_sect, tot_sect;
	DWORD use_sect;
	 
	FATFS *pfs;
	printf("\n*************** �豸��Ϣ��ȡ ***************\r\n");
  /* ��ȡ�豸��Ϣ�Ϳմش�С */
  res_flash = f_getfree("0:", &fre_clust, &pfs);

  /* ����õ��ܵ����������Ϳ��������� */
  tot_sect = (pfs->n_fatent - 2) * pfs->csize;
  fre_sect = fre_clust * pfs->csize;
	use_sect=tot_sect-fre_sect;
  /* ��ӡ��Ϣ(4096 �ֽ�/����) */
  printf("��SD���ܿռ�:  %10lu KB��\n��SD�����ÿռ�:%10lu KB��\n", tot_sect/2, fre_sect/2);
	printf("��SD�����ÿռ�:%10lu KB��\n",use_sect/2);
	return res_flash;
}

/*����excel���*/
static FRESULT Creat_File(char *name_budder,char *excel_top_name)
{
	 UINT br; 
	 //sprintf(name_budder,"0:ADCͳ����Ϣ%d.csv",name_i++);
	 res_flash = f_open(&fnew,name_budder,FA_WRITE|FA_CREATE_ALWAYS);
	 res_flash = f_write(&fnew,excel_top_name,strlen(excel_top_name),&br);
	 f_close(&fnew);    
	return res_flash;
}

/*��������*/
static FRESULT Tran_ADC_V(void)
{
	 UINT br; 
	 res_flash = f_open(&fnew,name_budder,FA_WRITE|FA_OPEN_ALWAYS);
	if ( res_flash == FR_OK )
	{
		 res_flash = f_lseek(&fnew,f_size(&fnew));
		 if ( res_flash == FR_OK )
		{
			res_flash = f_write(&fnew,SDIO_DATA_BUFFER,sizeof(SDIO_DATA_BUFFER),&br);
			//printf("д�ɹ�");
		}
	}
	 f_close(&fnew);
	return res_flash;
}

/***���ļ�д��һ������******/

static FRESULT Write_One_shuju(char *name_budder, char *str)
{
	 UINT br; 
	 res_flash = f_open(&fnew,name_budder,FA_WRITE|FA_OPEN_ALWAYS);
	if ( res_flash == FR_OK )
	{
		 res_flash = f_lseek(&fnew,f_size(&fnew));
		 if ( res_flash == FR_OK )
		{
			res_flash = f_write(&fnew,str,strlen(str),&br);
			//printf("д�ɹ�");
		}
	}
	 f_close(&fnew);
	return res_flash;
}


//�����ļ�ϵͳ
void Fat_init(u8 res_flash)
{
	if(res_flash == FR_NO_FILESYSTEM)
	{
		printf("��SD��û���ļ�ϵͳ���������и�ʽ��...\r\n");
    /* ��ʽ�� */
		res_flash=f_mkfs("0:",0,0);							
		
		if(res_flash == FR_OK)
		{
			printf("��SD�ɹ���ʽ���ļ�ϵͳ��\r");
      /* ��ʽ������ȡ������ */
			res_flash = f_mount(NULL,"0:",1);			
      /* ���¹���	*/			
			res_flash = f_mount(&fs,"0:",1);
		}
		else
		{
			printf("������ʽ��ʧ�ܡ�����\r\n");
			while(1);
		}
	}
  else if(res_flash!=FR_OK)
  {
    printf("�����ⲿSD�����ļ�ϵͳʧ�ܡ�(%d)\r\n",res_flash);
    printf("��������ԭ��SD��ʼ�����ɹ���\r\n");
		while(1);
  }
  else
  {
    printf(">>�ļ�ϵͳ���سɹ�...\n");
  }
}



void  ADC_Caiji_Mode()
{
		u8 step;//��DMA�������ڴ洢�Ĳ���

		if(Adc_Dma_Flag==1)
		{
			Adc_Dma_Flag=0;//�����־λ
			ADC_ConvertedValueLocal[0] =(float) ADC_ConvertedValue[0]/4096*3.3;//ת���ɵ�ѹֵ
			sprintf((char *)temp_adr,"%.5f\r\n",ADC_ConvertedValueLocal[0]);//�����5λС����ʽ
			step=7*1+2;
			temp_adr+=step;
			tra_i++;
			if((tra_i+1)*step>=1024)//���ڻ���������������
			{
				//����
				tra_i=0;
				temp_adr=SDIO_DATA_BUFFER;//�ָ�����ʼλ��
				Tran_ADC_V();
			}
		}
}


#if TEST_SD_NUM_MODE

void  SD_Test_Init(u32 blr,u32 times,u8 sec)
{
	int i;
	u8 flag;
	char temp[100];
	u32 tim_fir,tim_cishu;
	my_sd_data.test_times=sec;//��д�ٶȲ���ʱ��Ϊ5S
	my_sd_data.test_blc.block_num=blr<<9;
	//��ʼ����
	my_sd_data.test_blc.times=times;
	
	my_sd_data.now_err=0;//����0��
	my_sd_data.write_flag=0;//д���־
	
	for(i=0;i<512;i++)
	{
		my_sd_data.mshu_nuf[0].buf[i]=0x00;
		my_sd_data.mshu_nuf[1].buf[i]=0xff;
	}
	
	tim_fir=my_sd_data.test_blc.times;
	
	//������ʱ��
	Ding_Set_time=sec;
	TIM_Cmd(TIM6,ENABLE);
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
	while(Ding_Set_time>0)
	{
		Xie_Du_Test(0,0);
    Xie_Du_Test(1,0);

	}
	tim_cishu=my_sd_data.test_blc.times-tim_fir;
	my_sd_data.speed=tim_cishu/sec;
	sprintf(temp,"���ԵĿ��:%lld  ��ǰ��д����:%d   ƽ��д�ٶȣ�%d��ÿ��  %dKBÿ��",my_sd_data.test_blc.block_num>>9,my_sd_data.test_blc.times,my_sd_data.speed,my_sd_data.speed/2);
	save_log(temp,1);
//	Ding_Set_time=3;
//	TIM_Cmd(TIM6,ENABLE);
//	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
//	while(Ding_Set_time>0)
//	{
//		delay_us(300);
//		if(SD_ReadBlock(my_sd_data.read_buf,my_sd_data.test_blc.block_num,512))
//		{
//			//��������
//			//save_log("Read Error",1);
//			//save_error_log();
//		}
//		Du_cishu++;
//	}
//	delay_us(300);
//	flag=SD_ReadBlock(my_sd_data.read_buf,my_sd_data.test_blc.block_num,512);
//	sprintf(temp,"�����ٶ�%d��ÿs  %dKBÿ��  �������:%d ",Du_cishu/3,Du_cishu/6,flag);
//	printf("hhh:%d",flag);
//	save_log(temp,1);
	ILI9341_DispStringLine_EN(LINE(2),"SD Begin");
	
}



//0��д0
u8 Xie_Du_Test(u8 mode,u8 jiaoyan_flag)
{
		u8 flag,rec=1;
			//д�� 1
		flag=SD_WriteBlock(my_sd_data.mshu_nuf[mode].buf,my_sd_data.test_blc.block_num,8);
			//flag=SD_WriteBlock((u8 *)"hello,world",6000<<9,8);
		if(flag!=0)
		{
			printf("%d  ",flag);
			//д�����
			save_log("Write Error!",1);
			save_error_log();
			return 0;
		}
		my_sd_data.test_blc.times++;
//		//У�� ����
		if(jiaoyan_flag==1)
		{
			rec=Sd_Data_jiaoyan(mode);
		}
		return rec;
		
}





u8  Sd_Data_jiaoyan(char mode)
{
	int i;
	if(SD_ReadBlock(my_sd_data.read_buf,my_sd_data.test_blc.block_num,512))
		{
			//��������
			save_log("Read Error",1);
			save_error_log();
			return 0;
		}
		delay_ms(1);
//		//У��
		for(i=0;i<512;i++)
		{
			printf("%d   ",my_sd_data.read_buf[i]);
			if(my_sd_data.read_buf[i]!=my_sd_data.mshu_nuf[mode].buf[0])
			{
				//��¼һ��
				//λ�ú�����
				my_sd_data.err_data[0]=my_sd_data.read_buf[i];
				my_sd_data.err_index[0]=i;
				my_sd_data.write_flag=1;//д���־

				break;
			}
		}
		if(my_sd_data.write_flag==1)
		{
			//д�뱣��  ��¼����
			char temp[50];
			my_sd_data.write_flag=0;
			sprintf(temp,"д������Ϊ%d   ����λ��:%d  ��������:%d",mode,my_sd_data.err_index[0],my_sd_data.err_data[0]);
			save_log(temp,1);
			save_error_log();
			return 0;

		}
		return 1;
}


















//mode==0 ��׷��ʱ����Ϣ
void save_log(char *str,char mode)
{
	if(mode==1)
	{
		BJ_TimeVar =RTC_GetCounter() + TIME_ZOOM;
		to_tm(BJ_TimeVar, &systmtime);/*�Ѷ�ʱ����ֵת��Ϊ����ʱ��*/	
		sprintf(write_data,"%d:%d:%d   ",systmtime.tm_hour,systmtime.tm_min,systmtime.tm_sec);
		Write_One_shuju(name_budder,write_data);
	}
	sprintf(write_data,"%s\r\n",str);
	Write_One_shuju(name_budder,write_data);
}

void save_error_log()
{
	//д������log
	//��д����
	char temp[100];
	sprintf(temp,"��ǰ�Ŀ��:%lld  ��ǰ��д����:%d    ��д�ٶȣ�%d��ÿs",my_sd_data.test_blc.block_num>>9,my_sd_data.test_blc.times,my_sd_data.speed);
	save_log(temp,0);
}

void save_state_log()
{
		char temp[100];
		sprintf(temp,"��ǰ�Ŀ��:%lld  ��ǰ��д����:%d δ�����쳣",my_sd_data.test_blc.block_num>>9,my_sd_data.test_blc.times);
		save_log(temp,1);

}
#endif