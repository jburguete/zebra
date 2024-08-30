.PHONE: strip clean

CC = gcc -flto -g
CFLAGS = -march=native -Os -Wall -Wextra
LIBS = -lm

fig = discharge-hour.eps

all: article.pdf supplementary.pdf combine search leyenda.pdf $(fig)

$(fig): plot
	gnuplot plot

combine: combine.c Makefile
	$(CC) $(CFLAGS) combine.c -o combine

search: search.c Makefile
	$(CC) $(CFLAGS) search.c $(LIBS) -o search

leyenda.pdf: leyenda.tex Makefile
	latex leyenda
	dvipdf leyenda

supplementary.pdf: supplementary.tex macro.tex leyenda.pdf $(fig) Makefile
	pdflatex supplementary
	pdflatex supplementary

article.pdf: article.tex macro.tex bib.bib leyenda.pdf $(fig) Makefile
	pdflatex article
	bibtex article
	pdflatex article
	pdflatex article
