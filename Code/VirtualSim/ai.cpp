#include "ai.h"
#include "stdlib.h"
#include "stdio.h"

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
	NavNode blank = {0,0,0,0,0,0,0};
	for(i = 0; i < 100; i++)
	{
		emptyNodes[i] = blank;
	}
	mazeArray[0][0] = &root;

	// TEMPORARY dead end fix
	sawDeadEndLastTime = false;
	// End TEMPORARY
	
}


//TODO: We need to prune bad nodes
//TODO: modularize code. A lot. Ask for help on this.
// See ai.h for comment
int AI::makeDecision(int deltaDist, bool left, bool straight, bool right, bool back){
	//Grope for info (DONE)

/*	if(left && straight && right)
	{
		// TEMPORARY dead end fix
		sawDeadEndLastTime = true;
		// End TEMPORARY
		return AI_BACK;
	}

	// TEMPORARY dead end fix
	if(sawDeadEndLastTime == true){
		printf("Saw dead end last time!\n");
		sawDeadEndLastTime = false;
		if(!left)
		{
			return AI_LEFT;
		}
		else if(!right)
		{
			return AI_RIGHT;
		}
		else
		{
			return AI_BACK;
		}
	}
	// End TEMPORARY
	*/
	//I expect information as (leftIsWall, centerIsWall, rightIsWall, distFromLast)

	int leftRating = 99;
	int rightRating = 99;
	int forwardRating = 99;

	if(!sawDeadEndLastTime){
		int currX = (int) (*currentNode).xOffset;
		int currY = (int) (*currentNode).yOffset;
	
	

		//travel adjustment for initial state
		if(currX != -8 && currY != -8)
		{
			deltaDist--;
		}
	
		//determine the proper deltaDistance so that we can get an accurate location in the maze
		if(compass == AI_NORTH)
		{
			//deals with crossing the origin, as each square is measured by the outermost corner
			if((*currentNode).yOffset < 0 && deltaDist > -(*currentNode).yOffset)
			{
				//if we cross the origin, we modify the distance to skip zero
				deltaDist++;
			}
			currY += deltaDist;
		}
		else if(compass == AI_SOUTH)
		{
			if((*currentNode).yOffset > 0 && deltaDist > (*currentNode).yOffset)
			{
				deltaDist++;
			}
			currY -= deltaDist;
		}
		else if(compass == AI_EAST)
		{
			if((*currentNode).xOffset < 0 && deltaDist > -(*currentNode).xOffset)
			{
				deltaDist++;
			}
			currX += deltaDist;
		}
		else if(compass == AI_WEST)
		{
			if((*currentNode).xOffset > 0 && deltaDist > (*currentNode).xOffset)
			{
				deltaDist++;
			}
			currX -= deltaDist;
		}

		//Fix the node's position to match its real world location and store it in the maze array.
		(*currentNode).xOffset = currX;
		(*currentNode).yOffset = currY;
		printf("X: %d Y: %d", currX, currY);

		//Make adjustments to the index based on location
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

		//Add 8 so that the position can be indexed into the double array
		mazeArray[indX+8][indY+8] = currentNode;
	
		if (!left)
		{
			int nodePos = (compass + NODE_LEFT)%4;
			if (nodePos == AI_WEST)
			{
				(*currentNode).west = buildNode(AI_WEST, currX, currY);
				leftRating = (*(*currentNode).west).rating;
			}
			else if (nodePos == AI_NORTH)
			{
				(*currentNode).north = buildNode(AI_NORTH, currX, currY);
				leftRating = (*(*currentNode).north).rating;
			}
			else if (nodePos == AI_SOUTH)
			{
				(*currentNode).south = buildNode(AI_SOUTH, currX, currY);
				leftRating = (*(*currentNode).south).rating;
			}
			else if (nodePos == AI_EAST)
			{
				(*currentNode).east = buildNode(AI_EAST, currX, currY);
				leftRating = (*(*currentNode).east).rating;
			}
		}
		if (!straight)
		{
			int nodePos = (compass + NODE_STRAIGHT)%4;
			if (nodePos == AI_WEST)
			{
				(*currentNode).west = buildNode(AI_WEST, currX, currY);
				forwardRating = (*(*currentNode).west).rating;
			}
			else if (nodePos == AI_NORTH)
			{
				(*currentNode).north = buildNode(AI_NORTH, currX, currY);
				forwardRating = (*(*currentNode).north).rating;
			}
			else if (nodePos == AI_SOUTH)
			{
				(*currentNode).south = buildNode(AI_SOUTH, currX, currY);
				forwardRating = (*(*currentNode).south).rating;
			}
			else if (nodePos == AI_EAST)
			{
				(*currentNode).east = buildNode(AI_EAST, currX, currY);
				forwardRating = (*(*currentNode).east).rating;
			}
		}
		if (!right)
		{
			int nodePos = (compass + NODE_RIGHT)%4;
			if (nodePos == AI_WEST)
			{
				(*currentNode).west = buildNode(AI_WEST, currX, currY);
				rightRating = (*(*currentNode).west).rating;
			}
			else if (nodePos == AI_NORTH)
			{
				(*currentNode).north = buildNode(AI_NORTH, currX, currY);
				rightRating = (*(*currentNode).north).rating;
			}
			else if (nodePos == AI_SOUTH)
			{
				(*currentNode).south = buildNode(AI_SOUTH, currX, currY);
				rightRating = (*(*currentNode).south).rating;
			}
			else if (nodePos == AI_EAST)
			{
				(*currentNode).east = buildNode(AI_EAST, currX, currY);
				rightRating = (*(*currentNode).east).rating;
			}
		}

		int backPos = (compass + NODE_BACK) %4;

		if (backPos == AI_WEST)
			{
				(*currentNode).west = prevNode;
			}
			else if (backPos == AI_NORTH)
			{
				(*currentNode).north = prevNode;
			}
			else if (backPos == AI_SOUTH)
			{
				(*currentNode).south = prevNode;
			}
			else if (backPos == AI_EAST)
			{
				(*currentNode).east = prevNode;			
			}
	}
	else{
		int backDir = modFour(compass+NODE_BACK);

		if(compass == AI_WEST){
			forwardRating = (*(*currentNode).west).rating;
			leftRating = (*(*currentNode).south).rating;
			rightRating = (*(*currentNode).north).rating;
		}
		else if(compass == AI_NORTH){
			forwardRating = (*(*currentNode).north).rating;
			leftRating = (*(*currentNode).west).rating;
			rightRating = (*(*currentNode).east).rating;
		}
		else if(compass == AI_EAST){
			forwardRating = (*(*currentNode).east).rating;
			leftRating = (*(*currentNode).north).rating;
			rightRating = (*(*currentNode).south).rating;
		}
		else if(compass == AI_SOUTH){
			if ((*currentNode).south != NULL){
				forwardRating = (*(*currentNode).south).rating;
			}
			if ((*currentNode).east != NULL){
				leftRating = (*(*currentNode).east).rating;
			}
			if ((*currentNode).west != NULL){
				rightRating = (*(*currentNode).west).rating;
			}
		}
		sawDeadEndLastTime = false;
	}
	
	
	int choice;

	printf("left: %d right: %d forward: %d Compass:", leftRating, rightRating, forwardRating);
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
	}

	if(forwardRating == 99 && leftRating == 99 && rightRating == 99)
	{
		(*currentNode).rating = 99;
		sawDeadEndLastTime = true;
		choice = NODE_BACK;
	}
	else if((leftRating < rightRating) && (leftRating < forwardRating))
	{
		choice = NODE_LEFT;
	}
	else if((rightRating < leftRating) && (rightRating < forwardRating))
	{
		choice = NODE_RIGHT;
	}
	else if((forwardRating < leftRating) && (forwardRating < rightRating))
	{
		choice = NODE_STRAIGHT;
	}
	else if((forwardRating == rightRating) || (forwardRating == leftRating))
	{
		choice = NODE_STRAIGHT;
	}
	else
	{
		choice = NODE_RIGHT;
	}

	//make the node I chose the current node
	int picked = (compass + choice)%4;

	if(picked == AI_WEST)
	{
		prevNode = currentNode;
		currentNode = (*currentNode).west;
		compass = AI_WEST;
	}
	else if(picked == AI_NORTH)
	{
		prevNode = currentNode;
		currentNode = (*currentNode).north;
		compass = AI_NORTH;
	}
	else if(picked == AI_EAST)
	{
		prevNode = currentNode;
		currentNode = (*currentNode).east;
		compass = AI_EAST;
	}
	else if(picked == AI_SOUTH)
	{
		prevNode = currentNode;
		currentNode = (*currentNode).south;
		compass = AI_SOUTH;
	}

	return (choice +1)%4;

}
//Helper method that rates a node based on the heading and the direction of the turn
int AI::rateNode(int x, int y)
{
	return abs(x)+abs(y)-1;
}

NavNode* AI::buildNode(int turnDir, int currX, int currY)
{
	int newX = currX;
	int newY = currY;
	if(turnDir == AI_WEST)
		{
			newX--;
			if(newX == 0)
			{
				newX = -1;
			}
		}
	else if(turnDir == AI_NORTH)
		{
			newY++;
			if(newY == 0)
			{
				newY = 1;
			}
		}
	else if(turnDir == AI_EAST)
		{
			newX++;
			if(newX == 0)
			{
				newX = 1;
			}
		}
	else if(turnDir == AI_SOUTH)
		{
			newY--;
			if(newY == 0)
			{
				newY = -1;
			}
		}

	NavNode newNode = {rateNode(newX, newY), newX, newY, 0, 0, 0, 0};
	
	if(turnDir == AI_WEST)
		{
			newNode.east = currentNode;
		}
	else if(turnDir == AI_NORTH)
		{
			newNode.south = currentNode;
		}
	else if(turnDir == AI_EAST)
		{
			newNode.west = currentNode;
		}
	else if(turnDir == AI_SOUTH)
		{
			newNode.north = currentNode;
		}

	emptyNodes[memIndex] = newNode;
	NavNode* index = &emptyNodes[memIndex];
	memIndex++;
	return index;
}

int AI::modFour(int val){
	if(val > 4){
		val -= 4;
	}
	return val;
}

