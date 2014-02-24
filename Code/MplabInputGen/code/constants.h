/* constants.h
 *
 * Describes the constants used to generate bench test output code for 
 * Microtaur.
 */

#include <stdio.h>
#include <math.h>
#define PI (3.141592653589793)




/************ MAZE INFORMATION ************/

// One entire unit without any walls at the front or back of a unit
#define UNIT_CM_NO_WALL 18.0
// This thickness of wall will protrude onto one edge of one unit
#define WALL_CM_HALF_WIDTH 0.6
// The next wall is more than zero units away
#define UNIT_NO_WALL (UNIT_CM_NO_WALL/CLICKS_PER_CM)
// The next wall is zero units away (a wall is straight ahead)
#define UNIT_ONE_WALL ((UNIT_CM_NO_WALL-WALL_CM_HALF_WIDTH)/CLICKS_PER_CM)




/************ MICROTAUR DIMENSIONS ************/

// Defines real world dimensions of Microtaur
#define LENGTH_CM 9.777
#define WIDTH_CM 9.587

#define LENGTH (LENGTH_CM/CLICKS_PER_CM)
#define WIDTH (WIDTH_CM/CLICKS_PER_CM)

#define SENSOR_SHORT_RANGE 4.0
#define SENSOR_LONG_RANGE 40.0
#define SENSOR_TOO_FAR 50.0

// CM measurement converted into clicks
#define LR_SENSORS_FROM_TOP_OF_ROBOT_BASE (2.5/CLICKS_PER_CM)
#define LR_SENSORS_FROM_SIDE_OF_ROBOT_BASE (1.5/CLICKS_PER_CM)

// Click distance of Microtaur's edges from the maze when perfectly in
// the middle of a unit with walls on the left and right; we will use this
// value even if there is a wall zero units ahead of us because 0.6 cm makes
// very little difference
#define SIDE_DIST_FROM_WALL_CENTERED_IN_UNIT (((UNIT_CM_NO_WALL-2*WALL_CM_HALF_WIDTH-WIDTH_CM)/2)/CLICKS_PER_CM)
// Click distance from Microtaur's sensors to LR walls
#define SEN_DIST_FROM_WALL_CENTERED_IN_UNIT (SIDE_DIST_FROM_WALL_CENTERED_IN_UNIT+LR_SENSORS_FROM_SIDE_OF_ROBOT_BASE)
// LR sensor output when wall is immediately present; is constant
#define LR_CONST_SENSOR_OUT (SEN_DIST_FROM_WALL_CENTERED_IN_UNIT*sqrt(2))
// The number of clicks Microtaur is away from being in the tuple unit right
// when Microtaur sees the need to make a tuple
#define LOOKAHEAD ((LR_CONST_SENSOR_OUT/sqrt(2))-LR_SENSORS_FROM_SIDE_OF_ROBOT_BASE+LR_SENSORS_FROM_TOP_OF_ROBOT_BASE)
// Number of clicks Microtaur needs to move until centered in the next unit,
// since seeing the need for a tuple
#define CENTERED_IN_TUPLE_UNIT (LOOKAHEAD+UNIT_ONE_WALL/2+LENGTH/2)




/************ ENVIRONMENT CONTROLLER ************/

// Interval at which to read from the sensors; this is decremented from
// from sensor read after each read
#define INTERVAL 0.5
// The number of clicks of "normal" behavior with which to start and with
// which to end the program, just to make sure we're on track
#define START_END_CLICKS 3.0
// Used to convert from cm to clicks
#define CLICKS_PER_CM 0.98
