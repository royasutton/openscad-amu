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
*******************************************************************************/


//
// Doxygen main page documentation (optional)
//

/***************************************************************************//**
  \mainpage Design Main Page
  \tableofcontents

  \section overview Overview
    Overview.

  \subsection subsection Subsection.

  \note design.scad contains global members, but library.scad does not.

  \warning global members should ordinarily be avoided in libraries. using
           modules and/or functions with parameters facilitates reuse.

  \todo remaining documentation.
*******************************************************************************/


//
// documented functions and modules
//

//! design module 1 short description.
/***************************************************************************//**
  \param p1   <string>  parameter two.
  \param p2   <integer> parameter one.

  \details
    design module 1 long description.

  \todo complete design.
  \protected
*******************************************************************************/
module dm1( p1="X", p2=10 ) {
  text(text=p1, size=p2);
}


//
// globals
//

design_str="A";
design_int=10;

dm1(p1=design_str, p2=design_int);


//
// openscad-amu auxiliary scripts
//

/***************************************************************************//**
Begin_Scope scope1;
  Begin_MFScript;
    views   Name "views" Distance "100" Views "all";
    images  Name "sizes" Aspect "4:3" Wsizes "320";
    defines Name "str" Define "design_str" Strings "A B";
    defines Name "int" Define "design_int" Integers "1 2";

    variables
      set_Makefile "${__MAKE_FILE__}"  add_Depend "${__MAKE_FILE__}"
      set_Source "${__SOURCE_FILE__}"  set_Prefix "${__PREFIX__}"

      set_Ext "png"
      set_Convert_Exts "eps"  set_Convert_Opts "-verbose"
      set_Opts "--preview --projection=o --viewall"
      set_Opts_Combine "str int views sizes";

    script
      Begin_Makefile_New
        Include_Copy "${__LIB_PATH__}/include/mf/parallel_jobs.mk"
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
