#!/bin/sh

basepath="${0%/*}"
basename="${0##*/}"
rootname="${basename%.*}"

autoreconf_opts="--force --install -I m4"

echo "${rootname} begin"

# auto-configure if needed
if [ ! -x configure ] ; then
  autoreconf ${autoreconf_opts} || exit 1
fi

echo "${rootname} end"

#
# eof
#
