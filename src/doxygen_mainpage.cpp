/***************************************************************************//**

  \mainpage C++ Programs Documentation

  \dot
    digraph example {
      node [shape=Mrecord, fontname=Helvetica, fontsize=10];
      a [label="openscad-amu" URL="../../../share/lib/doxygen/html/index.html"];
      b [label="C++ Programs" URL="index.html" style="filled"];
      c [label="Makefile Script Library" URL="../../../share/lib/doxygen/html/index.html#library"];
      d [label="Support Scripts" URL="../../../share/scripts/doxygen/html/index.html"];
      a->b; a->c; a->d;
    }
  \enddot

  Here you will find documentation for the C++ programs that are part of
  the OpenSCAD Automake Utilities.

  \section programs Programs

  \subsection bash_dif bash-dif
    \subpage bash_dif_dtc

    \ref bash_dif_src "Program Source Documentation"

    \subsubsection bash_dif_help Help Text:
      \include bash-dif.help

  \subsection openscad_dif openscad-dif
    \subpage openscad_dif_dtc

    \ref openscad_dif_src "Program Source Documentation"

    \subsubsection openscad_dif_help Help Text:
      \include openscad-dif.help

  \subsection openscad_seam openscad-seam
    \ref openscad_seam_src "Source Documentation"

    \subsubsection openscad_seam_help Help Text:
      \include openscad-seam.help

  \todo create a test suite that compares output from each program to
        known good results.

*******************************************************************************/


/***************************************************************************//**
  \defgroup bash_dif_src Source Documentation: bash-dif

  \brief
    Doxygen input filter for Bourne Again SHell (Bash) scripts.
*******************************************************************************/


/***************************************************************************//**
  \defgroup openscad_dif_src Source Documentation: openscad-dif

  \brief
    Doxygen input filter for OpenSCAD scripts.
*******************************************************************************/


/***************************************************************************//**
  \defgroup openscad_seam_src Source Documentation: openscad-seam

  \brief
    OpenSCAD and Makefile script extractor and make utility.
*******************************************************************************/


/***************************************************************************//**
  \page bash_dif_dtc Documenting bash scripts

  This filter may be used to document bash scripts using Doygen.

  Comment patters are identified according to the rules in the
  following table. See the flex documentation on [patterns]
  (http://flex.sourceforge.net/manual/Patterns.html)
  for more information on how to interpret the patterns.

   patterns   | behavior
  :----------:|:-----------------------------------------
   "#"        | comment. ignored by filter.
   "##"[#]*   | documentation pass to Doxygen.
   "#/"[#!/]+ | open a comment block.
   "#"+"/"    | close comment block.
   "#"[#]*    | comment line while in open comment block.

  Documenting the code:
  \verbatim
  # This comment will not be passed to Doxygen.
  ## \ingroup the_group_name
  ## @{

  ## Create a camera view option set table from predefined names.
  #/##################################################################
  # \afn function
  #
  # \aparami <required named string> name       Set table name.
  # \aparami <named string>          translate  String of integers.
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
  \page openscad_dif_dtc Documenting OpenSCAD Scripts

    This filter may be used to document OpenSCAD scripts using Doygen.

    Comment patters are identified according to the rules in the
    following table. See the flex documentation on [patterns]
    (http://flex.sourceforge.net/manual/Patterns.html)
    for more information on how to interpret the patterns.


     patterns   | behavior
    :----------:|:--------------------------------
     "//"       | comment. ignored by filter.
     "//"[/!]?  | documentation pass to Doxygen.
     "/*"[*!]?  | open a comment block.
     "*"+"/"    | close comment block.

    Documenting the code:
    \verbatim
    // This comment will not be passed to Doxygen.
    //! \ingroup the_group_name
    //! @{

    //! Versatile all-inclusive computer aided vehicle design.
    /***************************************************************//**
     * \param <string> name       Table name.
     * \param <string> translate  String of integers.
     *
     * \details
     *   For a complete list of color names see the World Wide Web
     *   consortium's
     ******************************************************************/
     module example()

    //! @}
    \endverbatim
*******************************************************************************/


//
// eof
//
