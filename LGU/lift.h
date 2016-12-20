#ifndef _LIFT_H_
#define _LIFT_H_

#include <stdint.h>

void lift_task(void);
void lift_report(uint8_t src, int m);
void lift_brake(char x);
void lift_cancelAll(void);

#endif
