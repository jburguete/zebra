#!/usr/bin/env bash
if test -x "$(command -v gsed)"; then
	SED=gsed;
else
	SED=sed;
fi
$SED -i "s/"$1"\."$2"\."$3"/"$4"\."$5"\."$6"/g" $1.$2.$3/{configure.ac,Doxyfile,locales/*/*/*.po} README.*
git mv $1.$2.$3 $4.$5.$6
ln -sf $4.$5.$6 zebra
