##############################################################################
#
# top_srcdir/share/include/mfi
#
##############################################################################

files = \
	parallel_jobs.mk

files_am =

EXTRA_DIST = \
	$(files)

root_path = $(DESTDIR)$(pkgdatadir)
version_path = $(root_path)/$(PACKAGE_VERSION)
install_path = $(version_path)/include/mfi

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
