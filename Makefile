STANDARD=-g -Wall -std=c99
CC=gcc
TARGET=build/test
OBJS= build/chessian.o build/ch_test.o build/util.o
CFLAGS=$(STANDARD)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@

build/chessian.o: src/chessian.c src/chessian.h 
	$(CC) $(CFLAGS) -c src/chessian.c -o $@

build/ch_test.o: src/ch_test.c src/chessian.h 
	$(CC) $(CFLAGS) -c src/ch_test.c -o $@

build/util.o: src/util.c src/util.h
	$(CC) $(CFLAGS) -c src/util.c -o $@

.PHONY:
	clean
	VARS

VARS:
	@echo $(OBJS)
	@echo $(CFLAGS)

clean:
	rm -rf build/*
# del build\\* /q
