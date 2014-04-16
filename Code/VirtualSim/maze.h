//===========================
// include guard
//#pragma once

#include <iostream>
#include <stdlib.h>
#include <stddef.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <time.h>

#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GLUT/glut.h>
#else 
  #ifdef _WIN32
    #include <windows.h>
  #endif
  #include <GL/gl.h>
  #include <GL/glu.h>
  #include <GL/glut.h>
#endif

//#endif

#define DIMENSION 17
#define PIXELS_PER_SQUARE 72

// ***** In order to change which maze we are currently building,
// change this constant. 
// OPTIONS: 1, 2, 3
#define WHICH_MAZE 3

#define PI 3.14159265

#define WEST 0
#define NORTH 1
#define EAST 2
#define SOUTH 3

class Maze {
public:
	bool mazeArrayTop[DIMENSION][DIMENSION];
	bool mazeArrayLeft[DIMENSION][DIMENSION];
	// Coordinates of the bottom left corner of maze; allows
	// us to simulate mouse moving forward (OpenGL: must move
	// the environment, not the origin)
	int xCoord;
	int yCoord;
	// Tuples for the maze
	bool isTuple[DIMENSION-1][DIMENSION];
	bool isWallWNES[DIMENSION-1][DIMENSION][4];

	// Ctor
	Maze();
	// Initialize first configuration of maze by updating the 2D arrays
	void initMazeArrayTop1(bool (&mazeArray)[DIMENSION][DIMENSION], int m, int n);
	void initMazeArrayLeft1(bool (&mazeArray)[DIMENSION][DIMENSION], int m, int n);
	void initMazeArrayTop2(bool (&mazeArray)[DIMENSION][DIMENSION], int m, int n);
	void initMazeArrayLeft2(bool (&mazeArray)[DIMENSION][DIMENSION], int m, int n);
	void initMazeArrayTop3(bool (&mazeArray)[DIMENSION][DIMENSION], int m, int n);
	void initMazeArrayLeft3(bool (&mazeArray)[DIMENSION][DIMENSION], int m, int n);
	// Initializes where tuples are located in the maze
	void initIsTuple();
	// Determines if the mouse had entered into a tuple zone
	bool isTupleDetected(int mouseXGrid, int mouseYGrid);
	void drawMaze();
	void updateMazeCoords(float currentTime, int angle, bool isMouseGoingFwd);
	int getXCoord();
	int getYCoord();
};