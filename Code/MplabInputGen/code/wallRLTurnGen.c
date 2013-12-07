/* wallRLTurnGen.c
 * 
 * Describes the output of the IR sensors when we are turning left or
 * right in the "traditional turn" way. The turn looks like this:
 * 
 *              WALL LEFT 0    WALL LEFT 1        WALL LEFT 0    WALL LEFT 1
 *              WALL RIGHT 1   WALL RIGHT 0      WALL RIGHT 0    WALL RIGHT 1
 * ___________|_____________|________________|_________________|________________
 *            |             |                |                 |
 * WALL       |   ____      |       ____     |     _______     |      _
 * STRAIGHT 1 |   __  |     |      |  __     |     __   __     |     | |
 *            |     | |     |      | |       |       | |       |     | |
 *            |     | |     |      | |       |       | |       |     | |
 *            |             |                |                 |   DEAD ENO
 * ___________|_____________|________________|_________________|________________
 *            |             |                |                 |
 * WALL       |             |                |                 |
 * STRAIGHT 0 |     | |     |      | |       |       | |       |     | |
 *            |  ___| |     |      | |___    |    ___| |___    |     | |
 *            |  ___  |     |      |  ___    |    ___   ___    |     | |
 *            |     | |     |      | |       |       | |       |     | |
 *            |     | |     |      | |       |       | |       |  DO NOT USE
 * ___________|_____________|________________|_________________|________________
 *            |             |                |                 |
 */

#include "constants.h"

#define WALL_STRAIGHT 1
#define WALL_LEFT 0
#define WALL_RIGHT 1

int main(int argc, char* argv[]){
  // Loop iterator
  float i = 0.0;

  float sSen = SENSOR_TOO_FAR;
  float lSen = LR_CONST_SENSOR_OUT;
  float rSen = LR_CONST_SENSOR_OUT;

  // Step 1: There are walls on both sides, and all seems normal
  for(i = 0; i < START_END_CLICKS; i += INTERVAL){
    printf("(%.3f,%.3f,%.3f)#", lSen, sSen, rSen);
  }

#if (WALL_STRAIGHT == 1)
  sSen = LR_CONST_SENSOR_OUT;
#endif

#if (WALL_LEFT == 0)
  lSen = SENSOR_TOO_FAR;
#endif

#if (WALL_RIGHT == 0)
  rSen = SENSOR_TOO_FAR;
#endif

  // Step 2: We have recognized a tuple (change), and we read 20 more
  // sensor inputs until we have gone as many clicks as necessary
  for(i = 0; i < CENTERED_IN_TUPLE_UNIT; i += INTERVAL){
    printf("(%.3f,%.3f,%.3f)#", lSen, sSen, rSen);
  }

  sSen = SENSOR_TOO_FAR;
  lSen = LR_CONST_SENSOR_OUT;
  rSen = LR_CONST_SENSOR_OUT;

  // Step 3: Return to normalcy: walls on both sides
  for(i = 0; i < START_END_CLICKS; i += INTERVAL){
    printf("(%.3f,%.3f,%.3f)#", lSen, sSen, rSen);
  }

}










/*


























// Determines left, right, or both
// #define CASE 3
// Determines whether there is wall ahead or out of bounds
// #define WALL_STRAIGHT 0




int main2(int argc, char* argv[]){

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

  printf("Clicks until centered: %.3f\n", R_SENSOR_VERTICAL_LOOKAHEAD);
  printf("L sensor vertical lookahead: %.3f\n", L_SENSOR_VERTICAL_LOOKAHEAD);

  // We are going straight until we hit either the R_SENSOR_VERTICAL_LOOKAHEAD
  // or the L_SENSOR_VERTICAL_LOOKAHEAD boundaries
  while((straightSensor > UNIT_ONE_WALL+R_SENSOR_VERTICAL_LOOKAHEAD) ||
	straightSensor > UNIT_ONE_WALL+L_SENSOR_VERTICAL_LOOKAHEAD){
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
*/
