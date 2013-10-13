#include "maze.h"
#include "mouse.h"

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

// The current spinning angle
static GLfloat spinningAngle = 0.0;

// boolean that tells whether we are spinning (initially false)
static int spinning = 0;

Maze maze = Maze();
Mouse mouseRobot = Mouse();

// draws our scene
void drawScene() {
	// clear scene, handle depth
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// enter global transformation domain
	glPushMatrix();

	//********************************************************************************
	//********* your modifications to the drawScene method should start here *********
	// for now, just draw a single blue torus that is growing and shrinking
	//********************************************************************************

	// Draw the maze!
	maze.initMazeArray();
	maze.drawMaze();

	// Draw the mouse!
	mouseRobot.drawMouse(currentTime);

	//********************************************************************************
	//********* your modifications to the drawScene method should end here *********
	//********************************************************************************

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
	printf("Drawing new scene\n");

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
