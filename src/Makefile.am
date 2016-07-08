##############################################################################
#
# top_builddir/src
#
##############################################################################

bin_PROGRAMS = \
	bash-dif \
	openscad-dif \
	openscad-seam

programs_help = \
	bash-dif.help \
	openscad-dif.help \
	openscad-seam.help

# bash-dif
bash_dif_SOURCES = bash_dif_lexer.cc
bash_dif_CXXFLAGS = -W -Wall -Wextra -ansi
bash_dif_CPPFLAGS =
bash_dif_LDFLAGS =

# openscad-dif
openscad_dif_SOURCES = openscad_dif_lexer.cc
openscad_dif_CXXFLAGS = -W -Wall -Wextra -ansi
openscad_dif_CPPFLAGS =
openscad_dif_LDFLAGS =

# openscad-seam
openscad_seam_SOURCES = \
	openscad_seam_lexer.cc \
	openscad_seam_scanner.hpp \
	openscad_seam_scanner.cpp \
	openscad_seam_main.cpp

openscad_seam_CXXFLAGS = -W -Wall -Wextra -ansi
openscad_seam_CPPFLAGS = \
	$(BOOST_CPPFLAGS) \
	-D__BASH_PATH__=\"$(BASH_PATH)\" \
	-D__MAKE_PATH__=\"$(MAKE_PATH)\" \
	-D__LIB_PATH__=\"$(pkgdatadir)/$(PACKAGE_VERSION)\" \
	-D__OPENSCAD_PATH__=\"$(OPENSCAD_PATH)\"

openscad_seam_LDFLAGS = \
	$(BOOST_FILESYSTEM_LDFLAGS) $(BOOST_FILESYSTEM_LIBS) \
	$(BOOST_SYSTEM_LDFLAGS) $(BOOST_SYSTEM_LIBS) \
	$(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(BOOST_PROGRAM_OPTIONS_LIBS)

SUFFIXES = .bash .makefile .stamp .help

EXTRA_DIST = \
	doxygen_mainpage.cpp \
	bash_dif_lexer.ll \
	openscad_dif_lexer.ll \
	openscad_seam_lexer.ll

doxygen_stamp_other_dependacies = \
	doxygen_mainpage.cpp \
	$(bash_dif_SOURCES) \
	$(openscad_dif_SOURCES) \
	$(openscad_seam_SOURCES) \
	$(programs_help)

##############################################################################
# targets
##############################################################################

docs-local: doxygen.stamp

# run doxygen and update timestamp
doxygen.stamp: Doxyfile Makefile $(doxygen_stamp_other_dependacies)
	( cat Doxyfile ; \
		echo "INPUT=$(srcdir)" ; \
		echo "PROJECT_BRIEF=\"OpenSCAD AutoMake Utilities: Support Scripts\"" ; \
	) | $(DOXYGEN_PATH) - \
	&& touch doxygen.stamp
#	&& ( cd doxygen/latex && make )

$(programs_help): $(bin_PROGRAMS)
	$* --help > $@

bash_dif_lexer.cc: bash_dif_lexer.hh
bash_dif_lexer.hh \
bash_dif_lexer.cc: bash_dif_lexer.ll
	${LEX} \
		--outfile=bash_dif_lexer.cc \
		--header-file=bash_dif_lexer.hh \
		$(srcdir)/bash_dif_lexer.ll

openscad_dif_lexer.cc: openscad_dif_lexer.hh
openscad_dif_lexer.hh \
openscad_dif_lexer.cc: openscad_dif_lexer.ll
	${LEX} \
		--outfile=openscad_dif_lexer.cc \
		--header-file=openscad_dif_lexer.hh \
		$(srcdir)/openscad_dif_lexer.ll

openscad_seam_lexer.cc: openscad_seam_lexer.hh
openscad_seam_lexer.hh \
openscad_seam_lexer.cc: openscad_seam_lexer.ll
	${LEX} \
		--outfile=openscad_seam_lexer.cc \
		--header-file=openscad_seam_lexer.hh \
		$(srcdir)/openscad_seam_lexer.ll

install-exec-hook:
	@list='$(bin_PROGRAMS)' ; test -n "$(EXEEXT)" && \
	  list=`for p in $$list; do echo "$$p"; done | sed 's/$(EXEEXT)$$//'`; \
	echo soft linking [$$list] v$(PACKAGE_VERSION) extension [$(EXEEXT)]; \
	( cd '$(DESTDIR)$(bindir)' && \
		for p in $$list; do \
			$(am__mv) $$p$(EXEEXT) $$p-$(PACKAGE_VERSION)$(EXEEXT) && \
			$(LN_S) -vf $$p-$(PACKAGE_VERSION)$(EXEEXT) $$p$(EXEEXT) \
		; done \
	)

uninstall-local:
	@list='$(bin_PROGRAMS)' ; test -n "$(EXEEXT)" && \
	  list=`for p in $$list; do echo "$$p"; done | sed 's/$(EXEEXT)$$//'`; \
	echo unlinking [$$list] v$(PACKAGE_VERSION) extension [$(EXEEXT)]; \
	( cd '$(DESTDIR)$(bindir)' && \
		for p in $$list; do \
			rm -fv $$p-$(PACKAGE_VERSION)$(EXEEXT) \
		; done \
	)

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
	-test -d doxygen \
		&& ( cd doxygen && rm -rfv html latex man ) \
		&& rmdir -v doxygen

##############################################################################
# eof
##############################################################################
