#include <p18f27j13.h>
#include <timers.h>
#include <delays.h>
#include <adc.h>
#include <math.h>

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
#define CLICKS_FOR_NINETY 	0xA2
#define CLICKS_FOR_180		0xB0

#define GO_STRAIGHT 		0b11111001
#define GO_BACKWARD 		0b11110110
#define BREAK 				0b11111111

#define CLICKS_PER_CM 	55  // Was messing up in hex
#define CLICKS_PER_HALF_CM 27
#define ONE_UNIT_CM 	18
#define STRAIGHT_IR_STOP 128
#define IS_DEAD_END 	1

// Poll 2: No wall means wall is farther than digital value 250
// TODO: We may need to adjust this value lower, depending on testing
#define NO_WALL_STRAIGHT 350
#define NO_WALL_LEFT 250
#define NO_WALL_LEFT_IN_TUPLE (NO_WALL_LEFT + 150)
#define NO_WALL_RIGHT 215
#define NO_WALL_RIGHT_IN_TUPLE (NO_WALL_RIGHT + 150)

// Poll 3: Move mouse forward 4 cm
#define LEAVE_UNIT 4

#define CONTINUE_TO_CENTER_R_FIRST 18  // Unsure about this value....................
#define CONTINUE_TO_CENTER_L_FIRST 13

// Number of clicks to back up when we have crashed
#define CRASH_BACK_UP	330

// Stop the mouse because it is about to run into the wall, since the
// IR sensor readouts are too high and close to the 3 cm max readout
#define STOP 940

// Constants to trigger push autocorrect
#define LR_DIFF				160 // Changed from 150
#define ERROR_CORRECT_L_1	810-DIFF
#define ERROR_CORRECT_R_1	(ERROR_CORRECT_L_1 - LR_DIFF)
#define ERROR_CORRECT_L_2	810-DIFF
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
#define CLICKS_FOR_AC_2		0x20//was 18

// How long to wait in between states of the code
#define DELAY 5000


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

#define NULL 0
#define BLANK {0,0,0,0,0,0,0}

/****** STRUCT ******/

//NavNode Struct

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
void goForward(int distance); // Step 2 of 4 step polling process
void ifAutocorrect(void);
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
unsigned char rateNode(char x, char y);
NavNode* buildNode(unsigned char turnDir, char currX, char currY);
int modFour(int val);

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

/* Timer values which indicate how long the mouse has been stuck in
   the for loop (and is therefore probably stuck in front a wall) */
unsigned char stuck0L = 0;
unsigned char stuck0H = 0;
unsigned char stuck1L = 0;
unsigned char stuck1H = 0;

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
	for(i = 0; i < 93; i++){emptyNodes[i] = blank;}
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

		Delay10TCYx(1);
		irCvtL = adConvert(LEFT_IR_SELECT);
		Delay10TCYx(1);
		irCvtR = adConvert(RIGHT_IR_SELECT);
		Delay10TCYx(1);
		irCvtS = adConvert(STRAIGHT_IR_SELECT);

		PORTB = BREAK;

		// Determine if the L or R sensors have seen the absence of
		// a wall. If they have, then initiate the 4-step polling process
		if(irCvtLP4 !=0 && irCvtRP4 !=0) {
			noWallL = NO_WALL_LEFT;
			noWallR = NO_WALL_RIGHT;
		} // This if statement should end here because it gives the mouse
		// permission to make a tuple immediately, which is extremely necessary
		// where in situations where there is a tuple after a tuple

		if(irCvtL <= noWallL||irCvtLP4-irCvtL >= 250){
			PORTB=BREAK;
			msDelay(DELAY);
			//blinkTest();
			PORTB=GO_STRAIGHT;
			leftWall = false;
			// Set this value so that we can how far to continue to center,
			// since the ir sensors are placed at different angles, the right
			// sensor will always recognize a tuple first
			firstNoWall = left;
			init4StepPoll(!IS_DEAD_END);
		}
		else if(irCvtR <= noWallR||irCvtRP4-irCvtR >= 250){
			PORTB=BREAK;
			msDelay(DELAY);
			//blinkTest();
			PORTB=GO_STRAIGHT;
			rightWall = false;
			firstNoWall = right;
			init4StepPoll(!IS_DEAD_END);		
		}
		else if(irCvtS >= STOP){
			PORTB=BREAK;
			msDelay(DELAY);
			straightWall = true;
			init4StepPoll(IS_DEAD_END);
		}

		// Determine if we need to autocorrect, and if we do, then do so.
		// Do not autocorrect if we haven't traveled far enough away from the
		// current tuple, because statistically, immediate post-turn autocorrect
		// has put the mouse at an incorrect angle (turns are very accurate)
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

void ifAutocorrect(void){
	// Determine if autocorrect is needed
	// If we're too close on the left side or too far on the right side
	if(irCvtL >= ERROR_CORRECT_L_1/* || (irCvtR <= PULL_CORRECT_R_1 && irCvtR >= NO_WALL_RIGHT)*/){
		if(irCvtL >= ERROR_CORRECT_L_2/* || (irCvtR <= PULL_CORRECT_R_2 && irCvtR >= NO_WALL_RIGHT)*/){
			autocorrect(left, CLICKS_FOR_AC_2, forward);
		}
		else{
			autocorrect(left, CLICKS_FOR_AC_1, forward);
		}
	}
	// Else if we're too close on the right side or too far on the left side
	else if(irCvtR >= ERROR_CORRECT_R_1/* || (irCvtL <= PULL_CORRECT_L_1 && irCvtL >= NO_WALL_LEFT)*/){
		if(irCvtR >= ERROR_CORRECT_R_2/* || (irCvtL <= PULL_CORRECT_L_2 && irCvtL >= NO_WALL_LEFT)*/){
			autocorrect(right, CLICKS_FOR_AC_2, forward);
		}
		else{
			autocorrect(right, CLICKS_FOR_AC_1, forward);
		}
	}
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
	// local variables
	char decision;
	unsigned char i = 0;

	if(!isDeadEnd){

		// Step 2
		if(firstNoWall == left){
			goForward(CONTINUE_TO_CENTER_L_FIRST*2);
		}
		else{
			goForward(CONTINUE_TO_CENTER_R_FIRST*2);
		}

		msDelay(DELAY);
		for(i=0;i<5;++i) {
			Delay10TCYx(10);
			irCvtS = adConvert(STRAIGHT_IR_SELECT);
		}
		if(irCvtS <= (int)NO_WALL_STRAIGHT){
			straightWall = false;
		}
		else{
			straightWall = true;
		}
	}

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

	PORTB=BREAK;
	msDelay(DELAY);

	// Step 4: Continue a little past current unit
	goForward((LEAVE_UNIT-1)*2+1);

	PORTB=BREAK;
	msDelay(DELAY);

	// Start polling process again
	PORTB=GO_STRAIGHT;
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
	while(cmTraveled < distance)
	{
PORTB=BREAK;
		irCvtL = adConvert(LEFT_IR_SELECT);
PORTB=GO_STRAIGHT;
		irCvtR = adConvert(RIGHT_IR_SELECT);

//ifAutocorrect();

		// Continue to gather information about our tuple, specifically if
		// there is a wall to the left or the right
		if(cmTraveled <= 5 && (distance == CONTINUE_TO_CENTER_R_FIRST || distance == CONTINUE_TO_CENTER_L_FIRST)){
			if((irCvtL <= NO_WALL_LEFT_IN_TUPLE || irCvtLP4 - irCvtL >= 250) && leftWall==true){
				PORTB=BREAK;
				msDelay(DELAY);
				//blinkTest();
				PORTB=GO_STRAIGHT;
				leftWall = false;
			}
			else if((irCvtR <= NO_WALL_RIGHT_IN_TUPLE || irCvtRP4 - irCvtR >= 250) && rightWall==true){
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


		if(TMR0L >= CLICKS_PER_HALF_CM){
			cmTraveled++;
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
	  	if(highestIrCvtS >= 550 && highestIrCvtS < STOP){// && irCvtS - prevIrCvtS > 0){
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
	unsigned char turnClicks = CLICKS_FOR_NINETY;
	unsigned char numTurns = 1;
	unsigned char i = 0;
	int countA = 0;
	int countB = 0;
    clearTimers();

	if(direction == NODE_LEFT){
		PORTB=GO_LEFT;
	}
	else if(direction == NODE_RIGHT){
		PORTB=GO_RIGHT;
	}
	else if(direction == NODE_BACK){
		numTurns = 2;
		turnClicks = CLICKS_FOR_180;
		PORTB=GO_RIGHT;
	}
	else{ //(direction == straight){
		PORTB=GO_STRAIGHT;
		return;
	}

	for(i = 0; i < numTurns; ++i){

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
	while(traveled1 >= 990){
		unitsTraveled++;
		traveled1 = traveled1 - 990;
	}
	if(unitsTraveled >= 445)
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

unsigned char makeDecision(unsigned char deltaDist, unsigned char leftWall, unsigned char straightWall, unsigned char rightWall){
	//node Rating variables
	unsigned char leftRating = 99;
	unsigned char rightRating = 99;
	unsigned char forwardRating = 99;
	
	//location variables
	char currX = 0;
	char currY = 0;
	char nodePos = 0;
	char backPos = mod4(compass + NODE_BACK);
	unsigned char choice = 0;
	
	currX = (char) (*currentNode).xOffset;
	currY = (char) (*currentNode).yOffset;
	
	

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
			mazeArray[currX+7][currY+7] = currentNode;
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
			mazeArray[currX+8][currY+8] = currentNode;
		}
	
		if (leftWall == false){
			int nodePos = mod4(compass + NODE_LEFT);
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
		if (straightWall == false){
			int nodePos = mod4(compass + NODE_STRAIGHT);
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
		if (rightWall == false){
			int nodePos = mod4(compass + NODE_RIGHT);
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

		//originally was here		int backPos = (compass + NODE_BACK) %4;

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

	
	if(mod4(compass + choice) == AI_WEST)
	{
		prevNode = currentNode;
		currentNode = (*currentNode).west;
		compass = AI_WEST;
	}
	else if(mod4(compass + choice) == AI_NORTH)
	{
		prevNode = currentNode;
		currentNode = (*currentNode).north;
		compass = AI_NORTH;
	}
	else if(mod4(compass + choice) == AI_EAST)
	{
		prevNode = currentNode;
		currentNode = (*currentNode).east;
		compass = AI_EAST;
	}
	else if(mod4(compass + choice) == AI_SOUTH)
	{
		prevNode = currentNode;
		currentNode = (*currentNode).south;// dont do if backtracking
		compass = AI_SOUTH;
	}

	return mod4(choice +1);

}

unsigned char rateNode(char x, char y){
	//Make sure we are using positive values
	if (x<0){
		x=x*-1;
	}
	if (y<0){
		y=y*-1;
	}

	return (unsigned char) x + y -1;
}

unsigned char mod4(unsigned char value){
	while(value > 4)
		value -= 4;
	return value;
}

NavNode* buildNode(unsigned char turnDir, char currX, char currY)
{
	char newX = currX;
	char newY = currY;
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

	newNode.rating = rateNode(newX, newY);
	newNode.xOffset = newX; 
	newNode.yOffset = newY;
	
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