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

// User changeable defined values
#define WALL_STRAIGHT 0
#define WALL_LEFT 0
#define WALL_RIGHT 0
#define RANDOMNESS 0
// ** NOTE ** Only effective when WALL_LEFT == 0 && WALL_RIGHT == 0
#define TUPLE_RECOGNITION_UNSYNCED 1

// Not user changeable defined values
#define LEFT 0
#define RIGHT 1


int generateRandNum(char* purpose){
  if(strcmp(purpose, "whenTupleSeen") == 0){
    return (rand() % (int)(CENTERED_IN_TUPLE_UNIT/INTERVAL));
  }
  else{ // purpose == whichTupleFirst
    return (rand() % 2);
  }
}

int main(int argc, char* argv[]){
  // Used throughout main function
  int i = 0;
  int whichTupleFirst = 2;
  int whenTupleSeen = 100;

  // If tuple recognition is in real-world mode, then actually choose which
  // missing wall will be seen first
#if (TUPLE_RECOGNITION_UNSYNCED)
  srand(time(NULL));
  whichTupleFirst = generateRandNum("whichTupleFirst");
  whenTupleSeen = generateRandNum("whenTupleSeen");
#endif

  float sSen = SENSOR_TOO_FAR;
  float lSen = LR_CONST_SENSOR_OUT;
  float rSen = LR_CONST_SENSOR_OUT;

  // Step 1: There are walls on both sides, and all seems normal
  for(i = 0; i < (int)(START_END_CLICKS/INTERVAL); i ++){
    printf("(%.3f,%.3f,%.3f)#", lSen, sSen, rSen);
  }

  // TODO: generate random number to see who is first, L or R; then generate random number to see when the second gets to be seen; put the bottom code in the loop below
#if (WALL_STRAIGHT == 1)
  sSen = LR_CONST_SENSOR_OUT;
#endif

  // If perfect consition or real world condition
  if (WALL_LEFT==0&&(!TUPLE_RECOGNITION_UNSYNCED||whichTupleFirst==LEFT)){
    lSen = SENSOR_TOO_FAR;
  }

  if (WALL_RIGHT==0&&(!TUPLE_RECOGNITION_UNSYNCED||whichTupleFirst==RIGHT)){
    rSen = SENSOR_TOO_FAR;
  }

  // Step 2: We have recognized a tuple (change), and we read 20 more
  // sensor inputs until we have gone as many clicks as necessary
  for(i = 0; i < (int)(CENTERED_IN_TUPLE_UNIT/INTERVAL); i++){
    printf("(%.3f,%.3f,%.3f)#", lSen, sSen, rSen);

    if (WALL_LEFT == 0 && whichTupleFirst != LEFT && i == whenTupleSeen){
      lSen = SENSOR_TOO_FAR;
    }

    if (WALL_RIGHT == 0 && whichTupleFirst != RIGHT && i == whenTupleSeen){
      rSen = SENSOR_TOO_FAR;
    }
  }

  // Print a $ to symbolize interrupt from the encoders to tell the PIC
  // that the mouse has already gone CENTERED_IN_TUPLE_UNIT clicks
  printf("$#");

  // Back to normal/expectations of walls on both sides
  sSen = SENSOR_TOO_FAR;
  lSen = LR_CONST_SENSOR_OUT;
  rSen = LR_CONST_SENSOR_OUT;

  // Step 3: Return to normalcy: walls on both sides
  for(i = 0; i < (int)(START_END_CLICKS/INTERVAL); i ++){
    printf("(%.3f,%.3f,%.3f)#", lSen, sSen, rSen);
  }
}
