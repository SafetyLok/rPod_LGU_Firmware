#include "ser.h"
#include "term.h"

#define TERM_SERPORT_IDX 1

int16_t term_hal_read(void)
{
    return (int16_t)ser_read(&serport[TERM_SERPORT_IDX]);
}

void term_hal_tx(char x)
{
    ser_tx(&serport[TERM_SERPORT_IDX], (uint8_t)x);
}
