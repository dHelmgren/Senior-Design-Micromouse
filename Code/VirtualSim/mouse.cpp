#include "mouse.h"
#include "maze.h"

Mouse::Mouse(){
	xCoord = PIXELS_PER_SQUARE/2 - MOUSE_HALF_WIDTH;
	yCoord = PIXELS_PER_SQUARE/2 - MOUSE_HALF_HEIGHT;
	printf("xcoord %f, ycoord %f\n", xCoord, yCoord);
	angle = 90.0;
}

void Mouse::drawMouse(static float time){
	glPushMatrix();
	glRotatef(angle*time, 0, 0, 1);
	glColor3f(0,150,255);
	glBegin(GL_QUADS);
	glVertex2f(xCoord + MOUSE_WIDTH_PX, yCoord + MOUSE_HEIGHT_PX);
	glVertex2f(xCoord, yCoord + MOUSE_HEIGHT_PX);
	glVertex2f(xCoord, yCoord);
	glVertex2f(xCoord + MOUSE_WIDTH_PX, yCoord);
	glEnd();
	glPopMatrix();
}