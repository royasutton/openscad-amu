##############################################################################
#
# top_srcdir/src
#
##############################################################################

AUTOMAKE_OPTIONS = --warnings=no-portability

bin_PROGRAMS = \
	bash-dif \
	openscad-dif \
	openscad-seam

programs_help = \
	bash-dif.help \
	openscad-dif.help \
	openscad-seam.help

BUILD_DATE="$(shell date)"
BUILD_ARCH="$(shell uname -sr) ($(shell uname -m))"

AM_CXXFLAGS = \
	-W -Wall -Wextra -Wno-sign-compare \
	-ansi -U__STRICT_ANSI__

AM_CPPFLAGS = \
	$(BOOST_CPPFLAGS) \
	-D__BUILD_DATE__=\"$(BUILD_DATE)\" \
	-D__BUILD_ARCH__=\"$(BUILD_ARCH)\" \
	-D__BASH_PATH__=\"$(BASH_PATH)\" \
	-D__MAKE_PATH__=\"$(MAKE_PATH)\" \
	-D__LIB_PATH__=\"$(pkgdatadir)/$(PACKAGE_VERSION)\" \
	-D__OPENSCAD_PATH__=\"$(OPENSCAD_PATH)\"

AM_LDFLAGS = \
	$(BOOST_FILESYSTEM_LDFLAGS) $(BOOST_FILESYSTEM_LIBS) \
	$(BOOST_SYSTEM_LDFLAGS) $(BOOST_SYSTEM_LIBS) \
	$(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(BOOST_PROGRAM_OPTIONS_LIBS)

# bash-dif
bash_dif_SOURCES = bash_dif_lexer.cc
bash_dif_CPPFLAGS =
bash_dif_LDFLAGS =

# openscad-dif
openscad_dif_SOURCES = \
	openscad_dif_lexer.cc \
	openscad_dif_scanner.hpp \
	openscad_dif_scanner.cpp \
	openscad_dif_scanner_bif.cpp \
	openscad_dif_util.hpp \
	openscad_dif_util.cpp \
	openscad_dif_main.cpp

openscad_dif_LDFLAGS = \
	$(AM_LDFLAGS) \
	$(BOOST_REGEX_LDFLAGS) $(BOOST_REGEX_LIBS)

# openscad-seam
openscad_seam_SOURCES = \
	openscad_seam_lexer.cc \
	openscad_seam_scope.hpp \
	openscad_seam_scanner.hpp \
	openscad_seam_scanner.cpp \
	openscad_seam_main.cpp

EXTRA_DIST = \
	mainpage.dox \
	\
	bash_dif_lexer.ll \
	openscad_dif_lexer.ll \
	openscad_seam_lexer.ll

doxygen_stamp_other_dependacies = \
	mainpage.dox \
	\
	$(bash_dif_SOURCES) \
	$(openscad_dif_SOURCES) \
	$(openscad_seam_SOURCES) \
	$(programs_help)

root_path = $(DESTDIR)$(pkgdatadir)
version_path = $(root_path)/$(PACKAGE_VERSION)
install_path = $(version_path)

install_path_doc = $(version_path)/doc/src
install_path_html = $(install_path_doc)/doxygen/html
install_path_pdf = $(install_path_doc)/doxygen/latex
install_path_man = $(install_path_doc)/doxygen/man

SUFFIXES = .bash .makefile .stamp .help

##############################################################################
# targets
##############################################################################

docs-local: html-local

html-local: doxygen.stamp

pdf-local: doxygen.stamp doxygen/latex/refman.pdf

man-local: doxygen.stamp

doxygen/latex/refman.pdf:
	( cd doxygen/latex && make refman.pdf )

doxygen.stamp: Doxyfile Makefile $(doxygen_stamp_other_dependacies)
	( cat Doxyfile ; \
		echo "INPUT=$(srcdir)" ; \
		echo "PROJECT_BRIEF=\"OpenSCAD AutoMake Utilities: C++ Programs\"" ; \
	) | $(DOXYGEN_PATH) - \
	&& touch doxygen.stamp

$(programs_help): $(bin_PROGRAMS)
	./$* --help \
		| sed 's/--lib-path arg \(.*\)/--lib-path arg (=default)/' \
		| cat > $@

bash_dif_lexer.cc: bash_dif_lexer.ll
	${LEX} \
		--outfile=bash_dif_lexer.cc \
		--header-file=bash_dif_lexer.hh \
		$(srcdir)/bash_dif_lexer.ll

openscad_dif_lexer.cc: openscad_dif_lexer.ll
	${LEX} \
		--outfile=openscad_dif_lexer.cc \
		--header-file=openscad_dif_lexer.hh \
		$(srcdir)/openscad_dif_lexer.ll

openscad_seam_lexer.cc: openscad_seam_lexer.ll
	${LEX} \
		--outfile=openscad_seam_lexer.cc \
		--header-file=openscad_seam_lexer.hh \
		$(srcdir)/openscad_seam_lexer.ll

install-exec-hook:
	@list='$(bin_PROGRAMS)' ; test -n "$(EXEEXT)" && \
	  list=`for p in $$list; do echo "$$p"; done | sed 's/$(EXEEXT)$$//'`; \
	echo soft linking [$$list] version=[$(PACKAGE_VERSION)] extension=[$(EXEEXT)]; \
	( cd '$(DESTDIR)$(bindir)' && \
		for p in $$list; \
		do \
			$(am__mv) $$p$(EXEEXT) $$p-$(PACKAGE_VERSION)$(EXEEXT) && \
			$(LN_S) -vf $$p-$(PACKAGE_VERSION)$(EXEEXT) $$p$(EXEEXT) ; \
		done \
	)

install-docs-local: install-html-local

'$(install_path_html)': ; $(MKDIR_P) -v '$(install_path_html)'
install-html-local: html-local | '$(install_path_html)'
	( cd doxygen && cp -av html/* '$(install_path_html)' )

'$(install_path_pdf)': ; $(MKDIR_P) -v '$(install_path_pdf)'
install-pdf-local: pdf-local | '$(install_path_pdf)'
	( cd doxygen && cp -av latex/refman.pdf '$(install_path_pdf)' )

'$(install_path_man)': ; $(MKDIR_P) -v '$(install_path_man)'
install-man-local: man-local | '$(install_path_man)'
	( cd doxygen && cp -av man/* '$(install_path_man)' )

uninstall-local:
	@list='$(bin_PROGRAMS)' ; test -n "$(EXEEXT)" && \
	  list=`for p in $$list; do echo "$$p"; done | sed 's/$(EXEEXT)$$//'`; \
	echo unlinking [$$list] version=[$(PACKAGE_VERSION)] extension=[$(EXEEXT)]; \
	( cd '$(DESTDIR)$(bindir)' && \
		for p in $$list; \
		do \
			rm -fv $$p-$(PACKAGE_VERSION)$(EXEEXT) ; \
		done \
	)
	-test -d '$(install_path_doc)' && rm -rfv '$(install_path_doc)'

CLEANFILES = \
	doxygen.stamp \
	doxygen.tagfile \
	doxygen.warn \
	bash_dif_lexer.cc \
	bash_dif_lexer.hh \
	openscad_dif_lexer.cc \
	openscad_dif_lexer.hh \
	openscad_seam_lexer.cc \
	openscad_seam_lexer.hh \
	$(programs_help)

clean-local:
	-test -d doxygen && rm -rfv doxygen

##############################################################################
# eof
##############################################################################
