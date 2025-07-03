#!/usr/bin/env bash
if [[ $# != 3 ]]; then
	echo "The syntax is: ./build.sh A B C"
	echo "A: 1 on native"
	echo "B: 1 on PGO"
	echo "C: 1 on strip"
	exit 1
fi
if [[ $1 = 1 ]]; then
	a1="--with-native"
fi
if [[ $2 = 1 ]]; then
	a2="--with-pgo"
fi
if [[ $3 = 1 ]]; then
	a3="strip"
fi
aclocal
autoconf
automake --add-missing
./configure $a1 $a2
if test -x "`command -v gmake`"; then
	gmake $a3
else
	make $a3
fi
