#include "maze.h"
#include "mouse.h"

Maze::Maze(){
	// Set bottom left coordinates of maze
	xCoord = - PIXELS_PER_SQUARE/2;
	yCoord = - PIXELS_PER_SQUARE/2;
	
	int i, j;
	for(i = 0; i < DIMENSION; i++){
		for(j = 0; j < DIMENSION; j++){
			// Determines which maze we are currently building. See maze.h for the 
			// WHICH_MAZE constant.
			if(WHICH_MAZE == 1){
				// Call the functions associated with this maze
				initMazeArrayTop1(mazeArrayTop, i, j);
				initMazeArrayLeft1(mazeArrayLeft, i, j);
			}
			else if(WHICH_MAZE == 2){
				initMazeArrayTop2(mazeArrayTop, i, j);
				initMazeArrayLeft2(mazeArrayLeft, i, j);
			}
		}
	}
}

void Maze::initIsTuple(){
	// Bool array default is true, so set all to false
	// Initialize isTuple to tell us whether or not there is a tuple in a certain square
	// Also initialize the wall information per tuple
	int i, j, k;
	for(i = 0; i < DIMENSION - 1; i++){
		for(j = 0; j < DIMENSION - 1; j++){
			// There is not a tuple if we are seeing only 2 parallel walls
			if((mazeArrayTop[i][j] && mazeArrayTop[i][j+1] && !mazeArrayLeft[i][j] && !mazeArrayLeft[i+1][j]) ||
				(mazeArrayLeft[i][j] && mazeArrayLeft[i+1][j] && !mazeArrayTop[i][j] && !mazeArrayTop[i][j+1])){
				isTuple[i][j] = false;
			}
			// There is a tuple
			else{
				isTuple[i][j] = true;
				//DEBUG statement
				//printf("Tuple at %d, %d: WEST: %d, NORTH: %d, EAST: %d, SOUTH: %d\n", i, j, mazeArrayLeft[i][j], mazeArrayTop[i][j+1], mazeArrayLeft[i+1][j], mazeArrayTop[i][j]);
			}
			// Populate yet another data structure with maze wall information. Yes, this is yet another
			// data structure to store the same information, but it compacts the information into a more
			// readable format
			if(mazeArrayTop[i][j+1]){
				isWallWNES[i][j][NORTH] = true;
			}
			if(mazeArrayTop[i][j]){
				isWallWNES[i][j][SOUTH] = true;
			}
			if(mazeArrayLeft[i+1][j]){
				isWallWNES[i][j][EAST] = true;
			}
			if(mazeArrayLeft[i][j]){
				isWallWNES[i][j][WEST] = true;
			}
		}
	}
}

bool Maze::isTupleDetected(int mouseXGrid, int mouseYGrid){
	if(isTuple[mouseXGrid][mouseYGrid]){
		return true;
	}
	return false;
}

void Maze::updateMazeCoords(float currentTime, int angle, bool isMouseGoingFwd){	
	int sine = (int)sin((float)angle*PI/180);
	int cose = (int)cos((float)angle*PI/180);

	// Mouse is always going forward because of the fact that we took away the back
	// IR sensor. However, this code remains if ever teams do add a back IR sensor
	// and would like to simulate the mouse going backward (to simulate the mouse
	// moving backward, all you have to do is change the isMouseGoingFwd boolean)
	
	// Otherwise, this code just determines where the mouse is located in the maze
	// in order to draw it based on the AI's decision
	if(sine == 0 && cose == 1){
		if(isMouseGoingFwd){
			xCoord -= PIXEL_INCR;
		}
		else{
			xCoord += PIXEL_INCR;
		}
	}
	else if(sine == 1 && cose == 0){
		if(isMouseGoingFwd){
			yCoord -= PIXEL_INCR;
		}
		else{
			yCoord += PIXEL_INCR;
		}
	}
	else if(sine == 0 && cose == -1){
		if(isMouseGoingFwd){
			xCoord += PIXEL_INCR;
		}
		else{
			xCoord -= PIXEL_INCR;
		}
	}
	else if(sine == -1 && cose == 0){
		if(isMouseGoingFwd){
			yCoord += PIXEL_INCR;
		}
		else{
			yCoord -= PIXEL_INCR;
		}
	}
	else{
		printf("Well, Sine %d, cose %d\n", sine, cose);
	}
}

void Maze::drawMaze(){
	int i, j;

	glPushMatrix();
	glTranslatef(xCoord, yCoord, 0);
	glColor3f(255,255,255);
	glBegin(GL_LINES);

	for(i = 0; i < DIMENSION; i++){
		for(j = 0; j < DIMENSION; j++){
			// Draw the top wall, if it exists
			if(mazeArrayTop[i][j]){
				glVertex2f(i*PIXELS_PER_SQUARE, j*PIXELS_PER_SQUARE);
				glVertex2f((i+1)*PIXELS_PER_SQUARE, j*PIXELS_PER_SQUARE);
			}

			// Draw the left wall, if it exists
			if(mazeArrayLeft[i][j]){
				glVertex2f(i*PIXELS_PER_SQUARE, j*PIXELS_PER_SQUARE);
				glVertex2f(i*PIXELS_PER_SQUARE, (j+1)*PIXELS_PER_SQUARE);
			}
		}//for
	}//for
	glEnd();
	glPopMatrix();
}

void Maze::initMazeArrayTop1(bool (&mazeArray)[DIMENSION][DIMENSION], int m, int n){
	// This is the old and very not fun way of initializing the maze array. The new
	// and still confusing but better way to initialize the maze array is seen in
	// initMazeArrayTop2

	// Initialize all the walls on the sides of the maze
	int i;
	int j;
	for(i=0; i < DIMENSION; i++){
		for(j=0; j <DIMENSION; j++){
			mazeArray[i][j] = false;
		}
	}
	
	for(i = 0; i < DIMENSION -1 ; i++){
		mazeArray[i][0] = true;
		mazeArray[i][DIMENSION-1] = true;
	}

	mazeArrayLeft[1][0] = true;

	// Row 1
	for(i = 2; i < DIMENSION - 2; i++){
		if(i!=5 && i!=7){
			mazeArray[i][1] = true;
		}
	}

	// Row 2
	for(i = 2; i < DIMENSION - 2; i++){
		if(i!=4 && i!=6 && i!= 8 && i!=14){
			mazeArray[i][2] = true;
		}
	}

	// Row 3
	mazeArray[3][4] = true;
	mazeArray[7][4] = true;
	mazeArray[9][4] = true;
	mazeArray[12][4] = true;
	for(i = 0; i < DIMENSION - 3; i++){
		if(i!=1 && i!=4 && i!=5 && i!=7 && i!= 9){
			mazeArray[i][3] = true;
		}
	}

	// Row 4
	mazeArray[3][5] = true;
	mazeArray[4][5] = true;
	mazeArray[5][5] = true;
	mazeArray[6][5] = true;
	mazeArray[8][5] = true;
	mazeArray[11][5] = true;

	// Row 5
	mazeArray[3][6] = true;
	mazeArray[4][6] = true;
	mazeArray[6][6] = true;
	mazeArray[7][6] = true;
	mazeArray[9][6] = true;
	mazeArray[12][6] = true;

	// Row 6
	mazeArray[7][7] = true;
	mazeArray[8][7] = true;
	mazeArray[10][7] = true;
	mazeArray[12][7] = true;

	// Row 7
	mazeArray[1][8] = true;
	mazeArray[9][8] = true;
	mazeArray[12][8] = true;

	// Row 8
	mazeArray[2][9] = true;
	mazeArray[7][9] = true;
	mazeArray[10][9] = true;
	mazeArray[13][9] = true;
	mazeArray[14][9] = true;

	// Row 9
	mazeArray[1][10] = true;
	mazeArray[6][10] = true;
	mazeArray[8][10] = true;
	mazeArray[9][10] = true;
	mazeArray[13][10] = true;

	// Row 10
	mazeArray[2][11] = true;
	for(i = 6; i < 11; i++){
		mazeArray[i][11] = true;
	}

	// Row 11
	for(i = 4; i < 14; i++){
		if(i!=5){
			mazeArray[i][12] = true;
		}
	}

	// Row 12
	for(i = 1; i < 13; i++){
		if(i!=2 && i!=9){
			mazeArray[i][13] = true;
		}
	}

	// Row 13
	for(i = 2; i < 13; i++){
		if(i!=3 && i!=4 && i!=8 && i!=9){
			mazeArray[i][14] = true;
		}
	}

	// Row 14
	for(i = 3; i < 14; i++){
		if(i!=8 && i!=9){
			mazeArray[i][15] = true;
		}
	}
}

void Maze::initMazeArrayLeft1(bool (&mazeArray)[DIMENSION][DIMENSION], int m, int n){

	int i;
	for(i = 0; i < DIMENSION -1 ; i++){
		mazeArrayLeft[0][i] = true;
		mazeArrayLeft[DIMENSION-1][i] = true;
	}

	// Row 1
	mazeArrayLeft[1][1] = true;
	mazeArrayLeft[5][1] = true;
	mazeArrayLeft[6][1] = true;
	mazeArrayLeft[8][1] = true;
	mazeArrayLeft[15][1] = true;

	// Row 2
	mazeArrayLeft[2][2] = true;
	mazeArrayLeft[5][2] = true;
	mazeArrayLeft[7][2] = true;
	mazeArrayLeft[9][2] = true;
	mazeArrayLeft[15][2] = true;

	// Row 3
	mazeArrayLeft[2][3] = true;
	mazeArrayLeft[4][3] = true;
	mazeArrayLeft[5][3] = true;
	mazeArrayLeft[6][3] = true;
	mazeArrayLeft[8][3] = true;
	mazeArrayLeft[14][3] = true;
	mazeArrayLeft[15][3] = true;

	// Row 4
	mazeArrayLeft[1][4] = true;
	mazeArrayLeft[2][4] = true;
	mazeArrayLeft[5][4] = true;
	mazeArrayLeft[7][4] = true;
	mazeArrayLeft[9][4] = true;
	mazeArrayLeft[11][4] = true;
	mazeArrayLeft[13][4] = true;
	mazeArrayLeft[14][4] = true;
	mazeArrayLeft[15][4] = true;

	// Row 5
	mazeArrayLeft[1][5] = true;
	mazeArrayLeft[2][5] = true;
	mazeArrayLeft[3][5] = true;
	mazeArrayLeft[8][5] = true;
	mazeArrayLeft[10][5] = true;
	mazeArrayLeft[12][5] = true;
	mazeArrayLeft[14][5] = true;
	mazeArrayLeft[15][5] = true;

	// Row 6
	mazeArrayLeft[1][6] = true;
	mazeArrayLeft[2][6] = true;
	mazeArrayLeft[3][6] = true;
	mazeArrayLeft[5][6] = true;
	mazeArrayLeft[6][6] = true;
	mazeArrayLeft[9][6] = true;
	mazeArrayLeft[11][6] = true;
	mazeArrayLeft[13][6] = true;
	mazeArrayLeft[14][6] = true;
	mazeArrayLeft[15][6] = true;

	// Row 7
	for(i = 0; i < DIMENSION - 1; i++){
		if(i!=2 && i!=8 && i!=10 && i!=12){
			mazeArrayLeft[i][7] = true;
		}
	}

	// Row 8
	for(i = 0; i < DIMENSION; i++){
		if(i!=2 && i!=8 && i!=10 && i!=13 && i!=15){
			mazeArrayLeft[i][8] = true;
		}
	}

	// Row 9
	mazeArrayLeft[3][9] = true;
	mazeArrayLeft[4][9] = true;
	mazeArrayLeft[5][9] = true;
	mazeArrayLeft[6][9] = true;
	mazeArrayLeft[8][9] = true;
	mazeArrayLeft[10][9] = true;
	mazeArrayLeft[12][9] = true;
	mazeArrayLeft[15][9] = true;

	// Row 10
	mazeArrayLeft[1][10] = true;
	mazeArrayLeft[3][10] = true;
	mazeArrayLeft[5][10] = true;
	mazeArrayLeft[11][10] = true;
	mazeArrayLeft[12][10] = true;
	mazeArrayLeft[13][10] = true;
	mazeArrayLeft[14][10] = true;
	mazeArrayLeft[15][10] = true;

	// Row 11
	mazeArrayLeft[1][11] = true;
	mazeArrayLeft[2][11] = true;
	mazeArrayLeft[4][11] = true;
	mazeArrayLeft[5][11] = true;
	mazeArrayLeft[13][11] = true;
	mazeArrayLeft[15][11] = true;

	// Row 12
	mazeArrayLeft[1][12] = true;
	mazeArrayLeft[3][12] = true;
	mazeArrayLeft[14][12] = true;
	mazeArrayLeft[15][12] = true;

	// Row 13
	mazeArrayLeft[1][13] = true;
	mazeArrayLeft[4][13] = true;
	mazeArrayLeft[9][13] = true;
	mazeArrayLeft[10][13] = true;
	mazeArrayLeft[14][13] = true;
	mazeArrayLeft[15][13] = true;

	// Row 14
	mazeArrayLeft[1][14] = true;
	mazeArrayLeft[2][14] = true;
	mazeArrayLeft[4][14] = true;
	mazeArrayLeft[8][14] = true;
	mazeArrayLeft[9][14] = true;
	mazeArrayLeft[14][14] = true;
	mazeArrayLeft[15][14] = true;

	// Row 15
	mazeArrayLeft[2][15] = true;
}

void Maze::initMazeArrayTop2(bool (&mazeArray)[DIMENSION][DIMENSION], int m, int n){
	/* This is important information from the users manual:
	   1) The first and last columns of the 2D top array (ie. tempMazeArray[x][0] and 
	       tempMazeArray[x][15]) should have all values equal to true, except for x = 16 
	       because this top maze array is off the screen.
	   2) The last row of the 2D top array (ie. tempMazeArray[16][x]) should all have 
	       values equal to false, because this would create a wall outside of the 
	       boundaries of the maze. */
	
	
	bool tempMazeArray[DIMENSION][DIMENSION] = {
		{true,	false,	false,	false,	false,	true,	false,	false,	false,	true,	false,	false,	false,	false,	false,	false,	true},

		{true,	false,	false,	false,	false,	false,	false,	false,	true,	false,	false,	false,	false,	true,	false,	true,	true},
		{true,	true,	false,	false,	false,	false,	false,	true,	true,	true,	false,	false,	true,	false,	true,	false,	true},
		{true,	true,	false,	false,	false,	false,	true,	true,	true,	true,	true,	false,	true,	false,	false,	false,	true},

		{true,	false,	true,	true,	false,	true,	true,	true,	true,	true,	false,	true,	false,	false,	true,	true,	true},
		{true,	false,	true,	true,	true,	true,	true,	false,	true,	false,	true,	true,	true,	false,	false,	true,	true},
		{true,	true,	true,	true,	true,	true,	false,	true,	false,	false,	true,	false,	false,	true,	false,	false,	true},
		{true,	true,	true,	true,	true,	true,	true,	true,	false,	false,	false,	true,	true,	true,	true,	false,	true},

		{true,	true,	true,	true,	true,	true,	true,	true,	false,	true,	true,	true,	true,	true,	false,	false,	true},
		{true,	true,	false,	true,	true,	true,	true,	false,	false,	false,	true,	true,	true,	false,	true,	true,	true},
		{true,	false,	false,	false,	true,	false,	true,	true,	false,	false,	false,	true,	true,	true,	true,	true,	true},
		{true,	false,	true,	false,	true,	true,	false,	true,	true,	false,	true,	true,	true,	true,	true,	true,	true},

		{true,	true,	true,	true,	true,	true,	true,	true,	true,	false,	true,	true,	true,	true,	true,	true,	true},
		{true,	true,	true,	true,	true,	true,	true,	false,	true,	false,	true,	true,	true,	true,	true,	true,	true},
		{true,	true,	false,	true,	false,	false,	false,	false,	false,	false,	true,	true,	false,	true,	false,	true,	true},

		{true,	false,	false,	false,	false,	false,	false,	false,	false,	false,	true,	false,	false,	false,	false,	false,	true},
		{false,	false,	false,	false,	false,	false,	false,	false,	false,	false,	false,	false,	false,	false,	false,	false,	false}};

	// Perform reassignment because C++ can't cast from type (&mazeArray)[][] to tempMazeArray[DIMENSION][DIMENSION]
	for(int i = 0; i < DIMENSION; i++){
		for(int j = 0; j < DIMENSION; j++){
			mazeArray[i][j] = tempMazeArray[i][j];
		}
	}
}

void Maze::initMazeArrayLeft2(bool (&mazeArray)[DIMENSION][DIMENSION], int m, int n){
	/* This is important information from the users manual:
	   3) The first and last rows of the 2D left array (ie. tempMazeArray[0][x] and 
	       tempMazeArray[15][x]) should have all values equal to true, except for x = 16 
		   because this left maze array is off the screen.
       4) The last column of the 2D left array (ie. tempMazeArray[x][15]) should all 
	       have values equal to false because this would create a wall outside of the 
		   boundaries of the maze. */

	bool tempMazeArray[DIMENSION][DIMENSION] = {
		{true,	true,	true,	true,	true,	true,	true,	true,	true,	true,	true,	true,	true,	true,	true,	true,	false},
		
		{true,	true,	true,	true,	false,	false,	true,	true,	false,	false,	true,	true,	true,	true,	true,	false,	false},
		{false,	true,	true,	true,	true,	true,	true,	false,	false,	true,	false,	true,	false,	false,	false,	false,	false},
		{false,	false,	true,	true,	true,	true,	false,	false,	false,	false,	true,	false,	true,	true,	true,	false,	false},
		{false,	true,	false,	true,	true,	false,	false,	false,	false,	true,	false,	true,	false,	true,	false,	false,	false},
		
		{true,	false,	false,	false,	false,	false,	false,	false,	false,	false,	false,	false,	true,	false,	false,	false,	false},
		{false,	false,	false,	false,	false,	false,	true,	true,	true,	false,	false,	true,	true,	true,	false,	true,	false},
		{false,	false,	false,	false,	false,	false,	false,	true,	true,	true,	true,	false,	false,	false,	true,	false,	false},
		{false,	false,	false,	false,	false,	false,	false,	false,	false,	true,	false,	false,	false,	false,	true,	false,	false},
		
		{false,	false,	false,	false,	false,	false,	false,	true,	true,	false,	false,	false,	false,	true,	false,	false,	false},
		{false,	true,	true,	false,	false,	false,	false,	true,	true,	true,	false,	false,	false,	false,	false,	false,	false},
		{false,	true,	true,	true,	false,	true,	false,	false,	true,	false,	false,	false,	false,	false,	false,	false,	false},
		{false,	false,	false,	false,	false,	false,	false,	false,	false,	true,	false,	false,	false,	false,	false,	false,	false},
		
		{false,	false,	false,	false,	false,	false,	false,	false,	true,	false,	false,	false,	false,	false,	false,	false,	false},
		{false,	false,	false,	false,	false,	false,	true,	true,	false,	true,	false,	false,	false,	false,	false,	false,	false},
		{false,	true,	true,	false,	true,	true,	true,	true,	true,	false,	false,	true,	true,	true,	true,	false,	false},

		{true,	true,	true,	true,	true,	true,	true,	true,	true,	true,	true,	true,	true,	true,	true,	true,	false}};

	// Perform reassignment because C++ can't cast from type (&mazeArray)[][] to tempMazeArray[DIMENSION][DIMENSION]
	for(int i = 0; i < DIMENSION; i++){
		for(int j = 0; j < DIMENSION; j++){
			mazeArray[i][j] = tempMazeArray[i][j];
		}
	}
}

int Maze::getXCoord(){
	return xCoord;
}

int Maze::getYCoord(){
	return yCoord;
}