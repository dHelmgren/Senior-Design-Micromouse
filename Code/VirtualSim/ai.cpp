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
		return AI_LEFT;
	}
	else if(dummyVar == 4){
		dummyVar++;
		return AI_LEFT;
	}
	else if(dummyVar == 5){
		dummyVar++;
		return AI_BACK;
	}
	else if(dummyVar == 6){
		dummyVar++;
		return AI_LEFT;
	}
	else if(dummyVar == 7){
		dummyVar++;
		return AI_RIGHT;
	}
	else if(dummyVar == 8 || dummyVar == 9){
		dummyVar++;
		return AI_LEFT;
	}
	else if(dummyVar == 10 || dummyVar == 11){
		dummyVar++;
		return AI_STRAIGHT;
	}
	else if(dummyVar == 12 || dummyVar == 13 || dummyVar == 14){
		dummyVar++;
		return AI_LEFT;
	}
	else if(dummyVar == 15 || dummyVar == 16 || dummyVar == 17){
		dummyVar++;
		return AI_RIGHT;
	}
	else if(dummyVar == 18){
		dummyVar++;
		return AI_LEFT;
	}
	else if(dummyVar == 19 || dummyVar == 20){
		dummyVar++;
		return AI_STRAIGHT;
	}
	else if(dummyVar == 21){
		dummyVar++;
		return AI_LEFT;
	}
	else if(dummyVar == 22 || dummyVar == 23){
		dummyVar++;
		return AI_RIGHT;
	}
	else if(dummyVar == 24 || dummyVar == 25){
		dummyVar++;
		return AI_LEFT;
	}
	else if(dummyVar == 26){
		dummyVar++;
		return AI_RIGHT;
	}
	else if(dummyVar == 27 || dummyVar == 28){
		dummyVar++;
		return AI_LEFT;
	}
	else if(dummyVar == 29){
		dummyVar++;
		return AI_STRAIGHT;
	}

	return AI_RIGHT;
}