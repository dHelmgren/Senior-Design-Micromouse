#include <p18f27j13.h>
#include <timers.h>
#include <delays.h>
#include <adc.h>

/****** MACROS ******/

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
#define CLICKS_FOR_NINETY 	0xB0
#define CLICKS_FOR_AC_1		0x20	// Autocorrect
#define CLICKS_FOR_AC_2		0x18	// Autocorrect
#define GO_LEFT 			0b11101010
#define GO_RIGHT 			0b11110101
#define GO_STRAIGHT 		0b11111001
#define GO_BACKWARD 		0b11110110
#define BREAK_R_WHEEL 		0b11111101
#define BREAK_L_WHEEL		0b11111011
#define BREAK 				0b11111111
#define BACKWARD_L_WHEEL	0b11111110
#define BACKWARD_R_WHEEL	0b11110111

#define DELAY 			1000
#define CLICKS_PER_CM 	55  // Was messing up in hex
#define ONE_UNIT_CM 	18
#define STRAIGHT_IR_STOP 128
#define IS_DEAD_END 	1
#define ERROR_CORRECT_1	600	// Slight correction needed
#define ERROR_CORRECT_2	750	// Large correction needed
#define ERROR_CORRECT_CAP 340	// Ir sensor readout cap after turning; value < this constant means there is no wall
#define LR_DIFF 		150 //was 120
#define TURN_AC_BUFFER	120
#define RIGHT_BUFFER	50

// Poll 2: No wall means wall is farther than digital value 250
// TODO: We may need to adjust this value lower, depending on testing
#define NO_WALL 250

// Poll 3: Move mouse forward 4 cm
#define LEAVE_UNIT 4

#define CONTINUE_TO_CENTER 13

// Stop the mouse because it is about to run into the wall, since the
// IR sensor readouts are too high and close to the 3 cm max readout
#define STOP 950

/****** FUNCTION DEFINITIONS ******/

void initial(void);
void blinkTest(void);
void driveTest(void);
unsigned int adConvert(unsigned char channel);
void msDelay(unsigned int time);
void stopTest(void);
//char voltsToClicksTest(void);
void init4StepPoll(unsigned char isDeadEnd);
unsigned char makeDecision(unsigned char leftWall, unsigned char straightWall, unsigned char rightWall);
void autocorrect(unsigned char direction, unsigned char clicks, unsigned char howAutocorrect);
void turn(unsigned char direction); // Step 3
void goForward(int distance); // Step 2 & 4
void autocorrectTurn(void);

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

unsigned char dummy = true;

unsigned int countA = 0;
unsigned int countB = 0;
int irCvtL = 0; // Ir converted left value
int irCvtR = 0;
int irCvtS = 0;

/****** CODE ******/

#pragma code 

void main(void)
{
	// Initialization of constants
	unsigned char straightIR;
	unsigned char tempF;
	unsigned char tempL;
	unsigned char tempR;
	unsigned char leftIR;
	unsigned char rightIR;

	Nop();
	initial();
	

	/**** BEGIN! ****/

	msDelay(5000); // For Devon's sanity :)

	PORTB=GO_STRAIGHT;

	while(true) {

		irCvtL = adConvert(LEFT_IR_SELECT);
		irCvtR = adConvert(RIGHT_IR_SELECT);
		irCvtS = adConvert(STRAIGHT_IR_SELECT);

/* ADDED
		// Determine if the L or R sensors have seen the absence of
		// a wall. If they have, then initiate the 4-step polling process
		if(irCvtL <= NO_WALL){ // TODO: Change NO_WALL to have different constants per IR sensor
			leftWall = false;
		}
		if(irCvtR <= NO_WALL){
			rightWall = false;
		}

		// Take out emergency stop because tuple stops us before then
		if(irCvtS >= STOP && leftWall && rightWall){
			PORTB=BREAK;
			straightWall = true;
			init4StepPoll(IS_DEAD_END);
		}
		else if(!leftWall || !rightWall){
			init4StepPoll(!IS_DEAD_END);
		}//else
*///END ADDED

		// Determine if autocorrect is needed
		if(irCvtL >= ERROR_CORRECT_1){
			PORTB=BREAK;
			if(irCvtL >= ERROR_CORRECT_2){
				PORTB=BREAK;
				//autocorrect(left, CLICKS_FOR_AC_2, backward);
			}
			else{
				//autocorrect(left, CLICKS_FOR_AC_1, forward);
			}
			continue;
		}
		else if(irCvtR >= ERROR_CORRECT_1){
			if(irCvtR >= ERROR_CORRECT_2){
				PORTB=BREAK;
				//autocorrect(right, CLICKS_FOR_AC_2, backward);
			}
			else{
				//autocorrect(right, CLICKS_FOR_AC_1, forward);
			}
			continue;
		}//else if

		// Determine if the L or R sensors have seen the absence of
		// a wall. If they have, then initiate the 4-step polling process
		if(irCvtL <= NO_WALL){ // TODO: Change NO_WALL to have different constants per IR sensor
			leftWall = false;
		}
		if(irCvtR <= NO_WALL){
			rightWall = false;
		}

		// Take out emergency stop because tuple stops us before then
		if(irCvtS >= STOP && leftWall && rightWall){
			PORTB=BREAK;
			straightWall = true;
			init4StepPoll(IS_DEAD_END);
		}
		else if(!leftWall || !rightWall){
			init4StepPoll(!IS_DEAD_END);
		}//else

	}//while(true)

}//main

void autocorrect(unsigned char direction, unsigned char clicks, unsigned char howAutocorrect){
    TMR0L = 0;
	TMR1L = 0;
	TMR0H = 0;
	TMR1H = 0;
	countA = 0;
	countB = 0;

	if(direction == left){ // We need to turn a little right

		if(howAutocorrect == backward){
			PORTB=BACKWARD_R_WHEEL;
		}
		else{
			PORTB=BREAK_R_WHEEL;
		}

		while(countA < clicks && countB < clicks) {
			// check again and repeat.
			countA = TMR0L;
			countB = TMR1L;
		}
	}//if

	else{ // direction == right; we need to turn left

		if(howAutocorrect == backward){
			PORTB=BACKWARD_L_WHEEL;
		}
		else{
			PORTB=BREAK_L_WHEEL;
		}

		while(countA < clicks && countB < clicks) {
			// check again and repeat.
			countA = TMR0L;
			countB = TMR1L;
		}
	}//else

	PORTB=BREAK;
	msDelay(5000);

	PORTB=GO_STRAIGHT;

}// autocorrect

void init4StepPoll(unsigned char isDeadEnd){
	// local variables
	char decision;
	PORTB=BREAK;

	if(!isDeadEnd){

		// Step 2
		PORTB=GO_STRAIGHT;
		goForward(CONTINUE_TO_CENTER);

		msDelay(5000);

		// Once we have traveled to the middle of a tupled unit, then
		// recheck the sensors to make sure we know that the agent is
		// given the correct wall information.
		irCvtS = adConvert(STRAIGHT_IR_SELECT);
		if(irCvtS <= NO_WALL){
			straightWall = false;
		}
		else{
			straightWall = true;
		}
	}
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

	// We have finished our turn, and therefore we want to
	// reset our L, R, S wall information
	leftWall = true;
	rightWall = true;
	straightWall = false;

	if(!isDeadEnd){
		// Continuation of step 3
		//autocorrectTurn();
		msDelay(5000);

		// Step 4: Continue a little past current unit
		goForward(LEAVE_UNIT);

		msDelay(5000);
	}

	// Start polling process again
	PORTB=GO_STRAIGHT;
}

unsigned char makeDecision(unsigned char leftWall, unsigned char straightWall, unsigned char rightWall){
	if(!leftWall){
		return left;
	}
	else if(!straightWall){
		return straight;
	}
	else if(!rightWall){
		return right;
	}
	else{
		return turnAround;
	}
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
		if(TMR0L >= CLICKS_PER_CM){
			cmTraveled++;
			TMR0L = 0;

			// Check that we are not running into a wall while going
			// to center after seeing a tuple
			irCvtR = adConvert(RIGHT_IR_SELECT);
			irCvtL = adConvert(LEFT_IR_SELECT);

			// As we are moving forward to the middle of tupled unit, check
			// if there is no wall to the left and right and adjust variables
			// accordingly so we pass in the right into to the agent
			//PORTB=BREAK; // For DEBUG
			if(irCvtR <= NO_WALL){
				rightWall = false;
			}
			if(irCvtL <= NO_WALL){
				leftWall = false;
			}
			//PORTB=GO_STRAIGHT;	// FOR DEBUG

			// We are are about to hit a wall, autocorrect backing up, so we
			// don't it hit
			/*if(irCvtR >= STOP){
				autocorrect(right, 80, backward);
			}
			else if(irCvtL >= STOP){
				autocorrect(left, 80, backward);
			}*/
		}
	}
	// We are continually moving closer to a wall, and haven't gotten too
	// close yet. Therefore, keep going (using wall ahead as a marker) until
	// we are in center of tupled unit.
	irCvtS = adConvert(STRAIGHT_IR_SELECT);
	//PORTB=BREAK;
	if(irCvtS >= 250 && irCvtS - prevIrCvtS > 0){
		PORTB=BREAK;
		straightWall = true;
		PORTB=GO_STRAIGHT;
		while(irCvtS < STOP){
			irCvtS = adConvert(STRAIGHT_IR_SELECT);
		}
	}
	// This if statement should already be taken care of in init4StepPoll() method.
	else if(irCvtS >= NO_WALL){
		straightWall = false;
	}

	PORTB=BREAK; //stop the right wheel
	Nop();
	//msDelay(10);
	TMR0L = 0;
	TMR0H = 0;
}

void autocorrectTurn(void){
	unsigned char i = 0;
	unsigned char isCorrectingLeft = 0;
	unsigned char isCorrectingRight = 0;

	for(i = 0; i < 3; i++){
		msDelay(5000);

		// Determine if we need to autocorrect immediately after turn
		irCvtL = adConvert(LEFT_IR_SELECT);
		irCvtR = adConvert(RIGHT_IR_SELECT);
		
		isCorrectingLeft = irCvtR > ERROR_CORRECT_CAP && irCvtR > irCvtL-LR_DIFF+TURN_AC_BUFFER;
		isCorrectingRight = irCvtL > ERROR_CORRECT_CAP && irCvtL > LR_DIFF+TURN_AC_BUFFER+irCvtR+RIGHT_BUFFER;
		countA = 0;
		countB = 0;
	   	TMR0L = 0;
		TMR1L = 0;
		TMR0H = 0;
		TMR1H = 0;
	
		// If left IR sensor sees a wall and left wall is closer than right wall
		if(isCorrectingLeft && !isCorrectingRight){
			PORTB=GO_LEFT;
			while(countA < 10 && countB < 10) {
				// check again and repeat.
				countA = TMR0L;
				countB = TMR1L;
			}
			PORTB=BREAK; //break
			msDelay(200);
		}//if
		else if(isCorrectingRight && !isCorrectingLeft){
			// Autocorrect by turning a smidge right
			PORTB=GO_RIGHT;
			while(countA < 10 && countB < 10) {
				// check again and repeat.
				countA = TMR0L;
				countB = TMR1L;
			}
			PORTB=BREAK; //break
			msDelay(200);
		}//else if
	
		countA = 0;
		countB = 0;
	   	TMR0L = 0;
		TMR1L = 0;
		TMR0H = 0;
		TMR1H = 0;
	}
	// Now that we've autocorrected for our turn, make sure there is not
	// a wall in front of our nose (ie. less than 5 cm away)
	if(adConvert(STRAIGHT_IR_SELECT) > 950){
		PORTB = BREAK;
		while(true){} // TODO: Just for now, so we can see errors
	}
}

void turn(unsigned char direction)
{
	// local variables
	unsigned char numTurns = 1;
	unsigned char i = 0;
    TMR0L = 0;
	TMR1L = 0;
	TMR0H = 0;
	TMR1H = 0;
	countA = 0;
	countB = 0;

	if(direction == left){
		PORTB=GO_LEFT;
	}
	else if(direction == right){
		PORTB=GO_RIGHT;
	}
	else if(direction == turnAround){
		numTurns = 2;
		if(dummy){
			PORTB=GO_RIGHT;
		}
		else{
			PORTB=GO_LEFT;
		}
		dummy=!dummy;
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

	msDelay(500);
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
