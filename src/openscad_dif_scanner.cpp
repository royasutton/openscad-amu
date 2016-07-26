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

  // must match equivalent definition for {id_var} in openscad_dif_lexer.ll
  // lexer use flex while varm uses <boost/regex.hpp>
  varm.set_prefix( "${" );
  varm.set_suffix( "}" );
  varm.set_regexp( "\\${[_[:alnum:]]+}" );
  varm.set_escape_prefix( "\\\\" );
  varm.set_escape_suffix( "" );
  varm.set_escape_prefix_length( 1 );
  varm.set_escape_suffix_length( 0 );

  // report options could be passed to the command line interface
  // for run-time configuration.
  varm.set_report( true );
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
  cerr << endl << ops << m;

  if( n )           cerr << ", at line " << n;
  if( t.length() )  cerr << ", near [" << t << "]";

  cerr << ", aborting..." << endl;

  exit( EXIT_FAILURE );
}

string
ODIF::ODIF_Scanner::amu_error_msg(const string& m)
{
  ostringstream os;

  os << "<tt>"
     << ops
     << input_name
     << ", line " << dec << lineno() << ", "
     << get_word(amu_parsed_text, 1)
     << " " << m
     << "</tt>";

  return ( os.str() );
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
ODIF::ODIF_Scanner::remove_chars(const string &s, const string &c)
{
  return ( replace_chars(s, c, '\0') );
}

string
ODIF::ODIF_Scanner::replace_chars(const string &s, const string &c, const char r)
{
  string result;

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
  // assumes quotes exists as first and last characters if any

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

  // search substring for first and last quote characters if any

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
ODIF::ODIF_Scanner::fx_pend(void)
{
  fi_eline = lineno();

  bool found = false;

  string result;

  if      (fx_name.compare("eval")==0)    { found=true; result=bif_eval(); }
  else if (fx_name.compare("shell")==0)   { found=true; result=bif_shell(); }
  else if (fx_name.compare("combine")==0) { found=true; result=bif_combine(); }
  else if (fx_name.compare("image_table")==0) { found=true; result=bif_image_table(); }
  else if (fx_name.compare("html_viewer")==0) { found=true; result=bif_html_viewer(); }
  else                                    { found = false; }

  if ( found )
  {
    // output to scanner or store to variable map
    if ( fx_tovar.length() == 0 )
      scanner_output( result );
    else
      varm.store(fx_tovar, result);
  }
  else
  {
    string et = amu_error_msg("unknown function.");

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
  if ( fx_tovar.length() )
    abort("previously defined var: " + fx_tovar, lineno(), YYText());
  fx_tovar = YYText();
}

void
ODIF::ODIF_Scanner::fx_set_arg_name(void)
{
  // remove '=' from argument name in matched text (last character)
  string mt = YYText();
  fx_argv.set_next_name( mt.substr(0,mt.length()-1) );
}

void
ODIF::ODIF_Scanner::fx_store_arg_escaped(void)
{
  // remove 'escape-prefix' from variable name in matched text.
  string mt = YYText();
  fx_argv.store( mt.substr(varm.get_escape_prefix_length(),mt.length()) );
}

void
ODIF::ODIF_Scanner::fx_app_qarg_escaped(void)
{
  // remove 'escape-prefix' from variable name in matched text (first character)
  string mt = YYText();
  fx_qarg+=mt.substr(varm.get_escape_prefix_length(),mt.length());
}

/***************************************************************************//**

  \details

    For the current text returned by the lexer matching
    <tt>("++"|"--"){id}</tt> or <tt>{id}("++"|"--")</tt>, such as
    <tt>var++</tt> or <tt>--var</tt>, get the operator and variable name. Then
    lookup the current value of the variable from the environment variable map
    and update its value based on the operation. If a named function argument
    has been specified using \ref ODIF::func_args::set_next_name, then assign
    the pre or post operation value to a function argument with this name.
    Finally, store the updated value. Variables with trailing operators are stored
    in the environment variable map, while variable with preceding operators are
    stored as a named function argument in the form of
    <tt>(variable-name)=(post-operation-value)</tt>.

     operation | behavior
    :---------:|:----------------------------------------------
      x++      | store increment as environment variable
      ++x      | store increment as function argument (x=${x})
      y=x++    | store function argument (y=(${x}))
      y=++x    | store function argument (y=(${x}+1))

*******************************************************************************/
void
ODIF::ODIF_Scanner::fx_incr_arg(bool post)
{
  string mt = YYText();

  // get operator and variable name
  string vn, op;
  if ( post )
  {
     vn=mt.substr(0, mt.length()-2);
     op=mt.substr(mt.length()-2,2);
  } else {
     vn=mt.substr(2, mt.length()-2);
     op=mt.substr(0, 2);
  }

  // get the current value of the variable from the environment variable map
  long old_val;
  if ( varm.exists( vn ) )      old_val = atoi( varm.expand("${" + vn + "}").c_str() );
  else                          old_val = 0;

  // update the variable value
  long new_val;
  if ( op.compare("++") == 0 )  new_val = old_val + 1;
  else                          new_val = old_val - 1;

  // conditionally assign value the named function argument?
  if ( fx_argv.get_next_name().length() )
  {
    if ( post )                 fx_argv.store( to_string(old_val) );      // var++
    else                        fx_argv.store( to_string(new_val) );      // ++var
  }

  // store variable:
  // var++ : as named function argument (name=count)
  // ++var : in the environment variable map
  if ( post )                   varm.store( vn, to_string(new_val) );     // var++
  else                          fx_argv.store( vn, to_string(new_val) );  // ++var
}


void
ODIF::ODIF_Scanner::def_init(void)
{
  apt_clear();
  apt();

  def_name.clear();
  def_text.clear();

  def_bline = lineno();
}

void
ODIF::ODIF_Scanner::def_pend(void)
{
  def_eline = lineno();

  // if variable name not specified, copy definition to output
  // otherwise store in variable map.
  if ( def_name.length() == 0 )
    scanner_output( def_text );
  else
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
  if ( def_name.length() )
    abort("previously defined var: " + def_name, lineno(), YYText());
  def_name=YYText();
}


/***************************************************************************//**

  \details

    Create a local copy of the current variable map (scope), add all named
    variable arguments pairs to local variable scope (name=value), then
    form the output text by appending each each positional argument value
    (separated by a single space character), skipping the function name,
    to a string and recursively expanding all variables in the text. Escaped
    variables are reduced and checked during subsequent expansions. The
    expanded string is returned when there are no variable substitutions
    or escaped variable reductions in the text.

*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_eval(void)
{
  // create local variable scope map
  env_var vm = varm;

  // add all argument name=value pairs to local variable scope
  vector<string> nv = fx_argv.names_v(true, false);
  for ( vector<string>::iterator it=nv.begin(); it!=nv.end(); ++it )
    vm.store( *it, fx_argv.arg( *it ) );

  string result;

  // process each positional argument value, skip function name (position zero)
  vector<string> pv = fx_argv.values_v(false, true);
  for ( vector<string>::iterator it=pv.begin()+1; it!=pv.end(); ++it ) {
    // expand variables in the positional argument value text
    string epat = vm.expand_text( *it );

    if ( result.length() != 0 ) result.append(" ");

    result.append( epat );
  }

  return( result );
}

/***************************************************************************//**

  \details

    Use the C/C++ function popen() to submit the string at the first
    positional argument in a shell. The output generated by the command is
    captured to a string, striped of all return and linefeed characters
    and returned.

    Prefix the option with \c ++ to enable and \c \-- to disable. For example
    <tt>++eval</tt> will turn on variable expansion.

     option  | default | description
    :-------:|:-------:|:---------------------------------------
      stderr | false   | capture standard error output,
      rmnl   | true    | remove line-feeds / carriage returns.
      eval   | false   | expand variables in text prior to return.

*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_shell(void)
{
  bool opt_stde = false;
  bool opt_rmnl = true;
  bool opt_eval = false;

  // validate arguments:
  // enforce one positional argument (plus arg0),
  if ( fx_argv.size(false, true) != 2 )
    return(amu_error_msg("requires a single positional argument."));

  // process named arguments.
  vector<string> av = fx_argv.names_v(true, false);
  for ( vector<string>::iterator it=av.begin(); it!=av.end(); ++it )
  {
    if      (it->compare("stderr")==0) opt_stde=( atoi(fx_argv.arg(*it).c_str()) > 0 );
    else if (it->compare("rmnl")==0)   opt_rmnl=( atoi(fx_argv.arg(*it).c_str()) > 0 );
    else if (it->compare("eval")==0)   opt_eval=( atoi(fx_argv.arg(*it).c_str()) > 0 );
    else
      return( amu_error_msg(*it + " invalid. options are (++|--)(stderr|rmnl|eval)") );
  }

  // unquote the command string
  string scmd = unquote( fx_argv.arg( 1 ) );

  if ( opt_stde )
    scmd.append(" 2>&1");

  FILE* pipe;
  char buffer[128];

  // XXX replace popen _popen for windows.
  pipe = popen( scmd.c_str(), "r" );

  if (!pipe)
    return( amu_error_msg("popen() failed for " + scmd) );

  string result;
  while ( !feof(pipe) )
  {
    if ( fgets(buffer, 128, pipe) != NULL )
        result.append( buffer );
  }

  // XXX replace pclose with _pclose for windows.
  pclose(pipe);

  if (opt_rmnl)
    result = replace_chars(result, "\n\r", ' ');

  if (opt_eval)
    result = varm.expand_text(result);

  return( result );
}

/***************************************************************************//**

  \details

    For the list of sets set1, set2, ..., setn, combine each element of
    each set one at a time to form a named n-tuple word. Each word has
    the configurable prefix added to the front and configurable suffix
    added to the end and are joined by a configurable word separator.

    The elements of each set should (obviously) be enclosed within
    quotation marks.

*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_combine(void)
{
  // get the positional arguments
  vector<string> pa = fx_argv.values_v(false, true);

  // check for named arguments
  string prefix = unquote( fx_argv.arg_firstof("prefix", "p") );
  string suffix = unquote( fx_argv.arg_firstof("suffix", "s") );
  string separator = unquote( fx_argv.arg_firstof("separator", "f") );

  // validate arguments:
  // enforce three argument minimum: 0:<function> 1:<base> 2:<opt1>
  if ( prefix.length()==0 ) {
    if ( pa.size() < 3 )
      return(amu_error_msg("requires at least two positional arguments."));

    prefix = unquote( pa[ 1 ] );

    pa.erase( pa.begin() ); // arg0 function
    pa.erase( pa.begin() ); // arg1 opt1
  } else {
    if ( pa.size() < 2 )
      return(amu_error_msg("requires at least one positional argument."));

    pa.erase( pa.begin() ); // arg0 function
  }

  vector<string> sv;
  for ( vector<string>::iterator it=pa.begin(); it!=pa.end(); ++it )
    sv.push_back( unquote( *it ) );

  // if named suffix, add to end of set vector
  if ( suffix.length()!=0 ) {
    sv.push_back( unquote( suffix ) );
  }

  // default separator
  if ( separator.length()==0 ) {
    separator = ",";
  }

  string result;
  bif_combineR( prefix, sv, result, separator );

  return( result );
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

/***************************************************************************//**
  \details
*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_image_table(void)
{
  string type           = unquote( fx_argv.arg_firstof("type",          "t") );
  string table_width    = unquote( fx_argv.arg_firstof("table_width",   "tw") );
  string table_title    = unquote( fx_argv.arg_firstof("table_title",   "tt") );
  string columns        = unquote( fx_argv.arg_firstof("columns",       "c" ) );
  string column_titles  = unquote( fx_argv.arg_firstof("column_titles", "ct") );
  string image_width    = unquote( fx_argv.arg_firstof("image_width",   "iw") );
  string image_height   = unquote( fx_argv.arg_firstof("image_height",  "ih") );
  string image_files    = unquote( fx_argv.arg_firstof("image_files",   "if") );
  string image_titles   = unquote( fx_argv.arg_firstof("image_titles",  "it") );

  // validate arguments:
  // required arguments: type must exists and be one of (html | latex)
  if ( ! fx_argv.exists("type") )
    return( amu_error_msg("table type must be specified.") );
  else if ( type.compare("html") && type.compare("latex") )
    return( amu_error_msg( "type " + type + " is invalid. may be (html|latex).") );


  cout  << endl
        << "type = [" << type << "]" << endl
        << "table_width = [" << table_width << "]" << endl
        << "table_title = [" << table_title << "]" << endl
        << "columns = [" << columns << "]" << endl
        << "column_titles = [" << column_titles << "]" << endl
        << "image_width = [" << image_width << "]" << endl
        << "image_height = [" << image_height << "]" << endl
        << "image_files = [" << image_files << "]" << endl
        << "image_titles = [" << image_titles << "]" << endl;

  string result = replace_chars(amu_parsed_text, "\n\r", ' ');
  result = "<under development>";

  return( result );
}

/***************************************************************************//**
  \details
*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_html_viewer(void)
{
  string result;

  result = amu_error_msg("unimplemented.");

  return( result );
}


/*******************************************************************************
// eof
*******************************************************************************/
