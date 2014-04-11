/****** INCLUDES ******/
#include <p18f27j13.h>
#include <timers.h>
#include <delays.h>
#include <adc.h>
//#include <math.h> // This is not used because we implemented our
                    // own mod function

/****** MACROS ******/

// Commenting in and commenting out the below #define will determine
// if we are programming on Fibrotaur right now or on Microtaur
//#define FIBROTAUR 1
#ifdef FIBROTAUR

/********************* FIBROTAUR ********************/
// Navigation constants for Fibrotaur

#define GO_LEFT 			0b11101010
#define GO_RIGHT 			0b11110101
#define BREAK_L_WHEEL		0b11111011
#define BREAK_R_WHEEL 		0b11111101
#define BACKWARD_L_WHEEL	0b11111110
#define BACKWARD_R_WHEEL	0b11110111

#define CLICKS_FOR_180		0xA8
#define CLICKS_FOR_NINETY_L	0x9B
#define CLICKS_FOR_NINETY_R	0x9B
#define CLICKS_FOR_45		0x4C

// Poll 1
#define CONTINUE_TO_CENTER_R_FIRST 16
#define CONTINUE_TO_CENTER_L_FIRST 14

// Poll 2 constants for Fibrotaur
#define NO_WALL_LEFT 100
#define NO_WALL_LEFT_IN_TUPLE (NO_WALL_LEFT + 100)
#define NO_WALL_RIGHT 180
#define NO_WALL_RIGHT_IN_TUPLE (NO_WALL_RIGHT + 100)
#define TUPLE_CHANGE 1024

// Poll 3: Move mouse forward
#define LEAVE_UNIT 5

// Constants to trigger push autocorrect
#define LR_DIFF				250
#define ERROR_CORRECT_L_1	700 //used to be 780
#define ERROR_CORRECT_R_1	(ERROR_CORRECT_L_1 - LR_DIFF)
#define ERROR_CORRECT_L_2	700 //used to be 780
#define ERROR_CORRECT_R_2	(ERROR_CORRECT_L_2 - LR_DIFF)
#define ERROR_CORRECT_CAP_L	340
#define ERROR_CORRECT_CAP_R	(ERROR_CORRECT_CAP_L - LR_DIFF)
#define RIGHT_BUFFER		30

// Constants for reading from the ADC
#define STRAIGHT_IR_SELECT 	0b00000001
#define RIGHT_IR_SELECT 		0b00000101
#define LEFT_IR_SELECT 	0b00001001

// Stop the mouse because it is about to run into the wall, since the
// IR sensor readouts are too high and close to the 3 cm max readout
#define STOP 980

//////////////////END FIBROTAUR//////////////////

/***************** MICROTAUR ********************/
#else // We are programming on Microtaur, not Fibrotaur
// Navigation constants for Microtaur
#define GO_LEFT 			0b11101010
#define GO_RIGHT 			0b11110101
#define BREAK_L_WHEEL		0b11111011
#define BREAK_R_WHEEL 		0b11111101
#define BACKWARD_L_WHEEL	0b11111110
#define BACKWARD_R_WHEEL	0b11110111

#define CLICKS_FOR_180		0xAC //AC is optimal for competition maze
#define CLICKS_FOR_NINETY_L 0xB1 //B1 is optimal for competition maze
#define CLICKS_FOR_NINETY_R	0xA7 //A7 is optimal for competition maze
#define CLICKS_FOR_45		0x3C

// Poll 1
#define CONTINUE_TO_CENTER_R_FIRST 16 // was 16
#define CONTINUE_TO_CENTER_L_FIRST 14 // was 12

// Poll 2 constants for Microtaur
#define NO_WALL_LEFT 250
#define NO_WALL_LEFT_IN_TUPLE (NO_WALL_LEFT + 100)
#define NO_WALL_RIGHT 200
#define NO_WALL_RIGHT_IN_TUPLE (NO_WALL_RIGHT + 30)
#define TUPLE_CHANGE 250

// Poll 3: Move mouse forward 4 cm
#define LEAVE_UNIT 5

// Constants to trigger push autocorrect
#define LR_DIFF				160 // Changed from 150
#define ERROR_CORRECT_L_1	760
#define ERROR_CORRECT_R_1	(ERROR_CORRECT_L_1 - LR_DIFF)
#define ERROR_CORRECT_L_2	760
#define ERROR_CORRECT_R_2	(ERROR_CORRECT_L_2 - LR_DIFF)
#define ERROR_CORRECT_CAP_L	340
#define ERROR_CORRECT_CAP_R	(ERROR_CORRECT_CAP_L - LR_DIFF)
#define RIGHT_BUFFER		30

// Constants for reading from the ADC
#define STRAIGHT_IR_SELECT 	0b00000001
#define LEFT_IR_SELECT 		0b00000101
#define RIGHT_IR_SELECT 	0b00001001

// Stop the mouse because it is about to run into the wall, since the
// IR sensor readouts are too high and close to the 3 cm max readout
#define STOP 940

#endif
//////////////////////////END MICROTAUR///////////////////////

#define true    	1 
#define false   	0 
#define left		1
#define right		0
#define straight 	2
#define turnAround 	3
#define backward	0
#define forward		1

#define GO_STRAIGHT 		0b11111001
#define GO_BACKWARD 		0b11110110
#define BREAK 				0b11111111

#define CLICKS_PER_CM 	55  // Was messing up in hex
#define CLICKS_PER_HALF_CM 27
#define ONE_UNIT_CM 	18
#define STRAIGHT_IR_STOP 128
#define IS_DEAD_END 	1

// Poll 2: No wall means wall is farther than digital value 250
#define NO_WALL_STRAIGHT 350

// Number of clicks to back up when we have crashed
#define CRASH_BACK_UP	330

// Constants to trigger pull autocorrect
#define PULL_CORRECT_L_1	480
#define PULL_CORRECT_R_1	(PULL_CORRECT_L_1 - LR_DIFF)
#define PULL_CORRECT_L_2	480
#define PULL_CORRECT_R_2	(PULL_CORRECT_L_2 - LR_DIFF)

// Constants to define how much to autocorrect
#define CLICKS_FOR_AC_1		0x20
#define CLICKS_FOR_AC_2		0x20//was 0x18

// How long to wait in between states of the code
#define DELAY 70

// How long to make the right wheel to move before the left wheel moves
// because the left wheel tends to jerk the robot left
#define DELAY_JERK 100


// Constants for AI Program
#define AI_WEST 0
#define AI_NORTH 1
#define AI_EAST 2
#define AI_SOUTH 3

#define AI_LEFT 0
#define AI_STRAIGHT 1
#define AI_RIGHT 2
#define AI_BACK 3

#define NODE_STRAIGHT 0
#define NODE_RIGHT 1
#define NODE_BACK 2
#define NODE_LEFT 3
#define NODE_45	4

#define CHILD_PENALTY 5
#define NULL 0
#define BLANK {0,0,0,0,0,0,0}

/****** STRUCT ******/

typedef struct NavNode{
	char rating;
	char xOffset;
	char yOffset;
	struct NavNode * west;
	struct NavNode * north;
	struct NavNode * east;
	struct NavNode * south;
}NavNode;


/****** FUNCTION DEFINITIONS ******/

unsigned int adConvert(unsigned char channel);
void autocorrect(unsigned char direction, unsigned char clicks, unsigned char howAutocorrect);
void blinkTest(void);
unsigned char calcUnitsTraveled(void);
void clearTimers(void);
void fwdNoJerk(void);
void goForward(int distance); // Step 2 of 4 step polling process
unsigned char ifAutocorrect(void);
void ifSuicide(void);
void initial(void);
void init4StepPoll(unsigned char isDeadEnd);
unsigned char makeDecision(unsigned char deltaDist, unsigned char leftWall, unsigned char straightWall, unsigned char rightWall);
void msDelay(unsigned int time);
unsigned char mod4(unsigned char value);
void readTimersToTraveled(void);
void stopTest(void);
void turn(unsigned char direction); // Step 3

//AI Defs
int rateNode(int x, int y);
unsigned char rateDir(int compass, int turnDir, int xx, int yy);
unsigned char rateVisitedNode(NavNode* node, int compass, int turnDir);
NavNode* buildNode(int currX, int currY);
unsigned char mod4(unsigned char val);
int numChildren(NavNode* check, int compass);
int pickPath(int leftL, int forwardF, int rightR);

/****** GLOBAL VARIABLES ******/

/* Number of centimeters traveled since last time optical encoders
 * were cleared */
unsigned char cmTraveled = 0;

/* Whether or not there is a wall on the left */
unsigned char leftWall = true;

/* Whether or not there is a wall on the right */
unsigned char rightWall = true;

/* Whether or not there is a wall straight ahead */
unsigned char straightWall = true;

/* Can make tuple. This boolean is false if a tuiple is not allowed to be made at this time. */
unsigned char canCallTuple = true;

/* Which absence of a wall caused a tuple to be recognized first? */
unsigned char firstNoWall = -1;

int irCvtL = 0;
int irCvtR = 0;
int irCvtS = 0;
int irCvtLP1 = 0;
int irCvtRP1 = 0;
int irCvtLP2 = 0;
int irCvtRP2 = 0;
int irCvtLP3 = 0;
int irCvtRP3 = 0;
int irCvtLP4 = 0;
int irCvtRP4 = 0;

int noWallL = -1;
int noWallR = -1;

/* Timer values which indicate how far mouse has traveled since
   last tuple */
unsigned int traveled0 = 0;
unsigned int traveled1 = 0;

/* Since we have run into trouble when the mouse has tried to autocorrect
   immediately after a tuple, wait just a few moments to make sure there isn't
   another tuple ahead of us */
unsigned char waitToAutocorrect = false;

/* AI agent delete-me-later variable which keeps track of where the
   mouse is (hardcoded) in the maze and makes turn decisions based
   on that */
unsigned char hardcodedAgent = 0;

//AI variables

int compass = AI_NORTH;

//Temporary stopgap to access the memory array
int memIndex = 0;


unsigned char sawDeadEndLastTime = false;
NavNode root = {14, -8,-8, 0, 0, 0, 0}; 
NavNode blank = {0,0,0,0,0,0,0};
NavNode* currentNode;
NavNode* prevNode;
unsigned int i = 0;
unsigned int j = 0;

//#pragma idata mazeArray
#pragma udata BIGDATA1
NavNode * ram mazeArray[16][16];
#pragma udata
//#pragma idata emptyNodes
#pragma udata BIGDATA2
NavNode ram emptyNodes[116];
#pragma udata




/****** CODE ******/
#pragma code 

void main(void)
{
	// Initialization of constants
	unsigned char straightIR = 0;
	unsigned char tempF = 0;
	unsigned char tempL = 0;
	unsigned char tempR = 0;
	unsigned char leftIR = 0;
	unsigned char rightIR = 0;
	unsigned char oldIrCvtL = 0;
	unsigned char oldIrCvtR = 0;
	unsigned char oldIrCvtS = 0;
	for(i = 0; i < 116; i++){emptyNodes[i] = blank;}
	for(i = 0; i < 16; i++){for(j=0; j < 16; j++){mazeArray[i][j] = NULL;}}

	currentNode = &root;
	prevNode = &root;
	mazeArray[0][0] = &root;


	Nop();
	initial();

	/**** BEGIN! ****/
	msDelay(10000);
	while(true) {

		PORTB = GO_STRAIGHT;

		// Insert a delay due to the fact that without the delay, unrealiable
		// variations in IR sensor readings would be read
		Delay10TCYx(1);
		irCvtL = adConvert(LEFT_IR_SELECT);
		Delay10TCYx(1);
		irCvtR = adConvert(RIGHT_IR_SELECT);
		Delay10TCYx(1);
		irCvtS = adConvert(STRAIGHT_IR_SELECT);

		PORTB = BREAK;
		Nop();

		// Determine if the L or R sensors have seen the absence of
		// a wall. If they have, then initiate the 4-step polling process
		if(irCvtLP4 !=0 && irCvtRP4 !=0) {
			noWallL = NO_WALL_LEFT;
			noWallR = NO_WALL_RIGHT;
		} // This if statement should end here because it gives the mouse
		// permission to make a tuple immediately, which is extremely necessary
		// where in situations where there is a tuple after a tuple

		// If we are about to run into a wall, and we have not seen the absence
		// of a wall to the right or left, then we know we have hit a dead end
		if(irCvtS >= STOP){
			PORTB=BREAK;
			msDelay(DELAY);
			straightWall = true;
			init4StepPoll(IS_DEAD_END);
		}

		// Determine if we need to autocorrect, and if we do, then do so.
		// Do not autocorrect if we haven't traveled far enough away from the
		// current tuple, because statistically, immediate post-turn autocorrect
		// has put the mouse at an incorrect angle (turns are very accurate)
		if(ifAutocorrect()==true){
			continue;
		}

		// Either the left IR sensor value is smaller than a certain threshold
		// or the IR sensor value has dropped drastically, telling Microtaur
		// that it has reached a decision point (a tuple). The drastic drop
		// is computed based on the last 5 IR sensor readings.
		if(irCvtL <= noWallL||irCvtLP4-irCvtL >= TUPLE_CHANGE){
			PORTB=BREAK;
			msDelay(DELAY);
			PORTB=GO_STRAIGHT;
			leftWall = false;
			// Set this value so that we can how far to continue to center,
			// since the ir sensors are placed at different angles, the right
			// sensor will always recognize a tuple first
			firstNoWall = left;
			init4StepPoll(!IS_DEAD_END);
		}
		else if(irCvtR <= noWallR||irCvtRP4-irCvtR >= TUPLE_CHANGE){
			PORTB=BREAK;
			msDelay(DELAY);
			PORTB=GO_STRAIGHT;
			rightWall = false;
			firstNoWall = right;
			init4StepPoll(!IS_DEAD_END);		
		}

		// Reinitialize the last 5 IR sensor readings.
		irCvtLP4 = irCvtLP3;
		irCvtRP4 = irCvtRP3;
		irCvtLP3 = irCvtLP2;
		irCvtRP3 = irCvtRP2;
		irCvtLP2 = irCvtLP1;
		irCvtRP2 = irCvtRP1;
		irCvtLP1 = irCvtL;
		irCvtRP1 = irCvtR;

	}//while(true)

}//main

unsigned char ifAutocorrect(void){
	// Determine if autocorrect is needed
	// If we're too close on the left side or too far on the right side

	// The commented-out code in the if-statements is the pull autocorrect
	// code left there left there, in case a future team decides to use it.
	// Currently, the non-commented-out code is the push autocorrect (we push
	// away from a wall when it is too close to us).
	if(irCvtL >= ERROR_CORRECT_L_1/* || (irCvtR <= PULL_CORRECT_R_1 && irCvtR >= NO_WALL_RIGHT)*/){
		if(irCvtL >= ERROR_CORRECT_L_2/* || (irCvtR <= PULL_CORRECT_R_2 && irCvtR >= NO_WALL_RIGHT)*/){
			autocorrect(left, CLICKS_FOR_AC_2, forward);
			return true;
		}
		else{
			autocorrect(left, CLICKS_FOR_AC_1, forward);
			return true;
		}
	}
	// Else if we're too close on the right side or too far on the left side
	else if(irCvtR >= ERROR_CORRECT_R_1/* || (irCvtL <= PULL_CORRECT_L_1 && irCvtL >= NO_WALL_LEFT)*/){
		if(irCvtR >= ERROR_CORRECT_R_2/* || (irCvtL <= PULL_CORRECT_L_2 && irCvtL >= NO_WALL_LEFT)*/){
			autocorrect(right, CLICKS_FOR_AC_2, forward);
			return true;
		}
		else{
			autocorrect(right, CLICKS_FOR_AC_1, forward);
			return true;
		}
	}
	return false;
}

void autocorrect(unsigned char direction, unsigned char clicks, unsigned char howAutocorrect){
	PORTB=BREAK;

	// Add the distance Microtaur has traveled thus far to the total 
	// traveled since the last tuple was made
	readTimersToTraveled();

	// Reset the timers to count exactly how far Microtaur has traveled
	// in this method because these variables are "local"
	clearTimers();

	PORTB=GO_STRAIGHT;
  
  	if(direction == left){ // We need to turn a little right
  
  		if(howAutocorrect == backward){
  			PORTB=BACKWARD_R_WHEEL;
  		}
  		else{
  			PORTB=BREAK_R_WHEEL;
  		}
  		
		// Wait until the optical encoders reach at least "clicks"
  		while(TMR0L < clicks && TMR0H < 255 && TMR1L < clicks && TMR1H < 255) {
			Nop();
		}
  	}//if
  
  	else{ // direction == right; we need to turn left
  
  		if(howAutocorrect == backward){
  			PORTB=BACKWARD_L_WHEEL;
  		}
  		else{
  			PORTB=BREAK_L_WHEEL;
  		}
  
		// Wait until the optical encoders reach at least "clicks"
  		while(TMR0L < clicks && TMR0H < 255 && TMR1L < clicks && TMR1H < 255) {
			Nop();
		}
  	}//else
  
	// Do not reset timers here because we want timers to continue to count
	// even during autocorrect (else we will lose data because we are still
	// moving forward
  
  	PORTB=GO_STRAIGHT;
}//autocorrect

void init4StepPoll(unsigned char isDeadEnd){
	// local variables must be declared at the top of the function in C18
	char decision;
	unsigned char i = 0;

	// Please note that the first step in the four step polling process was
	// to recognize the presence of a decision point (a tuple)

	if(!isDeadEnd){

		// Step 2 of four step polling process
		if(firstNoWall == left){
			goForward(CONTINUE_TO_CENTER_L_FIRST*2);
		}
		else{
			goForward(CONTINUE_TO_CENTER_R_FIRST*2);
		}

		// Delay for our own debug purposes (so we can understand where
		// Microtaur is in the code when watching it run around the maze)
		msDelay(DELAY);

		// We inserted this for loop because we were getting weird readings
		// from the front IR sensor (readings that were clearly larger than
		// the allowed max value 1024)
		for(i=0;i<5;++i) {
			Delay10TCYx(10);
			irCvtS = adConvert(STRAIGHT_IR_SELECT);
		}

		// This straight sensor reading is correct, so use it to test
		if(irCvtS <= (int)NO_WALL_STRAIGHT){
			straightWall = false;
		}
		else{
			straightWall = true;
		}
	}

	// Debug break
	PORTB=BREAK;

	// This if statement is just to be safe; it can be removed once
	// this information is verified
	if(isDeadEnd){
		leftWall = true;
		straightWall = true;
		rightWall = true;
	}

	// Step 3
	// Add the distance Microtaur has traveled thus far to the total 
	// traveled since the last tuple was made; AI will need this info
	// Add the distance Microtaur has traveled thus far to the total 
	// traveled since the last tuple was made
	readTimersToTraveled();
	decision = makeDecision(calcUnitsTraveled(), leftWall, straightWall, rightWall);
	turn(decision);
	// Determine if the possibly erroneous turn caused us to be directly
	// in front of a wall

	// Reset all variables used in the main loop to prepare for the
	// next tuple or autocorrect
	irCvtLP1 = 0;
	irCvtRP1 = 0;
	irCvtLP2 = 0;
	irCvtRP2 = 0;
	irCvtLP3 = 0;
	irCvtRP3 = 0;
	irCvtLP4 = 0;
	irCvtRP4 = 0;
	noWallL = -1;
	noWallR = -1;

	// Reset wall data
	leftWall = true;
	rightWall = true;
	straightWall = false;

	// Reset crash and burn, and click counting for the AI
	clearTimers();
	traveled0 = 0;
	traveled1 = 0;

	// Break and delay code such as the following two lines are typically
	// used as debug so that we, watching Microtaur run around in the maze,
	// can see where exactly it is in the code. In order for the delays to 
	// be visible, the DELAY variable will probably need to be increased
	PORTB=BREAK;
	msDelay(DELAY);

	// Step 4: Continue a little past current unit
	goForward((LEAVE_UNIT-1)*2+1);

	// Debug break
	PORTB=BREAK;
	msDelay(DELAY);

	PORTB=GO_STRAIGHT;
}

unsigned int adConvert(unsigned char channel)
{
	int irValue;
	//set's the Analog input channel to the given input
	ADCON0 = 0;
	ADCON1 = 0b10100001; //0xa1
	ADCON0 = channel;
	//CHARGE the CAPACITOR!!!
	//must wait 2.45us
	//instead waiting 10ms 
			//Note: this timing can be played with to get a
			//more accurate reading
	Delay10TCYx(0.5);
	//turn go on!
	ADCON0 = (ADCON0 | 0x02); //or-ed with 2
	//wait for the done bit to be cleared
	//meaning the conversion is done
	while(ADCON0bits.NOT_DONE)
	{
		Nop();
	}
	//read in value to the variable
	irValue = ReadADC();
	//close the converter module
	CloseADC();

	return irValue;
}

void goForward(int distance){
	// Note that this function is used both to get Microtaur to the center
	// of the tupled unit (step 2 of four step polling process) and to get
	// Microtaur out of the current unit (step 4 of the polling process)
	unsigned int highestIrCvtS = 0;
	unsigned int tempIrCvtS = 0;

	// Add the distance Microtaur has traveled thus far to the total 
	// traveled since the last tuple was made
	readTimersToTraveled();

	// Reset the timers to count exactly how far Microtaur has traveled
	// in this method because these variables are "local"
	clearTimers();

	PORTB=GO_STRAIGHT; //Drive forward
	cmTraveled = 0;

	// While Microtaur has not traveled as far as the function's parameter
	// desires
	while(cmTraveled < distance)
	{
		// Get the values for the left and right IR sensors
		irCvtL = adConvert(LEFT_IR_SELECT);
		irCvtR = adConvert(RIGHT_IR_SELECT);

		// Continue to gather information about our tuple, specifically if
		// there is a wall to the left or the right
		if(cmTraveled <= 5 && (distance == CONTINUE_TO_CENTER_R_FIRST || distance == CONTINUE_TO_CENTER_L_FIRST)){
			// This constant, NO_WALL_LEFT_IN_TUPLE is necessary because
			// as soon as Microtaur has entered a tupled unit, then Microtaur
			// continues to drive closer and closer to (possibly) a wall ahead.
			// Therefore, if Microtaur has not yet seen the absence of a wall
			// on one of its sides, a smaller constant cutoff is used to see the
			// wall absence.
			if((irCvtL <= NO_WALL_LEFT_IN_TUPLE || irCvtLP4 - irCvtL >= 250) && leftWall==true){
				// Debug break
				PORTB=BREAK;
				msDelay(DELAY);
				PORTB=GO_STRAIGHT;
				leftWall = false;
			}
			else if((irCvtR <= NO_WALL_RIGHT_IN_TUPLE || irCvtRP4 - irCvtR >= 250) && rightWall==true){
				// Debug break
				PORTB=BREAK;
				msDelay(DELAY);
				PORTB=GO_STRAIGHT;
				rightWall = false;
			}

			// Continue to update previous sensor readings, like in the 
			// main method
			irCvtLP4 = irCvtLP3;
			irCvtRP4 = irCvtRP3;
			irCvtLP3 = irCvtLP2;
			irCvtRP3 = irCvtRP2;
			irCvtLP2 = irCvtLP1;
			irCvtRP2 = irCvtRP1;
			irCvtLP1 = irCvtL;
			irCvtRP1 = irCvtR;
		}//if

		// Use even a different wall constant cutoff (details in the previous 
		// comment above the "if") while traveling between 5 and 7 cm
		else if(cmTraveled > 5 && cmTraveled <=7 && (distance == CONTINUE_TO_CENTER_R_FIRST*2 || distance == CONTINUE_TO_CENTER_L_FIRST*2)){
			if((irCvtL <= (NO_WALL_LEFT_IN_TUPLE+50) || irCvtLP4 - irCvtL >= 250) && leftWall==true){
				PORTB=BREAK;
				msDelay(DELAY);
				//blinkTest();
				PORTB=GO_STRAIGHT;
				leftWall = false;
			}
			else if((irCvtR <= (NO_WALL_RIGHT_IN_TUPLE+50) || irCvtRP4 - irCvtR >= 250) && rightWall==true){
				PORTB=BREAK;
				msDelay(DELAY);
				//blinkTest();
				PORTB=GO_STRAIGHT;
				rightWall = false;
			}

			// Continue to update previous sensor readings, like in the 
			// main method
			irCvtLP4 = irCvtLP3;
			irCvtRP4 = irCvtRP3;
			irCvtLP3 = irCvtLP2;
			irCvtRP3 = irCvtRP2;
			irCvtLP2 = irCvtLP1;
			irCvtRP2 = irCvtRP1;
			irCvtLP1 = irCvtL;
			irCvtRP1 = irCvtR;
		}//if


		// This if statement will be executed only when we have traveled
		// have a centimeter
		if(TMR0L >= CLICKS_PER_HALF_CM){
			cmTraveled++; // The name of this variable really should be
						  // "halfCmTraveled", since we shortened the 
						  // measuring factor to 1/2 a cm very soon before
						  // competition
			// Reset the timer
			TMR0L = TMR0L - CLICKS_PER_HALF_CM;
			tempIrCvtS = 0;
			Delay10TCYx(1);
			tempIrCvtS = adConvert(STRAIGHT_IR_SELECT);
			if(tempIrCvtS >= highestIrCvtS){
				if(tempIrCvtS >= STOP){
					PORTB=BREAK;
					highestIrCvtS = tempIrCvtS;
					// Prevents Microtaur from crashing into a wall when the
					// constant CONTINUE_TO_CENTER may be too high
					break;
				}
				else if(tempIrCvtS < 1024){
					// We are getting a faulty reading from the adConvert method
					// so work around it by not letting the mouse get to the other
					// side of the voltage curve
					highestIrCvtS = tempIrCvtS;
				}
			}//if
		}//if
	}//while

	// We are continually moving closer to a wall, and haven't gotten too
  	// close yet. Therefore, keep going (using wall ahead as a marker) until
  	// we are in center of tupled unit.
	PORTB=BREAK;
	if(distance == CONTINUE_TO_CENTER_L_FIRST || distance == CONTINUE_TO_CENTER_R_FIRST){
		Delay10TCYx(1);
	  	tempIrCvtS = adConvert(STRAIGHT_IR_SELECT);
	  	if(highestIrCvtS >= 300 && highestIrCvtS < STOP){// && irCvtS - prevIrCvtS > 0){
			// Our stopping point is a wall ahead of us in this same tuple, so
			// track to that wall
	  		straightWall = true;
	  		PORTB=GO_STRAIGHT;
	  		while(highestIrCvtS < STOP){
				Delay10TCYx(1);
				tempIrCvtS = adConvert(STRAIGHT_IR_SELECT);
	  			highestIrCvtS = tempIrCvtS;
	  		}
			PORTB=BREAK;
	  	}//if
	}//if

	PORTB=BREAK;
}

void turn(unsigned char direction)
{
	// local variables
	unsigned char turnClicks = CLICKS_FOR_NINETY_L;
	unsigned char numTurns = 1;
	unsigned char i = 0;
	int countA = 0;
	int countB = 0;
    clearTimers();

	// Execute the decision made by the AI agent
	if(direction == NODE_LEFT){
		PORTB=GO_LEFT;
	}
	else if(direction == NODE_RIGHT){
		turnClicks = CLICKS_FOR_NINETY_R;
		PORTB=GO_RIGHT;
	}
	else if(direction == NODE_BACK){
		numTurns = 2;
		turnClicks = CLICKS_FOR_180;
		PORTB=GO_RIGHT;
	}
	else if(direction == NODE_STRAIGHT){
		PORTB=GO_STRAIGHT;
		return;
	}
	else { // direction == NODE_45
		// Because we are turning 45 degrees, we don't want to turn
		// directly against a wall. Thus, go backward first
		PORTB=GO_BACKWARD;

		Delay10TCYx(10);
		while(countA < 90 && countB < 90) {
			// check again and repeat.
			countA = TMR0L;
			countB = TMR1L;
		}
		countA = 0;
		countB = 0;
		PORTB=BREAK;
		msDelay(DELAY);

		clearTimers();

		// Now initiate the 45 degree turn by setting the port and continuing
		// in the for loop below
		irCvtL = adConvert(LEFT_IR_SELECT);
		irCvtR = adConvert(RIGHT_IR_SELECT);
		if(irCvtL <= irCvtR){
			PORTB=GO_LEFT;
		}
		else{
			PORTB=GO_RIGHT;
		}
		
		turnClicks = CLICKS_FOR_45;
	}//else

	for(i = 0; i < numTurns; ++i){

		// As you can see, the following lines of code are copied and 
		// pasted twice. This is because Microtaur would go in circles
		// and never stop if we just multiplied turnClicks by two. This
		// is due to overflow of countA.
		Delay10TCYx(2000); //10ms delay

		while(countA < turnClicks && countB < turnClicks) {
			// check again and repeat.
			countA = TMR0L;
			countB = TMR1L;
		}
		countA = 0;
		countB = 0;
    	clearTimers();

		Delay10TCYx(2000); //10ms delay

		while(countA < turnClicks && countB < turnClicks) {
			// check again and repeat.
			countA = TMR0L;
			countB = TMR1L;
		}

		countA = 0;
		countB = 0;
    	clearTimers();
	}

	PORTB=BREAK;
}

void initial(void)
{
	//set up the internal oscillator to 8MHz
	OSCCONbits.IRCF = 0b111;

	//Remap pin 12 to be Timer0
	EECON2 = 0x55;
	EECON2 = 0xAA;
	PPSCON = 0;
	RPINR4 = 0xC;
	PPSCON = 1;

	//Set up timer0
	T0CON = 0b10111000; //0xb8
	//Set up timer1
	T1GCON = 0b01001100;
	T1CON = 0b10000111;


	//set RB0, RB1, RB2, RB3 as outputs
	TRISB = 0;
	//set RA0, RA1, RA2 as analog and RA3, RA5 as Digital
	ANCON0 = 0b111000;
	//set RA3 and RA5 as outputs, RA0, RA1, RA2 as inputs
	TRISA = 0b00000111;
	//initializing the A/D module with AN0 as intital channel selected
	ANCON0 = 0b00000001;

			//#TODO##
			//I tried setting this register, and I got that weird behavior of the A/D converter
			//So then I tried a different value, but I got the same behavior.  Then I tried commenting it out
			//And leaving it in the default configuration, but the behavior stayed.  I am very confused about what
			//I should set the ANCON0 and ANCON1 registers to because they talk about something about two different
			//clocks, and Tad variable. I am doing the A/D via polling, but it can be done via interrupts.  Would that 
			//be better?? Am I not giving the A/D converter enough time to convert the value?

	//ANCON1 = 0b10000000;
	
	// Even with this issue, Microtaur worked just fine.
		
	//Set RC0 and RC1 as inputs
	TRISC = 0b00000011;

	//Clear all port values
	PORTA = 0;
	PORTB = 0;
	PORTC = 0;
	clearTimers();
}

void clearTimers(void){
	// High must be written to first and low must be read first 
	// due to buffer updates in the background. Else, code won't work. 
	TMR0H = 0; //Right wheel
	TMR0L = 0;

	TMR1H = 0; //Left wheel
	TMR1L = 0;
}

void readTimersToTraveled(void){
	int tempInt = 0;
	Delay100TCYx(1);
	// Add the distance Microtaur has traveled thus far to the total 
	// traveled since the last tuple was made
	traveled0 += TMR0L;
	tempInt = (int)TMR0H;
	traveled0 += (tempInt << 8);
	tempInt = 0;
	traveled1 += TMR1L;
	tempInt = (int)TMR1H;
	traveled1 += (tempInt << 8);
}

unsigned char calcUnitsTraveled(void){
	unsigned char unitsTraveled = 0;
	unsigned int tempTraveled = traveled1;
	
	// The PIC microcontroller is not very good at division by a non
	// power of two. Therefore, we implemented our own.
	while(tempTraveled >= 990){
		unitsTraveled++;
		tempTraveled = tempTraveled - 990;
	}
	if(tempTraveled >= 445)
		unitsTraveled++;
	return unitsTraveled;
}

void blinkTest(void)
{
	//Blink LEDs on
	PORTA = 0b000000;
	msDelay(500);
	//Blink LEDs off
	PORTA = 0b101000;
	msDelay(500);
	//Blink LEDs on
	PORTA = 0b000000;
	msDelay(500);
	//Blink LEDs off
	PORTA = 0b101000;
	msDelay(500);
}

void msDelay(unsigned int itime)
{
	unsigned int i;
	unsigned char j;
	for(i = 0;i < itime;i++)
	{
		for(j=0; j<165; j++)
		{
			Nop();
		}
	}
}


//TODO: We need to prune bad nodes
// See ai.h for comment
unsigned char makeDecision(unsigned char deltaDist, unsigned char leftWall, unsigned char straightWall, unsigned char rightWall){
	//I expect information as (leftIsWall, centerIsWall, rightIsWall, distFromLast)

	int leftRating = 99;
	int rightRating = 99;
	int forwardRating = 99;
	unsigned char choice = 5;
	int picked = 5;
	int backPos = (compass + NODE_BACK) %4;
	int indX = -9;
	int indY = -9;
	int currX = (int) currentNode->xOffset;
	int currY = (int) currentNode->yOffset;

	// Need to correct if deltaDist == 0?

	if(deltaDist == 0 && sawDeadEndLastTime == true){
		compass = mod4(compass+1);
		return NODE_RIGHT;
	}

	sawDeadEndLastTime = false;

	//determine the proper deltaDistance so that we can get an accurate location in the maze
	if(compass == AI_NORTH)
	{
		//deals with crossing the origin, as each square is measured by the outermost corner
		if(currentNode->yOffset < 0 && deltaDist >= -currentNode->yOffset)
		{
			//if we cross the origin, we modify the distance to skip zero
			deltaDist++;
		}
		currY += deltaDist;

	}
	else if(compass == AI_SOUTH)
	{
		if(currentNode->yOffset > 0 && deltaDist >= currentNode->yOffset)
		{
			deltaDist++;
		}
		currY -= deltaDist;
	}
	else if(compass == AI_EAST)
	{
		if(currentNode->xOffset < 0 && deltaDist >= -currentNode->xOffset)
		{
			deltaDist++;
		}
		currX += deltaDist;
	}
	else if(compass == AI_WEST)
	{
		if(currentNode->xOffset > 0 && deltaDist >= currentNode->xOffset)
		{
			deltaDist++;
		}
		currX -= deltaDist;
	}
	//Make adjustments to the index based on location (CAN BE DONE WITHOUT INDX AND INDY as in Main.c on microtaur code)
	indX = currX;
	indY = currY;
	if (indX > 0)
	{
		indX--;
	}
	if (indY > 0)
	{
		indY--;
	}

	indY += 8;
	indX += 8;

	//We come to a totally new tuple
	if(mazeArray[indX][indY] == NULL)
	{
		//BUILD A NEW NODE
		mazeArray[indX][indY] = buildNode(currX, currY);
		prevNode = currentNode;
		currentNode = mazeArray[indX][indY];
		//LINK THE NODE IN THE APPROPRIATE DIRECTION
		if(backPos == AI_WEST){			
			currentNode->west = prevNode;
			prevNode->east = currentNode;
		}
		else if(backPos == AI_NORTH){
			currentNode->north = prevNode;
			prevNode->south = currentNode;
		}
		else if(backPos == AI_EAST){
			currentNode->east = prevNode;
			prevNode->west = currentNode;
		}
		else if(backPos == AI_SOUTH){
			currentNode->south = prevNode;
			prevNode->north = currentNode;
		}

		//rate the directions given to us
		if(leftWall == 0){
			leftRating = rateDir(compass, NODE_LEFT, currX, currY);	
		}
		if(straightWall == 0){
			forwardRating = rateDir(compass, NODE_STRAIGHT, currX, currY);
		}
		if(rightWall == 0){
			rightRating = rateDir(compass, NODE_RIGHT, currX, currY);
		}

		choice = pickPath(leftRating, forwardRating, rightRating);
	}
	else{
		//ALWAYS LINK NODES: input mistakes will happen, but will take care of themselves
		prevNode = currentNode;
		currentNode = mazeArray[indX][indY];
		//LINK THE NODE IN THE APPROPRIATE DIRECTION
		if(backPos == AI_WEST){			
			currentNode->west = prevNode;
			prevNode->east = currentNode;
		}
		else if(backPos == AI_NORTH){
			currentNode->north = prevNode;
			prevNode->south = currentNode;
		}
		else if(backPos == AI_EAST){
			currentNode->east = prevNode;
			prevNode->west = currentNode;
		}
		else if(backPos == AI_SOUTH){
			currentNode->south = prevNode;
			prevNode->north = currentNode;
		}
		 
		currentNode->rating += 5;

		if(leftWall == 0){
			leftRating = rateVisitedNode(currentNode, compass, NODE_LEFT);
		}
		if(straightWall == 0){
			forwardRating = rateVisitedNode(currentNode, compass, NODE_STRAIGHT);
		}
		if(rightWall == 0){
			rightRating = rateVisitedNode(currentNode, compass, NODE_RIGHT);
		}

		choice = pickPath(leftRating, forwardRating, rightRating);

	}
	//make the node I chose the current node
	picked = (compass + choice)%4;

	if(picked == AI_WEST)
	{
		compass = AI_WEST;
	}
	else if(picked == AI_NORTH)
	{
		compass = AI_NORTH;
	}
	else if(picked == AI_EAST)
	{
		compass = AI_EAST;
	}
	else if(picked == AI_SOUTH)
	{
		compass = AI_SOUTH;
	}

	return choice;

}
//Helper method that rates a node based on the heading and the direction of the turn
int rateNode(int x, int y)
{
	if(x < 0)
	{
		x = x*-1;
	}
	if(y <0)
	{
		y = y * -1;
	}

	return x+y-1;
}

//Helper method that rates an adjacent square based on its location
unsigned char rateDir(int compass, int turnDir, int xx, int yy)
{
	int resultDir = mod4(compass+turnDir);

	int newX = xx;
	int newY = yy;
	if(resultDir == AI_WEST)
	{
		newX--;
		if(newX == 0)
		{
			newX = -1;
		}
	}
	else if(resultDir == AI_NORTH)
	{
		newY++;
		if(newY == 0)
		{
			newY = 1;
		}
	}
	else if(resultDir == AI_EAST)
	{
		newX++;
		if(newX == 0)
		{
			newX = 1;
		}
	}
	else if(resultDir == AI_SOUTH)
	{
		newY--;
		if(newY == 0)
		{
			newY = -1;
		}
	}

	return rateNode(newX, newY);
}

int pickPath(int leftL, int forwardF, int rightR){
	int choice = NODE_RIGHT;
	if(forwardF == 99 && leftL == 99 && rightR == 99)
	{
		currentNode->rating = 99;
		sawDeadEndLastTime = true;
		choice = NODE_BACK;
	}
	else if((leftL < rightR) && (leftL < forwardF))
	{
		choice = NODE_LEFT;
	}
	else if((rightR < leftL) && (rightR < forwardF))
	{
		choice = NODE_RIGHT;
	}
	else if((forwardF < leftL) && (forwardF < rightR))
	{
		choice = NODE_STRAIGHT;
	}
	else if((forwardF == leftL))
	{
		choice = NODE_LEFT;
	}
	else if (forwardF == rightR)
	{
		choice = NODE_RIGHT;
	}
	return choice;
}

//rates a node that the AI has seen before for a cardinal direction. If the direction has a node attached, it rates that, otherwise, it rates the square
//ratings for nodes that have been explored more have a higher (and therefore worse) rating
unsigned char rateVisitedNode(NavNode* node, int compass, int turnDir){
	int nodePos = mod4(compass+turnDir);
	unsigned char* badThings = NULL;
	if(nodePos == AI_WEST){
		if(node->west != NULL)
		{
			return node->west->rating + CHILD_PENALTY * (1 + numChildren(node->west, AI_EAST)); 
		}
		else{
			return rateDir(compass, turnDir, node->xOffset, node->yOffset);	
		}
	}
	else if(nodePos == AI_NORTH){
		if(node->north != NULL)
		{
			return node->north->rating + CHILD_PENALTY * (1 + numChildren(node->north, AI_SOUTH)); 
		}
		else{
			return rateDir(compass, turnDir, node->xOffset, node->yOffset);	
		}
	}
	else if(nodePos == AI_EAST){
		if(node->east != NULL)
		{
			return node->east->rating + CHILD_PENALTY * (1 + numChildren(node->east, AI_WEST)); 
		}
		else{
			return rateDir(compass, turnDir, node->xOffset, node->yOffset);	
		}
	}
	else if(nodePos == AI_SOUTH){
		if(node->south != NULL)
		{
			return node->south->rating + CHILD_PENALTY * (1 + numChildren(node->south, AI_NORTH)); 
		}
		else{
			return rateDir(compass, turnDir, node->xOffset, node->yOffset);	
		}
	}
	//something has gone terribly wrong, this should catch it
	while(true)
	{
		blinkTest();
	}
	return *badThings;
}

int numChildren(NavNode* check, int compass)
{
	int numChildren = 0;
	if(check->east != NULL && compass != AI_WEST)
	{
		numChildren = numChildren +1;
	}
	if(check->west != NULL && compass != AI_EAST)
	{
		numChildren = numChildren +1;
	}
	if(check->north != NULL && compass != AI_SOUTH)
	{
		numChildren = numChildren +1;
	}
	if(check->south != NULL && compass != AI_NORTH)
	{
		numChildren = numChildren +1;
	}
	return numChildren;

}

NavNode* buildNode(int currX, int currY)
{
	int newX = currX;
	int newY = currY;
	NavNode * nodePtr;
	NavNode newNode = {rateNode(newX, newY), newX, newY, 0, 0, 0, 0};

	emptyNodes[memIndex] = newNode;
	nodePtr = &emptyNodes[memIndex];
	memIndex++;
	return nodePtr;
}

void fwdNoJerk(void){
	PORTB=GO_LEFT;
	Delay10TCYx(DELAY_JERK);
	PORTB=GO_STRAIGHT;
}

unsigned char mod4(unsigned char val){
	while(val > 3){
		val -= 4;
	}
	return val;
}