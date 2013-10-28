#include "ai.h"

// See ai.h for comment
AI::AI(){
	compass = AI_NORTH;
	dummyVar = 0;
	//these are what I am using to store relevant information until we talk about what will actually happen
	NavNode currentNode;
	NavNode root = {14, -8,-8, AI_NORTH, NULL, NULL, NULL, NULL, 0};
	currentNode = root;
}

typedef struct NavNode
{
	int rating;
	int xOffset;
	int yOffset;
	NavNode *parent;
	NavNode *left;
	NavNode *forward;
	NavNode *right;
}NavNode;

//TODO: We need to prune bad nodes!!
// See ai.h for comment
int AI::makeDecision(int deltaDist, bool left, bool straight, bool right, bool back){
	//Grope for info (DONE)
	
	//I expect information as (leftIsWall, centerIsWall, rightIsWall, distFromLast)
	
	int currX = currentNode.xOffset;
	int currY = currentNode.yOffset;
		
	//if we are coming back to this node, we don't re-invent the wheel
	if(currentNode.left == NULL && currentNode.right == NULL && currentNode.forward == NULL)
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
		
		int leftRating = 99;
		int rightRating = 99;
		int forwardRating = 99;
		if(left == TRUE)
		{		
			currentNode.left = buildNode(NODE_LEFT, currX, currY);
			leftRating = (*currentNode.left).rating;
		}
		if(right == TRUE)
		{
			currentNode.right = buildNode(NODE_RIGHT, currX, currY);
			rightRating = (*currentNode.right).rating;
		}
		if(straight == TRUE)
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
		return AI_FORWARD;
	}
	else if(forwardRating == rightRating || forwardRating == leftRating)
	{
		currentNode = *(currentNode.forward);
		return AI_FORWARD;
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
	int newX = currNode.xOffset;
	int newY = currNode.yOffset;
	//Determine the location of this next node
	turnDir = (turnDir+currentNode.heading) % 4;
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
	NavNode newNode = {rateNode(newX, newY), newX, newY, currentNode, NULL, NULL, NULL};
	return newNode*;
}