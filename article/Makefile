.PHONE: strip clean

CC = gcc -flto -g
CFLAGS = -march=native -Os -Wall -Wextra
LIBS = -lm

fig = discharge-hour.eps

all: article-new.pdf supplementary.pdf combine search leyenda.pdf $(fig)

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

article-new.tex: article.tex article-old.tex macro.tex bib.bib Makefile
	latexdiff article-old.tex article.tex > article-new.tex
	sed -i 's/\\DIFaddend {EqMusselSettlementConcentration}/{EqMusselSettlementConcentration}\\DIFaddend/' article-new.tex
	sed -i 's/\\DIFaddend {EqMusselSettlementProbability}/{EqMusselSettlementProbability}\\DIFaddend/' article-new.tex
	sed -i 's/\\DIFaddend {EqMusselDecayTau}/{EqMusselDecayTau}\\DIFaddend/' article-new.tex
	sed -i 's/\\DIFdel{EqTheta}//' article-new.tex
	sed -i 's/\\DIFadd{EqTheta}/EqTheta/' article-new.tex
	vim article-new.tex

article-new.pdf: article-new.tex leyenda.pdf $(fig)
	pdflatex article-new
	bibtex article-new
	pdflatex article-new
	pdflatex article-new
