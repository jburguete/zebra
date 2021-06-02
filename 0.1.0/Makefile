.PHONY: clean strip

all: zebra

hdr = config.h tool.h nutrient.h species.h point.h cell.h pipe.h junction.h
src = nutrient.c species.c main.c
obj = nutrient.o species.o main.o
cfg = config.h tool.h Makefile
libs = libxml-2.0 glib-2.0

CC = gcc
COPT = -O3 -march=native -flto
CFLAGS = -c -Wall -Wextra $(COPT) `pkg-config --cflags $(libs)`
LDFLAGS = -flto -lm `pkg-config --libs $(libs)`

zebra: $(obj)
	$(CC) $(obj) $(LDFLAGS) -o zebra

nutrient.o: nutrient.c nutrient.h $(cfg)
	$(CC) $(CFLAGS) nutrient.c -o nutrient.o

species.o: species.c species.h $(cfg)
	$(CC) $(CFLAGS) species.c -o species.o

main.o: main.c $(hdr) Makefile
	$(CC) $(CFLAGS) main.c -o main.o

clean:
	rm zebra $(obj)

strip:
	make
	strip zebra
