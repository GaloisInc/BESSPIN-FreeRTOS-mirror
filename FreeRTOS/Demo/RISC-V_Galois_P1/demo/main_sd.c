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



void sd_demo(void);
//#include "SDLib.h"
/**
 * This tasks controls GPIO and the connected motors
 */
static void prvSdTestTask0(void *pvParameters)
{
    (void)pvParameters;
    // const char *filename = "01.log";

    // const char * entry =
    //     "hello "
    //     "test01xxxxaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbccccccccccccccccdddddddddddd"
    //     "ddddeeeeeeeeeeeeeeeeffffffffffffffffgggggggggggggggghhhhhhhhhhhhhhhhii"
    //     "iiiiiiiiiiiiiijjjjjjjjjjjjjjjjkkkkkkkkkkkkkkkkllllllllllllllllmmmmmmmm"
    //     "mmmmmmmmnnnnnnnnnnnnnnnnooooooooooooooo"; // 256 chars including final \0

    // uint8_t buf[512] = {0};
    sd_demo();
    // Enter endless loop to be consistent with other tests
    // size_t r;
    // uint16_t cnt = 1;
    for (;;)
    {   
        // r = sdlib_write_to_file(filename, (const uint8_t*)entry, strlen(entry));
        // printf("#%u: written %u bytes\r\n", cnt, r);
        // vTaskDelay(pdMS_TO_TICKS(100));
        // r = sdlib_read_from_file(filename, buf, sizeof(buf));
        // printf("#%u: read %u bytes\r\n", cnt, r);
        // cnt++;
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
