#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "p18f27j13.h"
#include "ai.h"

void makeDecision(float clicksTraveled, int left, int straight, int right, int back){
	if(left){
		printf("Turning left.\n");
	}
	else if(straight){
		printf("Going straight.\n");
	}
	else if(right){
		printf("Turning right.\n");
	}
	else{
		printf("Turning 180 degrees.\n");
	}
}