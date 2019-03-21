
#ifndef __UART_16550_H__
#define __UART_16550_H__

int uart0_rxready(void);
int uart0_rxchar(void);
int uart0_txchar(int c);
int uart0_txbuffer(char *ptr, int len);
void uart0_flush(void);
void uart0_init(void);

#endif
