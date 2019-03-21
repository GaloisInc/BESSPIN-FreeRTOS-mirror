#include <stddef.h> // maybe not needed
#include <stdint.h>
#include <string.h> // for `memset`
#include "uart.h"

#include "FreeRTOS.h"
#include "semphr.h"

/* Xilinx driver includes. */
#include "xuartns550.h"
#include "bsp.h"
#include "plic_driver.h"


#define UART_TX_DELAY pdMS_TO_TICKS(500)
#define UART_BUFFER_SIZE	64

/* Instance of UART device */
static XUartNs550 UartNs550_0;

struct UartDriver {
  XUartNs550 Device; /* Xilinx Uart driver */
  uint8_t SendBuffer[UART_BUFFER_SIZE];	/* Buffer for Transmitting Data */
  uint8_t RecvBuffer[UART_BUFFER_SIZE];	/* Buffer for Receiving Data */
  /* Counters used to determine when buffer has been send and received */
  volatile int TotalReceivedCount;
  volatile int TotalSentCount;
  volatile int TotalErrorCount;
  volatile uint8_t Errors;
  int tx_len;
  SemaphoreHandle_t mutex;
  TaskHandle_t tx_task; /* handle for task that called TX */
};


static int uart_txbuffer(struct UartDriver* Uart, char *ptr, int len);
static void uart_init(struct UartDriver* Uart, uint8_t device_id, uint8_t plic_source_id);

static void UartNs550StatusHandler(void *CallBackRef, u32 Event, 
	unsigned int EventData);


struct UartDriver Uart0;

void uart0_init(void) {
  uart_init(&Uart0, XPAR_UARTNS550_0_DEVICE_ID, PLIC_SOURCE_UART0);
}


static void uart_init(struct UartDriver* Uart, uint8_t device_id, uint8_t plic_source_id)
{
  // Initialize struct
  Uart->mutex = xSemaphoreCreateMutex();
  Uart->Device = UartNs550_0;
  Uart->tx_task = NULL;

  /* Initialize the UartNs550 driver so that it's ready to use */
	configASSERT( XUartNs550_Initialize(&Uart->Device, device_id) == XST_SUCCESS);

  /* Setup interrupt system */
  configASSERT( PLIC_register_interrupt_handler(&Plic, plic_source_id,
    (XInterruptHandler)XUartNs550_InterruptHandler, Uart) != 0);

  /* Set UART status handler to indicate that UartNs550StatusHandler
	should be called when there is an interrupt */
	XUartNs550_SetHandler(&Uart->Device, UartNs550StatusHandler, Uart);

  /* Enable interrupts, enable loopback mode, and enable FIFOs */
	uint16_t Options = XUN_OPTION_DATA_INTR | XUN_OPTION_LOOPBACK |
			XUN_OPTION_FIFOS_ENABLE | XUN_FIFO_TX_RESET | XUN_FIFO_RX_RESET;
	XUartNs550_SetOptions(&Uart->Device, Options);

  /* Zero transmit and receive buffer */
  memset(Uart->SendBuffer, 0, UART_BUFFER_SIZE);
  memset(Uart->RecvBuffer, 0, UART_BUFFER_SIZE);
}


int uart0_rxready(void)
{
  return 0;
}


int uart0_rxchar(void)
{
  return 0;
}


static int uart_txbuffer(struct UartDriver* Uart, char *ptr, int len) {
  Uart->tx_task = xTaskGetCurrentTaskHandle();
  Uart->tx_len = len;
  /* Send buffer */
	XUartNs550_Send(&Uart->Device, (uint8_t*)ptr, len);
  /* wait for notification */
	if (xTaskNotifyWait( 0, 0, NULL, UART_TX_DELAY )) {
    /* TX succesfull, return number of transmitted bytes */
    return Uart->TotalSentCount;
  } else {
    /* timeout occured */
    return -1;
  }
}

int uart0_txbuffer(char *ptr, int len) {
  int cnt;
  configASSERT( Uart0.mutex != NULL);
  configASSERT( xSemaphoreTake(Uart0.mutex, portMAX_DELAY) == pdTRUE);
  cnt = uart_txbuffer(&Uart0, ptr, len);
  xSemaphoreGive( Uart0.mutex );
  return cnt;
}

int uart0_txchar(int c)
{
  return c;
}

/* Wait for transmitter shift register/FIFO to empty */
void uart0_flush(void)
{
}


static void UartNs550StatusHandler(void *CallBackRef, u32 Event, 
	unsigned int EventData)
{
  struct UartDriver* Uart = (struct UartDriver*)CallBackRef;

	/* All of the data was sent */
	if (Event == XUN_EVENT_SENT_DATA) {
		Uart->TotalSentCount = EventData;
    if (Uart->tx_len == Uart->TotalSentCount) {
      // data was sent, notify the task
      Uart->tx_len = 0;
      configASSERT( Uart->tx_task != NULL);
	    static BaseType_t askForContextSwitch = pdFALSE;
	    vTaskNotifyGiveFromISR( Uart->tx_task, &askForContextSwitch);
    }
	}

	/* All of the data was received */
	if (Event == XUN_EVENT_RECV_DATA) {
		Uart->TotalReceivedCount = EventData;
	}

	/* Data was received, but different than expected number of bytes */
	if (Event == XUN_EVENT_RECV_TIMEOUT) {
		Uart->TotalReceivedCount = EventData;
	}

	/* Data was received with an error */
	if (Event == XUN_EVENT_RECV_ERROR) {
		Uart->TotalReceivedCount = EventData;
		Uart->TotalErrorCount++;
		Uart->Errors = XUartNs550_GetLastErrors(&Uart->Device);
	}
}