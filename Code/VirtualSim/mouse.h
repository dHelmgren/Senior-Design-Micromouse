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

#define PI 3.14159265

#define MOUSE_WIDTH_CM 7	//centimeters
#define MOUSE_HEIGHT_CM 9	//centimeters
#define MOUSE_WIDTH_PX (PIXELS_PER_SQUARE/18)*MOUSE_WIDTH_CM
#define MOUSE_HEIGHT_PX (PIXELS_PER_SQUARE/18)*MOUSE_HEIGHT_CM 
#define MOUSE_HALF_WIDTH (MOUSE_WIDTH_PX)/2
#define MOUSE_HALF_HEIGHT (MOUSE_HEIGHT_PX)/2
#define STRAIGHT 0
#define RIGHT 1
#define LEFT 2
#define BACK 3


class Mouse {
public:
	int direction;
	int angle;
	int gridLocX;
	int gridLocY;
	// Distance the mouse has moved (in grid units) since the last tuple
	// has been created.
	int deltaDistance;
	// Used to calculate how far (in grid units) the mouse has moved
	// since the last tuple
	int prevDeltaX;
	int prevDeltaY;

	// Ctor
	Mouse();
	// Draws the mouse as a blue square
	void drawMouse();
	// Determines where in the maze grid the mouse is currently located
	void updateMouseLocation(int mazeXCoord, int mazeYCoord);
	// Turns the mouse left and then keeps going straight
	void left();
	// Turns the mouse right and then keeps going straight
	void right();
	// Keeps going straight
	void straight();
	// Go backward
	void back();
	// Returns the angle that the mouse is currently traveling at
	int getAngle();
	// Returns true if the mouse is exactly in the middle of a grid unit
	bool isInCenterOfGrid(int mazeStartXCoord, int mazeStartYCoord);

	int getGridLocX();
	int getGridLocY();
	int getDeltaDistance();
	void resetDeltaDistance();
};