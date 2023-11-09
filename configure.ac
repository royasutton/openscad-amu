##############################################################################
#
# Process this file with autoconf to produce a configure script.
#
##############################################################################
AC_PREREQ(2.50)

# obtain source versions from git
AC_REVISION([m4_esyscmd_s([git describe --tags --dirty --always])])

AC_INIT(
  [openscad-amu],
  [m4_esyscmd_s([git describe --tags --dirty --always])],
  [royasutton@hotmail.com],
  [openscad-amu],
  [https://royasutton.github.io/openscad-amu])

AC_CONFIG_AUX_DIR([scripts])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([
  --warnings=all
  --warnings=error
  foreign
  dist-xz
  dist-zip
])

##############################################################################
# check: required build tools
##############################################################################
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL

# flex
AC_MSG_CHECKING([fast lexical analyser generator (flex)])
AC_PROG_LEX
if test "x$LEX" != xflex; then
  LEX="$SHELL $missing_dir/missing flex"
  AC_SUBST([LEX_OUTPUT_ROOT], [lex.yy])
  AC_SUBST([LEXLIB], [''])
fi

##############################################################################
# check: required programs
##############################################################################
# gnu make command name
AC_MSG_CHECKING([GNU Make])
AX_CHECK_GNU_MAKE()
if test "x$_cv_gnu_make_command" = "x" ; then
  MAKE_COMMAND="make"
  AC_MSG_WARN([gnumake not found, using ${MAKE_COMMAND}... no])
else
  MAKE_COMMAND="$_cv_gnu_make_command"
  AC_MSG_CHECKING([GNUMake found, as ${MAKE_COMMAND}])
  AC_MSG_RESULT([version ${ax_check_gnu_make_version}... yes])
fi

# gnu make path
AC_MSG_CHECKING([GNU Make path])
AC_PATH_PROG(MAKE_PATH, ${MAKE_COMMAND}, false)
if test "$MAKE_PATH" = false ; then
  MAKE_PATH="${MAKE_COMMAND}"
  AC_MSG_WARN([make not found, using ${MAKE_PATH}... no])
  AM_CONDITIONAL(MAKE_PATH_FOUND, false)
else
  AM_CONDITIONAL(MAKE_PATH_FOUND, true)
fi

# bash path
AC_MSG_CHECKING([GNU Bourne-Again SHell])
AC_PATH_PROG(BASH_PATH, bash, false)
if test "$BASH_PATH" = false ; then
  BASH_PATH="/usr/bin/env bash"
  AC_MSG_WARN([bash not found, using ${BASH_PATH}... no])
  AM_CONDITIONAL(BASH_PATH_FOUND, false)
else
  AM_CONDITIONAL(BASH_PATH_FOUND, true)
fi

# openscad path
AC_MSG_CHECKING([OpenSCAD])
AC_PATH_PROG(OPENSCAD_PATH, openscad, false)
if test "$OPENSCAD_PATH" = false ; then
  OPENSCAD_PATH="openscad"
  AC_MSG_WARN([openscad not found, using ${OPENSCAD_PATH}... no])
  AM_CONDITIONAL(OPENSCAD_PATH_FOUND, false)
else
  AM_CONDITIONAL(OPENSCAD_PATH_FOUND, true)
fi

# doxygen path
AC_MSG_CHECKING([doxygen])
AC_PATH_PROG(DOXYGEN_PATH, doxygen, false)
if test "$DOXYGEN_PATH" = false ; then
  DOXYGEN_PATH="doxygen"
  AC_MSG_WARN([doxygen not found, using ${DOXYGEN_PATH}... no])
  AM_CONDITIONAL(DOXYGEN_PATH_FOUND, false)
else
  AM_CONDITIONAL(DOXYGEN_PATH_FOUND, true)
fi

# pdflatex path
AC_MSG_CHECKING([pdflatex])
AC_PATH_PROG(PDFLATEX_PATH, pdflatex, false)
if test "$PDFLATEX_PATH" = false ; then
  PDFLATEX_PATH="pdflatex"
  AC_MSG_WARN([pdflatex not found, using ${PDFLATEX_PATH}... no])
  AM_CONDITIONAL(PDFLATEX_PATH_FOUND, false)
else
  AM_CONDITIONAL(PDFLATEX_PATH_FOUND, true)
fi

##############################################################################
# check: required libraries
##############################################################################
AC_MSG_CHECKING([for required boost libraries])
BOOST_REQUIRE([1.35.0])
BOOST_SYSTEM
BOOST_FILESYSTEM
BOOST_PROGRAM_OPTIONS
BOOST_REGEX

##############################################################################
# check: functions
##############################################################################
AC_CHECK_FUNCS([popen])

##############################################################################
# create: other options
##############################################################################
AC_CONFIG_HEADERS([src/config.h])

AM_EXTRA_RECURSIVE_TARGETS([
docs
install-docs
man
install-man
])

##############################################################################
# create: configured files
##############################################################################
AC_CONFIG_FILES([
  Makefile

  share/Makefile

  share/examples/Makefile
  share/examples/convert.bash
  share/examples/hello_world.bash
  share/examples/log_segregate.bash
  share/examples/vehicle.doxyfile

  share/functions/Makefile

  share/include/Makefile
  share/include/mfi/Makefile
  share/include/pmf/Makefile
  share/include/pmf/df1/Makefile

  share/lib/Makefile
  share/lib/Doxyfile

  share/scripts/Makefile
  share/scripts/Doxyfile
  share/scripts/mainpage.bash
  share/scripts/segregate.bash

  share/templates/Makefile
  share/templates/df1/Makefile

  src/Makefile
  src/Doxyfile

  tests/Makefile
])

AC_OUTPUT

##############################################################################
# show: configuration summary
##############################################################################
AC_MSG_NOTICE([
**************************************************************************
configuration:
  source version            $PACKAGE_VERSION

  c++ compiler:             $CXX
  lexical analyser:         $LEX

  install \${prefix}:        $prefix
  install \${exec_prefix}:   $exec_prefix
  system data root dir:     $datarootdir
  binaries dir:             $bindir

  gnu make path:            $MAKE_PATH
  bash path:                $BASH_PATH
  openscad path:            $OPENSCAD_PATH
  doxygen path:             $DOXYGEN_PATH
  pdflatex path:            $PDFLATEX_PATH

  make targets:

    all, install, uninstall:
    - build/(un)install all programs and scripts.

    docs, install-docs:
    - build/install documentation in all format.

    html, pdf, man, install-html, install-pdf, install-man:
    - build/install documentation in html, pdf, or man format.

    check:
    - run the build tests.

  run 'make' or 'make all' to build.

**************************************************************************
])

##############################################################################
# eof
##############################################################################
