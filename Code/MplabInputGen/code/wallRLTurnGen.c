/* wallRLTurnGen.c
 * 
 * Describes the output of the IR sensors when we are turning left or
 * right in the "traditional turn" way. The turn looks like this:
 *
 *                CASE 0    |      CASE 1    |     CASE 2
 * _________________________|________________|_________________________
 *            |             |                |
 * WALL       |   ____      |       ____     |     _______
 * STRAIGHT 0 |   __  |     |      |  __     |     __   __
 *            |     | |     |      | |       |       | |
 *            |     | |     |      | |       |       | |
 *            |             |                |
 * ___________|_____________|________________|_________________________
 *            |             |                |
 * WALL       |             |                |
 * STRAIGHT 1 |     | |     |      | |       |       | |
 *            |  ___| |     |      | |___    |    ___| |___
 *            |  ___  |     |      |  ___    |    ___   ___
 *            |     | |     |      | |       |       | |
 *            |     | |     |      | |       |       | |
 *            |             |                |
 *            |             |                |
 */

#include "constants.h"


// Determines left, right, or both
#define CASE 3
// Determines whether there is wall ahead or out of bounds
#define WALL_STRAIGHT 0




int main(int argc, char* argv[]){

  float straightSensor;
#if (WALL_STRAIGHT == 0)
  straightSensor = UNIT_ONE_WALL+START_END_CLICKS;
#else
  straightSensor = OUT_OF_BOUNDS;
#endif
  float leftSensor = LEFT_MIDDLE;
  float rightSensor = RIGHT_MIDDLE;
  int arrayCounter = 0;

  // We are going straight but there is still a wall on the right side
  while(straightSensor > UNIT_ONE_WALL){
    printf("(%.3f,%.3f,%.3f)#", leftSensor, straightSensor, rightSensor);
    straightSensor = straightSensor - INTERVAL;
    arrayCounter++;
  }

  // We have noticed that there is no wall at either the right or left,
  // depending on the macro above; keep moving straight until robot 
  // is in center of unit
  while (straightSensor >= MIDDLE_DIST_FROM_WALL){
    rightSensor = straightSensor*sqrt(2);
#if (CASE == 1)
    printf("(%.3f,%.3f,%.3f)#", leftSensor, straightSensor, rightSensor);
#elif (CASE == 2)
    printf("(%.3f,%.3f,%.3f)#", rightSensor, straightSensor, rightSensor);
#else
    printf("(%.3f,%.3f,%.3f)#", rightSensor, straightSensor, leftSensor);
#endif
    straightSensor = straightSensor - INTERVAL;
    arrayCounter++;
  }

  // We have completed the turn, thus return to normalcy
  straightSensor = OUT_OF_BOUNDS;
  rightSensor = RIGHT_MIDDLE/CLICKS_PER_CM;
  int i;
  for(i = 0; i < (START_END_CLICKS/INTERVAL); i++){
    printf("(%.3f,%.3f,%.3f)#", leftSensor, straightSensor, rightSensor);
    straightSensor = straightSensor - INTERVAL;
    arrayCounter++;
  }

  //  printf("%d\n", arrayCounter);

}
