#!/usr/bin/env bash
make
for i in 0,6-1 1,8-1; do
	pushd v$i
	../shell $i.json
	gnuplot plot
	popd
done
lualatex shell
lualatex shell
