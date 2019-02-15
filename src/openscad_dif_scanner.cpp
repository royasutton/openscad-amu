/***************************************************************************//**

  \file   openscad_dif_scanner.cpp

  \author Roy Allen Sutton
  \date   2016-2019

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
    OpenSCAD Doxygen input filter scanner class source.

  \ingroup openscad_dif_src
*******************************************************************************/

#include "openscad_dif_scanner.hpp"

#include <boost/filesystem.hpp>
#include <boost/assign/list_of.hpp>

#include <set>
// #include <map>

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

using namespace std;
namespace bfs=boost::filesystem;

////////////////////////////////////////////////////////////////////////////////
// scanner public
////////////////////////////////////////////////////////////////////////////////

ODIF::ODIF_Scanner::ODIF_Scanner(const string& f, const string& s)
{
  // initialize output prefix string
  set_ops( s );

  // default operation
  scanner_output_on = true;
  debug_filter = false;

  // initialize variable map
  gevm.clear();

  // must match equivalent definition for {id_var} in openscad_dif_lexer.ll
  // lexer use flex while gevm uses <boost/regex.hpp>
  gevm.set_prefix( "${" );
  gevm.set_suffix( "}" );
  gevm.set_regexp( "\\${[_[:alnum:]]+}" );
  gevm.set_escape_prefix( "\\\\" );
  gevm.set_escape_suffix( "" );
  gevm.set_escape_prefix_length( 1 );
  gevm.set_escape_suffix_length( 0 );

  // report options could be passed to the command line interface
  // for run-time configuration.
  gevm.set_report( true );
  gevm.set_report_message("<tt><UNDEFINED></tt>");

  //
  // setup predefined environment variables
  //

  // relative path name of the root input file 'f'
  bfs::path ifap = f;
  bfs::path ifrp = UTIL::get_relative_path(ifap, bfs::current_path());
  // setup root file name variables
  gevm.store( "ABS_FILE_NAME", ifap.string() );
  gevm.store( "ABS_PATH_NAME", ifap.parent_path().string() );
  gevm.store( "FILE_NAME", ifrp.string() );
  gevm.store( "PATH_NAME", ifrp.parent_path().string() );
  gevm.store( "BASE_NAME", ifrp.filename().string() );
  gevm.store( "STEM_NAME", ifrp.stem().string() );
  gevm.store( "EXT_NAME", ifrp.extension().string() );

  // amu_eval field separator
  gevm.store( "EFS", " " );

  // initialize include file variables
  gevm.store( "FILE_CURRENT", "" );
  gevm.store( "FILE_LIST", "" );

  // initialize function argument positional prefix
  fx_argv.clear();
  fx_argv.set_prefix("arg");

  // clear the input file stream vector
  ifs_v.clear();

  // the passed file 'f' is the root input file
  start_file( f );
}

void
ODIF::ODIF_Scanner::update_gevm(void)
{
  // configuration variables
  gevm.store( "AUTO_CONFIG_PATH", get_config_prefix() );

  gevm.store( "DOXYGEN_OUTPUT", get_doxygen_output() );
  gevm.store( "HTML_OUTPUT", get_html_output() );
  gevm.store( "LATEX_OUTPUT", get_latex_output() );
  gevm.store( "DOCBOOK_OUTPUT", get_docbook_output() );
  gevm.store( "RTF_OUTPUT", get_rtf_output() );

  gevm.store( "SCOPE_JOINER", get_scopejoiner() );

  gevm.store( "OUTPUT_PREFIX", get_output_prefix() );
  gevm.store( "OPENSCAD_PATH", get_openscad_path() );
  gevm.store( "OPENSCAD_EXT", get_openscad_ext() );
}

////////////////////////////////////////////////////////////////////////////////
// general
////////////////////////////////////////////////////////////////////////////////

void
ODIF::ODIF_Scanner::start_file( const string file )
{
  ifstream *ifs = new ifstream();

  // open file stream
  ifs->open( file.c_str() );

  if ( !ifs->good() )
    abort( "unable to open input file", lineno(), file );

  // get canonical file path
  string file_path = bfs::canonical( bfs::path(file) ).string();

  // append to ${FILE_LIST}
  string list = gevm.expand( gevm.get_prefix() + "FILE_LIST" + gevm.get_suffix() );
  if ( list.length() ) list += " ";
  list += file_path;
  gevm.store( "FILE_LIST", list );

  // update ${FILE_CURRENT}
  gevm.store( "FILE_CURRENT", file_path );

  // save line number update of current stream if it exists
  if ( ! ifs_v.empty() )
    ifs_v.back().line = yylineno;

  // add next stream to back of input stream vector
  ifs_s ifs_next = { file_path, ifs, 1 };
  ifs_v.push_back( ifs_next );

  // reset line number count for next file
  yylineno = 1;

  // switch input stream
  switch_streams( ifs );
}

int
ODIF::ODIF_Scanner::yywrap(void)
{
  if ( ifs_v.empty() )          // input stream vector empty
    return 1;

  ifs_v.back().ifs->close();    // close current file
  delete ifs_v.back().ifs;      // free storage
  ifs_v.pop_back();             // discard and get next stream

  if ( ifs_v.empty() )          // has last file has been closed?
    return 1;

  // update ${FILE_CURRENT}
  gevm.store( "FILE_CURRENT", ifs_v.back().name );

  // restore saved line number count
  yylineno = ifs_v.back().line;

  // return to previous input stream
  switch_streams( ifs_v.back().ifs );

  return 0;
}

string
ODIF::ODIF_Scanner::get_input_name(bool root)
{
  if ( ifs_v.empty() )              // no input streams
    return "none";

  if ( root )
    return ( ifs_v.front().name );  // first is root
  else
    return ( ifs_v.back().name );   // last is current
}

void
ODIF::ODIF_Scanner::scanner_output( const char* buf, int size )
{
  if (scanner_output_on) {
    LexerOutput( buf, size );
  }
}

void
ODIF::ODIF_Scanner::error(const string& m, const int &n,
                          const string &t, bool a)
{
  string om;

  om = ops + "ERROR in "
     + gevm.expand( gevm.get_prefix() + "FILE_CURRENT" + gevm.get_suffix() )
     +  ", " + m;

  if( n )           om += ", at line " + UTIL::to_string( n );
  if( t.length() )  om += ", near [" + t + "]";
  if( !a )          om += ", continuing..." ;

  cerr << om << endl;
  scanner_output( "<tt>" + om + "</tt><br>\n" );

  if( a )
    LexerError( string(ops + "aborting...").c_str() );
  else
    return;
}

string
ODIF::ODIF_Scanner::amu_error_msg(const string& m)
{
  string om;

  om  = ops + "ERROR in "
      + gevm.expand( gevm.get_prefix() + "FILE_CURRENT" + gevm.get_suffix() )
      + ", at line " + UTIL::to_string( lineno() );

  // delete command characters [\@] from parsed text to prevent them
  // from being interpreted by doxygen and/or the html browser.
  om += ", near [" + UTIL::replace_chars( amu_parsed_text, "\\@" ) + "]"
      +  ", " + m;

  cerr << om << endl;

  return( "<tt>" + om + "</tt><br>" );
}

////////////////////////////////////////////////////////////////////////////////
// amu_* function
////////////////////////////////////////////////////////////////////////////////

void
ODIF::ODIF_Scanner::fx_init(void)
{
  // update local copy of global variable map (memberwise)
  levm = gevm;

  apt_clear();
  apt();

  fx_name.clear();
  fx_var.clear();
  fx_argv.clear();

  fx_qarg.clear();

  fx_body_text.clear();
  fx_body_level = 0;

  // remove '\amu_' from function name in matched text
  // might be good to convert name to all lower case.
  string mt = YYText();
  fx_name = mt.substr(5,mt.length());

  // store function name as first argument.
  fx_store_arg( fx_name );

  fx_bline = lineno();
}

/***************************************************************************//**

  \details

    Compare name with each of the built-in functions. If does not match one
    of the built-in functions, check for function in ${lib_path}/functions.
    If matched is found, call and obtain result. Copy result to output
    or store to named results variable.

*******************************************************************************/
void
ODIF::ODIF_Scanner::fx_end(void)
{
  fx_eline = lineno();

  // prototype of build-in function: string functionname( void );
  typedef map< string, string (ODIF::ODIF_Scanner::*)(void) > function_table_type;

  // function jump table
  static function_table_type function_table = boost::assign::map_list_of
      ("combine",       &ODIF::ODIF_Scanner::bif_combine)
      ("copy",          &ODIF::ODIF_Scanner::bif_copy)
      ("eval",          &ODIF::ODIF_Scanner::bif_eval)
      ("file",          &ODIF::ODIF_Scanner::bif_file)
      ("filename",      &ODIF::ODIF_Scanner::bif_filename)
      ("find",          &ODIF::ODIF_Scanner::bif_find)
      ("foreach",       &ODIF::ODIF_Scanner::bif_foreach)
      ("image",         &ODIF::ODIF_Scanner::bif_image)
      ("image_table",   &ODIF::ODIF_Scanner::bif_image_table)
      ("make",          &ODIF::ODIF_Scanner::bif_make)
      ("openscad",      &ODIF::ODIF_Scanner::bif_openscad)
      ("pathid",        &ODIF::ODIF_Scanner::bif_pathid)
      ("replace",       &ODIF::ODIF_Scanner::bif_replace)
      ("scope",         &ODIF::ODIF_Scanner::bif_scope)
      ("seq",           &ODIF::ODIF_Scanner::bif_seq)
      ("shell",         &ODIF::ODIF_Scanner::bif_shell)
      ("source",        &ODIF::ODIF_Scanner::bif_source)
      ("table",         &ODIF::ODIF_Scanner::bif_table)
      ("viewer",        &ODIF::ODIF_Scanner::bif_viewer)
      ("word",          &ODIF::ODIF_Scanner::bif_word)
  ;

  string result;
  bool success = false;

  //
  // locate and call named function
  //

  function_table_type::iterator entry = function_table.find ( fx_name );
  if ( entry != function_table.end() )
  { // found in function table
    result = (this->*(entry->second))();
    success = true;
  }
  else
  {
    /* check external functions */
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
        UTIL::sys_command( scmd, result, success, false, false );
      }
      else
      {
        result = exfx_path.string() + " is not a regular file.";
      }
    }
    else
    {
      /* built-in not matched and external function does not exists */
      result = "unknown function [amu_" + fx_name + "].";
    }
  }

  if ( success )
  {
    // output result to scanner or store to global variable map
    if ( fx_var.length() == 0 )
      scanner_output( result );
    else
    {
      gevm.store(fx_var, result);

      filter_debug( fx_var + "=[" + result + "]" );
    }
  }
  else
  {
    scanner_output( amu_error_msg(result) );
  }

  // output blank lines to maintain file length when functions are
  // broken across multiple lines (don't begin and end on the same line).
  for(size_t i=fx_bline; i<fx_eline; i++) scanner_output("\n");
}

void
ODIF::ODIF_Scanner::fx_set_var(void)
{
  if ( fx_var.length() )
    error("previously defined var: " + fx_var, lineno(), YYText());
  fx_var = YYText();
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
  fx_argv.store( mt.substr(levm.get_escape_prefix_length(),mt.length()) );
}

void
ODIF::ODIF_Scanner::fx_app_qarg_escaped(void)
{
  // remove 'escape-prefix' from variable name in matched text (first character)
  string mt = YYText();
  fx_qarg+=mt.substr(levm.get_escape_prefix_length(),mt.length());
}

/***************************************************************************//**

  \details

    Function arguments may use pre and post <tt>'++'</tt> and/or
    <tt>'\-\-'</tt> operations, where post have global scope and pre
    have scope local to the function where they are defined. Prefix a
    flag with <tt>++</tt> to enable it and <tt>\-\-</tt> to disable it.
    For example \p ++name enables \p name and \p \-\-name disables it.

     operation | description
    :---------:|:-------------------------------------------------------
      x++      | increment global variable
      ++x      | increment local variable
      y=x++    | assign function argument ${x} then increment global x
      y=++x    | increment local x then assign function argument ${x}

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

  // get current value from local environment variable map
  long old_val = 0;
  if ( levm.exists( vn ) )
  {
    string old_val_string = levm.expand("${" + vn + "}");

    if ( UTIL::is_number( old_val_string ) )
      old_val = atoi( old_val_string.c_str() );
  }

  // compute new value
  long new_val;
  if ( op.compare("++") == 0 )  new_val = old_val + 1;          // op == '++'
  else                          new_val = old_val - 1;          // op == '--'

  // store value to named argument with assignment identifier
  if ( fx_argv.get_next_name().length() )
  {
    if ( post ) fx_argv.store( UTIL::to_string(old_val) );      // id = var++
    else        fx_argv.store( UTIL::to_string(new_val) );      // id = ++var
  }

  // store value (vn=new_val)
  //  var++ : in global environment variable map
  //  ++var : in function arguments
  if ( post )   gevm.store( vn, UTIL::to_string(new_val) );     // var++
  else          fx_argv.store( vn, UTIL::to_string(new_val) );  // ++var

  // store value to local environment variable map
  levm.store( vn, UTIL::to_string(new_val) );
}

////////////////////////////////////////////////////////////////////////////////
// amu_define
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**

  \details

    Define a macro that expands to the specified text when used in other amu
    function arguments. The macro is stored to the named variable. When
    no variable is given, the text is immediately copied to the output.

*******************************************************************************/
void
ODIF::ODIF_Scanner::def_init(void)
{
  // update local copy of global variable map (memberwise)
  levm = gevm;

  apt_clear();
  apt();

  def_var.clear();
  def_text.clear();

  def_bline = lineno();
}

void
ODIF::ODIF_Scanner::def_end(void)
{
  def_eline = lineno();

  // if variable name not specified, copy definition to output
  // otherwise store in global variable map.
  if ( def_var.length() == 0 )
    scanner_output( def_text );
  else
  {
    gevm.store( def_var, def_text );

    filter_debug( def_var + "=[" + def_text + "]" );
  }

  // output blank lines to maintain file length when definitions are
  // broken across multiple lines (don't begin and end on the same line).
  for(size_t i=def_bline; i<def_eline; i++) scanner_output("\n");
}

void
ODIF::ODIF_Scanner::def_set_var(void)
{
  if ( def_var.length() )
    error("previously defined var: " + def_var, lineno(), YYText());
  def_var=YYText();
}

////////////////////////////////////////////////////////////////////////////////
// amu_if
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**

  \details

    Output conditional text when an expression evaluates to true.

    \code
    \amu_if <variable> ( expression )
    {
      conditional text
    }
    else if ( expression )
    {
      conditional text
    }
    else if ( expression )
      ${variable}
    else
    {
      unconditional text
    }
    endif
    \endcode

    White space between an \c else and \c if is optional. \c elseif and
    \c else \c if are equivalent.

    Expression value constants are case insensitive:

     value true  | value false
    :-----------:|:-----------:
     true        | false
     t           | f
     1           | 0

    Expressions may be combined as follows:

    \verbatim
    ( expression )
       Returns  the  value of expression.  This may be used to
       override the normal precedence of operators.
    ! expression
       True if expression is false.
    expression1 && expression2
       True if both expression1 and expression2 are true.
    expression1 || expression2
       True if either expression1 or expression2 is true.
    \endverbatim

    The && and || operators evaluate expression2 even if the value of
    expression1 is sufficient to determine the return value of the
    entire conditional expression.

    Expression can be composed using several built-in tests.

    \b Example:
    \code
    \amu_if ( -z ${error_test} )
    {
      The error test output is empty.
    }
    elseif ( ${error_count} > 0 )
    {
      The error count is ${error_count}
    }
    else
    {
      No reportable errors.
    }
    endif
    \endcode

*******************************************************************************/
void
ODIF::ODIF_Scanner::if_init(void)
{
  // update local copy of global variable map (memberwise)
  levm = gevm;

  apt_clear();
  apt();

  if_var.clear();
  if_text.clear();

  if_matched = false;
  if_case_true = false;
  if_else_true = false;

  if_bline = lineno();
}

void
ODIF::ODIF_Scanner::if_init_case(bool expr)
{
  // previous else with no expression must be last case
  if ( if_else_true )
    error("case after else", lineno(), YYText());

  // clear case body text
  if_case_text.clear();
  if_case_level = 0;

  // clear operation and value stack
  while ( ! if_opr.empty() )  if_opr.pop();
  while ( ! if_val.empty() )  if_val.pop();

  if ( expr )
  {
    if_opr.push( '(' );     // start new if case expression
    if_case_true = false;
  }
  else if ( !if_matched )
  {
    if_else_true = true;    // select else case
  }
}

void
ODIF::ODIF_Scanner::if_eval_expr(void)
{
  bool end_expr = false;

  while ( !if_opr.empty() && !end_expr )
  {
    char op = if_opr.top();

    switch( op )
    {
      case '(' :
        // group ( expression )
        end_expr = true;
        break;

      case '!' :
        // negate: ! expression
        if_val.top() = ! if_val.top();
        break;

      case '&' :
        // and: expression1 && expression2
      case '|' :
        //  or: expression1 || expression2
        bool a1 = false;
        bool a2 = false;

        if (if_val.empty())
          error("missing value 1 for " + string(op=='&'?"&&":"||"), lineno(), YYText());
        else
        {
          a1 = if_val.top();
          if_val.pop();
        }

        if (if_val.empty())
          error("missing value 2 for " + string(op=='&'?"&&":"||"), lineno(), YYText());
        else
        {
          a2 = if_val.top();
          if_val.pop();
        }

        if ( op == '&' )
          if_val.push( a1 && a2 );
        else
          if_val.push( a1 || a2 );
        break;
    }

    if_opr.pop();
  }

  if ( if_opr.empty() )
  { // operation stack is empty result is top value.
    if ( if_val.size() == 1 )
      if_case_true = if_val.top();
    else
      error("expression error", lineno(), YYText());
  }
}

void
ODIF::ODIF_Scanner::if_end_case(void)
{
  if ( !if_matched  && (if_case_true || if_else_true) )
  {
    if_matched = true;

    // expand case body text and assign as result
    if_text = levm.expand_text( if_case_text );
  }
}

void
ODIF::ODIF_Scanner::if_end(void)
{
  if_eline = lineno();

  // if variable name not specified, copy definition to output
  // otherwise store in global variable map.
  if ( if_var.length() == 0 )
    scanner_output( if_text );
  else
  {
    gevm.store( if_var, if_text );

    filter_debug( if_var + "=[" + if_text + "]" );
  }

  // output blank lines to maintain file length when definitions are
  // broken across multiple lines (don't begin and end on the same line).
  for(size_t i=if_bline; i<if_eline; i++) scanner_output("\n");
}

void
ODIF::ODIF_Scanner::if_set_var(void)
{
  if ( if_var.length() )
    error("previously defined var: " + if_var, lineno(), YYText());
  if_var=YYText();
}

////////////////////////////////////////////////////////////////////////////////
// amu_include
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**

  \details

    Include a file into the current input stream, switch and process it
    until its end, and then return to resume and complete the current.

     options    | sc  | description
    :----------:|:---:|:--------------------------------------
     copy       | c   | copy the file contents to the output
     no_switch  | n   | do not switch the input stream
     search     | s   | search include paths for file

    The options change the commands default behavior and are useful
    during development and debugging. When using these options, they
    are placed after the include command and before the opening
    parentheses as shown in the example.

    \b Example:
    \code
    \amu_include ( ${root}/path/file )

    \amu_include copy no_switch ( ${root}/path/debuging )
    \endcode

*******************************************************************************/
void
ODIF::ODIF_Scanner::inc_init(void)
{
  // update local copy of global variable map (memberwise)
  levm = gevm;

  apt_clear();
  apt();

  inc_text.clear();

  inc_copy = false;
  inc_switch = true;
  inc_search = false;

  inc_bline = lineno();
}

void
ODIF::ODIF_Scanner::inc_end(void)
{
  inc_eline = lineno();

  // unquote and expand variables in argument text
  string file_arg = levm.expand_text( UTIL::unquote_trim( inc_text ) );

  string file_inc;

  if ( inc_search )
  { // search for include file
    bool found = false;
    file_inc = file_rl( file_arg, NO_FORMAT_OUTPUT, found );

    if ( ! found  )
      abort( "unable to find file" , lineno(), file_arg );
  }
  else
  {  // use named argument
    file_inc = file_arg;
  }

  // copy file to output
  if ( inc_copy )
  {
    ifstream ifs( file_inc.c_str() );

    if ( ! ifs.is_open() )
      abort( "unable to read file" , lineno(), file_arg );

    string line;
    while ( !ifs.eof() )
    {
      getline( ifs, line );
      scanner_output( line + "\n" );
    }

    ifs.close();
  }

  // switch stream to file
  if ( inc_switch )
    start_file( file_inc );

  // output blank lines to maintain file length when definitions are
  // broken across multiple lines (don't begin and end on the same line).
  for(size_t i=inc_bline; i<inc_eline; i++) scanner_output("\n");
}

////////////////////////////////////////////////////////////////////////////////
// nested comment block
////////////////////////////////////////////////////////////////////////////////

void
ODIF::ODIF_Scanner::nc_init(void)
{
  nc_bline = lineno();
}

void
ODIF::ODIF_Scanner::nc_end(void)
{
  nc_eline = lineno();

  // output blank lines to maintain file length when definitions are
  // broken across multiple lines (don't begin and end on the same line).
  for(size_t i=nc_bline; i<nc_eline; i++) scanner_output("\n");
}

////////////////////////////////////////////////////////////////////////////////
// utility
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**

  \param m          debug message.
  \param h          start verbatim block.
  \param f          end verbatim block.
  \param s          output to scanner.

  \details

    Output the message \p m to the console via stderr. The message is
    also copied to the scanner output when \p s is \b true.

*******************************************************************************/
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
    // output to scanner
    if ( s )
    {
      // start new header verbatim block
      if ( h )
      {
        scanner_output( "\n\\if __INCLUDE_FILTER_DEBUG__\n" );
        scanner_output( "\\verbatim\n" );
      }

      // output message
      scanner_output( "(line " + UTIL::to_string( lineno() ) + ") " + m );
      scanner_output( "\n" );

      // end header verbatim block
      if ( f )
      {
        scanner_output( "\\endverbatim\n" );
        scanner_output( "\\endif\n" );
      }
    }

    // output to console via stderr
    cerr << ops << "(line " << dec << lineno() << ") " << m << endl;
  }
}

/***************************************************************************//**

  \param file       file to locate.
  \param subdir     destination subdirectory for file copy.
  \param found      status of if the file was located.
  \param extension  return file name with file extension.
  \param copy       copy the found file to the specified subdir.
  \param rid        rename the copied file with an random identifier.

  \returns  a string with (a) \p file when not located, (b) path to the
            target file when located and copied, or (c) path to the source
            file when located and not copied.

  \details

    Attempt to locate the named \p file in each include path. A check is
    performed (1) without and (2) with the files' path prefix in each include
    path successively. If the file is not located then \p found = \p false and
    the original \p file string is returned.

    When located, \p found = \p true, the file is copied to the \p subdir
    (if \p copy == \p true), and the path to the target file is returned.
    If \p rid == \p true, then the target file name is assigned a random
    target file name. When \p copy == \p false, the located source file
    path is returned. For located files, the file reference can be returned
    with or without a file name extension as indicated by the \p extension
    parameter.

    When \p subdir == \p ODIF::NO_FORMAT_OUTPUT, a copy will not be
    performed for any located file regarded of the parameter \p copy.

  \todo Only search for local files. Files that match the pattern of a
        remote URL should be returned immediately.

*******************************************************************************/
string
ODIF::ODIF_Scanner::file_rl(
  const string& file,
  const string& subdir,
        bool& found,
  const bool& extension,
  const bool& copy,
  const bool& rid
)
{
  bfs::path file_path ( file );
  bfs::path file_found;

  found = false;

  //
  // search include paths for file (test if file has a parent path).
  //
  if ( file_path.has_parent_path() )
  {
    // has parent path, check each include path in reverse order.
    filter_debug("locate path [" + file + "]", true, false, false);

    for( vector<string>::reverse_iterator it = include_path.rbegin();
                                          it != include_path.rend() && !found;
                                        ++it )
    {
      bfs::path p = *it / file_path;              // full filepath

      filter_debug(" checking-path: " + p.string(), false, false, false);
      if ( exists(p) && is_regular_file(p) )
      {
        found = true;
        file_found = p;

        break;
      }
    }
  }
  else
  {
    // has no parent path, check each include path in order.
    filter_debug("locate file [" + file + "]", true, false, false);

    for( vector<string>::iterator it = include_path.begin();
                                  it != include_path.end() && !found;
                                ++it )
    {
      bfs::path p = *it / file_path.filename();   // filename only

      filter_debug(" checking-file: " + p.string(), false, false, false);
      if ( exists(p) && is_regular_file(p) )
      {
        found = true;
        file_found = p;

        break;
      }
    }
  }

  string reference;

  //
  // found
  //
  if ( found )
  {
    bfs::path target = file_found;

    filter_debug(" found [" + file_found.string() + "]", false, false, false);

    //
    // format output not specified, disabled
    //
    if ( subdir.compare(NO_FORMAT_OUTPUT) == 0 )
    {
      filter_debug(" format output disabled.", false, false, false);
    }

    //
    // format output specified
    //
    else
    {
      bfs::path outpath  = get_doxygen_output();
                outpath /= subdir;

      filter_debug(" root output path [" + outpath.string() + "]", false, false, false);

      //
      // copy to output
      //
      if ( copy )
      {
        bfs::path source = file_found;
        bfs::path outname;

        filter_debug(" copying...", false, false, false);

        // output file name
        if ( rid )
        {
          outname  = bfs::unique_path( bfs::path("%%%%-%%%%-%%%%-%%%%-%%%%-%%%%") );
          outname += source.filename().extension();

          filter_debug(" rid [" + outname.string() + "]", false, false, false);
        }
        else
        {
          outname = source.filename();
        }

        // target prefix path relative to outpath
        bfs::path prefix = UTIL::get_relative_path(source.parent_path(), outpath, true);

        if ( ! prefix.empty() )
        {
          filter_debug(" adding target prefix [" + prefix.string() + "]", false, false, false);

          // create output directory prefix path(s)
          std::string m;
          if ( UTIL::make_dir(prefix.string(), m, true, outpath.string()) )
            filter_debug(" " + m, false, false, false);

          target = outpath / prefix / outname;
        }
        else
        {
          target = outpath / outname;
        }

        filter_debug(" target [" + target.string() + "]", false, false, false);

        bool copy_needed = true;

        // skip copy when file of same size exists.
        if ( exists(target) && is_regular_file(target) )
        {
          if ( bfs::file_size( source ) == bfs::file_size( target ) )
          {
            copy_needed = false;

            filter_debug("  same sized target exists.", false, false, false);
          }
          else
          { // differ in size, remove.
            bfs::remove( target );
          }
        }

        // could skip copy when source path is subdirectory of output path
        // ie: (outpath / get_relative_path(source, outpath) == source)

        if ( copy_needed )
        {
          // copy source to target
          bfs::copy_file( source, target, bfs::copy_option::overwrite_if_exists );

          filter_debug(" copy done.", false, false, false);
        }

        // remake return reference to target path relative to parent outpath
        // remove outpath from: target = outpath / prefix / outname
        target = UTIL::get_relative_path(target, outpath, true);

        filter_debug(" make target reference relative to parent output path.", false, false, false);
      }

      //
      // do not copy to output
      //
      else
      {
        filter_debug(" copy disabled.", false, false, false);

        // remake return reference to target path relative to outpath
        // with full return to common parent via successive '../'
        target = UTIL::get_relative_path(target, outpath, false);

        filter_debug(" make target reference relative to output path.", false, false, false);
      }

    } // output specified

    //
    // reference returned with or without extension
    //
    if ( extension )
      reference = target.string();
    else
      reference = target.stem().string();
  }

  //
  // not found
  //
  else
  {
    filter_debug(" not found.", false, false, false);

    reference = file;
  }

  filter_debug(" --> reference [" + reference + "]", false, true, false);

  return( reference );
}


/*******************************************************************************
// eof
*******************************************************************************/
