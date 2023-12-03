/***************************************************************************//**

  \file   openscad_dif_scanner_bif5.cpp

  \author Roy Allen Sutton
  \date   2018-2019

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
#include <set>

// #include <boost/filesystem.hpp>

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

using namespace std;
namespace bfs = boost::filesystem;


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

/***************************************************************************//**
  \details

    Run an in-line OpenSCAD script.

    This function uses the following form:

    \code
    \amu_NAME <variable> ( arguments ) { script }
    \endcode

    The in-line script is copied to the specified file and compiled
    using OpenSCAD with the supplied command line arguments. When no
    file name is provided a unique temporary file name is generated and
    subsequently removed (default) after compilation.

    The options and flags (and their short codes) are summarized in the
    following tables.

    Named arguments:

     options          | sc  | default                                     | description
    :----------------:|:---:|:--------------------------------------------|:------------------------
      file            | f   | <auto-temp>                                 | script file name
      args            | a   |                                             | command line arguments
      ...             |     |                                             | ...
      format_command  | fx  | "2,\code,\endcode,4,<b>Command</b>"         | command output format
      format_script   | fp  | "2,\code{.C},\endcode,4,<b>Script</b>"      | script output format
      format_console  | fc  | "2,\verbatim,\endverbatim,4,<b>Output</b>"  | console output format

    Text-block indentation affects the format behavior of Doxygen.
    Indentation, formatting, and titles can be controlled for each
    output type using a format string of comma separated values as
    defined below:

    \verbatim
    "<output-indent>, <pre-output>, <post-output>, <title-indent>, <title-text>"
    \endverbatim

    The '-' character can be used to indicate that a field should keep
    its default value shows above.

    \b Examples:
    \verbatim
    format_console=""                         // no indentation, formatting, or title
    format_console=",,,,Result"               // no indentation or formatting with title
    format_console="2"                        // indentation specified with no formatting or title
    format_console="-,\code{.C},\endcode,-,-" // keep defaults except for the pre and post-output
    format_console="-,-,-,-,-"                // default all
    \endverbatim

    Flags that modify the output:

     flags      | sc  | default | description
    :----------:|:---:|:-------:|:-----------------------------------------
      rmecho    | o   | false   | remove OpenSCAD quoted [ECHO: "..."]
      rmfile    | d   | true    | remove script file after compilation
      shfile    | sf  | false   | show script file with command arguments
      shbin     | sb  | true    | show openscad bin with command arguments
      debug     | g   | false   | include command debug infomation

    Flags that produce output:

     flags      | sc  | default | description
    :----------:|:---:|:-------:|:-----------------------------------------
      command   | x   | false   | output command line arguments
      script    | p   | false   | output in-line script
      console   | c   | false   | output script console output

    \b Example:
    \code{.C}
    \amu_define  file_path_stem (${DOXYGEN_OUTPUT}${HTML_OUTPUT}/${OUTPUT_PREFIX}/${ext}/${STEM_NAME}_${name})
    \amu_eval get_png_file_stem (name=test ext=png ${file_path_stem})

    \amu_openscad
    (
      args="
      --D m=\\"cone\\"
      --D n=\'\"a cone\"\'
      --D r=\'[4, 1]\'
      --D h=8
      --D f=30
      --D d=\'[\"cone\", \"a cone\", [4, 1], 8, 30]\'
      --D e=\'[m, n, r, h, f]\'
      --D c=true
      --autocenter --viewall --imgsize=320,240 --view=axes --projection=o
      --o ${get_png_file_stem}.png
      "

      ++command ++script ++console
    )
    {
      // in-line script demo

      cylinder( r1=r[0], r2=r[1], h=h, $fn=f );

      if ( c == true )
      {
        echo(m=m, n=n, r=r, h=h, f=f);
        echo(d=d);
        echo(e=e);
      }
    }

    \amu_image (c="A Cone" f="${get_png_file_stem}.png")
    \endcode

    Comment blocks may not be used within the in-line script. Doxygen
    searches for file, examples, images, etc., before it processes the
    source input files. Therefore, image, and other, files generated by
    in-line OpenSCAD scripts will only be found during subsequent
    Doxygen processing passes. If images are not showing in the
    documentation, run Doxygen once again after the image has been
    generated by the \\amu_openscad() function. Be sure the image,
    example, etal., paths are properly set in the doxygen configuration
    file.

    For more information on how to specify and use function arguments
    see \ref openscad_dif_sm_a.
*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_openscad(void)
{
  using namespace UTIL;

  // options declaration: vana & vans.
  // !!DO NOT REORDER WITHOUT UPDATING POSITIONAL DEPENDENCIES BELOW!!
  string vana[] =
  {
  "file",             "f",
  "args",             "a",

  "format_command",   "fx",
  "format_script",    "fp",
  "format_console",   "fc",

  "rmecho",           "o",
  "rmfile",           "d",
  "shfile",           "sf",
  "shbin",            "sb",
  "debug",            "g",

  "command",          "x",
  "script",           "p",
  "console",          "c"
  };
  set<string> vans(vana, vana + sizeof(vana)/sizeof(string));

  // output format defaults
  string fmt_cmd_def = "2,\\code,\\endcode,4,<b>Command</b>";
  string fmt_scr_def = "2,\\code{.C},\\endcode,4,<b>Script</b>";
  string fmt_con_def = "2,\\verbatim,\\endverbatim,4,<b>Output</b>";

  // assign local variable values: positions must match declaration above.
  size_t ap=0;
  string file           = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string args           = unquote     (fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;

  string fmt_cmd        = unquote     (fx_argv.arg_firstof(fmt_cmd_def,vana[ap],vana[ap+1])); ap+=2;
  string fmt_scr        = unquote     (fx_argv.arg_firstof(fmt_scr_def,vana[ap],vana[ap+1])); ap+=2;
  string fmt_con        = unquote     (fx_argv.arg_firstof(fmt_con_def,vana[ap],vana[ap+1])); ap+=2;

  bool rmecho   = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool rmfile   = ( atoi( unquote_trim(fx_argv.arg_firstof("1",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool shfile   = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool shbin    = ( atoi( unquote_trim(fx_argv.arg_firstof("1",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool debug    = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;

  bool command  = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool script   = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool console  = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;

  // generate options help string.
  string help = "options: [";
  for(size_t it=0; it < ap; it+=2) {
    if (it) help.append( ", " );
    help.append( vana[it] + " (" + vana[it+1] + ")" );
  }
  help.append( "]" );

  // validate named arguments: (must be one of the declared options).
  vector<string> av = fx_argv.names_v(true, false);
  for ( vector<string>::iterator it=av.begin(); it!=av.end(); ++it )
    if ( vans.find( *it ) == vans.end() )
      return( amu_error_msg(*it + " invalid option. " + help) );

  //
  // general argument validation:
  //

  // enforce zero positional arguments (except arg0).
  if ( fx_argv.size(false, true) != 1 )
    return(amu_error_msg("requires zero positional argument. " + help));

  // start debug
  filter_debug( "amu_" + fx_argv.arg(0) + " begin.", true, false );

  // when file is not specified create unique temporary file name
  if ( file.empty() )
  {
    bfs::path temp_file;
    temp_file  = "/tmp";
    temp_file /= gevm.expand(gevm.get_prefix() + "STEM_NAME" + gevm.get_suffix());
    temp_file += bfs::unique_path( bfs::path("-%%%%-%%%%") );
    temp_file += get_openscad_ext();

    file = temp_file.string();

    filter_debug( "temp_file: " + file, false, false );
  }

  // create script file
  ofstream ofs ( file.c_str() );
  if ( ofs.is_open() )
  {
    ofs << fx_body_text;
    ofs.close();

    filter_debug( "file created...", false, false );
  }
  else
  {
    return( amu_error_msg( "unable to write in-line script to: " + file ) );
  }

  // construct system command
  string command_string;

  // remove newlines
  command_string = replace_chars(get_openscad_path() + " " + args + " " + file, "\n\r", ' ');

  filter_debug( "issuing command: " + command_string, false, false );

  string command_output;
  bool command_good = false;

  // issue system command
  UTIL::sys_command( command_string, command_output, command_good, true, false );

  filter_debug( "command return: " + string(command_good?"ok":"fail"), false, false );

  // remove script file
  if ( rmfile )
  {
    if ( bfs::exists(file) && bfs::is_regular_file(file) )
    {
      filter_debug( "removing file: " + file, false, false );

      bfs::remove(file);
    }
  }

  // remove script first in case of return on error
  if ( !command_good )
    return( amu_error_msg( "unable to execute command: " + command_string ) );

  // remove OpenSCAD quoted [ECHO: "..."]
  if ( rmecho )
  {
    filter_debug( "removing output echo...", false, false );

    command_output = openscad_rmecho_text( command_output );
  }

  // remove escape character before debug output
  filter_debug( "format_command [" + replace_chars(fmt_cmd,"\\", 'X') + "]", false, false );
  filter_debug( " format_script [" + replace_chars(fmt_scr,"\\", 'X') + "]", false, false );
  filter_debug( "format_console [" + replace_chars(fmt_con,"\\", 'X') + "]", false, false );

  // start results on newline
  string result = "\n";

  // include command debug infomation
  if ( debug )
  {
    result += indent_text("\n\\verbatim\n" + command_string + "\n\\endverbatim\n\n", 4);
  }

  // append command line arguments to result
  if ( command )
  {
    int    oi = atoi( get_field( 0, fmt_cmd, fmt_cmd_def ).c_str() );
    string po =       get_field( 1, fmt_cmd, fmt_cmd_def );
    string oo =       get_field( 2, fmt_cmd, fmt_cmd_def );
    int    ti = atoi( get_field( 3, fmt_cmd, fmt_cmd_def ).c_str() );
    string tt =       get_field( 4, fmt_cmd, fmt_cmd_def );

    // indent path to same indentation as 'args'
    string openscad = indent_line( get_openscad_path(), get_indent(args) );

    if ( !tt.empty() ) { result += indent_line(tt + "\n", ti); }
    if ( !po.empty() ) { result += indent_line(po + "\n", ti); }

                         result += indent_text
                                   (
                                       (shbin?(openscad + " "):"")
                                     + args
                                     + (shfile?(" " + file):""), oi
                                   );

    if ( !oo.empty() ) { result += indent_line(oo + "\n", ti); }
  }

  // append in-line script to result
  if ( script )
  {
    int    oi = atoi( get_field( 0, fmt_scr, fmt_scr_def ).c_str() );
    string po =       get_field( 1, fmt_scr, fmt_scr_def );
    string oo =       get_field( 2, fmt_scr, fmt_scr_def );
    int    ti = atoi( get_field( 3, fmt_scr, fmt_scr_def ).c_str() );
    string tt =       get_field( 4, fmt_scr, fmt_scr_def );

    if ( !tt.empty() ) { result += indent_line(tt + "\n", ti); }
    if ( !po.empty() ) { result += indent_line(po + "\n", ti); }

                         result += indent_text(fx_body_text, oi);

    if ( !oo.empty() ) { result += indent_line(oo + "\n", ti); }
  }

  // append script console output to result
  if ( console )
  {
    int    oi = atoi( get_field( 0, fmt_con, fmt_con_def ).c_str() );
    string po =       get_field( 1, fmt_con, fmt_con_def );
    string oo =       get_field( 2, fmt_con, fmt_con_def );
    int    ti = atoi( get_field( 3, fmt_con, fmt_con_def ).c_str() );
    string tt =       get_field( 4, fmt_con, fmt_con_def );

    if ( !tt.empty() ) { result += indent_line(tt + "\n", ti); }
    if ( !po.empty() ) { result += indent_line(po + "\n", ti); }

                         result += indent_text(command_output, oi);

    if ( !oo.empty() ) { result += indent_line(oo + "\n", ti); }
  }

  // end debug
  filter_debug( "amu_" + fx_argv.arg(0) + " end.", false, true );

  return( result );
}

/***************************************************************************//**
  \details

    Perform file system operations.

    The options and flags (and their short codes) are summarized in the
    following tables.

    Named arguments:

     options    | sc  | default | description
    :----------:|:---:|:-------:|:----------------------------------------------
      parent    | p   | <empty> | a parent path
      mkrel     | mr  |         | return each path relative to parent path
      mkrelp    | mrp |         | return each path relative to parent path (common)
      mkdir     | md  |         | make one or more directories
      mkdirp    | mdp |         | make directory paths including parents

    Flags that produce output:

     flags      | sc  | default | description
    :----------:|:---:|:-------:|:----------------------------------------
      uuid      | u   | false   | return a unique identifier

    Flags that modify output:

     flags      | sc  | default | description
    :----------:|:---:|:-------:|:----------------------------------------
      verbose   | v   | false   | use verbose output

    The tokenizer character that separates list members are summarized
    in the following table.

     type     | any of
    :--------:|:------------:
     paths    | [,[:space:]]

    For more information on how to specify and use function arguments
    see \ref openscad_dif_sm_a.
*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_filesystem(void)
{
  using namespace UTIL;

  // options declaration: vana & vans.
  // !!DO NOT REORDER WITHOUT UPDATING POSITIONAL DEPENDENCIES BELOW!!
  string vana[] =
  {
  "verbose",  "v",

  "parent",   "p",
  "mkrel",    "mr",
  "mkrelp",   "mrp",
  "mkdir",    "md",
  "mkdirp",   "mdp",

  "uuid",     "u"
  };
  set<string> vans(vana, vana + sizeof(vana)/sizeof(string));

  // assign local variable values: positions must match declaration above.
  size_t ap=14;
  bool verbose  = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[0],vana[1])).c_str() ) > 0 );
  bfs::path parent;

  // tokenizer delimiters
  string toks = ", ";

  // generate options help string.
  string help = "options: [";
  for(size_t it=0; it < ap; it+=2) {
    if (it) help.append( ", " );
    help.append( vana[it] + " (" + vana[it+1] + ")" );
  }
  help.append( "]" );

  string result;

  // iterate over the arguments, skipping function name (position zero)
  for ( vector<func_args::arg_term>::iterator it=fx_argv.argv.begin()+1;
                                              it!=fx_argv.argv.end();
                                              ++it )
  {
    string n = it->name;
    string v = unquote_trim(it->value);
    bool flag = ( atoi( v.c_str() ) > 0 );   // assign flag value

    if ( it->positional )
    { // invalid
      return( amu_error_msg(n + "=" + v + " invalid option. " + help) );
    }
    else
    {
      if      (
                !(n.compare(vana[0])&&n.compare(vana[1]))     // verbose
              )
      {
        // do nothing, control flags values set above.
      }
      else if (!(n.compare(vana[2])&&n.compare(vana[3])))
      { // parent
        parent = v;
      }
      else if (!(n.compare(vana[4])&&n.compare(vana[5])))
      { // mkrel
        vector<string> vv = string_tokenize_to_vector(v, toks);

        for ( vector<string>::iterator it=vv.begin(); it != vv.end(); ++it)
        {
          bfs::path p = *it;

          result.append
          (
            (result.empty()?"":",")
            + get_relative_path(p, parent, false).string()
          );
        }
      }
      else if (!(n.compare(vana[6])&&n.compare(vana[7])))
      { // mkrelp
        vector<string> vv = string_tokenize_to_vector(v, toks);

        for ( vector<string>::iterator it=vv.begin(); it != vv.end(); ++it)
        {
          bfs::path p = *it;

          result.append
          (
            (result.empty()?"":",")
            + get_relative_path(p, parent, true).string()
          );
        }
      }
      else if (!(n.compare(vana[8])&&n.compare(vana[9])))
      { // mkdir
        vector<string> vv = string_tokenize_to_vector(v, toks);

        for ( vector<string>::iterator it=vv.begin(); it != vv.end(); ++it)
        {
          bfs::path p;

          if ( parent.empty() ) p = *it;
          else                  p = parent / *it;

          std::string m;
          bool ok = UTIL::make_dir(p.string(), m, false);

          if ( verbose || !ok ) result.append( (result.empty()?"":", ") + m );
        }
      }
      else if (!(n.compare(vana[10])&&n.compare(vana[11])))
      { // mkdirp
        vector<string> vv = string_tokenize_to_vector(v, toks);

        for ( vector<string>::iterator it=vv.begin(); it != vv.end(); ++it)
        {
          bfs::path p;

          if ( parent.empty() ) p = *it;
          else                  p = parent / *it;

          std::string m;
          bool ok = UTIL::make_dir(p.string(), m, true);

          if ( verbose || !ok ) result.append( (result.empty()?"":", ") + m );
        }
      }

      else if (!(n.compare(vana[12])&&n.compare(vana[13])) && flag)
      { // uuid
        result.append
        (
          (result.empty()?"":" ")
          + bfs::unique_path( bfs::path("%%%%-%%%%-%%%%-%%%%-%%%%-%%%%") ).string()
        );
      }

      else
      { // invalid
        return( amu_error_msg(n + "=" + v + " invalid option. " + help) );
      }
    }
  }

  return ( result );
}

/*******************************************************************************
// eof
*******************************************************************************/
