/******************************************************************************/
/**
  \file   openscad_dif_lexer.ll

  \author Roy Allen Sutton
  \date   2016

  \copyright

    This file is part of OpenSCAD AutoMake Utilities ([openscad-amu]
    (https://github.com/royasutton/openscad-amu)).

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
    This is the brief.

  \details

*******************************************************************************/

%{

#include <string>
#include <iostream>
#include <fstream>

#include "config.h"

using namespace std;

%}

%option c++
%option prefix="openscad_dif"
%option stack
%option noyywrap
%option nounput
/* %option nodefault */
%option debug

%s COMMENT

comment_line                      "//"[/!]?
comment_open                      "/*"[*!]?
comment_close                     "*"+"/"

%%

  /* outside comments: replace OpenSCAD commands with C equivalents,
                       output everything unchanged */

<INITIAL>{comment_open}           { ECHO; yy_push_state(COMMENT); }
<INITIAL>{comment_line}           { ECHO; }
<INITIAL>include                  { LexerOutput( "#include", 8 ); }
<INITIAL>use                      { LexerOutput( "#include", 8 ); }
<INITIAL>.                        { ECHO; }

  /*  inside comments: output everything unchanged */

<COMMENT>{comment_close}          { ECHO; yy_pop_state(); }
<COMMENT><<EOF>>                  { LexerError("ERROR: unterminated comment."); }
<COMMENT>.                        { ECHO; }

%%

int main( int argc, char** argv ) {
  string  arg1;
  bool    help    = false;
  bool    version = false;

  if ( argc == 2 ) {
    arg1 = argv[1];
    help    = !arg1.compare("-h") || !arg1.compare("--help");
    version = !arg1.compare("-v") || !arg1.compare("--version");
  }

  if ( help || argc != 2 ) {
    cout << argv[0] << " v" << PACKAGE_VERSION << endl
         << endl
         << "Doxygen input filter for OpenSCAD source files. Can be used in\n"
            "conjunction with doxygen tags INPUT_FILTER and FILTER_*." << endl
         << endl
         << "Example:" << endl
         << "  FILTER_PATTERNS = *.scad=<prefix>/bin/" << argv[0] << endl
         << endl
         << "Usage: " << endl
         << "  " << argv[0] << " ifile" << endl
         << endl
         << "Options: " << endl
         << "  -h [ --help]     Print this message." << endl
         << "  -v [ --version]  Report tool version." << endl
         << endl;

    if ( help ) exit( EXIT_SUCCESS );
    else        exit( EXIT_FAILURE );
  }

  if ( version ) {
    cout << PACKAGE_VERSION << endl;
    exit( EXIT_SUCCESS );
  }

  ifstream infile ( arg1.c_str() );

  if ( infile.good() ) {
    openscad_difFlexLexer* lexer = new openscad_difFlexLexer( &infile , &cout);

    while( lexer->yylex() != 0 )
      ;

  } else {

    cerr << "ERROR: unable to open file [" << arg1 << "]" << endl;
    exit( EXIT_FAILURE );

  }

  infile.close();
  exit( EXIT_SUCCESS );
}


/*******************************************************************************
// eof
*******************************************************************************/
