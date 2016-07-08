#!/bin/sh

KERNEL="$(uname -s)"
SYSNAME="${KERNEL%%-*}"

builddir=build
srcdirs="share/examples share/lib share/mk share/scripts share src tests"

# build directory
if [ -d  ${builddir}/${SYSNAME} ] ; then
  ( cd ${builddir}/${SYSNAME} && \
    make distclean && \
    for d in $srcdirs ; do rmdir -v $d ; done \
  ) && \
  rmdir -v ${builddir}/${SYSNAME} && \
  rmdir -v ${builddir}
fi

# top_builddir dirs
rm -vrf \
  autom4te.cache \
  config.h.in \
  config.h.in~ \
  scripts

# top_builddir files
rm -vf \
  aclocal.m4 \
  configure

# top_builddir/m4
(cd m4; \
rm -vf \
  libtool.m4 \
  lt~obsolete.m4 \
  ltoptions.m4 \
  ltsugar.m4 \
  ltversion.m4 \
)

for d in . $srcdirs ; do
  ( cd $d; rm -vf Makefile.in )
done

echo "done"
