EXE = main
SRC = main.c
LINKS = -lSDL2 -lui
INCDIR = -I/usr/include/SDL2
all:
	gcc -o $(EXE) $(SRC) $(LINKS) $(INCDIR)
