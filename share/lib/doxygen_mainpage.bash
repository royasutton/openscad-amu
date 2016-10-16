#/##############################################################################
  \mainpage OpenSCAD Automake Utilities

  \dot
    digraph example {
      node [shape=Mrecord, fontname=Helvetica, fontsize=10];
      a [label="openscad-amu" URL="\ref index" style="filled"];
      b [label="C++ Programs" URL="\ref programs"];
      c [label="Makefile Script Library" URL="\ref mslibrary"];
      d [label="Support Scripts" URL="\ref scripts"];
      a->b; a->c; a->d;
    }
   \enddot

  \section what_is_amu What is it?

    [openscad-amu](https://github.com/royasutton/openscad-amu) is a
    collection of \ref programs "programs", a \ref mslibrary "script library"
    written in [bash] (https://www.gnu.org/software/bash/), and support
    \ref scripts "scripts." It provides a framework that can be used to
    structure the automated compilation of [OpenSCAD] (http://www.openscad.org/)
    designs and provides utilities that allows *.scad source code to be documented
    using [Doxygen](http://www.stack.nl/~dimitri/doxygen/index.html) markup
    commands.

  \section have_a_look Have a look

    To see what openscad-amu can do, we start with this \ref vehicle.scad
    "source file" that represents a simplified but typical OpenSCAD
    parameterized design library. It differs only in that it includes
    (1) \em markups that document how to use the source library and
    (2) comment-embedded \em auxiliary \em scripts that code how to
    compile design targets. This augmented source example is able to
    produce the images that are part of the library documentation and
    render 96 targets of interest <tt>(*.png, *.stl, etc.)</tt>.

    \dot
      digraph example {
        rankdir="LR";
        node [fontname=Helvetica fontsize=10];
        edge [arrowhead=vee];
        a  [shape=note label="Augmented\nSource\n(vehicle.scad)" URL="\ref vehicle.scad"];
        b  [shape=oval peripheries=2 label="Automatic\nDesign\nFlow" URL="\ref design_flow"];
        c  [shape=component label="Target (1)\n...\nTarget (96)" URL="../../../examples/build/html/vehicle_test_car_17.stl"];
        d  [shape=folder label="Design\nLibrary\nDocumentation" URL="../../../examples/build/html/index.html"];
        a->b; b->{c d};
      }
    \enddot

    \em openscad-amu brings together several standard tools to automate
    the production of \em OpenSCAD design targets and the design source
    <a href="../../../examples/build/html/index.html">documentation</a>
    <a href="../../../examples/build/latex/refman.pdf">(PDF)</a>.

  \section introduction Introduction

    To document language-based mechanical designs, it seems natural to
    leverage existing tools that extract its documentation directly from
    the source code. Inasmuch, openscad-amu provides a <tt>*.scad</tt>
    source filter that may be used in conjunction with the widely used
    Doxygen.

    For automated target production, openscad-amu uses a framework that
    allows auxiliary scripts to be written into structured comment blocks
    of <tt>*.scad</tt> source. These scripts are later interpreted
    to construct Makefiles that automate OpenSCAD design target renderings.
    These embedded scripts may be used for general model rendering,
    testing, documentation image production, use-case examples, animation,
    etc.

    \subpage augmenting_source

    \subpage design_flow

    \subpage an_example "More on the Example"

  \section framework_components Framework Components

    \li \subpage programs "C++ Programs"
    \li \subpage mslibrary "Makefile Script Library"
    \li \subpage scripts "Support Scripts"


  \todo work on an example that demonstrates dimensioning.
  \todo work on an example that demonstrates animation creation.
  \todo create include makefile for creating videos (may need to export
        stem name to a makefile variable).
  \todo incorporate support for web-based stl viewer like
        [JavaScript 3D library](https://github.com/mrdoob/three.js).
  \todo a syntax highlighter extension for a text editor would be nice:
        Doxygen with amu extensions and amu auxiliary script scopes for
        OpenSCAD and MFScript (extended BASH).
###############################################################################/


#/##############################################################################
  \page augmenting_source Augmenting the Source

    To avoid interfering with the primary OpenSCAD design behavior,
    auxiliary scripts and design documentation are placed inside
    source code comment sections. A hierarchical scoping scheme is used
    to track and identify them as shown the following diagram:

    \dotfile embedding_scheme.dot

    In this way, the documentation and scripts may co-exists with the
    design source. They are subsequently identified and extracted to
    generate the design documentation and automate the design rendering.

    The design source augmentation looks like this:

    \include embedding_scheme.scad

    In the above example, there are three modeling and three build scripts.
    The utility \ref openscad_seam is used to identify and extract them as
    shown in this \ref embedding_scheme.scripts "output example." The
    utility \ref openscad_dif is used to pre-process the embedded
    documentation that is generated by Doxygen.
###############################################################################/


#/##############################################################################
  \page design_flow The Design Flow

    [openscad-amu](https://github.com/royasutton/openscad-amu) brings
    together [OpenSCAD](http://www.openscad.org/),
    [Doxygen](http://www.stack.nl/~dimitri/doxygen/),
    [make](https://www.gnu.org/software/make), and hierarchal scoped
    \ref mslibrary "scripting" to automate the production of the design
    documentation and design targets. A design project will typically
    include a project Makefile, a project Doxygen configuration file
    (Doxyfile), and the project source files. For each scope of each
    source file there will be a modeling script, a build script, a
    generated makefile, and targets, as shown in the design flow
    diagram below.

    \dot
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
            f1 [shape=component label="Targets\n(png)" URL="../../../examples/build/html/vehicle_test_car_17_green_diag_320x240.png"];
            f2 [shape=component label="Targets\n(stl)" URL="../../../examples/build/html/vehicle_test_car_17.stl"];
            f3 [shape=none label="..."];
            fn [shape=component label="Targets\n(n)" URL="../../../examples/build"];

            c->e; d->e;
            e->f1; e->f2; e->f3 [style=invis]; e->fn;
        }

        g  [shape=box peripheries=2 style="filled" label="openscad-dif\n(Doxygen filter)" URL="\ref openscad_dif_ht"];
        h  [shape=box peripheries=2 label="Doxygen" URL="http://www.stack.nl/~dimitri/doxygen"];
        i  [shape=folder label="Generated\nDocumentation\nhtml, pdf, etc." URL="../../../examples/build/html/index.html"];

        a2->{b g}; a3->h; b->{c d};
        g->h; h->i;

        { rank=same; a1; a2; a3; }
      }
    \enddot

    The design flow is best automated using a makefile for the entire
    project. A basic project makefile  example is shown below for a
    project with two scopes.

  \section project_makefile Project Makefile

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

###############################################################################/


#/##############################################################################
  \page an_example An Example

    Here is the \ref vehicle.scad "design example" that was introduced
    in the section \ref have_a_look "\"Have a look.\"" As discussed, it
    has had documentation markups and auxiliary scripts added to the
    source. This source file together with a \ref vehicle.doxyfile
    "configuration file" are used to generate the design documentation,
    render the documentation images, and render the STL design models.
    If there is ever a need to change the source, all 96 targets can be
    updated as required by the scope makefiles generated from the accompanying
    build scripts (typically invoked from the project makefile).

    In this example, there are four auxliary scripts in two scopes: namely
    (1) \em test and (2) \em document. The scripts are extracted with the
    utility \ref openscad_seam and are used to create makefiles that control
    the build process for each scope.

    These scripts and makefiles are summarized in the following table:

    Scope    | Modeling Script            | Build Script               | Makefile
    :-------:|:--------------------------:|:--------------------------:|:------------------------------:
    test     | \ref vehicle_test.scad     | \ref vehicle_test.bash     | \ref vehicle_test.makefile
    document | \ref vehicle_document.scad | \ref vehicle_document.bash | \ref vehicle_document.makefile

    The following command extracts the scripts, generated the makefiles, and
    builds the targets for all scopes of the input source file:

    \verbatim
      $ openscad-seam \
          --input vehicle.scad \
          --prefix build \
          --run yes \
          --make yes \
          --define __INCLUDE_PATH__="./" \
          --verbose
    \endverbatim

    To format the documentation, the source file is pre-processed using
    \ref openscad_dif and passed to Doxygen. This is specified using the
    \c INPUT_FILTER or \c FILTER_PATTERNS configuration option as shown
    in the \ref vehicle.doxyfile.

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

    Here is the resulting library documentation in
    <a href="../../../examples/build/html/index.html">HTML format</a> and latex
    <a href="../../../examples/build/latex/refman.pdf">generated PDF</a>.
###############################################################################/


#/##############################################################################
  \page mslibrary Makefile Script Library

  \dot
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

    - \ref library_core "Core Members"
    - \ref library_support "Support Members"

  \section library_files Files

    - bootloader.bash
    - print.bash
    - print_textbox.bash
    - exception.bash
    - table.bash
    - openscad_opts.bash
    - utility.bash
    - make_makefile.bash

    To use the libray functions, a script must first set the environment
    variable \c ${\__LIB_PATH\__}. Optionally, \c ${\__VERBOSE\__} can be
    set to \c true to enable verbose output within the library functions.
    Next, source the \c bootloader.bash file to initialize the library.
    See the \ref hello_world example below for an example.

  \section hello_world Hello World

    \includelineno hello_world.bash

    This simple script will create a makefile with four targets. Line 11
    through 15 create two set tables, named \c view and \c size, that
    contain command line option strings. Each entry has a table name,
    a key name, and a key value string . Line 17 specified the basic
    parameters of the makefile to be generated, including the two table
    names that were created with the command line options. Line 25 begins
    the creation of the makefile.

    When this script is run, it will generate a makefile named
    \c hello_world.makefile with four targets. Each target will invoke OpenSCAD
    once for the four resulting combinations of the two entries in the two
    table. The commons OpenSCAD options set by \c set_opts on line 22 will
    be used during the invokation all of the generated targets.

    Here is the resulting \link hello_world.makefile makefile.\endlink
###############################################################################/


#/##############################################################################
  \defgroup library_arguments Argument Naming Convention

    The library is written using the Bourne Again SHell (bash)
    scripting language. Bash variables are untyped and are all basically
    character strings. Their use depends on the context. Bash function
    parameters are passed as positional character strings with no type
    checking. To use named function parameters, parameter parsing code
    is required. This library makes use of both positional and named
    parameters.

    To help document the use of these functions, the following
    conventions are made over five parameters characteristics:

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
  \defgroup library_core Makefile Script Library Core

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
  \defgroup library_support Makefile Script Library Support

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
