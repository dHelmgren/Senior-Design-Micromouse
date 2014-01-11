#include "p18f27j13.h"
///copied from Janel's code
#define RC0 PORTCbits.RC0
#define RC1 PORTCbits.RC1
#define RB0 PORTBbits.RB0
#define RB1 PORTBbits.RB1
#define RB2 PORTBbits.RB2
#define RB3 PORTBbits.RB3
#define RB4 PORTBbits.RB4
#define true    1 
#define false   0 
#define W		0
#define F		1
///end copy

void initial(void);
void initTimers(void);
void getClicks(void);
void clearClicks(void);
void pollClicks(void);
void initIR1(void);
void pollIR1(void);
char clicksToUnits(char highByte, char lowByte);
void fourStepPollingSequence(char firstConstant, char secondConstant, char thirdConstant, char fourthConstant, char numQuarterTurns);
void checkAgainForWalls(void);
void haltMinitaur(void);
void initiateSpin(void);
void tupleToAgent(void);
		// note that the second step of the polling sequence does not require a constant.

// NOTE: MPLAB seems to need its declared variables to come first
/*
	NOTE: Comments in this code of the style seen here are used
	to note that there is a commented-out BRA instruction
	which is not desired for every simulation but will be necessary
	for Microtaur to function properly.
*/

//TODO write four step polling sequence

void main(void) {

		char sampleParam = 5;
		char numTurns = 2;

		initial();
		initTimers();
		getClicks();
		pollClicks();
		initIR1();
		pollIR1();
		clicksToUnits(1, sampleParam);
		fourStepPollingSequence(sampleParam,sampleParam,sampleParam,sampleParam, numTurns);
}

//
// Functions
//

/*
	Initialization code provided by Janel
*/
void initial(void)
{
	//set up the internal oscillator to 8MHz
	OSCCONbits.IRCF = 0b111;


	//set RB0, RB1, RB2, RB3 as outputs
	TRISBbits.TRISB0 = 0b0;
	TRISBbits.TRISB1 = 0b0;
	TRISBbits.TRISB2 = 0b0;
	TRISBbits.TRISB3 = 0b0;
	
	//Set RC0 and RC1 as outputs
	TRISCbits.TRISC0 = 0b0;
	TRISCbits.TRISC1 = 0b0;

	RC0 = 0b0;
	RC1 = 0b0;
	RB0 = 0b0;
	RB1 = 0b0;
	RB2 = 0b0;
	RB3 = 0b0;
}

/*
	Initialize the timers to count the pulses
	for going forward or backward
*/
void initTimers(void)
{
		_asm//TODO: Consider rewriting in C.
		// set up the proper ports to be inputs
        BSF TRISC, 0,0                //RC0 is now an input
    	BSF TRISC, 1,0                //RC1 is now an input		

		// clear the timers
		CLRF TMR1L,0
		CLRF TMR1H,0
		CLRF TMR3L,0
		CLRF TMR3H,0

		//TODO ensure mapping works.
		// RC0 == T1CKI, so no remapping is necessary
		// RC1 != T3CKI && RC1 == RP12, so remap T3CKI to RP12
		MOVLW 0b00001100			//setting RP12 as Timer3
		MOVWF RPINR6,0				//remap. pg164

		// initialize timer1
		MOVLW 0b10000111
									//external
									//1:1 prescale value
									//external
									//do not synchronize external clock input
									//Enables register read/write of timer in one 16-bit operation
									//timer1 enabled
		MOVWF T1CON,0

		// initialize timer3
		MOVLW 0b10000111
									//clock source through T3CKI
									//no prescale	
									//do not synchronize
									//Enables register read/write of timer in one 16-bit operationerations
									//Enables timer3
		MOVWF T3CON,0
		_endasm
}

/*
	Move the contents of the timers to respective variables.
*/
void getClicks(void)
{
// Here is the code to capture the values
// The values in WREG may then be moved to the appropriate location(s),
// which is likely hardcoded, before the next value is loaded to WREG.
// Here we just override sampleAddress four times

		//TODO insert proper variables
		char sampleAddress = 0;

		_asm
L1:		
		// the left encoder
 		MOVF TMR1L,0xE8,0//address FE8 == WREG
		MOVWF sampleAddress,0
		MOVF TMR1H,0xE8,0
		MOVWF sampleAddress,0
		// the right encoder
		MOVF TMR3L,0xE8,0
		MOVWF sampleAddress,0
		MOVF TMR3H,0xE8,0
		MOVWF sampleAddress,0
/*
		BRA L1
*/
		_endasm
}

/*
	Clear the timers.
*/
void clearClicks()
{
// clear timers 1 and 3
		_asm
		CLRF TMR1L,0
		CLRF TMR1H,0
		CLRF TMR3L,0
		CLRF TMR3H,0
		_endasm
}

/*
	Clear the timers and watch for the timers to reach
	a pre-set value. Note that this code is only set to watch
	the low byte of each timer.
*/
void pollClicks(void)//TODO this param should take a literal which will be the constant compared with.
{
//TODO consider polling on only one motor, or checking both simultaneously.
		clearClicks();
		_asm
		MOVLW 25// some constant which will have to be determined by measurement
L1:		CPFSGT TMR1L,0

		BRA L1

// we may choose to only moniter one of the encoders for simplicity, but both are coded here
L2:		CPFSGT TMR3L,0
/*
		BRA L2
*/
		_endasm	
		// both of the timers are greater than the hardcoded constant
}

/*
	Set up one IR and initialize the ADC. This code assumes that
	one of the IRs is connected to RA1.
*/
void initIR1(void)
{
		_asm
		BSF TRISA, 0,0          //RA0 is now an input
		BSF TRISA, 1,0          //RA1 is now an input
		BSF TRISA, 2,0          //RA2 is now an input
//TODO: complete the initialization for all of the IR sensors.
//        MOVLW 0b00 000101        //pg368 from text.
                                //internal RC used for clock source
                                //CHAN0 selected
                                //GO/DONE cleared to keep the process from starting.
                                //ADON set to turn the ADC on
        MOVWF ADCON0,0                //
        MOVLW 0b10000000        //pg508 from text.
                                //bits are right justified
                                //internal RC used for clock source
                                //all ANx ports set to recieve analog in
                                //Vdd set to Vref+ and Vss set to Vref-
        MOVWF ADCON1,0                //
		_endasm
}

void pollIR1(void)
{
        char dist0 = 0;
        char dist1 = 0;

		_asm
		BSF ADCON0, 1,0         //start the conversion
WAIT:   BTFSC ADCON0, 1,0       //keep polling for end-of-conversion
        BRA WAIT               
        MOVFF ADRESL, dist0     
        MOVFF ADRESH, dist1               
		_endasm

		//TODO write four linear functions with if statements
		// return char
}

char clicksToUnits(char highByte, char lowByte)// =)
{
		// given: 1 unit = 1 click
		// assume: 1 click == 1 cm (truth: 1 cm == 0.98 clicks)

		char numUnitSquares = 0;
		
		//NOTE: highByte will only be 0 or 1. If there is anything else, we have a problem!
		//TODO: Emilia, print to console if:
		//if (highbyte != 0 && highbyte != 1)

		if (highByte == 1) {
			numUnitSquares = numUnitSquares + 14;
			lowByte = lowByte + 4;
		}			
		
		// repeated subtraction
		while (lowByte >= 18) {
			lowByte = lowByte - 18;
			numUnitSquares = numUnitSquares + 1;
		}
		
		// round for the final value
		if (lowByte < 7) return numUnitSquares;
		return numUnitSquares + 1;
}

/*
	Call this function when it is discovered that a tuple is ahead.
*/
void fourStepPollingSequence(char firstConstant, char secondConstant, char thirdConstant, char fourthConstant, char numQuarterTurns)
{
	// Step one: We have seen that a tuple is ahead. Drive a short distance and check for 
	// walls/no_walls to ensure that the tuple will be made correctly.
	// We are still sending the same instructions to the motors, so we just poll for the
	// next distance. We will only check the encoder attached to TMR1.

		char TMR1L_future;
		char TMR1H_future = TMR1H;
		char turnCount;// for use in Step 3

		_asm
		// set up the following:
		// TMR1L_future = firstConstant + TMR1L
		// TMR1H_future = TMR1H + carry
		MOVF 	TMR1L,0xE8,0//0xE8 being WREG
		// assuming unsigned
		ADDWF 	firstConstant, 0xE8,0//WREG = firstConstant + TMR1L
		BNC 	L1
		INCF 	TMR1H_future, F,0// increase by one if carry
L1:		MOVWF	TMR1L_future,0//TMR1L_future = WREG
		_endasm

		// now commence the actual polling.
		while(1) if (TMR1L_future <= TMR1L) break;
		while(1) if (TMR1H_future <= TMR1H) break;

	// Step one is complete
		checkAgainForWalls();
	// Step two: continue to middle of next unit square
		
		TMR1H_future = TMR1H;

		_asm
		// set up the following:
		// TMR1L_future = firstConstant + TMR1L
		// TMR1H_future = TMR1H + carry
		MOVF 	TMR1L,0xE8,0//0xE8 being WREG
		// assuming unsigned
		ADDWF 	secondConstant, 0xE8,0//WREG = firstConstant + TMR1L
		BNC 	L2
		INCF 	TMR1H_future, F,0// increase by one if carry
L2:		MOVWF	TMR1L_future,0//TMR1L_future = WREG
		_endasm

		// now commence the actual polling.
		while(1) if (TMR1L_future <= TMR1L) break;
		while(1) if (TMR1H_future <= TMR1H) break;

	// Step two is complete
		haltMinitaur();		//NOTE: there will likely be other code in this area 
		tupleToAgent();		//to pass around parameters and do any other necessary funtions.
		initiateSpin();		//This is just a basic outline.
	// Step three: poll for the spin.

		turnCount = 0;
		while(numQuarterTurns != turnCount) {// for turning either one or two times.
			//TODO consider polling on only one motor, or checking both simultaneously.
					clearClicks();// we start counting the clicks again from zero.
					_asm
					MOVF thirdConstant, W, 0
L3:					CPFSGT TMR1L,0
					BRA L3
			
			// we may choose to only moniter one of the encoders for simplicity, but both are coded here
			/*
			L4:		CPFSGT TMR3L,0
					BRA L4
			*/
					_endasm	
					// both of the timers are greater than the hardcoded constant
					turnCount = turnCount + 1;
		}// end while

	// Step three is complete.
		// no operations between the end of the third poll and the beginning of the fourth poll.
	// Step four: drive forward a few clicks before we begin to read in the sensor data.

		clearClicks();// we start counting the clicks again from zero.
		_asm
		MOVF thirdConstant, W, 0
L5:		CPFSGT TMR1L,0
		BRA L5
		_endasm

	// Step four complete.
	// Return to buisiness as usual.
}

//TODO: Emilia fill. This function will scan the walls again and save the results to make the tuple later.
void checkAgainForWalls(void){}

//TODO: Janel fill. This function sends the commands to the motors to stop Minitaur.
void haltMinitaur(void){}

//TODO: Emilia fill. This function calles Devins agent. There will be parameters, but void for now.
void tupleToAgent(void){}

//TODO: Janel fill. This function sends the commands to the motors to spin Minitaur. Will likely have right/left parameters.
void initiateSpin(void){}
