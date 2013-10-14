#include "mouse.h"
#include "maze.h"

Mouse::Mouse(){
	angle = 90;
	isGoingForward = true;
	gridLocX = 0;
	gridLocY = 0;
	deltaDistance = 0;
	prevDeltaX = 0;
	prevDeltaY = 0;
}

void Mouse::left(){
	angle += 90;
	angle = angle%360;
}

void Mouse::right(){
	angle -= 90;
	if(angle < 0){
		angle += 360;
	}
	angle = angle%360;
}

void Mouse::straight(){
	// Do nothing, for now
}

void Mouse::back(){
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
	// Calculate the mouse's new location
	gridLocX = std::abs(mazeXCoord)/PIXELS_PER_SQUARE;
	gridLocY = std::abs(mazeYCoord)/PIXELS_PER_SQUARE;
	// If the mouse had moved an entire unit, then update its delta distance
	if(gridLocX != prevDeltaX || gridLocY != prevDeltaY){
		deltaDistance++;
		prevDeltaX = gridLocX;
		prevDeltaY = gridLocY;
		//printf("Delta distance is %d\n", deltaDistance);
	}
	//printf("Mouse grid location: %d, %d\n", gridLocX, gridLocY);
}

bool Mouse::isInCenterOfGrid(int mazeStartXCoord, int mazeStartYCoord){
	if(((mazeStartXCoord + (PIXELS_PER_SQUARE)/2) % PIXELS_PER_SQUARE == 0) &&
		((mazeStartYCoord + (PIXELS_PER_SQUARE)/2) % PIXELS_PER_SQUARE == 0)){
			return true;
	}
	return false;
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

int Mouse::getDeltaDistance(){
	//int returnDistance = deltaDistance;
	//deltaDistance = 0;
	return deltaDistance;
}

void Mouse::resetDeltaDistance(){
	deltaDistance = 0;
}

void Mouse::updateIsGoingForward(bool newIsGoingForward){
	isGoingForward = newIsGoingForward;
}

bool Mouse::getIsGoingForward(){
	return isGoingForward;
}