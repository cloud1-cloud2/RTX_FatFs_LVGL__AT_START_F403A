#include "file_system.h"

void creat_file_tree(void);

void file_system_init()
{
	FATFS *fs;     /* Ponter to the filesystem object */
  FRESULT res;   /* Result code */
	DIR dir;       /* Directory object */

//  FIL fil;        /* File object */
//  char line[100]; /* Line buffer */
//  UINT bw; /* Bytes written */
//  UINT br; /* Bytes read */

	if ((fs = malloc(sizeof (FATFS))) == NULL)
  {
    printf("malloc failed!\r\n");
    while(1);
  }

//	uint8_t work[FF_MAX_SS];
//  MKFS_PARM mkfs_parm;
//  mkfs_parm.fmt = FM_FAT | FM_SFD; // Format as FAT with SFD
//  mkfs_parm.n_fat = 1; // Number of FATs
//  mkfs_parm.align = 8; // Data area alignment (unit: sector)
//  mkfs_parm.n_root = 0; // default value (0) and any invaid value gives 512.
//  mkfs_parm.au_size = 4096; // Cluster size (byte)
//  if ((res = f_mkfs("",&mkfs_parm,work,sizeof(work))) != FR_OK)
//  {
//    printf("f_mkfs failed!,%d\r\n", res);
//    while(1);
//  }
	
	if ((res = f_mount(fs, "0:", 1)) != FR_OK)
  {
    printf("f_mount failed!,%d\r\n", res);
    while(1);
  }
	

  // /* Creat a text file */
  // if ((res = f_open(&fil, "0:/test.txt", FA_CREATE_ALWAYS | FA_WRITE | FA_READ)) != FR_OK)
  // {
  //   printf("f_open test.txt failed!,%d\r\n", res);
  //   while(1);
  // }

  // /* Write some text to the file */
  // sprintf(line, "Hello, FatFs!\r\n");
  // if ((res = f_write(&fil, line, strlen(line)+1, &bw)) != FR_OK)
  // {
  //   printf("f_write test.txt failed!,%d\r\n", res);
  //   f_close(&fil);
  //   while(1);
  // }

  // /*Read content from test.txt*/
  // if ((res = f_lseek(&fil, 0)) != FR_OK) // Move to the beginning of the file
  // {
  //   printf("f_lseek test.txt failed!,%d\r\n", res);
  //   f_close(&fil);
  //   while(1);
  // }

  // if ((res = f_read(&fil, line, 16, &br)) != FR_OK)
  // {
  //   printf("f_read test.txt failed!,%d\r\n", res);
  //   f_close(&fil);
  //   while(1);
  // }

  // printf("Read from test.txt: %s\r\n", line);

  // /* Close the file */
  // if ((res = f_close(&fil)) != FR_OK)
  // {
  //   printf("f_close test.txt failed!,%d\r\n", res);
  //   while(1);
  // }

  free(fs);

  // creat_file_tree();
}

void creat_file_tree(void)
{
  FIL file;       /* File object */
  FRESULT res;    /* Result code */
  UINT bytesWritten;
  const char *text = "";
  
  if ((res = f_mkdir("0:/folder1")) != FR_OK)
  {
    printf("f_mkdir folder1 failed!,%d\r\n", res);
    while(1);
  }
  
  if ((res = f_mkdir("0:/folder2")) != FR_OK)
  {
    printf("f_mkdir folder2 failed!,%d\r\n", res);
    while(1);
  }
  
  
  if ((res = f_open(&file, "0:/folder1/file1.txt", FA_CREATE_ALWAYS | FA_WRITE)) != FR_OK)
  {
    printf("f_open file1.txt failed!,%d\r\n", res);
    while(1);
  }

  //写一些文本
  text = "This is file1.txt in folder1.\r\n";
  if ((res = f_write(&file, text, strlen(text), &bytesWritten)) != FR_OK)
  {
    printf("f_write file1.txt failed!,%d\r\n", res);
    while(1);
  }

  f_close(&file);
  
  if ((res = f_open(&file, "0:/folder1/file2.txt", FA_CREATE_ALWAYS | FA_WRITE)) != FR_OK)
  {
    printf("f_open file2.txt failed!,%d\r\n", res);
    while(1);
  }

  //写一些文本
  text = "This is file2.txt in folder1.\r\n";
  if ((res = f_write(&file, text, strlen(text), &bytesWritten)) != FR_OK)
  {
    printf("f_write file2.txt failed!,%d\r\n", res);
    while(1);
  }

  f_close(&file);
  
  if ((res = f_open(&file, "0:/folder2/file3.txt", FA_CREATE_ALWAYS | FA_WRITE)) != FR_OK)
  {
    printf("f_open file3.txt failed!,%d\r\n", res);
    while(1);
  }

  //写一些文本
  text = "This is file3.txt in folder2.\r\n";
  if ((res = f_write(&file, text, strlen(text), &bytesWritten)) != FR_OK)
  {
    printf("f_write file3.txt failed!,%d\r\n", res);
    while(1);
  }

  f_close(&file);
}

