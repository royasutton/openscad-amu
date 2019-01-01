/***************************************************************************//**

  \file   openscad_dif_scanner_bif2.cpp

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
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

using namespace std;
namespace bfs = boost::filesystem;


/***************************************************************************//**
  \details

    Copy the list of files to the output directory for the specified
    documentation types.

     options    | sc  | default | description
    :----------:|:---:|:-------:|:-----------------------------
      files     | f   |         | list of files
      types     | t   |         | html, latex, docbook, or rtf

    The tokenizer character that separates list members are summarized in the
    following table.

     type     | any of
    :--------:|:------------:
     files    | [,[:space:]]
     types    | [,[:space:]]

    For more information on how to specify and use function arguments
    see \ref openscad_dif_sm_a.

*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_copy(void)
{
  // options declaration: vana & vans.
  // !!DO NOT REORDER WITHOUT UPDATING POSITIONAL DEPENDENCIES BELOW!!
  string vana[] =
  {
  "files",            "f",
  "types",            "t"
  };
  set<string> vans(vana, vana + sizeof(vana)/sizeof(string));

  // variables
  size_t ap=0;
  string files;  ap+=2;
  string types;  ap+=2;

  // generate options help string.
  string help = "options: [";
  for(size_t it=0; it < ap; it+=2) {
    if (it) help.append( ", " );
    help.append( vana[it] + " (" + vana[it+1] + ")" );
  }
  help.append( "]" );

  // iterate over the arguments, skipping function name (position zero)
  for ( vector<func_args::arg_term>::iterator it=fx_argv.argv.begin()+1;
                                              it!=fx_argv.argv.end();
                                              ++it )
  {
    string n = it->name;
    string v = it->value;

    if ( it->positional )
    { // invalid
      return( amu_error_msg(n + "=" + v + " invalid option. " + help) );
    }
    else
    {
      if      (!(n.compare(vana[0])&&n.compare(vana[1])))
      { // files
        files = UTIL::unquote_trim( v );
      }
      else if (!(n.compare(vana[2])&&n.compare(vana[3])))
      { // types
        types = UTIL::unquote_trim( v );
      }
      else
      { // invalid
        return( amu_error_msg(n + "=" + v + " invalid option. " + help) );
      }
    }
  }

  //
  // validate arguments
  //
  typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
  boost::char_separator<char> fsep(", ");

  tokenizer f_tok( files, fsep );
  tokenizer t_tok( types, fsep );

  vector<string> outpath;

  for ( tokenizer::iterator tit=t_tok.begin(); tit!=t_tok.end(); ++tit )
  {
    if      ( !tit->compare("html") )
    {
      outpath.push_back( get_html_output() );
    }
    else if ( !tit->compare("latex") )
    {
      outpath.push_back( get_latex_output() );
    }
    else if ( !tit->compare("docbook") )
    {
      outpath.push_back( get_docbook_output() );
    }
    else if ( !tit->compare("rtf") )
    {
      outpath.push_back( get_rtf_output() );
    }
    else
    {
      return( amu_error_msg( *tit + " invalid type option. "
                                    "may be [html, latex, docbook, or rtf].") );
    }
  }

  filter_debug(fx_name + " begin", true, false, true);

  filter_debug("files=[" + files + "]", false, false, true);
  filter_debug("types=[" + types + "]", false, false, true);

  string result;

  // iterate over each file and output path
  for ( tokenizer::iterator fit=f_tok.begin(); fit!=f_tok.end(); ++fit )
  {
    for ( vector<string>::iterator pit=outpath.begin(); pit!=outpath.end(); ++pit )
    {
      string file( *fit );
      string path( *pit );

      bool found = false;
      string rl = file_rl( file, path, found );

      if ( found == false )
        result.append(rl + " ");

      filter_debug("copy " + file + " to " + path + " : " +
                   (found?string("ok"):string("not found")),
                   false, false, true);
    }
  }

  filter_debug(fx_name + " end", false, true, true);

  if ( result.length() )
  {
    return ( amu_error_msg("not found: " + result) );
  }
  else
  {
    return( result );
  }
}

/***************************************************************************//**
  \details

    Search for files in the include-paths. If found, return the located
    file paths.

    The options and flags (and their short codes) are summarized in the
    following tables.

     options    | sc  | default | description
    :----------:|:---:|:-------:|:-----------------------------
      files     | f   |         | list of files
      nfl       | n   | 1       | file not-found limit (0 = unlimited)

    Command flags.

     flags     | sc  | default | description
    :---------:|:---:|:-------:|:-----------------------------------------
      rmnl     | r   | true    | remove line-feeds / carriage returns
      skip     | s   | false   | skip files that are not found

    The tokenizer character that separates list members are summarized
    in the following table.

     type     | any of
    :--------:|:------------:
     files    | [,[:space:]]

  \note List of files should be enclosed in quotations (single or
        double).

    For more information on how to specify and use function arguments
    see \ref openscad_dif_sm_a.

*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_find(void)
{
  // options declaration: vana & vans.
  // !!DO NOT REORDER WITHOUT UPDATING POSITIONAL DEPENDENCIES BELOW!!
  string vana[] =
  {
  "files",    "f",
  "nfl",      "n",
  "rmnl",     "r",
  "skip",     "s"
  };
  set<string> vans(vana, vana + sizeof(vana)/sizeof(string));

  // variables
  size_t ap=0;
  string files;           ap+=2;
  int nfl = 1;            ap+=2;

  // flags
  bool flag_rmnl = true;  ap+=2;
  bool flag_skip = false; ap+=2;

  // generate options help string.
  string help = "options: [";
  for(size_t it=0; it < ap; it+=2) {
    if (it) help.append( ", " );
    help.append( vana[it] + " (" + vana[it+1] + ")" );
  }
  help.append( "]" );

  // iterate over the arguments, skipping function name (position zero)
  for ( vector<func_args::arg_term>::iterator it=fx_argv.argv.begin()+1;
                                              it!=fx_argv.argv.end();
                                              ++it )
  {
    string n = it->name;
    string v = it->value;

    if ( it->positional )
    { // invalid
      return( amu_error_msg(n + "=" + v + " invalid option. " + help) );
    }
    else
    {
      if      (!(n.compare(vana[0])&&n.compare(vana[1])))
      { // files
        files = UTIL::unquote_trim( v );
      }
      else if (!(n.compare(vana[2])&&n.compare(vana[3])))
      { // nfl (file not found limit)
        nfl = atoi( v.c_str() );
      }
      else if (!(n.compare(vana[4])&&n.compare(vana[5])))
      { // rmnl
        flag_rmnl=( atoi( v.c_str() ) > 0 );
      }
      else if (!(n.compare(vana[6])&&n.compare(vana[7])))
      { // skip
        flag_skip=( atoi( v.c_str() ) > 0 );
      }
      else
      { // invalid
        return( amu_error_msg(n + "=" + v + " invalid option. " + help) );
      }
    }
  }

  // remove line-feeds / carriage returns
  if ( flag_rmnl )
    files = UTIL::replace_chars(files, "\n\r", ' ');

  typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
  boost::char_separator<char> fsep(", ");

  tokenizer f_tok( files, fsep );

  filter_debug(fx_name + " begin", true, false, true);

  filter_debug("files=[" + files + "]", false, false, true);

  string result;
  int nfc = 0;

  // iterate over each file
  for ( tokenizer::iterator fit=f_tok.begin(); fit!=f_tok.end(); ++fit )
  {
    string file( *fit );

    bool found = false;
    string rl = file_rl( file, NO_FORMAT_OUTPUT, found );

    if ( found == false )
      nfc++;

    // add unless both not found and flag_skip is true
    if ( ! ((found == false) && (flag_skip == true)) )
      result.append(rl + " ");

    filter_debug("find " + file + " : " +
                 (found?(string("found --> ") + rl):string("not found")),
                 false, false, true);
  }

  filter_debug(fx_name + " end", false, true, true);

  // skip test when nfl <= 0
  if ( (nfc > nfl) && (nfl > 0) )
    return
    ( amu_error_msg
      (
        UTIL::to_string(nfc) + "(>" +
        UTIL::to_string(nfl) + ") file(s) not found: " + result
      )
    );
  else
    return( result );
}

/***************************************************************************//**
  \details

    Return information about initial input path components.

    The options and flags (and their short codes) are summarized in the
    following tables.

    Options that require arguments.

     options      | sc  | default | description
    :------------:|:---:|:-------:|:-----------------------------------------
      head        | h   |         | path name from head at location
      tail        | t   |         | path name from tail at location

    Flags that produce output.

     flags        | sc  | default | description
    :------------:|:---:|:-------:|:-----------------------------------------
      full        | f   | false   | full path name to input from root
      path        | a   | false   | full path name to input after root
      full_parent | fp  | false   | full parent path name to input from root
      path_parent | ap  | false   | full parent path name to input after root

    Flags that produce output.

     flags        | sc  | default | description
    :------------:|:---:|:-------:|:-----------------------------------------
      root        | r   | false   | root path name
      stem        | s   | false   | input stem name
      dir         | d   | false   | input directory name
      parent      | p   | false   | input parent directory name

    For more information on how to specify and use function arguments
    see \ref openscad_dif_sm_a.

*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_pathid(void)
{
  using namespace UTIL;

  // options declaration: vana & vans.
  // !!DO NOT REORDER WITHOUT UPDATING POSITIONAL DEPENDENCIES BELOW!!
  string vana[] =
  {
  "head",         "h",
  "tail",         "t",

  "full",         "f",
  "path",         "a",
  "full_parent",  "fp",
  "path_parent",  "ap",

  "root",         "r",
  "stem",         "s",
  "dir",          "d",
  "parent",       "p"
  };
  set<string> vans(vana, vana + sizeof(vana)/sizeof(string));

  // assign local variable values: positions must match declaration above.
  size_t ap=20;

  // generate options help string.
  string help = "options: [";
  for(size_t it=0; it < ap; it+=2) {
    if (it) help.append( ", " );
    help.append( vana[it] + " (" + vana[it+1] + ")" );
  }
  help.append( "]" );

  string result;

  bfs::path input_path = get_input_name( true );
  bfs::path parent_path;

  string path_joiner = "_";

  // get relative parent path.
  input_path = UTIL::get_relative_path(input_path, bfs::current_path());
  parent_path = input_path.parent_path();

  //
  // create vector of parent path levels
  //
  vector<string> input_path_vec;

  // start with 'root'
  input_path_vec.push_back("root");

  // add each directory of parent_path from left to right
  for (bfs::path::const_iterator pi=parent_path.begin();
                                 pi!=parent_path.end();
                                 ++pi)
  {
    input_path_vec.push_back( pi->string() );
  }

  // iterate over the arguments, skipping function name (position zero)
  for ( vector<func_args::arg_term>::iterator it=fx_argv.argv.begin()+1;
                                              it!=fx_argv.argv.end();
                                              ++it )
  {
    string n = it->name;
    string v = it->value;

    bool flag = ( atoi( v.c_str() ) > 0 );    // assign flag value
    bool limited = false;
    size_t index = atoi( v.c_str() );         // assign value as integer

    // limit index
    if ( index > (input_path_vec.size()-1) )
    {
      index = input_path_vec.size()-1;
      limited = true;
    }

    if ( it->positional )
    { // invalid
      return( amu_error_msg(n + "=" + v + " invalid option. " + help) );
    }
    else
    {
      //
      // [ option with integer argument ]
      //

      if      (!(n.compare(vana[0])&&n.compare(vana[1])))
      { // head
        if ( result.size() ) result.append( path_joiner );
        if ( limited ) result.append( "limited" + path_joiner );
        result.append( input_path_vec[ index ] );
      }
      else if (!(n.compare(vana[2])&&n.compare(vana[3])))
      { // tail
        if ( result.size() ) result.append( path_joiner );
        result.append( input_path_vec[ input_path_vec.size() - 1 - index ] );
        if ( limited ) result.append( path_joiner + "limited" );
      }

      //
      // [ flags ]
      //

      else if (!(n.compare(vana[4])&&n.compare(vana[5])) && flag)
      { // full
        for( vector<string>::iterator vit=input_path_vec.begin();
                                      vit!=input_path_vec.end();
                                      ++vit)
        {
          if ( result.size() ) result.append( path_joiner );
          result.append( *vit );
        }
      }
      else if (!(n.compare(vana[6])&&n.compare(vana[7])) && flag)
      { // path
        if ( input_path_vec.size() > 1 )
        {
          for( vector<string>::iterator vit=input_path_vec.begin()+1;
                                        vit!=input_path_vec.end();
                                        ++vit)
          {
            if ( result.size() ) result.append( path_joiner );
            result.append( *vit );
          }
        }
      }
      else if (!(n.compare(vana[8])&&n.compare(vana[9])) && flag)
      { // full_parent
        if ( input_path_vec.size() > 1 )
        {
          for( vector<string>::iterator vit=input_path_vec.begin();
                                        vit!=input_path_vec.end()-1;
                                        ++vit)
          {
            if ( result.size() ) result.append( path_joiner );
            result.append( *vit );
          }
        }
        else
        {
          result.append( "none" );
        }
      }
      else if (!(n.compare(vana[10])&&n.compare(vana[11])) && flag)
      { // path_parent
        if ( input_path_vec.size() > 2 )
        {
          for( vector<string>::iterator vit=input_path_vec.begin()+1;
                                        vit!=input_path_vec.end()-1;
                                        ++vit)
          {
            if ( result.size() ) result.append( path_joiner );
            result.append( *vit );
          }
        }
      }

      //
      // [ flags ]
      //

      else if (!(n.compare(vana[12])&&n.compare(vana[13])) && flag)
      { // root
        if ( result.size() ) result.append( path_joiner );
        result.append( input_path_vec[ 0 ] );
      }
      else if (!(n.compare(vana[14])&&n.compare(vana[15])) && flag)
      { // stem
        if ( result.size() ) result.append( path_joiner );
        result.append( input_path.stem().string() );
      }
      else if (!(n.compare(vana[16])&&n.compare(vana[17])) && flag)
      { // dir
        if ( result.size() ) result.append( path_joiner );
        result.append( input_path_vec[ input_path_vec.size() - 1 ] );
      }
      else if (!(n.compare(vana[18])&&n.compare(vana[19])) && flag)
      { // parent or 'none'
        if ( result.size() ) result.append( path_joiner );
        if ( input_path_vec.size() > 1 )
          result.append( input_path_vec[ input_path_vec.size() - 2 ] );
        else
          result.append( "none" );
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

    Perform filename operations on a list of files.

    The options and flags (and their short codes) are summarized in the
    following tables.

    Options that require arguments.

     options      | sc  | default       | description
    :------------:|:---:|:-------------:|:-----------------------------------------
      files       | f   |               | list of files
      tokenizer   | t   | [,[:space:]]  | tokenizer to separate files in list
      separator   | r   | [[:space:]]   | file separator for resulting list

    Flags that produce output.

     flags     | sc  | default | description
    :---------:|:---:|:-------:|:-----------------------------------------
      file     | n   | false   | return filename for each file
      path     | p   | false   | return pathname for each file
      base     | b   | false   | return basename for each file
      stem     | s   | false   | return stemname for each file
      ext      | e   | false   | return extension for each file

    For more information on how to specify and use function arguments
    see \ref openscad_dif_sm_a.

*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_filename(void)
{
  using namespace UTIL;

  // options declaration: vana & vans.
  // !!DO NOT REORDER WITHOUT UPDATING POSITIONAL DEPENDENCIES BELOW!!
  string vana[] =
  {
  "files",      "f",

  "tokenizer",  "t",
  "separator",  "r",

  "file",       "n",
  "path",       "p",
  "base",       "b",
  "stem",       "s",
  "ext",        "e"
  };
  set<string> vans(vana, vana + sizeof(vana)/sizeof(string));

  // generate options help string.
  size_t ap=16;
  string help = "options: [";
  for(size_t it=0; it < ap; it+=2) {
    if (it) help.append( ", " );
    help.append( vana[it] + " (" + vana[it+1] + ")" );
  }
  help.append( "]" );

  //
  // assemble result
  //
  string result;

  string tokl = ", ";   // assign default token list
  string fsep = " ";    // assign default output file separator

  vector<string> fl_v;

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
      if (!(n.compare(vana[0])&&n.compare(vana[1])))
      { // file list
        string fl_s = unquote( v );

        typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
        boost::char_separator<char> fsep( tokl.c_str() );
        tokenizer fl_tok( fl_s, fsep );

        fl_v.clear();
        for ( tokenizer::iterator fit=fl_tok.begin(); fit!=fl_tok.end(); ++fit )
          fl_v.push_back( boost::trim_copy( *fit ) );
      }

      else if (!(n.compare(vana[2])&&n.compare(vana[3])))
      { // tokenizer
        tokl = unquote( v );
      }
      else if (!(n.compare(vana[4])&&n.compare(vana[5])))
      { // separator
        fsep = unquote( v );
      }

      //
      // flags
      //
      else if (!(n.compare(vana[6])&&n.compare(vana[7])) && flag)
      { // file
        for ( vector<string>::const_iterator fit=fl_v.begin(); fit!=fl_v.end(); ++fit )
        {
          if ( result.size() ) result.append( fsep );
            result.append( bfs::path( *fit ).string() );
        }
      }
      else if (!(n.compare(vana[8])&&n.compare(vana[9])) && flag)
      { // path
        for ( vector<string>::const_iterator fit=fl_v.begin(); fit!=fl_v.end(); ++fit )
        {
          if ( result.size() ) result.append( fsep );
            result.append( bfs::path( *fit ).parent_path().string() );
        }
      }
      else if (!(n.compare(vana[10])&&n.compare(vana[11])) && flag)
      { // base
        for ( vector<string>::const_iterator fit=fl_v.begin(); fit!=fl_v.end(); ++fit )
        {
          if ( result.size() ) result.append( fsep );
          result.append( bfs::path( *fit ).filename().string() );
        }
      }
      else if (!(n.compare(vana[12])&&n.compare(vana[13])) && flag)
      { // stem
        for ( vector<string>::const_iterator fit=fl_v.begin(); fit!=fl_v.end(); ++fit )
        {
          if ( result.size() ) result.append( fsep );
          result.append( bfs::path( *fit ).stem().string() );
        }
      }
      else if (!(n.compare(vana[14])&&n.compare(vana[15])) && flag)
      { // ext
        for ( vector<string>::const_iterator fit=fl_v.begin(); fit!=fl_v.end(); ++fit )
        {
          if ( result.size() ) result.append( fsep );
          result.append( bfs::path( *fit ).extension().string() );
        }
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

    Perform operations on text files.

    The options and flags (and their short codes) are summarized in the
    following tables.

    Options that require arguments.

     options      | sc  | default | description
    :------------:|:---:|:-------:|:--------------------------------------
      file        | f   |         | name of source file
      text        | t   |         | use content of text as source file
      first       | i   | 1       | start line of file
      last        | l   | 0       | end line of file, 0=<eof>
      separator   | s   | []      | separator for joining results

    Flags that modify the output

     flags     | sc  | default | description
    :---------:|:---:|:-------:|:-----------------------------------------
      rmecho   | o   | false   | remove OpenSCAD quoted [ECHO: "..."]
      rmnl     | r   | false   | remove line-feeds / carriage returns
      eval     | e   | false   | expand variables in text

    Flags that produce output

     flags     | sc  | default | description
    :---------:|:---:|:-------:|:-----------------------------------------
      read     | rd  | false   | return processed \p file contents
      lines    | lc  | false   | return processed line count
      chars    | cc  | false   | return processed character count
      max      | xl  | false   | return processed max line size
      min      | nl  | false   | return processed min line size

    For more information on how to specify and use function arguments
    see \ref openscad_dif_sm_a.

    [printf]: https://en.wikipedia.org/wiki/Printf_format_string
*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_file(void)
{
  using namespace UTIL;

  // options declaration: vana & vans.
  // !!DO NOT REORDER WITHOUT UPDATING POSITIONAL DEPENDENCIES BELOW!!
  string vana[] =
  {
  "file",       "f",
  "text",       "t",

  "first",      "i",
  "last",       "l",

  "separator",  "s",

  "rmecho",     "o",
  "rmnl",       "r",
  "eval",       "e",

  "read",       "rd",
  "lines",      "lc",
  "chars",      "cc",
  "max",        "xl",
  "min",        "nl"
  };
  set<string> vans(vana, vana + sizeof(vana)/sizeof(string));

  // assign local variable values: positions must match declaration above.
  size_t ap=0;
  string file = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string text = unquote     (fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;

  uint first  = atoi( unquote_trim(fx_argv.arg_firstof("1",vana[ap],vana[ap+1])).c_str() ); ap+=2;
  uint last   = atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ); ap+=2;

  string wsep = unquote     (fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;

  bool rmecho = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool rmnl   = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool eval   = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;

  bool read   = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool lines  = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool chars  = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool maxln  = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool minln  = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;

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

  // file and text arguments mutually exclusive
  if ( !file.empty() && !text.empty() )
    return(amu_error_msg("file and text arguments mutually exclusive. " + help));

  string result;

  bool ids_ready = false;

  ifstream      ifs;          // file stream
  istringstream iss;          // string stream

  if ( !file.empty() )
  { // use file stream
    bool found = false;
    string rl = file_rl( file, NO_FORMAT_OUTPUT, found );

    if ( found  )
    {
      ifs.open( rl.c_str() );
      ids_ready = ifs.is_open();

      if ( !ids_ready )
        result.append( amu_error_msg("unable to open: " + file) );
    }
    else
      result.append( amu_error_msg("unable to find: " + file) );
  }
  else if ( !text.empty() )
  { // use text stream
    iss.str( text );
    ids_ready = true;
  }
  else
  { // error: no source
    result.append( amu_error_msg("no source file or text.") );
    ids_ready = false;
  }

  // assign input data stream source
  istream& ids = ifs.is_open() ? static_cast<istream&>(ifs)
                               : static_cast<istream&>(iss);

  if ( ids_ready  )
  {
    uint ids_line   = 0;          // current line of input

    uint text_line  = 0;          // current stats of processed input
    uint char_count = 0;
    uint line_max   = 0;
    uint line_min   = UINT_MAX;

    string line;
    string data;

    while ( !ids.eof() )
    {
      getline(ids, line);
      ids_line++;

      if ( (ids_line >= first) && ( (last == 0) || (ids_line<=last) ) )
      {
        text_line++;

        // remove echo
        if ( rmecho )
        { // format [[ECHO:][ ][" ... echo-content ... "]endl]

          // [ECHO:]
          if ( line.find( "ECHO:" ) == 0 )
          { // 'ECHO:' iff at pos==0
            line.erase( 0, 5 );

            // [ ] single space character at pos==0
            // if ( line.find_first_of( " " ) == 0 )
            //  line.erase( 0, 1 );

            // [ ] all white space from pos==0
            size_t p = line.find_first_not_of( " \t" );
            if ( (p != 0) && (p != string::npos) )
              line.erase( 0, p );

            // [" ... echo-content ... "]
            if ( line.find_first_of( "\"" ) == 0 )
            { // open quote at pos==0
              line.erase( 0, 1 );

              // close quote at pos=eol
              size_t l = line.length();
              if ( (l != 0) && (line.find_last_of( "\"" ) == (l-1)) )
                line.erase( l-1, 1 );
              else
                line.append( "< " + amu_error_msg("close quote missing") );
            }
          }
        }

        // eval variables
        if ( eval )
          line = levm.expand_text(line);

        if (line.size() > line_max)
          line_max = line.size();

        if ( line.size() && (line.size() < line_min) )
          line_min = line.size();

        // handle end of line
        if ( rmnl )
          line += wsep;
        else
          line += "\n";

        char_count += line.size();

        data.append( line );
      }
    }

    //
    // produce output
    //

    if ( read )
    {
      if ( result.size() ) result.append( wsep );
      result.append( data );
    }

    if ( lines )
    {
      if ( result.size() ) result.append( wsep );
      result.append( UTIL::to_string( text_line ) );
    }

    if ( chars )
    {
      if ( result.size() ) result.append( wsep );
      result.append( UTIL::to_string( char_count ) );
    }

    if ( maxln )
    {
      if ( result.size() ) result.append( wsep );
      result.append( UTIL::to_string( line_max ) );
    }

    if ( minln )
    {
      if ( result.size() ) result.append( wsep );
      result.append( UTIL::to_string( line_min ) );
    }
  }

  // close file if open
  if ( ifs.is_open() )
    ifs.close();

  return ( result );
}


/*******************************************************************************
// eof
*******************************************************************************/
