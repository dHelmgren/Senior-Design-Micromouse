#include "mouse.h"
#include "maze.h"

//Mouse::Mouse(){
//	xCoord = PIXELS_PER_SQUARE/2 - MOUSE_HALF_WIDTH;
//	yCoord = PIXELS_PER_SQUARE/2 - MOUSE_HALF_HEIGHT;
//	printf("xcoord %f, ycoord %f\n", xCoord, yCoord);
//	angle = 90.0;
//}

Mouse::Mouse(){
	angle = 90;
	direction = STRAIGHT;
	gridLocX = 0;
	gridLocY = 1;
}

void Mouse::left(){
	angle -= 90;
}

void Mouse::right(){
	angle += 90;
}

void Mouse::straight(){
	// Do nothing, for now
}

void Mouse::drawMouse(){
	glPushMatrix();
	glRotatef(angle, 0, 0, 1);
	glColor3f(0,150,255);
	glBegin(GL_QUADS);
	glVertex2f(-1*MOUSE_HALF_HEIGHT, -1*MOUSE_HALF_WIDTH);
	glVertex2f(-1*MOUSE_HALF_HEIGHT, MOUSE_HALF_WIDTH);
	glVertex2f(MOUSE_HALF_HEIGHT, MOUSE_HALF_WIDTH);
	glVertex2f(MOUSE_HALF_HEIGHT, -1*MOUSE_HALF_WIDTH);
	
	glEnd();
	glPopMatrix();
}

void Mouse::updateMouseLocation(int mazeXCoord, int mazeYCoord){
	gridLocX = std::abs(mazeXCoord)/PIXELS_PER_SQUARE;
	gridLocY = std::abs(mazeYCoord)/PIXELS_PER_SQUARE;
	//printf("Mouse grid location: %d, %d\n", gridLocX, gridLocY);
}

int Mouse::getAngle(){
	return angle;
}

int Mouse::getGridLocX(){
	return gridLocX;
}

int Mouse::getGridLocY(){
	return gridLocY;
}