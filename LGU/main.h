#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdint.h>

#include <lgu_commands.h>

enum
{
	CMDSRC_PICOMM,
	CMDSRC_CANBUS,
	CMDSRC_SERTERM,
};

extern void manlift(uint8_t src, int motor, signed int speed, unsigned int time);

void stop_all(void);
void stop_allLifts(void);
void stop_allAuxprop(void);

#endif
