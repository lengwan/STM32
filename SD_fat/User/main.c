#include "main.h"
#define MAX_FILE_SIZE 100
extern __IO uint16_t ADC_ConvertedValue[NOFCHANEL];
float ADC_ConvertedValueLocal[NOFCHANEL]; 
int name_i=0;
int tra_i=0;
int tra_j=0;
char excel_top_name[100]="ADC1,ADC2,ADC3,ADC4\r\n";
char name_budder[20];
u8 *temp_adr=SDIO_DATA_BUFFER;

char Adc_Dma_Flag=0;



void Fat_init(u8 res_flash);
static FRESULT Get_Dev_Infor(void);
static FRESULT Creat_File(char *name_budder,char *excel_top_name);//�ļ�������ͷ
static FRESULT Tran_ADC_V(void);
void  ADC_Caiji_Mode(void);
static FRESULT Write_One_shuju(char *name_budder, char *str);




int main(void)
{
	//
	u8 res=0;
	uint32_t BJ_TimeVar;
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
	sprintf(excel_top_name,"\r\n%d-%d-%d  %d:%d:%d\r\n",systmtime.tm_year,systmtime.tm_mon,systmtime.tm_mday,systmtime.tm_hour,systmtime.tm_min,systmtime.tm_sec);
	//Creat_File(name_budder,excel_top_name);
	Write_One_shuju(name_budder,excel_top_name);
	
	
	
	SD_Test_Init(3000,0,5);//3000�飬0�ο�ʼ��5s
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




void  SD_Test_Init(u32 blr,u32 times,u8 sec)
{
	int i;
	u32 tim_fir,tim_end,tim_cishu;
	my_sd_data.test_times=sec;//��д�ٶȲ���ʱ��Ϊ5S
	my_sd_data.test_blc.block_num=blr;
	//��ʼ����
	my_sd_data.test_times=times;
	
	
	
	for(i=0;i<512;i++)
	{
		my_sd_data.buf0[i]=0x00;
		my_sd_data.buf1[i]=0xff;
	}
	
	tim_fir=my_sd_data.test_times;
	
	//������ʱ��
	Ding_Con_time=sec;
	while(Ding_Con_time!=0)
	{
		//д��
		SD_WriteBlock(my_sd_data.buf0,blr<<9,512);

	}

	
}
