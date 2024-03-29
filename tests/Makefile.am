##############################################################################
#
# top_srcdir/tests
#
##############################################################################

bash_dif = ${top_builddir}/src/bash-dif$(EXEEXT)
openscad_dif = ${top_builddir}/src/openscad-dif$(EXEEXT)
openscad_seam = ${top_builddir}/src/openscad-seam$(EXEEXT)

EXTRA_DIST = \
	test1.bash \
	test1.scad \
	test2.scad \
	test3.scad

##############################################################################
# targets
##############################################################################

check-local:	test2.count \
							test1.bash-dif \
							test1.scad-dif \
							build/test1_doc.makefile.timestamp

# test2.count
test2.count: $(openscad_seam) $(srcdir)/test1.scad
	$(openscad_seam) \
		--input $(srcdir)/test2.scad \
		--mode count \
		--verbose > test2.count

# test1.bash-dif
test1.bash-dif: $(bash_dif) $(srcdir)/test1.bash
	$(bash_dif) $(srcdir)/test1.bash > test1.bash-dif

# test1.scad-dif
test1.scad-dif: $(openscad_dif) $(srcdir)/test1.scad
	$(openscad_dif) $(srcdir)/test1.scad > test1.scad-dif

# build/test1_doc.makefile.timestamp
build:
	mkdir -v build

build/test1_doc.makefile.timestamp: build/test1_doc.makefile
	OPENSCADPATH=${srcdir} \
	make -f build/test1_doc.makefile all
	touch build/test1_doc.makefile.timestamp

build/test1_doc.makefile: build/test1_doc.bash
	@echo "running makefile script"
	./build/test1_doc.bash

build/test1_doc.bash: $(openscad_seam) $(srcdir)/test1.scad | build
	$(openscad_seam) \
		--input $(srcdir)/test1.scad \
		--prefix build \
		--prefix-ipp 0 \
		--prefix-scripts yes \
		--comments yes \
		--show no \
		--run no \
		--make no \
		--lib-path $(top_srcdir)/share \
		--define __TEST_INCLUDE_PATH__=\"$(top_srcdir)/share/include/mfi\" \
		--verbose

# test3_doc.makefile (currently not built during checks)
test3_doc.makefile: $(openscad_seam) $(srcdir)/test3.scad | build
	$(openscad_seam) \
		--input $(srcdir)/test3.scad \
		--prefix build \
		--prefix-ipp 0 \
		--prefix-scripts yes \
		--run yes \
		--make yes \
		--lib-path $(top_srcdir)/share \
		--define __TEST_INCLUDE_PATH__=\"$(top_srcdir)/share/include/mfi\" \
		--verbose

CLEANFILES = \
	test1.bash-dif \
	test1.scad-dif \
	build/test1_doc.bash \
	build/test1_doc.scad \
	build/test1_doc.makefile.timestamp \
	test2.count


clean-local:
	-make -f build/test1_doc.makefile clean
	-rm -fv build/test1_doc.makefile
	-rmdir build

##############################################################################
# eof
##############################################################################
