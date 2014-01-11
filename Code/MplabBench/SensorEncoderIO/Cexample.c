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
///end copy

void initial(void);
void initTimers(void);
void getClicks(void);
void clearClicks(void);
void pollClicks(void);
void initIR1(void);
void pollIR1(void);

// NOTE: MPLAB seems to need its declared variables to come first
/*
	NOTE: Comments in this code of the style seen here are used
	to note that there is a commented-out BRA instruction
	which is not desired for every simulation but will be necessary
	for Microtaur to function properly.
*/

void main(void) {

		initial();
		initTimers();
		getClicks();
		pollClicks();
		initIR1();
		pollIR1();
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
		_asm
		// set up the proper ports to be inputs
     	   	BSF TRISC, 0,0                //RC0 is now an input
    		BSF TRISC, 1,0                //RC1 is now an input		
		// clear the timers
		CLRF TMR1L,0
		CLRF TMR1H,0
		CLRF TMR3L,0
		CLRF TMR3H,0

		// RC0 == T1CKI, so no remapping is necessary
		// RC1 != T3CKI && RC1 == RP12, so remap T3CKI to RP12
		MOVLW 0b00001100			//setting RP12 as Timer3
		MOVWF RPINR6,0			

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
void pollClicks(void)
{
		clearClicks();
		_asm
		MOVLW 25// some constant which will have to be determined by measurement
L1:		CPFSGT TMR1L,0
/*
		BRA L1
*/
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
		BSF TRISA, 1,0          //RA1 is now an input
        MOVLW 0b11000101        //pg507 from text.
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
}