#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "p18f27j13.h"
#include "benchTests.h"
#include "decisionMaker.h"
#include "demo.h"

#define VERBOSE 0

#pragma config XINST = OFF // Disable extended CPU mode configuration
rom char * clickPtr = &leftStraightRightTurnPerfectClickArray;
ram unsigned char len = 0;
ram unsigned float left = 0.0;
ram unsigned float straight = 0.0;
ram unsigned float right = 0.0;
ram char lTemp[8];
ram char sTemp[8];
ram char rTemp[8];

void updateNextSensorOutputs()
{
	// If clickPtr has read an interrupt from the encoders
	// saying that the mouse is centered in the tupled unit
	if(*clickPtr == '$'){
		interruptCenteredInUnit();
		clickPtr++; // Advance over the '$'
		clickPtr++; // Advance over the '#'
	}
	else{
		// If clickPtr has gone off the end of the array
		if(*clickPtr != '('){
			printf("\nEnd of simulation\n\n");
			while(1);
		}
		clickPtr++; // Advance over the '('

		// Get the left number of clicks and convert to a float
		len = 0;
		while(*clickPtr != ','){
			lTemp[len] = *clickPtr;
			clickPtr++;
			len++;
		}
		lTemp[len] = '\0';
		left = atof(lTemp);
		clickPtr++; // Advance over the ','

		// Get the straight number of clicks and convert to a float
		len = 0;
		while(*clickPtr != ','){
			sTemp[len] = *clickPtr;
			clickPtr++;
			len++;
		}
		sTemp[len] = '\0';
		straight = atof(sTemp);
		clickPtr++; // Advance over the ','

		// Get the right number of clicks and convert to a float
		len = 0;
		while(*clickPtr != ')'){
			rTemp[len] = *clickPtr;
			clickPtr++;
			len++;
		}
		rTemp[len] = '\0';
		right = atof(rTemp);
		clickPtr++; // Advance over the ')'
		clickPtr++; // Advance over the '#'
		
		#if (VERBOSE == 1)
			printf("\nNew run!!!!:");
			printf("Flo: %d,", (int)(left*100));
			printf("%d,", (int)(straight*100));
			printf("%d\n", (int)(right*100));
		#endif
	}//else
}

float getLeftSensor(){
#if (VERBOSE == 1)
	printf("\nReturning left %d\n", (int)(left*100));
#endif
	return left;
}

float getStraightSensor(){
#if (VERBOSE == 1)
	printf("\nReturning straight %d\n", (int)(straight*100));
#endif
	return straight;
}

float getRightSensor(){
#if (VERBOSE == 1)
	printf("\nReturning right %d\n", (int)(right*100));
#endif
	return right;
}