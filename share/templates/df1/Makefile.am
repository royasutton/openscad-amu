##############################################################################
#
# top_srcdir/share/templates/df1
#
##############################################################################

files = \
	Doxyfile \
	design.scad \
	library.scad

files_am = \
	Project_Makefile

EXTRA_DIST = \
	$(files)

root_path = $(DESTDIR)$(pkgdatadir)
version_path = $(root_path)/$(PACKAGE_VERSION)

install_path = $(version_path)/templates/df1

##############################################################################
# targets
##############################################################################

Project_Makefile: $(srcdir)/Project_Makefile.in Makefile
	$(SED) \
		-e 's,__LIBPATH__,$(version_path),g' \
		-e 's,__PACKAGE_VERSION__,$(PACKAGE_VERSION),g' \
		$< > $@

all-local: $(files_am)

'$(install_path)': ; $(MKDIR_P) -v '$(install_path)'

install-data-local: $(files_am) | '$(install_path)'
	( cd ${srcdir} && \
		for f in $(files); do \
			$(INSTALL_DATA) $$f '$(install_path)' \
		; done \
	) && \
	( cd ${builddir} && \
		for f in $(files_am); do \
			$(INSTALL_DATA) $$f '$(install_path)' \
		; done \
	)

uninstall-local:
	for f in $(files) $(files_am); do \
		rm -fv $(install_path)/$$f \
	; done
	-rmdir -v '$(install_path)'

##############################################################################
# eof
##############################################################################
