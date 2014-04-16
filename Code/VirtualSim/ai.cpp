#include "ai.h"
#include "stdlib.h"
#include "stdio.h"

// See ai.h for comment
// See ai.h for comment
AI::AI(){
	compass = AI_NORTH;
	dummyVar = 0;
	memIndex = 0;
	//NavNode tempArray[16][16] = {0};
	//mazeArray = tempArray;
	//these are what I am using to store relevant information until we talk about what will actually happenL
	//zero is used as 0 pointer
	NavNode temp = {14, -8,-8, 0, 0, 0, 0};
	root = temp;
	currentNode = &root;
	prevNode = &root;
	int i = 0;
	int j = 0;
	NavNode blank = {0,0,0,0,0,0,0};
	for(i = 0; i < 100; i++)
	{
		emptyNodes[i] = blank;
	}
	for(i = 0; i < 16; i++){
		for(j = 0; j < 16; j++){
			mazeArray[i][j] = NULL;
		}
	}

	mazeArray[0][0] = &root;

	// TEMPORARY dead end fix
	sawDeadEndLastTime = false;
	newLocation = true;
	
}


//TODO: We need to prune bad nodes
// See ai.h for comment
int AI::makeDecision(int deltaDist, bool left, bool straight, bool right, bool back){
	//I expect information as (leftIsWall, centerIsWall, rightIsWall, distFromLast)

	int leftRating = 99;
	int rightRating = 99;
	int forwardRating = 99;
	int choice = 5;
	int backPos = (compass + NODE_BACK) %4;

	int currX = (int) currentNode->xOffset;
	int currY = (int) currentNode->yOffset;
	//travel adjustment for initial state
	if(currX != -8 && currY != -8)
	{
		//deltaDist--;
	}
	
	//determine the proper deltaDistance so that we can get an accurate location in the maze
	if(compass == AI_NORTH)
	{
		//deals with crossing the origin, as each square is measured by the outermost corner
		if(currentNode->yOffset < 0 && deltaDist >= -currentNode->yOffset)
		{
			//if we cross the origin, we modify the distance to skip zero
			deltaDist++;
		}
		currY += deltaDist;
		
	}
	else if(compass == AI_SOUTH)
	{
		if(currentNode->yOffset > 0 && deltaDist >= currentNode->yOffset)
		{
			deltaDist++;
		}
		currY -= deltaDist;
	}
	else if(compass == AI_EAST)
	{
		if(currentNode->xOffset < 0 && deltaDist >= -currentNode->xOffset)
		{
			deltaDist++;
		}
		currX += deltaDist;
	}
	else if(compass == AI_WEST)
	{
		if(currentNode->xOffset > 0 && deltaDist >= currentNode->xOffset)
		{
			deltaDist++;
		}
		currX -= deltaDist;
	}
		

	//Make adjustments to the index based on location (CAN BE DONE WITHOUT INDX AND INDY as in Main.c on microtaur code)
	int indX = currX;
	int indY = currY;
	if (indX > 0)
	{
		indX--;
	}
	if (indY > 0)
	{
		indY--;
	}

	indY += 8;
	indX += 8;
		
	//We come to a totally new tuple
	if(mazeArray[indX][indY] == NULL)
	{
		//BUILD A NEW NODE
		mazeArray[indX][indY] = buildNode(currX, currY);
		prevNode = currentNode;
		currentNode = mazeArray[indX][indY];
		//LINK THE NODE IN THE APPROPRIATE DIRECTION
		if(backPos == AI_WEST){			
			currentNode->west = prevNode;
			prevNode->east = currentNode;
		}
		else if(backPos == AI_NORTH){
			currentNode->north = prevNode;
			prevNode->south = currentNode;
		}
		else if(backPos == AI_EAST){
			currentNode->east = prevNode;
			prevNode->west = currentNode;
		}
		else if(backPos == AI_SOUTH){
			currentNode->south = prevNode;
			prevNode->north = currentNode;
		}

		//rate the directions given to us
		if(!left){
			leftRating = rateDir(compass, NODE_LEFT, currX, currY);	
		}
		if(!straight){
			forwardRating = rateDir(compass, NODE_STRAIGHT, currX, currY);
		}
		if(!right){
			rightRating = rateDir(compass, NODE_RIGHT, currX, currY);
		}

		choice = pickPath(leftRating, forwardRating, rightRating);
	}
	else{
		//ALWAYS LINK NODES: input mistakes will happen, but will take care of themselves
		prevNode = currentNode;
		currentNode = mazeArray[indX][indY];
		//LINK THE NODE IN THE APPROPRIATE DIRECTION
		if(backPos == AI_WEST){			
			currentNode->west = prevNode;
			prevNode->east = currentNode;
		}
		else if(backPos == AI_NORTH){
			currentNode->north = prevNode;
			prevNode->south = currentNode;
		}
		else if(backPos == AI_EAST){
			currentNode->east = prevNode;
			prevNode->west = currentNode;
		}
		else if(backPos == AI_SOUTH){
			currentNode->south = prevNode;
			prevNode->north = currentNode;
		} 

		currentNode->rating += 2;

		if(!left){
			leftRating = rateVisitedNode(currentNode, compass, NODE_LEFT);
		}
		if(!straight){
			forwardRating = rateVisitedNode(currentNode, compass, NODE_STRAIGHT);
		}
		if(!right){
			rightRating = rateVisitedNode(currentNode, compass, NODE_RIGHT);
		}


		choice = pickPath(leftRating, forwardRating, rightRating);

	}
	

	
	/*printf("left: %d right: %d forward: %d Compass:", leftRating, rightRating, forwardRating);
	if (compass == AI_WEST)
	{
		printf(" west\n");
	}
	else if (compass == AI_NORTH)
	{
		printf(" north\n");
	}
	else if (compass == AI_EAST)
	{
		printf(" east\n");
	}
	else if (compass == AI_SOUTH)
	{
		printf(" south\n");
	}*/
	
	//make the node I chose the current node
	int picked = (compass + choice)%4;

	if(picked == AI_WEST)
	{
		compass = AI_WEST;
	}
	else if(picked == AI_NORTH)
	{
		compass = AI_NORTH;
	}
	else if(picked == AI_EAST)
	{
		compass = AI_EAST;
	}
	else if(picked == AI_SOUTH)
	{
		compass = AI_SOUTH;
	}

	return (choice +1)%4;

}
//Helper method that rates a node based on the heading and the direction of the turn
int AI::rateNode(int x, int y)
{
	return abs(x)+abs(y)-1;
}

//Helper method that rates an adjacent square based on its location
unsigned char AI::rateDir(int compass, int turnDir, int xx, int yy)
{
	int resultDir = modFour(compass+turnDir);

	int newX = xx;
	int newY = yy;
	if(resultDir == AI_WEST)
	{
		newX--;
		if(newX == 0)
		{
			newX = -1;
		}
	}
	else if(resultDir == AI_NORTH)
	{
		newY++;
		if(newY == 0)
		{
			newY = 1;
		}
	}
	else if(resultDir == AI_EAST)
	{
		newX++;
		if(newX == 0)
		{
			newX = 1;
		}
	}
	else if(resultDir == AI_SOUTH)
	{
		newY--;
		if(newY == 0)
		{
			newY = -1;
		}
	}

	return rateNode(newX, newY);
}

int AI::pickPath(int left, int forward, int right){
	int choice = NODE_RIGHT;
	if(forward == 99 && left == 99 && right == 99)
	{
		currentNode->rating = 99;
		sawDeadEndLastTime = true;
		choice = NODE_BACK;
	}
	else if((left < right) && (left < forward))
	{
		choice = NODE_LEFT;
	}
	else if((right < left) && (right < forward))
	{
		choice = NODE_RIGHT;
	}
	else if((forward < left) && (forward < right))
	{
		choice = NODE_STRAIGHT;
	}
	else if((forward == left))
	{
		choice = NODE_LEFT;
	}
	else if (forward == right)
	{
		choice = NODE_RIGHT;
	}
	return choice;
}

//rates a node that the AI has seen before for a cardinal direction. If the direction has a node attached, it rates that, otherwise, it rates the square
//ratings for nodes that have been explored more have a higher (and therefore worse) rating
unsigned char AI::rateVisitedNode(NavNode* node, int compass, int turnDir){
	int nodePos = modFour(compass+turnDir);
	if(nodePos == AI_WEST){
		if(node->west != NULL)
		{
			return node->west->rating + CHILD_PENALTY * (1 + numChildren(node->west, AI_EAST)); 
		}
		else{
			return rateDir(compass, turnDir, node->xOffset, node->yOffset);	
		}
	}
	else if(nodePos == AI_NORTH){
		if(node->north != NULL)
		{
			return node->north->rating + CHILD_PENALTY * (1 + numChildren(node->north, AI_SOUTH)); 
		}
		else{
			return rateDir(compass, turnDir, node->xOffset, node->yOffset);	
		}
	}
	else if(nodePos == AI_EAST){
		if(node->east != NULL)
		{
			return node->east->rating + CHILD_PENALTY * (1 + numChildren(node->east, AI_WEST)); 
		}
		else{
			return rateDir(compass, turnDir, node->xOffset, node->yOffset);	
		}
	}
	else if(nodePos == AI_SOUTH){
		if(node->south != NULL)
		{
			return node->south->rating + CHILD_PENALTY * (1 + numChildren(node->south, AI_NORTH)); 
		}
		else{
			return rateDir(compass, turnDir, node->xOffset, node->yOffset);	
		}
	}
	//something has gone terribly wrong, this should catch it
	unsigned char* badThings = NULL;
	return *badThings;
}

int AI::numChildren(NavNode* check, int compass)
{
	int numChildren = 0;
	if(check->east != NULL && compass != AI_WEST)
	{
		numChildren = numChildren +1;
	}
	if(check->west != NULL && compass != AI_EAST)
	{
		numChildren = numChildren +1;
	}
	if(check->north != NULL && compass != AI_SOUTH)
	{
		numChildren = numChildren +1;
	}
	if(check->south != NULL && compass != AI_NORTH)
	{
		numChildren = numChildren +1;
	}
	return numChildren;

}

NavNode* AI::buildNode(int currX, int currY)
{
	int newX = currX;
	int newY = currY;

	NavNode newNode = {rateNode(newX, newY), newX, newY, 0, 0, 0, 0};
	
	emptyNodes[memIndex] = newNode;
	NavNode* nodePtr = &emptyNodes[memIndex];
	memIndex++;
	return nodePtr;
}

int AI::modFour(int val){
	while(val > 3){
		val -= 4;
	}
	return val;
}

