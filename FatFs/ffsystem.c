/*------------------------------------------------------------------------*/
/* A Sample Code of User Provided OS Dependent Functions for FatFs        */
/*------------------------------------------------------------------------*/

#include "ff.h"

#if FF_USE_LFN == 3	/* Use dynamic memory allocation */
#include "stdio.h"
#include "cmsis_os2.h"

// 为FatFS创建专用的内存池
#define FATFS_MEM_BLOCK_SIZE  1120
#define FATFS_MEM_BLOCK_COUNT 10

static osMemoryPoolId_t fatfs_mempool = NULL;

void* ff_memalloc(UINT msize)
{
    //printf("[DEBUG] ff_memalloc: %u bytes\n", msize);
    
    // 第一次使用时初始化内存池
    if (fatfs_mempool == NULL) {
        osMemoryPoolAttr_t mempool_attr = {
            .name = "FatFS_MemPool",
            .attr_bits = 0,
            .cb_mem = NULL,
            .cb_size = 0,
            .mp_mem = NULL,
            .mp_size = 0
        };
        fatfs_mempool = osMemoryPoolNew(FATFS_MEM_BLOCK_COUNT, 
                                       FATFS_MEM_BLOCK_SIZE, 
                                       &mempool_attr);
        
        if (fatfs_mempool == NULL) {
            printf("[ERROR] Failed to create FatFS memory pool\n");
            return NULL;
        }
    }
    
    // 检查请求大小是否合适
    if (msize > FATFS_MEM_BLOCK_SIZE) {
        printf("[ERROR] Requested size %u exceeds block size %u\n", 
               msize, FATFS_MEM_BLOCK_SIZE);
        return NULL;
    }
    
    // 从内存池分配
    void* ptr = osMemoryPoolAlloc(fatfs_mempool, 0); // 0表示无限等待
    //printf("[DEBUG] Allocation result: %p\n", ptr);
    
    return ptr;
}

void ff_memfree(void* mblock)
{
    //printf("[DEBUG] ff_memfree: %p\n", mblock);
    
    if (mblock != NULL && fatfs_mempool != NULL) {
        osMemoryPoolFree(fatfs_mempool, mblock);
    }
}

#endif

#if FF_FS_REENTRANT	/* Mutal exclusion */
/*------------------------------------------------------------------------*/
/* Definitions of Mutex                                                   */
/*------------------------------------------------------------------------*/

#define OS_TYPE	5	/* 0:Win32, 1:uITRON4.0, 2:uC/OS-II, 3:FreeRTOS, 4:CMSIS-RTOS, 5:CMSIS-RTOS2 */


#if   OS_TYPE == 0	/* Win32 */
#include <windows.h>
static HANDLE Mutex[FF_VOLUMES + 1];	/* Table of mutex handle */

#elif OS_TYPE == 1	/* uITRON */
#include "itron.h"
#include "kernel.h"
static mtxid Mutex[FF_VOLUMES + 1];		/* Table of mutex ID */

#elif OS_TYPE == 2	/* uc/OS-II */
#include "includes.h"
static OS_EVENT *Mutex[FF_VOLUMES + 1];	/* Table of mutex pinter */

#elif OS_TYPE == 3	/* FreeRTOS */
#include "FreeRTOS.h"
#include "semphr.h"
static SemaphoreHandle_t Mutex[FF_VOLUMES + 1];	/* Table of mutex handle */

#elif OS_TYPE == 4	/* CMSIS-RTOS */
#include "cmsis_os.h"
static osMutexId Mutex[FF_VOLUMES + 1];	/* Table of mutex ID */

#elif OS_TYPE == 5	/* CMSIS-RTOS2 */
#include "cmsis_os2.h"
static osMutexId_t Mutex[FF_VOLUMES + 1];	/* Table of mutex ID */

#endif



/*------------------------------------------------------------------------*/
/* Create a Mutex                                                         */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount function to create a new mutex
/  or semaphore for the volume. When a 0 is returned, the f_mount function
/  fails with FR_INT_ERR.
*/

const osMutexAttr_t Fatfs016_Mutex_attr = {
  NULL,     // human readable mutex name
  osMutexPrioInherit,     // attr_bits
  NULL,                // memory for control block   
  0U                   // size for control block
};	

int ff_mutex_create (	/* Returns 1:Function succeeded or 0:Could not create the mutex */
	int vol				/* Mutex ID: Volume mutex (0 to FF_VOLUMES - 1) or system mutex (FF_VOLUMES) */
)
{
#if OS_TYPE == 0	/* Win32 */
	Mutex[vol] = CreateMutex(NULL, FALSE, NULL);
	return (int)(Mutex[vol] != INVALID_HANDLE_VALUE);

#elif OS_TYPE == 1	/* uITRON */
	T_CMTX cmtx = {TA_TPRI,1};

	Mutex[vol] = acre_mtx(&cmtx);
	return (int)(Mutex[vol] > 0);

#elif OS_TYPE == 2	/* uC/OS-II */
	OS_ERR err;

	Mutex[vol] = OSMutexCreate(0, &err);
	return (int)(err == OS_NO_ERR);

#elif OS_TYPE == 3	/* FreeRTOS */
	Mutex[vol] = xSemaphoreCreateMutex();
	return (int)(Mutex[vol] != NULL);

#elif OS_TYPE == 4	/* CMSIS-RTOS */
	osMutexDef(cmsis_os_mutex);

	Mutex[vol] = osMutexCreate(osMutex(cmsis_os_mutex));
	return (int)(Mutex[vol] != NULL);
	
#elif OS_TYPE == 5	/* CMSIS-RTOS2 */
	Mutex[vol] = osMutexNew(&Fatfs016_Mutex_attr);
	return (int)(Mutex[vol] != NULL);

#endif
}


/*------------------------------------------------------------------------*/
/* Delete a Mutex                                                         */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount function to delete a mutex or
/  semaphore of the volume created with ff_mutex_create function.
*/

void ff_mutex_delete (	/* Returns 1:Function succeeded or 0:Could not delete due to an error */
	int vol				/* Mutex ID: Volume mutex (0 to FF_VOLUMES - 1) or system mutex (FF_VOLUMES) */
)
{
#if OS_TYPE == 0	/* Win32 */
	CloseHandle(Mutex[vol]);

#elif OS_TYPE == 1	/* uITRON */
	del_mtx(Mutex[vol]);

#elif OS_TYPE == 2	/* uC/OS-II */
	OS_ERR err;

	OSMutexDel(Mutex[vol], OS_DEL_ALWAYS, &err);

#elif OS_TYPE == 3	/* FreeRTOS */
	vSemaphoreDelete(Mutex[vol]);

#elif OS_TYPE == 4	/* CMSIS-RTOS */
	osMutexDelete(Mutex[vol]);

#elif OS_TYPE == 5	/* CMSIS-RTOS2 */
	osMutexDelete(Mutex[vol]);

#endif
}


/*------------------------------------------------------------------------*/
/* Request a Grant to Access the Volume                                   */
/*------------------------------------------------------------------------*/
/* This function is called on enter file functions to lock the volume.
/  When a 0 is returned, the file function fails with FR_TIMEOUT.
*/

int ff_mutex_take (	/* Returns 1:Succeeded or 0:Timeout */
	int vol			/* Mutex ID: Volume mutex (0 to FF_VOLUMES - 1) or system mutex (FF_VOLUMES) */
)
{
#if OS_TYPE == 0	/* Win32 */
	return (int)(WaitForSingleObject(Mutex[vol], FF_FS_TIMEOUT) == WAIT_OBJECT_0);

#elif OS_TYPE == 1	/* uITRON */
	return (int)(tloc_mtx(Mutex[vol], FF_FS_TIMEOUT) == E_OK);

#elif OS_TYPE == 2	/* uC/OS-II */
	OS_ERR err;

	OSMutexPend(Mutex[vol], FF_FS_TIMEOUT, &err));
	return (int)(err == OS_NO_ERR);

#elif OS_TYPE == 3	/* FreeRTOS */
	return (int)(xSemaphoreTake(Mutex[vol], FF_FS_TIMEOUT) == pdTRUE);

#elif OS_TYPE == 4	/* CMSIS-RTOS */
	return (int)(osMutexWait(Mutex[vol], FF_FS_TIMEOUT) == osOK);

#elif OS_TYPE == 5	/* CMSIS-RTOS2 */
	return (int)(osMutexAcquire(Mutex[vol], FF_FS_TIMEOUT) == osOK);

#endif
}



/*------------------------------------------------------------------------*/
/* Release a Grant to Access the Volume                                   */
/*------------------------------------------------------------------------*/
/* This function is called on leave file functions to unlock the volume.
*/

void ff_mutex_give (
	int vol			/* Mutex ID: Volume mutex (0 to FF_VOLUMES - 1) or system mutex (FF_VOLUMES) */
)
{
#if OS_TYPE == 0	/* Win32 */
	ReleaseMutex(Mutex[vol]);

#elif OS_TYPE == 1	/* uITRON */
	unl_mtx(Mutex[vol]);

#elif OS_TYPE == 2	/* uC/OS-II */
	OSMutexPost(Mutex[vol]);

#elif OS_TYPE == 3	/* FreeRTOS */
	xSemaphoreGive(Mutex[vol]);

#elif OS_TYPE == 4	/* CMSIS-RTOS */
	osMutexRelease(Mutex[vol]);

#elif OS_TYPE == 5	/* CMSIS-RTOS2 */
	osMutexRelease(Mutex[vol]);

#endif
}

#endif	/* FF_FS_REENTRANT */

