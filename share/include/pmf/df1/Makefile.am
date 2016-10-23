##############################################################################
#
# top_srcdir/share/include/pmf/df1
#
##############################################################################

files = \
	base_constants \
	base_defaults \
	base_functions \
	base_rules \
	base_system \
	base_variables \
	\
	doxygen_defaults \
	doxygen_functions \
	doxygen_rules \
	doxygen_system \
	doxygen_variables \
	\
	scopes_defaults \
	scopes_functions \
	scopes_rules \
	scopes_system \
	scopes_variables

files_am =

EXTRA_DIST = \
	$(files)

root_path = $(DESTDIR)$(pkgdatadir)
version_path = $(root_path)/$(PACKAGE_VERSION)
install_path = $(version_path)/include/pmf/df1

##############################################################################
# targets
##############################################################################

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
			$(INSTALL_SCRIPT) $$f '$(install_path)' \
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
