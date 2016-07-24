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

#include "openscad_dif_scanner.hpp"

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

#include "config.h"

using namespace std;


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
  fx_argv.set_prefix("arg");
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

    if ( append )       result += *its;
    else if (r != '\0') result += r;
  }

  return( result );
}

string
ODIF::ODIF_Scanner::unquote(const string &s)
{
  string r = s;

  /*
  // check if string has at least two characters
  if ( s.length()>1 ) {
    // check if first character is ['] or ["]
    if ( s.at(0) == '\'' || s.at(s.length()-1) == '\"' ) {
      // now check if first and last characters match
      if ( s.at(0) == s.at(s.length()-1) ) {
        // check for quoted NULL ""
        if ( s.length() == 2 )
          r.clear();
        else
          // update 'r' with removed the first and last characters
          r = s.substr( 1, s.length()-2 );
      }
    }
  }
  */

  size_t fp = s.find_first_of("\"\'");
  size_t lp = s.find_last_of("\"\'");

  // make sure different character pointers
  if ( fp != lp ) {
    // make sure the characters match: ie '' or ""
    if ( s.at(fp) == s.at(lp) ) {
      // check for quoted NULL ""
      if ( (lp-fp) < 2 )
        r.clear();
      else
        r = s.substr( fp+1, lp-1 );
    }
  }

  return( r );
}

void
ODIF::ODIF_Scanner::fx_init(void)
{
  apt_clear();
  apt();

  fx_name.clear();
  fx_tovar.clear();
  fx_argv.clear();

  fx_qarg.clear();

  fi_bline = lineno();

  // remove '\amu_' from function name in matched text
  // XXX: might be good to convert name to all lower case.
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
  else if (fx_name.compare("combine")==0) { found=true; bif_combine(); }
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
    scanner_output("\n");  // XXX: make output more portable: (cr, lf, cr+lf)
}

void
ODIF::ODIF_Scanner::fx_set_tovar(void)
{
  if ( fx_tovar.length() ) abort("previously defined var: " + fx_tovar);
  fx_tovar = remove_chars(YYText(), "| \t");
}

void
ODIF::ODIF_Scanner::fx_set_path(void)
{
  if ( fx_path.length() ) abort("previously defined opt: " + fx_path);
  fx_path = YYText();
}

void
ODIF::ODIF_Scanner::fx_set_arg_name(void)
{
  // remove '=' from argument name in matched text (last character)
  std::string mt = YYText();
  fx_argv.set_next_name( mt.substr(0,mt.length()-1) );
}

void
ODIF::ODIF_Scanner::fx_store_arg_escaped(void)
{
  // remove '\' from variable name in matched text (first character)
  std::string mt = YYText();
  fx_argv.set_next_name( mt.substr(1,mt.length()) );
}

void
ODIF::ODIF_Scanner::fx_app_qarg_escaped(void)
{
  // remove '\' from variable name in matched text (first character)
  std::string mt = YYText();
  fx_qarg+=mt.substr(1,mt.length());
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
    scanner_output("\n");  // XXX: make output more portable: (cr, lf, cr+lf)
}

void
ODIF::ODIF_Scanner::def_set_name(void)
{
  if ( def_name.length() ) abort("previously defined var: " + def_name);
  def_name=YYText();
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
ODIF::ODIF_Scanner::bif_combine(void) {
  // get the positional arguments
  vector<string> pa = fx_argv.values_v(false, true);

  // check for named arguments
  string prefix = remove_chars( fx_argv.arg_firstof("--prefix", "-p"), "\"\'" );
  string suffix = remove_chars( fx_argv.arg_firstof("--suffix", "-s"), "\"\'" );
  string separator = remove_chars( fx_argv.arg_firstof("--separator", "-f"), "\"\'" );

  // validate arguments:
  // enforce three argument minimum: 0:<function> 1:<base> 2:<opt1>
  if ( prefix.length()==0 ) {
    if ( pa.size() < 3 )
      return;

    prefix = remove_chars( pa[ 1 ], "\"\'" );

    pa.erase( pa.begin() ); // arg0 function
    pa.erase( pa.begin() ); // arg1 opt1
  } else {
    if ( pa.size() < 2 )
      return;

    pa.erase( pa.begin() ); // arg0 function
  }

  vector<string> sv;
  for ( vector<string>::iterator it=pa.begin(); it!=pa.end(); ++it )
    sv.push_back( remove_chars( *it, "\"\'" ) );

  // if named suffix, add to end of set vector
  if ( suffix.length()!=0 ) {
    sv.push_back( remove_chars( suffix, "\"\'" ) );
  }

  // default separator
  if ( separator.length()==0 ) {
    separator = ",";
  }

  string result;
  bif_combineR( prefix, sv, result, separator );

  // output to scanner or store to variable
  if ( fx_tovar.length() == 0 )
    scanner_output( result );
  else
    varm.store(fx_tovar, result);
}

void
ODIF::ODIF_Scanner::bif_combineR( const string &s, vector<string> sv,
                                        string &r, const string &rs)
{
  if ( sv.size()== 0 )
  {
    if( r.length() !=0 ) r+= rs;

    r+=s;
  }
  else
  {
    // separate member words of first set of 'sv' and store in 'sm'
    string          fs = sv.front();
    vector<string>  sm;

    boost::char_separator<char> sep(", ");
    boost::tokenizer< boost::char_separator<char> > tokens(fs, sep);

    for( boost::tokenizer< boost::char_separator<char> >::iterator
          it=tokens.begin();
          it!=tokens.end();
        ++it )
    {
      sm.push_back( *it );
    }

    // for each word in 'sm' combine with words of remaining sets
    sv.erase( sv.begin() );
    for ( vector<string>::iterator it=sm.begin(); it!=sm.end(); ++it ) {
      bif_combineR(s + *it, sv, r, rs);
    }
  }
}


/*******************************************************************************
// eof
*******************************************************************************/
