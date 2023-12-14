.PHONY: strip clean

CC = gcc -g -flto
FLAGS = -Wall -Wextra -Os -march=native

ALL = results-pipes results-points discharges random-input chlorine-input

all: $(ALL)

results-pipes: results-pipes.c Makefile
	$(CC) $(FLAGS) results-pipes.c -o results-pipes

results-points: results-points.c Makefile
	$(CC) $(FLAGS) results-points.c -o results-points

discharges: discharges.c Makefile
	$(CC) $(FLAGS) discharges.c -o discharges

random-input: random-input.c Makefile
	$(CC) $(FLAGS) random-input.c `pkg-config --cflags --libs gsl` \
		-o random-input
chlorine-input: chlorine-input.c Makefile
	$(CC) $(FLAGS) chlorine-input.c -o chlorine-input


strip:
	make
	strip $(ALL)

clean:
	rm $(ALL)
