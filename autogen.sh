#!/bin/sh

basepath="${0%/*}"
basename="${0##*/}"
rootname="${basename%.*}"

echo "${rootname} begin"

# auto-configure if needed
if [ ! -x configure ] ; then
  autoreconf --force --install -I m4 || exit 1
fi

echo "${rootname} end"

#
# eof
#
