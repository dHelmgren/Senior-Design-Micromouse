#include <p18f27j13.h>
#include <timers.h>
#include <delays.h>
#include <adc.h>

/****** MACROS ******/

//#define FIBROTAUR 1
#ifdef FIBROTAUR
#define DIFF 100
#define GO_RIGHT 			0b11101010
#define GO_LEFT 			0b11110101
#define BREAK_R_WHEEL		0b11111011
#define BREAK_L_WHEEL 		0b11111101
#define BACKWARD_R_WHEEL	0b11111110
#define BACKWARD_L_WHEEL	0b11110111
#else
#define DIFF 0
#define GO_LEFT 			0b11101010
#define GO_RIGHT 			0b11110101
#define BREAK_L_WHEEL		0b11111011
#define BREAK_R_WHEEL 		0b11111101
#define BACKWARD_L_WHEEL	0b11111110
#define BACKWARD_R_WHEEL	0b11110111
#endif

#define true    	1 
#define false   	0 
#define left		1
#define right		0
#define straight 	2
#define turnAround 	3
#define backward	0
#define forward		1

#define STRAIGHT_IR_SELECT 	0b00000001
#define LEFT_IR_SELECT 		0b00000101
#define RIGHT_IR_SELECT 	0b00001001
#define CLICKS_FOR_NINETY 	0x9C

#define GO_STRAIGHT 		0b11111001
#define GO_BACKWARD 		0b11110110
#define BREAK 				0b11111111

#define DELAY 			1000
#define CLICKS_PER_CM 	55  // Was messing up in hex
#define ONE_UNIT_CM 	18
#define STRAIGHT_IR_STOP 128
#define IS_DEAD_END 	1

// Poll 2: No wall means wall is farther than digital value 250
// TODO: We may need to adjust this value lower, depending on testing
#define NO_WALL_STRAIGHT 350
#define NO_WALL_LEFT 250
#define NO_WALL_RIGHT 175

// Poll 3: Move mouse forward 4 cm
#define LEAVE_UNIT 4

#define CONTINUE_TO_CENTER 14-1

// Stop the mouse because it is about to run into the wall, since the
// IR sensor readouts are too high and close to the 3 cm max readout
#define STOP 900

// Constants to trigger push autocorrect
#define LR_DIFF				150
#define ERROR_CORRECT_L_1	780-DIFF
#define ERROR_CORRECT_R_1	(ERROR_CORRECT_L_1 - LR_DIFF)
#define ERROR_CORRECT_L_2	780-DIFF
#define ERROR_CORRECT_R_2	(ERROR_CORRECT_L_2 - LR_DIFF)
#define ERROR_CORRECT_CAP_L	340-DIFF
#define ERROR_CORRECT_CAP_R	(ERROR_CORRECT_CAP_L - LR_DIFF)
#define RIGHT_BUFFER		30

// Constants to trigger pull autocorrect
#define PULL_CORRECT_L_1	480
#define PULL_CORRECT_R_1	(PULL_CORRECT_L_1 - LR_DIFF)//450-100
#define PULL_CORRECT_L_2	480
#define PULL_CORRECT_R_2	(PULL_CORRECT_L_2 - LR_DIFF)//380-100

// Constants to define how much to autocorrect
#define CLICKS_FOR_AC_1		0x20
#define CLICKS_FOR_AC_2		0x18

// Number of times we have iterated through the while loop without
// autocorrecting or making a tuple; therefore, we may be stuck!
#define STUCK 10000
#define STUCK_POLL 200

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

/****** FUNCTION DEFINITIONS ******/

unsigned int adConvert(unsigned char channel);
void autocorrect(unsigned char direction, unsigned char clicks, unsigned char howAutocorrect);
void blinkTest(void);
void driveTest(void);
void goForward(int distance); // Step 2 of 4 step polling process
void ifAutocorrect(void);
void ifSuicide(void);
void initial(void);
void init4StepPoll(unsigned char isDeadEnd);
unsigned char makeDecision(unsigned char leftWall, unsigned char straightWall, unsigned char rightWall);
void msDelay(unsigned int time);
void stopTest(void);
void turn(unsigned char direction); // Step 3

//AI Defs
int rateNode(int x, int y);
NavNode* buildNode(int turnDir, int currX, int currY);
int modFour(int val);

/****** STRUCT ******/

//NavNode Struct

typedef struct NavNode
{
	int rating;
	int xOffset;
	int yOffset;
	NavNode *west;
	NavNode *north;
	NavNode *east;
	NavNode *south;
}NavNode;

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

unsigned int stuck = 0;

//AI variables

int compass = AI_NORTH;

//Temporary stopgap to access the memory array
int memIndex = 0;

bool sawDeadEndLastTime = false;

NavNode* mazeArray[16][16];
NavNode root = {14, -8,-8, 0, 0, 0, 0};
NavNode* currentNode = &root;
NavNode* prevNode = &root;
NavNode emptyNodes[100] = null;//TODO: Is this legal?? 
NavNode blank = {0,0,0,0,0,0,0};
	for(i = 0; i < 100; i++)
	{
		emptyNodes[i] = blank;
	}
mazeArray[0][0] = &root;

/****** CODE ******/

#pragma code 

void main(void)
{
	// Initialization of constants
	unsigned char countA = 0;
	unsigned char countB = 0;
	unsigned char straightIR;
	unsigned char tempF;
	unsigned char tempL;
	unsigned char tempR;
	unsigned char leftIR;
	unsigned char rightIR;

	Nop();
	initial();
	
	/**** BEGIN! ****/
msDelay(10000);

	PORTB=GO_STRAIGHT;

	while(true) {
stuck++;
Delay10TCYx(1);
		irCvtL = adConvert(LEFT_IR_SELECT);
Delay10TCYx(1);
		irCvtR = adConvert(RIGHT_IR_SELECT);
Delay10TCYx(1);
		irCvtS = adConvert(STRAIGHT_IR_SELECT);

		// Determine if the L or R sensors have seen the absence of
		// a wall. If they have, then initiate the 4-step polling process
		if(irCvtLP4 !=0 && irCvtRP4 !=0) {
			noWallL = NO_WALL_LEFT;
			noWallR = NO_WALL_RIGHT;
		}
			if(irCvtL <= noWallL||irCvtLP4-irCvtL >= 250){
				PORTB=BREAK;
				blinkTest();
				PORTB=GO_STRAIGHT;
				leftWall = false;
				init4StepPoll(!IS_DEAD_END);
				stuck=0;
			}
			else if(irCvtR <= noWallR||irCvtRP4-irCvtR >= 250){
				PORTB=BREAK;
				blinkTest();
				PORTB=GO_STRAIGHT;
				rightWall = false;
				init4StepPoll(!IS_DEAD_END);
				stuck=0;		
			}
			else if(irCvtS >= STOP){
				PORTB=BREAK;
				straightWall = true;
				init4StepPoll(IS_DEAD_END);
				stuck=0;
			}

		// Determine if we are in a suicide run (aka running directly into
		// a wall)
		//ifSuicide();
		// Determine if we need to autocorrect, and if we do, then do so.
		ifAutocorrect();

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

void ifSuicide(void){
	// Determine which sensor is causing the suicide run, if we are in
	// a suicide situation at all
	if(irCvtS >= STOP+50 || stuck > STUCK){
		PORTB=GO_BACKWARD;
		//reset the timers
		TMR0L = 0;
		TMR0H = 0;
		while(TMR0L < STUCK_POLL){
			Nop();
		}
		PORTB=BREAK;
		// TODO: We will later pass in a distance traveled of zero so that
		// the agent can modify the last NavNode created and modify the
		// agent's choice.
		irCvtS = adConvert(STRAIGHT_IR_SELECT);
		if(irCvtS >= 600){
			turn(makeDecision(false, true, false));
		}
		stuck=0;
	}
	else if(irCvtL >= STOP+50){

	}
	else if(irCvtR >= STOP+50){

	}
	PORTB=GO_STRAIGHT;
}

void ifAutocorrect(void){
	// Determine if autocorrect is needed
	// If we're too close on the left side or too far on the right side
	if(irCvtL >= ERROR_CORRECT_L_1/* || (irCvtR <= PULL_CORRECT_R_1 && irCvtR >= NO_WALL_RIGHT)*/){
		if(irCvtL >= ERROR_CORRECT_L_2/* || (irCvtR <= PULL_CORRECT_R_2 && irCvtR >= NO_WALL_RIGHT)*/){
			autocorrect(left, CLICKS_FOR_AC_2, backward);
		}
		autocorrect(left, CLICKS_FOR_AC_1, forward);
	}
	// Else if we're too close on the right side or too far on the left side
	else if(irCvtR >= ERROR_CORRECT_R_1/* || (irCvtL <= PULL_CORRECT_L_1 && irCvtL >= NO_WALL_LEFT)*/){
		if(irCvtR >= ERROR_CORRECT_R_2/* || (irCvtL <= PULL_CORRECT_L_2 && irCvtL >= NO_WALL_LEFT)*/){
			autocorrect(right, CLICKS_FOR_AC_2, backward);
		}
		autocorrect(right, CLICKS_FOR_AC_1, forward);
	}
}

void autocorrect(unsigned char direction, unsigned char clicks, unsigned char howAutocorrect){
	PORTB=BREAK;
	stuck=0;
	TMR0L = 0;
  	TMR1L = 0;
  	TMR0H = 0;
  	TMR1H = 0;
	PORTB=GO_STRAIGHT;
  
  	if(direction == left){ // We need to turn a little right
  
  		if(howAutocorrect == backward){
  			PORTB=BACKWARD_R_WHEEL;
  		}
  		else{
  			PORTB=BREAK_R_WHEEL;
  		}
  		
		// Wait until the optical encoders reach at least "clicks"
  		while(TMR0L < clicks && TMR1L < clicks) {}
  	}//if
  
  	else{ // direction == right; we need to turn left
  
  		if(howAutocorrect == backward){
  			PORTB=BACKWARD_L_WHEEL;
  		}
  		else{
  			PORTB=BREAK_L_WHEEL;
  		}
  
		// Wait until the optical encoders reach at least "clicks"
  		while(TMR0L < clicks && TMR1L < clicks) {}
  	}//else
  
  	PORTB=BREAK;
  	msDelay(50);
  
  	PORTB=GO_STRAIGHT;
}//autocorrect

void init4StepPoll(unsigned char isDeadEnd){
	// local variables
	//int irCvtS = 0;
	char decision;
	unsigned char i = 0;

	if(!isDeadEnd){

		// Step 2
		goForward(CONTINUE_TO_CENTER);

PORTB=BREAK;
		msDelay(5000);
PORTB=BREAK;
		for(i=0;i<5;++i) {
			Delay10TCYx(10);
			irCvtS = adConvert(STRAIGHT_IR_SELECT);
			irCvtS = irCvtS + 1;
		}
		if(irCvtS <= (int)NO_WALL_STRAIGHT){
			straightWall = false;
		}
		else{
			straightWall = true;
		}
	}
PORTB=GO_STRAIGHT;
	// This if statement is just to be safe; it can be removed once
	// this information is verified
	if(isDeadEnd){
		leftWall = true;
		straightWall = true;
		rightWall = true;
	}

	// Step 3
	decision = makeDecision(leftWall, straightWall, rightWall);
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

	leftWall = true;
	rightWall = true;
	straightWall = false;

	TMR0L = 0;
	TMR1L = 0;

PORTB=BREAK;
	msDelay(5000);

	// Step 4: Continue a little past current unit
	// TODO: Perhaps looking for another way to leave the tupled unit, since
	// this sometimes take the robot too far and therefore it can't recognize
	// the next tuple (at worst case, the robot just thinks it is in a dead end
	// and turns 180 degrees around). Even LEAVE_UNIT-1 sometimes isn't enough
	// for mouse to leave tupled unit, so it sees the current tupled unit again,
	// but thinks it's the next tuple. Need to brainstorm ideas.
	goForward(LEAVE_UNIT-1);

	//msDelay(5000);

	// Start polling process again
	PORTB=GO_STRAIGHT;
}

void driveTest(void)
{	
	PORTB = GO_STRAIGHT; //Drive forward!
	msDelay(DELAY);
	msDelay(DELAY);
	msDelay(DELAY);
	turn(left);
	msDelay(DELAY);
	PORTB = GO_STRAIGHT; //Drive forward!
	msDelay(DELAY);
	PORTB = BREAK; //break
	msDelay(DELAY);
}

unsigned int adConvert(unsigned char channel)
{
	/*unsigned */int irValue;
	//set's the Analog input channel to the given input
	ADCON0 = 0;
	ADCON1 = 0b10100001; //0xa1
	ADCON0 = channel;
	//CHARGE the CAPACITOR!!!
	//must wait 2.45us
	//instead waiting 10ms 
			//Note: this timing can be played with to get a
			//more accurate reading
	Delay10TCYx(10);
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

void goForward(int distance)
{
	int prevIrCvtS = 0;
	TMR0L = 0;
	TMR0H = 0;
	TMR1L = 0;
	TMR1H = 0;
	PORTB=GO_STRAIGHT; //Drive forward
	cmTraveled = 0;
	while(cmTraveled < distance)
	{
		irCvtL = adConvert(LEFT_IR_SELECT);
		irCvtR = adConvert(RIGHT_IR_SELECT);
		if(cmTraveled <= 6){
			if(irCvtL <= NO_WALL_LEFT && irCvtLP4 - irCvtL >= 250){
				PORTB=BREAK;
				blinkTest();
				PORTB=GO_STRAIGHT;
				leftWall = false;
			}
			else if(irCvtR <= NO_WALL_RIGHT && irCvtRP4 - irCvtR >= 250){
				PORTB=BREAK;
				blinkTest();
				PORTB=GO_STRAIGHT;
				rightWall = false;
			}
			irCvtLP4 = irCvtLP3;
			irCvtRP4 = irCvtRP3;
			irCvtLP3 = irCvtLP2;
			irCvtRP3 = irCvtRP2;
			irCvtLP2 = irCvtLP1;
			irCvtRP2 = irCvtRP1;
			irCvtLP1 = irCvtL;
			irCvtRP1 = irCvtR;
		}//if
		if(TMR0L >= CLICKS_PER_CM){
			cmTraveled++;
			TMR0L = TMR0L - CLICKS_PER_CM;
			if(cmTraveled == distance){
				prevIrCvtS = adConvert(STRAIGHT_IR_SELECT);
			}
			if(distance != CONTINUE_TO_CENTER){
				//ifSuicide();
				// Determine if autocorrect is necessary
				//ifAutocorrect();
			}
		}
	}//while

	// We are continually moving closer to a wall, and haven't gotten too
  	// close yet. Therefore, keep going (using wall ahead as a marker) until
  	// we are in center of tupled unit.
PORTB=BREAK;
	Delay10KTCYx(1);
  	irCvtS = adConvert(STRAIGHT_IR_SELECT);
  	if(irCvtS >= 350){// && irCvtS - prevIrCvtS > 0){
		msDelay(5000);
  		PORTB=BREAK;
  		straightWall = true;
  		PORTB=GO_STRAIGHT;
  		while(irCvtS < STOP){
			Delay10TCYx(1);
  			irCvtS = adConvert(STRAIGHT_IR_SELECT);
  		}
  	}//if

	PORTB=BREAK; //stop the right wheel
	Nop();
	//msDelay(10);
	TMR0L = 0;
	TMR0H = 0;
}

void turn(unsigned char direction)
{
	// local variables
	unsigned char numTurns = 1;
	unsigned char i = 0;
	int countA = 0;
	int countB = 0;
    TMR0L = 0;
	TMR1L = 0;
	TMR0H = 0;
	TMR1H = 0;

	if(direction == left){
		PORTB=GO_LEFT;
	}
	else if(direction == right){
		PORTB=GO_RIGHT;
	}
	else if(direction == turnAround){
		numTurns = 2;
		PORTB=GO_RIGHT;
	}
	else if(direction == straight){
		PORTB=GO_STRAIGHT;
		return;
	}
	else{ // This case should never be reached
		PORTB=BREAK;
		while(1){blinkTest();}
		return;
	}

	for(i = 0; i < numTurns; ++i){

		Delay10TCYx(2000); //10ms delay

		while(countA < CLICKS_FOR_NINETY && countB < CLICKS_FOR_NINETY) {
			// check again and repeat.
			countA = TMR0L;
			countB = TMR1L;
		}
		countA = 0;
		countB = 0;
    	TMR0L = 0;
		TMR1L = 0;
		TMR0H = 0;
		TMR1H = 0;

		Delay10TCYx(2000); //10ms delay

		while(countA < CLICKS_FOR_NINETY && countB < CLICKS_FOR_NINETY) {
			// check again and repeat.
			countA = TMR0L;
			countB = TMR1L;
		}

		countA = 0;
		countB = 0;
    	TMR0L = 0;
		TMR1L = 0;
		TMR0H = 0;
		TMR1H = 0;
	}

	PORTB=BREAK; //break
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
	
	
		
	//Set RC0 and RC1 as inputs
	TRISC = 0b00000011;

	//Clear all port values
	PORTA = 0;
	PORTB = 0;
	PORTC = 0;
	TMR0L = 0;
	TMR0H = 0;
	TMR1L = 0;
	TMR1H = 0;
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

unsigned char makeDecision(unsigned char leftWall, unsigned char straightWall, unsigned char rightWall){
	//node Rating variables
	int leftRating = 99;
	int rightRating = 99;
	int forwardRating = 99;
	
	//location variables
	int currX = 0;
	int currY = 0;
	int nodePos = 0;
	int backPos = (compass + NODE_BACK) %4;
	int choice = 0;
	
	int currX = (int) (*currentNode).xOffset;
		int currY = (int) (*currentNode).yOffset;
	
	

		//travel adjustment for initial state
		if(currX != -8 && currY != -8)
		{
			deltaDist--;
		}
	
		//determine the proper deltaDistance so that we can get an accurate location in the maze
		if(compass == AI_NORTH)
		{
			//deals with crossing the origin, as each square is measured by the outermost corner
			if((*currentNode).yOffset < 0 && deltaDist > -(*currentNode).yOffset)
			{
				//if we cross the origin, we modify the distance to skip zero
				deltaDist++;
			}
			currY += deltaDist;
		}
		else if(compass == AI_SOUTH)
		{
			if((*currentNode).yOffset > 0 && deltaDist > (*currentNode).yOffset)
			{
				deltaDist++;
			}
			currY -= deltaDist;
		}
		else if(compass == AI_EAST)
		{
			if((*currentNode).xOffset < 0 && deltaDist > -(*currentNode).xOffset)
			{
				deltaDist++;
			}
			currX += deltaDist;
		}
		else if(compass == AI_WEST)
		{
			if((*currentNode).xOffset > 0 && deltaDist > (*currentNode).xOffset)
			{
				deltaDist++;
			}
			currX -= deltaDist;
		}

		//Fix the node's position to match its real world location and store it in the maze array.
		currentNode -> xOffset = currX;
		currentNode -> yOffset = currY;

		//Make adjustments to the index based on location
		//Add 8 so that the position can be indexed into the double array
		if(currX > 0 && currY > 0){
			//currX -1 currY -1
			mazeArray[indX+7][indY+7] = currentNode;
		}
		else if (currX > 0)
		{
			//currX -1
			mazeArray[currX+7][currY+8] = currentNode;
		}
		else if (currY > 0)
		{
			//currY -1
			mazeArray[currX+8][currY+7] = currentNode;
		}
		else{
			//No adjustments
			mazeArray[indX+8][indY+8] = currentNode;
		}
	
		if (!left){
			int nodePos = (compass + NODE_LEFT)%4;
			if (nodePos == AI_WEST){
				(*currentNode).west = buildNode(AI_WEST, currX, currY);
				leftRating = currentNode->west->rating;
			}
			else if (nodePos == AI_NORTH){
				(*currentNode).north = buildNode(AI_NORTH, currX, currY);
				leftRating = currentNode->north->rating;
			}
			else if (nodePos == AI_SOUTH){
				(*currentNode).south = buildNode(AI_SOUTH, currX, currY);
				leftRating = currentNode->south->rating;
			}
			else if (nodePos == AI_EAST){
				currentNode->east = buildNode(AI_EAST, currX, currY);
				leftRating = currentNode->east->rating;
			}
		}
		if (!straight){
			int nodePos = (compass + NODE_STRAIGHT)%4;
			if (nodePos == AI_WEST){
				currentNode->west = buildNode(AI_WEST, currX, currY);
				forwardRating = currentNode->west->rating;
			}
			else if (nodePos == AI_NORTH){
				currentNode->north = buildNode(AI_NORTH, currX, currY);
				forwardRating = currentNode->north->rating;
			}
			else if (nodePos == AI_SOUTH){
				currentNode->south = buildNode(AI_SOUTH, currX, currY);
				forwardRating = currentNode->south->rating;
			}
			else if (nodePos == AI_EAST){
				currentNode->east = buildNode(AI_EAST, currX, currY);
				forwardRating = currentNode->east->rating;
			}
		}
		if (!right){
			int nodePos = (compass + NODE_RIGHT)%4;
			if (nodePos == AI_WEST){
				currentNode->west = buildNode(AI_WEST, currX, currY);
				rightRating = currentNode->west->rating;
			}
			else if (nodePos == AI_NORTH){
				currentNode->north = buildNode(AI_NORTH, currX, currY);
				rightRating = currentNode->north->rating;
			}
			else if (nodePos == AI_SOUTH){
				currentNode->south = buildNode(AI_SOUTH, currX, currY);
				rightRating = currentNode->south->rating;
			}
			else if (nodePos == AI_EAST){
				currentNode->east = buildNode(AI_EAST, currX, currY);
				rightRating = currentNode->east->rating;
			}
		}

		int backPos = (compass + NODE_BACK) %4;

		if (backPos == AI_WEST){
			currentNode->west = prevNode;
		}
		else if (backPos == AI_NORTH){
			currentNode->north = prevNode;
		}
		else if (backPos == AI_SOUTH){
			currentNode->south = prevNode;
		}
		else if (backPos == AI_EAST){
			currentNode->east = prevNode;			
		}

	if(forwardRating == 99 && leftRating == 99 && rightRating == 99)
	{
		currentNode->rating = 99;
		//sawDeadEndLastTime = true;
		choice = NODE_BACK;
	}
	else if((leftRating < rightRating) && (leftRating < forwardRating))
	{
		choice = NODE_LEFT;
	}
	else if((rightRating < leftRating) && (rightRating < forwardRating))
	{
		choice = NODE_RIGHT;
	}
	else if((forwardRating < leftRating) && (forwardRating < rightRating))
	{
		choice = NODE_STRAIGHT;
	}
	else if((forwardRating == rightRating) || (forwardRating == leftRating))
	{
		choice = NODE_STRAIGHT;
	}
	else
	{
		choice = NODE_RIGHT;
	}

	
	if((compass + choice)%4 == AI_WEST)
	{
		prevNode = currentNode;
		currentNode = (*currentNode).west;
		compass = AI_WEST;
	}
	else if((compass + choice)%4 == AI_NORTH)
	{
		prevNode = currentNode;
		currentNode = (*currentNode).north;
		compass = AI_NORTH;
	}
	else if((compass + choice)%4 == AI_EAST)
	{
		prevNode = currentNode;
		currentNode = (*currentNode).east;
		compass = AI_EAST;
	}
	else if((compass + choice)%4 == AI_SOUTH)
	{
		prevNode = currentNode;
		currentNode = (*currentNode).south;// dont do if backtracking
		compass = AI_SOUTH;
	}

	return (choice +1)%4;


}

int rateNode(int x, int y){
	//Make sure we are using positive values
	if (x<0){
		x=x*-1;
	}
	if (y<0){
		y=y*-1;
	}

	return x + y -1;
}

buildNode(int turnDir, int currX, int currY)
{
	int newX = currX;
	int newY = currY;
	NavNode newNode = blank;
	NavNode* index = &emptyNodes[memIndex];

	if(turnDir == AI_WEST)
		{
			newX--;
			if(newX == 0)
			{
				newX = -1;
			}
		}
	else if(turnDir == AI_NORTH)
		{
			newY++;
			if(newY == 0)
			{
				newY = 1;
			}
		}
	else if(turnDir == AI_EAST)
		{
			newX++;
			if(newX == 0)
			{
				newX = 1;
			}
		}
	else if(turnDir == AI_SOUTH)
		{
			newY--;
			if(newY == 0)
			{
				newY = -1;
			}
		}

	newNode = {rateNode(newX, newY), newX, newY, 0, 0, 0, 0};
	
	if(turnDir == AI_WEST)
		{
			newNode.east = currentNode;
		}
	else if(turnDir == AI_NORTH)
		{
			newNode.south = currentNode;
		}
	else if(turnDir == AI_EAST)
		{
			newNode.west = currentNode;
		}
	else if(turnDir == AI_SOUTH)
		{
			newNode.north = currentNode;
		}

	emptyNodes[memIndex] = newNode;
	index = &emptyNodes[memIndex];
	memIndex++;
	return index;
}