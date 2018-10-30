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

install_path_doc = $(version_path)/doc/share/lib
install_path_html = $(install_path_doc)/doxygen/html
install_path_pdf = $(install_path_doc)/doxygen/latex
install_path_man = $(install_path_doc)/doxygen/man

doxygen_stamp_other_dependacies = \
	$(top_builddir)/share/examples/examples.stamp \
	$(top_builddir)/share/scripts/doxygen.tagfile \
	$(top_builddir)/src/doxygen.tagfile

##############################################################################
# targets
##############################################################################

docs-local: html-local

html-local: doxygen.stamp

pdf-local: doxygen.stamp doxygen/latex/refman.pdf

man-local: doxygen.stamp

doxygen/latex/refman.pdf:
	( cd doxygen/latex && make refman.pdf )

doxygen.stamp: Doxyfile Makefile $(files) $(doxygen_stamp_other_dependacies)
	( cat Doxyfile ; \
		echo "INPUT=$(srcdir)" ; \
		echo "PROJECT_BRIEF=\"OpenSCAD AutoMake Utilities: Makefile Script Library\"" ; \
	) | $(DOXYGEN_PATH) - \
	&& touch doxygen.stamp

'$(install_path)': ; $(MKDIR_P) -v '$(install_path)'
install-data-local: $(files_am) | '$(install_path)'
	( cd ${srcdir} && \
		for f in $(files); \
		do \
			$(INSTALL_DATA) $$f '$(install_path)' ; \
		done \
	) && \
	( cd ${builddir} && \
		for f in $(files_am); \
		do \
			$(INSTALL_SCRIPT) $$f '$(install_path)' ; \
		done \
	)

install-docs-local: install-html-local

'$(install_path_html)': ; $(MKDIR_P) -v '$(install_path_html)'
install-html-local: html-local | '$(install_path_html)'
	( cd doxygen && cp -av html/* '$(install_path_html)' )

'$(install_path_pdf)': ; $(MKDIR_P) -v '$(install_path_pdf)'
install-pdf-local: pdf-local | '$(install_path_pdf)'
	( cd doxygen && cp -av latex/refman.pdf '$(install_path_pdf)' )

'$(install_path_man)': ; $(MKDIR_P) -v '$(install_path_man)'
install-man-local: man-local | '$(install_path_man)'
	( cd doxygen && cp -av man/* '$(install_path_man)' )

uninstall-local:
	for f in $(files) $(files_am); \
	do \
		rm -fv "$(install_path)/$$f" ; \
	done
	-test -d '$(install_path)' && rmdir -v '$(install_path)'
	-test -d '$(install_path_doc)' && rm -rfv '$(install_path_doc)'

CLEANFILES = \
	doxygen.stamp \
	doxygen.tagfile \
	doxygen.warn

clean-local:
	-test -d doxygen && rm -rfv doxygen

##############################################################################
# eof
##############################################################################
