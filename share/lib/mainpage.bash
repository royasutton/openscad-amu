#/##############################################################################
  \mainpage OpenSCAD Automake Utilities

  \dot "Compile-Scripting and Documentation-Generatation for OpenSCAD Designs."
    digraph example {
      node [shape=Mrecord, fontname=Helvetica, fontsize=10];
      a [label="openscad-amu" URL="\ref index" style="filled"];
      b [label="C++ Programs" URL="\ref programs"];
      c [label="Makefile Script Library" URL="\ref mslibrary"];
      d [label="Support Scripts" URL="\ref scripts"];
      a->b; a->c; a->d;
    }
   \enddot

    A _framework_ and tools for automating the compilation of
    [OpenSCAD] designs with a _preprocessor_ that allows
    <i><b>*.scad</b></i> to be documented using [Doxygen].


  \section what_is_amu What is it?

    [openscad-amu] is being developed to support the construction of
    automated design flows with Doxygen generated documentation for
    OpenSCAD language-based mechanical design. It provides a framework
    that allows documentation and build-scripts to be embedded into
    <i><b>*.scad</b></i> design source. It uses a collection of C++
    \ref programs "programs", a makefile script \ref mslibrary
    "library", and support \ref scripts "scripts" for automating the
    compilation of [OpenSCAD] designs. It includes _preprocessor_
    source code filters that allows <i><b>*.scad</b></i> (and
    <i><b>*.bash</b></i>) to be documented using [Doxygen]. The
    embedded build scripts are extracted during design compilation to
    construct [Makefiles] that manage the generation of design targets.
    The source code input-filters extend [Doxygen] and provide
    additional [special commands] with features useful in mechanical
    design.

  \section an_example Example

    See this source \ref vehicle.scad "file" which represents a trivial
    design library. It differs from a typical OpenSCAD design library
    in that it includes documentation embedded in comment sections,
    and <em>auxiliary build scripts</em> that describe how to render
    numerous design targets.

    \dot "Design Flow Overview."
      digraph example {
        rankdir="LR";
        node [fontname=Helvetica fontsize=10];
        edge [arrowhead=vee];
        a  [shape=note label="Annotated\nSource\n(vehicle.scad)" URL="\ref vehicle.scad"];
        b  [shape=oval peripheries=2 label="Automated\nDesign\nFlow" URL="\ref design_flow"];
        c  [shape=component label="Target (1)\n...\nTarget (96)" URL="../../../examples/build/html/stl/vehicle_test_car_17.stl"];
        d  [shape=folder label="Design\nLibrary\nDocumentation" URL="../../../examples/build/html/index.html"];
        a->b; b->{c d};
      }
    \enddot

    When compiled, this simplified example generates over 750 targets
    <tt>(*.png, *.stl, etc.)</tt> and assembles the library
    documentation (including two animated GIF images). [openscad-amu]
    brings together several widely-used tools to automate the
    production of [OpenSCAD] design targets and the design
    documentation in various output formats ([HTML][html-example])
    (PDF).

    \note The PDF version of this example is not built by default. To
    build it, go the the latex build directory, <tt>(cd
    .../build/Linux/share/examples/build/latex)</tt>, add
    <tt>"\graphicspath{{png2eps/}}"</tt> just below
    <tt>"\usepackage{graphicx}"</tt> in \c refman.tex, and type \c make
    to build \c refman.pdf.

  \section getting_started Getting Started

    [openscad-amu] incorporated two complementary but distinct features
    that may be used together or independantly: (1) Scripted design
    compilation and (2) design documentation generation. In either
    case, the design <i><b>*.scad</b></i> source files are augmented
    with auxiliary comments. If you are familiar with [Doxygen], adding
    basic documentation to your [OpenSCAD] designs using [openscad-amu]
    is straight forward. Markup each of your design files with the
    [special commands], name each file in the project makefile, and
    type \c make to generate your documentation. You can start from a
    template created by the \c setup-amu.bash script, then customize as
    needed.

    \li \subpage source_annotate
    \li \subpage design_flow
    \li \subpage an_example_more "More on the example"

  \section framework_components Framework Components

    \li \subpage programs "C++ Programs"
    \li \subpage mslibrary "Makefile Script Library"
    \li \subpage scripts "Support Scripts"

  [GNU General Public License]: https://www.gnu.org/licenses/gpl.html
  [Makefiles]: https://www.gnu.org/software/make

  [openscad-amu]: https://royasutton.github.io/openscad-amu
  [repository]: https://github.com/royasutton/openscad-amu
  [issue]: https://github.com/royasutton/openscad-amu/issues

  [OpenSCAD]: http://www.openscad.org/

  [Doxygen]: http://www.doxygen.nl
  [special commands]: http://www.doxygen.nl/manual/commands.html

  [html-example]: ../../../examples/build/html/index.html
  [pdf-example]: ../../../examples/build/latex/refman.pdf
###############################################################################/


#/##############################################################################
  \page source_annotate Annotating the source

    To avoid interfering with the primary OpenSCAD code, documentation
    is placed inside of comments sections in the source following the
    rules established by Doxygen. Auxiliary models and build scripts
    are placed inside named nestable block sections within comments.
    These auxiliary script are embedded in a hierarchical scoping
    scheme as shown the following diagram:

    \dotfile embedding_scheme.dot "Nestable Auxiliary Scripts."

    Therefore, the documentation and build scripts coexists within a
    design source file. They are subsequently extracted to generate the
    design documentation and to automate the design target generation.

    The source annotation structue looks like this:

    \include embedding_scheme.scad

    In this example, there are three modeling and three build scripts
    embedded in the auxiliary scopes: A, A_1, and B. The utility \ref
    openscad_seam is used to identify and/or extract them as shown in
    \ref embedding_scheme.scripts "here." The utility \ref openscad_dif
    is used to pre-process the embedded special commands for Doxygen.
###############################################################################/


#/##############################################################################
  \page design_flow The Design flow

    A project includes a Project Makefile, the project source files,
    and a documentation configuration file (Doxyfile). Any source file
    may be annotated with documentation and/or one or more automation
    scripts. Each script is extracted at compile time to generate a
    _scope_ makefile that controls the generation of corresponding
    targets.

    [openscad-amu] brings together [OpenSCAD], [Doxygen], [GNU Make],
    and \ref mslibrary "custom scripting" (based on [GNU Bash]) to
    automate the generation of the design documentation and design
    targets.

    \dot "Design Flow."
      digraph example {
        rankdir="LR";
        node [fontname=Helvetica fontsize=12];
        edge [arrowhead=vee];

        a1 [shape=note label="Project\nMakefile" URL="\ref project_makefile"];
        a2 [shape=note label="Source\n(*.scad)" URL="\ref vehicle.scad"];
        a3 [shape=note label="Project\nDoxyfile" URL="\ref vehicle.doxyfile"];
        b  [shape=box peripheries=2 style="filled" label="openscad-seam\n(extractor)" URL="\ref openscad_seam_ht"];

        subgraph cluster_scope {
        graph[fontname=Helvetica, fontsize=12, fontcolor=gray, color=gray style=dashed];
        label="Source Scope";

            c  [shape=note label="OpenSCAD\nModeling\nScript\n(*.scad)" URL="\ref vehicle_document.scad"];
            d  [shape=note label="MFScript\nBuild\nScript\n(*.bash)" URL="\ref vehicle_document.bash"];
            e  [shape=note label="Makefile" URL="\ref vehicle_document.makefile"];
            f1 [shape=component label="Targets\n(png)" URL="../../../examples/build/html/png/vehicle_test_car_17_green_diag_320x240.png"];
            f2 [shape=component label="Targets\n(stl)" URL="../../../examples/build/html/stl/vehicle_test_car_17.stl"];
            f3 [shape=none label="..."];
            fn [shape=component label="Targets\n(n)" URL="../../../examples/build"];

            c->e; d->e;
            e->f1; e->f2; e->f3 [style=invis]; e->fn;
        }

        g  [shape=box peripheries=2 style="filled" label="openscad-dif\n(Doxygen filter)" URL="\ref openscad_dif_ht"];
        h  [shape=box peripheries=2 label="Doxygen" URL="http://www.doxygen.nl"];
        i  [shape=folder label="Generated\nDocumentation\nhtml, pdf, etc." URL="../../../examples/build/html/index.html"];

        a2->{b g}; a3->h; b->{c d};
        g->h; h->i;

        { rank=same; a1; a2; a3; }
      }
    \enddot

    The overall design flow is controlled using a root makefile for the
    entire project. A basic project makefile example is shown below for
    a simple project with two scopes.

  \section project_makefile A minimal Project Makefile

    \code{makefile}
      \#
      \# Project Makefile
      \#

      srcdir=./

      all: doxygen.stamp

      doxygen.stamp: vehicle.scad vehicle_test.stamp vehicle_document.stamp
        doxygen vehicle.doxyfile

      .makefile.stamp:
        OPENSCADPATH="${srcdir}" \
        make -f $< all
        touch $@

      vehicle_document.makefile \
      vehicle_test.makefile: vehicle.scad | build
        openscad_seam \
          --input $< \
          --prefix build --prefix-ipp 0 --prefix-scripts no \
          --run yes --make no

      build: ; mkdir -v build
    \endcode

    [openscad-amu]: https://royasutton.github.io/openscad-amu
    [OpenSCAD]: http://www.openscad.org
    [Doxygen]: http://www.doxygen.nl
    [GNU Make]: https://www.gnu.org/software/make
    [GNU Bash]: https://www.gnu.org/software/bash
###############################################################################/


#/##############################################################################
  \page an_example_more More on the example

    Here is the design \ref vehicle.scad "example" that was introduced
    in the section \ref an_example "\"An example\"".

    As discussed, it includes structured documentation and build
    scripts added to the source in comments. This source file together
    with a \ref vehicle.doxyfile "configuration" file are used to
    generate the design documentation, render the documentation images,
    and render the STL design models. If and when there is a need to
    change the source, all targets can be updated as required by the
    scope makefiles generated from the accompanying build scripts
    (typically invoked from the project makefile).

    In this example, there are four auxiliary scripts in two scopes:
    namely \em test and \em document. The scripts are extracted with
    the utility \ref openscad_seam and are used to create makefiles
    that control the build process for each scope.

    These scripts and makefiles are summarized in the following table:

    | Scope    | Makefile Script Model      | Makefile Script            | Makefile                       |
    |:--------:|:--------------------------:|:--------------------------:|:------------------------------:|
    | test     | \ref vehicle_test.scad     | \ref vehicle_test.bash     | \ref vehicle_test.makefile     |
    | document | \ref vehicle_document.scad | \ref vehicle_document.bash | \ref vehicle_document.makefile |

    The following command extracts the model and makefile scripts,
    generates the makefiles, and builds the targets for all scopes of
    the input source file:

    \verbatim
      $ openscad-seam \
          --input vehicle.scad \
          --prefix build \
          --run yes \
          --make yes \
          --define __INCLUDE_PATH__="./" \
          --verbose
    \endverbatim

    To format the Doxygen special commands, the source file is
    pre-processed using \ref openscad_dif and then passed to Doxygen.
    This is specified using the \c INPUT_FILTER or \c FILTER_PATTERNS
    configuration option as shown in the \ref vehicle.doxyfile.

    \warning The Doxygen \c FILTER_PATTERNS
             [bug \#504305](https://bugzilla.gnome.org/show_bug.cgi?id=504305)
             has been fixed in Doxygen version 1.8.12. A workaround that
             had been used ("*.ext=\"filter opts\\\"") no longer works after
             the fix. To deal with this bug across doxygen versions, use
             \c INPUT_FILTER to specify any filter that requires command line
             options until version 1.8.12 reaches more repositories. The
             new way to specify the filter pattern with command line
             options is (*.ext="filter options").

    The following command generates the documentation:

    \verbatim
      $ doxygen vehicle.doxyfile
    \endverbatim

    Here is the resulting library documentation ([HTML][html-example])
    (PDF).

  [html-example]: ../../../examples/build/html/index.html
  [pdf-example]: ../../../examples/build/latex/refman.pdf
###############################################################################/


#/##############################################################################
  \page mslibrary Makefile script library

  \dot "Compile-Scripting and Documentation-Generatation for OpenSCAD Designs."
    digraph example {
      node [shape=Mrecord, fontname=Helvetica, fontsize=10];
      a [label="openscad-amu" URL="\ref index"];
      b [label="C++ Programs" URL="\ref programs"];
      c [label="Makefile Script Library" URL="\ref mslibrary" style="filled"];
      d [label="Support Scripts" URL="\ref scripts"];
      a->b; a->c; a->d;
    }
   \enddot

  \section library_groups Groups

    - \ref library_core "Core members"
    - \ref library_support "Support members"

  \section library_files Files

    - bootloader.bash
    - print.bash
    - print_textbox.bash
    - exception.bash
    - table.bash
    - openscad_opts.bash
    - utility.bash
    - make_makefile.bash

    To use the library functions, a script must first set the environment
    variable \c ${\__LIB_PATH\__}. Optionally, \c ${\__VERBOSE\__} can be
    set to \c true to enable verbose output within the library functions.
    Next, source the \c bootloader.bash file to initialize the library.
    See the \ref hello_world example below for an example.

  \section hello_world Hello World

    \includelineno hello_world.bash

    This script will create a makefile with four targets. Line 11
    through 15 create two set tables, named \c view and \c size, that
    contain command line option strings. Each entry has a table name, a
    key name, and a key value string . Line 17 specified the basic
    parameters of the makefile to be generated, including the two table
    names that were created with the command line options. Line 25
    begins the creation of the makefile.

    When this script is run, it will generate a makefile named \c
    hello_world.makefile with four targets. Each target will invoke
    OpenSCAD once for the four resulting combinations of the two
    entries in the options table. The commons OpenSCAD options set by
    \c set_opts on line 22 will be used during the invocation all of
    the generated targets.

    Here is the resulting generated
    \link hello_world.makefile makefile.\endlink
###############################################################################/


#/##############################################################################
  \defgroup library_arguments Argument naming convention

    The library is written using the Bourne Again SHell (bash)
    scripting language. Bash variables are untyped and are all basically
    character strings. Their use depends on the context. Bash function
    parameters are passed as positional character strings with no type
    checking. To use named function parameters, parameter parsing code
    is required. This library makes use of both positional and named
    parameters.

    To help document the use of these functions, the following
    conventions are made:

    - Required vs. optional,
    - Matching method,
    - Type or use,
    - Limits, and
    - Dimension.

    \b Required:
    Some parameters are required and some are optional. When a parameters
    is required, its type description begins with the word \c required.
    When a parameter is optional, there is no need to label it such
    unless this is desired just for clarification.

    \b Method:
    Two matching methods are used. Positional and named. Positional
    arguments will have the description \c arg1, \c arg2, ..., \argn.
    The description \c argv is used to identify the entire list of arguments.
    When arguments are named, the description \c name is used. The parameter
    name keyword will be identified by the final name after the type
    description.

    \b Type:
    Although all type are essentially character strings, this is used to
    give a better idea of how the parameter is used. Typical values are:
    \c character, \c string, \c integer, \c file, \c path, \c function.

    \b Limits:
    This give guidance on what are acceptable values for the parameter.
    It it often used to indicate that a preset value is expected. It may
    also used to set the value ranges on integers, such as 1-10.

    \b Dimension:
    Parameter values may be either a single words or a list of words.
    To assign a list of words to a parameter value, the list must be
    enclosed in single or double quotation marks.
###############################################################################/


#/##############################################################################
  \defgroup library_core Makefile script library core

  \copydoc library_arguments

  \warning  As in all bash scripts, long lines may be broken up into several
            smaller lines and continued using the '\\' character at the end
            of each preceding line. When writing scripts directly this must
            be done explicitly. When writing embedded scripts that will be
            extracted using \ref openscad_seam, long lines are explicitly
            terminated using the ';' character. Therefore this is no need
            to use the continuation character. Long lines will be joined
            together and written to a single line when the line termination
            character is encountered.

    here is an example stand along script:

    \include log_segregate.bash

    here is an example of an embedded script:

    \dontinclude vehicle.scad
    \skipline Begin_Scope document
    \until End_Scope

  \note All script function names are case sensitive. The function argument
        keywords are case insensitive.
###############################################################################/


#/##############################################################################
  \defgroup library_support Makefile script library support

  \copydoc library_arguments
###############################################################################/


#/##############################################################################
  \example convert.bash
    An example script that uses target output conversion.
  \example convert.makefile
    Resulting makefile generated by convert.bash.

  \example hello_world.bash
    A simple hello world makefile script example.
  \example hello_world.makefile
    Resulting makefile generated by hello_world.bash.

  \example log_segregate.bash
    An example script that segregates target output logs.
  \example log_segregate.makefile
    Resulting makefile generated by log_segregate.bash.

  \example embedding_scheme.scad
    OpenSCAD and makefile script hierarchical embedding scheme.
  \example embedding_scheme.scripts
    Parsed OpenSCAD and makefile script names in embedding_scheme.scad.

  // complete example files
  \example vehicle.scad
    A complete working example using the openscad-amu framework.
  \example vehicle.doxyfile
    The doxygen configuration file used to create the documents for vehicle.scad.

  \example vehicle_document.scad
    OpenSCAD script extracted from vehicle.scad.
  \example vehicle_document.bash
    MFSript extracted from vehicle.scad.
  \example vehicle_document.makefile
    Makefile created by extracted MFSript vehicle_document.bash

  \example vehicle_test.scad
    OpenSCAD script extracted from vehicle.scad.
  \example vehicle_test.bash
    MFSript extracted from vehicle.scad.
  \example vehicle_test.makefile
    Makefile created by extracted MFSript vehicle_test.bash

  \example animate_move.scad
    OpenSCAD script extracted from animate.scad.
  \example animate_move.bash
    MFSript extracted from animate.scad.
  \example animate_move.makefile
    Makefile created by extracted MFSript animate_move.bash

  \example animate_rotate.scad
    OpenSCAD script extracted from animate.scad.
  \example animate_rotate.bash
    MFSript extracted from animate.scad.
  \example animate_rotate.makefile
    Makefile created by extracted MFSript animate_rotate.bash
###############################################################################/


#
# eof
#
