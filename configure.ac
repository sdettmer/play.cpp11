AC_INIT([cpp11], [0.1], [steffen.dettmer@gmail.com], [cpp11], [])
AC_PREREQ([2.59])

AM_PROG_AR
AM_PATH_CPPUNIT()
AM_INIT_AUTOMAKE([1.10 -Wall no-define foreign])
AM_SILENT_RULES([yes])

AC_PROG_CXX
AC_PROG_RANLIB

CPPFLAGS='-I$(top_srcdir)/src'

dnl A function to check if compiler accepts option "-std=c++11" (g++)
AC_DEFUN([AC_STD_CHECK_STD], [
AC_MSG_CHECKING([Compiler flag to use 2011 ISO C++ standard plus amendments])
dnl The 2011 ISO C++ standard plus amendments
AC_SUBST(CXX_ISOCXX11)
CXX_ISOCXX11=-std=c++11
AC_LANG_PUSH(C++)
ac_std_cxx_flags="$CXXFLAGS"
CXXFLAGS="$CXX_ISOCXX11 $CXXFLAGS"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[]])],
  [ac_std_check_std_result='yes'],
  [ac_std_check_std_result='no'; CXX_ISOCXX11=""])
AC_MSG_RESULT([$ac_std_check_std_result])
CXXFLAGS="$ac_std_cxx_flags"
CXXFLAGS="$CXX_ISOCXX11 $CXXFLAGS"
AC_LANG_POP()
])
AC_STD_CHECK_STD

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
