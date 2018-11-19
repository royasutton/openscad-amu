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

    Perform search and replace on text using the [boost] regular
    expression libraries [regex] and [xpressive].

    The options and flags (and their short codes) are summarized in the
    following tables.

    Options that require arguments.

     options    | sc  | default | description
    :----------:|:---:|:-------:|:--------------------------
      text      | t   |         | text string to seach
      search    | s   | []      | search regular expression
      replace   | r   | []      | format string for matched text

    Regex [syntax flags][syntax_option_type].

     flags                | sc    | description
    :--------------------:|:-----:|:------------------------------------
      icase               | sic   | match without regard to case
      nosubs              | sns   | no sub-expression matches are stored
      optimize            | soe   | optimize regular expression
      collate             | sce   | ranges "[a-b]" are locale sensitive
      single_line         | ssl   | ^ and $ do not match at line breaks
      not_dot_null        | snn   | '.' does not match null character
      not_dot_newline     | snl   | '.' does not match newline character
      ignore_white_space  | sis   | ignore non-escaped white-space

    Match [behavior flags][match_flag_type].

     flags                | sc    | description
    :--------------------:|:-----:|:------------------------------------
      match_not_bol       | mbl   | ^ not matched in [first,first)
      match_not_eol       | mel   | $ not matched in [last,last)
      match_not_bow       | mbw   | \\b not matched in [first,first)
      match_not_eow       | mew   | \\b not matched in [last,last)
      match_any           | maa   | on multi-match any are acceptable
      match_not_null      | mnn   | do not match against empty sequence
      match_continuous    | mcf   | must match a sub-sequence from first
      match_partial       | mpa   | on no match, partial is acceptable
      match_prev_avail    | mpr   | --first is a valid iterator position

    Match [behavior flags][match_flag_type].

     flags                | sc    | description
    :--------------------:|:-----:|:------------------------------------
      format_sed          | fsd   | use sed format construction rules
      format_perl         | fpl   | use perl format construction rules
      format_no_copy      | fnc   | do not copy non-matching characters
      format_first_only   | ffo   | output first match occurrence only
      format_literal      | flf   | treat the format string as a literal
      format_all          | fas   | enable all syntax extensions

    Also see [format string syntax] and [character class names] in the
    boost documentation.

    For more information on how to specify and use function arguments
    see \ref openscad_dif_sm_a.

    [boost]: https://www.boost.org

    [regex]: https://www.boost.org/doc/libs/1_58_0/libs/regex/doc/html/index.html
    [xpressive]: https://www.boost.org/doc/libs/1_58_0/doc/html/xpressive/user_s_guide.html

    [syntax_option_type]: https://www.boost.org/doc/libs/1_58_0/libs/regex/doc/html/boost_regex/ref/syntax_option_type.html
    [match_flag_type]: https://www.boost.org/doc/libs/1_58_0/libs/regex/doc/html/boost_regex/ref/match_flag_type.html

    [format string syntax]: https://www.boost.org/doc/libs/1_58_0/libs/regex/doc/html/boost_regex/format.html
    [character class names]: https://www.boost.org/doc/libs/1_58_0/libs/regex/doc/html/boost_regex/syntax/character_classes.html
*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_replace(void)
{
  using namespace UTIL;

  // options declaration: vana & vans.
  // !!DO NOT REORDER WITHOUT UPDATING POSITIONAL DEPENDENCIES BELOW!!
  string vana[] =
  {
  "text",               "t",
  "search",             "s",
  "replace",            "r",

  "icase",              "sic",
  "nosubs",             "sns",
  "optimize",           "soe",
  "collate",            "sce",
  "single_line",        "ssl",
  "not_dot_null",       "snn",
  "not_dot_newline",    "snl",
  "ignore_white_space", "sis",

  "match_not_bol",      "mbl",
  "match_not_eol",      "mel",
  "match_not_bow",      "mbw",
  "match_not_eow",      "mew",
  "match_any",          "maa",
  "match_not_null",     "mnn",
  "match_continuous",   "mcf",
  "match_partial",      "mpa",
  "match_prev_avail",   "mpr",

  "format_sed",         "fsd",
  "format_perl",        "fpl",
  "format_no_copy",     "fnc",
  "format_first_only",  "ffo",
  "format_literal",     "flf",
  "format_all",         "fas"
  };
  set<string> vans(vana, vana + sizeof(vana)/sizeof(string));

  // assign local variable values: positions must match declaration above.
  size_t ap=0;
  string text     = unquote(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string search   = unquote(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string replace  = unquote(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;

  bool sic = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool sns = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool soe = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool sce = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool ssl = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool snn = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool snl = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool sis = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;

  bool mbl = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool mel = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool mbw = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool mew = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool maa = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool mnn = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool mcf = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool mpa = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool mpr = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;

  bool fsd = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool fpl = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool fnc = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool ffo = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool flf = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;
  bool fas = ( atoi( unquote_trim(fx_argv.arg_firstof("0",vana[ap],vana[ap+1])).c_str() ) > 0 ); ap+=2;

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

  // regex syntax flag
  regex_constants::syntax_option_type res_f;

  /*
    "icase",              "sic",
    "nosubs",             "sns",
    "optimize",           "soe",
    "collate",            "sce",
    "single_line",        "ssl",
    "not_dot_null",       "snn",
    "not_dot_newline",    "snl",
    "ignore_white_space", "sis",
  */

  res_f = regex_constants::ECMAScript;

  if ( sic ) res_f = res_f | regex_constants::icase;
  if ( sns ) res_f = res_f | regex_constants::nosubs;
  if ( soe ) res_f = res_f | regex_constants::optimize;
  if ( sce ) res_f = res_f | regex_constants::collate;
  if ( ssl ) res_f = res_f | regex_constants::single_line;
  if ( snn ) res_f = res_f | regex_constants::not_dot_null;
  if ( snl ) res_f = res_f | regex_constants::not_dot_newline;
  if ( sis ) res_f = res_f | regex_constants::ignore_white_space;

  sregex sre = sregex::compile( search, res_f );

  // regex algorithms behavior flag
  regex_constants::match_flag_type rab_f;

  /*
    "match_not_bol",      "mbl",
    "match_not_eol",      "mel",
    "match_not_bow",      "mbw",
    "match_not_eow",      "mew",
    "match_any",          "maa",
    "match_not_null",     "mnn",
    "match_continuous",   "mcf",
    "match_partial",      "mpa",
    "match_prev_avail",   "mpr",
  */

  rab_f = regex_constants::match_default | regex_constants::format_default;

  if ( mbl ) rab_f = rab_f | regex_constants::match_not_bol;
  if ( mel ) rab_f = rab_f | regex_constants::match_not_eol;
  if ( mbw ) rab_f = rab_f | regex_constants::match_not_bow;
  if ( mew ) rab_f = rab_f | regex_constants::match_not_eow;
  if ( maa ) rab_f = rab_f | regex_constants::match_any;
  if ( mnn ) rab_f = rab_f | regex_constants::match_not_null;
  if ( mcf ) rab_f = rab_f | regex_constants::match_continuous;
  if ( mpa ) rab_f = rab_f | regex_constants::match_partial;
  if ( mpr ) rab_f = rab_f | regex_constants::match_prev_avail;

  /*
    "format_sed",         "fsd",
    "format_perl",        "fpl",
    "format_no_copy",     "fnc",
    "format_first_only",  "ffo",
    "format_literal",     "flf",
    "format_all",         "fas"
  */

  if ( fsd ) rab_f = rab_f | regex_constants::format_sed;
  if ( fpl ) rab_f = rab_f | regex_constants::format_perl;
  if ( fnc ) rab_f = rab_f | regex_constants::format_no_copy;
  if ( ffo ) rab_f = rab_f | regex_constants::format_first_only;
  if ( flf ) rab_f = rab_f | regex_constants::format_literal;
  if ( fas ) rab_f = rab_f | regex_constants::format_all;

  result = regex_replace( text, sre, replace, rab_f );

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
