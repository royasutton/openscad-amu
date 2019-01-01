/***************************************************************************//**

  \file   openscad_dif_scanner_bif5.cpp

  \author Roy Allen Sutton
  \date   2018

  \copyright

    This file is part of OpenSCAD AutoMake Utilities ([openscad-amu]
    (https://royasutton.github.io/openscad-amu)).

    openscad-amu is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    openscad-amu is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    [GNU General Public License] (https://www.gnu.org/licenses/gpl.html)
    for more details.

    You should have received a copy of the GNU General Public License
    along with openscad-amu.  If not, see <http://www.gnu.org/licenses/>.

  \brief
    OpenSCAD Doxygen input filter scanner class built-in functions source.

  \ingroup openscad_dif_src
*******************************************************************************/

#include "openscad_dif_scanner.hpp"

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

using namespace std;


/***************************************************************************//**
  \details

    ### Single argument tests ###

    symbol  | test description
    :------:|:--------------------------------
     -n     | text length is nonzero
     -z     | text length is zero
     -d     | variable has been defined
     -l     | locate file in include paths

    \b Example:
    \code
      \amu_if ( ! -d ${var2} ) { true } else { false } endif
    \endcode

*******************************************************************************/
bool
ODIF::ODIF_Scanner::bif_if_exp_1a(string s)
{
  bool result = false;

  // lexer sent: 'operation argument'
  string op = UTIL::get_word(s, 1);
  string ag = UTIL::get_word(s, 2);

  // unquote and expand argument
  ag = levm.expand_text( UTIL::unquote(ag) );

  if ( op == "-n" )
    result = ( ag.size() > 0 );

  if ( op == "-z" )
    result = ( ag.size() == 0 );

  if ( op == "-d" )
    result = ( ag.compare( levm.get_report_message() ) != 0 );

  if ( op == "-l" )
    file_rl( ag, NO_FORMAT_OUTPUT, result );

  return( result );
}

/***************************************************************************//**
  \details

    ### Double argument tests ###

    symbol  | test description
    :------:|:----------------------------------------------
      ==    | arguments are equal
      !=    | arguments not equal
      <     | argument1 less than argument2
      \>    | argument1 greater than argument2
      <=    | argument1 less than or equal to argument2
      \>=   | argument1 greater than or equal to argument2

    \b Example:
    \code
      \amu_if ( ${var1} != ${var2} ) { true } else { false } endif
    \endcode

    When both argument1 and argument2 can be converted to into
    integers, the comparision is perfomred numerically, otherwise a
    string comparision is performed.

    Care must be taken when comparing variables that have not been
    previously defined. In such cases, the undefined variable will
    expand to the undefined warning message string and be compared
    using string comparison. This can lead to unexpected results. In
    such cases it best to guard the comparision as in the following
    example.

    \code
      \amu_if ( -d ${x} && (${x} > 1) ) { true } else { false } endif
    \endcode

*******************************************************************************/
bool
ODIF::ODIF_Scanner::bif_if_exp_2a(string s)
{
  bool result = false;

  // lexer sent: 'argument1 operation argument2'
  string a1 = UTIL::get_word(s, 1);
  string op = UTIL::get_word(s, 2);
  string a2 = UTIL::get_word(s, 3);

  // unquote and expand arguments
  a1 = levm.expand_text( UTIL::unquote(a1) );
  a2 = levm.expand_text( UTIL::unquote(a2) );

  // use numerical comparision
  if ( UTIL::is_number(a1) && UTIL::is_number(a2) )
  {
    int n1 = atoi( a1.c_str() );
    int n2 = atoi( a2.c_str() );

    if ( op == "==" )
      result = ( n1 == n2 );

    if ( op == "!=" )
      result = ( n1 != n2 );

    if ( op == "<" )
      result = ( n1 < n2 );

    if ( op == ">" )
      result = ( n1 > n2 );

    if ( op == "<=" )
      result = ( n1 <= n2 );

    if ( op == ">=" )
      result = ( n1 >= n2 );
  }
  // use string comparision
  else
  {
    if ( op == "==" )
      result = ( a1.compare( a2 ) == 0 );

    if ( op == "!=" )
      result = ( a1.compare( a2 ) != 0 );

    if ( op == "<" )
      result = ( a1.compare( a2 ) < 0 );

    if ( op == ">" )
      result = ( a1.compare( a2 ) > 0 );

    if ( op == "<=" )
      result = ( a1.compare( a2 ) < 0 ) || ( a1.compare( a2 ) == 0 );

    if ( op == ">=" )
      result = ( a1.compare( a2 ) > 0 ) || ( a1.compare( a2 ) == 0 );
  }

  return( result );
}


/*******************************************************************************
// eof
*******************************************************************************/
