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
#include "task.h"
#include "bsp.h"

void main_sd(void);
void print_array(char *name, uint8_t *buf, size_t len);

static void prvSdTestTask0(void *pvParameters);

void print_array(char *name, uint8_t *buf, size_t len)
{
    printf("%s = [\r\n", name);
    for (size_t i = 0; i < len; i++)
    {
        printf("0x%X, ", buf[i]);
        if (i % 10 == 0)
        {
            printf("\r\n");
        }
    }
    printf("];\r\n");
}

void main_sd(void)
{
    xTaskCreate(prvSdTestTask0, "prvSdTestTask0", configMINIMAL_STACK_SIZE * 3U, NULL, tskIDLE_PRIORITY + 1, NULL);
}


#include "SDLib.h"
uint8_t sd_test_buf[2048] = {0};
/**
 * This tasks controls GPIO and the connected motors
 */
static void prvSdTestTask0(void *pvParameters)
{
    (void)pvParameters;
    const char *log1 = "test0.log";
    const char *log2 = "test1.log";

    const char * entry1 =
        "hello "
        "test01xxxxaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbccccccccccccccccdddddddddddd"
        "ddddeeeeeeeeeeeeeeeeffffffffffffffffgggggggggggggggghhhhhhhhhhhhhhhhii"
        "iiiiiiiiiiiiiijjjjjjjjjjjjjjjjkkkkkkkkkkkkkkkkllllllllllllllllmmmmmmmm"
        "mmmmmmmmnnnnnnnnnnnnnnnnooooooooooooooo"; // 256 chars including final \0

    const char * entry2 =
        "hello "
        "test01xxxxaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbccccccccccccccccdddddddddddd"
        "ddddeeeeeeeeeeeeeeeeffffffffffffffffgggggggggggggggghhhhhhhhhhhhhhhhii"
        "iiiiiiiiiiiiiijjjjjjjjjjjjjjjjkkkkkkkkkkkkkkkkllllllllllllllllmmmmmmmm"
        "mmmmmmmmnnnnnnnnnnnnnnnnooooooooooooooo"; // 256 chars including final \0

    configASSERT(sdlib_initialize());

    size_t r;
    uint16_t cnt = 1;
    for (;;)
    {   
        r = sdlib_write_to_file(log1, (const uint8_t*)entry1, strlen(entry1));
        printf("#%u: %s written %u bytes\r\n", cnt, log1, r);
        r = sdlib_write_to_file(log2, (const uint8_t*)entry2, strlen(entry2));
        printf("#%u: %s written %u bytes\r\n", cnt, log2, r);
        r = sdlib_read_from_file(log1, sd_test_buf, sizeof(sd_test_buf));
        printf("#%u: %s read %u bytes\r\n", cnt, log1, r);
        r = sdlib_read_from_file(log2, sd_test_buf, sizeof(sd_test_buf));
        printf("#%u: %s read %u bytes\r\n", cnt, log2, r);
        cnt++;
        // Include small delay so we can show stats
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
