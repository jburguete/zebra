.PHONY: copy

figs = rl-re.eps

ensayos-center.pdf: ensayos-center.tex $(figs) Makefile
	lualatex ensayos-center
	lualatex ensayos-center

rl-re.eps: plot
	gnuplot plot

copy:
	tar cJf ensayos-center.tar.xz *.tex plot Makefile dn* *.xlsx
