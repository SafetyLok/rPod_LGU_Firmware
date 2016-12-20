#ifndef _PID_H_
#define _PID_H_

#include <stdint.h>

typedef struct
{
	float p;
	float i;
	float d;
	float i_decay;
	float out_max;
	float out_min;
	float i_max;
	float i_min;
}
pid_consts_t;

typedef struct
{
	float integral;
	float prev_error;
	float output;
}
pid_data_t;

void pid_reset(pid_data_t* data);
float pid_calc(pid_consts_t* consts, pid_data_t* state, float curr, float tgt);

#endif
