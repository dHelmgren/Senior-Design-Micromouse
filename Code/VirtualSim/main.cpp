#include "maze.h"
#include "mouse.h"
#include "ai.h"

// The number of distance units that are in our viewport
#define VIEWSIZE 1500

// The time between ticks, in milliseconds
#define TICK_INTERVAL 50

// The current time
static float currentTime = 0.0;

// a helper-function that returns a random number in the range [0.0,1.0)
GLfloat randUniform() {
	return rand()/(GLfloat)RAND_MAX;
}

// Create our objects: maze, mouse, and AI
Maze maze = Maze();
Mouse mouseRobot = Mouse();
AI *aiPtr = new AI();
AI ai = *(aiPtr);

// draws our scene
void drawScene() {
	// clear scene, handle depth
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// enter global transformation domain
	glPushMatrix();

	// Update the x and y coordinates of the maze according to 
	// where the mouse is going
	maze.updateMazeCoords(currentTime, mouseRobot.getAngle(), mouseRobot.getIsGoingForward());

	// "Undo" the translation of the maze to make it seem like the
	// mouse is the thing moving in the global environment
	glTranslatef(-maze.getXCoord(), -maze.getYCoord(), 0);

	// Draw the maze!
	maze.initMazeArray();
	maze.drawMaze();

	// Only process if the mouse is in the center of the grid
	if(mouseRobot.isInCenterOfGrid(maze.getXCoord(), maze.getYCoord())){
		int prevX = mouseRobot.getGridLocX();
		int prevY = mouseRobot.getGridLocY();

		// Update the maze location to determine if there is a tuple
		mouseRobot.updateMouseLocation(maze.getXCoord(), maze.getYCoord());

		int currX = mouseRobot.getGridLocX();
		int currY = mouseRobot.getGridLocY();

		// Only ask if there is a tuple if this is the first time the mouse has come into this grid coordinate
		if((currX != prevX || currY != prevY)){
			int gridX = mouseRobot.getGridLocX();
			int gridY = mouseRobot.getGridLocY();
			// Ask if there is a tuple here
			bool isMouseInTupleGrid = maze.isTupleDetected(gridX, gridY);
	
			if(isMouseInTupleGrid){
				int angle = mouseRobot.getAngle();
				bool left = false;
				bool straight = false;
				bool right = false;
				bool back = false;
				// Straight is EAST
				if(angle == 0){
					straight = maze.isWallWNES[gridX][gridY][EAST];
					left = maze.isWallWNES[gridX][gridY][NORTH];
					back = maze.isWallWNES[gridX][gridY][WEST];
					right = maze.isWallWNES[gridX][gridY][SOUTH];
				}
				// Straight is NORTH
				else if(angle == 90){
					straight = maze.isWallWNES[gridX][gridY][NORTH];
					left = maze.isWallWNES[gridX][gridY][WEST];
					back = maze.isWallWNES[gridX][gridY][SOUTH];
					right = maze.isWallWNES[gridX][gridY][EAST];
				}
				// Straight is WEST
				else if(angle == 180){
					straight = maze.isWallWNES[gridX][gridY][WEST];
					left = maze.isWallWNES[gridX][gridY][SOUTH];
					back = maze.isWallWNES[gridX][gridY][EAST];
					right = maze.isWallWNES[gridX][gridY][NORTH];
				}
				// Straight is SOUTH
				else if(angle == 270){
					straight = maze.isWallWNES[gridX][gridY][SOUTH];
					left = maze.isWallWNES[gridX][gridY][EAST];
					back = maze.isWallWNES[gridX][gridY][NORTH];
					right = maze.isWallWNES[gridX][gridY][WEST];
				}
				else{
					printf("Angle error. See main.cpp.\n");
				}

				//DEBUG
				//printf("Tuple at WEST %d, NORTH %d, EAST %d, SOUTH %d\n", maze.isWallWNES[gridX][gridY][WEST], maze.isWallWNES[gridX][gridY][NORTH], maze.isWallWNES[gridX][gridY][EAST], maze.isWallWNES[gridX][gridY][SOUTH]);
				//printf("Tuple at %d, %d\n", gridX, gridY);
				printf("Angle %d, Left %d, straight %d, right %d, back %d\n", angle, left, straight, right, back);
				//printf("Tuple's delta distance is %d\n", mouseRobot.getDeltaDistance());
			
				int decision = ai.makeDecision(mouseRobot.getDeltaDistance(), left, straight, right, back);
					
				// Now that we have made a decision, reset the delta distance
				mouseRobot.resetDeltaDistance();

				if(decision == AI_LEFT){
					mouseRobot.left();
					mouseRobot.updateIsGoingForward(true);
				}
				else if(decision == AI_STRAIGHT){
					mouseRobot.straight();
					mouseRobot.updateIsGoingForward(true);
				}
				else if(decision == AI_RIGHT){
					mouseRobot.right();
					mouseRobot.updateIsGoingForward(true);
				}
				else if(decision == AI_BACK){
					mouseRobot.back();
					mouseRobot.updateIsGoingForward(false);
				}
			}//if mouse is in a grid with a tuple
		}//if
	}//if

	// Draw the mouse!
	mouseRobot.drawMouse();

	// exit local transformation domain
	glPopMatrix();

	// swap buffers for double-buffering
	glutSwapBuffers();
}

// Occurs every time the clock ticks: update the angles and redraw the scene
void tick(int n) {
	// set up next "tick"
	glutTimerFunc(n, tick, n);

	// update the time
	currentTime += 1.0/TICK_INTERVAL;	

	glPushMatrix();

	glTranslatef(-600,-500,0);
	// draw the new scene
	drawScene();

	glPopMatrix();
}

// initializes the animation
void myInit() {

	// set background to gray
	glClearColor(0.5, 0.5, 0.5, 1.0);

	// enable depth comparisons
	glEnable(GL_DEPTH_TEST);

	// enable object material coloring
	glEnable(GL_COLOR_MATERIAL);

	// set shading to "smooth"
	glShadeModel(GL_SMOOTH);

	glTranslatef(100,100,0);

	// Initialize all the tuples (only has to be done once)
	maze.initIsTuple();
}

// handles a mouse-press
void mouse(int btn, int state, int x, int y) {
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// toggle the "spinning" boolean if the left mouse is down
		//spinning = !spinning;
		printf("%d, %d\n", x, y);
	}
	
}

// invoked when the window is resized
void myReshape(int w, int h) {
	// normalize coordinates for window of size w by h
	glViewport(0,0,w,h);

	// set up for specifying projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); // start with identity matrix
	// set the viewsize based on the minimum of the height
	// and the width
	if (w <= h) {
		glOrtho(-VIEWSIZE/2.0, VIEWSIZE/2.0,
			-VIEWSIZE/2.0*(GLfloat)h/(GLfloat)w,
			VIEWSIZE/2.0*(GLfloat)h/(GLfloat)w,
			-VIEWSIZE/2.0, VIEWSIZE/2.0);
	}
	else {
		glOrtho(-VIEWSIZE/2.0*(GLfloat)w/(GLfloat)h, 
			VIEWSIZE/2.0*(GLfloat)w/(GLfloat)h,
			-VIEWSIZE/2.0, VIEWSIZE/2.0,
			-VIEWSIZE/2.0, VIEWSIZE/2.0);
	}

	// go back to the model-view matrix; initialize to identity
	// matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// main program
int main(int argc, char **argv) {
	
	//Maze maze = new Maze();
	
	// seed random number generator
	srand(time(NULL));

	// initialize GLUT using command-line arguments
	glutInit(&argc, argv);

	// set up double-buffering and RGB color
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);

	// create window
	glutInitWindowPosition(50,50);
	glutInitWindowSize(750,750);
	glutCreateWindow("Micromouse Virtual Simulator");

	// initialize
	myInit();

	// set up callback functions
	glutDisplayFunc(drawScene); // display callback
	glutReshapeFunc(myReshape); // window-reshape callback
	glutTimerFunc(TICK_INTERVAL, tick, TICK_INTERVAL); // timer callback
	glutMouseFunc(mouse); // mouse callback

	// start main processing
	glutMainLoop();

	return 0;
}
