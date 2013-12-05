/* constants.h
 *
 * Describes the constants used to generate bench test output code for 
 * Microtaur.
 */

#include <stdio.h>
#include <math.h>


/************ MAZE INFORMATION ************/
/*** NOTE that these measurements don't take into account sensor angles! ***/

// One entire unit without any walls at the front or back of a unit
#define UNIT_CM_NO_WALL 18.0
// This thickness of wall will protrude onto one edge of one unit
#define WALL_CM_HALF_WIDTH 0.6
// The next wall is more than zero units away
#define UNIT_NO_WALL (UNIT_CM_NO_WALL/CLICKS_PER_CM)
// The next wall is zero units away (a wall is straight ahead)
#define UNIT_ONE_WALL ((UNIT_CM_NO_WALL-WALL_CM_HALF_WIDTH)/CLICKS_PER_CM)


/************ MICROTAUR DIMENSTIONS ************/

// Defines real world dimensions of Microtaur
#define LENGTH_CM 9.777
#define WIDTH_CM 9.587
// Defines the angles to the side (left or right edge of board) of Microtaur 
// at which the IR sensors are reading in
#define L_SENSOR_RADIANS 0.738379251
#define R_SENSOR_RADIANS 0.521108126
// Defines conversions to clicks
#define LENGTH (LENGTH_CM/CLICKS_PER_CM)
#define WIDTH (WIDTH_CM/CLICKS_PER_CM)
// Click distance of Microtaur's edges from the maze when perfectly in
// the middle of a unit with walls on the left and right; we will use this
// value even if there is a wall zero units ahead of us because 0.6 cm makes
// very little difference
#define MIDDLE_DIST_FROM_WALL (((UNIT_CM_NO_WALL-WIDTH_CM)/2)/CLICKS_PER_CM)


/************ ENCODER INFORMATION ************/

// Used to convert from cm to clicks
#define CLICKS_PER_CM 0.98


/************ IR SENSOR INFORMATION ************/

// Value of R IR sensor output when mouse in middle of unit
#define RIGHT_MIDDLE (MIDDLE_DIST_FROM_WALL/cos(R_SENSOR_RADIANS))
// Value of L IR sensor output when mouse in middle of unit
#define LEFT_MIDDLE (MIDDLE_DIST_FROM_WALL/cos(L_SENSOR_RADIANS))
// Value of sensors when walls are too far away for sensor to see
#define OUT_OF_BOUNDS 999.9


/************ ENVIRONMENT CONTROLLER ************/

// Interval at which to read from the sensors; this is decremented from
// from sensor read after each read
#define INTERVAL 0.5
// The number of clicks of "normal" behavior with which to start and with
// which to end
#define START_END_CLICKS 3.0
