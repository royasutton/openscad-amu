//
// (doxygen) file header.
//

/***************************************************************************//**
  \file   design.scad
  \author Designers' Name
  \date   2016

  \copyright
    Copyright notice.

  \brief
    Short design description.

  \details
    Long design description.

    \amu_make png_files ( append=scope1 extension=png ++pstarget )
    \htmlonly
      \amu_image_table
        (
          type=html
          table_heading="Design Examples"
          columns=6
          image_files="${png_files}"
          image_width="160"
        )
    \endhtmlonly

    \amu_make eps_files ( append=scope1 extension=png2eps ++pstarget )
    \latexonly
      \amu_image_table
        (
          type=latex
          table_heading="Design Examples"
          columns=6
          image_files="${eps_files}"
          image_width="1.00in"
        )
    \endlatexonly

*******************************************************************************/


//
// (doxygen) main page.
//

/***************************************************************************//**
  \mainpage Main Page
  \tableofcontents

  \section overview Overview
    Overview.

  \subsection design Design

    \ref design.scad "Documentation."

    \amu_combine    titles ( prefix="v" "10 11 12" "x1 x2" joiner="" separator="^" )
    \amu_make    stl_files ( append=scope1 extension=stl ++pstarget )

    \amu_combine png_files ( prefix="design" "v10 v11 v12" "1 2" "diag_320x240.png" )
    \htmlonly
      \amu_image_table
        (
          type=html
          table_heading="Downloadable STL"
          columns=2
          column_headings="scale 1^scale 2"
          image_width="160"
          image_files="${png_files}"
          image_titles="${titles}"
          image_headings="${titles}"
          image_urls="${stl_files}"
        )
    \endhtmlonly

    \amu_combine eps_files ( prefix="design" "v10 v11 v12" "1 2" "diag_320x240.eps" )
    \latexonly
      \amu_image_table
        (
          type=latex
          table_heading="Downloadable STL"
          columns=2
          column_headings="scale 1^scale 2"
          image_width="1.00in"
          image_files="${eps_files}"
          image_titles="${titles}"
          image_headings="${titles}"
          image_urls="${stl_files}"
        )
    \endlatexonly

  \subsection library Library

    \ref library.scad "Documentation."

    \amu_shell        seq4 ( "seq -f '(%g)' -s '^' 4" )
    \amu_combine    titles ( prefix="example" "A B C D" suffix="." joiner=" " separator="^" )
    \amu_make    stl_files ( set="library" append=scope1 extension=stl )

    \amu_combine png_files ( prefix="library_scope2" "A B C D" "2" "diag_320x240.png" )
    \htmlonly
      \amu_image_table
        (
          type=html
          table_heading="Downloadable STL"
          columns=4
          column_headings="${seq4}"
          image_width="160"
          image_files="${png_files}"
          image_titles="${titles}"
          image_headings="${titles}"
          image_urls="${stl_files}"
        )
    \endhtmlonly

    \amu_combine eps_files ( prefix="library_scope2" "A B C D" "2" "diag_320x240.eps" )
    \latexonly
      \amu_image_table
        (
          type=latex
          table_heading="Downloadable STL"
          columns=4
          column_headings="${seq4}"
          image_width="1.00in"
          image_files="${eps_files}"
          image_titles="${titles}"
          image_headings="${titles}"
          image_urls="${stl_files}"
        )
    \endlatexonly

  \section section Section

    \amu_date compiled ( Day dash Month dash year
                         space string at space
                         hour colon minute colon second )

    Compiled: \amu_eval ( ${compiled} ).

  \note design.scad contains global members, but library.scad does not.

  \warning global members should ordinarily be avoided in libraries. using
           modules and/or functions with parameters facilitates reuse.

  \todo remaining documentation.
*******************************************************************************/


//
// (doxygen) examples.
//

/***************************************************************************//**
  \example design_scope1.bash extracted auxiliary build script.
  \example design_scope1.makefile generated scope makefile.

  \example library_scope1.bash extracted auxiliary build script.
  \example library_scope1.scad extracted auxiliary modeling script.
  \example library_scope1.makefile generated scope makefile.

  \example library_scope2.bash extracted auxiliary build script.
  \example library_scope2.scad extracted auxiliary modeling script.
  \example library_scope2.makefile generated scope makefile.
*******************************************************************************/


//
// (openscad) functions and modules.
//

//! design module one short description.
/***************************************************************************//**
  \param p1   <string>  parameter one.
  \param p2   <integer> parameter two.

  \details
    design module one long description.

  \todo complete design.
  \protected
*******************************************************************************/
module dm1( p1="X", p2=10 ) {
  linear_extrude(height=p2*10)
  text(text=p1, size=p2*50);
}


//
// (openscad) globals.
//

design_str="A";
design_int=10;

dm1(p1=design_str, p2=design_int);


//
// (openscad-amu) auxiliary build script, single scope.
//

/***************************************************************************//**
Begin_Scope scope1;
  Begin_MFScript;
    views   Name "views" Distance "100" Views "top bottom diag";
    images  Name "sizes" Aspect "4:3" Wsizes "320";
    defines Name "str" Define "design_str" Strings "v10 v11 v12";
    defines Name "int" Define "design_int" Integers "1 2";

    variables
      set_Makefile "${__MAKE_FILE__}"  add_Depend "${__MAKE_FILE__}"
      set_Source "${__SOURCE_FILE__}"  set_Prefix "${__PREFIX__}"

      set_Ext "png"
      set_Convert_Exts "eps"  set_Convert_Opts "-verbose"
      set_Opts "--preview --projection=o --viewall --autocenter"
      set_Opts_Combine "str int views sizes";

    script
      Begin_Makefile_New
        Include_Copy "${__INCLUDE_PATH__}/parallel_jobs.mk"
        Summary  Tables  Targets  Menu_Ext
      End_Makefile;

    variables
      Set_Ext "stl"
      Clear_Convert
      Set_Opts "--render"
      Set_Opts_Combine "str int";

    script
      Begin_Makefile_Append
        Summary  Tables  Targets  Menu_Ext  Menu_Src  Menu_All
      End_Makefile;
  End_MFScript;
End_Scope;
*******************************************************************************/


////////////////////////////////////////////////////////////////////////////////
// end of file
////////////////////////////////////////////////////////////////////////////////
