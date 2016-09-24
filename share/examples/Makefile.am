##############################################################################
#
# top_srcdir/share/examples
#
##############################################################################

files = \
	embedding_scheme.scad \
	embedding_scheme.dot \
	vehicle.scad \
	animate.scad

files_am = \
	convert.bash \
	hello_world.bash \
	log_segregate.bash \
	vehicle.doxyfile

SUFFIXES = .bash .makefile .stamp

EXTRA_DIST = \
	$(files)

openscad_dif = ${top_builddir}/src/openscad-dif$(EXEEXT)
openscad_seam = ${top_builddir}/src/openscad-seam$(EXEEXT)

root_path = $(DESTDIR)$(pkgdatadir)
version_path = $(root_path)/$(PACKAGE_VERSION)

install_path = $(version_path)/examples

##############################################################################
# targets
##############################################################################

all-local: $(files_am)

docs-local: examples.stamp

CLEANFILES = \
	examples.stamp

examples.stamp: \
		embedding_scheme.scripts \
		\
		convert.makefile \
		hello_world.makefile \
		log_segregate.makefile \
		\
		vehicle_document.stamp \
		vehicle_test.stamp \
		animate_move.stamp \
		animate_rotate.stamp \
		\
		doxygen.stamp
	touch $@

CLEANFILES += \
	embedding_scheme.scripts

embedding_scheme.scripts: $(srcdir)/embedding_scheme.scad $(openscad_seam)
	$(openscad_seam) --mode count  --joiner=- --input $< --scope root --verbose \
		> embedding_scheme.scripts

CLEANFILES += \
	convert.makefile \
	hello_world.makefile \
	log_segregate.makefile

.bash.makefile:
	$(BASH_PATH) --norc --noprofile $<

CLEANFILES += \
	vehicle_document.stamp \
	vehicle_test.stamp \
	animate_move.stamp \
	animate_rotate.stamp

.makefile.stamp:
	OPENSCADPATH=${srcdir} \
	make -f $< all
	touch $@

build: ; mkdir -v build

CLEANFILES += \
	vehicle_document.scad \
	vehicle_document.bash \
	vehicle_test.scad \
	vehicle_test.bash \
	vehicle.conf

vehicle_document.makefile \
vehicle_test.makefile: $(srcdir)/vehicle.scad $(openscad_seam) | build
	$(openscad_seam) \
		--input $< \
		--prefix build --prefix-ipp 0 --prefix-scripts no \
		--comments yes --show no --run yes --make no \
		--lib-path $(top_srcdir)/share \
		--write-config yes \
		--verbose

CLEANFILES += \
	animate_move.scad \
	animate_move.bash \
	animate_rotate.scad \
	animate_rotate.bash \
	animate.conf

animate_move.makefile \
animate_rotate.makefile: $(srcdir)/animate.scad $(openscad_seam) | build
	$(openscad_seam) \
		--input $< \
		--prefix build --prefix-ipp 0 --prefix-scripts no \
		--comments yes --show no --run yes --make no \
		--lib-path $(top_srcdir)/share \
		--write-config yes \
		--verbose

CLEANFILES += \
	doxygen.stamp \
	doxygen.tagfile \
	doxygen.warn

doxygen.stamp: vehicle.doxyfile Makefile $(srcdir)/vehicle.scad
	$(DOXYGEN_PATH) vehicle.doxyfile \
	&& touch doxygen.stamp \
	&& ( cd build/latex && make )

'$(install_path)': ; $(MKDIR_P) -v '$(install_path)'

install-data-local: $(files_am) | '$(install_path)'
	( cd ${srcdir} && \
		for f in $(files); do \
			$(INSTALL_DATA) $$f '$(install_path)' \
		; done \
	) && \
	( cd ${builddir} && \
		for f in $(files_am); do \
			$(INSTALL_SCRIPT) $$f '$(install_path)' \
		; done \
	)

uninstall-local:
	for f in $(files) $(files_am); do \
		rm -fv $(install_path)/$$f \
	; done
	-rmdir -v '$(install_path)'

clean-local:
	-make -f vehicle_document.makefile clean
	-rm -fv vehicle_document.makefile
	-make -f vehicle_test.makefile clean
	-rm -fv vehicle_test.makefile
	-make -f animate_move.makefile clean
	-rm -fv animate_move.makefile
	-make -f animate_rotate.makefile clean
	-rm -fv animate_rotate.makefile
	-rm -rfv build/html build/latex build/man
	-rmdir build

##############################################################################
# eof
##############################################################################
