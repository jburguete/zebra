.PHONY: clean strip

all: zebra

hdr = config.h tools.h nutrient.h species.h point.h cell.h pipe.h junction.h \
	inlet.h
src = tools.c nutrient.c species.c inlet.c main.c
obj = tools.o nutrient.o species.o inlet.o main.o
cfg = config.h tools.h Makefile
libs = libxml-2.0 glib-2.0

CC = gcc
COPT = -O3 -march=native -flto
CFLAGS = -c -Wall -Wextra $(COPT) `pkg-config --cflags $(libs)`
LDFLAGS = -flto -lm `pkg-config --libs $(libs)`

zebra: $(obj)
	$(CC) $(obj) $(LDFLAGS) -o zebra

tools.o: tools.c $(cfg)
	$(CC) $(CFLAGS) tools.c -o tools.o

nutrient.o: nutrient.c nutrient.h $(cfg)
	$(CC) $(CFLAGS) nutrient.c -o nutrient.o

species.o: species.c species.h $(cfg)
	$(CC) $(CFLAGS) species.c -o species.o

inlet.o: inlet.c inlet.h species.h nutrient.h $(cfg)
	$(CC) $(CFLAGS) inlet.c -o inlet.o

main.o: main.c $(hdr) Makefile
	$(CC) $(CFLAGS) main.c -o main.o

clean:
	rm zebra $(obj)

strip:
	make
	strip zebra
