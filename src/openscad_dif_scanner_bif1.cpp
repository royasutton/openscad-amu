/***************************************************************************//**

  \file   openscad_dif_scanner_bif1.cpp

  \author Roy Allen Sutton
  \date   2016-2018

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

    Create a local copy of the global variable scope and process each
    argument one-by-one.

    \em Named argument value pairs are stored to the local variable
    scope by default. A \em flag arguments will store a value of 1 or
    -1 (ie: ++i will store i=1). The special flags \p ++/--global and
    \p ++/--local control if the respective variable scope will be
    updated for subsequent assignments.

     flags              | default | description
    :-------------------|:-------:|:------------------------------
     ++local, --local   | true    | update local variable scope
     ++global, --global | false   | update global variable scope

    \em Positional arguments are expanded and appended to the return
    value. Each argument is separated by a configurable feild
    separator, ${EFS}. By default, ${EFS}='[:space:]'. Escaped
    variables are reduced and checked during subsequent expansions.
    Delayed expansion is required to access the value of a variable
    stored using the \em flag syntax.

    The result is returned when there are no more named or positional
    arguments.

    For more information on how to specify and use function arguments
    see \ref openscad_dif_sm_a.

*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_eval(void)
{
  bool update_local = true;
  bool update_global = false;

  // setup feild separator variable reference
  string efs = levm.get_prefix() + "EFS" + levm.get_suffix();

  string result;

  // iterate over the arguments, skipping function name (position zero)
  for ( vector<func_args::arg_term>::iterator it=fx_argv.argv.begin()+1;
                                              it!=fx_argv.argv.end();
                                              ++it )
  {
    // argument is positional
    if ( it->positional )
    {
      // append feild separator
      if ( result.length() != 0 )
        result.append( UTIL::unquote( levm.expand(efs) ) );

      // expand and append positional argument text
      // do not unquote value, quotations in positional
      // arguments assumed to have significance.
      result.append( levm.expand_text( it->value ) );
    }
    // argument is named or flag
    else
    {
      // test for flags 'global' or 'local'
      if ( it->name.compare("global") == 0  )
        update_global = ( atoi( it->value.c_str() ) > 0 );
      else if ( it->name.compare("local") == 0  )
        update_local = ( atoi( it->value.c_str() ) > 0 );
      else
      // add name=value pair to variable map
      // unquote value before storing. no need to expand
      // text as this is done recursively by class 'env_var'.
      {
        // local
        if ( update_local ) levm.store( it->name, UTIL::unquote( it->value ) );

        // global
        if ( update_global ) gevm.store( it->name, UTIL::unquote( it->value ) );
      }
    }
  }

  return( result );
}

/***************************************************************************//**

  \details

    Execute the string at the first positional argument in a shell. The
    output generated by the command is captured to a string, striped of
    all return and linefeed characters and returned.

    The flags and their short codes are summarized in the following
    table.

     flags   | sc  | default | description
    :-------:|:---:|:-------:|:-----------------------------------------
      stderr | s   | false   | capture standard error output as well
      rmnl   | r   | true    | remove line-feeds / carriage returns
      eval   | e   | false   | expand variables in text

    For more information on how to specify and use function arguments
    see \ref openscad_dif_sm_a.

*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_shell(void)
{
  // options declaration: vana & vans.
  // !!DO NOT REORDER WITHOUT UPDATING POSITIONAL DEPENDENCIES BELOW!!
  string vana[] =
  {
  "stderr",   "s",
  "rmnl",     "r",
  "eval",     "e"
  };
  set<string> vans(vana, vana + sizeof(vana)/sizeof(string));

  // declare and assign flag defaults
  size_t ap=0;
  bool flag_stde = false; ap+=2;
  bool flag_rmnl = true;  ap+=2;
  bool flag_eval = false; ap+=2;

  // generate options help string.
  string help = "flags: [";
  for(size_t it=0; it < ap; it+=2) {
    if (it) help.append( ", " );
    help.append( vana[it] + " (" + vana[it+1] + ")" );
  }
  help.append( "]" );

  // assign local variable flags:
  vector<string> av = fx_argv.names_v(true, false);
  for ( vector<string>::iterator it=av.begin(); it!=av.end(); ++it )
  {
    bool found;

    if      (!(it->compare(vana[0])&&it->compare(vana[1])))
      flag_stde=( atoi(fx_argv.arg(*it, found).c_str()) > 0 );
    else if (!(it->compare(vana[2])&&it->compare(vana[3])))
      flag_rmnl=( atoi(fx_argv.arg(*it, found).c_str()) > 0 );
    else if (!(it->compare(vana[4])&&it->compare(vana[5])))
      flag_eval=( atoi(fx_argv.arg(*it, found).c_str()) > 0 );
    else
      return( amu_error_msg(*it + " invalid flag. " + help) );
  }

  //
  // general argument validation:
  //

  // enforce one positional argument (except arg0).
  if ( fx_argv.size(false, true) != 2 )
    return(amu_error_msg("requires a single positional argument. " + help));

  // unquote and trim the command string
  string scmd = UTIL::unquote_trim( fx_argv.arg( 1 ) );

  string result;
  bool good=false;

  filter_debug( scmd );
  UTIL::sys_command( scmd, result, good, flag_stde, flag_rmnl );

  if ( good == false )
  {
    return( amu_error_msg( result) );
  }
  else
  {
    if (flag_eval)
      result = levm.expand_text(result);

    return( result );
  }
}

/***************************************************************************//**

  \details

    Run \em make on an embedded MFScript using a specified target and
    capture the result.

    The scope identifier is initially the default \em root scope
    (specified by the program option set on the command line or in a
    configuration file). Each argument is then processes to modify the
    scope identifier according to the description in the following table.
    All positional arguments are treated as \p append options.

     options        | sc  | default      |  description
    :--------------:|:---:|:------------:|:------------------------------------
      set           | si  |              | set the scope identifier to argument
      append        | a   |              | append argument to scope identifier
      prepend       | p   |              | prepend argument to scope identifier
      extension     | e   |              | limit targets by extension
      target_prefix | tp  | echo_targets | make target prefix

    Command flags and result filtering.

     flags     | sc  | default | description
    :---------:|:---:|:-------:|:-----------------------------------------
      stderr   | s   | false   | capture standard error output as well
      rmnl     | r   | true    | remove line-feeds / carriage returns
      pstarget | pst | false   | target is from parent source file

    A makefile build script can generate targets from either a scope
    embedded script or the the parent source file. The flag \p pstarget
    is used to distinguish between these origins. Setting \p ++pstarget
    will use targets generated by the parent source file.

  \note Under normal operation, the output text from the makefile
        target should be contained within a single line so as to not
        change the line count of the source file.

    For more information on how to specify and use function arguments
    see \ref openscad_dif_sm_a.

*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_make(void)
{
  // options declaration: vana & vans.
  // !!DO NOT REORDER WITHOUT UPDATING POSITIONAL DEPENDENCIES BELOW!!
  string vana[] =
  {
  "set",              "si",
  "append",           "a",
  "prepend",          "p",
  "extension",        "e",
  "target_prefix",    "tp",
  "stderr",           "s",
  "rmnl",             "r",
  "pstarget",         "pst"
  };
  set<string> vans(vana, vana + sizeof(vana)/sizeof(string));

  size_t ap=16;

  // generate options help string.
  string help = "options: [";
  for(size_t it=0; it < ap; it+=2) {
    if (it) help.append( ", " );
    help.append( vana[it] + " (" + vana[it+1] + ")" );
  }
  help.append( "]" );

  //
  // assemble scope identifiers
  //
  string makefile_stem = get_rootscope();
  string make_target = get_rootscope();

  string target_prefix = "echo_targets";
  string mf_scopejoiner = "_";
  string target_ext;

  // flag defaults
  bool flag_stde = false;
  bool flag_rmnl = true;
  bool flag_pstarget = false;

  // iterate over the arguments, skipping function name (position zero)
  for ( vector<func_args::arg_term>::iterator it=fx_argv.argv.begin()+1;
                                              it!=fx_argv.argv.end();
                                              ++it )
  {
    string n = it->name;
    string v = it->value;

    if ( it->positional )
    { // append
      makefile_stem = makefile_stem + get_scopejoiner() + v;
      make_target = make_target + mf_scopejoiner + v;
    }
    else
    {
      if      (!(n.compare(vana[0])&&n.compare(vana[1])))
      { // set
        makefile_stem = v;
        make_target = v;
      }
      else if (!(n.compare(vana[2])&&n.compare(vana[3])))
      { // append
        makefile_stem = makefile_stem + get_scopejoiner() + v;
        make_target = make_target + mf_scopejoiner + v;
      }
      else if (!(n.compare(vana[4])&&n.compare(vana[5])))
      { // prepend
        makefile_stem = v + get_scopejoiner() + makefile_stem;
        make_target = v + mf_scopejoiner + make_target;
      }
      else if (!(n.compare(vana[6])&&n.compare(vana[7])))
      { // extension
        target_ext = v;
      }
      else if (!(n.compare(vana[8])&&n.compare(vana[9])))
      { // target_prefix
        target_prefix = v;
      }
      else if (!(n.compare(vana[10])&&n.compare(vana[11])))
      { // stderr
        flag_stde=( atoi( v.c_str() ) > 0 );
      }
      else if (!(n.compare(vana[12])&&n.compare(vana[13])))
      { // rmnl
        flag_rmnl=( atoi( v.c_str() ) > 0 );
      }
      else if (!(n.compare(vana[14])&&n.compare(vana[15])))
      { // pstarget
        flag_pstarget=( atoi( v.c_str() ) > 0 );
      }
      else
      { // invalid
        return( amu_error_msg(n + "=" + v + " invalid option. " + help) );
      }
    }
  }

  // parent source file target
  if ( flag_pstarget )
    make_target = get_rootscope();

  // target extension limiter
  if ( target_ext.length() )
    make_target = make_target + mf_scopejoiner + target_ext;

  //
  // assemble system command
  //
  string scmd;
  string opts = " --no-print-directory";

  // identify path prefix to makefile
  if ( get_prefix_scripts() )
    opts += " --directory=" + get_output_prefix();
  else if ( get_config_prefix().compare(".") && get_config_prefix().length() )
    opts += " --directory=" + get_config_prefix();

  scmd = get_make_path() + opts
       + " --makefile=" + makefile_stem + get_makefile_ext()
       + " " + target_prefix + mf_scopejoiner + make_target;

  // issue system command
  string result;
  bool good=false;

  filter_debug( scmd );
  UTIL::sys_command( scmd, result, good, flag_stde, flag_rmnl );

  if ( good )
  {
    return( result );
  }
  else
  {
    return( amu_error_msg( result ) );
  }
}

/***************************************************************************//**
  \details

    Return information about embedded scopes.

    The options and flags (and their short codes) are summarized in the
    following tables.

    Options that require arguments.

     options   | sc  | default | description
    :---------:|:---:|:-------:|:-----------------------------
      index    | i   |         | return scope at specified index

    Flags that produce output.

     flags     | sc  | default | description
    :---------:|:---:|:-------:|:-----------------------------------------
      count    | c   | false   | return scope count
      list     | l   | false   | return scope list
      join     | j   | false   | return scope joiner
      root     | r   | false   | return root scope

    Flags that control the produced output.

     flags     | sc  | default | description
    :---------:|:---:|:-------:|:-----------------------------------------
      make     | m   | true    | only consider scopes with makefiles
      sort     | s   | false   | sort scope list
      unique   | u   | true    | only consider unique scope names
      verbose  | v   | false   | use verbose output

    The \p make flag requires that the filter be run with the search
    command line option.

    For more information on how to specify and use function arguments
    see \ref openscad_dif_sm_a.

*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_scope(void)
{
  using namespace UTIL;

  // options declaration: vana & vans.
  // !!DO NOT REORDER WITHOUT UPDATING POSITIONAL DEPENDENCIES BELOW!!
  string vana[] =
  {
  "make",     "m",
  "sort",     "s",
  "unique",   "u",
  "verbose",  "v",

  "index",    "i",
  "count",    "c",
  "list",     "l",
  "join",     "j",
  "root",     "r"
  };
  set<string> vans(vana, vana + sizeof(vana)/sizeof(string));

  // assign local variable values: positions must match declaration above.
  size_t ap=18;
  bool make     = ( atoi( unquote_trim(fx_argv.arg_firstof("1",vana[0],vana[1])).c_str() ) > 0 );
  bool sort     = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[2],vana[3])).c_str() ) > 0 );
  bool unique   = ( atoi( unquote_trim(fx_argv.arg_firstof("1",vana[4],vana[5])).c_str() ) > 0 );
  bool verbose  = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[6],vana[7])).c_str() ) > 0 );

  // generate options help string.
  string help = "options: [";
  for(size_t it=0; it < ap; it+=2) {
    if (it) help.append( ", " );
    help.append( vana[it] + " (" + vana[it+1] + ")" );
  }
  help.append( "]" );

  string result;
  std::vector<std::string> sid_copy;

  // make: assign copy of scope identifiers
  if ( make )
    sid_copy = get_scope_id_mf();
  else
    sid_copy = get_scope_id();

  // sort scope identifiers
  if ( sort )
  {
    std::sort(sid_copy.begin(), sid_copy.end());
  }

  // unique: limit scope identifiers to those that are unique
  if ( unique )
  {
    std::vector<std::string> sid_temp;

    // add unique elements from copy in ordered
    for ( vector<std::string>::iterator it=sid_copy.begin();
                                        it!=sid_copy.end();
                                        ++it )
    {
      // compare against unique list
      bool found = false;
      for ( vector<std::string>::iterator uit=sid_temp.begin();
                                          uit!=sid_temp.end() && !found;
                                          ++uit )
      {
        found = ( *it == *uit );
      }

      // add if not found
      if ( !found )
        sid_temp.push_back( *it );
    }

    sid_copy = sid_temp;
  }

  // iterate over the arguments, skipping function name (position zero)
  for ( vector<func_args::arg_term>::iterator it=fx_argv.argv.begin()+1;
                                              it!=fx_argv.argv.end();
                                              ++it )
  {
    string n = it->name;
    string v = it->value;
    bool flag = ( atoi( v.c_str() ) > 0 );   // assign flag value

    if ( it->positional )
    { // invalid
      return( amu_error_msg(n + "=" + v + " invalid option. " + help) );
    }
    else
    {
      if      (
                !(n.compare(vana[0])&&n.compare(vana[1])) ||  // make
                !(n.compare(vana[2])&&n.compare(vana[3])) ||  // sort
                !(n.compare(vana[4])&&n.compare(vana[5])) ||  // unique
                !(n.compare(vana[6])&&n.compare(vana[7]))     // verbose
              )
      {
        // do nothing, control flags values set above.
      }
      else if (!(n.compare(vana[8])&&n.compare(vana[9])) && flag)
      { // index
        size_t index = atoi( v.c_str() );

        if ( index < 1 )                index = 1;
        if ( index > sid_copy.size() )  index = sid_copy.size();

        if ( result.size() ) result.append( " " );
        if ( verbose ) result.append( "index[" + to_string(index) + "] = " );

        result.append( sid_copy[ index - 1 ] );
      }
      else if (!(n.compare(vana[10])&&n.compare(vana[11])) && flag)
      { // count
        if ( result.size() ) result.append( " " );
        if ( verbose ) result.append( "count = " );

        result.append( to_string( sid_copy.size() ) );
      }
      else if (!(n.compare(vana[12])&&n.compare(vana[13])) && flag)
      { // list
        if ( verbose )
        {
          if ( result.size() ) result.append( " " );
          result.append( "list =" );
        }

        for ( vector<string>::iterator vit=sid_copy.begin();
                                       vit!=sid_copy.end();
                                       ++vit )
        {
          if ( result.size() ) result.append( " " );
          result.append( *vit );
        }
      }
      else if (!(n.compare(vana[14])&&n.compare(vana[15])) && flag)
      { // join
        if ( result.size() ) result.append( " " );
        if ( verbose ) result.append( "join = " );

        result.append( get_scopejoiner() );
      }
      else if (!(n.compare(vana[16])&&n.compare(vana[17])) && flag)
      { // root
        if ( result.size() ) result.append( " " );
        if ( verbose ) result.append( "root = " );

        result.append( get_rootscope() );
      }
      else
      { // invalid
        return( amu_error_msg(n + "=" + v + " invalid option. " + help) );
      }
    }
  }

  return ( result );
}

/***************************************************************************//**
  \details

    Return information about input file components.

    The options and flags (and their short codes) are summarized in the
    following tables.

    Flags that produce output.

     flags     | sc  | default | description
    :---------:|:---:|:-------:|:-----------------------------------------
      file     | f   | false   | return source filename
      path     | p   | false   | return source pathname
      base     | b   | false   | return source basename
      stem     | s   | false   | return source stemname
      ext      | e   | false   | return source file extension

    Flags that control the produced output.

     flags     | sc  | default | description
    :---------:|:---:|:-------:|:-----------------------------------------
      current  | c   | false   | current input file rather than initial
      absolute | a   | false   | return absolute path
      verbose  | v   | false   | use verbose output

    For more information on how to specify and use function arguments
    see \ref openscad_dif_sm_a.

*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_source(void)
{
  using namespace UTIL;

  // options declaration: vana & vans.
  // !!DO NOT REORDER WITHOUT UPDATING POSITIONAL DEPENDENCIES BELOW!!
  string vana[] =
  {
  "current",  "c",
  "absolute", "a",
  "verbose",  "v",

  "file",     "f",
  "path",     "p",
  "base",     "b",
  "stem",     "s",
  "ext",      "e"
  };
  set<string> vans(vana, vana + sizeof(vana)/sizeof(string));

  // assign local variable values: positions must match declaration above.
  size_t ap=16;
  bool current  = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[0],vana[1])).c_str() ) > 0 );
  bool absolute = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[2],vana[3])).c_str() ) > 0 );
  bool verbose  = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[4],vana[5])).c_str() ) > 0 );

  // generate options help string.
  string help = "options: [";
  for(size_t it=0; it < ap; it+=2) {
    if (it) help.append( ", " );
    help.append( vana[it] + " (" + vana[it+1] + ")" );
  }
  help.append( "]" );

  string result;

  bfs::path input_path;
  string path_type;

  if ( absolute )
  { // get absolute path
    path_type = "absolute";

    input_path = get_input_name( !current );
  }
  else
  { // get relative path
    path_type = "relative";

    input_path = get_input_name( !current );
    input_path = UTIL::get_relative_path(input_path, bfs::current_path());
  }

  // iterate over the arguments, skipping function name (position zero)
  for ( vector<func_args::arg_term>::iterator it=fx_argv.argv.begin()+1;
                                              it!=fx_argv.argv.end();
                                              ++it )
  {
    string n = it->name;
    string v = it->value;
    bool flag = ( atoi( v.c_str() ) > 0 );   // assign flag value

    if ( it->positional )
    { // invalid
      return( amu_error_msg(n + "=" + v + " invalid option. " + help) );
    }
    else
    {
      if      (
                !(n.compare(vana[0])&&n.compare(vana[1])) ||  // current
                !(n.compare(vana[2])&&n.compare(vana[3])) ||  // absolute
                !(n.compare(vana[4])&&n.compare(vana[5]))     // verbose
              )
      {
        // do nothing, control flags values set above.
      }
      else if (!(n.compare(vana[6])&&n.compare(vana[7])) && flag)
      { // file
        if ( result.size() ) result.append( " " );
        if ( verbose ) result.append( path_type + " filename = " );

        result.append( input_path.string() );
      }
      else if (!(n.compare(vana[8])&&n.compare(vana[9])) && flag)
      { // path
        if ( result.size() ) result.append( " " );
        if ( verbose ) result.append( path_type + " pathname = " );

        result.append( input_path.parent_path().string() );
      }
      else if (!(n.compare(vana[10])&&n.compare(vana[11])) && flag)
      { // base
        if ( result.size() ) result.append( " " );
        if ( verbose ) result.append( "basename = " );

        result.append( input_path.filename().string() );
      }
      else if (!(n.compare(vana[12])&&n.compare(vana[13])) && flag)
      { // stem
        if ( result.size() ) result.append( " " );
        if ( verbose ) result.append( "stemname = " );

        result.append( input_path.stem().string() );
      }
      else if (!(n.compare(vana[14])&&n.compare(vana[15])) && flag)
      { // ext
        if ( result.size() ) result.append( " " );
        if ( verbose ) result.append( "extension = " );

        result.append( input_path.extension().string() );
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
