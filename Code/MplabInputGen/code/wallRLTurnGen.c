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

  // For loop iteration
  float i = 0.0;

  // Set initial sensor values
  float straightSensor;
#if (WALL_STRAIGHT == 0)
  straightSensor = UNIT_ONE_WALL+VERTICAL_LOOKAHEAD+START_END_CLICKS;
#else
  straightSensor = OUT_OF_BOUNDS;
#endif
  float leftSensor = LEFT_MIDDLE;
  float rightSensor = RIGHT_MIDDLE;
  int arrayCounter = 0;

  printf("Clicks until centered: %.3f\n", CLICKS_FWD_UNTIL_CENTERED_IN_UNIT);
  //printf("L sensor vertical lookahead: %.3f\n", LEFT_MIDDLE);

  // We are going straight but there is still a wall on the right side
  while(straightSensor > UNIT_ONE_WALL+VERTICAL_LOOKAHEAD){
    printf("(%.3f,%.3f,%.3f)#", leftSensor, straightSensor, rightSensor);
    straightSensor = straightSensor - INTERVAL;
    arrayCounter++;
  }

  // We have noticed that there is no wall at either the right or left,
  // depending on the macro above; keep moving straight until robot 
  // is in center of unit
  for(i=0.0; i < CLICKS_FWD_UNTIL_CENTERED_IN_UNIT; i += INTERVAL){
    rightSensor = straightSensor*sqrt(2); // TODO correct this value
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

  for(i = 0.0; i < (START_END_CLICKS/INTERVAL); i++){
    printf("(%.3f,%.3f,%.3f)#", leftSensor, straightSensor, rightSensor);
    straightSensor = straightSensor - INTERVAL;
    arrayCounter++;
  }

  //  printf("%d\n", arrayCounter);

}
