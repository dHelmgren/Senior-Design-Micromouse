#include "ai.h"
#include "stdlib.h"

// See ai.h for comment
AI::AI(){
	compass = AI_NORTH;
	dummyVar = 0;
	//these are what I am using to store relevant information until we talk about what will actually happenL
	NavNode currentNode;
	//zero is used as 0 pointer
	NavNode root = {14, -8,-8, 0, 0, 0, 0};
	currentNode = root;
	
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
		
	//if we are coming back to this node, we don't re-invent the wheel
	if(currentNode.left == 0 && currentNode.right == 0 && currentNode.forward == 0)
	{
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
		else if(compass == AI_WEST)
		{
			if(currentNode.xOffset < 0 && deltaDist > -currentNode.xOffset)
			{
				deltaDist++;
			}
			currX += deltaDist;
		}
		else if(compass == AI_SOUTH)
		{
			if(currentNode.xOffset > 0 && deltaDist > currentNode.xOffset)
			{
				deltaDist++;
			}
			currX -= deltaDist;
		}
		
		
		if(left == true)
		{		
			currentNode.left = buildNode(NODE_LEFT, currX, currY);
			leftRating = (*currentNode.left).rating;
		}
		if(right == true)
		{
			currentNode.right = buildNode(NODE_RIGHT, currX, currY);
			rightRating = (*currentNode.right).rating;
		}
		if(straight == true)
		{
			currentNode.forward = buildNode(NODE_STRAIGHT, currX, currY);
			forwardRating = (*currentNode.forward).rating;
		}
	}
	
	if(leftRating < rightRating && leftRating < forwardRating)
	{
		currentNode = *(currentNode.left);
		return AI_LEFT;
	}
	else if(rightRating < leftRating && rightRating < forwardRating)
	{
		currentNode = *(currentNode.right);
		return AI_RIGHT;
	}
	else if(forwardRating < leftRating && forwardRating < rightRating)
	{
		currentNode = *(currentNode.forward);
		return AI_STRAIGHT;
	}
	else if(forwardRating == rightRating || forwardRating == leftRating)
	{
		currentNode = *(currentNode.forward);
		return AI_STRAIGHT;
	}
	else if(forwardRating == 99 && leftRating == 99 && rightRating == 99)
	{
		currentNode.rating = 99;
		currentNode = *(currentNode.parent);
		return AI_BACK;
	}
	
	
	
	
	
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
	//Determine the location of this next node
	turnDir = (turnDir+compass) % 4;
	if(turnDir == NODE_STRAIGHT)
		{
			newX--;
			if(newX == 0)
			{
				newX = -1;
			}
		}
		else if(turnDir == NODE_RIGHT)
		{
			newY++;
			if(newY == 0)
			{
				newY = 1;
			}
		}
		else if(turnDir == NODE_BACK)
		{
			newX++;
			if(newX == 0)
			{
				newX = 1;
			}
		}
		else if(turnDir == NODE_LEFT)
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