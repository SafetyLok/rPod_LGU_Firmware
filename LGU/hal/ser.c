#include "ser.h"
#include <sci.h>
#include <sys_vim.h>
#include <sys_core.h>

volatile ser_t serport[2];

// volatile FIFO variables must be read and modified atomically so SCI interrupts must be disabled when application thread calls FIFO related functions
//#define SCI_ENABLEINTS(x)  do { (x)->SETINT   = SCI_RX_INT; (x)->SETINT   = SCI_TX_INT; } while (0)
//#define SCI_DISABLEINTS(x) do { (x)->CLEARINT = SCI_RX_INT; (x)->CLEARINT = SCI_TX_INT; } while (0)
//#define SCI_ENABLEINTS(x)  do { (x)->SETINT   = SCI_RX_INT | SCI_TX_INT; } while (0)
//#define SCI_DISABLEINTS(x) do { (x)->CLEARINT = SCI_RX_INT | SCI_TX_INT; } while (0)
//#define SCI_ENABLEINTS(x)  do { (x)->SETINT = SCI_RX_INT | SCI_TX_INT; _enable_interrupt_(); } while (0)
//#define SCI_DISABLEINTS(x) do { _disable_interrupt_(); } while (0)
#define SCI_ENABLEINTS(x)  do { _enable_IRQ(); } while (0)
#define SCI_DISABLEINTS(x) do { _disable_IRQ(); } while (0)

static void ser_onTx(sciBASE_t*);
static void ser_onRx(sciBASE_t*);

void ser_initn(ser_t* ser)
{
    // initializes the FIFO
    SCI_DISABLEINTS(ser->scibase);
    ser->fifo_rx_wrt = 0;
    ser->fifo_rx_rd = 0;
    ser->fifo_tx_wrt = 0;
    ser->fifo_tx_rd = 0;
    ser->fifo_tx_busy = 0;
    sciEnableNotification(ser->scibase, SCI_RX_INT | SCI_TX_INT);
    SCI_ENABLEINTS(ser->scibase);
    ser->scibase->SETINT = SCI_RX_INT;
}

void ser_init()
{
	sciInit(); // TODO: is this being called anywhere else?
	vimEnableInterrupt(74, SYS_IRQ);
	vimEnableInterrupt(27, SYS_IRQ);
    serport[0].scibase = sciREG;
    ser_initn((ser_t*)&serport[0]);
    serport[1].scibase = scilinREG;
    ser_initn((ser_t*)&serport[1]);
}

int16_t ser_read(ser_t* ser)
{
    int16_t ret;
    SCI_DISABLEINTS(ser->scibase);
    if (ser->fifo_rx_wrt == ser->fifo_rx_rd) {
        // nothing in fifo
        ret = -1;
    }
    else {
        // something in FIFO, pop it out
        ret = ser->fifo_rx[ser->fifo_rx_rd];
        ser->fifo_rx_rd += 1;
        ser->fifo_rx_rd %= SER_FIFO_RX_SIZE;
    }
    SCI_ENABLEINTS(ser->scibase);
    return ret;
}

void ser_tx(ser_t* ser, uint8_t x)
{
	unsigned int n;
    SCI_DISABLEINTS(ser->scibase);
    // check if there is room in FIFO
    n = ser->fifo_tx_wrt;
    n += 1;
    n %= SER_FIFO_TX_SIZE;
    if (n != ser->fifo_tx_rd)
    {
        // there is room in the FIFO
        // push into FIFO
    	ser->fifo_tx_wrt = ser->fifo_tx_wrt >= SER_FIFO_TX_SIZE ? 0 : ser->fifo_tx_wrt;
        ser->fifo_tx[ser->fifo_tx_wrt] = x;
        ser->fifo_tx_wrt += 1;
        ser->fifo_tx_wrt %= SER_FIFO_TX_SIZE;
        ser->fifo_tx_wrt = ser->fifo_tx_wrt >= SER_FIFO_TX_SIZE ? 0 : ser->fifo_tx_wrt;
        // just pushing into the FIFO doesn't actually do anything with SCI
        // we check if the SCI is actually doing anything
        if (ser->fifo_tx_busy == 0)
        {
            // if it not doing anything
            // then we pop off what we just pushed ...
            ser->fifo_tx_rd += 1;
            ser->fifo_tx_rd %= SER_FIFO_TX_SIZE;
            ser->fifo_tx_busy = 1;
            ser->tx_byte = x;
            // ... and we force the SCI to send out the byte
            ser->scibase->TD = ser->tx_byte;
            ser->scibase->SETINT = SCI_TX_INT;
        }
    }
    SCI_ENABLEINTS(ser->scibase);
}

/*
most likely serial port will only be utilized for some form of human interaction terminal
but perhaps when that is not possible, we have an emulated pipe
the function prototype of this function will not be exposed
*/
void ser_fakeRx(ser_t* ser, uint8_t x)
{
	unsigned int n;
    SCI_DISABLEINTS(ser->scibase);
    // check size and ensure we have enough space in FIFO
    n = ser->fifo_rx_wrt;
    n += 1;
    n %= SER_FIFO_RX_SIZE;
    if (n != ser->fifo_rx_rd)
    {
        // space is available in FIFO
        // perform a FIFO push
    	ser->fifo_rx_wrt = ser->fifo_rx_wrt >= SER_FIFO_RX_SIZE ? 0 : ser->fifo_rx_wrt;
        ser->fifo_rx[ser->fifo_rx_wrt] = x;
        ser->fifo_rx_wrt += 1;
        ser->fifo_rx_wrt %= SER_FIFO_RX_SIZE;
        ser->fifo_rx_wrt = ser->fifo_rx_wrt >= SER_FIFO_RX_SIZE ? 0 : ser->fifo_rx_wrt;
    }
    SCI_ENABLEINTS(ser->scibase);
}

static void ser_onTx(sciBASE_t* sci)
{
    uint8 x;
    ser_t* ser = &serport[(sci == sciREG) ? 0 : 1];
    if (ser->fifo_tx_wrt == ser->fifo_tx_rd) {
        // there is nothing left to send, so we are now not busy
        ser->fifo_tx_busy = 0;
        ser->scibase->CLEARINT = (uint32)SCI_TX_INT;
    }
    else {
        // we still have something left to send
        // pop it out of the FIFO
        x = ser->fifo_tx[ser->fifo_tx_rd];
        ser->fifo_tx_rd += 1;
        ser->fifo_tx_rd %= SER_FIFO_TX_SIZE;
        ser->fifo_tx_busy = 1; // enforce the fact that we are busy
        ser->tx_byte = x;
        // force the SCI to send out another byte
        ser->scibase->TD = ser->tx_byte;
        ser->scibase->SETINT = (uint32)SCI_TX_INT;
    }
}

static void ser_onRx(sciBASE_t* sci)
{
    unsigned int n;
    ser_t* ser = &serport[(sci == sciREG) ? 0 : 1];
    ser->rx_byte = ser->scibase->RD & 0xFF;
    // check size and ensure we have enough space in FIFO
    n = ser->fifo_rx_wrt;
	n += 1;
	n %= SER_FIFO_RX_SIZE;
    if (n != ser->fifo_rx_rd)
    {
        // space is available in FIFO
        // perform a FIFO push
    	ser->fifo_rx_wrt = ser->fifo_rx_wrt >= SER_FIFO_RX_SIZE ? 0 : ser->fifo_rx_wrt;
        ser->fifo_rx[ser->fifo_rx_wrt] = ser->rx_byte;
        ser->fifo_rx_wrt += 1;
		ser->fifo_rx_wrt %= SER_FIFO_RX_SIZE;
		ser->fifo_rx_wrt = ser->fifo_rx_wrt >= SER_FIFO_RX_SIZE ? 0 : ser->fifo_rx_wrt;
    }
    SCI_ENABLEINTS(ser->scibase);
}

// this is a callback that is called from the SCI driver inside the SCI interrupt handler
void sciNotification(sciBASE_t *sci, uint32_t flags)
{
    if (flags == SCI_TX_INT) {
        ser_onTx(sci);
    }
    else if (flags == SCI_RX_INT) {
        ser_onRx(sci);
    }
    // ignore all other types of notifications
}
