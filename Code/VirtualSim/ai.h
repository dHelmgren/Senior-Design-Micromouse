/*
 *AI.h
 *
 * A pretty neat-o file where Devin plays with confusing things
 */
#define AI_WEST 0
#define AI_NORTH 1
#define AI_EAST 2
#define AI_SOUTH 3

#define AI_LEFT 0
#define AI_STRAIGHT 1
#define AI_RIGHT 2
#define AI_BACK 3

#define NODE_STRAIGHT 0
#define NODE_RIGHT 1
#define NODE_BACK 2
#define NODE_LEFT 3
typedef struct NavNode
{
	int rating;
	int xOffset;
	int yOffset;
	NavNode *west;
	NavNode *north;
	NavNode *east;
	NavNode *south;
}NavNode;

class AI{
private:
	// To keep track of what way is "forward" for the mouse
	int compass;
	int dummyVar;
	NavNode* mazeArray[16][16];
	int memIndex;
	bool sawDeadEndLastTime;

public:

	NavNode root;
	NavNode* currentNode;
	NavNode* prevNode;
	NavNode emptyNodes[100];
	unsigned char lastDecisions[3];
	// Default constructor initializes all of the above variables. See ai.cpp for more info
	AI();
	// TRUE = YES WALL
	// FALSE = NO WALL
	// Return: AI_LEFT, AI_STRAIGHT, AI_RIGHT, or AI_BACK
	// Just FYI, this method is called in drawScene() in main.cpp
	int makeDecision(int deltaDist, bool left, bool straight, bool right, bool back);
	int rateNode(int x, int y);
	NavNode* buildNode(int turnDir, int currX, int currY);
	int modFour(int val);
	int numChildern(NavNode* check, int compass);
};