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
static FRESULT Creat_File(char *name_budder,char *excel_top_name);//文件名，表头
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
	//程序修订
	u8 res=0,flag;
	u8 cehc_flg=0,open_check=0;
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
	//delay_ms(100);
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
	sprintf(write_data,"\r\n%d-%d-%d  %d:%d:%d\r\n",systmtime.tm_year,systmtime.tm_mon,systmtime.tm_mday,systmtime.tm_hour,systmtime.tm_min,systmtime.tm_sec);
	//Creat_File(name_budder,excel_top_name);


	SD_Test_Init(8000,50000,5);//3000块，0次开始，5s
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


#if TEST_SD_NUM_MODE

void  SD_Test_Init(u32 blr,u32 times,u8 sec)
{
	int i;
	u8 flag;
	char temp[100];
	u32 tim_fir,tim_cishu;
	my_sd_data.test_times=sec;//读写速度测试时间为5S
	my_sd_data.test_blc.block_num=blr<<9;
	//初始次数
	my_sd_data.test_blc.times=times;
	
	my_sd_data.now_err=0;//错误0个
	my_sd_data.write_flag=0;//写入标志
	
	for(i=0;i<512;i++)
	{
		my_sd_data.mshu_nuf[0].buf[i]=0x00;
		my_sd_data.mshu_nuf[1].buf[i]=0xff;
	}
	
	tim_fir=my_sd_data.test_blc.times;
	
	//开启定时器
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
	sprintf(temp,"测试的块号:%lld  当前读写次数:%d   平均写速度：%d次每秒  %dKB每秒",my_sd_data.test_blc.block_num>>9,my_sd_data.test_blc.times,my_sd_data.speed,my_sd_data.speed/2);
	save_log(temp,1);
//	Ding_Set_time=3;
//	TIM_Cmd(TIM6,ENABLE);
//	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
//	while(Ding_Set_time>0)
//	{
//		delay_us(300);
//		if(SD_ReadBlock(my_sd_data.read_buf,my_sd_data.test_blc.block_num,512))
//		{
//			//读出错误
//			//save_log("Read Error",1);
//			//save_error_log();
//		}
//		Du_cishu++;
//	}
//	delay_us(300);
//	flag=SD_ReadBlock(my_sd_data.read_buf,my_sd_data.test_blc.block_num,512);
//	sprintf(temp,"最大读速度%d次每s  %dKB每秒  错误代码:%d ",Du_cishu/3,Du_cishu/6,flag);
//	printf("hhh:%d",flag);
//	save_log(temp,1);
	ILI9341_DispStringLine_EN(LINE(2),"SD Begin");
	
}



//0是写0
u8 Xie_Du_Test(u8 mode,u8 jiaoyan_flag)
{
		u8 flag,rec=1;
			//写入 1
		flag=SD_WriteBlock(my_sd_data.mshu_nuf[mode].buf,my_sd_data.test_blc.block_num,8);
			//flag=SD_WriteBlock((u8 *)"hello,world",6000<<9,8);
		if(flag!=0)
		{
			printf("%d  ",flag);
			//写入错误
			save_log("Write Error!",1);
			save_error_log();
			return 0;
		}
		my_sd_data.test_blc.times++;
//		//校验 读出
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
			//读出错误
			save_log("Read Error",1);
			save_error_log();
			return 0;
		}
		delay_ms(1);
//		//校验
		for(i=0;i<512;i++)
		{
			printf("%d   ",my_sd_data.read_buf[i]);
			if(my_sd_data.read_buf[i]!=my_sd_data.mshu_nuf[mode].buf[0])
			{
				//记录一下
				//位置和数据
				my_sd_data.err_data[0]=my_sd_data.read_buf[i];
				my_sd_data.err_index[0]=i;
				my_sd_data.write_flag=1;//写入标志

				break;
			}
		}
		if(my_sd_data.write_flag==1)
		{
			//写入保存  记录次数
			char temp[50];
			my_sd_data.write_flag=0;
			sprintf(temp,"写入数据为%d   错误位置:%d  错误数据:%d",mode,my_sd_data.err_index[0],my_sd_data.err_data[0]);
			save_log(temp,1);
			save_error_log();
			return 0;

		}
		return 1;
}


















//mode==0 不追加时间信息
void save_log(char *str,char mode)
{
	if(mode==1)
	{
		BJ_TimeVar =RTC_GetCounter() + TIME_ZOOM;
		to_tm(BJ_TimeVar, &systmtime);/*把定时器的值转换为北京时间*/	
		sprintf(write_data,"%d:%d:%d   ",systmtime.tm_hour,systmtime.tm_min,systmtime.tm_sec);
		Write_One_shuju(name_budder,write_data);
	}
	sprintf(write_data,"%s\r\n",str);
	Write_One_shuju(name_budder,write_data);
}

void save_error_log()
{
	//写入错误的log
	//读写次数
	char temp[100];
	sprintf(temp,"当前的块号:%lld  当前读写次数:%d    读写速度：%d次每s",my_sd_data.test_blc.block_num>>9,my_sd_data.test_blc.times,my_sd_data.speed);
	save_log(temp,0);
}

void save_state_log()
{
		char temp[100];
		sprintf(temp,"当前的块号:%lld  当前读写次数:%d 未出现异常",my_sd_data.test_blc.block_num>>9,my_sd_data.test_blc.times);
		save_log(temp,1);

}
#endif