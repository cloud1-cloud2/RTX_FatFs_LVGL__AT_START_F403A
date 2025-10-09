/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2025        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Basic definitions of FatFs */
#include "diskio.h"		/* Declarations FatFs MAI */

/* Example: Declarations of the platform and disk functions in the project */
#include "flash_en25qh128a.h"

/* Example: Mapping of physical drive number for each drive */
#define DEV_FLASH	0	/* Map FTL to physical drive 0 */
#define DEV_MMC		1	/* Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Map USB MSD to physical drive 2 */
#define DEV_RAM		3	/* Map RAM disk to physical drive 3 */


static DSTATUS Stat = STA_NOINIT;	/* Disk status */

int FLASH_disk_status(void);
int Flash_disk_initialize(void);
int Flash_disk_read(BYTE* buff, LBA_t sector, UINT count);
int Flash_disk_write(const BYTE* buff, LBA_t sector, UINT count);
int Flash_disk_ioctl(BYTE cmd, void* buff);

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		//result = RAM_disk_status();

		// translate the reslut code here

		return stat;

	case DEV_MMC :
		//result = MMC_disk_status();

		// translate the reslut code here

		return stat;

	case DEV_USB :
		//result = USB_disk_status();

		// translate the reslut code here

		return stat;

	case DEV_FLASH :
		result = FLASH_disk_status();
		switch (result)
		{
		case 0 : 					// STA_NOINIT Flag is not set, disk is initialized
			stat &= ~STA_NOINIT; 	// Clear STA_NOINIT Flag
			break;
		case 1 : 					// STA_NOINIT Flag is set, disk is not initialized	
			stat |= STA_NOINIT; 	// Set STA_NOINIT Flag
			break;
		default:					// result is 0 or 1
			break;
		}
		return stat;
	}

	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		//result = RAM_disk_initialize();

		// translate the reslut code here

		return stat;

	case DEV_MMC :
		//result = MMC_disk_initialize();

		// translate the reslut code here

		return stat;

	case DEV_USB :
		//result = USB_disk_initialize();

		// translate the reslut code here

		return stat;
	
	case DEV_FLASH :
		result = Flash_disk_initialize();
		switch (result)
		{
		case 0 :
			stat &= ~STA_NOINIT; 	// Clear STA_NOINIT Flag
			break;
		
		default:
			break;
		}
		return stat;
	}
	
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

		//result = RAM_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_MMC :
		// translate the arguments here

		//result = MMC_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_USB :
		// translate the arguments here

		//result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_FLASH :
		result = Flash_disk_read(buff, sector, count);
		switch (result)
		{		
		case 0 : 					// Read successful
			res = RES_OK;
			break;
		case 1 : 					// Invalid count
			res = RES_PARERR;
			break;
		case 2 : 					// Read failed
			res = RES_ERROR;
			break;
		}
		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

		//result = RAM_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_MMC :
		// translate the arguments here

		//result = MMC_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_USB :
		// translate the arguments here

		//result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;
	
	case DEV_FLASH :
		result = Flash_disk_write(buff, sector, count);
		switch (result)
		{
		case 0 : 					// Write successful
			res = RES_OK;
			break;
		case 1 : 					// Erase or page write failed
			res = RES_ERROR;
			break;
		}
		return res;
	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_RAM :

		// Process of the command for the RAM drive

		return res;

	case DEV_MMC :

		// Process of the command for the MMC/SD card

		return res;

	case DEV_USB :

		// Process of the command the USB drive

		return res;
	
	case DEV_FLASH :
		result = Flash_disk_ioctl(cmd, buff);
		switch (result)
		{
		case 0 : 					// Success
			res = RES_OK;
			break;
		case 1 : 					// Unsupported command
			res = RES_PARERR;
			break;
		}
		return res;
	}

	return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* User defined functions                                                */

int FLASH_disk_status(void)
{
	if(Stat & STA_NOINIT)
	{
		return 1; 	// STA_NOINIT Flag is set, disk is not initialized
	}

	return 0; 		// STA_NOINIT Flag is not set, disk is initialized
}

int Flash_disk_initialize(void)
{
	Stat &= ~STA_NOINIT; 	// Clear STA_NOINIT Flag
	return 0;       // Success
}

int Flash_disk_read(BYTE* buff, LBA_t sector, UINT count)
{
	if (count == 0) return 1; // Invalid count

	// Translate sector and count to address and length
	uint32_t addr = sector * EN25QH_SECTOR_SIZE; 
	uint32_t len = count * EN25QH_SECTOR_SIZE;

	// Read data from SPI Flash
	if (!en25qh_read_data(addr, buff, len)) {
		return 2; // Read failed
	}

	return 0; // Read successful
}

int Flash_disk_write(const BYTE* buff, LBA_t sector, UINT count)
{
	if(en25qh_write_data((uint8_t*)buff, sector, count))
	{
		return 0; // Write successful
	}else{
		return 1; // Erase or page write failed
	}
}

int Flash_disk_ioctl(BYTE cmd, void* buff)
{
	switch (cmd) {
		case CTRL_SYNC:
			return 0; // Success

		case GET_SECTOR_COUNT:
			*(LBA_t*)buff = EN25QH_SECTOR_COUNT; // 4096 sectors
			return 0; // Success

		case GET_SECTOR_SIZE:
			*(WORD*)buff = EN25QH_SECTOR_SIZE; // Sector size is 4096 bytes
			return 0; // Success

		case GET_BLOCK_SIZE:
			*(DWORD*)buff = EN25QH_SECTOR_PER_BLOCK; // Block size is 8 sectors
			return 0; // Success

		default:
			return 1; // Unsupported command
	}
}

