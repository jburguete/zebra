.PHONY: copy

analysis_data = dn200pn16-1 dn250pn10-1 dn300pn10-1 dn300pn10-2 dn250pn10-3 \
	dn300pn10-3
figs = rl-re.eps

all: analysis ensayos-center.pdf analisis-center.pdf

analysis: analysis.c $(analysis_data) plot Makefile
	gcc -flto -Wall -Wextra -Os -march=native analysis.c -lm -o analysis

ensayos-center.pdf: ensayos-center.tex $(figs) Makefile
	lualatex ensayos-center
	lualatex ensayos-center

rl-re.eps: plot
	gnuplot plot

analisis-center.pdf: analisis-center.tex $(figs) Makefile
	lualatex analisis-center
	lualatex analisis-center

copy:
	tar cJf ensayos-center.tar.xz *.tex plot Makefile dn* *.xlsx
