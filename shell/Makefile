.PHONY: strip clean

CFLAGS = `pkg-config --cflags glib-2.0 json-glib-1.0` \
	-Wall -Wextra -O3 -flto -g #-march=native
LIBS = `pkg-config --libs json-glib-1.0 glib-2.0` -lm -flto -g
CFLAGS2 = -Wall -Wextra -Os -flto -g #-march=native
LIBS2 = -flto -g

all = shell process process2

all: $(all)

shell: shell.c Makefile
	$(CC) $(CFLAGS) shell.c -o shell $(LIBS)

process: process.c Makefile
	$(CC) $(CFLAGS2) process.c -o process $(LIBS2)

process2: process2.c Makefile
	$(CC) $(CFLAGS2) process2.c -o process2 $(LIBS2) -lm

strip:
	make
	strip $(all)

clean:
	rm $(all)
