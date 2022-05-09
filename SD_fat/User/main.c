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
static FRESULT Creat_File(char *name_budder,char *excel_top_name);//文件名，表头
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
	to_tm(BJ_TimeVar, &systmtime);/*把定时器的值转换为北京时间*/	
	
	Time_Base_Dingshi_Mode(7200-1,10000-1);
	
	
	
	
	ILI9341_Init ();         //LCD 初始化
	ILI9341_GramScan ( 6 );
	LCD_SetFont(&Font16x24);
	LCD_SetColors(RED,BLACK);

  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
	/********显示字符串示例*******/
  ILI9341_DispStringLine_EN(LINE(0),"   LCD    ");
#if GET_ADC_VCC_MODE
	ADCx_Init();
#endif

	
//			/* 配置RTC秒中断优先级 */

	
	 //LCD_ClearLine(LINE(4));/* 清除单行文字 */

	fs.drv=0;
	res=f_mount(&fs,"0:",1);
	ILI9341_DispStringLine_EN(LINE(1),"TEST SD");
	Fat_init(res);
	LCD_ClearLine(LINE(1));
	ILI9341_DispStringLine_EN(LINE(1),"SD OK");
	Get_Dev_Infor();
#if GET_ADC_VCC_MODE
	//初始化文件   ADC_VCC_HZ
	ILI9341_DispStringLine_EN(LINE(1),"ADC RECIEVE MODE");
	sprintf(name_budder,"%s","ADC_VCC_10HZ.csv");
	sprintf(excel_top_name,"%s","ADC\r\n");
	Creat_File(name_budder,excel_top_name);
#endif


/************测量SD读写次数*****************/
#if TEST_SD_NUM_MODE
	ILI9341_DispStringLine_EN(LINE(2),"SD TEST MODE");
	sprintf(name_budder,"%s","SD_test.txt");
	sprintf(excel_top_name,"\r\n%d-%d-%d  %d:%d:%d\r\n",systmtime.tm_year,systmtime.tm_mon,systmtime.tm_mday,systmtime.tm_hour,systmtime.tm_min,systmtime.tm_sec);
	//Creat_File(name_budder,excel_top_name);
	Write_One_shuju(name_budder,excel_top_name);
	
	
	
	SD_Test_Init(3000,0,5);//3000块，0次开始，5s
#endif






while(1)
	{
#if GET_ADC_VCC_MODE
		ADC_Caiji_Mode();//ADC采集功能
#endif

			
	}
}








static FRESULT Get_Dev_Infor()
{
	DWORD fre_clust, fre_sect, tot_sect;
	DWORD use_sect;
	 
	FATFS *pfs;
	printf("\n*************** 设备信息获取 ***************\r\n");
  /* 获取设备信息和空簇大小 */
  res_flash = f_getfree("0:", &fre_clust, &pfs);

  /* 计算得到总的扇区个数和空扇区个数 */
  tot_sect = (pfs->n_fatent - 2) * pfs->csize;
  fre_sect = fre_clust * pfs->csize;
	use_sect=tot_sect-fre_sect;
  /* 打印信息(4096 字节/扇区) */
  printf("》SD卡总空间:  %10lu KB。\n》SD卡可用空间:%10lu KB。\n", tot_sect/2, fre_sect/2);
	printf("》SD卡已用空间:%10lu KB。\n",use_sect/2);
	return res_flash;
}

/*创建excel表格*/
static FRESULT Creat_File(char *name_budder,char *excel_top_name)
{
	 UINT br; 
	 //sprintf(name_budder,"0:ADC统计信息%d.csv",name_i++);
	 res_flash = f_open(&fnew,name_budder,FA_WRITE|FA_CREATE_ALWAYS);
	 res_flash = f_write(&fnew,excel_top_name,strlen(excel_top_name),&br);
	 f_close(&fnew);    
	return res_flash;
}

/*传输数据*/
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
			//printf("写成功");
		}
	}
	 f_close(&fnew);
	return res_flash;
}

/***向文件写入一行数据******/

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
			//printf("写成功");
		}
	}
	 f_close(&fnew);
	return res_flash;
}


//挂载文件系统
void Fat_init(u8 res_flash)
{
	if(res_flash == FR_NO_FILESYSTEM)
	{
		printf("》SD还没有文件系统，即将进行格式化...\r\n");
    /* 格式化 */
		res_flash=f_mkfs("0:",0,0);							
		
		if(res_flash == FR_OK)
		{
			printf("》SD成功格式化文件系统。\r");
      /* 格式化后，先取消挂载 */
			res_flash = f_mount(NULL,"0:",1);			
      /* 重新挂载	*/			
			res_flash = f_mount(&fs,"0:",1);
		}
		else
		{
			printf("《《格式化失败。》》\r\n");
			while(1);
		}
	}
  else if(res_flash!=FR_OK)
  {
    printf("！！外部SD挂载文件系统失败。(%d)\r\n",res_flash);
    printf("！！可能原因：SD初始化不成功。\r\n");
		while(1);
  }
  else
  {
    printf(">>文件系统加载成功...\n");
  }
}



void  ADC_Caiji_Mode()
{
		u8 step;//向DMA缓冲器内存储的步进

		if(Adc_Dma_Flag==1)
		{
			Adc_Dma_Flag=0;//清除标志位
			ADC_ConvertedValueLocal[0] =(float) ADC_ConvertedValue[0]/4096*3.3;//转换成电压值
			sprintf((char *)temp_adr,"%.5f\r\n",ADC_ConvertedValueLocal[0]);//保存成5位小数形式
			step=7*1+2;
			temp_adr+=step;
			tra_i++;
			if((tra_i+1)*step>=1024)//大于缓存区，启动传输
			{
				//传输
				tra_i=0;
				temp_adr=SDIO_DATA_BUFFER;//恢复到起始位置
				Tran_ADC_V();
			}
		}
}




void  SD_Test_Init(u32 blr,u32 times,u8 sec)
{
	int i;
	u32 tim_fir,tim_end,tim_cishu;
	my_sd_data.test_times=sec;//读写速度测试时间为5S
	my_sd_data.test_blc.block_num=blr;
	//初始次数
	my_sd_data.test_times=times;
	
	
	
	for(i=0;i<512;i++)
	{
		my_sd_data.buf0[i]=0x00;
		my_sd_data.buf1[i]=0xff;
	}
	
	tim_fir=my_sd_data.test_times;
	
	//开启定时器
	Ding_Con_time=sec;
	while(Ding_Con_time!=0)
	{
		//写入
		SD_WriteBlock(my_sd_data.buf0,blr<<9,512);

	}

	
}
