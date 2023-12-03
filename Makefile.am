##############################################################################
#
# top_srcdir
#
##############################################################################

ACLOCAL_AMFLAGS = -I m4

files_src =

files_data = \
	autogen.sh \
	README.md

files_am =

# build src first
SUBDIRS = \
	src \
	share \
	tests

# doxygen documentation
DOXYGEN_DOXFILES = \
	docs_start.dox

EXTRA_DIST = \
	$(DOXYGEN_DOXFILES) \
	$(files_data)

doxygen_stamp_dependacies = \
	Makefile \
	Doxyfile \
	$(DOXYGEN_DOXFILES)

root_path = $(DESTDIR)$(pkgdatadir)
version_path = $(root_path)/$(PACKAGE_VERSION)

install_path_doc = $(version_path)/doc
install_path_html = $(install_path_doc)/doxygen/html
install_path_pdf = $(install_path_doc)/doxygen/latex
install_path_man = $(install_path_doc)/doxygen/man

SUFFIXES =

##############################################################################
# targets
##############################################################################

docs-local: html-local

html-local: doxygen.stamp

pdf-local: doxygen.stamp doxygen/latex/refman.pdf

man-local: doxygen.stamp

doxygen/latex/refman.pdf:
	( cd doxygen/latex && make refman.pdf )

doxygen.stamp: $(doxygen_stamp_dependacies)
	( cat Doxyfile ; \
		echo "INPUT=$(srcdir)" ; \
		echo "OUTPUT_DIRECTORY=doxygen" ; \
	) | $(DOXYGEN_PATH) - \
	&& touch doxygen.stamp

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
	-test -d '$(install_path_doc)' && rm -rfv '$(install_path_doc)/doxygen'
	-test -d '$(install_path_doc)' && rmdir -v '$(install_path_doc)'
	-test -d '$(version_path)' && rmdir -v '$(version_path)'
	-test -d '$(root_path)' && rmdir -v '$(root_path)'
	-test -d '$(pkgdatadir)' && rmdir -v '$(pkgdatadir)'
	-test -d '$(DESTDIR)$(datadir)' && rmdir -v '$(DESTDIR)$(datadir)'

CLEANFILES = \
	doxygen.stamp \
	doxygen.tagfile \
	doxygen.warn

clean-local:
	-test -d doxygen && rm -rfv doxygen

##############################################################################
# eof
##############################################################################

