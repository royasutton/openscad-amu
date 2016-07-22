/*******************************************************************************

  \file       test4.scad

*******************************************************************************/

include <file1.scad>;
use <file2.scad>;
use <file3.scad>;


//! (decimal) Epsilon, small distance to deal with overlapping shapes.
ep = 0.02;


//! Return the table row for a given index.
/*******************************************************************************
  \param    feild name (string).
  \returns  (vector) Table row.

  \note data not available for {b, w, and d1} for bearings lm{3|4}uu.
        Their values were extrapolated.

  \amu_define file1 (file1 file2 file3)
  \amu_define files (im1 im2 im3 im4)

  \amu_define newf ( type=html sizex=320 sizey=240 cols=3
                     files="${files}"
                     titles="title1 title2 title3" )

  \amu_func1             (args)
  \amu_func2 |var2       (args x=7 --roy=yes)
  \amu_func3 |var3       (a1 a2 a3 a 4)
  \amu_func4 |var4 func6 (a1 a2 a3 "a 4")
  \amu_func5 |var5 func7 (a1 a2 a3 "a 4")

  @amu_eval                     ( \${files} )

  \amu_eval  | files            ( '_op1 _op1' "_m1 _m2" "_x1, _x2")
  \amu_shell | files /path/func ( "base" "_op1 _op1" "_m1 _m2" "_x1, _x2" )
  \amu_shell                 ls ( "base" "_op1 _op1" "_m1 _m2" "_x1, _x2" )
  \amu_enum  | files            ( --base='base' "_op1 _op1" "_m1 _m2" "_x1, _x2" )

  \amu_image_table html ( sizex=320 sizey=240 cols=3
                          files="${files}"
                          titles="${title1} ${title2} \${title3}" )

  \amu_html_viewer png ( type=png )

  here are the files: \amu_eval (${files})

  \public
*******************************************************************************/
function bearing_lbb_tabler(size) =
  size == "lm3uu"   ? [  3,   7,  10,   7.3, 0.90,   6.7] :
  size == "lm4uu"   ? [  4,   8,  12,   8.8, 0.90,   7.6] :
  size == "lm5uu"   ? [  5,  10,  15,  10.2, 1.10,   9.6] :
-1;


//! Return the record feild value for a given index and feild name.
/*******************************************************************************
  \param    table row (string).
  \param    feild name (string).
  \returns  (vector) feild value.

  \public
*******************************************************************************/
function bearing_lbb_table(row, feild) =
  bearing_lbb_tabler(row)[bearing_lbb_tablef(feild)];


////////////////////////////////////////////////////////////////////////////////
// eof
////////////////////////////////////////////////////////////////////////////////
