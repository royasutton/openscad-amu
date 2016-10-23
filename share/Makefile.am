##############################################################################
#
# top_srcdir/share
#
##############################################################################

# lib depends on: examples and scripts (build afterwards)
SUBDIRS = \
	examples \
	scripts \
	lib \
	\
	functions \
	include \
	templates

root_path = $(DESTDIR)$(pkgdatadir)
version_path = $(root_path)/$(PACKAGE_VERSION)
install_path = $(version_path)

##############################################################################
# targets
##############################################################################

uninstall-local:
	-rmdir -v '$(version_path)'
	-rmdir -v '$(root_path)'

##############################################################################
# eof
##############################################################################
