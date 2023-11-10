##############################################################################
#
# top_srcdir/share
#
##############################################################################

# include/mfs_library depends on: examples and scripts,
# build examples and scripts before include/mfs_library
SUBDIRS = \
	dif_external \
	examples \
	scripts \
	include \
	templates

root_path = $(DESTDIR)$(pkgdatadir)
version_path = $(root_path)/$(PACKAGE_VERSION)
install_path = $(version_path)

root_path_doc = $(version_path)/doc
install_path_doc = $(root_path_doc)/share

##############################################################################
# targets
##############################################################################

uninstall-local:
	-test -d '$(install_path_doc)'&& rmdir -v '$(install_path_doc)'
	-test -d '$(root_path_doc)' && rmdir -v '$(root_path_doc)'
	-test -d '$(version_path)' && rmdir -v '$(version_path)'
	-test -d '$(root_path)' && rmdir -v '$(root_path)'

##############################################################################
# eof
##############################################################################
