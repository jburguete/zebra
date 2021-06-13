.PHONY: clean strip

all: zebra

hdr = config.h tools.h nutrient.h species.h point.h cell.h wall.h pipe.h \
	junction.h inlet.h junction.h network.h results.h simulation.h
src = tools.c nutrient.c species.c cell.c wall.c pipe.c inlet.c junction.c \
	network.c results.h simulation.c main.c
obj = tools.o nutrient.o species.o cell.o wall.o pipe.o inlet.o junction.o \
	network.o results.o simulation.o main.o
cfg = tools.h config.h Makefile
spe = species.h nutrient.h $(cfg)
pip = pipe.h cell.h wall.h point.h $(spe)
net = network.h inlet.h junction.h $(pip)
res = results.h $(net)
sim = simulation.h $(res)
libs = libxml-2.0 glib-2.0

CC = gcc
#COPT = -O3 -march=native -flto
COPT = -O3 -flto
CFLAGS = -c -g -Wall -Wextra $(COPT) `pkg-config --cflags $(libs)`
LDFLAGS = -g -flto -lm `pkg-config --libs $(libs)`

zebra: $(obj)
	$(CC) $(obj) $(LDFLAGS) -o zebra

tools.o: tools.c $(cfg)
	$(CC) $(CFLAGS) tools.c -o tools.o

nutrient.o: nutrient.c nutrient.h $(cfg)
	$(CC) $(CFLAGS) nutrient.c -o nutrient.o

species.o: species.c $(spe)
	$(CC) $(CFLAGS) species.c -o species.o

cell.o: cell.c cell.h point.h $(spe)
	$(CC) $(CFLAGS) cell.c -o cell.o

wall.o: wall.c wall.h cell.h point.h $(spe)
	$(CC) $(CFLAGS) wall.c -o wall.o

pipe.o: pipe.c $(pip)
	$(CC) $(CFLAGS) pipe.c -o pipe.o

inlet.o: inlet.c inlet.h $(pip)
	$(CC) $(CFLAGS) inlet.c -o inlet.o

junction.o: junction.c junction.h $(pip)
	$(CC) $(CFLAGS) junction.c -o junction.o

network.o: network.c $(net)
	$(CC) $(CFLAGS) network.c -o network.o

results.o: results.c $(res)
	$(CC) $(CFLAGS) results.c -o results.o

simulation.o: simulation.c $(sim)
	$(CC) $(CFLAGS) simulation.c -o simulation.o

main.o: main.c $(sim)
	$(CC) $(CFLAGS) main.c -o main.o

clean:
	rm zebra $(obj)

strip:
	make
	strip zebra
