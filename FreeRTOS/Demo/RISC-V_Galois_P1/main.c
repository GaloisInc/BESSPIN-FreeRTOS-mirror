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

/**
 * This is docs for main
 */

/* FreeRTOS kernel includes. */
#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>

/* Bsp includes. */
#include "bsp.h"

/******************************************************************************
 * This project provides test applications for Galois P1 SSITH processor.
 */
#if mainDEMO_TYPE == 1
#pragma message "Demo type 1: Basic Blinky"
extern void main_blinky(void);
#elif mainDEMO_TYPE == 2
#pragma message "Demo type 2: Full FreeRTOS API"
extern void main_full(void);
#elif mainDEMO_TYPE == 3
#pragma message "Demo type 3: IIC test"
extern void main_iic(void);
#elif mainDEMO_TYPE == 4
#pragma message "Demo type 4: GPIO test"
extern void main_gpio(void);
#elif mainDEMO_TYPE == 5
#pragma message "Demo type 5: TCP/UDP test"
extern void main_tcp(void);
#elif mainDEMO_TYPE == 6
#pragma message "Demo type 6: SD card test"
extern void main_sd(void);
#elif mainDEMO_TYPE == 7
#pragma message "Demo type 7: UART test"
extern void main_uart(void);
#elif mainDEMO_TYPE == 9
#pragma message "Demo type 9: HTTP peek/poke test"
extern void main_peekpoke(void);
#elif mainDEMO_TYPE == 10
#pragma message "Demo type 10: RTC test"
extern void main_rtc(void);
#elif mainDEMO_TYPE == 12
#undef configGENERATE_RUN_TIME_STATS
#pragma message "Demo type 12: FETT"
extern void main_fett(void);
#elif mainDEMO_TYPE == 13
#undef configGENERATE_RUN_TIME_STATS
#pragma message "Demo type 13: Netboot"
extern void main_netboot(void);

#else
#error "Unsupported demo type"
#endif

/* Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file.  See https://www.freertos.org/a00016.html */
void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
void vApplicationTickHook(void);

/* Returns the current value of mcycle register*/
uint64_t get_cycle_count(void);

#if configGENERATE_RUN_TIME_STATS
/* Buffer and a task for displaying runtime stats */
char statsBuffer[4096];
static void prvStatsTask(void *pvParameters);
#endif /* configGENERATE_RUN_TIME_STATS */

#if USE_LED_BLINK_TASK
#include "gpio.h"
#define MAIN_LED_DELAY_MS 100
static void vTestLED(void *pvParameters);
#endif
/*-----------------------------------------------------------*/

#if __riscv_xlen == 64
#define read_csr(reg) ({ unsigned long __tmp; \
  asm volatile ("csrr %0, " #reg : "=r"(__tmp)); \
  __tmp; })
#endif

/**
 * Capture the current 64-bit cycle count.
 */
uint64_t get_cycle_count(void)
{
#if __riscv_xlen == 64
	return read_csr(mcycle);
#else
	uint32_t cycle_lo, cycle_hi;
	asm volatile(
		"%=:\n\t"
		"csrr %1, mcycleh\n\t"
		"csrr %0, mcycle\n\t"
		"csrr t1, mcycleh\n\t"
		"bne  %1, t1, %=b"
		: "=r"(cycle_lo), "=r"(cycle_hi)
		: // No inputs.
		: "t1");
	return (((uint64_t)cycle_hi) << 32) | (uint64_t)cycle_lo;
#endif
}

/**
 * Use `mcycle` counter to get usec resolution.
 * On RV32 only, reads of the mcycle CSR return the low 32 bits,
 * while reads of the mcycleh CSR return bits 63–32 of the corresponding
 * counter.
 * We convert the 64-bit read into usec. The counter overflows in roughly an hour
 * and 20 minutes. Probably not a big issue though.
 * At 50HMz clock rate, 1 us = 50 ticks
 */
uint32_t port_get_current_mtime(void)
{
	return (uint32_t)(get_cycle_count() / (configCPU_CLOCK_HZ / 1000000));
}

/**
 * Main application entry
 */
int main(void)
{
	prvSetupHardware();

#if mainDEMO_TYPE == 1
	{
		main_blinky();
	}
#elif mainDEMO_TYPE == 2
	{
		main_full();
	}
#elif mainDEMO_TYPE == 3
	{
		main_iic();
	}
#elif mainDEMO_TYPE == 4
	{
		main_gpio();
	}
#elif mainDEMO_TYPE == 5
	{
		main_tcp();
	}
#elif mainDEMO_TYPE == 6
	{
		main_sd();
	}
#elif mainDEMO_TYPE == 7
	{
		main_uart();
	}
#elif mainDEMO_TYPE == 9
	{
		main_peekpoke();
	}
#elif mainDEMO_TYPE == 10
	{
		main_rtc();
	}
#elif mainDEMO_TYPE == 12
	{
		main_fett();
	}
#elif mainDEMO_TYPE == 13
	{
		main_netboot();
	}
#endif

#if configGENERATE_RUN_TIME_STATS
	xTaskCreate(prvStatsTask, "prvStatsTask", configMINIMAL_STACK_SIZE * 20, NULL, tskIDLE_PRIORITY, NULL);
#endif

#if USE_LED_BLINK_TASK
	xTaskCreate(vTestLED, "LED Test", 1000, NULL, 0, NULL);
#endif

	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the Idle and/or
	timer tasks to be created.  See the memory management section on the
	FreeRTOS web site for more details on the FreeRTOS heap
	http://www.freertos.org/a00111.html. */
	vTaskStartScheduler();
	for (;;)
		;
}
/*-----------------------------------------------------------*/

#if configGENERATE_RUN_TIME_STATS

// Returns percentage utilization of the ISR stack
#include "portmacro.h"
extern const StackType_t xISRStackTop;
extern const uint32_t _stack_end[];
const StackType_t xISRStackEnd = ( StackType_t ) _stack_end;

static uint8_t prvIsrStackUtilization(void)
{
	uint8_t percent = 0;
	uint32_t idx;
	uint32_t stack_len = (xISRStackTop - xISRStackEnd)/4; // # words
	uint32_t *stack_ptr = (uint32_t*) xISRStackEnd;

	for (idx=0; idx<stack_len; idx++) {
		if (*stack_ptr != 0xabababab) {
			break;
		}
		stack_ptr++;
	}

	percent = 100 - idx*100/stack_len;

	return percent;
}

static void prvStatsTask(void *pvParameters)
{
	(void)pvParameters;
	printf(("prvStatsTask: starting\r\n"));
	vTaskDelay(pdMS_TO_TICKS(1000));

	for (;;)
	{
		vTaskGetRunTimeStats(statsBuffer);
		printf("prvStatsTask: xPortGetFreeHeapSize() = %u\r\n", xPortGetFreeHeapSize());
		printf("prvStatsTask: prvIsrStackUtilization() = %u\r\n", prvIsrStackUtilization());
		printf("prvStatsTask: Run-time stats\r\nTask\t\tAbsTime\t\t%%time\tStackHighWaterMark\r\n");
		printf("%s\r\n", statsBuffer);
		vTaskDelay(pdMS_TO_TICKS(10000));
	}
}
#endif /* configGENERATE_RUN_TIME_STATS */

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook(void)
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	__asm volatile("ebreak");
	for (;;)
		;
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook(void)
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
	(void)pcTaskName;
	(void)pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	__asm volatile("ebreak");
	for (;;)
		;
}
/*-----------------------------------------------------------*/

void vApplicationTickHook(void)
{
/* The tests in the full demo expect some interaction with interrupts. */
#if (mainDEMO_TYPE == 2)
	{
		extern void vFullDemoTickHook(void);
		vFullDemoTickHook();
	}
#endif
}

#if USE_LED_BLINK_TASK
void vTestLED(void *pvParameters)
{
    (void)pvParameters;

    printf("vTestLED starting\r\n");

    for(;;)
    {
        /* Write to every LED */
        led_write(0);
        vTaskDelay(pdMS_TO_TICKS(MAIN_LED_DELAY_MS));
        led_write(1);
        vTaskDelay(pdMS_TO_TICKS(MAIN_LED_DELAY_MS));
        led_write(2);
        vTaskDelay(pdMS_TO_TICKS(MAIN_LED_DELAY_MS));
        led_write(3);
        vTaskDelay(pdMS_TO_TICKS(MAIN_LED_DELAY_MS));
        led_write(4);
        vTaskDelay(pdMS_TO_TICKS(MAIN_LED_DELAY_MS));
        led_write(5);
        vTaskDelay(pdMS_TO_TICKS(MAIN_LED_DELAY_MS));
        led_write(6);
        vTaskDelay(pdMS_TO_TICKS(MAIN_LED_DELAY_MS));
        led_write(7);
        vTaskDelay(pdMS_TO_TICKS(MAIN_LED_DELAY_MS));

        /* Clear every LED */
        led_clear(0);
        vTaskDelay(pdMS_TO_TICKS(MAIN_LED_DELAY_MS));
        led_clear(1);
        vTaskDelay(pdMS_TO_TICKS(MAIN_LED_DELAY_MS));
        led_clear(2);
        vTaskDelay(pdMS_TO_TICKS(MAIN_LED_DELAY_MS));
        led_clear(3);
        vTaskDelay(pdMS_TO_TICKS(MAIN_LED_DELAY_MS));
        led_clear(4);
        vTaskDelay(pdMS_TO_TICKS(MAIN_LED_DELAY_MS));
        led_clear(5);
        vTaskDelay(pdMS_TO_TICKS(MAIN_LED_DELAY_MS));
        led_clear(6);
        vTaskDelay(pdMS_TO_TICKS(MAIN_LED_DELAY_MS));
        led_clear(7);
        vTaskDelay(pdMS_TO_TICKS(MAIN_LED_DELAY_MS));
    }
}
#endif /* USE_LED_BLINK_TASK */