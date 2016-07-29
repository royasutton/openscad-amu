#/##############################################################################

  \mainpage OpenSCAD Automake Utilities Documentation

  \dot
    digraph example {
      node [shape=Mrecord, fontname=Helvetica, fontsize=10];
      a [label="openscad-amu" URL="\ref introduction" style="filled"];
      b [label="C++ Programs" URL="\ref programs"];
      c [label="Makefile Script Library" URL="\ref library"];
      d [label="Support Scripts" URL="\ref scripts"];
      a->b; a->c; a->d;
    }
   \enddot

  Here you will find documentation for the OpenSCAD Automake Utilities.

  \section introduction Introduction

    OpenSCAD Automake Utilities ([openscad-amu]
    (https://github.com/royasutton/openscad-amu)) are a collection of
    \ref programs "programs", a \ref library "script library" written in
    [bash] (https://www.gnu.org/software/bash/), and \ref scripts "scripts"
    to automate the invocation of [OpenSCAD] (http://www.openscad.org/).

    OpenSCAD currently lacks a scripting interface and openscad-amu attempts
    to bridge this gap by offering basic scripting capabilities via the
    OpenSCAD command line interface. Openscad-amu also aims to provide a
    framework for documenting OpenSCAD design scripts and, more importantly,
    OpenSCAD library CAD models using [Doxygen]
    (http://www.stack.nl/~dimitri/doxygen/index.html).

    When developing an OpenSCAD parameterizable model, test code
    is almost always present in the model itself; or in separate
    accompanying auxiliary scripts. Moreover, when documenting these language
    based design models, labeled images of the model parts and clear
    descriptions of its parameters are essential in conveying its function,
    use, and optional features.

    Openscad-amu establishes a framework that allows an unlimited hierarchy of
    OpenSCAD invocation scripts (\e MFScript) and/or OpenSCAD auxiliary
    scripts to accompany a related CAD model, embedded within structured
    comments of the model itself. With openscad-amu, these embedded scripts
    may, be used for, among other things, design model testing, model
    renditions for documentation, model use-case examples, model animation,
    etc. This framework reduces development efforts by connecting OpenSCAD
    script-based models with other language-based compilation methods.

    \dotfile embedding_scheme.dot "Comment Embedded Actors"

    In addition to CAD model documentation generation, these utilities prove
    to be helpful for structuring the build process of complex designs
    with numerous interdependent components. Both the design model and design
    documentation build process can be easily structured so that only the
    required targets are rebuilt as needed using constructed makefiles,
    generated from the simple bash-based makefile scripting library,
    \e MFScript.

  \section quick_start Quick Start

    To use the \e MFScript libray functions, a makefile script must
    first set the environment variable \c ${\__LIBPATH\__}. Optionally,
    \c ${\__VERBOSE\__} can be set to \c true to enable verbose output
    within the library functions. Next, source the \c bootloader.bash file
    to initialize the library. See the \ref hello_world example below for
    an introduction.

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

  \section embedding_scheme Script Embedding Scheme

    The openscad-amu methodology is based on embedding supplemental scripts,
    and documentation, into comment blocks of OpenSCAD design
    scripts. These scripts and documentation are subsequently extracted
    within the design flow to perform scripted acts on the design, producing
    targets of interest and the design documentation.

    Here is the basic structure:

    \include embedding_scheme.scad

    Using this scheme, auxiliary scripts may be contained within
    comments that do not interfere with normal OpenSCAD script
    processing. And using openscad-amu, these embedded supplemental
    OpenSCAD scripts (test, documentation, example, etc.) and makefile
    scripts (OpenSCAD invocation control) can be extracted and used within
    structured automatic build procedures.

    Here are the resulting \link embedding_scheme.scripts script file
    names\endlink extracted from the above embedding example using
    \ref openscad_seam.

  \section example Complete Example

    To better show how to use openscad-amu, here is a trivial, but fully
    functional, documented example library. The library consists of a single
    OpenSCAD \link vehicle.scad design file\endlink. This file,
    together with a Doxygen \link vehicle.doxyfile configuration
    file\endlink are used to generate the complete library documentation,
    render over 90 different views of modules in the library, and render
    six STL design models.

    Embedded within comments toward the end of the design file are two auxiliary
    script scopes: \b test and \b document. These scopes become part of the
    extracted script names. The utility \ref openscad_seam
    extracts these OpenSCAD auxiliary scripts in both scopes
    (\link vehicle_document.scad document\endlink and
     \link vehicle_test.scad test\endlink)
    and the MFScripts auxiliary scripts in both scopes
    (\link vehicle_document.bash document\endlink and
     \link vehicle_test.bash test\endlink).
    In addition, it runs each extracted \e MFScript to generate
    the makefiles for both scopes
    (\link vehicle_document.makefile document\endlink and
     \link vehicle_test.makefile test\endlink),
    and subsequently runs \c make on these makefiles to generate all of
    the appropriate target; in this case, image renderings, image conversions,
    and STL model generation.

    Extract scripts, generate makefile, and make all targets:

    \verbatim
      $ openscad-seam \
          --input vehicle.scad \
          --prefix build \
          --run yes \
          --make yes \
          --define __INCLUDE_PATH__="./" \
          --verbose
    \endverbatim

    To produce the design documentation, the Doxygen input filter,
    \ref openscad_dif, is used to pre-process the Doxygen documentation
    embedded in the design model. See the \c FILTER_* setting in the
    example Doxygen \link vehicle.doxyfile configuration file\endlink.

    Finally, \c doxygen is run to generate documentation (please consult
    the [Doxygen] (http://www.stack.nl/~dimitri/doxygen/index.html)
    project web site for details on its use):

    \verbatim
      $ doxygen vehicle.doxyfile
    \endverbatim

    Here is the resulting library documentation in
    <a href="../../../examples/build/html/index.html">HTML format</a> and latex
    <a href="../../../examples/build/latex/refman.pdf">generated PDF</a>.

  \section library Makefile Script Library

    - \ref library_core "Core Members"
    - \ref library_support "Support Members"

  \subsection library_files Files

    - bootloader.bash
    - print.bash
    - print_textbox.bash
    - exception.bash
    - table.bash
    - openscad_opts.bash
    - utility.bash
    - make_makefile.bash

  \todo work on an example that demonstrates dimensioning.
  \todo create include mk for creating videos (export stem name to makefile)
  \todo work on an example that demonstrates animation creation.
  \todo debug missing double dash on cygwin doxygen builds
  \todo incorporate support for web-based stl viewer like
        [JavaScript 3D library] (https://github.com/mrdoob/three.js)
  \todo a syntax highlighter extension would be nice: Doxygen with
        amu extensions and amu auxilary script scopes for OpenSCAD and
        MFScript (extended BASH).

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

    This is a sample text. fill me in.

    Its bash. Any bash command can be use.

    here is an example:

    \include log_segregate.bash

  \warning

    As in all bash scripts, long lines may be broken up into several
    smaller lines and continued using the '\' character at the end of each
    preceding line. When writing scripts directly this must be done
    explicitly. When writing embedded scripts that will be extracted using
    \ref openscad_seam, long lines are explicitly terminated using the ';'
    character. Therefore this is no need to use the continuation character.
    Long lines will be joined together and written to a single line when
    the line termination character is encountered.

    \dontinclude vehicle.scad
    \skipline Begin_Scope _document
    \until End_Scope

  \note

    All script function names are case sensitive. The function argument
    keywords are case insensitive.

###############################################################################/


#/##############################################################################

  \defgroup library_support Makefile Script Library Support

    \copydoc library_arguments

    This is a sample text. fill me in.

    Its bash. Any bash command can be use.

    here is an example:

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

###############################################################################/


#
# eof
#
