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

  @amu_eval          ( y-- --y x=--x h=y++ ++xyz qq-- g++ z="s"
                       \${files} \${x} \${h} \${y}  ${y} )

  \amu_eval      files3   ( '_op1 _op2' "_m1 _m2" "_x1, _x2" )
  \amu_shell     files4   ( "echo ${files1} 's d d'" )
  \amu_shell     date     ( "date" ++stderr ++e )
  \amu_shell              ( "ls /non-existent-directory " ++s )
  \amu_combine   files5   ( p='base1' s=".jpg" "op1 op2" "m1 m2" "x1 x2" separator=" " joiner='_' )
  \amu_combine   files6   ( p='base1' s=".jpg" "op1 op2" "m1 m2" "x1 x2" f=" ")

  \amu_file_viewer ( type="png, svg, stl" )

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

  \amu_image_table (
    type=html
    table_caption="Know is the time."
    columns=3
    column_headings="
      one^  two  ^
      three
    "
    image_width=200
    image_height=100
    cell_files="${files5}"
    cell_titles=
      "     a title 1^     a title 2^a title 3^a title 4^
       title 5~title 6~title 7~title 8"
    cell_urls="
      http://www.google.com
      http://www.google.com
      http://www.google.com
      http://www.google.com
      http://www.google.com
      http://www.google.com
      http://www.google.com
      http://www.google.com
    "
  )

  \amu_image_table (
    type=latex
    id=my_latex_table
    table_caption="Know is the time."
    columns=4
    column_headings="
      one^  two  ^
      three^four
    "
    image_width=2in
    image_height=1in
    cell_files="${files5}"
    cell_captions="
      heading 1^
      heading 2^
      heading 3^
      heading 4^
      heading 5^
      heading 6^
      heading 7^
      heading 8
    "
    cell_titles=
      "     a title 1^     a title 2^a title 3^a title 4^
       title 5~title 6~title 7~title 8"
    cell_urls="
      http://www.google.com
      http://www.google.com
      http://www.google.com
      http://www.google.com
      http://www.google.com
      http://www.google.com
      http://www.google.com
      http://www.google.com
    "
  )

  \amu_table (
    id=my_text_table
    table_caption="Know is the time to return to text."
    columns=4
    column_headings="
      one^  two  ^
      three^four
    "
    cell_texts="
      one^
      two^
      three^
      four^
      five^
      six^
      seven^
      eight
    "
    cell_captions="
      heading 1^
      heading 2^
      heading 3^
      heading 4^
      heading 5^
      heading 6^
      heading 7^
      heading 8
    "
    cell_urls="
      http://www.google.com
      http://www.google.com
      http://www.google.com
      http://www.google.com
      http://www.google.com
      http://www.google.com
      http://www.google.com
      http://www.google.com
    "
  )

  \amu_date ( d l a p h c i l s )

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
