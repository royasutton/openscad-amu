/*******************************************************************************

  \file   bash_dif_lexer.ll

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

  \todo consider bash declared variable support
  \todo consider function and general script code support

*******************************************************************************/

%{

#include <fstream>
#include <string>
#include <queue>

#include "config.h"

using namespace std;

// comment text block class
class block
{
  public:
    string format( void );

    // comment block text
    void app_text( const string& t ) { text += t; }
    string get_text( void ) { return text; }

    bool empty ( void ) { return (text.length() == 0); }
    void clear( void ) { text.clear(); fn.clear(); }

    // function
    void set_fn( const string& t ) { fn = t; }
    string get_fn( void ) { return fn; }
    bool has_fn( void ) { return fn.size(); }

    // function parameters
    void read_fp( string& p, string& t ) { p = pn.front(); t = pt.front(); }
    int pop_fp( void ) { pn.pop(); pt.pop(); return pn.size(); }
    int fp_cnt( void ) { return pn.size(); }

    // current parsed parameters
    void set_cpn( const string& t ) { cpn = t; }
    string get_cpn( void ) { return cpn; }

    void set_cpt( const string& t ) { cpt = t; }
    string get_cpt( void ) { return cpt; }

    void set_cpd( const string& t ) { cpd = t; }
    string get_cpd( void ) { return cpd; }

    void push_cp( void ) { pn.push(cpn); pt.push(cpt); }
    void clear_cp( void ) { cpn.clear(); cpt.clear(); cpd.clear(); }

  private:
    string remove_chars( const string &s, const string &c );

    string            text;     // comment block text

    string            fn;       // function name if any
    queue<string>     pn;       // function parameter names
    queue<string>     pt;       // function parameter types

    string            cpn;      // current parameter type
    string            cpt;      // current parameter type
    string            cpd;      // current parameter direction
};

// remove characters in list from string
string
block::remove_chars( const string &s, const string &c ) {
  string r;

  for ( string::const_iterator its=s.begin(); its!=s.end(); ++its ) {
    bool append = true;

    for ( string::const_iterator itc=c.begin(); itc!=c.end(); ++itc )
      if ( *its == *itc ) { append = false; break; }

    if (append) r += *its;
  }

  return( r );
}

// end of comment block text formatting
string
block::format( void ) {
  if ( empty() ) return( "" );

  string formated_text;

  // format function prototype
  string fproto;
  if ( has_fn() ) {
    fproto = get_fn() + "(";

    while( fp_cnt() ) {
      string pn, pt;
      read_fp(pn , pt);

      if ( pt.length() )
        fproto += pt + " ";

      // strip special characters "\-." from {id}
      fproto += remove_chars( pn, "\\-.");

      if ( pop_fp() )
        fproto += ", ";
    }
    fproto += ")";
  }

  // copy comment block text
  formated_text = get_text();

  // repeat prototype after comment block
  if ( has_fn() )
    formated_text += fproto + ";\n";

  // clear the comment block
  clear();

  return( formated_text );
}

// error handler
void
abort( const string& m, const int &n = 0, const string &t = "" ) {
  cerr << "ERROR: " << m;

  if( n )           cerr << ", at line " << n;
  if( t.length() )  cerr << ", near [" << t << "]";

  cerr << ", aborting..." << endl;

  exit( EXIT_FAILURE );
}

// comment block global variable allocation
block cb;

%}

%option c++
%option prefix="bash_dif"
%option stack
%option yylineno
%option noyywrap
%option nounput
%option nodefault
%option debug

%s COMMENT READCL READAP READFN

ws                                [ \t]
nr                                [\n\r]
  /* id with and without spaces */
id                                [_\\\-.[:alnum:]]+
ids                               [ _\\\-.[:alnum:]]+

dir                               "["{id}"]"
type                              "<"{ids}">"

bcmt                              "#/"[#!/]+
ecmt                              "#"+"/"

  /* match from beginning of line only for cmtld, cmtli */
cmtld                             ^{ws}*"##"[#]*    /* pass comment */
cmtli                             ^{ws}*"#"[#]*     /* in comment */

  /* append trailing whitespace {ws} to avoid substring matching */
kw_afn                            [\\@](?i:afn){ws}
kw_aparam                         [\\@](?i:aparam){ws}
kw_aparami                        [\\@](?i:aparami){ws}
kw_aparamo                        [\\@](?i:aparamo){ws}

%%

  /* outsize comment block */
<INITIAL>{bcmt}                   { cb.app_text( "/**" ); yy_push_state(COMMENT); }
<INITIAL>{cmtld}                  { cb.app_text( "///" ); yy_push_state(READCL); }
<INITIAL>{nr}                     { cb.app_text( YYText() ); cout << cb.format(); }
<INITIAL>.                        ;
<INITIAL><<EOF>>                  { cout << cb.format(); return 0; }

  /* inside comment block */
<COMMENT>{bcmt}                   { abort("nested comment blocks", lineno(), YYText()); }
<COMMENT>{ecmt}                   { cb.app_text( " */" );  yy_pop_state(); }
<COMMENT>{cmtli}                  { cb.app_text( " *" ); }
<COMMENT>{nr}                     { cb.app_text( YYText() ); }
<COMMENT>.                        { cb.app_text( YYText() ); }
<COMMENT><<EOF>>                  { abort("unterminated comment block", lineno()); }

  /* read comment line outside of comment block */
<READCL>{nr}                      { cb.app_text( YYText() ); yy_pop_state(); }
<READCL>.                         { cb.app_text( YYText() ); }

  /* keyword processing */
<COMMENT,READCL>{kw_afn}          { yy_push_state(READFN); }
<COMMENT,READCL>{kw_aparam}       { cb.clear_cp();
                                    string mt = YYText();
                                    string kw = mt.substr(0,1)+"param";
                                    cb.app_text( kw );
                                    yy_push_state(READAP); }
<COMMENT,READCL>{kw_aparami}      { cb.clear_cp(); cb.set_cpd("[in]");
                                    string mt = YYText();
                                    string kw = mt.substr(0,1)+"param";
                                    cb.app_text( kw + " " + cb.get_cpd() );
                                    yy_push_state(READAP); }
<COMMENT,READCL>{kw_aparamo}      { cb.clear_cp(); cb.set_cpd("[out]");
                                    string mt = YYText();
                                    string kw = mt.substr(0,1)+"param";
                                    cb.app_text( kw + " " + cb.get_cpd() );
                                    yy_push_state(READAP); }

  /* function name */
<READFN>{id}                      { cb.set_fn( YYText() ); yy_pop_state(); }
<READFN>{ws}+                     ;
<READFN>{nr}                      { abort("missing function name", lineno() ); }
<READFN>.                         { abort("invalid function name", lineno(), YYText()); }

  /* function parameter type, direction, and name */
<READAP>{type}                    { string mt = YYText();
                                    cb.set_cpt( mt.substr(1,mt.length()-2) ); }
<READAP>{dir}                     { cb.set_cpd( YYText() );
                                    cb.app_text( " " + cb.get_cpd() ); }
<READAP>{id}                      { cb.set_cpn( YYText() );
                                    cb.app_text( " " + cb.get_cpn() );
                                    cb.push_cp(); yy_pop_state(); }
<READAP>{ws}+                     ;
<READAP>{nr}                      { abort("missing parameter name", lineno()); }
<READAP>.                         { abort("invalid parameter name", lineno(), YYText()); }

%%

// main
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
         << "Doxygen input filter for bash source files. Can be used in\n"
            "conjunction with doxygen tags INPUT_FILTER and FILTER_*." << endl
         << endl
         << "Example:" << endl
         << "  FILTER_PATTERNS = *.bash=<prefix>/bin/" << argv[0] << endl
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
    bash_difFlexLexer* lexer = new bash_difFlexLexer( &infile , &cout );

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
 eof
*******************************************************************************/
