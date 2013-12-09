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
#define WALL_RIGHT 0

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
  // Print a $ to symbolize interrupt from the encoders to tell the PIC
  // that the mouse has already gone CENTERED_IN_TUPLE_UNIT clicks
  printf("$#");

  sSen = SENSOR_TOO_FAR;
  lSen = LR_CONST_SENSOR_OUT;
  rSen = LR_CONST_SENSOR_OUT;

  // Step 3: Return to normalcy: walls on both sides
  for(i = 0; i < START_END_CLICKS; i += INTERVAL){
    printf("(%.3f,%.3f,%.3f)#", lSen, sSen, rSen);
  }

}
