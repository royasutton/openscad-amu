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
    \amu_shell file_cnt ( "echo ${png_files} | wc -w" )
    \amu_shell   labels ( "seq -f '(%g)' -s '^' ${file_cnt}" )

    \htmlonly
      \amu_image_table
        (
          type=html columns=6 image_width="160"
          table_caption="Design Examples"
          cell_files="${png_files}"
          cell_captions="${labels}"
        )
    \endhtmlonly

    \amu_make eps_files ( append=scope1 extension=png2eps ++pstarget )
    \latexonly
      \amu_image_table
        (
          type=latex columns=6 image_width="1.00in"
          table_caption="Design Examples"
          cell_files="${eps_files}"
          cell_captions="${labels}"
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

    \ref design.scad "Design Documentation."

    \amu_combine    titles ( "v10 v11 v12" "x" "1 2" joiner=" " separator="^" )
    \amu_make    stl_files ( append=scope1 extension=stl ++pstarget )

    \amu_combine png_files ( prefix="design" "v10 v11 v12" "1 2" "diag_320x240.png" )
    \htmlonly
      \amu_image_table
        (
          type=html columns=2 image_width="160"
          table_caption="Downloadable STL"
          column_headings="Scale=1^Scale=2"
          cell_files="${png_files}"
          cell_titles="${titles}"
          cell_captions="${titles}"
          cell_urls="${stl_files}"
        )
    \endhtmlonly

    \amu_combine eps_files ( prefix="design" "v10 v11 v12" "1 2" "diag_320x240.eps" )
    \latexonly
      \amu_image_table
        (
          type=latex columns=2 image_width="1.00in"
          table_caption="Downloadable STL"
          column_headings="Scale=1^Scale=2"
          cell_files="${eps_files}"
          cell_titles="${titles}"
          cell_captions="${titles}"
          cell_urls="${stl_files}"
        )
    \endlatexonly

  \subsection library Library

    \ref library.scad "Library Documentation."

    \amu_shell        seq4 ( "seq -f '(%g)' -s '^' 4" )
    \amu_combine    titles ( prefix="Example" "A B C D" suffix="." joiner=" " separator="^" )
    \amu_make    stl_files ( set="library" append=scope1 extension=stl )

    \amu_combine png_files ( prefix="library_scope2" "A B C D" "2" "diag_320x240.png" )
    \htmlonly
      \amu_image_table
        (
          type=html columns=4 image_width="160"
          table_caption="Downloadable STL"
          column_headings="${seq4}"
          cell_files="${png_files}"
          cell_titles="${titles}"
          cell_captions="${titles}"
          cell_urls="${stl_files}"
        )
    \endhtmlonly

    \amu_combine eps_files ( prefix="library_scope2" "A B C D" "2" "diag_320x240.eps" )
    \latexonly
      \amu_image_table
        (
          type=latex columns=4 image_width="1.00in"
          table_caption="Downloadable STL"
          column_headings="${seq4}"
          cell_files="${eps_files}"
          cell_titles="${titles}"
          cell_captions="${titles}"
          cell_urls="${stl_files}"
        )
    \endlatexonly

  \section references References

    \amu_table
      (
        id="textid" columns="2"
        column_headings="Left^Right"
        cell_texts="
          OpenSCAD Home Page^
          openscad-amu on Github^
          Doxygen Home Page^
          GNU Make Documentation"
        cell_urls="
          http://www.openscad.org
          https://github.com/royasutton/openscad-amu
          http://www.stack.nl/~dimitri/doxygen
          https://www.gnu.org/software/make
        "
      )

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
  \example design_logo.bash extracted auxiliary build script.
  \example design_logo.makefile generated scope makefile.

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

design_str="A";   ///< A string with the text to render.
design_int=10;    ///< An integer scaling factor for the text.

dm1(p1=design_str, p2=design_int);


//
// (openscad-amu) auxiliary build script, two scopes.
//

/***************************************************************************//**
Begin_Scope logo;
  Begin_MFScript;
    defines Name "str" Define "design_str" Strings "logo";
    views   Name "views" Translate "650,180,50" Distance "1700" Views "top";
    images  Name "sizes" Aspect "4:1" Xsizes "200";

    variables
      set_Makefile "${__MAKE_FILE__}"  add_Depend "${__MAKE_FILE__}"
      set_Source "${__SOURCE_FILE__}"  set_Prefix "${__PREFIX__}"

      set_Ext "png"
      set_Convert_Exts "eps"  set_Convert_Opts "-verbose"
      set_Opts "--preview --projection=p"
      set_Opts_Combine "str views sizes";

    script Begin_Makefile Summary  Tables  Targets  Menu End_Makefile;
  End_MFScript;
End_Scope;

Begin_Scope scope1;
  Begin_MFScript;
    defines Name "str" Define "design_str" Strings "v10 v11 v12";
    defines Name "int" Define "design_int" Integers "1 2";
    views   Name "views" Distance "100" Views "top bottom diag";
    images  Name "sizes" Aspect "4:3" Wsizes "320";

    variables
      set_Makefile "${__MAKE_FILE__}"  add_Depend "${__MAKE_FILE__}"
      set_Source "${__SOURCE_FILE__}"  set_Prefix "${__PREFIX__}"

      set_Ext "png"
      set_Convert_Exts "eps"  set_Convert_Opts "-verbose"
      set_Opts "--preview --projection=o --viewall --autocenter"
      set_Opts_Combine "str int views sizes";

    script
      Begin_Makefile_New
        Include_Copy "${__AMU_INCLUDE_PATH__}/parallel_jobs.mk"
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
