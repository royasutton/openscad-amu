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

EXTRA_DIST = \
	$(files)

openscad_dif = ${top_builddir}/src/openscad-dif$(EXEEXT)
openscad_seam = ${top_builddir}/src/openscad-seam$(EXEEXT)

root_path = $(DESTDIR)$(pkgdatadir)
version_path = $(root_path)/$(PACKAGE_VERSION)
install_path = $(version_path)/examples

install_path_doc = $(version_path)/doc/share/examples
install_path_html = $(install_path_doc)/build/html
install_path_pdf = $(install_path_doc)/build/latex
install_path_man = $(install_path_doc)/build/man

SUFFIXES = .bash .makefile .stamp

##############################################################################
# targets
##############################################################################

all-local: $(files_am)

docs-local: html-local pdf-local man-local

html-local: examples.stamp

pdf-local: examples.stamp doxygen.stamp build/latex/refman.pdf

man-local: examples.stamp

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
		animate_move.makefile \
		animate_rotate.makefile \
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

build/latex/refman.pdf:
	( cd build/latex && make refman.pdf )

doxygen.stamp: vehicle.doxyfile Makefile $(srcdir)/vehicle.scad
	$(DOXYGEN_PATH) vehicle.doxyfile \
	&& touch doxygen.stamp

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

install-docs-local: install-html-local install-pdf-local install-man-local

'$(install_path_html)': ; $(MKDIR_P) -v '$(install_path_html)'
install-html-local: html-local | '$(install_path_html)'
	( cd build && cp -av html/* '$(install_path_html)' )

'$(install_path_pdf)': ; $(MKDIR_P) -v '$(install_path_pdf)'
install-pdf-local: pdf-local | '$(install_path_pdf)'
	( cd build && cp -av latex/refman.pdf '$(install_path_pdf)' )

'$(install_path_man)': ; $(MKDIR_P) -v '$(install_path_man)'
install-man-local: man-local | '$(install_path_man)'
	( cd build && cp -av man/* '$(install_path_man)' )

uninstall-local:
	for f in $(files) $(files_am); do \
		rm -fv "$(install_path)/$$f" \
	; done
	-test -d '$(install_path)' && rmdir -v '$(install_path)'
	-test -d '$(install_path_doc)' && rm -rfv '$(install_path_doc)'

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
	-test -d build && rmdir -v build

##############################################################################
# eof
##############################################################################
