#ifndef _SYSTMR_H_
#define _SYSTMR_H_

#include <stdint.h>

typedef int32_t systmr_t;

void systmr_init(void);
systmr_t systmr_nowMillis(void);
char systmr_check10msFlag(void);
void systmr_clear10msFlag(void);

#endif
