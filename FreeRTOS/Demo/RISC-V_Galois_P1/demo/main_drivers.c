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
//#include "task.h"
//#include "queue.h"

/* xillinx driver include */
#include "iic.h"
//#include "plic_driver.h"
//#include "bsp.h"

/*-----------------------------------------------------------*/

void main_drivers( void );

static void prvIicTestTask( void *pvParameters );

/*-----------------------------------------------------------*/

void main_drivers( void )
{
	xTaskCreate( prvIicTestTask, "Driver_test_task", configMINIMAL_STACK_SIZE * 2U, NULL, tskIDLE_PRIORITY + 1, NULL );

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	for( ;; );
}
/*-----------------------------------------------------------*/


static void prvIicTestTask( void *pvParameters ) {
	printf("Starting drivers task\n");
	uint8_t addr = 0x18;
	uint8_t tx_data[] = {1,2,3};
	uint8_t tx_len = 3;

	for (;;) {
		printf("Sending data\r\n");
		int res = iic0_transmit(addr, tx_data, tx_len);
		printf("res = %u\r\n", res);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}
