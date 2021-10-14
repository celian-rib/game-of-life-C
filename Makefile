CFLAGS = --std=c11 -Wall -Wextra -pedantic

life: life.o


clean:
	rm -rf *.o life