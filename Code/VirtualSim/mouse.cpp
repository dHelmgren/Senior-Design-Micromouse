#include "mouse.h"
#include "maze.h"

Mouse::Mouse(){
	// The mouse starts out at a 90 degree angle (even though it is
	// going "north"). In OpenGL, a zero degree angle would mean that 
	// the mouse is moving right.
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
	// Ensure no negative angles
	if(angle < 0){
		angle += 360;
	}
	// Ensure no angles over 270 degrees
	angle = angle%360;
}

void Mouse::straight(){
	// Do nothing, because when we go straight, we don't
	// have to adjust the angle at all
}

void Mouse::back(){
	// MODIFY THIS CODE if you ever decide to implement the mouse traveling
	// backward. Right now, because the mouse never travels backward, if the
	// AI returns NODE_BACK, then Microtaur does a 180 degree turn instead.
	angle += 180;
	angle = angle%360;
}

void Mouse::drawMouse(){
	// Allow the transformations made here to be "popped" later
	glPushMatrix();

	// Rotate the world (NOT the mouse object)
	glRotatef(angle, 0, 0, 1);

	// Set the color to light blue
	glColor3f(0,150,255);

	// This line tells OpenGL that we are aout to draw a quadrilateral (aka the rectangle box
	// for the mouse)
	glBegin(GL_QUADS);

	// Place each of the four vertices of the mosue
	glVertex2f(-1*MOUSE_HALF_HEIGHT, -1*MOUSE_HALF_WIDTH);
	glVertex2f(-1*MOUSE_HALF_HEIGHT, MOUSE_HALF_WIDTH);
	glVertex2f(MOUSE_HALF_HEIGHT, MOUSE_HALF_WIDTH);
	glVertex2f(MOUSE_HALF_HEIGHT, -1*MOUSE_HALF_WIDTH);
	
	// We are done drawing the mouse
	glEnd();

	// Remove all of the transformations we made in this function
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
		// DEBUG statement
		//printf("Delta distance is %d\n", deltaDistance);
	}
	//printf("Mouse grid location: %d, %d\n", gridLocX, gridLocY);
}

bool Mouse::isInCenterOfGrid(int mazeStartXCoord, int mazeStartYCoord){
	// It is important to know when the mouse is in the center of the grid because
	// as soon as this method returns true, then it is time to call the AI method (which
	// is called in main.cpp)
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
	// This is equal to the number of units the mouse has traveled since the last
	// decision point
	return deltaDistance;
}

void Mouse::resetDeltaDistance(){
	// This is called right after we have called the AI agent for a decision
	deltaDistance = 0;
}

void Mouse::updateIsGoingForward(bool newIsGoingForward){
	isGoingForward = newIsGoingForward;
}

bool Mouse::getIsGoingForward(){
	return isGoingForward;
}