#!/usr/bin/env bash
for i in ../cases/Montesusin/results-pipe-*; do
	./translate $i $i.csv
done
