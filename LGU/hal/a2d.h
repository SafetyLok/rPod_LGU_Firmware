#ifndef _A2D_H_
#define _A2D_H_

#include <stdint.h>

#define PIN2ADCCHAN(x) (uint8_t)((x) & 0x3F)

extern volatile uint32_t adc_conversionCount;

void a2d_init(void);
void a2d_task(void);
uint16_t a2d_getRaw(uint8_t chan);
char a2d_checkNew(uint8_t chan);
float a2d_getFiltered(uint8_t chan);

#endif
