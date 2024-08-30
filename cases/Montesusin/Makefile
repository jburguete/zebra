.PHONY: strip clean

CC = gcc -g -flto
FLAGS = -Wall -Wextra -Os -march=native

ALL = results-pipes results-points graphic-pipes discharges \
	random-input chlorine-input average-hour map

all: $(ALL)

results-pipes: results-pipes.c Makefile
	$(CC) $(FLAGS) results-pipes.c -o results-pipes

results-points: results-points.c Makefile
	$(CC) $(FLAGS) results-points.c -o results-points

graphic-pipes: graphic-pipes.c Makefile
	$(CC) $(FLAGS) graphic-pipes.c -o graphic-pipes

discharges: discharges.c Makefile
	$(CC) $(FLAGS) discharges.c -o discharges

random-input: random-input.c Makefile
	$(CC) $(FLAGS) random-input.c `pkg-config --cflags --libs gsl` \
		-o random-input
chlorine-input: chlorine-input.c Makefile
	$(CC) $(FLAGS) chlorine-input.c -o chlorine-input

average-hour: average-hour.c Makefile
	$(CC) $(FLAGS) average-hour.c -o average-hour

map: map.c Makefile
	$(CC) $(FLAGS) map.c -lm -o map

strip:
	make
	strip $(ALL)

clean:
	rm $(ALL)
