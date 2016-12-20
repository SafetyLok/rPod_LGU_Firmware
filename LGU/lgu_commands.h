#ifndef _LGU_COMMANDS_H_
#define _LGU_COMMANDS_H_

#include <stdint.h>

// this is the "index" portion of Pi Comms
enum
{
	LGUCMD_STOP_ALL,           // stops all lift and auxprop motors
	LGUCMD_LIFT_LOWER_ALL,     // lower to the limit switch
	LGUCMD_LIFT_RAISE_ALL,     // raise to the limit switch
	LGUCMD_LIFT_LOWER_SINGLE,  // lower to the limit switch, all other motors stop, for testing only
	LGUCMD_LIFT_RAISE_SINGLE,  // raise to the limit switch, all other motors stop, for testing only
	LGUCMD_LIFT_MOVE_ALL,      // move to a certain position
	LGUCMD_LIFT_MANUAL_RAISE_ALL, // move at 100% speed for a certain time, ignores MLP sensors, for testing only
	LGUCMD_LIFT_MANUAL_LOWER_ALL, // move at 100% speed for a certain time, ignores MLP sensors, for testing only
	LGUCMD_LIFT_STOP_ALL,      // stops all lift motors
	LGUCMD_LIFT_REPORT_ALL,    // causes four reports to be sent, state machine status, switches, and pot position
	LGUCMD_LIFT_REPORT_SINGLE, // causes a single report to be sent, state machine status, switches, and pot position
	LGUCMD_DRIVE_STOP_ALL,     // stops both aux prop motors
	LGUCMD_DRIVE_MOVE_ALL,     // move both aux prop motors at the same speed for a certain duration
	LGUCMD_DRIVE_MOVE_LEFT,    // move only the left  aux prop motor at a speed and duration, stops the right aux prop motor, for testing only
	LGUCMD_DRIVE_MOVE_RIGHT,   // move only the right aux prop motor at a speed and duration, stops the left  aux prop motor, for testing only
	LGUCMD_DRIVE_REPORT_ALL,   // reports current status of both aux prop motors, and the status of the clutch
	LGUCMD_DRIVE_CLUTCH,       // turns clutch on and off
};

// this is the "index" portion of Pi Comms
enum
{
	LGUREPORT_ACK,
	LGUREPORT_NACK,
	LGUREPORT_LIFT_POSITION,
	LGUREPORT_LIFT_SWITCHES,
	LGUREPORT_DRIVE_MODE,
};

// indices used to select a particular lift motor
enum
{
	LGULIFT_FRONT_LEFT  = 0,
	LGULIFT_FRONT_RIGHT = 1,
	LGULIFT_BACK_LEFT   = 2,
	LGULIFT_BACK_RIGHT  = 3,
};

// indices used to select a particular auxprop motor
enum
{
	LGUDRIVE_LEFT  = 0,
	LGUDRIVE_RIGHT = 1,
};

#endif
