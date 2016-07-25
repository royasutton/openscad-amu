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

  \amu_define files1 ( file1
                       file2
                       file3 )
  \amu_define files2 ( im1 im2 im3 im4 ${files3} )

  \amu_define textf  ( type=html sizex=320 sizey=240 cols=3
                      files="${files1} ${files2}"
                      titles="title1 title2 title3" )

  @amu_eval          ( -x=2 -y=3 z="s" \${files} )

  \amu_eval      files3   ( '_op1 _op2' "_m1 _m2" "_x1, _x2" )
  \amu_shell     files4   ( "echo ${files1} 's d d'" )
  \amu_shell     date     ( "date" )
  \amu_combine   files5   (   'base1' "_op1 _op2" "_m1 _m2" "_x1 _x2" ".jpg" --separator=" " )
  \amu_combine   files6   ( -p='base1' -s=".jpg" "_op1 _op2" "_m1 _m2" "_x1 _x2" -f=" " )

  \amu_image_table ( type=html sizex=320 sizey=240 cols=3
                     files="${files}"
                     titles="${title1} ${title2} \${title3}" )

  \amu_html_viewer ( type="png, svg, stl" )

  here are the files: \amu_eval ( f1=2
                                  f2="${files5}"
                                  a b c \${f1} \${f2}
                                )
  \amu_define a1 ( "a1" )
  \amu_define a2 ( "a2" and ${a1}} )
  \amu_define a3 ( ${a1} "${a1}" ${a2} "${a2}" )
  \amu_define a4 ( ${a3} '${a3}' \${a-3} \${_a3} )

  \amu_eval   a5 ( a4 \= \[${a4}\] )
  \amu_eval      ( a4 \= \[${a4}\] now ${a5} "\"\${xx}\'")

  \amu_eval (files1 \= \[${files1}\])
  \amu_eval (files2 \= \[${files2}\])
  \amu_eval (files3 \= \[${files3}\])
  \amu_eval (files4 \= \[${files4}\])
  \amu_eval (files5 \= \[${files5}\])
  \amu_eval (files6 \= \[${files6}\])
  \amu_eval (textf  \= \[${textf}\])
  \amu_eval (date   \= \[${date}\])

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
