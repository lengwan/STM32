/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/
#include <string.h>
#include "diskio.h"
#include "stm32f10x.h"
#include "mysd.h"
#include "bsp_rtc.h"
extern struct rtc_time systmtime;
/* Ϊÿ���豸����һ�������� */
#define ATA			           0     // SD��

#define SD_BLOCKSIZE     512 

extern  SD_CardInfo SDCardInfo;

/*-----------------------------------------------------------------------*/
/* ��ȡ�豸״̬                                                          */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status (
	BYTE pdrv		/* ������ */
)
{
	DSTATUS status = STA_NOINIT;
	
	switch (pdrv) {
		case ATA:	/* SD CARD */
			status &= ~STA_NOINIT;
			break;

		default:
			status = STA_NOINIT;
	}
	return status;
}

/*-----------------------------------------------------------------------*/
/* �豸��ʼ��                                                            */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize (
	BYTE pdrv				/* ������ */
)
{
	DSTATUS status = STA_NOINIT;	
	switch (pdrv) {
		case ATA:	         /* SD CARD */
			if(SD_Init()==SD_OK)
			{
				status &= ~STA_NOINIT;
			}
			else 
			{
				status = STA_NOINIT;
			}
		
			break;
      
		default:
			status = STA_NOINIT;
	}
	return status;
}


/*-----------------------------------------------------------------------*/
/* ����������ȡ�������ݵ�ָ���洢��                                              */
/*-----------------------------------------------------------------------*/
DRESULT disk_read (
	BYTE pdrv,		/* �豸������(0..) */
	BYTE *buff,		/* ���ݻ����� */
	DWORD sector,	/* �����׵�ַ */
	UINT count		/* ��������(1..128) */
)
{
	DRESULT status = RES_PARERR;
	SD_Error SD_state = SD_OK;
	
	switch (pdrv) {
		case ATA:	/* SD CARD */						
			SD_state=SD_ReadDisk(buff,sector,count);
		  if(SD_state==SD_OK)
			{
				/* Check if the Transfer is finished */
				SD_state=SD_WaitReadOperation();
				while(SD_GetStatus() != SD_TRANSFER_OK);
			}
			if(SD_state!=SD_OK)
				status = RES_PARERR;
		  else
			  status = RES_OK;	
			break;   
    
		default:
			status = RES_PARERR;
	}
	return status;
}



/*-----------------------------------------------------------------------*/
/* д������������д��ָ�������ռ���                                      */
/*-----------------------------------------------------------------------*/
#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			  /* �豸������(0..) */
	const BYTE *buff,	/* ��д�����ݵĻ����� */
	DWORD sector,		  /* �����׵�ַ */
	UINT count			  /* ��������(1..128) */
)
{
	DRESULT status = RES_PARERR;
	SD_Error SD_state = SD_OK;
	
	if (!count) {
		return RES_PARERR;		/* Check parameter */
	}

	switch (pdrv) {
		case ATA:	/* SD CARD */  
			SD_state=SD_WriteDisk((u8 *)buff,sector,count);
			if(SD_state==SD_OK)
			{
				/* Check if the Transfer is finished */
				SD_state=SD_WaitWriteOperation();

				/* Wait until end of DMA transfer */
				while(SD_GetStatus() != SD_TRANSFER_OK);			
			}
			if(SD_state!=SD_OK)
				status = RES_PARERR;
		  else
			  status = RES_OK;	
		break;

    
		default:
			status = RES_PARERR;
	}
	return status;
}
#endif


/*-----------------------------------------------------------------------*/
/* ��������                                                              */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* ������ */
	BYTE cmd,		  /* ����ָ�� */
	void *buff		/* д����߶�ȡ���ݵ�ַָ�� */
)
{
	DRESULT status = RES_PARERR;
	switch (pdrv) {
		case ATA:	/* SD CARD */
			switch (cmd) 
			{
				// Get R/W sector size (WORD) 
				case GET_SECTOR_SIZE :    
					*(WORD * )buff = SD_BLOCKSIZE;
				break;
				// Get erase block size in unit of sector (DWORD)
				case GET_BLOCK_SIZE :      
					*(DWORD * )buff = 1;
				break;

				case GET_SECTOR_COUNT:
					*(DWORD * )buff = SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize;
					break;
				case CTRL_SYNC :
				break;
			}
			status = RES_OK;
			break;

		default:
			status = RES_PARERR;
	}
	return status;
}
#endif

							 
__weak DWORD get_fattime(void) {
	/* ���ص�ǰʱ��� */
	 /*  �ѱ�׼ʱ��ת��Ϊ����ʱ��*/
		 uint32_t BJ_TimeVar;
	   BJ_TimeVar =RTC_GetCounter() + TIME_ZOOM;

	   to_tm(BJ_TimeVar, &systmtime);/*�Ѷ�ʱ����ֵת��Ϊ����ʱ��*/	
	return	  ((DWORD)(systmtime.tm_year - 1980) << 25)	/* Year 2015 */
			| ((DWORD)systmtime.tm_mon << 21)				/* Month 1 */
			| ((DWORD)systmtime.tm_mday << 16)				/* Mday 1 */
			| ((DWORD)systmtime.tm_hour << 11)				/* Hour 0 */
			| ((DWORD)systmtime.tm_min<< 5)				  /* Min 0 */
			| ((DWORD)systmtime.tm_sec >> 1);				/* Sec 0 */
}

