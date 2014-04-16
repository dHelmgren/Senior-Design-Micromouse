#include "ai.h"
#include "stdlib.h"
#include "stdio.h"


/* AI Constructor
* Initializes the MazeArray, the compass and current and previous node information.
*/
AI::AI(){
	compass = AI_NORTH;
	dummyVar = 0;
	memIndex = 0;
	//temp represents the root node, where zero is used as null pointer
	NavNode temp = {14, -8,-8, 0, 0, 0, 0};
	root = temp;
	currentNode = &root;
	prevNode = &root;
	int i = 0;
	int j = 0;
	//A blank node that will be copied to fill up the empty nodes
	NavNode blank = {0,0,0,0,0,0,0};
	for(i = 0; i < 100; i++)
	{
		//In order to access these blank nodes, we need to have all of the initialized somewhere. 
		//We store them in empty nodes so we can always reach their address
		emptyNodes[i] = blank;
	}
	for(i = 0; i < 16; i++){
		for(j = 0; j < 16; j++){
			//
			mazeArray[i][j] = NULL;
		}
	}

	//Set the root to the bottom left location of the maze.
	mazeArray[0][0] = &root;

	//The maze is stored in memory such that the location 0,0 is where the mouse starts, 
	//and the GUI shows this square as the bottom left square

	//Used to recognize dead ends.
	sawDeadEndLastTime = false;
	newLocation = true;

}


// makeDecision(deltaDist, left, straight, right, back)
// The function that the device will call when a node is reached.
// deltaDist :: The distance traveled since the last time make decision was called
// left, straight, right, back :: indicates the presence of a wall if true, the presence of an opening if false
int AI::makeDecision(int deltaDist, bool left, bool straight, bool right, bool back){
	//I expect information as (leftIsWall, centerIsWall, rightIsWall, distFromLast)

	// initializes ratings, where 99 is a wall
	int leftRating = 99;
	int rightRating = 99;
	int forwardRating = 99;
	// 5 means stuck. Not used in virtual environment
	int choice = 5;
	//initialize which direction (North South East or West) is currently reverse
	int backPos = (compass + NODE_BACK) %4;
	
	//Grabs our old x,y position 
	int currX = (int) currentNode->xOffset;
	int currY = (int) currentNode->yOffset;

	//determine the proper deltaDistance so that we can get an accurate location in the maze
	if(compass == AI_NORTH)
	{
		//these if's deal with crossing the origin, as each square is measured by the outermost corner
		if(currentNode->yOffset < 0 && deltaDist >= -currentNode->yOffset)
		{
			//if we cross the origin, we modify the distance to skip zero
			deltaDist++;
		}
		//then adjust to match our current location
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
	//currX and currY go from -8 to 8, skipping zero. This helps us calculate where the center of the maze is.
	//indX and indY go from 0 to 15. This helps us index into the array.
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

	//We come to a totally unseen tuple.
	if(mazeArray[indX][indY] == NULL)
	{
		//BUILD A NEW NODE
		mazeArray[indX][indY] = buildNode(currX, currY);
		prevNode = currentNode;
		currentNode = mazeArray[indX][indY];
		//LINK THE NODE IN THE APPROPRIATE DIRECTION
		if(backPos == AI_WEST){
			//our previous node is behind us (west in this case)
			currentNode->west = prevNode;
			//and our compass tells us we just went east
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
		//Picks the best option
		choice = pickPath(leftRating, forwardRating, rightRating);
	}
	else{
		//ALWAYS LINK NODES: input mistakes will happen, but if old input was bad, we've overwritten it.
		//if we got two sets of bad input, we still have the old links if we missed them this time.
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

		//Again, get the ratings
		if(!left){
			leftRating = rateVisitedNode(currentNode, compass, NODE_LEFT);
		}
		if(!straight){
			forwardRating = rateVisitedNode(currentNode, compass, NODE_STRAIGHT);
		}
		if(!right){
			rightRating = rateVisitedNode(currentNode, compass, NODE_RIGHT);
		}
		//make a choice
		choice = pickPath(leftRating, forwardRating, rightRating);

	}


	// DEBUG statements, in case you're getting lost.
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

	//set the node we picked to be the current node.
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

	//tell the environment or robot which direction to go.
	return (choice +1)%4;

}
//Helper method that spits back raw score for a node.
int AI::rateNode(int xx, int yy)
{
	return abs(xx)+abs(yy)-1;
}

//Helper method that calculates the position of a node based on direction, and calls a function to return raw score.
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

//chooses the optimal path out of the potential 4 options, based on node ratings.
int AI::pickPath(int left, int forward, int right){
	int choice = NODE_RIGHT;
	//Handle dead ends first.
	if(forward == 99 && left == 99 && right == 99)
	{
		currentNode->rating = 99;
		sawDeadEndLastTime = true;
		choice = NODE_BACK;
	}
	//left is best
	else if((left < right) && (left < forward))
	{
		choice = NODE_LEFT;
	}
	//right is best
	else if((right < left) && (right < forward))
	{
		choice = NODE_RIGHT;
	}
	//forward is best
	else if((forward < left) && (forward < right))
	{
		choice = NODE_STRAIGHT;
	}
	//forward and left tie.
	else if((forward == left))
	{
		choice = NODE_LEFT;
	}
	//forward and right tie
	else if (forward == right)
	{
		choice = NODE_RIGHT;
	}
	else if(left == right){
		//this is something that can be optimized to choose the node visited less.
		choice = NODE_RIGHT;
	}
	return choice;
}

//rates a node that the AI has seen before for a cardinal direction. If the direction has a node attached, it rates that, otherwise, it rates the square
//ratings for nodes that have been explored more have a higher (and therefore worse) rating
unsigned char AI::rateVisitedNode(NavNode* node, int compass, int turnDir){
	int nodePos = modFour(compass+turnDir);
	//determine the relative position of the node.
	if(nodePos == AI_WEST){
		if(node->west != NULL)
		{
			//if the node has been seen, penalize the node for the number of explored children it has
			return node->west->rating + CHILD_PENALTY * (1 + numChildren(node->west, compass)); 
		}
		else{
			//otherwise, just rate the node.
			return rateDir(compass, turnDir, node->xOffset, node->yOffset);	
		}
	}
	else if(nodePos == AI_NORTH){
		if(node->north != NULL)
		{
			return node->north->rating + CHILD_PENALTY * (1 + numChildren(node->north, compass)); 
		}
		else{
			return rateDir(compass, turnDir, node->xOffset, node->yOffset);	
		}
	}
	else if(nodePos == AI_EAST){
		if(node->east != NULL)
		{
			return node->east->rating + CHILD_PENALTY * (1 + numChildren(node->east, compass)); 
		}
		else{
			return rateDir(compass, turnDir, node->xOffset, node->yOffset);	
		}
	}
	else if(nodePos == AI_SOUTH){
		if(node->south != NULL)
		{
			return node->south->rating + CHILD_PENALTY * (1 + numChildren(node->south, compass)); 
		}
		else{
			return rateDir(compass, turnDir, node->xOffset, node->yOffset);	
		}
	}
	//something has gone terribly wrong, this should catch it
	unsigned char* badThings = NULL;
	return *badThings;
}

//Counts the number of nodes that have been explored from the current node.
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

//fills in an empty node, and does bookkeeping to load the next empty node.
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

//C and C++ have modulo. C18 does not.
int AI::modFour(int val){
	while(val > 3){
		val -= 4;
	}
	return val;
}