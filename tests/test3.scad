/*******************************************************************************

  \file       test3.scad

*******************************************************************************/

/*******************************************************************************
Begin_Scope doc;

  Begin_OpenSCAD;
  max=5;

  echo (max=max);

  for (x = [1 : 1 : max]) {
    echo (str("BEGIN_RECORD ", x));
    for (y = [1 : 1 : 3]) {
      for (z = [1 : 1 : 3]) {
        echo ( str(x, " + ", y," + ", z, " = ", x+y+z) );
      }
    }
    echo ("END_RECORD");
  }
  End_OpenSCAD;

  Begin_MFScript;

    defines Name "mode" Define "max" Integers "10 20 30";

    variables
      set_Makefile "${__MAKE_FILE__}"  add_Depend "${__MAKE_FILE__}"
      set_Source "${__SCOPE_FILE__}"   set_Prefix "${__PREFIX__}"

      set_log_begin_record "BEGIN_RECORD"
      set_log_end_record   "END_RECORD"

      set_Ext "csg"
      set_Opts "--render"
      set_Opts_Combine "mode";

    script
      Begin_Makefile_New
        Include_Copy "${__TEST_INCLUDE_PATH__}/parallel_jobs.mk"
        Summary  Tables  Targets  Menu
      End_Makefile;

  End_MFScript;

End_Scope;
*******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
// eof
////////////////////////////////////////////////////////////////////////////////


