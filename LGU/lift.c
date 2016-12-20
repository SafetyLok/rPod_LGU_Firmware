#include <lift.h>
#include <main.h>
#include <linear_actuator.h>
#include <systmr.h>
#include <picomm.h>
#include <term.h>
#include <stdlib.h>
#include <math.h>

#define LIFT_TASK_RATE_MS 10
#define LIFT_MOVE_SPEED 2
#define LIFT_MIN_SPEED 5

#define LIFT_PIIDX_REPORT_FLAGS(x)    (0x0100 | (x))
#define LIFT_PIIDX_REPORT_POSITION(x) (0x0200 | (x))
#define LIFT_PIIDX_REPORT_SPEED(x)    (0x0300 | (x))
#define LIFT_PIIDX_REPORT_STATE(x)    (0x0400 | (x))

static const pid_consts_t lift_pid;
static char sync_required = 0;
static float sync_target = -1;
static float moving_target = -1;
static systmr_t last_task_time = 0;
static uint32_t no_move_counter;

void lift_task()
{
	int i;

	systmr_t time_delta, now;
	now = systmr_nowMillis();
	time_delta = now - last_task_time;

	for (i = 0; i < LIFT_MOTORS_CNT; i++)
	{
		linearmotor_limitChecki(i);
	}

	for (i = 0; i < LIFT_MOTORS_CNT; i++)
	{
		linearmotor_getPositioni(i);
	}

	if (sync_required != 0)
	{
		if (time_delta < LIFT_TASK_RATE_MS)
		{
			return;
		}
		last_task_time = now;

		lift_syncMove();
	}
	else
	{
		for (i = 0; i < LIFT_MOTORS_CNT; i++)
		{
			//lift_individualMove(i);
		}
	}

	for (i = 0; i < LIFT_MOTORS_CNT; i++)
	{
		linearmotor_limitChecki(i);
	}
}

void lift_syncMove()
{
	int i;
	char can_move = 0;
	float min_error = 10000;
	float max_error = 0;
	float speed[4];
	char has_speed[4];
	int ids[4]; // used for sorting errors
	// use this loop to fill the ids array and to do PID calcs
	for (i = 0; i < LIFT_MOTORS_CNT; i++) {
		float spd;
		ids[i] = i;
		speed[i] = spd = pid_calc((pid_consts_t*)&lift_pid, &linearmotors[i].pid, (float)linearmotors[i].position, moving_target);
		has_speed[i] = (fabsf(spd) > 1) ? 1 : 0;
	}

	// bubble sort according to error
	char is_sorted;
	do
	{
		is_sorted = 1;
		for (i = 0; i < (LIFT_MOTORS_CNT - 1); i++)
		{
			float abs_err_1 = fabsf(linearmotors[ids[i]].pid.prev_error);
			float abs_err_2 = fabsf(linearmotors[ids[i + 1]].pid.prev_error);
			if (abs_err_1 > abs_err_2) {
				int tmp = ids[i];
				ids[i] = ids[i + 1];
				ids[i + i] = tmp;
				is_sorted = 0;
			}

		}
	}
	while (is_sorted == 0);
	// at the end of this, idx 0 has the least amount of error

	// finds the least and most error
	for (i = 0; i < LIFT_MOTORS_CNT; i++)
	{
		float abs_err = fabsf(linearmotors[i].pid.prev_error);
		if (abs_err > max_error) {
			max_error = abs_err;
		}
		else if (abs_err < min_error) {
			min_error = abs_err;
		}
	}

	float err_range = max_error - min_error;
	for (i = 0; i < LIFT_MOTORS_CNT; i++)
	{
		float err_diff = fabsf(linearmotors[i].pid.prev_error) - min_error;
		err_diff /= err_range;
		float speedtmp = speed[i];
		float speed25 = speedtmp / 4;
		float speed75 = speedtmp - speed25;
		float reduc = speed75 * err_diff;
		speed[i] = speed25 + reduc;
	}

	// if error limits are reached, do not allow the moving target to advance
	if (max_error > (1000 / 8) || min_error > (1000 / 32)) {
		can_move = 0;
		if (no_move_counter < 1000) {
			no_move_counter++;
		}
		else {
			for (i = 0; i < LIFT_MOTORS_CNT; i++) {
				linearmotors[i].flags |= LINEARMOTOR_FLAG_OUTOFSYNC;
			}
		}
	}

	if (can_move != 0)
	{
		no_move_counter = 0;
		if (sync_target > moving_target)
		{
			moving_target += LIFT_MOVE_SPEED;
			if (moving_target > sync_target) {
				moving_target = sync_target;
			}
		}
		else if (sync_target < moving_target)
		{
			moving_target -= LIFT_MOVE_SPEED;
			if (moving_target < sync_target) {
				moving_target = sync_target;
			}
		}

		for (i = 0; i < LIFT_MOTORS_CNT; i++) {
			linearmotors[i].flags &= ~LINEARMOTOR_FLAG_OUTOFSYNC;
		}
	}

	// enforce a minimum speed
	for (i = 0; i < LIFT_MOTORS_CNT; i++)
	{
		if (has_speed[i] != 0)
		{
			if (speed[i] >= 0 && speed[i] < LIFT_MIN_SPEED) {
				speed[i] = LIFT_MIN_SPEED;
			}
			else if (speed[i] <= 0 && speed[i] > -LIFT_MIN_SPEED) {
				speed[i] = -LIFT_MIN_SPEED;
			}
		}
		linearmotor_movei(i, lroundf(speed[i]));
	}

	char is_done_all = 1;
	for (i = 0; i < LIFT_MOTORS_CNT; i++) {
		char is_done = 1;
		if (fabsf(linearmotors[i].pid.prev_error) > 5
			|| fabsf(linearmotors[i].pid.integral) > 5
			|| fabsf(speed[i]) >= LIFT_MIN_SPEED
				) {
			is_done = 0;
		}
		if (is_done == 0) {
			is_done_all = 0;
		}
	}

	if (is_done_all != 0)
	{
		for (i = 0; i < LIFT_MOTORS_CNT; i++) {
			linearmotors[i].state = LIFT_STATE_DONE;
		}
	}
}

void lift_brake(char x)
{
	linearmotor_brake(x);
}

void lift_report(uint8_t src, int m)
{
	int i;
	if (m >= 0) {
		i = m;
	}
	else {
		i = 0;
	}
	for (; (i < LIFT_MOTORS_CNT && m < 0) || i == m; i++)
	{

		linearmotor_t* motor = &linearmotors[i];
		if (src == CMDSRC_PICOMM)
		{
			PICOMM_TX_U8(LIFT_PIIDX_REPORT_STATE(i), motor->state);
			PICOMM_TX_U8(LIFT_PIIDX_REPORT_FLAGS(i), motor->flags);
			PICOMM_TX_S16(LIFT_PIIDX_REPORT_POSITION(i), motor->position);
			PICOMM_TX_S16(LIFT_PIIDX_REPORT_SPEED(i), motor->last_speed);
		}
		if (src == CMDSRC_SERTERM)
		{
			TERM_PRINTF("lift[%u]: "
					"state 0x%02X, "
					"flags 0x%02X, "
					"pos = %d, "
					"tgt = %d, "
					"adc = %d, "
					"spd = %d, "
					"\r\n",
					i,
					motor->state,
					motor->flags,
					(int32_t)motor->position,
					(int32_t)motor->adc,
					(int32_t)motor->last_speed
					);
		}
		if (src == CMDSRC_CANBUS)
		{
			// TODO: CAN bus lifter report
		}
	}
}

void lift_cancelAll()
{
	int i;
	sync_required = 0;
	sync_target = -1;
	moving_target = -1;
	for (i = 0; i < LIFT_MOTORS_CNT; i++) {
		linearmotors[i].state = LIFT_STATE_IDLE;
		linearmotor_movei(i, 0);
	}
}
