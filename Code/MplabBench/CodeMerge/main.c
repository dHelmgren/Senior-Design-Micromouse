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
#define STRAIGHT_IR_SELECT 0b00000001
#define LEFT_IR_SELECT 0b00000101
#define RIGHT_IR_SELECT 0b00001001
#define CLICKS_FOR_90 0xB7
#define GO_LEFT 0b11101010
#define GO_RIGHT 0b11110101
#define GO_FORWARD 0b11111001
#define GO_BACKWARD 0b11110110
#define BREAK_R_WHEEL 0b11111101
#define BREAK 0b11111111
#define DELAY 1000
#define CLICKS_PER_CM 55  // Was messing up in hex
#define ONE_UNIT_CM 18
#define STRAIGHT_IR_STOP 128

// Poll 2: No wall means wall is farther than 11 cm
#define NO_WALL 280

// Poll 3: Move mouse forward 4 cm
#define LEAVE_UNIT 4

#define CONTINUE_TO_CENTER 14

/****** FUNCTION DEFINITIONS ******/

void initial(void);
void blinkTest(void);
void driveTest(void);
unsigned int adConvert(unsigned char channel);
void msDelay(unsigned int time);
void turn(unsigned char direction); // Step 3
void stopTest(void);
//char voltsToClicksTest(void);
void goForward(int distance); // Step 2 of 4 step polling process
void init4StepPoll(void);
unsigned char makeDecision(unsigned char leftWall, unsigned char straightWall, unsigned char rightWall);

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
	int irCvtL = 0; // Ir converted left value
	int irCvtR = 0;
	int irCvtS = 0;

	Nop();
	initial();
	

	/**** BEGIN! ****/

	PORTB=GO_FORWARD;

	while(true) {

		irCvtL = adConvert(LEFT_IR_SELECT);
		irCvtR = adConvert(RIGHT_IR_SELECT);
		irCvtS = adConvert(STRAIGHT_IR_SELECT);

		if(irCvtS >= 950){
			PORTB=BREAK;
			blinkTest();
			msDelay(10000);
			continue;
		}

		// Determine if the L or R sensors have seen the absence of
		// a wall. If they have, then initiate the 4-step polling process
		if(irCvtL <= NO_WALL){
			leftWall = false;
			init4StepPoll();
		}
		if(irCvtR <= NO_WALL){
			rightWall = false;
			init4StepPoll();
		}

		leftWall = true;
		rightWall = true;
		straightWall = true;

	}//while(true)

}//main

void init4StepPoll(void){
	// local variables
	int irCvtS = 0;
	char decision;

	// Step 2
	goForward(CONTINUE_TO_CENTER);

	msDelay(5000);

	irCvtS = adConvert(STRAIGHT_IR_SELECT);
	if(irCvtS <= NO_WALL){
		straightWall = false;
	}

	// TODO: Call agent
	decision = makeDecision(leftWall, straightWall, rightWall);

	// Step 3
	// TODO: modify turn to also turn 180 degrees
	turn(decision);

	msDelay(5000);

	// Step 4: Continue a little past current unit
	goForward(LEAVE_UNIT);

	msDelay(5000);

	// Start polling process again
	PORTB=GO_FORWARD;
}

unsigned char makeDecision(unsigned char leftWall, unsigned char straightWall, unsigned char rightWall){
	if(!leftWall){
		return left;
	}
	else if(!rightWall){
		return right;
	}
	else if(!straightWall){
		return straight;
	}
	else{ // This case should never be reached for the time being
		return turnAround;
	}
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

void driveTest(void)
{	
	PORTB = GO_FORWARD; //Drive forward!
	msDelay(DELAY);
	msDelay(DELAY);
	msDelay(DELAY);
	turn(left);
	msDelay(DELAY);
	PORTB = GO_FORWARD; //Drive forward!
	msDelay(DELAY);
	PORTB = BREAK; //break
	msDelay(DELAY);
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

void goForward(int distance)
{
	TMR0L = 0;
	TMR0H = 0;
	TMR1L = 0;
	TMR1H = 0;
	PORTB=GO_FORWARD; //Drive forward
	cmTraveled = 0;
	while(cmTraveled < distance)
	{
		if(TMR0L >= CLICKS_PER_CM){
			cmTraveled++;
			TMR0L = 0;
		}
	}

	PORTB=BREAK; //stop the right wheel
	Nop();
	//msDelay(10);
	TMR0L = 0;
	TMR0H = 0;
}

void turn(unsigned char direction)
{
	// local variables
	char countA = 0;
	char countB = 0;
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
	else{ // This case should never be reached
		return;
	}

	while(countA < CLICKS_FOR_90 && countB < CLICKS_FOR_90) {
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
	while(countA < CLICKS_FOR_90 && countB < CLICKS_FOR_90) {
		// check again and repeat.
		countA = TMR0L;
		countB = TMR1L;
	}
	PORTB=BREAK; //break
}

/*char voltsToClicksTest(void)
{// for now we will just set it up for the straight IR.
// assume using 12 bits in adc, assume that the bits are right justified.
// TODO Janel, see above.
// eventually we will want to make this pass in a param to determine the IR
// THESE CONSTANTS WILL NEED TO BE TESTED AND REWRITTEN.
		char straightIRH;
		char straightIRL;
		straightIRH = adConvert(STRAIGHT_IR_SELECT);
		straightIRL = ADRESL;
		if(straightIRH > 8 && straightIRL > 100) {
			//blinkTest();
			return 4;
		}
		if(straightIRH > 4 && straightIRL > 100) {
			//blinkTest();
			return 5;
		}
		if(straightIRH > 2 && frontIRL > 100) {
			//blinkTest();
			return 6;
		}
		if(frontIRH > 1 && frontIRL > 100) {
			//blinkTest();
			return 7;
		}
		return 8;
}*/