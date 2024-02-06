#!/usr/bin/env bash
for i in ../cases/Montesusin/results-pipe-*; do
	./translate csv $i $i.csv
done
