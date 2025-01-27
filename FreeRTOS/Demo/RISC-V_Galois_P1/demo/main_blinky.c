/*
 * FreeRTOS Kernel V10.1.1
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/******************************************************************************
 * NOTE 1:  This project provides two demo applications.  A simple blinky
 * style project, and a more comprehensive test and demo application.  The
 * mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting in main.c is used to select
 * between the two.  See the notes on using mainCREATE_SIMPLE_BLINKY_DEMO_ONLY
 * in main.c.  This file implements the simply blinky style version.
 *
 * NOTE 2:  This file only contains the source code that is specific to the
 * basic demo.  Generic functions, such FreeRTOS hook functions, and functions
 * required to configure the hardware are defined in main.c.
 ******************************************************************************
 *
 * main_blinky() creates one queue, and two tasks.  It then starts the
 * scheduler.
 *
 * The Queue Send Task:
 * The queue send task is implemented by the prvQueueSendTask() function in
 * this file.  prvQueueSendTask() sits in a loop that causes it to repeatedly
 * block for 1000 milliseconds, before sending the value 100 to the queue that
 * was created within main_blinky().  Once the value is sent, the task loops
 * back around to block for another 1000 milliseconds...and so on.
 *
 * The Queue Receive Task:
 * The queue receive task is implemented by the prvQueueReceiveTask() function
 * in this file.  prvQueueReceiveTask() sits in a loop where it repeatedly
 * blocks on attempts to read data from the queue that was created within
 * main_blinky().  When data is received, the task checks the value of the
 * data, and if the value equals the expected 100, writes 'Blink' to the UART
 * (the UART is used in place of the LED to allow easy execution in QEMU).  The
 * 'block time' parameter passed to the queue receive function specifies that
 * the task should be held in the Blocked state indefinitely to wait for data to
 * be available on the queue.  The queue receive task will only leave the
 * Blocked state when the queue send task writes to the queue.  As the queue
 * send task writes to the queue every 1000 milliseconds, the queue receive
 * task leaves the Blocked state every 1000 milliseconds, and therefore toggles
 * the LED every 200 milliseconds.
 */

/* Standard includes. */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "uart.h"
#include "iic.h"

/* Priorities used by the tasks. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY (tskIDLE_PRIORITY + 2)
#define mainQUEUE_SEND_TASK_PRIORITY (tskIDLE_PRIORITY + 1)

/* The rate at which data is sent to the queue.  The 200ms value is converted
to ticks using the pdMS_TO_TICKS() macro. */
#define mainQUEUE_SEND_FREQUENCY_MS pdMS_TO_TICKS(1000)

/* The maximum number items the queue can hold.  The priority of the receiving
task is above the priority of the sending task, so the receiving task will
preempt the sending task and remove the queue items each time the sending task
writes to the queue.  Therefore the queue will never have more than one item in
it at any time, and even with a queue length of 1, the sending task will never
find the queue full. */
#define mainQUEUE_LENGTH (1)

/*-----------------------------------------------------------*/

/*
 * Called by main when mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is set to 1 in
 * main.c.
 */
void main_blinky(void);

#ifdef BIN_SOURCE_LMCO
/**
 * LMCO smoketest
 * from: https://gitlab-ext.galois.com/ssith/ta1-lmco/-/blob/master/default_source/smoke002/smoke002.c
 */
int malware(void);
#endif /* BIN_SOURCE_LMCO */

/*
 * The tasks as described in the comments at the top of this file.
 */
static void prvQueueReceiveTask(void *pvParameters);
static void prvQueueSendTask(void *pvParameters);

/*-----------------------------------------------------------*/

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

/*-----------------------------------------------------------*/

void main_blinky(void)
{
	/* Create the queue. */
	xQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(uint32_t));
	configASSERT(xQueue != NULL);

	/* Start the two tasks as described in the comments at the top of this
		file. */
	xTaskCreate(prvQueueReceiveTask,			 /* The function that implements the task. */
				"Rx",							 /* The text name assigned to the task - for debug only as it is not used by the kernel. */
				configMINIMAL_STACK_SIZE * 2U,   /* The size of the stack to allocate to the task. */
				NULL,							 /* The parameter passed to the task - not used in this case. */
				mainQUEUE_RECEIVE_TASK_PRIORITY, /* The priority assigned to the task. */
				NULL);							 /* The task handle is not required, so NULL is passed. */

	xTaskCreate(prvQueueSendTask, "TX", configMINIMAL_STACK_SIZE * 2U, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL);
}
/*-----------------------------------------------------------*/

static void prvQueueSendTask(void *pvParameters)
{
	TickType_t xNextWakeTime;
	const unsigned int ulValueToSend = 100UL;
	BaseType_t xReturned;

	unsigned int cnt = 0;

	/* Remove compiler warning about unused parameter. */
	(void)pvParameters;

	/* Initialise xNextWakeTime - this only needs to be done once. */
	xNextWakeTime = xTaskGetTickCount();

	for (;;)
	{
		printf("[%u]: Hello from TX\r\n", cnt);
		cnt++;

		/* Place this task in the blocked state until it is time to run again. */
		vTaskDelayUntil(&xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS);

		printf("[%u] TX: awoken\r\n", cnt);

		/* Send to the queue - causing the queue receive task to unblock and
		toggle the LED.  0 is used as the block time so the sending operation
		will not block - it shouldn't need to block as the queue should always
		be empty at this point in the code. */
		xReturned = xQueueSend(xQueue, &ulValueToSend, 0U);
		configASSERT(xReturned == pdPASS);
		printf("[%u] TX: sent\r\n", cnt);
	}
}
/*-----------------------------------------------------------*/

static void prvQueueReceiveTask(void *pvParameters)
{
	unsigned int ulReceivedValue;
	const unsigned int ulExpectedValue = 100UL;

	unsigned int cnt = 0;

	/* Remove compiler warning about unused parameter. */
	(void)pvParameters;

	for (;;)
	{
		printf("[%u]: Hello from RX\r\n", cnt);
		cnt++;

		/* Wait until something arrives in the queue - this task will block
		indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
		FreeRTOSConfig.h. */
		xQueueReceive(xQueue, &ulReceivedValue, portMAX_DELAY);

		printf("[%u] RX: received value\r\n", cnt);

		/*  To get here something must have been received from the queue, but
		is it the expected value?  If it is, toggle the LED. */
		if (ulReceivedValue == ulExpectedValue)
		{
			printf("Blink !!!\r\n");
			ulReceivedValue = 0U;
		}
		else
		{
			printf("Unexpected value received\r\n");
		}

		#ifdef BIN_SOURCE_LMCO
			if (cnt==3) {
				printf("Lanching malware sequence!\r\n");
				malware();
				printf("Malware sequence finished, we didn't catch it\r\n");
			}
		#endif /* BIN_SOURCE_LMCO */
	}
}
/*-----------------------------------------------------------*/

#ifdef BIN_SOURCE_LMCO
int malware()
{
	int temporary;
	int i;

	// benign filler
	for (i = 0; i < 10; i++)
	{
		temporary = i;
	}

	// fault smoke pattern
	//	values DO NOT increase		-> FAULT
	//	values less than 0x90			-> no warning
	{
		__asm__("slti zero,zero,0x10");
	}
	{
		__asm__("slti zero,zero,0x20");
	}
	{
		__asm__("slti zero,zero,0x03");
	}
	{
		__asm__("slti zero,zero,0x40");
	}

	// benign filler
	for (i = 10; i > 0; i--)
	{
		temporary = i;
	}

	return temporary;
}
#endif /* BIN_SOURCE_LMCO */
