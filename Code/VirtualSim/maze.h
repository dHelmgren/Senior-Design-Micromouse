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

class Maze {
public:
	Maze();
	bool mazeArrayTop[DIMENSION][DIMENSION];
	bool mazeArrayLeft[DIMENSION][DIMENSION];
	void initMazeArray();
	void drawMaze();
};