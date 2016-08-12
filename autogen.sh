#!/bin/sh

basepath="${0%/*}"
basename="${0##*/}"
rootname="${basename%.*}"

autoreconf_opts="--force --install -I m4"

echo "${rootname} begin"

autoreconf ${autoreconf_opts} || exit 1

echo "${rootname} end"

#
# eof
#
