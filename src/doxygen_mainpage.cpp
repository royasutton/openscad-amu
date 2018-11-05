/***************************************************************************//**
  \mainpage C++ Programs

  \dot "Compile-Scripting and Documentation-Generatation for OpenSCAD Designs."
    digraph example {
      node [shape=Mrecord, fontname=Helvetica, fontsize=10];
      a [label="openscad-amu" URL="../../../share/lib/doxygen/html/index.html"];
      b [label="C++ Programs" URL="index.html" style="filled"];
      c [label="Makefile Script Library" URL="../../../share/lib/doxygen/html/mslibrary.html"];
      d [label="Support Scripts" URL="../../../share/scripts/doxygen/html/index.html"];
      a->b; a->c; a->d;
    }
  \enddot

  \section programs Programs

  \subsection bash_dif bash-dif

    \li \subpage bash_dif_dtc "Doxygen markup for Bash script (*.bash)"
    \li \subpage bash_dif_ht "Command line help"
    \li \ref bash_dif_src "Source code"

  \subsection openscad_dif openscad-dif

    \li \subpage openscad_dif_dtc "Doxygen markup for OpenSCAD script (*.scad)"
    \li \subpage openscad_dif_ht "Command line help"
    \li \ref openscad_dif_src "Source code"

  \subsection openscad_seam openscad-seam

    \li \subpage openscad_seam_ht "Command line help"
    \li \ref openscad_seam_src "Source code"


  \todo create a test suite that compares output from each program to
        known good results.
*******************************************************************************/


/***************************************************************************//**
  \defgroup bash_dif_src Source code: bash-dif
  \brief
    The Doxygen input filter for \em Bash scripts (<tt>*.bash</tt>).
*******************************************************************************/
/***************************************************************************//**
  \defgroup openscad_dif_src Source code: openscad-dif
  \brief
    The Doxygen input filter for \em OpenSCAD scripts (<tt>*.scad</tt>).
*******************************************************************************/
/***************************************************************************//**
  \defgroup openscad_seam_src Source code: openscad-seam
  \brief
    The auxiliary script extractor for \em OpenSCAD scripts (<tt>*.scad</tt>).
*******************************************************************************/


/***************************************************************************//**
  \page bash_dif_ht Command line help: bash-dif
  \include bash-dif.help
*******************************************************************************/
/***************************************************************************//**
  \page openscad_dif_ht Command line help: openscad-dif
  \include openscad-dif.help
*******************************************************************************/
/***************************************************************************//**
  \page openscad_seam_ht Command line help: openscad-seam
  \include openscad-seam.help
*******************************************************************************/


/***************************************************************************//**
  \page bash_dif_dtc Documenting Bash script (*.bash)

  This filter may be used to markup bash scripts for use with Doygen.
  See [Documenting the code]
  (http://www.stack.nl/~dimitri/doxygen/manual/docblocks.html)
  in the Doxygen manual for information.

  <b>Comment Sections:</b>

  Comment patters are identified according to the rules in the
  following table. See the flex documentation on [patterns]
  (http://flex.sourceforge.net/manual/Patterns.html)
  for more information.

   patterns   | behavior
  :----------:|:-----------------------------------------
   "#"        | comment. ignored by filter.
   "##"[#]*   | documentation pass to Doxygen.
   "#/"[#!/]+ | open a comment block.
   "#"+"/"    | close comment block.
   "#"[#]*    | comment line while in open comment block.

  <b>Additional Doxygen Commands:</b>

  In addition to all of the normal Doxygen [special commands,]
  (http://www.stack.nl/~dimitri/doxygen/manual/commands.html)
  here is a list of additional filter-provided ones that can be used for
  document markup:

  | Additional Commands             | Description
  |:--------------------------------|:--------------------------
  | \\afn id                        | function name
  | \\aparam	<type> [direction] id | function parameter
  | \\aparami <type> id             | function parameter input
  | \\aparamo <type> id             | function parameter output

  \section bash_markup_ex Example Markup

    \verbatim
    # This comment will not be passed to Doxygen.
    ## \ingroup the_group_name
    ## @{

    ## Create a camera view option set table from predefined names.
    #/##################################################################
    # \afn function
    #
    # \aparam  [in] <optional integer>      width      Set table width.
    # \aparami      <required named string> name       Set table name.
    # \aparamo      <named string>          translate  String of integers.
    #
    # \returns void
    #
    # \details
    #   This function provides an abstracted way to create option
    #    set tables for predefined views.
    ###################################################################/

    ## @}
    \endverbatim
*******************************************************************************/

/***************************************************************************//**
  \page openscad_dif_dtc Documenting OpenSCAD scripts (*.scad)

  This filter may be used to markup OpenSCAD scripts for use with Doygen.
  See [Documenting the code]
  (http://www.stack.nl/~dimitri/doxygen/manual/docblocks.html)
  in the Doxygen manual for information.

  <b>Comment Sections:</b>

  Comment patters are identified according to the rules in the
  following table. See the flex documentation on [patterns]
  (http://flex.sourceforge.net/manual/Patterns.html)
  for more information.

   patterns   | behavior
  :----------:|:--------------------------------
   "//"       | comment. ignored by filter.
   "//"[/!]?  | documentation pass to Doxygen.
   "/*"[*!]?  | open a comment block.
   "*"+"/"    | close comment block.

  The filter provides additional command functions that take the form:

  \code{.C}
  \amu_<function> <variable> ( <arguments> )
  \endcode

  The \em variable must be the first word that follows the function name,
  but does not have to be on the same line if present. The \em arguments,
  including the opening and closing parenthesis can also be split across
  any number of lines. The \em arguments may be blank, however, the
  parenthesis are required.

  <b>Variabes:</b>

  The result of the function will be inserted into the output of the
  input file being filtered unless an optional results variable name
  follows the function name. When a variable name is provided, the
  command function's result is written to the specified variable and
  is not to copied the filters output.

  These variables can be referenced in the context of other function
  \em arguments of the filtered input file. A variable is referenced
  using the form: <tt>${VAR}</tt>. Variable expansions are recursive and
  expansion can be postponed using expansion escaping of the form:
  <tt>\\${VAR}</tt>.

  <b>Arguments:</b>

  Arguments are function dependent, however, there are four categories
  that may be used by any particular function. These are summarized
  in the following table:

  Type        | Example
  :-----------|:---------------:
  positional  | value
  named       | name=value
  flag        | ++flag /--flag
  incr/decr   | var++ / var--

  <b>Additional Doxygen Commands:</b>

  In addition to the normal Doxygen [special commands,]
  (http://www.stack.nl/~dimitri/doxygen/manual/commands.html) here is a
  list of additional filter-provided commands that can be used for
  document markup:

  | Internal Commands                                              | Description
  |:---------------------------------------------------------------|:------------------------
  | \ref ODIF::ODIF_Scanner::def_init() "\\amu_define"             | define text macro
  | \ref ODIF::ODIF_Scanner::bif_eval() "\\amu_eval"               | evaluate arguments
  | \ref ODIF::ODIF_Scanner::bif_shell() "\\amu_shell"             | execute shell command
  | \ref ODIF::ODIF_Scanner::bif_combine() "\\amu_combine"         | combine words
  | \ref ODIF::ODIF_Scanner::bif_table() "\\amu_table"             | generate a text table
  | \ref ODIF::ODIF_Scanner::bif_image_table() "\\amu_image_table" | generate an image table
  | \ref ODIF::ODIF_Scanner::bif_viewer() "\\amu_viewer"           | file viewer
  | \ref ODIF::ODIF_Scanner::bif_make() "\\amu_make"               | make script interface
  | \ref ODIF::ODIF_Scanner::bif_copy() "\\amu_copy"               | copy references
  | \ref ODIF::ODIF_Scanner::bif_find() "\\amu_find"               | search for output files
  | \ref ODIF::ODIF_Scanner::bif_scope() "\\amu_scope"             | query scope data
  | \ref ODIF::ODIF_Scanner::bif_source() "\\amu_source"           | query source filename

  External commands also exists and can be found in the directory:
  <tt><install-prefix>/functions/</tt>. For now, see the command itself for its
  documentation. Here is a list:

  | External Commands                 | Description
  |:----------------------------------|:--------------------------------
  | \\amu_date                        | current date and/or time
  | \\amu_list                        | format arguments into a list

  \section openscad_markup_ex Example Markup

    \verbatim
    // This comment will not be passed to Doxygen.
    //! \ingroup the_group_name
    //! @{

    //! Parameterized vehicle design module.
    /***************************************************************//**
     * \param <string> name       Table name.
     * \param <string> translate  String of integers.
     *
     * \details
     *   For a complete list of color names see the World Wide Web
     *   consortium's
     ******************************************************************/
     module example() { }

    /***************************************************************//**
     * \page imt Image Table
     *
     * \amu_define  var1  ( table_of_all )
     * \amu_shell   var2  ( "seq -f 'Col (%g)' -s '^' 10" )
     * \amu_define  var3  ( Table of all Generated Images. )
     * \amu_shell   var4  ( "seq -f 'h%g' -s '^' 84" )
     * \amu_combine var5  ( joiner=" " separator="^" tokenizer=" " prefix="a"
     *                      "car truck van"
     *                      "with" "12 17" "wheels" "thats"
     *                      "blue, green," "viewed" "from"
     *                      "right. bottom. diagonal. left. front. back. top." )
     * \amu_make    var6  ( append=test extension=png )
     *
     * \htmlonly
     *   \amu_image_table ( type=html
     *                      id="${var1}" table_caption="${var3}"
     *                      columns="10" column_headings="${var2}"
     *                      cell_captions="${var4}"
     *                      cell_files="${var6}" image_width="92"
     *                      cell_titles="${var5}" )
     * \endhtmlonly
    ******************************************************************/

    //! @}
    \endverbatim

  \section openscad_dif_aef Adding Commands

    Of course, additional commands can be added to the filter by coding
    the function in C++ and incorporating it into the source (see the
    the command source in src/openscad_dif_scanner_bif.cpp as a starting
    point). However, the easiest way to add a custom command is to use an
    external function in the language of your choice.

    External functions can be found at:

    \code{.C}
    <install-prefix>/functions/amu_<function>
    \endcode

    When the filter encounters a command of the form <tt>amu_<function></tt>
    that it does not recognize, it checks this location and if it finds
    a match this external command is called to handle the command. See
    an example there as a starting point.
*******************************************************************************/


//
// eof
//
