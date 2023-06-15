/** @file sys_main.c 
*   @brief Application main file
*   @date 11-Dec-2018
*   @version 04.07.01
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com 
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */
/* USER CODE END */

/* Include Files */

#include <arch/sys_arch.h>
#include <stdio.h>
#include <lwip/api.h>
#include "sys_common.h"
#include "FreeRTOS.h"
#include "os_task.h"
#include "sci.h"
#include "string.h"

/* USER CODE BEGIN (1) */
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
extern void EMAC_LwIP_Main(uint8_t *emacAddress);

/* USER CODE END */

uint8 emacAddress[6U] = {0x00U, 0x08U, 0xEEU, 0x03U, 0xA6U, 0x6CU};
uint32 emacPhyAddress = 1U;

void lwipTestTask(void *args) {
    EMAC_LwIP_Main(emacAddress);
}

void monitorTask(void *arg) {
    char pWriteBuffer[2048];
    while (1) {
        sys_msleep(5000);
        vTaskList((char *) &pWriteBuffer);
        printf("task_name   task_state  priority   stack  tasK_num\n");
        printf("%s\n", pWriteBuffer);
    }
    vTaskDelete(NULL);
}

int main(void) {
/* USER CODE BEGIN (3) */
    sciInit();
    EMAC_LwIP_Main(emacAddress);
//    xTaskCreate(monitorTask, "monitorTask", 4096, NULL, 1, NULL);
    vTaskStartScheduler();
/* USER CODE END */

    return 0;
}

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName ) {
    printf("-----------stack_over_flow:%s", pcTaskName);
}

/* USER CODE BEGIN (4) */
int fputs(const char *_ptr, FILE *_fp)
{
    unsigned int i, len;
    len = strlen(_ptr);
    for(i=0 ; i<len ; i++)
    {
        while ((scilinREG->FLR & (uint32)SCI_TX_INT) == 0U);
        scilinREG->TD = (unsigned char) _ptr[i];
    }
    return len;
}
int fputc(int ch, FILE *f)
{
    while ((scilinREG->FLR & (uint32)SCI_TX_INT) == 0U);//循环发送,直到发送完毕
    scilinREG->TD = (uint8) ch;
    return ch;
}
/* USER CODE END */
