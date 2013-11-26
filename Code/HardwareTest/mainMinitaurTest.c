#include <p18f27j13.h>
#define RC0 PORTCbits.RC0
#define RC1 PORTCbits.RC1
#define RB0 PORTBbits.RB0
#define RB1 PORTBbits.RB1
#define RB2 PORTBbits.RB2
#define RB3 PORTBbits.RB3
#define RB4 PORTBbits.RB4
#define true    1 
#define false   0 

void initial(void);
void blinkTest(void);
void driveTest(void);
void msDelay(unsigned int time);

void main(void)
{
	initial();
	blinkTest();
}

void initial(void)
{
	//set RB0, RB1, RB2, RB3 as outputs
	TRISBbits.TRISB0 = 0;
	TRISBbits.TRISB1 = 0;
	TRISBbits.TRISB2 = 0;
	TRISBbits.TRISB3 = 0;
	
	//Set RC0 and RC1 as outputs
	TRISCbits.TRISC0 = 0;
	TRISCbits.TRISC1 = 0;

	RC0 = 0;
	RC1 = 0;
	RB0 = 0;
	RB1 = 0;
	RB2 = 0;
	RB3 = 0;
}

void driveTest(void)
{
	//Drive straight for 250ms
	RB0 = 1;
	RB1 = 0;
	RB2 = 1;
	RB3 = 0;
	msDelay(250);
	//drive back for 100ms
	RB0 = 0;
	RB1 = 1;
	RB2 = 0;
	RB3 = 1;
	msDelay(250);
	//Spin clockwise
	RB0 = 0;
	RB1 = 1;
	RB2 = 1;
	RB3 = 0;
	msDelay(250);
	//Spin counterclockwise
	RB0 = 1;
	RB1 = 0;
	RB2 = 0;
	RB3 = 1;
	msDelay(250);
	//Stop
	RB0 = 0;
	RB1 = 0;
	RB2 = 0;
	RB3 = 0;
}

void blinkTest(void)
{
	while(true)
	{
		//Blink LEDs on
		RC0 = 1;
		RC1 = 1;
		msDelay(10);
		//Blink LEDs off
		RC0 = 0;
		RC1 = 0;
		msDelay(10);
	}
}

void msDelay(unsigned int itime)
{
	unsigned int i;
	unsigned char j;
	for(i = 0;i < itime;i++)
	{
		for(j=0; j<165; j++)
		{
			Nop()
		}
	}
}