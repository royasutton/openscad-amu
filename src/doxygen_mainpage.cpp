/**

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

  \subsection bash-dif bash-dif
    \include bash-dif.help

  \subsection openscad-dif openscad-dif
    \include openscad-dif.help

  \subsection openscad-seam openscad-seam
    \include openscad-seam.help

  \todo complete base documentation for the C++ programs.
  \todo filter the openscad-seam run summary info based on the run mode.

*/

/**
  \defgroup src_bash_dif Source: bash-dif

  \brief
    Doxygen input filter for Bourne Again SHell (Bash) scripts.

  \details
    This filter may be used to document bash scripts using Doygen.

    Comments are interpreted as follows:

    string | behavior
    :-----:|:--------------------------------
     #     | bash comment. ignored by filter.
     ##    | documentation pass to Doxygen.
     #/#   | open a comment block.
     #/    | close comment block.

    Example use:
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
*/

/**
  \defgroup src_openscad_dif Source: openscad-dif

  \brief
    Doxygen input filter for OpenSCAD scripts.

  \details
    This filter may be used to document OpenSCAD scripts using Doygen.

    Example use:
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
*/

/**
  \defgroup src_openscad_seam Source: openscad-seam

  \brief
    OpenSCAD and Makefile script extractor and make utility.

  \details

*/

//
// eof
//
