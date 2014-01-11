#include <stdio.h>
#include "decisionMaker.h"
#include "demo.h"

// Debugging output
#define VERBOSE 1

// Microtaur and environment macros
#define ERROR_HALT_IMMEDIATELY 2
#define WALL 0
#define FALSE 0
#define NO_WALL 1
#define TRUE 1

// IR sensor benchmark for seeing a wall vs. seeing no wall
#define MAX_NO_TUPLE_READOUT 15.0
#define MOVE_FORWARD 8.0

// Communication with motors
#define STOP 0
#define FORWARD 1
#define BACKWARD 2
#define BRAKE 3

// Global variables

unsigned int isMoving = STOP;

unsigned int isTurning = 0;

unsigned int centeredInUnit = FALSE;

unsigned int leftIsTuple = WALL;

unsigned int straightIsTuple = WALL;

unsigned int rightIsTuple = WALL;

// Whether or not we have already told the optical encoders
// that there is a tuple and therefore the mouse is moving
// to the center of the unit
unsigned int isHeadingToCtrOfUnit = FALSE;

unsigned float leSenCl = 0.0;

unsigned float strSenCl = 0.0;

unsigned float riSenCl = 0.0;

// TODO: Determine how far Microtaur has moved
unsigned float leftClicksTraveled = 0.0;

unsigned float rightClicksTraveled = 0.0;

unsigned float clicksTraveled = 0.0;

/* setMove
 * Will communicate with hardware to actually move mouse.
 */
void setMove(int move){
	isMoving = move;
	printf("Microtaur isMoving = %d.\n", isMoving);
}

/* printSensorInfo
 * Logging information. Must convert the float values to ints
 * because MPLAB's printf does not support %f (float) printing.
 */
void printSensorInfo(float l, float s, float r){
	printf("Flo: %d,", (int)(l*100));
	printf("%d,", (int)(s*100));
	printf("%d\n", (int)(r*100));
}

/* interruptCenteredInUnit
 * Now that Microtaur is centered in the tupled unit, then
 * do the following:
 *    1. BRAKE Microtaur
 *    2. Gather information for agent
 *    3. Call the agent function (ask where to go next)
 *    4. Perform that action by telling the motors to move
 *    5. Reset variables so that Microtaur expects walls to
 *       the left and right, just like before created a tuple
 *    6. Set Microtaur FORWARD again and tell Microtaur to move
 *       8 clicks so that it is guaranteed that Microtaur's left
 *       and right sensors see the next unit.
 */
void interruptCenteredInUnit(){
	centeredInUnit = TRUE;	
	isHeadingToCtrOfUnit = FALSE;

	// Step 1
	setMove(BRAKE);

	// Step 2
	if(strSenCl >= MAX_NO_TUPLE_READOUT){
		straightIsTuple = NO_WALL;
	}
	
	printf("Left %d, straight %d, right %d\n", leftIsTuple, straightIsTuple, rightIsTuple);

	// Step 3
	// Put in dummy number of clicks traveled for now
	makeDecision(18.0, leftIsTuple, straightIsTuple, rightIsTuple, NO_WALL);

	// Step 4 (currently, this is implemented as part of
	// makeDecision because we don't know how to communicate with
	// encoders/drivers of motors yet
	// TODO: insert Step 4 here

	// Step 5
	leftIsTuple = WALL;
	straightIsTuple = WALL;
	rightIsTuple = WALL;
	
	// Step 6: TODO
	setMove(FORWARD);
	
}

/* main
 * Main controller for the mouse.
 */
void main(void){
	setMove(FORWARD);

	while(1){
		// This line of code is for demo.c only
		updateNextSensorOutputs();
		
		// If we have received the interrupt that we are centered in
		// the unit, for the virtual environment's sake, we have to
		// process the next set of sensor input, else leSenCl, strSenCl,
		// and riSenCl will have the previous sensor readout values
		if(centeredInUnit){
			centeredInUnit = FALSE;
			updateNextSensorOutputs();
		}
	
		leSenCl = getLeftSensor();
		strSenCl = getStraightSensor();
		riSenCl = getRightSensor();

		// Read in the sensor information
		#if (VERBOSE == 1)
		printSensorInfo(leSenCl, strSenCl, riSenCl);
		#endif

		// Determine if the left and right sensors see no wall; if
		// they don't see a wall, tell the encoders to move the motors
		// forward to the center of the next unit
		if((leSenCl >= MAX_NO_TUPLE_READOUT) && (leftIsTuple == WALL) &&
			!isHeadingToCtrOfUnit){
			leftIsTuple = NO_WALL;
			// For now, this line of code tells the optical encoders to
			// move the motors to the cneter of the unit; we still have
			// to figure out how exactly this communication happens
			isHeadingToCtrOfUnit = TRUE;
		}
		if((riSenCl >= MAX_NO_TUPLE_READOUT) && (rightIsTuple == WALL) &&
			!isHeadingToCtrOfUnit){
			rightIsTuple = NO_WALL;
			isHeadingToCtrOfUnit = TRUE;
		}
	}//while
}