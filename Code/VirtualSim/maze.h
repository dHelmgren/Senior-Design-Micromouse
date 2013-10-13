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

#define PI 3.14159265

class Maze {
public:
	bool mazeArrayTop[DIMENSION][DIMENSION];
	bool mazeArrayLeft[DIMENSION][DIMENSION];
	// Coordinates of the bottom left corner of maze; allows
	// us to simulate mouse moving forward (OpenGL: must move
	// the environment, not the origin)
	int xCoord;
	int yCoord;
	//TupleSet tupleSet;

	// Ctor
	Maze();
	// Initialize first configuration of maze by updating the 2D arrays
	void initMazeArray();
	void drawMaze();
	void updateMazeCoords(float currentTime, int angle);
	int getXCoord();
	int getYCoord();
};