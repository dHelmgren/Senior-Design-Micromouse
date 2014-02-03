# MPLAB IDE generated this makefile for use with GNU make.
# Project: mouse.mcp
# Date: Sun Feb 02 20:48:19 2014

AS = MPASMWIN.exe
CC = mcc18.exe
LD = mplink.exe
AR = mplib.exe
RM = rm

mouse.cof : main.o
	$(LD) /p18F27J13 /l"C:\Program Files (x86)\Microchip\mplabc18\v3.46\lib" "main.o" /u_CRUNTIME /o"mouse.cof" /M"mouse.map" /W

main.o : main.c ../../../../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/timers.h ../../../../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/delays.h ../../../../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/adc.h main.c ../../../../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/p18f27j13.h ../../../../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/pconfig.h ../../../../../../../../../Program\ Files\ (x86)/Microchip/mplabc18/v3.46/h/p18cxxx.h
	$(CC) -p=18F27J13 /i"C:\Program Files (x86)\Microchip\mplabc18\v3.46\h" "main.c" -fo="main.o" -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-

clean : 
	$(RM) "main.o" "mouse.cof" "mouse.hex"

