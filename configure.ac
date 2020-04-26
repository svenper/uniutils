AC_PREREQ(2.59)
AC_INIT(uniutils, 2.27,billposer@alum.mit.edu)
AC_CONFIG_SRCDIR([unidesc.c])
AM_CONFIG_HEADER([config.h])

AC_DEFUN([AC_HAVE_NUMBER_SEPARATOR],
[AC_CACHE_CHECK(for number separator, ac_numsep,
     AC_TRY_RUN([printf("%'6d",123456)],
   ac_numsep=yes,
   ac_numsep=no)
   if test $ac_numsep = yes; then
     AC_DEFINE(HAVE_NUMBER_GROUP_SEPARATOR, 1, [printf understands number group separator])
   fi
])

# Checks for programs.
AC_PROG_CC

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h libintl.h locale.h stdlib.h string.h unistd.h stdio.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_OFF_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MEMCMP
AC_CHECK_FUNCS([malloc setlocale strtol strtoul])

AC_CONFIG_FILES([Makefile])
AM_INIT_AUTOMAKE
AC_OUTPUT
