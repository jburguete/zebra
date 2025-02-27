#!/usr/bin/env bash
./build-release.sh
./clean.sh
mv bin/release/* .
rm -rf bin
for i in 0,6-1 1,8-1; do
	pushd v$i
	../shell $i.json
	gnuplot plot
  for j in *.eps; do
    epstopdf $j;
  done
	popd
done
lualatex shell
lualatex shell
