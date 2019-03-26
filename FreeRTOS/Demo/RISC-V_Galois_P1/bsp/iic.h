#ifndef __IIC_H__
#define __IIC_H__

#include "bsp.h"

#if BSP_USE_IIC0
    void iic0_init(void);
    void iic0_acquire(void);
    void iic0_release(void);
    void iic0_transmit(void);
    void iic0_receive(void);
    void iic0_reset(void);
#endif

#if BSP_USE_IIC1
    void iic1_init(void);
    void iic1_acquire(void);
    void iic1_release(void);
    void iic1_transmit(void);
    void iic1_receive(void);
    void iic1_reset(void);
#endif

#endif
