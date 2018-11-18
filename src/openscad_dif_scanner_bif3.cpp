/***************************************************************************//**

  \file   openscad_dif_scanner_bif3.cpp

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

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/xpressive/xpressive.hpp>

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

using namespace std;
namespace bfs = boost::filesystem;


/***************************************************************************//**

  \details

    For a list of sets: <tt>set1, set2, ..., setn,</tt> combine each
    element of each set one at a time to form a named n-tuple word
    connected by a configurable joiner string. Each word has a
    configurable prefix added to its front and configurable suffix
    added to its end and are appended to the return value with a
    configurable word separator.

    Each positional argument is a separate set. Sets with multiple
    members require its members to be enclosed within quotation marks.
    The tokenizer character are used to separate the lists of set
    members.

    The options and their short codes are summarized in the following
    table.

     options     | sc  | default        | description
    :-----------:|:---:|:--------------:|:--------------------------------
      prefix     | p   |                | word prefix
      suffix     | s   |                | word suffix
      joiner     | j   | [_]            | set member joiner
      separator  | f   | [,]            | word separator
      tokenizer  | t   | [\|,[:space:]] | set-members tokenizer characters

    For more information on how to specify and use function arguments
    see \ref openscad_dif_sm_a.

*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_combine(void)
{
  using namespace UTIL;

  // options declaration: vana & vans.
  // !!DO NOT REORDER WITHOUT UPDATING POSITIONAL DEPENDENCIES BELOW!!
  string vana[] =
  {
  "prefix",     "p",
  "suffix",     "s",
  "joiner",     "j",
  "separator",  "f",
  "tokenizer",  "t"
  };
  set<string> vans(vana, vana + sizeof(vana)/sizeof(string));

  // assign local variable values: positions must match declaration above.
  // do not trim to allow combining of natural language.
  size_t ap=0;
  string prefix     = unquote(fx_argv.arg_firstof(   "",vana[ap],vana[ap+1])); ap+=2;
  string suffix     = unquote(fx_argv.arg_firstof(   "",vana[ap],vana[ap+1])); ap+=2;
  string joiner     = unquote(fx_argv.arg_firstof(  "_",vana[ap],vana[ap+1])); ap+=2;
  string separator  = unquote(fx_argv.arg_firstof(  ",",vana[ap],vana[ap+1])); ap+=2;
  string tokenizer  = unquote(fx_argv.arg_firstof("|, ",vana[ap],vana[ap+1])); ap+=2;

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

  // get the positional arguments as vector (except arg0).
  vector<string> pa = fx_argv.values_v(false, true);
  pa.erase( pa.begin() );

  // created the vector of sets to combine with grouping quotes removed.
  // do not trim to allow combining of natural language.
  vector<string> sv;
  for ( vector<string>::iterator it=pa.begin(); it!=pa.end(); ++it )
    sv.push_back( unquote( *it ) );

  string result;
  bif_combineR( result, sv, prefix, suffix, joiner, separator, tokenizer);

  return( result );
}

void
ODIF::ODIF_Scanner::bif_combineR( string &r, vector<string> sv,
                                  const string &p, const string &s,
                                  const string &j, const string &ws,
                                  const string &t )
{
  // check recursion termination.
  if ( sv.size()== 0 )
  {
    // append word separator after the first word
    if( r.length() !=0 ) r+= ws;

    r+=p + s;
  }
  else
  {
    // separate member words of first set of 'sv' and store in 'sm'
    string          fs = sv.front();
    vector<string>  sm;

    typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

    boost::char_separator<char> sep( t.c_str() );
    tokenizer tokens(fs, sep);

    for ( tokenizer::iterator it=tokens.begin(); it!=tokens.end(); ++it )
      sm.push_back( j + *it );

    // for each word in 'sm' combine with words of remaining sets
    sv.erase( sv.begin() );
    for ( vector<string>::iterator it=sm.begin(); it!=sm.end(); ++it )
      bif_combineR(r, sv, p + *it, s, j, ws, t);
  }
}

/***************************************************************************//**
  \details

    Perform search and replace on text. Regular expression support is
    provided by the [boost] regex_replace() library function.

    The options and flags (and their short codes) are summarized in the
    following tables.

    Options that require arguments.

     options    | sc  | default | description
    :----------:|:---:|:-------:|:--------------------------
      text      | t   |         | text string to seach
      search    | s   | []      | search regular expression
      replace   | r   | []      | format string for matched text

    Flags.

     flags          | sc  | default | description
    :--------------:|:---:|:-------:|:------------------------------------
      global        | g   | true    | replace all occurances
      no_copy       | n   | false   | do not copy text that do not match

    Format flags.

     flags          | sc  | default | description
    :--------------:|:---:|:-------:|:------------------------------------
      fmt_literal   | fl  | false   | treat format string as literal
      fmt_perl      | fp  | false   | recognize perl format sequences
      fmt_sed       | fs  | false   | recognize sed format sequences
      fmt_all       | fa  | false   | recognize all format sequences

    For more information on how to specify and use function arguments
    see \ref openscad_dif_sm_a.

    [boost]: https://www.boost.org
*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_replace(void)
{
  using namespace UTIL;

  // options declaration: vana & vans.
  // !!DO NOT REORDER WITHOUT UPDATING POSITIONAL DEPENDENCIES BELOW!!
  string vana[] =
  {
  "text",           "t",
  "search",         "s",
  "replace",        "r",

  "global",         "g",
  "no_copy",        "n",

  "fmt_literal",    "fl",
  "fmt_perl",       "fp",
  "fmt_sed",        "fs",
  "fmt_all",        "fa"
  };
  set<string> vans(vana, vana + sizeof(vana)/sizeof(string));

  // assign local variable values: positions must match declaration above.
  size_t ap=0;
  string text         = unquote(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string search       = unquote(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string replace      = unquote(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;

  bool global         = ( atoi( unquote_trim(fx_argv.arg_firstof("1",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool no_copy        = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;

  bool fmt_literal    = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool fmt_perl       = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool fmt_sed        = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool fmt_all        = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;

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

  string result;

  using namespace boost::xpressive;
  sregex sre = sregex::compile( search );
  regex_constants::match_flag_type flags;

  flags = regex_constants::format_default;

  if ( !global )        flags = flags | regex_constants::format_first_only;
  if ( no_copy )        flags = flags | regex_constants::format_no_copy;

  if ( fmt_literal )    flags = flags | regex_constants::format_literal;
  if ( fmt_perl )       flags = flags | regex_constants::format_perl;
  if ( fmt_sed )        flags = flags | regex_constants::format_sed;
  if ( fmt_all )        flags = flags | regex_constants::format_all;

  result = regex_replace( text, sre, replace, flags );

  return ( result );
}

/***************************************************************************//**
  \details

    Count or select a word from a list of words.

    The options and flags (and their short codes) are summarized in the
    following tables.

    Options that require arguments.

     options      | sc  | default         | description
    :------------:|:---:|:---------------:|:-------------------------------------
      words       | w   |                 | list of words
      index       | i   |                 | return word \p i in list
      tokenizer   | t   | [~^,[:space:]]  | tokenizer to separate words in list
      separator   | r   | [^]             | separator for resulting list

    Flags that produce output.

     flags     | sc  | default | description
    :---------:|:---:|:-------:|:-----------------------------------------
      count    | c   | false   | return number of words in list
      first    | f   | false   | return first word in list
      last     | l   | false   | return last word in list
      list     | s   | false   | return word list

    For more information on how to specify and use function arguments
    see \ref openscad_dif_sm_a.

*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_word(void)
{
  using namespace UTIL;

  // options declaration: vana & vans.
  // !!DO NOT REORDER WITHOUT UPDATING POSITIONAL DEPENDENCIES BELOW!!
  string vana[] =
  {
  "words",      "w",
  "index",      "i",

  "tokenizer",  "t",
  "separator",  "r",

  "count",      "c",
  "first",      "f",
  "last",       "l",
  "list",       "s"
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

  string tokl = "~^, "; // assign default token list
  string wsep = "^";    // assign default output file separator

  vector<string> wl_v;

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
      { // word list
        string wl_s = unquote( v );

        typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
        boost::char_separator<char> wsep( tokl.c_str() );
        tokenizer wl_tok( wl_s, wsep );

        wl_v.clear();
         for ( tokenizer::iterator wit=wl_tok.begin(); wit!=wl_tok.end(); ++wit )
          wl_v.push_back( boost::trim_copy( *wit ) );
      }

      else if (!(n.compare(vana[2])&&n.compare(vana[3])))
      { // index
        size_t i = atoi( v.c_str() );

        if ( (i>0) && (i<wl_v.size()) )
        {
          if ( result.size() ) result.append( wsep );
            result.append( wl_v[ i - 1 ] );
        }
      }

      else if (!(n.compare(vana[4])&&n.compare(vana[5])))
      { // tokenizer
        tokl = unquote( v );
      }
      else if (!(n.compare(vana[6])&&n.compare(vana[7])))
      { // separator
        wsep = unquote( v );
      }

      //
      // flags
      //
      else if (!(n.compare(vana[8])&&n.compare(vana[9])) && flag)
      { // count
        if ( result.size() ) result.append( wsep );
          result.append( UTIL::to_string(wl_v.size()) );
      }
      else if (!(n.compare(vana[10])&&n.compare(vana[11])) && flag)
      { // first
        if ( ! wl_v.empty() )
        {
          if ( result.size() ) result.append( wsep );
            result.append( wl_v[ 0 ] );
        }
      }
      else if (!(n.compare(vana[12])&&n.compare(vana[13])) && flag)
      { // last
        if ( ! wl_v.empty() )
        {
          if ( result.size() ) result.append( wsep );
            result.append( wl_v[ wl_v.size() -1 ] );
        }
      }
      else if (!(n.compare(vana[14])&&n.compare(vana[15])) && flag)
      { // list
        for ( vector<string>::const_iterator wit=wl_v.begin(); wit!=wl_v.end(); ++wit )
        {
          if ( result.size() ) result.append( wsep );
          result.append( *wit );
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

    Generate a sequence of numbers.

    The options and flags (and their short codes) are summarized in the
    following tables.

    Options that require arguments.

     options      | sc  | default | description
    :------------:|:---:|:-------:|:------------------------------
      first       | f   | 1       | start of sequence
      incr        | i   | 1       | sequence increment
      last        | l   |         | end of sequence
      prefix      | p   | []      | element prefix text
      suffix      | s   | []      | element suffix text
      separator   | r   | [^]     | separator for resulting list
      format      | o   | []      | format using [printf] function

    Flags that produce output.

     flags     | sc  | default | description
    :---------:|:---:|:-------:|:-----------------------------------------
      number   | n   | false   | generate numerical sequence
      roman    | m   | false   | generate roman numeral sequence

    For more information on how to specify and use function arguments
    see \ref openscad_dif_sm_a.

    [printf]: https://en.wikipedia.org/wiki/Printf_format_string
*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_seq(void)
{
  using namespace UTIL;

  // options declaration: vana & vans.
  // !!DO NOT REORDER WITHOUT UPDATING POSITIONAL DEPENDENCIES BELOW!!
  string vana[] =
  {
  "first",      "f",
  "incr",       "i",
  "last",       "l",
  "prefix",     "p",
  "suffix",     "s",
  "separator",  "r",
  "format",     "o",

  "number",     "n",
  "roman",      "m"
  };
  set<string> vans(vana, vana + sizeof(vana)/sizeof(string));

  // generate options help string.
  size_t ap=18;
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

  int first = 1;
  int incr  = 1;
  int last  = 0;

  string prefix;
  string suffix;

  string wsep = "^";

  string format;

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
      { // first
        first = atoi( unquote( v ).c_str() );
      }
      else if (!(n.compare(vana[2])&&n.compare(vana[3])))
      { // incr
        incr = atoi( unquote( v ).c_str() );
      }
      else if (!(n.compare(vana[4])&&n.compare(vana[5])))
      { // last
        last = atoi( unquote( v ).c_str() );
      }
      else if (!(n.compare(vana[6])&&n.compare(vana[7])))
      { // prefix
        prefix = unquote( v );
      }
      else if (!(n.compare(vana[8])&&n.compare(vana[9])))
      { // suffix
        suffix = unquote( v );
      }
      else if (!(n.compare(vana[10])&&n.compare(vana[11])))
      { // separator
        wsep = unquote( v );
      }
      else if (!(n.compare(vana[12])&&n.compare(vana[13])))
      { // format
        format = unquote( v );
      }

      //
      // flags
      //
      else if (!(n.compare(vana[14])&&n.compare(vana[15])) && flag)
      { // number
        for (int seq = first; seq <= last; seq += incr)
        {
          if ( result.size() ) result.append( wsep );
          if ( prefix.size() ) result.append( prefix );

          if ( format.size() )
          {
            // determine buffer requirement
            size_t bsize = snprintf(NULL, 0, format.c_str(), seq);

            if ( bsize > 0 )
            {
              char buffer[bsize+1];
              snprintf(buffer, bsize+1, format.c_str(), seq);
              result.append( buffer );
            }
          }
          else
          {
            result.append(UTIL::to_string( seq ));
          }

          if ( suffix.size() ) result.append( suffix );
        }
      }
      else if (!(n.compare(vana[16])&&n.compare(vana[17])) && flag)
      { // roman
        for (int seq = first; seq <= last; seq += incr)
        {
          if ( result.size() ) result.append( wsep );
          if ( prefix.size() ) result.append( prefix );

          if ( format.size() )
          {
            // determine buffer requirement
            size_t bsize = snprintf(NULL, 0, format.c_str(),
                                    UTIL::to_roman_numeral( seq ).c_str());;

            if ( bsize > 0 )
            {
              char buffer[bsize+1];
              snprintf(buffer, bsize+1, format.c_str(),
                       UTIL::to_roman_numeral( seq ).c_str());
              result.append( buffer );
            }
          }
          else
          {
            result.append(UTIL::to_roman_numeral( seq ));
          }

          if ( suffix.size() ) result.append( suffix );
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


/*******************************************************************************
// eof
*******************************************************************************/
