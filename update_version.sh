#!/usr/bin/env bash
if test -x "$(command -v gsed)"; then
	SED=gsed;
else
	SED=sed;
fi
$SED -i "s/"$1"\."$2"\."$3"/"$4"\."$5"\."$6"/g" $1.$2.$3/{configure.ac,Doxyfile,locale/*/*/*.po} README.*
git mv $1.$2.$3 $4.$5.$6
