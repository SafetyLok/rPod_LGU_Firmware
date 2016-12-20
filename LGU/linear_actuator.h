#ifndef _LINEAR_ACTUATOR_H_
#define _LINEAR_ACTUATOR_H_

#include <stdint.h>
#include <gpin.h>
#include <systmr.h>
#include <pid.h>

#define LIFT_MOTORS_CNT 4

#define LIFTTGT_STOP    (-1)
#define LIFTTGT_RAISED  (-2)
#define LIFTTGT_LOWERED (-3)

extern volatile char linearmotor_gpinInt;

enum
{
	LIFT_STATE_IDLE,
	LIFT_STATE_RAISE,
	LIFT_STATE_LOWER,
	LIFT_STATE_MOVE,
	LIFT_STATE_DONE,
	LIFT_STATE_ERROR,
};

enum
{
	LINEARMOTOR_FLAG_SW_POS = 0x01,
	LINEARMOTOR_FLAG_SW_NEG = 0x02,
	LINEARMOTOR_FLAG_OUTOFSYNC = 0x04,
};

typedef struct
{
	float m;
	float d;
	float b;
}
linearmotor_nvm_t;

typedef struct
{
	int id;
	int last_speed;
	uint8_t state;
	uint8_t flags;
	int16_t target;
	int16_t moving_target;
	int16_t adc;
	int16_t position;
	pid_data_t pid;
	linearmotor_nvm_t* nvm;
}
linearmotor_t;

extern linearmotor_t linearmotors[LIFT_MOTORS_CNT];

extern void linearmotor_setup(void);
void linearmotor_init(void);
int8_t limitswitch_hit(pinnum_t);
void linearmotor_move(linearmotor_t* motor, signed int speed);
void linearmotor_movei(int motor, signed int speed);
char linearmotor_limitCheck(linearmotor_t* motor);
char linearmotor_limitChecki(int motor);
float linearmotor_getPosition(linearmotor_t* motor);
float linearmotor_getPositioni(int motor);
void linearmotor_brake(char);
void linearmotor_gioNotification(void);

#endif
