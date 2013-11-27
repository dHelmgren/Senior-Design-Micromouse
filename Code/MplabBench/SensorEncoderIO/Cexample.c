#include "p18f27j13.h"

void main(void) {


//Code to set up the CCP6 and CCP7 to capture the pulse values entering RA6 and RA7 from the encoder.
// assume that the CCP are at RA6 for left and RA7 for right encoder.

	char distL0;
	char distL1;
	char distR0;
	char distR1;

	char dist0;
	char dist1;

	_asm
	BSF TRISA, 6,0		//RA6 is now an input
    BSF TRISA, 7,0		//RA7 is now an input
	MOVLW 0b00000000	//pg258 from text. 
				//CCP7 is based off of TMR1/TMR2
				//CCP6 is based off of TMR1/TMR2

	MOVWF CCPTMRS1,0		//
	MOVWF CCPTMRS2,0		//
	MOVLW 0b00000100	//pg257 from text.
				//compare on falling edge

	MOVWF CCP7CON,0		//
	MOVWF CCP6CON,0		//

	//at this point the set up should be complete. The set may need to be
	//modified for the actual PIC
	

	// here is the code to capture the values
	MOVFF CCPR6L, distL0		//
	MOVFF CCPR6H, distR0		//
	MOVFF CCPR7L, distL1		//
	MOVFF CCPR7H, distR1		//
	_endasm

///////////////////////////////////////////////////////////////////////////////////////
//Code to recieve data from a sensor and pass it to stand-in variables DIST0 and DIST1
//Let's pretend that Vo of the sensor is connected to RA2 of the PIC.



	_asm
	BSF TRISA, 1,0		//RA1 is now an input
	MOVLW 0b11000101	//pg507 from text. 
				//internal RC used for clock source
				//CHAN0 selected
				//GO/DONE cleared to keep the process from starting.
				//ADON set to turn the ADC on
	MOVWF ADCON0,0		//
	MOVLW 0b10000000	//pg508 from text. 
				//bits are right justified
				//internal RC used for clock source
				//all ANx ports set to recieve analog in
				//Vdd set to Vref+ and Vss set to Vref-
	MOVWF ADCON1,0		//

	//at this point the set up should be complete. The set up will need to be
	//modified for the actual PIC
	

REPEAT:	BSF ADCON0, 1,0		//start the conversion
WAIT:	BTFSC ADCON0, 1,0	//keep polling for end-of-conversion
	BRA WAIT		//
	MOVFF ADRESL, dist0	//
	MOVFF ADRESH, dist1	//
	BRA REPEAT		//
DONE:			//
	_endasm

}

