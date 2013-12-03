#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "p18f27j13.h"
#include "decisionMaker.h"
#include "demo.h"

#define VERBOSE 1

#pragma config XINST = OFF // Disable extended CPU mode configuration
rom char clickArray[] = "(5.628,20.755,5.628)#(5.628,20.255,5.628)#(5.628,19.755,5.628)#(5.628,19.255,5.628)#(5.628,18.755,5.628)#(5.628,18.255,5.628)#(5.628,17.755,5.628)#(5.628,17.255,24.402)#(5.628,16.755,23.695)#(5.628,16.255,22.988)#(5.628,15.755,22.281)#(5.628,15.255,21.574)#(5.628,14.755,20.867)#(5.628,14.255,20.160)#(5.628,13.755,19.453)#(5.628,13.255,18.746)#(5.628,12.755,18.038)#(5.628,12.255,17.331)#(5.628,11.755,16.624)#(5.628,11.255,15.917)#(5.628,10.755,15.210)#(5.628,10.255,14.503)#(5.628,9.755,13.796)#(5.628,9.255,13.089)#(5.628,8.755,12.382)#(5.628,8.255,11.674)#(5.628,7.755,10.967)#(5.628,7.255,10.260)#(5.628,6.755,9.553)#(5.628,6.255,8.846)#(5.628,5.755,8.139)#(5.628,5.255,7.432)#(5.628,4.755,6.725)#(5.628,4.255,6.018)#(5.628,3.755,5.311)#(5.628,3.255,4.603)#(5.628,2.755,3.896)#(5.628,36.122,5.628)#(5.628,35.622,5.628)#(5.628,35.122,5.628)#(5.628,34.622,5.628)#(5.628,34.122,5.628)#(5.628,33.622,5.628)#";
rom char * clickPtr = &clickArray;
ram unsigned char len = 0;
ram unsigned float left = 0.0;
ram unsigned float straight = 0.0;
ram unsigned float right = 0.0;
ram char lTemp[8];
ram char sTemp[8];
ram char rTemp[8];

void updateNextSensorOutputs()
{
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