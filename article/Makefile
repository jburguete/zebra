.PHONE: strip clean

CC = gcc -flto -g
CFLAGS = -march=native -Os -Wall -Wextra
LIBS = -lm

all: article.pdf combine search

combine: combine.c Makefile
	$(CC) $(CFLAGS) combine.c -o combine

search: search.c Makefile
	$(CC) $(CFLAGS) search.c $(LIBS) -o search

article.pdf: article.tex bib.bib Makefile
	lualatex article
	lualatex article
	bibtex article
	lualatex article
