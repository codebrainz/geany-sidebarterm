#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([sidebarterm], [0.0.1], [mbrush@leftclick.ca])
AC_CONFIG_SRCDIR([sidebarterm.c])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([sidebarterm], [0.0.1])
LT_INIT([dlopen])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for packages.
PKG_CHECK_MODULES([GEANY], [geany])
PKG_CHECK_MODULES([VTE], [vte])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_OUTPUT(Makefile)
