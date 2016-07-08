##############################################################################
#
# top_builddir/share
#
##############################################################################

files_am = \
	doxygen_mainpage.bash \
	segregate.bash

root_path = $(DESTDIR)$(pkgdatadir)
version_path = $(root_path)/$(PACKAGE_VERSION)

install_path = $(version_path)/scripts

##############################################################################
# targets
##############################################################################

docs-local: doxygen.stamp

# run doxygen and update timestamp
doxygen.stamp: Doxyfile Makefile $(files_am)
	( cat Doxyfile ; \
		echo "INPUT=$(builddir)" ; \
		echo "PROJECT_BRIEF=\"OpenSCAD AutoMake Utilities: Support Scripts\"" ; \
	) | $(DOXYGEN_PATH) - \
	&& touch doxygen.stamp
#	&& ( cd doxygen/latex && make )

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
	-rmdir -v '$(version_path)'
	-rmdir -v '$(root_path)'

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
