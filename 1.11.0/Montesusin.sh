#!/usr/bin/env bash
for i in ../cases/Montesusin/results-pipe-*; do
	./translate 0 $i $i.csv
done
