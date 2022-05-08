#include "main.h"
#define MAX_FILE_SIZE 100
// ADC1转换的电压值通过MDA方式传到SRAM
extern __IO uint16_t ADC_ConvertedValue[NOFCHANEL];
// 局部变量，用于保存转换计算后的电压值 	 
float ADC_ConvertedValueLocal[NOFCHANEL]; 
int name_i=0;
int tra_i=0;
int tra_j=0;
char excel_top_name[50]="ADC1,ADC2,ADC3,ADC4\r\n";
char name_budder[20];
u8 *temp_adr=SDIO_DATA_BUFFER;

char Adc_Dma_Flag=0;


void Fat_init(u8 res_flash);
static FRESULT Get_Dev_Infor(void);
static FRESULT Creat_Excel(char *name_budder,char *excel_top_name);//文件名，表头
static FRESULT Tran_ADC_V(void);


int main(void)
{
	//
	u8 res=0;
	u8 step;//向DMA缓冲器内存储的步进
	delay_init();
	USART_Config();
	RTC_NVIC_Config();
	RTC_CheckAndConfig(&systmtime);
	ILI9341_Init ();         //LCD 初始化
	ILI9341_GramScan ( 6 );
	LCD_SetFont(&Font16x24);
	LCD_SetColors(RED,BLACK);

  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
	/********显示字符串示例*******/
  ILI9341_DispStringLine_EN(LINE(0),"   LCD    ");
	ADCx_Init();
	
//			/* 配置RTC秒中断优先级 */

	
	 //LCD_ClearLine(LINE(4));/* 清除单行文字 */

	fs.drv=0;
	res=f_mount(&fs,"0:",1);
	ILI9341_DispStringLine_EN(LINE(1),"TEST SD");
	Fat_init(res);
	LCD_ClearLine(LINE(1));
	ILI9341_DispStringLine_EN(LINE(1),"SD OK");
	
	
	
	
	Get_Dev_Infor();
	//初始化文件
	sprintf(name_budder,"%s","ADC_VCC_10HZ.csv");
	sprintf(excel_top_name,"%s","ADC\r\n");
	Creat_Excel(name_budder,excel_top_name);
	//git_test
	
	
	while(1)
	{
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
//				if(++tra_j>MAX_FILE_SIZE)
//				{
//					tra_j=0;
//					Creat_Excel();					
//				}
			}
		}
			
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
static FRESULT Creat_Excel(char *name_budder,char *excel_top_name)
{
	 UINT br; 
	 //sprintf(name_budder,"0:ADC统计信息%d.csv",name_i++);
	 res_flash = f_open(&fnew,name_budder,FA_WRITE|FA_CREATE_ALWAYS);
	 res_flash = f_write(&fnew,excel_top_name,sizeof(excel_top_name),&br);
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




