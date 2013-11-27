# Makefile to compile all .c and .h files

DEST_E = ../executables
DEST_O = ../objects

all: $(DEST_E)/wallRLTurnGen.exe

$(DEST_E)/wallRLTurnGen.exe: $(DEST_O)/wallRLTurnGen.o
	gcc -o $(DEST_E)/wallRLTurnGen.exe $(DEST_O)/wallRLTurnGen.o

$(DEST_O)/wallRLTurnGen.o: wallRLTurnGen.c
	gcc -c wallRLTurnGen.c -o $(DEST_O)/wallRLTurnGen.o

clean:
	rm $(DEST_O)/wallRLTurnGen.o $(DEST_E)/wallRLTurnGen.exe