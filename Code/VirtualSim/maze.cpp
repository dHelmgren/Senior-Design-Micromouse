#include "maze.h"
#include "mouse.h"

Maze::Maze(){
	// Set bottom left coordinates of maze
	xCoord = - PIXELS_PER_SQUARE/2;
	yCoord = - PIXELS_PER_SQUARE/2;
	
	int i, j;
	for(i = 0; i < DIMENSION; i++){
		for(j = 0; j < DIMENSION; j++){
			mazeArrayTop[i][j] = false;
			mazeArrayLeft[i][j] = false;
		}
	}
	initMazeArray();
}

void Maze::initIsTuple(){
	// Bool array default is true, so set all to false
	// Initialize isTuple to tell us whether or not there is a tuple in a certain square
	// Also initialize the wall information per tuple
	int i, j, k;
	for(i = 0; i < DIMENSION - 1; i++){
		for(j = 0; j < DIMENSION - 1; j++){
			// There is not a tuple if we are seeing only 2 parallel walls; IF we are seeing that the left and right
			// walls are present but the front wall is not present (dead end), we still want to make a tuple.
			if((mazeArrayTop[i][j] && mazeArrayTop[i][j+1] && !mazeArrayLeft[i][j] && !mazeArrayLeft[i+1][j]) ||
				(mazeArrayLeft[i][j] && mazeArrayLeft[i+1][j] && !mazeArrayTop[i][j] && !mazeArrayTop[i][j+1])){
				isTuple[i][j] = false;
			}
			// There is a tuple
			else{
				isTuple[i][j] = true;
				//DEBUG
				//printf("Tuple at %d, %d: WEST: %d, NORTH: %d, EAST: %d, SOUTH: %d\n", i, j, mazeArrayLeft[i][j], mazeArrayTop[i][j+1], mazeArrayLeft[i+1][j], mazeArrayTop[i][j]);
			}
			for(k = 0; k < 4; k++){
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
	// Debug print loop to verify tuples
	// Was used in email to verify that each tupled unit is correctly idenitifed (screenshot)
	/*for(int i = 0; i < DIMENSION - 1; i ++){
		printf("%d:\t", i);
		for(int j = 0; j < DIMENSION - 1; j++){
			printf("%d ", isTuple[i][j]);
		}
		printf("\n");
	}*/
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

void Maze::initMazeArray(){
	// Initialize all the walls on the sides of the maze
	int i;
	for(i = 0; i < DIMENSION -1 ; i++){
		mazeArrayTop[i][0] = true;
		mazeArrayTop[i][DIMENSION-1] = true;
		mazeArrayLeft[0][i] = true;
		mazeArrayLeft[DIMENSION-1][i] = true;
	}

	mazeArrayLeft[1][0] = true;

	// Row 1
	mazeArrayLeft[1][1] = true;
	mazeArrayLeft[5][1] = true;
	mazeArrayLeft[6][1] = true;
	mazeArrayLeft[8][1] = true;
	mazeArrayLeft[15][1] = true;
	for(i = 2; i < DIMENSION - 2; i++){
		if(i!=5 && i!=7){
			mazeArrayTop[i][1] = true;
		}
	}

	// Row 2
	mazeArrayLeft[2][2] = true;
	mazeArrayLeft[5][2] = true;
	mazeArrayLeft[7][2] = true;
	mazeArrayLeft[9][2] = true;
	mazeArrayLeft[15][2] = true;
	for(i = 2; i < DIMENSION - 2; i++){
		if(i!=4 && i!=6 && i!= 8 && i!=14){
			mazeArrayTop[i][2] = true;
		}
	}

	// Row 3
	mazeArrayLeft[2][3] = true;
	mazeArrayLeft[4][3] = true;
	mazeArrayLeft[5][3] = true;
	mazeArrayLeft[6][3] = true;
	mazeArrayLeft[8][3] = true;
	mazeArrayLeft[14][3] = true;
	mazeArrayLeft[15][3] = true;
	mazeArrayTop[3][4] = true;
	mazeArrayTop[7][4] = true;
	mazeArrayTop[9][4] = true;
	mazeArrayTop[12][4] = true;
	for(i = 0; i < DIMENSION - 3; i++){
		if(i!=1 && i!=4 && i!=5 && i!=7 && i!= 9){
			mazeArrayTop[i][3] = true;
		}
	}

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
	mazeArrayTop[3][5] = true;
	mazeArrayTop[4][5] = true;
	mazeArrayTop[5][5] = true;
	mazeArrayTop[6][5] = true;
	mazeArrayTop[8][5] = true;
	mazeArrayTop[11][5] = true;

	// Row 5
	mazeArrayLeft[1][5] = true;
	mazeArrayLeft[2][5] = true;
	mazeArrayLeft[3][5] = true;
	mazeArrayLeft[8][5] = true;
	mazeArrayLeft[10][5] = true;
	mazeArrayLeft[12][5] = true;
	mazeArrayLeft[14][5] = true;
	mazeArrayLeft[15][5] = true;
	mazeArrayTop[3][6] = true;
	mazeArrayTop[4][6] = true;
	mazeArrayTop[6][6] = true;
	mazeArrayTop[7][6] = true;
	mazeArrayTop[9][6] = true;
	mazeArrayTop[12][6] = true;

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
	mazeArrayTop[7][7] = true;
	mazeArrayTop[8][7] = true;
	mazeArrayTop[10][7] = true;
	mazeArrayTop[12][7] = true;

	// Row 7
	for(i = 0; i < DIMENSION - 1; i++){
		if(i!=2 && i!=8 && i!=10 && i!=12){
			mazeArrayLeft[i][7] = true;
		}
	}
	mazeArrayTop[1][8] = true;
	mazeArrayTop[9][8] = true;
	mazeArrayTop[12][8] = true;

	// Row 8
	for(i = 0; i < DIMENSION; i++){
		if(i!=2 && i!=8 && i!=10 && i!=13 && i!=15){
			mazeArrayLeft[i][8] = true;
		}
	}
	mazeArrayTop[2][9] = true;
	mazeArrayTop[7][9] = true;
	mazeArrayTop[10][9] = true;
	mazeArrayTop[13][9] = true;
	mazeArrayTop[14][9] = true;

	// Row 9
	mazeArrayLeft[3][9] = true;
	mazeArrayLeft[4][9] = true;
	mazeArrayLeft[5][9] = true;
	mazeArrayLeft[6][9] = true;
	mazeArrayLeft[8][9] = true;
	mazeArrayLeft[10][9] = true;
	mazeArrayLeft[12][9] = true;
	mazeArrayLeft[15][9] = true;
	mazeArrayTop[1][10] = true;
	mazeArrayTop[6][10] = true;
	mazeArrayTop[8][10] = true;
	mazeArrayTop[9][10] = true;
	mazeArrayTop[13][10] = true;

	// Row 10
	mazeArrayLeft[1][10] = true;
	mazeArrayLeft[3][10] = true;
	mazeArrayLeft[5][10] = true;
	mazeArrayLeft[11][10] = true;
	mazeArrayLeft[12][10] = true;
	mazeArrayLeft[13][10] = true;
	mazeArrayLeft[14][10] = true;
	mazeArrayLeft[15][10] = true;
	mazeArrayTop[2][11] = true;
	for(i = 6; i < 11; i++){
		mazeArrayTop[i][11] = true;
	}

	// Row 11
	mazeArrayLeft[1][11] = true;
	mazeArrayLeft[2][11] = true;
	mazeArrayLeft[4][11] = true;
	mazeArrayLeft[5][11] = true;
	mazeArrayLeft[13][11] = true;
	mazeArrayLeft[15][11] = true;
	for(i = 4; i < 14; i++){
		if(i!=5){
			mazeArrayTop[i][12] = true;
		}
	}

	// Row 12
	mazeArrayLeft[1][12] = true;
	mazeArrayLeft[3][12] = true;
	mazeArrayLeft[14][12] = true;
	mazeArrayLeft[15][12] = true;
	for(i = 1; i < 13; i++){
		if(i!=2 && i!=9){
			mazeArrayTop[i][13] = true;
		}
	}

	// Row 13
	mazeArrayLeft[1][13] = true;
	mazeArrayLeft[4][13] = true;
	mazeArrayLeft[9][13] = true;
	mazeArrayLeft[10][13] = true;
	mazeArrayLeft[14][13] = true;
	mazeArrayLeft[15][13] = true;
	for(i = 2; i < 13; i++){
		if(i!=3 && i!=4 && i!=8 && i!=9){
			mazeArrayTop[i][14] = true;
		}
	}

	// Row 14
	mazeArrayLeft[1][14] = true;
	mazeArrayLeft[2][14] = true;
	mazeArrayLeft[4][14] = true;
	mazeArrayLeft[8][14] = true;
	mazeArrayLeft[9][14] = true;
	mazeArrayLeft[14][14] = true;
	mazeArrayLeft[15][14] = true;
	for(i = 3; i < 14; i++){
		if(i!=8 && i!=9){
			mazeArrayTop[i][15] = true;
		}
	}

	// Row 15
	mazeArrayLeft[2][15] = true;
}

int Maze::getXCoord(){
	return xCoord;
}

int Maze::getYCoord(){
	return yCoord;
}