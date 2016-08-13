##############################################################################
#
# top_srcdir/share/scripts
#
##############################################################################

files_am = \
	doxygen_mainpage.bash \
	segregate.bash

scripts_help = \
	segregate.help

root_path = $(DESTDIR)$(pkgdatadir)
version_path = $(root_path)/$(PACKAGE_VERSION)

install_path = $(version_path)/scripts

SUFFIXES = .bash .help

##############################################################################
# targets
##############################################################################

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

install-data-local: | '$(install_path)'
	( cd ${builddir} && \
		for f in $(files_am); do \
			$(INSTALL_SCRIPT) $$f '$(install_path)' \
		; done \
	)

uninstall-local:
	for f in $(files_am); do \
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
