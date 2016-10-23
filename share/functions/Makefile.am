##############################################################################
#
# top_srcdir/share/functions
#
##############################################################################

files = \
	amu_date \
	amu_list

files_am =

EXTRA_DIST = \
	$(files)

root_path = $(DESTDIR)$(pkgdatadir)
version_path = $(root_path)/$(PACKAGE_VERSION)
install_path = $(version_path)/functions

##############################################################################
# targets
##############################################################################

all-local: $(files_am)

installdirs-local: ; $(MKDIR_P) -v '$(install_path)'

install-data-local: $(files_am) | installdirs-local
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

##############################################################################
# eof
##############################################################################
