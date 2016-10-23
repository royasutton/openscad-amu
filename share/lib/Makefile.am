##############################################################################
#
# top_srcdir/share/lib
#
##############################################################################

files = \
	doxygen_mainpage.bash \
	bootloader.bash \
	exception.bash \
	make_makefile.bash \
	openscad_opts.bash \
	table.bash \
	print.bash \
	print_textbox.bash \
	utility.bash

files_am =

EXTRA_DIST = \
	$(files)

root_path = $(DESTDIR)$(pkgdatadir)
version_path = $(root_path)/$(PACKAGE_VERSION)
install_path = $(version_path)/lib

doxygen_stamp_other_dependacies = \
	$(top_builddir)/share/examples/examples.stamp \
	$(top_builddir)/share/scripts/doxygen.tagfile \
	$(top_builddir)/src/doxygen.tagfile

##############################################################################
# targets
##############################################################################

html-local: doxygen.stamp

man-local: doxygen.stamp

pdf-local: doxygen.stamp doxygen/latex/refman.pdf

doxygen/latex/refman.pdf:
	( cd doxygen/latex && make refman.pdf )

# run doxygen and update timestamp
doxygen.stamp: Doxyfile Makefile $(files) $(doxygen_stamp_other_dependacies)
	( cat Doxyfile ; \
		echo "INPUT=$(srcdir)" ; \
		echo "PROJECT_BRIEF=\"OpenSCAD AutoMake Utilities: Makefile Script Library\"" ; \
	) | $(DOXYGEN_PATH) - \
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

uninstall-local:
	for f in $(files) $(files_am); do \
		rm -fv $(install_path)/$$f \
	; done
	-rmdir -v '$(install_path)'

CLEANFILES = \
	doxygen.stamp \
	doxygen.tagfile \
	doxygen.warn

clean-local:
	-test -d doxygen \
		&& ( cd doxygen && rm -rfv html latex man ) \
		&& rmdir -v doxygen

##############################################################################
# eof
##############################################################################
