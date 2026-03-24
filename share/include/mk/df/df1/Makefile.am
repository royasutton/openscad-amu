##############################################################################
#
# top_srcdir/share/include/mk/df/df1
#
##############################################################################

files = \
	base_constants.mk \
	base_defaults.mk \
	base_functions.mk \
	base_rules.mk \
	base_system.mk \
	base_variables.mk \
	\
	doxygen_defaults.mk \
	doxygen_functions.mk \
	doxygen_rules.mk \
	doxygen_system.mk \
	doxygen_variables.mk \
	\
	scopes_defaults.mk \
	scopes_functions.mk \
	scopes_rules.mk \
	scopes_system.mk \
	scopes_variables.mk \
	\
	groups_defaults.mk \
	groups_functions.mk \
	groups_rules.mk \
	groups_system.mk \
	groups_variables.mk

files_am =

EXTRA_DIST = \
	$(files)

root_path = $(DESTDIR)$(pkgdatadir)
version_path = $(root_path)/$(PACKAGE_VERSION)
install_path = $(version_path)/include/mk/df/df1

##############################################################################
# targets
##############################################################################

all-local: $(files_am)

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

uninstall-local:
	for f in $(files) $(files_am); \
	do \
		rm -fv "$(install_path)/$$f" ; \
	done
	-test -d '$(install_path)' && rmdir -v '$(install_path)'

##############################################################################
# eof
##############################################################################
