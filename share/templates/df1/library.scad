//
// (doxygen) file header.
//

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

    \amu_make  stl_files ( append=scope1 extension=stl )
    \amu_shell stl_words ( "echo ${stl_files} | sed 's/ /^/g'" )

    Here is a list of STL files that have been generated by the build script
    embedded in \em scope1: \amu_list ( ${stl_files} )

    Here are the same list items in a table:
    \amu_table
      (
        columns=2
        cell_captions="A^B^C^D"
        cell_texts="${stl_words}"
        cell_urls="${stl_files}"
      )

    \amu_make png_files ( append=scope2 extension=png )
    \amu_shell file_cnt ( "echo ${png_files} | wc -w" )
    \amu_shell   labels ( "seq -f '(%g)' -s '^' ${file_cnt}" )

    \htmlonly
      \amu_image_table
        (
          type=html columns=6 image_width="160"
          table_caption="Library Model Examples"
          cell_files="${png_files}"
          cell_captions="${labels}"
        )
    \endhtmlonly

    \amu_make eps_files ( append=scope2 extension=png2eps )
    \latexonly
      \amu_image_table
        (
          type=latex columns=6 image_width="1.00in"
          table_caption="Library Model Examples"
          cell_files="${eps_files}"
          cell_captions="${labels}"
        )
    \endlatexonly

*******************************************************************************/


//
// (openscad) functions and modules.
//

//! library module one short description.
/***************************************************************************//**
  \param p1   <string>  parameter one.
  \param p2   <integer> parameter two.

  \details
    library module one long description.

  \todo complete library.
  \private
*******************************************************************************/
module lm1( p1="X", p2=10 ) {
  linear_extrude(height=p2*10)
  text(text=p1, size=p2*50);
}


//
// (openscad) globals.
//

/*
  in general, libraries should avoid (or a least minimize) the use of globals.
  use functions and parameters to facilitate reuse.
*/


//
// (openscad-amu) auxiliary build and modeling scripts, two scopes.
//

/***************************************************************************//**
Begin_Scope scope1;
  Begin_OpenSCAD;
    use <library.scad>;

    aux_script_str="A";
    aux_script_int=10;

    lm1(p1=aux_script_str, p2=aux_script_int);
  End_OpenSCAD;

  Begin_MFScript;
    defines Name "str" Define "aux_script_str" Strings "A B C D";
    defines Name "int" Define "aux_script_int" Integers "2";

    variables
      set_Makefile "${__MAKE_FILE__}"  add_Depend "${__MAKE_FILE__}"
      set_Source "${__SCOPE_FILE__}"   set_Prefix "${__PREFIX__}"

      Set_Ext "stl"
      Clear_Convert
      Set_Opts "--render"
      Set_Opts_Combine "str int";

    script
      Begin_Makefile_New
        Include_Copy "${__INCLUDE_PATH__}/parallel_jobs.mk"
        Summary  Tables  Targets  Menu
      End_Makefile;
  End_MFScript;
End_Scope;

Begin_Scope scope2;
  Begin_OpenSCAD;
    use <library.scad>;

    aux_script_str="A";
    aux_script_int=10;

    lm1(p1=aux_script_str, p2=aux_script_int);
  End_OpenSCAD;

  Begin_MFScript;
    defines Name "str" Define "aux_script_str" Strings "A B C D";
    defines Name "int" Define "aux_script_int" Integers "2";
    views   Name "views" Distance "100" Views "top bottom diag";
    images  Name "sizes" Aspect "4:3" Wsizes "320";

    variables
      set_Makefile "${__MAKE_FILE__}"  add_Depend "${__MAKE_FILE__}"
      set_Source "${__SCOPE_FILE__}"   set_Prefix "${__PREFIX__}"

      set_Ext "png"
      set_Convert_Exts "eps"  set_Convert_Opts "-verbose"
      set_Opts "--preview --projection=o --viewall --autocenter"
      set_Opts_Combine "str int views sizes";

    script
      Begin_Makefile_New
        Include_Copy "${__INCLUDE_PATH__}/parallel_jobs.mk"
        Summary  Tables  Targets  Menu
      End_Makefile;
  End_MFScript;
End_Scope;
*******************************************************************************/


////////////////////////////////////////////////////////////////////////////////
// end of file
////////////////////////////////////////////////////////////////////////////////
