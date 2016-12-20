#ifndef _SER_H_
#define _SER_H_

#include <sci.h>
#include <stdint.h>

#define SER_FIFO_RX_SIZE ((unsigned int)1024)
#define SER_FIFO_TX_SIZE ((unsigned int)1024)

typedef struct
{
    sciBASE_t* scibase;
    volatile uint8_t rx_byte;
    volatile uint8_t tx_byte;
    volatile unsigned int fifo_rx_wrt;
    volatile unsigned int fifo_rx_rd;
    volatile unsigned int fifo_tx_wrt;
    volatile unsigned int fifo_tx_rd;
    volatile char fifo_tx_busy;
    volatile uint8_t fifo_rx[SER_FIFO_RX_SIZE];
    volatile uint8_t fifo_tx[SER_FIFO_TX_SIZE];
}
ser_t;

extern volatile ser_t serport[2];

void ser_init(void);
int16_t ser_read(ser_t*);
void ser_tx(ser_t*, uint8_t);

#endif
