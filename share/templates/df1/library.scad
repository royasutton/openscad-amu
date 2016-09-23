/***************************************************************************//**
  \file   library.scad
  \author Designers' Name
  \date   2016

  \copyright
    Copyright notice.

  \brief
    Short library description.

  \details
    Long library description.
*******************************************************************************/


//
// documented functions and modules
//

//! library module 1 short description.
/***************************************************************************//**
  \param p1   <string>  parameter two.
  \param p2   <integer> parameter one.

  \details
    library module 1 long description.

  \todo complete library.
  \private
*******************************************************************************/
module lm1( p1="X", p2=10 ) {
  text(text=p1, size=p2);
}


//
// globals
//


//
// openscad-amu auxiliary scripts
//

/***************************************************************************//**
Begin_Scope scope1;
  Begin_OpenSCAD;
    aux_script_str="A";
    aux_script_int=10;

    lm1(p1=aux_script_str, p2=aux_script_int);
  End_OpenSCAD;

  Begin_MFScript;
    views   Name "views" Distance "100" Views "all";
    images  Name "sizes" Aspect "4:3" Wsizes "320";
    defines Name "str" Define "aux_script_str" Strings "A B";
    defines Name "int" Define "aux_script_int" Integers "1 2";

    variables
      set_Makefile "${__MAKE_FILE__}"  add_Depend "${__MAKE_FILE__}"
      set_Source "${__SCOPE_FILE__}"   set_Prefix "${__PREFIX__}"

      Set_Ext "stl"
      Clear_Convert
      Set_Opts "--render"
      Set_Opts_Combine "str int";

    script
      Begin_Makefile_New
        Include_Copy "${__LIBPATH__}/include/mf/parallel_jobs.mk"
        Summary  Tables  Targets  Menu
      End_Makefile;
  End_MFScript;
End_Scope;

Begin_Scope scope2;
  Begin_OpenSCAD;
    aux_script_str="A";
    aux_script_int=10;

    lm1(p1=aux_script_str, p2=aux_script_int);
  End_OpenSCAD;

  Begin_MFScript;
    views   Name "views" Distance "100" Views "all";
    images  Name "sizes" Aspect "4:3" Wsizes "320";
    defines Name "str" Define "aux_script_str" Strings "A B";
    defines Name "int" Define "aux_script_int" Integers "1 2";

    variables
      set_Makefile "${__MAKE_FILE__}"  add_Depend "${__MAKE_FILE__}"
      set_Source "${__SCOPE_FILE__}"   set_Prefix "${__PREFIX__}"

      set_Ext "png"
      set_Convert_Exts "eps"  set_Convert_Opts "-verbose"
      set_Opts "--preview --projection=o --viewall"
      set_Opts_Combine "str int views sizes";

    script
      Begin_Makefile_New
        Include_Copy "${__LIBPATH__}/include/mf/parallel_jobs.mk"
        Summary  Tables  Targets  Menu
      End_Makefile;
  End_MFScript;
End_Scope;
*******************************************************************************/


////////////////////////////////////////////////////////////////////////////////
// end of file
////////////////////////////////////////////////////////////////////////////////
