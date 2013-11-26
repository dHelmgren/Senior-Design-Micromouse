#include <stdio.h>
#include <math.h>

int main(int argc, char* argv[]){

  float straightSensor = 17.4/0.98+3;
  float leftSensor = 3.9*sqrt(2)/0.98;
  float rightSensor = 3.9*sqrt(2)/0.98;
  int arrayCounter = 0;

  // We are going straight but there is still a wall
  while(straightSensor > 17.4/0.98){
    printf("(%.3f,%.3f,%.3f)#", leftSensor, straightSensor, rightSensor);
    straightSensor = straightSensor - .5;
    arrayCounter++;
  }

  // We have noticed that there is no wall at right; keep moving straight
  // until robot in center of unit
  while (straightSensor >= (17.4-12.5)/(2*0.98)){
    rightSensor = straightSensor*sqrt(2);
    printf("(%.3f,%.3f,%.3f)#", leftSensor, straightSensor, rightSensor);
    straightSensor = straightSensor - .5;
    arrayCounter++;
  }

  // We have completed the turn, thus return to normalcy
  straightSensor = (17.4+18)/0.98;
  rightSensor = 3.9*sqrt(2)/0.98;
  while(straightSensor >= (17.4+18)/0.98-3){
    printf("(%.3f,%.3f,%.3f)#", leftSensor, straightSensor, rightSensor);
    straightSensor = straightSensor - .5;
    arrayCounter++;
  }

  //  printf("%d\n", arrayCounter);

}
