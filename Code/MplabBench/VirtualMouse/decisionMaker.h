#ifndef __DECISIONMAKER_H__
#define __DECISIONMAKER_H__

// Data Processing
void minitaurMain(float leSenCl, float strSenCl, float riSenCl);
int isTuple(float sides);

// Hardware communication methods; later will be moved into different .c file
void setMove(int move);

// Logging methods
void printSensorInfo(float l, float s, float r);

#endif