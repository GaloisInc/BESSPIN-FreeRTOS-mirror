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

/* Standard includes. */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* Kernel includes. */
#include "FreeRTOS.h"

// Drivers
#include "uart.h"
#include "gpio.h"

// Devices
#include "vcnl4010.h"
#include "serLcd.h"

/*-----------------------------------------------------------*/

void main_drivers(void);

static void prvIicTestTask0(void *pvParameters);
static void prvIicTestTask1(void *pvParameters);
static void prvUartTxTestTask(void *pvParameters);
static void prvUartRx0TestTask(void *pvParameters);
static void prvUartRx1TestTask(void *pvParameters);
static void prvLcdTestTask(void *pvParameters);

/*-----------------------------------------------------------*/

void main_drivers(void)
{
	xTaskCreate(prvIicTestTask0, "prvIicTestTask0", configMINIMAL_STACK_SIZE * 2U, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(prvIicTestTask1, "prvIicTestTask1", configMINIMAL_STACK_SIZE * 2U, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(prvUartTxTestTask, "prvUartTxTestTask", configMINIMAL_STACK_SIZE * 2U, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(prvUartRx0TestTask, "prvUartRx0TestTask", configMINIMAL_STACK_SIZE * 2U, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(prvUartRx1TestTask, "prvUartRx1TestTask", configMINIMAL_STACK_SIZE * 2U, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(prvLcdTestTask, "prvLcdTestTask", configMINIMAL_STACK_SIZE * 2U, NULL, tskIDLE_PRIORITY + 1, NULL);

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	for (;;)
		;
}
/*-----------------------------------------------------------*/

static void prvLcdTestTask(void *pvParameters)
{
	(void)pvParameters;
	int cnt = 0;
	char str[16];

	for (;;)
	{
		sprintf(str, "Counter: %d\r\n", cnt);
		serLcdPrintf(str);
		cnt++;
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

static void prvIicTestTask1(void *pvParameters)
{
	(void)pvParameters;
	struct Vcnl4010_t sensor1;
	uint16_t proximity = 0;
	uint16_t ambient_light = 0;
	printf("#1 Starting drivers task\n");
	configASSERT(vcnl4010_init(&sensor1, &Iic1));

	for (;;)
	{
		proximity = vcnl4010_readProximity(&sensor1);
		printf("#1 Proximity: %u\r\n", proximity);

		ambient_light = vcnl4010_readAmbient(&sensor1);
		printf("#1 ambient: %u\r\n", ambient_light);

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

static void prvIicTestTask0(void *pvParameters)
{
	(void)pvParameters;
	struct Vcnl4010_t sensor0;
	uint16_t proximity = 0;
	uint16_t ambient_light = 0;
	printf("#0 Starting drivers task\n");
	configASSERT(vcnl4010_init(&sensor0, &Iic0));

	for (;;)
	{
		proximity = vcnl4010_readProximity(&sensor0);
		printf("#0 Proximity: %u\r\n", proximity);

		ambient_light = vcnl4010_readAmbient(&sensor0);
		printf("#0 ambient: %u\r\n", ambient_light);

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

static void prvUartTxTestTask(void *pvParameters)
{
	(void)pvParameters;
	int cnt = 0;
	char str[12];

	for (;;)
	{
		gpio1_clear(0);
		gpio1_clear(1);
		sprintf(str, "%d\r\n", cnt);
		int len = uart1_txbuffer(str, strlen(str));
		if (len == -1)
		{
			printf("Timeout\r\n");
		}
		cnt++;
		vTaskDelay(pdMS_TO_TICKS(1000));
		gpio1_write(0);
		gpio1_write(1);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

static void prvUartRx0TestTask(void *pvParameters)
{
	(void)pvParameters;
	int cnt = 0;
	char str[12];

	for (;;)
	{
		gpio2_clear(1);
		gpio2_clear(2);
		int len = uart0_rxbuffer(str, 1);
		if (len == -1)
		{
			gpio2_write(1);
		}
		else
		{
			gpio2_write(2);
		}
		cnt++;
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

static void prvUartRx1TestTask(void *pvParameters)
{
	(void)pvParameters;
	int cnt = 0;
	char str[12];

	for (;;)
	{
		gpio2_clear(3);
		gpio2_clear(4);
		int len = uart1_rxbuffer(str, 1);
		if (len == -1)
		{
			gpio2_write(3);
		}
		else
		{
			gpio2_write(4);
		}
		cnt++;
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}