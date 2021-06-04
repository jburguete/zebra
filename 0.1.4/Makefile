.PHONY: clean strip

all: zebra

hdr = config.h tools.h nutrient.h species.h point.h cell.h pipe.h junction.h \
	inlet.h outlet.h network.h
src = tools.c nutrient.c species.c inlet.c outlet.c network.c main.c
obj = tools.o nutrient.o species.o inlet.o outlet.o network.o main.o
cfg = tools.h config.h Makefile
spe = species.h nutrient.h $(cfg)
net = network.h outlet.h inlet.h junction.h pipe.h cell.h point.h $(spe)
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

species.o: species.c $(spe)
	$(CC) $(CFLAGS) species.c -o species.o

inlet.o: inlet.c inlet.h $(spe)
	$(CC) $(CFLAGS) inlet.c -o inlet.o

outlet.o: outlet.c outlet.h $(cfg)
	$(CC) $(CFLAGS) outlet.c -o outlet.o

network.o: network.c $(net)
	$(CC) $(CFLAGS) network.c -o network.o

main.o: main.c $(net)
	$(CC) $(CFLAGS) main.c -o main.o

clean:
	rm zebra $(obj)

strip:
	make
	strip zebra
