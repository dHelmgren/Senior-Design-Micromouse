#define MOUSE_WIDTH_CM 7	//centimeters
#define MOUSE_HEIGHT_CM 9	//centimeters
#define MOUSE_WIDTH_PX (PIXELS_PER_SQUARE/18)*MOUSE_WIDTH_CM
#define MOUSE_HEIGHT_PX (PIXELS_PER_SQUARE/18)*MOUSE_HEIGHT_CM 
#define MOUSE_HALF_WIDTH (MOUSE_WIDTH_PX)/2
#define MOUSE_HALF_HEIGHT (MOUSE_HEIGHT_PX)/2

class Mouse{
public:
	Mouse();
	float xCoord;
	float yCoord;
	float angle;
	void drawMouse(static float time);
};