##############################################################################
#
# top_srcdir/share/include
#
##############################################################################

SUBDIRS = \
	mfi \
	mfs_library \
	pmf

root_path = $(DESTDIR)$(pkgdatadir)
version_path = $(root_path)/$(PACKAGE_VERSION)
install_path = $(version_path)/include

root_path_doc = $(version_path)/doc
install_path_doc = $(root_path_doc)/share/include

##############################################################################
# targets
##############################################################################

uninstall-local:
	-test -d '$(install_path_doc)'&& rmdir -v '$(install_path_doc)'
	-test -d '$(install_path)' && rmdir -v '$(install_path)'

##############################################################################
# eof
##############################################################################
