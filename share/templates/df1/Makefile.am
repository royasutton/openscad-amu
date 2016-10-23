##############################################################################
#
# top_srcdir/share/templates/df1
#
##############################################################################

files = \
	Doxyfile \
	design.scad \
	library.scad

files_am =

# am_local: source files
files_am_local_src = \
	Project_Makefile.inl

# am_local: output files
files_am_local = \
	Project_Makefile

CLEANFILES = \
	$(files_am_local)

EXTRA_DIST = \
	$(files) $(files_am_local_src)

root_path = $(DESTDIR)$(pkgdatadir)
version_path = $(root_path)/$(PACKAGE_VERSION)
install_path = $(version_path)/templates/df1

##############################################################################
# targets
##############################################################################

# am_local: source edit script
do_am_local_subst = \
	$(SED) \
		-e 's,[@]__LIB_PATH__[@],$(version_path),g' \
		-e 's,[@]__PACKAGE_VERSION__[@],$(PACKAGE_VERSION),g' \
		-e 's,[@]abs_top_builddir[@],$(abs_top_builddir),g' \
		-e 's,[@]abs_top_srcdir[@],$(abs_top_srcdir),g' \
		-e 's,[@]bindir[@],$(bindir),g'

Project_Makefile: $(srcdir)/Project_Makefile.inl Makefile
	$(do_am_local_subst) $< > $@

all-local: $(files_am) $(files_am_local)

'$(install_path)': ; $(MKDIR_P) -v '$(install_path)'
install-data-local: $(files_am) $(files_am_local) | '$(install_path)'
	( cd ${srcdir} && \
		for f in $(files); do \
			$(INSTALL_DATA) $$f '$(install_path)' \
		; done \
	) && \
	( cd ${builddir} && \
		for f in $(files_am) $(files_am_local); do \
			$(INSTALL_DATA) $$f '$(install_path)' \
		; done \
	)

uninstall-local:
	for f in $(files) $(files_am) $(files_am_local); do \
		rm -fv $(install_path)/$$f \
	; done
	-rmdir -v '$(install_path)'

##############################################################################
# eof
##############################################################################
