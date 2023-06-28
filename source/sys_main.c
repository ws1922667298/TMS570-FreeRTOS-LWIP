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
#include "gio.h"

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
/* USER CODE END */
#include "lwip/opt.h"
#include <lwip/sockets.h>
#include <lwiperf.h>
#include "lwip/sys.h"

#if LWIP_SOCKET    //需要开启Scoket才能使用

#define RECV_DATA            (1024UL)
#define LOCAL_PORT            (6133UL)
#define BACKLOG            (5UL)/*最大监听数*/

#define LWIP_TCP_DEBUG_ENABLE    1

#if LWIP_TCP_DEBUG_ENABLE
#define LWIP_TCP_DEBUG    printf
#else
#define LWIP_TCP_DEBUG(...)
#endif


static void tcp_server_thread(void *arg) {

    int sockfd = -1, connected; /*socket句柄和建立连接后的句柄*/
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_size;
    int recv_data_len;
    uint8_t recv_data[RECV_DATA];

    LWIP_TCP_DEBUG("tcp server port %d\n\n", LOCAL_PORT);

    again:

    //创建scoket描述符 AF_INET 使用ipv4地址
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        LWIP_TCP_DEBUG("Socket error\n");
        close(sockfd);
        vTaskDelay(100);
        goto again;
    }
    //
    server_addr.sin_family = AF_INET;                //该属性表示接收本机或其他机器传输
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);    //本机IP
    server_addr.sin_port = htons(LOCAL_PORT);            //端口号
    memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr)) == -1) {
        LWIP_TCP_DEBUG("Unable to bind\n");
        close(sockfd);
        vTaskDelay(100);
        goto again;
    }

    //设置最大监听数
    if (listen(sockfd, BACKLOG) == -1) {
        LWIP_TCP_DEBUG("Listen error\n");
        close(sockfd);
        vTaskDelay(100);
        goto again;
    }

    while (1) {
        sin_size = sizeof(struct sockaddr_in);

        //在这里阻塞知道接收到消息，参数分别是socket句柄，接收到的地址信息以及大小
        connected = accept(sockfd, (struct sockaddr *) &client_addr, &sin_size);

        LWIP_TCP_DEBUG("new client connected from (%s, %d)\n", inet_ntoa(client_addr.sin_addr),
                       ntohs(client_addr.sin_port));

        int tcp_nodelay = 1;//don't delay send to coalesce packets
        setsockopt(connected, IPPROTO_TCP, TCP_NODELAY, (void *) &tcp_nodelay, sizeof(int));

        while (1) {
            recv_data_len = recv(connected, recv_data, RECV_DATA, 0);

            if (recv_data_len <= 0) {
                break;
            }


            LWIP_TCP_DEBUG("recv %d len data\n", recv_data_len);
            //发送内容
            write(connected, recv_data, recv_data_len);
        }

        if (connected >= 0) {
            close(connected);
        }

        connected = -1;
    }

    if (sockfd >= 0) {
        close(sockfd);
    }

}

#endif

void tcp_server_init(void) {
#if LWIP_SOCKET
    sys_thread_new("tcpecho_thread", tcp_server_thread, NULL, 4096, 5);
#endif
}

void IntMasterIRQEnable(void);


#define PORT_NUMBER 5001
#define BUFFER_SIZE 1024

void tcp_server_task(void *pvParameters) {
    int server_fd, new_socket;
    struct sockaddr_in server_addr, client_addr;
    int addrlen = sizeof(server_addr);
    char buffer[BUFFER_SIZE] = {0};

    // create TCP socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("socket creation failed\n");
        vTaskDelete(NULL);
    }

    // bind socket to a specific IP address and port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT_NUMBER);

    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        printf("bind failed\n");
        vTaskDelete(NULL);
    }

    // start listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        printf("listen failed\n");
        vTaskDelete(NULL);
    }

    while (1) {
        // accept incoming connection
        if ((new_socket = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &addrlen)) < 0) {
            printf("accept failed\n");
            continue;
        }

        // receive data from client
        int bytes_read = recv(new_socket, buffer, BUFFER_SIZE, 0);

        // send response back to client
        send(new_socket, buffer, bytes_read, 0);

        // close the socket
        close(new_socket);
    }
}

void perfTask(void *parm){
    lwiperf_start_tcp_server_default(NULL, NULL);
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(void) {
/* USER CODE BEGIN (3) */
    sciInit();
    EMAC_LwIP_Main(emacAddress);
//    tcp_server_init();
    xTaskCreate(perfTask, "perfTask", 4096, NULL, 1, NULL);

    IntMasterIRQEnable();
    _enable_FIQ();
//    xTaskCreate(monitorTask, "monitorTask", 2048, NULL, 1, NULL);
    vTaskStartScheduler();
/* USER CODE END */

    return 0;
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    printf("-----------stack_over_flow:%s", pcTaskName);
}

/* USER CODE BEGIN (4) */
int fputs(const char *_ptr, FILE *_fp) {
    unsigned int i, len;
    len = strlen(_ptr);
    for (i = 0; i < len; i++) {
        while ((scilinREG->FLR & (uint32) SCI_TX_INT) == 0U);
        scilinREG->TD = (unsigned char) _ptr[i];
    }
    return len;
}

int fputc(int ch, FILE *f) {
    while ((scilinREG->FLR & (uint32) SCI_TX_INT) == 0U);//循环发送,直到发送完毕
    scilinREG->TD = (uint8) ch;
    return ch;
}

