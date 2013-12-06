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
#define L_SENSOR_TO_BASE_SIDE_CM 0.894
#define R_SENSOR_TO_BASE_SIDE_CM 1.960
// Defines the angles to the side (left or right edge of board) of Microtaur 
// at which the IR sensors are reading in
#define L_SENSOR_RADIANS 0.738379251
#define R_SENSOR_RADIANS 0.521108126
// Defines conversions to clicks
#define LENGTH (LENGTH_CM/CLICKS_PER_CM)
#define WIDTH (WIDTH_CM/CLICKS_PER_CM)
#define L_SENSOR_TO_BASE_SIDE (L_SENSOR_TO_BASE_SIDE_CM/CLICKS_PER_CM)
#define R_SENSOR_TO_BASE_SIDE (R_SENSOR_TO_BASE_SIDE_CM/CLICKS_PER_CM)
// Click distance of Microtaur's edges from the maze when perfectly in
// the middle of a unit with walls on the left and right; we will use this
// value even if there is a wall zero units ahead of us because 0.6 cm makes
// very little difference
#define SIDE_DIST_FROM_WALL_CENTERED_IN_UNIT (((UNIT_CM_NO_WALL-2*WALL_CM_HALF_WIDTH-WIDTH_CM)/2)/CLICKS_PER_CM)
// Distance from the sensor indented portion to the wall; this is NOT what the
// sensor sees, but rather a physical measurement 
#define R_SENSOR_DIST_FROM_WALL_CENTERED_IN_UNIT (SIDE_DIST_FROM_WALL_CENTERED_IN_UNIT+R_SENSOR_TO_BASE_SIDE)
#define L_SENSOR_DIST_FROM_WALL_CENTERED_IN_UNIT (SIDE_DIST_FROM_WALL_CENTERED_IN_UNIT+L_SENSOR_TO_BASE_SIDE)



/************ ENCODER INFORMATION ************/

// Used to convert from cm to clicks
#define CLICKS_PER_CM 0.98
// Number of clicks Microtaur has to move forward from the time that it recognizes
// there is a tuple to the time it is centered in the unit
#define CLICKS_FWD_UNTIL_CENTERED_IN_UNIT (VERTICAL_LOOKAHEAD+UNIT_ONE_WALL/2+LENGTH/2)



/************ IR SENSOR INFORMATION ************/

// Value of R and L IR sensor output when mouse in middle of unit
// LEFT_MIDDLE correct readout: 6.20956272357 = (((16.8-9.587)/2)/0.98+(0.894/0.98))/cos(0.738379251)
// RIGHT_MIDDLE correct readout: 6.54941929271 = (((16.8-9.587)/2)/0.98+(1.960/0.98))/cos(0.521108126)
#define RIGHT_MIDDLE (R_SENSOR_DIST_FROM_WALL_CENTERED_IN_UNIT/cos(R_SENSOR_RADIANS))
#define LEFT_MIDDLE (L_SENSOR_DIST_FROM_WALL_CENTERED_IN_UNIT/cos(L_SENSOR_RADIANS))
// Value of sensors when walls are too far away for sensor to see
#define OUT_OF_BOUNDS 999.9
// Vertical (Straight ahead) distance that the side sensors can see farther
// than the straight sensor, since the side sensors are at an angle to the
// vertical.
// L_SENSOR_V_L correct readout: 4.1797 = 6.2096*sin(0.7384)
// R_SENSOR_V_L correct readout: 3.2606 = 6.5494*sin(0.5211)
#define L_SENSOR_VERTICAL_LOOKAHEAD (LEFT_MIDDLE*sin(L_SENSOR_RADIANS))
#define R_SENSOR_VERTICAL_LOOKAHEAD (RIGHT_MIDDLE*sin(R_SENSOR_RADIANS))
// For the purposes of less complexity, average the lookaheads, thus assuming
// that both the IR sensors look ahead the same distance
#define VERTICAL_LOOKAHEAD ((L_SENSOR_VERTICAL_LOOKAHEAD+R_SENSOR_VERTICAL_LOOKAHEAD)/2)



/************ ENVIRONMENT CONTROLLER ************/

// Interval at which to read from the sensors; this is decremented from
// from sensor read after each read
#define INTERVAL 0.5
// The number of clicks of "normal" behavior with which to start and with
// which to end the program, just to make sure we're on track
#define START_END_CLICKS 3.0

//#define CONVERT_
