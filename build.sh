#!/bin/sh

KERNEL="$(uname -s)"
SYSNAME="${KERNEL%%-*}"

builddir=build/${SYSNAME}
prefix="$HOME/projects/cnc"

[ ! -x configure ] \
  && autoreconf --force --install -I m4

mkdir -pv ${builddir} \
  && cd ${builddir} \
  && ../../configure --prefix="${prefix}" \
                     --bindir="${prefix}/bin/${SYSNAME}" \
  && make all

echo "build done"
