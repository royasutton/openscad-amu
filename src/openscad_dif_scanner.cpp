/***************************************************************************//**

  \file   openscad_dif_scanner.cpp

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
    OpenSCAD Doxygen input filter scanner class source.

  \ingroup openscad_dif_src
*******************************************************************************/

using namespace std;

#include "openscad_dif_scanner.hpp"

ODIF::ODIF_Scanner::ODIF_Scanner(const string& f, const string& s)
{
  // initialize output prefix string
  set_ops( s );

  input_name = f;
  scanner_output_on = true;

  init();
}

ODIF::ODIF_Scanner::~ODIF_Scanner(void)
{
  if ( input_file.is_open() ) {
    input_file.close();
  }
}

void
ODIF::ODIF_Scanner::init(void)
{
  if ( input_file.is_open() ) {
    input_file.close();
  }

  input_file.open( input_name.c_str() );

  if ( !input_file.good() ) {
    cerr << ops << "unable to open input file [" << input_name << "]";
    LexerError(", aborting...");
  }

  // set lexer input to opened input file
  switch_streams( &input_file );

  // initialize variable map
  varm.clear();
  varm.set_report_message("<tt><UNDEFINED></tt>");

  // initialize function argument positional prefix
  fx_argv.clear();
  fx_argv.set_pos_prefix("arg");
}

void
ODIF::ODIF_Scanner::scanner_output( const char* buf, int size )
{
  if (scanner_output_on) {
    LexerOutput( buf, size );
  }
}

void
ODIF::ODIF_Scanner::abort(const string& m, const int &n, const string &t)
{
  cerr << "ERROR: " << m;

  if( n )           cerr << ", at line " << n;
  if( t.length() )  cerr << ", near [" << t << "]";

  cerr << ", aborting..." << endl;

  exit( EXIT_FAILURE );
}

string
ODIF::ODIF_Scanner::get_word(const string& w, const int n)
{
  istringstream iss(w);
  string iw, rw;

  for (int l=1; iss >> iw; l++)
    if ( l==n ) {
      rw=iw;
      break;
    }

  return rw;
}

string
ODIF::ODIF_Scanner::remove_chars( const string &s, const string &c ) {
  string r;

  for ( string::const_iterator its=s.begin(); its!=s.end(); ++its ) {
    bool append = true;

    for ( string::const_iterator itc=c.begin(); itc!=c.end(); ++itc )
      if ( *its == *itc ) { append = false; break; }

    if (append) r += *its;
  }

  return( r );
}

void
ODIF::ODIF_Scanner::fx_init(void) {
  apt_clear();
  apt();

  fx_name.clear();
  fx_tovar.clear();
  fx_path.clear();
  fx_argv.clear();

  fx_qarg.clear();

  fi_bline = lineno();

  // remove '\amu_' from function name in matched text
  string mt = YYText();
  fx_name = mt.substr(5,mt.length());

  // store function name as first argument.
  fx_store_arg( fx_name );
}

void
ODIF::ODIF_Scanner::fx_eval(void) {
  fi_eline = lineno();

  bool found = false;

  /* string     fx_name, fx_tovar, fx_path
     func_args  fx_argv */

  if      (fx_name.compare("eval")==0)    { found=true; bif_eval(); }
  else if (fx_name.compare("shell")==0)   { found=true; bif_shell(); }
  else if (fx_name.compare("enum")==0)    { found=true; bif_enum(); }
  else
  {
    // search scripts

  }

  if ( !found ) {
    string et = "<tt>" + ops + get_word(amu_parsed_text, 1) + " unknown</tt>";

    scanner_output( et );
  }

  // output blank lines to maintain file length when functions are
  // broken across multiple lines (don't begin and end on the same line).
  for(size_t i=fi_bline; i<fi_eline; i++)
    scanner_output("\n");  // XXX make output more portable
}

void
ODIF::ODIF_Scanner::bif_eval(void) {
  scanner_output( "eval" );
}

void
ODIF::ODIF_Scanner::bif_shell(void) {
  scanner_output( "shell" );
}

void
ODIF::ODIF_Scanner::bif_enum(void) {
  scanner_output( "enum" );
  fx_argv.dump();

  cout << "[" << fx_argv.pairs_str(true, true) << "]";
}

void
ODIF::ODIF_Scanner::def_init(void) {
  apt_clear();
  apt();

  def_name.clear();
  def_text.clear();

  def_bline = lineno();
}

void
ODIF::ODIF_Scanner::def_store(void) {
  def_eline = lineno();

  varm.store( def_name, def_text );

  def_name.clear();
  def_text.clear();

  // output blank lines to maintain file length when definitions are
  // broken across multiple lines (don't begin and end on the same line).
  for(size_t i=def_bline; i<def_eline; i++)
    scanner_output("\n");  // XXX make output more portable
}


/*******************************************************************************
// eof
*******************************************************************************/
