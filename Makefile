CFLAGS = --std=c11 -Wall -Wextra -pedantic
LDFLAGS=-lncurses

life: life.o

clean:
	rm -rf *.o life *.out