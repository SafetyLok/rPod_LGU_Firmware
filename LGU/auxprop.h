#ifndef _AUXPROP_H_
#define _AUXPROP_H_

#include <stdint.h>
#include <systmr.h>

enum
{
	AUXPROPSTATE_IDLE,
	AUXPROPSTATE_GO_UNLIMITED,
	AUXPROPSTATE_GO_TIMED,
	AUXPROPSTATE_GO_SINGLE_UNLIMITED,
	AUXPROPSTATE_GO_SINGLE_TIMED,
};

typedef struct
{
	char state;
	systmr_t stop_time;
	char clutch;
	char enable;
	signed int speed;
}
auxprop_state_t;

extern auxprop_state_t auxprop_state;

void auxprop_init(void);
void auxprop_clutch(char clutch);
void auxprop_enable(char enable);
void auxprop_moveSpeedDist(signed int speed, unsigned int dist);
void auxprop_moveSpeedDistSingle(char which, signed int speed, unsigned int dist);
void auxprop_task(void);
void auxprop_report(uint8_t);

#endif
