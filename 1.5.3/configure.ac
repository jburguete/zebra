AC_PREREQ([2.69])
AC_INIT([zebra],[1.5.3],[https://github.com/jburguete/zebra])
#AC_CONFIG_HEADERS([config.h])

# Checks for operative systems
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE
AC_MSG_NOTICE([Build: $build])
AC_MSG_NOTICE([Host: $host])
win=0
#native=1
if test $host = "x86_64-w64-mingw32"; then
	AC_MSG_NOTICE([Microsoft Windows 64 bits + MinGW])
	win=1
elif test $host = "x86_64-pc-cygwin"; then
	AC_MSG_NOTICE([Microsoft Windows 64 bits + Cygwin])
	win=1
elif test `uname -s` = "Darwin"; then
	AC_MSG_NOTICE([MacOS])
elif test `uname -s` = "FreeBSD"; then
	AC_MSG_NOTICE([FreeBSD])
	LDFLAGS="$LDFLAGS -L/usr/local/lib"
elif test `uname -s` = "NetBSD"; then
	AC_MSG_NOTICE([NetBSD])
elif test `uname -s` = "OpenBSD"; then
	AC_MSG_NOTICE([OpenBSD])
	#native=0
elif test `uname -s` = "DragonFly"; then
	AC_MSG_NOTICE([DragonFly BSD])
	LDFLAGS="$LDFLAGS -L/usr/local/lib"
	CPPFLAGS="$CPPFLAGS -I/usr/local/include"
elif test `uname -s` = "SunOS"; then
	AC_MSG_NOTICE([SunOS])
	AC_SUBST(GNU, [/usr/gnu/bin/])
fi

# Checks for programs
AC_PROG_CC([gcc-13 gcc13 gcc-12 gcc12 gcc-11 gcc11 gcc-10 gcc10 egcc gcc clang])
AC_MSG_NOTICE([Compiler: $CC])
$CC -v
AC_PROG_MAKE_SET
AC_LANG([C])
PKG_PROG_PKG_CONFIG
if test $win = 1; then
	AC_SUBST(SO, [.dll])
	AC_SUBST(EXE, [.exe])
else
	AC_SUBST(SO, [.so])
fi
if test "`command -v gmake`"; then
	AC_SUBST(MAKE, [gmake])
else
	AC_SUBST(MAKE, [make])
fi

# Checking -march=native compiler flag
AC_ARG_WITH([native],
	AS_HELP_STRING([--with-native],
		[Build using -march=native compiler flag]))
AC_MSG_NOTICE([With native: $with_native])
if test "x$with_native" = "xyes"; then
#	if test "x$native" = "x1"; then
	compiler=$CC
	CC="$CC -march=native"
	AC_LINK_IFELSE([AC_LANG_PROGRAM([[]], [[]])], [arch=1], [arch=0])
	CC=$compiler
	if test $arch = 0; then
		AC_MSG_NOTICE([checking for -march=native... no])
		AC_SUBST(ARCH, "")
	else
		AC_MSG_NOTICE([checking for -march=native... yes])
		AC_SUBST(ARCH, "-march=native")
	fi
#	fi
fi

# Checking -flto compiler flag
compiler=$CC
CC="$CC -flto"
AC_LINK_IFELSE([AC_LANG_PROGRAM([[]], [[]])], [lto=1], [lto=0])
CC=$compiler
if test $lto = 0; then
	AC_MSG_NOTICE([checking for -flto... no])
	AC_SUBST(LTO, "")
else
	AC_MSG_NOTICE([checking for -flto... yes])
	AC_SUBST(LTO, "-flto")
fi

# Checking PGO optimiztion
AC_ARG_WITH([pgo],
	AS_HELP_STRING([--with-pgo], [Build using PGO]))
if test "x$with_pgo" = "xyes"; then
	CC="$CC -fprofile-generate"
	AC_LINK_IFELSE([AC_LANG_PROGRAM([[]], [[]])], [pgo=1], [pgo=0])
	CC=$compiler
	if test "x$pgo" = "x0"; then
		AC_MSG_NOTICE("checking for PGO... no")
		AC_SUBST(PGO, "0")
	else
		AC_MSG_NOTICE("checking for PGO... yes")
		AC_SUBST(PGO, "1")
	fi
fi

# Checks for libraries
AC_SEARCH_LIBS([fmaxl], [m])
AC_SEARCH_LIBS([textdomain], [intl])
PKG_CHECK_MODULES([XML], [libxml-2.0])
PKG_CHECK_MODULES([GLIB], [glib-2.0])
PKG_CHECK_MODULES([GSL], [gsl])

# Checks for header files
AC_CHECK_HEADERS([stdio.h stdlib.h locale.h math.h],,
	AC_MSG_ERROR([Missing standard headers]))

# Checks for library functions

# Final output
#AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
