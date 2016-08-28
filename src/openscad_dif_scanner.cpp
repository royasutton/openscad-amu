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

#include <boost/filesystem.hpp>

#include <set>

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

using namespace std;
namespace bfs=boost::filesystem;


ODIF::ODIF_Scanner::ODIF_Scanner(const string& f, const string& s)
{
  // initialize output prefix string
  set_ops( s );

  input_name = f;
  scanner_output_on = true;
  debug_filter = false;

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
  // might be good to convert name to all lower case.
  string mt = YYText();
  fx_name = mt.substr(5,mt.length());

  // store function name as first argument.
  fx_store_arg( fx_name );
}

/***************************************************************************//**

  \details

    Compare name with each of the built-in functions. If does not match one
    of the built-in functions, check for function in ${lib_path}/functions.
    If matched is found, call and obtain result. Copy result to output
    or store to named results variable.

  \todo support function amu_mfscript_targets ( scope=test ext=stl ) that
        returns a list of targets for the named makefile scope, extension,
        etc.

*******************************************************************************/
void
ODIF::ODIF_Scanner::fx_pend(void)
{
  fi_eline = lineno();

  string result;
  bool has_result = false;

  /* check build-in functions */
  if      (fx_name.compare("eval")==0)
  {
    has_result=true;
    result=bif_eval();
  }
  else if (fx_name.compare("shell")==0)
  {
    has_result=true;
    result=bif_shell();
  }
  else if (fx_name.compare("combine")==0)
  {
    has_result=true;
    result=bif_combine();
  }
  else if (fx_name.compare("image_table")==0)
  {
    has_result=true;
    result=bif_image_table();
  }
  else if (fx_name.compare("viewer")==0)
  {
    has_result=true;
    result=bif_viewer();
  }
  else if (fx_name.compare("make")==0)
  {
    has_result=true;
    result=bif_make();
  }
  else
  {
    /* check external function */
    bfs::path exfx_path;

    exfx_path  = lib_path;
    exfx_path /= "functions";
    exfx_path /= "amu_" + fx_name;

    if ( bfs::exists( exfx_path ) )
    {
      if ( bfs::is_regular_file( exfx_path ) )
      {
        string scmd = exfx_path.string();

        // append arguments
        typedef vector<func_args::arg_term>::iterator fa_iter;
        for ( fa_iter it=fx_argv.argv.begin()+1; it!=fx_argv.argv.end(); ++it ) {
          scmd.append( " " );
          if ( it->positional ) scmd.append( it->value );
          else                  scmd.append( it->name + "=" + it->value );
        }

        filter_debug( scmd );
        UTIL::sys_command( scmd, result, has_result, false );
      }
      else
      {
        result = exfx_path.string() + " is not a regular file.";
      }
    }
    else
    {
      /* built-in not matched and external function does not exists */
      result = "unknown function.";
    }
  }

  if ( has_result )
  {
    // output result to scanner or store to variable map
    if ( fx_tovar.length() == 0 )
      scanner_output( result );
    else {
      varm.store(fx_tovar, result);

      filter_debug( fx_tovar + "=[" + result + "]" );
    }
  }
  else
  {
    scanner_output( amu_error_msg(result) );
  }

  // output blank lines to maintain file length when functions are
  // broken across multiple lines (don't begin and end on the same line).
  for(size_t i=fi_bline; i<fi_eline; i++) scanner_output("\n");
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
  // remove '=' appended argument name in matched text (ie: 'id=' last character)
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
  for(size_t i=def_bline; i<def_eline; i++) scanner_output("\n");
}

void
ODIF::ODIF_Scanner::def_set_name(void)
{
  if ( def_name.length() )
    abort("previously defined var: " + def_name, lineno(), YYText());
  def_name=YYText();
}

void
ODIF::ODIF_Scanner::filter_debug(
  const string& m,
  const bool& h,
  const bool& f,
  const bool& s
)
{
  if ( debug_filter )
  {
    if (s )
    {
      if ( h )
      {
        scanner_output( "\n\\if __INCLUDE_FILTER_DEBUG__\n" );
        scanner_output( "\\verbatim\n" );
      }

      scanner_output( m );
      scanner_output( "\n" );

      if ( f )
      {
        scanner_output( "\\endverbatim\n" );
        scanner_output( "\\endif\n" );
      }
    }

    cerr << ops << m << endl;
  }
}

/***************************************************************************//**

  \param file       file to locate.
  \param found      status of if the file was located.
  \param extension  return file name with file extension.
  \param uri        return file name with URI formatting.
  \param copy       copy the found file to the specified outdir.
  \param outdir     destination directory for file copy.
  \param rid        rename the copied file with an random identifier.

*******************************************************************************/
string
ODIF::ODIF_Scanner::file_rl(
  const string& file,
        bool& found,
  const bool& extension,
  const bool& uri,
  const bool& copy,
  const string& outdir,
  const bool& rid
)
{
  filter_debug( "locate file: [" + file + "]", true, false, false);

  // check each include path for file.
  bfs::path found_file;
  found = false;
  for( vector<string>::iterator it = include_path.begin();
                                it != include_path.end() && !found;
                              ++it )
  {
    bfs::path f( file );
    bfs::path p;

    // check with just the filename.
    p  = *it;
    p /= f.filename();

    filter_debug( "  checking: " + p.string(), false, false, false);
    if ( exists(p) && is_regular_file(p) ) {
      found = true;
      found_file = p;

      break;
    }

    // check with existing prefixed path.
    p  = *it;
    p /= f;

    filter_debug( "  checking: " + p.string(), false, false, false);
    if ( exists(p) && is_regular_file(p) ) {
      found = true;
      found_file = p;

      break;
    }
  }

  string return_file;
  if ( found )
  {
    filter_debug( " found.", false, false, false);

    // target is found file name without a prefixed path.
    bfs::path target( found_file.filename() );

    if ( copy )
    {
      // source file
      bfs::path source( found_file );

      // generate rid for target file?
      if ( rid == true ) {
        target = bfs::unique_path( bfs::path("%%%%-%%%%-%%%%-%%%%-%%%%-%%%%") );
        target += found_file.extension();
      }

      // handle configuration prefix if not current directory (or empty)
      bfs::path out_path;
      if ( get_config_prefix().compare(".") && get_config_prefix().length() )
        out_path /= get_config_prefix();

      // were scripts and configuration already prefixed by seam?
      // if not, add the output prefix to the output path.
      if ( get_prefix_scripts() == false )
        out_path /= get_output_prefix();

      out_path /= outdir;

      bfs::path target_path ( out_path / target );
      bool copy_target = true;

      filter_debug( " copying to [" + out_path.string() + "]", false, false, false);

      // do not copy if the file exists and has same size.
      if ( exists(target_path) && is_regular_file(target_path) )
      {
        if ( bfs::file_size( source ) == bfs::file_size( target_path ) )
        {
          filter_debug( "  same sized file exists.", false, false, false);

          copy_target = false;
        }
        else
        {
          bfs::remove( target_path );
        }
      }

      if ( copy_target )
      {
        bfs::copy_file( source, target_path, bfs::copy_option::overwrite_if_exists );
        filter_debug( " done.", false, false, false);
      }
    }

    // return file name with file extension?
    if ( extension == true )
      return_file = target.string();
    else
      return_file = target.stem().string();

    // return file name with URI formating?
    if ( uri == true )
      return_file = "file://" + return_file;
  }
  else
  { // file not located, return original reference string.
    return_file = file;
  }

  filter_debug( " reference [" + return_file + "]", false, true, false);

  return( return_file );
}


/*******************************************************************************
// eof
*******************************************************************************/
