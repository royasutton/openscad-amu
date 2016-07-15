#!/bin/sh

basepath="${0%/*}"
basename="${0##*/}"
rootname="${basename%.*}"

echo "${rootname} begin"

# auto-configure if needed
[ ! -x configure ] \
  && autoreconf --force --install -I m4 || exit 1

echo "${rootname} end"

#
# eof
#
