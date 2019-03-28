#ifndef __IIC_H__
#define __IIC_H__

#include "bsp.h"

#if BSP_USE_IIC0
    void iic0_init(void);
    int iic0_transmit(uint8_t addr, uint8_t *tx_data, uint8_t tx_len);
    int iic0_receive(uint8_t addr, uint8_t *rx_data, uint8_t rx_len);
#endif

#if BSP_USE_IIC1
    void iic1_init(void);
    int iic1_transmit(uint8_t addr, uint8_t *tx_data, uint8_t tx_len);
    int iic1_receive(uint8_t addr, uint8_t *rx_data, uint8_t rx_len);
#endif

#endif
