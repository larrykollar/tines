#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([tines], [1.12.0], [bugs@bunny.com])
AM_INIT_AUTOMAKE
LT_INIT
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lncurses':
AC_CHECK_LIB([ncurses], [main])


# Checks for header files.
AC_CHECK_HEADERS([fcntl.h locale.h stdlib.h string.h sys/param.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_CHECK_FUNCS([memmove memset setlocale strchr strdup strncasecmp strstr strtol])

AC_CONFIG_FILES([Makefile
                 include/Makefile
                 libcli/Makefile
                 src/Makefile
                 util/Makefile])
AC_OUTPUT
