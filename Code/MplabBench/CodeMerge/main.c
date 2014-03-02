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
#define CLICKS_FOR_NINETY 	0xA0
#define CLICKS_FOR_180		0xB0

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
#define NO_WALL_LEFT_IN_TUPLE (NO_WALL_LEFT + 150)
#define NO_WALL_RIGHT 215
#define NO_WALL_RIGHT_IN_TUPLE (NO_WALL_RIGHT + 150)

// Poll 3: Move mouse forward 4 cm
#define LEAVE_UNIT 4

#define CONTINUE_TO_CENTER_R_FIRST 16  // Unsure about this value....................
#define CONTINUE_TO_CENTER_L_FIRST 12

// Number of clicks to back up when we have crashed
#define CRASH_BACK_UP	330

// Stop the mouse because it is about to run into the wall, since the
// IR sensor readouts are too high and close to the 3 cm max readout
#define STOP 940

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
#define CLICKS_FOR_AC_2		0x20//was 18


/****** FUNCTION DEFINITIONS ******/

unsigned int adConvert(unsigned char channel);
void autocorrect(unsigned char direction, unsigned char clicks, unsigned char howAutocorrect);
void blinkTest(void);
void clearTimers(void);
void goForward(int distance); // Step 2 of 4 step polling process
void ifAutocorrect(void);
void ifSuicide(void);
void initial(void);
void init4StepPoll(unsigned char isDeadEnd);
unsigned char makeDecision(unsigned char leftWall, unsigned char straightWall, unsigned char rightWall);
void msDelay(unsigned int time);
void stopTest(void);
void turn(unsigned char direction); // Step 3

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
unsigned char hardcodedAgent = 3;


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

	Nop();
	initial();

	/**** BEGIN! ****/
	msDelay(10000);

	PORTB=GO_STRAIGHT;

	while(true) {

	PORTB = GO_STRAIGHT;

		stuck0L = TMR0L;
		stuck1L = TMR1L;
		stuck0H = TMR0H;
		stuck1H = TMR1H;
		if(stuck1H >= 2 || stuck0H >= 2){
			waitToAutocorrect = false;
		}
		if(stuck1H >= 20 || stuck0H >= 20){
			PORTB=BREAK;
			msDelay(5000);
			ifSuicide();
			// TODO: Reset stuck and subtract the amount that we were stuck from traveled.
		}

PORTB=BREAK;
		Delay10TCYx(1);
		irCvtL = adConvert(LEFT_IR_SELECT);
PORTB=GO_STRAIGHT;
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
				blinkTest();
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
				blinkTest();
				PORTB=GO_STRAIGHT;
				rightWall = false;
				firstNoWall = right;
				init4StepPoll(!IS_DEAD_END);		
			}
			else if(irCvtS >= STOP){
				PORTB=BREAK;
				msDelay(5000);
				straightWall = true;
				init4StepPoll(IS_DEAD_END);
			}
		//}//if

		// Determine if we are in a suicide run (aka running directly into
		// a wall)
		//ifSuicide();

		// Determine if we need to autocorrect, and if we do, then do so.
		// Do not autocorrect if we haven't traveled far enough away from the
		// current tuple, because statistically, immediate post-turn autocorrect
		// has put the mouse at an incorrect angle (turns are very accurate)
		//if(waitToAutocorrect == false){
			ifAutocorrect();
		//}

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
	int tempInt = 0;
	int traveledInt0 = 0;	
	int traveledInt1 = 0;

	// Add the distance Microtaur has traveled thus far to the total 
	// traveled since the last tuple was made
	traveled0 += TMR0L;
	tempInt = (int)TMR0H;
	traveled0 += (tempInt << 8);
	tempInt = 0;
	traveled1 += TMR1L;
	tempInt = (int)TMR1H;
	traveled1 += (tempInt << 8);

	// Determine which sensor is causing the suicide run, if we are in
	// a suicide situation at all
	clearTimers();
	PORTB=GO_BACKWARD;
	while(traveledInt0 <= CRASH_BACK_UP || traveledInt1 <= CRASH_BACK_UP){
		traveledInt0 = 0;
		traveledInt1 = 0;
		tempInt = 0;

		traveledInt0 += (int)TMR0L;
		tempInt = (int)TMR0H;
		traveledInt0 += (tempInt << 8);
		tempInt = 0;

		traveledInt1 += (int)TMR1L;
		tempInt = (int)TMR1H;
		traveledInt1 += (tempInt << 8);
	}

	PORTB=BREAK;
	clearTimers();

	// if(nothing has changed && (irCvtR == r || "" == l || "" == s))
}

void ifAutocorrect(void){
	// Determine if autocorrect is needed
	// If we're too close on the left side or too far on the right side
	if(irCvtL >= ERROR_CORRECT_L_1/* || (irCvtR <= PULL_CORRECT_R_1 && irCvtR >= NO_WALL_RIGHT)*/){
		if(irCvtL >= ERROR_CORRECT_L_2/* || (irCvtR <= PULL_CORRECT_R_2 && irCvtR >= NO_WALL_RIGHT)*/){
			autocorrect(left, CLICKS_FOR_AC_2, forward);
			//goForward(2);
			//autocorrect(right, CLICKS_FOR_AC_2>>1, forward);
		}
		else{
			autocorrect(left, CLICKS_FOR_AC_1, forward);
		}
	}
	// Else if we're too close on the right side or too far on the left side
	else if(irCvtR >= ERROR_CORRECT_R_1/* || (irCvtL <= PULL_CORRECT_L_1 && irCvtL >= NO_WALL_LEFT)*/){
		if(irCvtR >= ERROR_CORRECT_R_2/* || (irCvtL <= PULL_CORRECT_L_2 && irCvtL >= NO_WALL_LEFT)*/){
			autocorrect(right, CLICKS_FOR_AC_2, forward);
			//goForward(2);
			//autocorrect(left, CLICKS_FOR_AC_2>>1, forward);
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
	traveled0 += TMR0L;
	traveled0 += (int)(TMR0H << 8);
	traveled1 += TMR1L;
	traveled1 += (int)(TMR1H << 8);

	// Reset the timers to count exactly how far Microtaur has traveled
	// in this method because these variables are "local"
	clearTimers();
Nop();
	PORTB=GO_STRAIGHT;
  
  	if(direction == left){ // We need to turn a little right
  
  		if(howAutocorrect == backward){
  			PORTB=BACKWARD_R_WHEEL;
  		}
  		else{
  			PORTB=BREAK_R_WHEEL;
  		}
  		
		// Wait until the optical encoders reach at least "clicks"
  		while(TMR0L < clicks && TMR0H < 255 || TMR1L < clicks && TMR1H < 255) {}
  	}//if
  
  	else{ // direction == right; we need to turn left
  
  		if(howAutocorrect == backward){
  			PORTB=BACKWARD_L_WHEEL;
  		}
  		else{
  			PORTB=BREAK_L_WHEEL;
  		}
  
		// Wait until the optical encoders reach at least "clicks"
  		while(TMR0L < clicks && TMR0H < 255 || TMR1L < clicks && TMR1H < 255) {}
  	}//else
  
	// Reset the timers so that we don't add in extra clicks that were
	// used for autocorrect and not the actual traversal
	clearTimers();

  	PORTB=BREAK;
  	msDelay(50);
  
  	PORTB=GO_STRAIGHT;
}//autocorrect

void init4StepPoll(unsigned char isDeadEnd){
	// local variables
	char decision;
	unsigned char i = 0;

	if(!isDeadEnd){

		// Step 2
		if(firstNoWall == left){
			goForward(CONTINUE_TO_CENTER_L_FIRST);
		}
		else{
			goForward(CONTINUE_TO_CENTER_R_FIRST);
		}

PORTB=BREAK;
		msDelay(5000);
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

	// Reset wall data
	leftWall = true;
	rightWall = true;
	straightWall = false;

	// Reset crash and burn, and click counting for the AI
	clearTimers();
	stuck0H = TMR0H;
	stuck1H = TMR1H;
	stuck0L = TMR0L;
	stuck1L = TMR1L;
	traveled0 = 0;
	traveled1 = 0;

PORTB=BREAK;
	msDelay(5000);

	// Step 4: Continue a little past current unit
	goForward(LEAVE_UNIT-1);

	// Do not autocorrect immediately after going forward some time. Wait
	// until confirmation that the next unit is not a tuple
	//waitToAutocorrect = true;
PORTB=BREAK;
msDelay(10000);

	// Start polling process again
	PORTB=GO_STRAIGHT;
}

unsigned char makeDecision(unsigned char leftWall, unsigned char straightWall, unsigned char rightWall){
	if(hardcodedAgent == 0 && !rightWall){
		hardcodedAgent++;
		return right;
	}
	else if(hardcodedAgent == 1 && !leftWall){
		hardcodedAgent++;
		return left;
	}
	else if(hardcodedAgent == 2 && !straightWall){
		hardcodedAgent++;
		return straight;
	}
	else if(hardcodedAgent == 3 && !rightWall){
		hardcodedAgent++;
		return right;
	}
	else if(hardcodedAgent == 4 && !leftWall){
		hardcodedAgent++;
		return left;
	}
	else if(hardcodedAgent == 5 && !leftWall){
		hardcodedAgent++;
		return left;
	}
	else if(hardcodedAgent == 6 && !rightWall){
		hardcodedAgent++;
		return right;
	}
	else if(hardcodedAgent == 7 && !rightWall){
		hardcodedAgent++;
		return right;
	}
	else if(hardcodedAgent == 8 && !leftWall){
		hardcodedAgent++;
		return left;
	}
	else if(hardcodedAgent == 9 && !leftWall){
		hardcodedAgent++;
		return left;
	}
	else if(hardcodedAgent == 10 && !rightWall){
		hardcodedAgent++;
		return right;
	}
	else if(hardcodedAgent == 11 && !rightWall){
		hardcodedAgent++;
		return right;
	}
	else if(hardcodedAgent == 12 && !rightWall){
		hardcodedAgent++;
		return right;
	}
	else if(hardcodedAgent == 13 && !leftWall){
		hardcodedAgent++;
		return left;
	}
	else if(hardcodedAgent == 14 && !leftWall){
		hardcodedAgent++;
		return left;
	}
	else if(hardcodedAgent == 15 && !rightWall){
		hardcodedAgent++;
		return right;
	}
	else if(hardcodedAgent == 16 && !rightWall){
		hardcodedAgent++;
		return right;
	}
	else if(hardcodedAgent == 17 && !straightWall){
		hardcodedAgent++;
		return straight;
	}
	else if(hardcodedAgent == 18 && !leftWall){
		hardcodedAgent++;
		return left;
	}
	else if(hardcodedAgent == 19 && !rightWall){
		hardcodedAgent++;
		return right;
	}
	else if(hardcodedAgent == 20 && !leftWall){
		hardcodedAgent++;
		return left;
	}
	else if(hardcodedAgent == 21 && !rightWall){
		hardcodedAgent++;
		return right;
	}
	else if(hardcodedAgent == 22 && !rightWall){
		hardcodedAgent++;
		return right;
	}
	else if(hardcodedAgent == 23 && !rightWall){
		hardcodedAgent++;
		return right;
	}
	else if(hardcodedAgent == 24 && !leftWall){
		hardcodedAgent++;
		return left;
	}
	else if(hardcodedAgent == 25 && !rightWall){
		hardcodedAgent++;
		return right;
	}
	else if(hardcodedAgent == 26 && !rightWall){
		hardcodedAgent++;
		return right;
	}
	else if(hardcodedAgent == 27 && !leftWall){
		hardcodedAgent++;
		return left;
	}
	else if(hardcodedAgent == 28 && !leftWall){
		hardcodedAgent++;
		return left;
	}
	else if(hardcodedAgent == 29 && !rightWall){
		hardcodedAgent++;
		return right;
	}
	else if(hardcodedAgent == 30 && !leftWall){
		hardcodedAgent++;
		return left;
	}
	else if(hardcodedAgent == 31 && !leftWall){
		hardcodedAgent++;
		return left;
	}
	else if(hardcodedAgent == 32 && !straightWall){
		hardcodedAgent++;
		return straight;
	}
	else if(hardcodedAgent == 33 && !rightWall){
		return right;
	}

	// Previous version of the hardcoded ai
	if(!leftWall){
		return left;
	}
	else if(!rightWall){
		return right;
	}
	else if(!straightWall){
		return straight;
	} 
	else{
		return turnAround;
	}
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
	traveled0 += TMR0L;
	traveled0 += (int)(TMR0H << 8);
	traveled1 += TMR1L;
	traveled1 += (int)(TMR1H << 8);

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
		// Continue to gather information about our tuple, specifically if
		// there is a wall to the left or the right
		if(cmTraveled <= 5 && (distance == CONTINUE_TO_CENTER_R_FIRST || distance == CONTINUE_TO_CENTER_L_FIRST)){
			if((irCvtL <= NO_WALL_LEFT_IN_TUPLE || irCvtLP4 - irCvtL >= 250) && leftWall==true){
				PORTB=BREAK;
				blinkTest();
				PORTB=GO_STRAIGHT;
				leftWall = false;
			}
			else if((irCvtR <= NO_WALL_RIGHT_IN_TUPLE || irCvtRP4 - irCvtR >= 250) && rightWall==true){
				PORTB=BREAK;
				blinkTest();
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

		else if(cmTraveled > 5 && cmTraveled <=7 && (distance == CONTINUE_TO_CENTER_R_FIRST || distance == CONTINUE_TO_CENTER_L_FIRST)){
			if((irCvtL <= (NO_WALL_LEFT_IN_TUPLE+50) || irCvtLP4 - irCvtL >= 250) && leftWall==true){
				PORTB=BREAK;
				blinkTest();
				PORTB=GO_STRAIGHT;
				leftWall = false;
			}
			else if((irCvtR <= (NO_WALL_RIGHT_IN_TUPLE+50) || irCvtRP4 - irCvtR >= 250) && rightWall==true){
				PORTB=BREAK;
				blinkTest();
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


		if(TMR0L >= CLICKS_PER_CM){
			cmTraveled++;
			TMR0L = TMR0L - CLICKS_PER_CM;
			tempIrCvtS = 0;
			Delay10TCYx(1);
			tempIrCvtS = adConvert(STRAIGHT_IR_SELECT);
			if(tempIrCvtS >= highestIrCvtS){
				if(tempIrCvtS >= STOP){
PORTB=BREAK;
					// Prevents Microtaur from crashing into a wall when the
					// constant CONTINUE_TO_CENTER may be too high
					break;
				}
				else if(tempIrCvtS < 1024){
					// We are getting a faulty reading from the adConvert method
					// so work around it by not letting the mouse get to the other
					// side of the voltage curve
					highestIrCvtS = 0;
					highestIrCvtS = tempIrCvtS;
				}
			}//if
Nop();
			if(distance != CONTINUE_TO_CENTER_L_FIRST && distance != CONTINUE_TO_CENTER_R_FIRST){
				//ifSuicide();
				// Determine if autocorrect is necessary
				//ifAutocorrect();
			}
		}//if
	}//while
/*
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
//			blinkTest();
			//msDelay(5000);
	  		straightWall = true;
	  		PORTB=GO_STRAIGHT;
	  		while(highestIrCvtS < STOP){
				// TODO: Add stuck code!!!
				Delay10TCYx(1);
				tempIrCvtS = adConvert(STRAIGHT_IR_SELECT);
				//if(tempIrCvtS < highestIrCvtS){
				//	break;
				//}
	  			highestIrCvtS = tempIrCvtS;
	  		}
			PORTB=BREAK;
//			blinkTest();
	  	}//if
		else{
			// Wall is at least one tuple away, so our tracking point is now
			// the presence of an wall or wall stub on one of the 
			
		}
	}//if
*/
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

	if(direction == left){
		PORTB=GO_LEFT;
	}
	else if(direction == right){
		PORTB=GO_RIGHT;
	}
	else if(direction == turnAround){
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
	TMR0H = 0;
	TMR0L = 0;
	TMR1H = 0;
	TMR1L = 0;
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