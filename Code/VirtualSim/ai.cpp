#include "ai.h"
#include "stdlib.h"

// See ai.h for comment
AI::AI(){
	compass = AI_NORTH;
	dummyVar = 0;
	//NavNode tempArray[16][16] = {0};
	//mazeArray = tempArray;
	//these are what I am using to store relevant information until we talk about what will actually happenL
	currentNode;
	//zero is used as 0 pointer
	NavNode temp = {14, -8,-8, 0, 0, 0, 0};
	root = temp;
	currentNode = root;
	//mazeArray[0][0] = root;
	
}


//TODO: We need to prune bad nodes!!
// See ai.h for comment
int AI::makeDecision(int deltaDist, bool left, bool straight, bool right, bool back){
	//Grope for info (DONE)
	
	//I expect information as (leftIsWall, centerIsWall, rightIsWall, distFromLast)
	
	int currX = currentNode.xOffset;
	int currY = currentNode.yOffset;
	int leftRating = 99;
	int rightRating = 99;
	int forwardRating = 99;
		
	//Stop me if I've seen this before
	/*if(currentNode.north != 0 && currentNode.east != 0 || currentNode.east != 0 && currentNode.south != 0
		|| currentNode.south != 0 && currentNode.west != 0 || currentNode.west != 0 && currentNode.north != 0)
	*/{
		if(compass == AI_NORTH)
		{
			//deals with crossing the origin, as each square is measured by the outermost corner
			if(currentNode.yOffset < 0 && deltaDist > -currentNode.yOffset)
			{
				//if we cross the origin, we modify the distance to skip zero
				deltaDist++;
			}
			currY += deltaDist;
		}
		else if(compass == AI_SOUTH)
		{
			if(currentNode.yOffset > 0 && deltaDist > currentNode.yOffset)
			{
				deltaDist++;
			}
			currY -= deltaDist;
		}
		else if(compass == AI_EAST)
		{
			if(currentNode.xOffset < 0 && deltaDist > -currentNode.xOffset)
			{
				deltaDist++;
			}
			currX += deltaDist;
		}
		else if(compass == AI_WEST)
		{
			if(currentNode.xOffset > 0 && deltaDist > currentNode.xOffset)
			{
				deltaDist++;
			}
			currX -= deltaDist;
		}

		//Fix the node's position to match its real world location and store it in the maze array.
		currentNode.xOffset = currX;
		currentNode.yOffset = currY;
		mazeArray[currX+8][currY+8] = currentNode;
		
		if (!left)
		{
			int nodePos = (compass + NODE_LEFT)%4;
			if (nodePos == AI_WEST)
			{
				currentNode.west = buildNode(AI_WEST, currX, currY);
				leftRating = (*currentNode.west).rating;
			}
			else if (nodePos == AI_NORTH)
			{
				currentNode.north = buildNode(AI_NORTH, currX, currY);
				leftRating = (*currentNode.north).rating;
			}
			else if (nodePos == AI_SOUTH)
			{
				currentNode.south = buildNode(AI_SOUTH, currX, currY);
				leftRating = (*currentNode.south).rating;
			}
			else if (nodePos == AI_EAST)
			{
				currentNode.east = buildNode(AI_EAST, currX, currY);
				leftRating = (*currentNode.east).rating;
			}
		}
		if (!straight)
		{
			int nodePos = (compass + NODE_STRAIGHT)%4;
			if (nodePos == AI_WEST)
			{
				currentNode.west = buildNode(AI_WEST, currX, currY);
				forwardRating = (*currentNode.west).rating;
			}
			else if (nodePos == AI_NORTH)
			{
				currentNode.north = buildNode(AI_NORTH, currX, currY);
				forwardRating = (*currentNode.north).rating;
			}
			else if (nodePos == AI_SOUTH)
			{
				currentNode.south = buildNode(AI_SOUTH, currX, currY);
				forwardRating = (*currentNode.south).rating;
			}
			else if (nodePos == AI_EAST)
			{
				currentNode.east = buildNode(AI_EAST, currX, currY);
				forwardRating = (*currentNode.east).rating;
			}
		}
		if (!right)
		{
			int nodePos = (compass + NODE_RIGHT)%4;
			if (nodePos == AI_WEST)
			{
				currentNode.west = buildNode(NODE_RIGHT, currX, currY);
				rightRating = (*currentNode.west).rating;
			}
			else if (nodePos == AI_NORTH)
			{
				currentNode.north = buildNode(NODE_RIGHT, currX, currY);
				rightRating = (*currentNode.north).rating;
			}
			else if (nodePos == AI_SOUTH)
			{
				currentNode.south = buildNode(NODE_RIGHT, currX, currY);
				rightRating = (*currentNode.south).rating;
			}
			else if (nodePos == AI_EAST)
			{
				currentNode.east = buildNode(NODE_RIGHT, currX, currY);
				rightRating = (*currentNode.east).rating;
			}
		}
	}
	
	int choice;
	if((leftRating < rightRating) && (leftRating < forwardRating))
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
	else if(forwardRating == 99 && leftRating == 99 && rightRating == 99)
	{
		currentNode.rating = 99;
		choice = NODE_BACK;
	}
	else
	{
		choice = NODE_RIGHT;
	}

	//make the node I chose the current node
	int picked = compass + choice;
	if(picked == AI_WEST)
	{
		currentNode = *(currentNode.west);
		compass = AI_WEST;
	}
	else if(picked == AI_NORTH)
	{
		currentNode = *(currentNode.north);
		compass = AI_NORTH;
	}
	else if(picked == AI_EAST)
	{
		currentNode = *(currentNode.east);
		compass = AI_EAST;
	}
	else if(picked == AI_SOUTH)
	{
		currentNode = *(currentNode.south);
		compass = AI_SOUTH;
	}

	return (choice +1)%4;
	
	
	//Determine options, find best
	
	//create the node that we will use (node will be a struct)
	
	//tell the robot to follow the correct path
	
	//loop back and continue groping for info.
}
//Helper method that rates a node based on the heading and the direction of the turn
int AI::rateNode(int x, int y)
{
	return abs(x)+abs(y);
}

NavNode* AI::buildNode(int turnDir, int currX, int currY)
{
	int newX = currentNode.xOffset;
	int newY = currentNode.yOffset;
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
	NavNode newNode = {rateNode(newX, newY), newX, newY, &currentNode, 0, 0, 0};
	return &newNode;
}