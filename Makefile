# cs3350 hw1
# to compile your project, type make and press enter

all: hw1


lab1: hw1.cpp fonts.h defs.h
	g++ hw1.cpp libggfonts.a -Wall -olab1 -lX11 -lGL -lGLU -lm

clean:
	rm -f hw1
	rm -f *.o

