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

void main(void) {
		
// for reasons unknown to me, MPLAB wanted these
// variable instansiations to be on top.
        char distL0;
        char distL1;
        char distR0;
        char distR1;

        char dist0;
        char dist1;

		initial();
		_asm
		CLRF distL0,0
		CLRF distL1,0
		CLRF distR0,0
		CLRF distR1,0
		CLRF dist0,0
		CLRF dist1,0
		_endasm

// Set up the configure bits as instructed by Janel:
//		_asm
//		MOVLW 0xAC
//		MOVWF 0x1FFF8,0
//		MOVLW 0xF7
//		MOVWF 0x1FFF9,0//problem
//		MOVLW 0xFB
//		MOVWF 0x1FFFA,0
//		MOVLW 0xFF
//		MOVWF 0x1FFFB,0
//		MOVWF 0x1FFFC,0
//		MOVWF 0x1FFFD,0
//		MOVWF 0x1FFFE,0
//		MOVWF 0x1FFFF,0
//		_endasm

// Code to set up the CCP6 and CCP7 to capture the pulse values entering RA6 and RA7 from the encoder.
// assume that the CCP are at RA6 for left and RA7 for right encoder.

        _asm
		// set up the proper ports to be inputs
        BSF TRISC, 0,0                //RC0 is now an input
    	BSF TRISC, 1,0                //RC1 is now an input		

		// clear the timers
		CLRF TMR1L,0
		CLRF TMR1H,0
		CLRF TMR3L,0
		CLRF TMR3H,0
/*
		// clear the registers which will be used to 
		// count the pulses
		CLRF CCPR4L,0
		CLRF CCPR4H,0
		CLRF CCPR7L,0
		CLRF CCPR7H,0
*/
/*
		// capture on falling edge
        MOVLW 0b00000100        
        MOVWF CCP7CON,0
        MOVWF CCP4CON,0                 //pg.257 from text
*/
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

		///testing
		// toggle RC0 and RC1 a few times
		BSF PORTC,0,0
		BCF PORTC,0,0
		BSF PORTC,0,0
		BCF PORTC,0,0
		BSF PORTC,0,0
		BCF PORTC,0,0
		BSF PORTC,0,0
		BCF PORTC,0,0

		BSF PORTC,1,0
		BCF PORTC,1,0
		BSF PORTC,1,0
		BCF PORTC,1,0
		BSF PORTC,1,0
		BCF PORTC,1,0

INFIN:	NOP
        // here is the code to capture the values
		// the left encoder
		MOVF TMR1L,0xE8,0//address FE8 == WREG
		MOVWF distL0,0
		MOVF TMR1H,0xE8,0
		MOVWF distL1,0
		// the right encoder
		MOVF TMR3L,0xE8,0
		MOVWF distR0,0
		MOVF TMR3H,0xE8,0
		MOVWF distR1,0

		BRA INFIN
/*
        MOVFW TMR1L
		MOVWF distL0,0
/*              //
        MOVFF TMR1H, distL1                // the left encoder
        MOVFF TMR3L, distR0                //
        MOVFF TMR3H, distR1                // the right encoder
/*
		// set timer1 to ccp7
		// set timer3 to ccp4
        MOVLW 0b00000010
        MOVWF CCPTMRS1,0                //pg.258 of text, pg.261 of text
*/
/*   /// old code /// 
		// initialize timer0
		MOVLW 0b10111000
									//timer0 enabled
									//16bit
									//external
									//falling edge
									//no prescaler
		MOVWF T0CON,0

		// initialize timer1
		MOVLW 0b10000101
									//external
									//1:1 prescale value
									//external
									//do not synchronize external clock input
									//read write of timer1 in two 8bit operations
									//timer1 enabled
		MOVWF T1CON,0

		// remap RC1 to T0CKI
		MOVLW 0b00001100			//setting RP12 as Timer0
		MOVWF RPINR4,0				//remap. pg164

        MOVLW 0b00000000        //pg258 from text.
                                //CCP7 is based off of TMR1/TMR2
                                //CCP6 is based off of TMR1/TMR2

        MOVWF CCPTMRS1,0                //
        MOVWF CCPTMRS2,0                //
        MOVLW 0b00000100        //pg257 from text.
                                //capture on falling edge

        MOVWF CCP7CON,0                //
        MOVWF CCP6CON,0                //

								// clear the registers which will be used to 
								// count the pulses
		CLRF CCPR6L,0
		CLRF CCPR6H,0
		CLRF CCPR7L,0
		CLRF CCPR7H,0
*/ /// end old code ///

        _endasm

///////////////////////////////////////////////////////////////////////////////////////
//Code to recieve data from a sensor and pass it to stand-in variables DIST0 and DIST1
//Let's pretend that Vo of the sensor is connected to RA2 of the PIC.



        _asm
        BSF TRISA, 1,0                //RA1 is now an input
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

        //at this point the set up should be complete. The set up will need to be
        //modified for the actual PIC
        

REPEAT: BSF ADCON0, 1,0                //start the conversion
WAIT:   BTFSC ADCON0, 1,0        //keep polling for end-of-conversion
        BRA WAIT                //
        MOVFF ADRESL, dist0        //
        MOVFF ADRESH, dist1        //
        BRA REPEAT                //
DONE:                        //
        _endasm

}