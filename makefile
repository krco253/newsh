#Project 3 Makefile
# newsh

EXE=newsh
OBJ= cole_shell.o
CFLAGS=-std=c++11
SOURCE=cole_shell.cpp

$(EXE): $(OBJ)
	gcc $(CFLAGS) -o $(EXE) $(OBJ)
$(OBJ): $(SOURCE)
	gcc $(CFLAGS) -c $(SOURCE)

