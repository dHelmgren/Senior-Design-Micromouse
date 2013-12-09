#include <stdio.h>
#include "decisionMaker.h"
#include "demo.h"

#define ERROR_HALT_IMMEDIATELY 2
#define WALL 0
#define NO_WALL 1

#define VERBOSE 1

// Global variables
unsigned int isMoving = 0;
unsigned int isTurning = 0;
unsigned int leftIsTuple = WALL;
unsigned int rightIsTuple = WALL;
unsigned float leftClicksTraveled = 0.0;
unsigned float rightClicksTraveled = 0.0;
unsigned float clicksTraveled = 0.0;
unsigned float leSenCl = 0.0;
unsigned float strSenCl = 0.0;
unsigned float riSenCl = 0.0;
rom unsigned float perfectMiddle = 5.628;
rom unsigned float wallOneUnitAway = 19.796;

/* setMove
 * Will communicate with hardware to actually move mouse.
 */
void setMove(int move){
	isMoving = move;
	//printf("Microtaur isMoving = %d.\n", isMoving);
}

/* printSensorInfo
 * Logging information.
 */
void printSensorInfo(float l, float s, float r){
	printf("\nFlo: %d,", (int)(l*100));
	printf("%d,", (int)(s*100));
	printf("%d", (int)(r*100));
}

/* isTuple
 * Checks the left and right sensor info to see if a tuple
 * needs to be created. The straight sensor is checked with
 * another method.
 */
int isTuple(float sides){
	// Must check if the sides variable is in between the
	// possible range of the sensor and 2 * perfectMiddle;
	// If sides > 2 * perfectMiddle, we know that a wall 
	// could not possibly exist there, since even our robot
	// on the opposite end of the wall will trigger a tuple
	
	// Assume that max sensor range is 4 clicks
	if(sides < 4){
		return ERROR_HALT_IMMEDIATELY;
	}
	else if(sides > 2 * perfectMiddle){
		// Yes, make a tuple
		return NO_WALL;
	}
	else{
		// No tuple necessary
		return WALL;
	}
}

/* minitaurMain
 * Main controller for the mouse.
 */
void main(void){//(float leSenCl, float strSenCl, float riSenCl){
	setMove(1);

	while(1){
		updateNextSensorOutputs();
		
		leSenCl = getLeftSensor();
		strSenCl = getStraightSensor();
		riSenCl = getRightSensor();

		// Read in the sensor information
		#if (VERBOSE == 1)
		printSensorInfo(leSenCl, strSenCl, riSenCl);
		#endif

		// Compare the current left sensor to the perfectMiddle
		leftIsTuple = isTuple(leSenCl);
		rightIsTuple = isTuple(riSenCl);

		// Check if we are at the center of a tupled unit, so that
		// we can ask the agent to do its data processing
		// Currently, we know that the straight sensor has to be less
		// than 3 clicks away
		if(strSenCl <= 3){
			makeDecision(clicksTraveled, leftIsTuple, NO_WALL, rightIsTuple, NO_WALL);
		}

		//if(leftIsTuple || rightIsTuple){
			// We know we need to get the number of clicks
			// traveled and reset that value back to 0.0.
			// Maybe we will move these two lines of code to
			// outside of this if statement later because we
			// would like to autocorrect units based on clicks
			// traveled
			//leftClicksTraveled = getLeftClicks();
			//rightClicksTraveled = getRightClicks();

			

			// Tell the AI to return its decision to us
			//	makeDecision(clicksTraveled, left, straight, right, back);
			//}


			// Set the _IsTuple variables back to zero
			leftIsTuple = 0;
			rightIsTuple = 0;
	}
}