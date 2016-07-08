#!/bin/sh

KERNEL="$(uname -s)"
SYSNAME="${KERNEL%%-*}"

builddir=build/${SYSNAME}

[ ! -x ${builddir} ] \
  && sh ./build.sh

cd ${builddir} \
  && make all docs

echo "update done"
