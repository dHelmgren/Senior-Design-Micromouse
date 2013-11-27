/* wallRLTurnGen.c
 * 
 * Describes the output of the IR sensors when we are turning left or
 * right in the "traditional turn" way. The turn looks like this:
 *    ____         ____
 *   |  __    OR   __  |
 *   | |             | |
 *   | |             | |
 */

#include "constants.h"

int main(int argc, char* argv[]){

  float straightSensor = 17.4/CLICKS_PER_CM+3;
  float leftSensor = LEFT_MIDDLE/CLICKS_PER_CM;
  float rightSensor = RIGHT_MIDDLE/CLICKS_PER_CM;
  int arrayCounter = 0;

  // We are going straight but there is still a wall on the right side
  while(straightSensor > 17.4/CLICKS_PER_CM){
    printf("(%.3f,%.3f,%.3f)#", leftSensor, straightSensor, rightSensor);
    straightSensor = straightSensor - .5;
    arrayCounter++;
  }

  // We have noticed that there is no wall at right; keep moving straight
  // until robot in center of unit
  while (straightSensor >= (17.4-12.5)/(2*CLICKS_PER_CM)){
    rightSensor = straightSensor*sqrt(2);
    printf("(%.3f,%.3f,%.3f)#", leftSensor, straightSensor, rightSensor);
    straightSensor = straightSensor - .5;
    arrayCounter++;
  }

  // We have completed the turn, thus return to normalcy
  straightSensor = (17.4+18)/CLICKS_PER_CM;
  rightSensor = RIGHT_MIDDLE/CLICKS_PER_CM;
  while(straightSensor >= (17.4+18)/CLICKS_PER_CM-3){
    printf("(%.3f,%.3f,%.3f)#", leftSensor, straightSensor, rightSensor);
    straightSensor = straightSensor - .5;
    arrayCounter++;
  }

  //  printf("%d\n", arrayCounter);

}