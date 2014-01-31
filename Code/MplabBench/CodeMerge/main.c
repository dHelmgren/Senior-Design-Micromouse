#include <p18f27j13.h>
#include <timers.h>
#include <adc.h>

/****** MACROS ******/

#define true    1 
#define false   0 
#define left	1
#define right	0
#define FRONT_IR_SELECT 0b00000001
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
#define PULSES_PER_CM 53  // Was messing up in hex
#define ONE_UNIT_CM 18
#define FRONT_IR_STOP 128

/****** FUNCTION DEFINITIONS ******/

void initial(void);
void blinkTest(void);
void driveTest(void);
unsigned int adConvert(unsigned char channel);
void msDelay(unsigned int time);
void ninetyDegreeTurnTest(unsigned char direction);
void stopTest(void);
char voltsToClicksTest(void);
void goForwardTest(void);

/****** GLOBAL VARIABLES ******/

/* Number of centimeters traveled since last time optical encoders
 * were cleared
 */
unsigned char cmTraveled = 0;

/****** CODE ******/

#pragma code 

void main(void)
{
	unsigned char countA;
	unsigned char countB;
	unsigned char frontIR;
	unsigned char tempF;
	unsigned char tempL;
	unsigned char tempR;
	unsigned char leftIR;
	unsigned char rightIR;
	int irConvert = 0;

	Nop();
	initial();
	blinkTest();
	countA = 0;
	countB = 0;

	// initiate 90degree spin test
	//ninetyDegreeSpinTest();
	// driveTest();

	// initiate stop test
	// this should cause minitaur to stop when it sees a wall directly ahead.
	//stopTest();


	PORTB=GO_FORWARD; //Drive forward

	while (irConvert <= 0x0040) {
		//Nop();
		PORTB = BREAK;
		//msDelay(1);
		irConvert = adConvert(FRONT_IR_SELECT);
		//msDelay(1);
		PORTB = GO_FORWARD;
	}
	PORTB = BREAK; //break
	msDelay(DELAY);

/*
	goForwardTest();
	ninetyDegreeTurnTest(right);
	cmTraveled = 0;
	goForwardTest();
*/
	while(true)
	{
		blinkTest();
		msDelay(2000);
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
	ninetyDegreeTurnTest(left);
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
	msDelay(1000);
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
/*
	irValue = irValue >>2;
	char charVal = (char)irValue;
	//return value
	return charValue*/
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

void goForwardTest(void)
{
	TMR0L = 0;
	TMR0H = 0;
	TMR1L = 0;
	TMR1H = 0;
	PORTB=GO_FORWARD; //Drive forward
	while(cmTraveled < ONE_UNIT_CM)
	{
		if(TMR0L >= PULSES_PER_CM){
			cmTraveled++;
			TMR0L = 0;
		}
	}

	PORTB=BREAK; //stop the right wheel
	//blinkTest();
	Nop();
	msDelay(1000);
	//blinkTest();
	TMR0L = 0;
	TMR0H = 0;
}

void ninetyDegreeTurnTest(unsigned char direction)
{
	char countA = 0;
	char countB = 0;

	//PORTB=GO_FORWARD; //Drive forward
	//msDelay(10000);
	//PORTB=BREAK; //break
	//msDelay(1000);

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

	msDelay(1000);
}

char voltsToClicksTest(void)
{// for now we will just set it up for the front IR.
// assume using 12 bits in adc, assume that the bits are right justified.
// TODO Janel, see above.
// eventually we will want to make this pass in a param to determine the IR
// THESE CONSTANTS WILL NEED TO BE TESTED AND REWRITTEN.
		char frontIRH;
		char frontIRL;
		frontIRH = adConvert(FRONT_IR_SELECT);
		frontIRL = ADRESL;
		if(frontIRH > 8 && frontIRL > 100) {
			//blinkTest();
			return 4;
		}
		if(frontIRH > 4 && frontIRL > 100) {
			//blinkTest();
			return 5;
		}
		if(frontIRH > 2 && frontIRL > 100) {
			//blinkTest();
			return 6;
		}
		if(frontIRH > 1 && frontIRL > 100) {
			//blinkTest();
			return 7;
		}
		return 8;
}