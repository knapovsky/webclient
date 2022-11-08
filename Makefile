########################################################################
# Makefile pro IPK Projekt #1
# autor:	Martin Knapovsky
#			xknapo02@stud.fit.vutbr.cz
########################################################################
CC=g++
CFLAGS=-c -Wall

all: webinfo

webinfo: main.o
			$(CC) main.o -o webinfo

main.o: main.cpp
			$(CC) $(CFLAGS) main.cpp

clean: 
			rm -rf main.o
