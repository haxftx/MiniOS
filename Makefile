#!/bin/bash
CC=gcc
CFLAGS= -Wall -Wall -Wextra  -O3
IN = in_file
OUT = out_file
EXEC = miniOS
FILE = miniOS.c

build: $(EXEC)

miniOS.o:miniOS.c
	$(CC) $(CFLAGS) -o miniOS.o -c $(FILE)
miniOS:miniOS.o
		$(CC) $(CFLAGS) -o miniOS miniOS.o 
run:$(EXEC)
		./$(EXEC) $(IN) $(OUT) 

.PHONY: clean

clean:
	rm -f *.o $(EXEC)

checkMem:
	valgrind --leak-check=yes --track-origins=yes ./$(EXEC) $(IN) $(OUT)
