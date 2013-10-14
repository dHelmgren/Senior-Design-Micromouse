#include "ai.h"

// See ai.h for comment
AI::AI(){
	compass = AI_NORTH;
	dummyVar = 0;
}

// See ai.h for comment
int AI::makeDecision(int deltaDist, bool left, bool straight, bool right, bool back){
	if(dummyVar == 0){
		dummyVar++;
		return AI_RIGHT;
	}
	else if(dummyVar == 1){
		dummyVar++;
		return AI_RIGHT;
	}
	else if(dummyVar == 2){
		dummyVar++;
		return AI_LEFT;
	}
	else if(dummyVar == 3){
		dummyVar++;
		return AI_BACK;
	}
	else if(dummyVar == 4){
		dummyVar++;
		return AI_RIGHT;
	}
	return AI_RIGHT;
}