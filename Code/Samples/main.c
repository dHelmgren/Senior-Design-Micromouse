#include <p18f27j13.h>
#define true    1 
#define false   0 


void initial(void);
void blinkTest(void);
void driveTest(void);
void msDelay(unsigned int time);

#pragma code 

void main(void)
{
	unsigned char countA;
	unsigned char countB;
	Nop();
	initial();
	blinkTest();
	//PORTB=0b11101010; //Drive forward
	//msDelay(8000);
	//PORTB = 0b11111111; //break
	countA = 0;
	countB = 0;
 	while(true)
	{
		blinkTest();
		PORTB=0b11101010; //Drive forward
		msDelay(2000);
		PORTB = 0b11111111; //break
		countA = TMR0L;
		countB = TMR1L;
		msDelay(100);
		countA = TMR0L;
		countB = TMR1L;
		msDelay(100);
		countA = TMR0L;
		countB = TMR1L;
	}

	//blinkTest();
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
	//set RA0, RA1, RA2 as inputs
	//set RA3 and RA5 as outputs
	TRISA = 0b00000111;
	
		
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
	PORTB=0b11101010; //Drive forward
	msDelay(1000);
	PORTB = 0b11110101; //spin in a circle
	msDelay(1000);
	PORTB = 0b11110110; //Drive backwards
	msDelay(1000);
	PORTB = 0b11101010; //Drive forward
	msDelay(1000);
	PORTB = 0b11111111; //break
	msDelay(1000);
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