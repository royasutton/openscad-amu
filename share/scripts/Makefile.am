##############################################################################
#
# top_srcdir/share/scripts
#
##############################################################################

files_src = \
	setup-amu.bash

files_data =

files_am = \
	docs_scripts.bash \
	\
	segregate.bash

scripts_help = \
	setup-amu.help \
	segregate.help

EXTRA_DIST = \
	$(files_src) \
	$(files_data)

root_path = $(DESTDIR)$(pkgdatadir)
version_path = $(root_path)/$(PACKAGE_VERSION)
install_path = $(version_path)/scripts

install_path_doc = $(version_path)/doc/share/scripts
install_path_html = $(install_path_doc)/doxygen/html
install_path_pdf = $(install_path_doc)/doxygen/latex
install_path_man = $(install_path_doc)/doxygen/man

SUFFIXES = .bash .help

##############################################################################
# targets
##############################################################################

all-local: $(files_am)

docs-local: html-local

html-local: doxygen.stamp

pdf-local: doxygen.stamp doxygen/latex/refman.pdf

man-local: doxygen.stamp

doxygen/latex/refman.pdf:
	( cd doxygen/latex && make refman.pdf )

doxygen.stamp: Doxyfile Makefile $(files_am) $(scripts_help)
	( cat Doxyfile ; \
		echo "INPUT=$(builddir)" ; \
		echo "OUTPUT_DIRECTORY=doxygen" ; \
	) | $(DOXYGEN_PATH) - \
	&& touch doxygen.stamp

.bash.help:
	$(BASH_PATH) --norc --noprofile $< --help > $@

'$(install_path)': ; $(MKDIR_P) -v '$(install_path)'
install-data-local: $(files_am) | '$(install_path)'
	( cd ${srcdir} && \
		for f in $(files_src); \
		do \
			$(INSTALL_SCRIPT) $$f '$(install_path)' ; \
		done \
	) && \
	( cd ${srcdir} && \
		for f in $(files_data); \
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
	for f in $(files_src) $(files_data) $(files_am); \
	do \
		rm -fv "$(install_path)/$$f" ; \
	done
	-test -d '$(install_path_doc)' && rm -rfv '$(install_path_doc)'
	-test -d '$(install_path)' && rmdir -v '$(install_path)'

CLEANFILES = \
	doxygen.stamp \
	doxygen.tagfile \
	doxygen.warn \
	$(scripts_help)

clean-local:
	-test -d doxygen && rm -rfv doxygen

##############################################################################
# eof
##############################################################################
