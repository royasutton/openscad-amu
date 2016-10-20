##############################################################################
#
# top_srcdir/share/scripts
#
##############################################################################

files = \
	bootstrap.bash

files_am = \
	doxygen_mainpage.bash \
	segregate.bash

scripts_help = \
	bootstrap.help \
	segregate.help

SUFFIXES = .bash .help

EXTRA_DIST = \
	$(files)

root_path = $(DESTDIR)$(pkgdatadir)
version_path = $(root_path)/$(PACKAGE_VERSION)

install_path = $(version_path)/scripts

##############################################################################
# targets
##############################################################################

all-local: $(files_am)

docs-local: doxygen.stamp

# run doxygen and update timestamp
doxygen.stamp: Doxyfile Makefile $(files_am) $(scripts_help)
	( cat Doxyfile ; \
		echo "INPUT=$(builddir)" ; \
		echo "PROJECT_BRIEF=\"OpenSCAD AutoMake Utilities: Support Scripts\"" ; \
	) | $(DOXYGEN_PATH) - \
	&& touch doxygen.stamp
#	&& ( cd doxygen/latex && make )

.bash.help:
	$(BASH_PATH) --norc --noprofile $< --help > $@

'$(install_path)': ; $(MKDIR_P) -v '$(install_path)'

install-data-local: $(files_am) | '$(install_path)'
	( cd ${srcdir} && \
		for f in $(files); do \
			$(INSTALL_SCRIPT) $$f '$(install_path)' \
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
	doxygen.warn \
	$(scripts_help)

clean-local:
	-test -d doxygen \
		&& ( cd doxygen && rm -rfv html latex man ) \
		&& rmdir -v doxygen

##############################################################################
# eof
##############################################################################
