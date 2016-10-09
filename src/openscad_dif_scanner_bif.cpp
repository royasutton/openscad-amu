/***************************************************************************//**

  \file   openscad_dif_scanner_bif.cpp

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
    OpenSCAD Doxygen input filter scanner class built-in functions source.

  \ingroup openscad_dif_src
*******************************************************************************/

#include "openscad_dif_scanner.hpp"

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

#include <set>

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

using namespace std;


/***************************************************************************//**

  \details

    Create a local copy of the current variable map (scope). Then process
    each argument one-by-one (skipping function name):

    Named argument value pairs are stored to the local variable scope
    (name=value).

    Positional arguments are expanded using the local variable map and
    appended to the return result. Escaped variables are reduced and
    checked during subsequent expansions.

    The result is returned when there are no more named or positional
    arguments.

  \todo might be nice to have a flag that allows for local assignments to
        be stored in the global environment variable map (++global).
        this would not offer new capability, just shorter syntax.
  \todo might be nice to have a configurable word joiner thats stored in
        the variable map that defaults to " " (joiner="<string>").
        this would offer new capability.

*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_eval(void)
{
  // create local copy of the global variable scope map
  env_var vm = varm;

  string result;

  // iterate over the arguments, skipping function name (position zero)
  for ( vector<func_args::arg_term>::iterator it=fx_argv.argv.begin()+1;
                                              it!=fx_argv.argv.end();
                                              ++it )
  {
    if ( it->positional )     // expand positional argument value string
    {
      if ( result.length() != 0 ) result.append(" ");

      result.append( vm.expand_text( it->value ) );
    }
    else                      // add name=value pairs to variable scope
    {
      vm.store( it->name, it->value );
    }
  }

  return( result );
}

/***************************************************************************//**

  \details

    Use the C/C++ function popen() to execute the string at the first
    positional argument in a shell. The output generated by the command is
    captured to a string, striped of all return and linefeed characters
    and returned.

    Prefix the flag with \c ++ to enable and \c \-- to disable. For example
    <tt>++eval</tt> will turn on variable expansion. The flags and their
    short codes are summarized in the following table.

     flags   | sc  | default | description
    :-------:|:---:|:-------:|:-----------------------------------------
      stderr | s   | false   | capture standard error output as well
      rmnl   | r   | true    | remove line-feeds / carriage returns
      eval   | e   | false   | expand variables in text

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
  "eval",     "e",
  };
  set<string> vans(vana, vana + sizeof(vana)/sizeof(string));

  size_t ap=6;

  // generate options help string.
  string help = "flags: [";
  for(size_t it=0; it < ap; it+=2) {
    if (it) help.append( ", " );
    help.append( vana[it] + " (" + vana[it+1] + ")" );
  }
  help.append( "]" );

  // declare and assign flag defaults
  bool flag_stde = false;
  bool flag_rmnl = true;
  bool flag_eval = false;

  // assign local variable flags:
  vector<string> av = fx_argv.names_v(true, false);
  for ( vector<string>::iterator it=av.begin(); it!=av.end(); ++it )
  {
    if      (!(it->compare(vana[0])&&it->compare(vana[1])))
      flag_stde=( atoi(fx_argv.arg(*it).c_str()) > 0 );
    else if (!(it->compare(vana[2])&&it->compare(vana[3])))
      flag_rmnl=( atoi(fx_argv.arg(*it).c_str()) > 0 );
    else if (!(it->compare(vana[4])&&it->compare(vana[5])))
      flag_eval=( atoi(fx_argv.arg(*it).c_str()) > 0 );
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
      result = varm.expand_text(result);

    return( result );
  }
}

/***************************************************************************//**

  \details

    For a list of sets: set1, set2, ..., setn, combine each element of
    each set one at a time to form a named n-tuple word connected by a
    configurable joiner string. Each word has the configurable prefix
    added to its' front and configurable suffix added to its' end and are
    appended to the results string with a configurable word separator.

    Each set is identified as any positional argument. Sets with multiple
    members require its members to be enclosed within quotation marks.
    The options are and their short codes are summarized in the following
    table.

     options     | sc  | default        | description
    :-----------:|:---:|:--------------:|:--------------------------------
      prefix     | p   |                | word prefix
      suffix     | s   |                | word suffix
      joiner     | j   | [_]            | set member joiner
      separator  | f   | [,]            | word separator
      tokenizer  | t   | [\|,[:space:]] | set-members tokenizer characters

    The tokenizer character are used to separate the lists of set members.

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
  "tokenizer",  "t",
  };
  set<string> vans(vana, vana + sizeof(vana)/sizeof(string));

  // assign local variable values: positions must match declaration above.
  // do not trim to allow combining of natural language.
  size_t ap=0;
  string prefix     = unquote(fx_argv.arg_firstof(vana[ap],vana[ap+1])); ap+=2;
  string suffix     = unquote(fx_argv.arg_firstof(vana[ap],vana[ap+1])); ap+=2;
  string joiner     = unquote(fx_argv.arg_firstof(vana[ap],vana[ap+1])); ap+=2;
  string separator  = unquote(fx_argv.arg_firstof(vana[ap],vana[ap+1])); ap+=2;
  string tokenizer  = unquote(fx_argv.arg_firstof(vana[ap],vana[ap+1])); ap+=2;

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

  // assign default joiner when not specified.
  if ( ! fx_argv.exists( vana[4] ) && ! fx_argv.exists( vana[5] ) )
    joiner = "_";

  // assign default separator when not specified.
  if ( ! fx_argv.exists( vana[6] ) && ! fx_argv.exists( vana[7] ) )
    separator = ",";

  // assign default tokenizer when not specified.
  if ( ! fx_argv.exists( vana[8] ) && ! fx_argv.exists( vana[9] ) )
    tokenizer = "|, ";

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

    Output a table of images in in \c html or \c latex format. The options
    are and their short codes are summarized in the following table.

     options            | sc  | default | tokenizer | description
    :------------------:|:---:|:-------:|:---------:|:----------------------------
      type              | t   |         |           | table type (html or latex)
      id                | i   |         |           |  table id
      table_width       | tw  |         |           |  table width
      table_heading     | th  |         |           |  table heading
      columns           | c   |    3    |           |  number of columns
      column_headings   | ch  |         |  titles   |  column headings list
      image_width       | ix  |         |           |  width for each image
      image_height      | iy  |         |           |  height for each image
      image_files       | if  |         |  files    |  image files list
      image_titles      | it  |         |  titles   |  image file titles list
      image_headings    | ih  |         |  titles   |  image cell heading list
      image_urls        | iu  |         |  urls     |  image cell reference URL list

    The tokenizer character that separates lists are summarized in the
    following table.

     type   | any of
    :------:|:---------------:
     files  | [,[:space:]]
     titles | [~^]
     urls   | [^\|#[:space:]]

  \todo might be nice to use a more general way of added attributes to the
        table elements using an attributes database based on the environment
        variables. each tag checks the database for existing attributes.

*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_image_table(void)
{
  using namespace UTIL;

  // options declaration: vana & vans.
  // !!DO NOT REORDER WITHOUT UPDATING POSITIONAL DEPENDENCIES BELOW!!
  string vana[] =
  {
  "type",             "t",
  "id",               "i",
  "table_width",      "tw",
  "table_heading",    "th",
  "columns",          "c",
  "column_headings",  "ch",
  "image_width",      "ix",
  "image_height",     "iy",
  "image_files",      "if",
  "image_titles",     "it",
  "image_headings",   "ih",
  "image_urls",       "iu"
  };
  set<string> vans(vana, vana + sizeof(vana)/sizeof(string));

  // assign local variable values: positions must match declaration above.
  size_t ap=0;
  string type             = unquote_trim(fx_argv.arg_firstof(vana[ap],vana[ap+1])); ap+=2;
  string id               = unquote_trim(fx_argv.arg_firstof(vana[ap],vana[ap+1])); ap+=2;
  string table_width      = unquote_trim(fx_argv.arg_firstof(vana[ap],vana[ap+1])); ap+=2;
  string table_heading    = unquote_trim(fx_argv.arg_firstof(vana[ap],vana[ap+1])); ap+=2;
  string columns          = unquote_trim(fx_argv.arg_firstof(vana[ap],vana[ap+1])); ap+=2;
  string column_headings  = unquote_trim(fx_argv.arg_firstof(vana[ap],vana[ap+1])); ap+=2;
  string image_width      = unquote_trim(fx_argv.arg_firstof(vana[ap],vana[ap+1])); ap+=2;
  string image_height     = unquote_trim(fx_argv.arg_firstof(vana[ap],vana[ap+1])); ap+=2;
  string image_files      = unquote_trim(fx_argv.arg_firstof(vana[ap],vana[ap+1])); ap+=2;
  string image_titles     = unquote_trim(fx_argv.arg_firstof(vana[ap],vana[ap+1])); ap+=2;
  string image_headings   = unquote_trim(fx_argv.arg_firstof(vana[ap],vana[ap+1])); ap+=2;
  string image_urls       = unquote_trim(fx_argv.arg_firstof(vana[ap],vana[ap+1])); ap+=2;

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

  // required arguments: type must exists and be one of 'html' or 'latex'
  if ( ! fx_argv.exists( vana[0] ) && ! fx_argv.exists( vana[1] ) )
    return( amu_error_msg("table type must be specified. may be (html|latex).") );
  else if ( type.compare("html") && type.compare("latex") )
    return( amu_error_msg( "type " + type + " is invalid. may be (html|latex).") );

  // update column count default when specified.
  size_t columns_cnt = 3;
  if ( columns.length() ) columns_cnt = atoi( columns.c_str() );

  //
  // tokenize arguments with list members to vectors
  //
  typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

  boost::char_separator<char> fsep(", ");   // file name separators
  boost::char_separator<char> tsep("~^");   // general text separators
  boost::char_separator<char> usep("^|# "); // general text separators

  // list members [ column_headings ]
  tokenizer ct_tok( column_headings, tsep );
  vector<string> ct_v;
  for ( tokenizer::iterator it=ct_tok.begin(); it!=ct_tok.end(); ++it )
    ct_v.push_back( boost::trim_copy( *it ) );

  // list members [ image_files ]
  tokenizer if_tok( image_files, fsep );
  vector<string> if_v;
  for ( tokenizer::iterator it=if_tok.begin(); it!=if_tok.end(); ++it )
    if_v.push_back( boost::trim_copy( *it ) );

  // list members [ image_titles ]
  tokenizer it_tok( image_titles, tsep );
  vector<string> it_v;
  for ( tokenizer::iterator it=it_tok.begin(); it!=it_tok.end(); ++it )
    it_v.push_back( boost::trim_copy( *it ) );

  // list members [ image_headings ]
  tokenizer ih_tok( image_headings, tsep );
  vector<string> ih_v;
  for ( tokenizer::iterator it=ih_tok.begin(); it!=ih_tok.end(); ++it )
    ih_v.push_back( boost::trim_copy( *it ) );

  // list members [ image_urls ]
  tokenizer iu_tok( image_urls, usep );
  vector<string> iu_v;
  for ( tokenizer::iterator it=iu_tok.begin(); it!=iu_tok.end(); ++it )
    iu_v.push_back( boost::trim_copy( *it ) );

  // if specified, their must be a title for every column (vana[column titles])
  if ( (ct_v.size() >0) && (ct_v.size() != columns_cnt) )
    return( amu_error_msg("mismatched " + vana[10] + ": " + to_string(ct_v.size()) +
                          " titles for " + to_string(columns_cnt) + " columns.") );

  // if specified, their must be a title for every file (vana[image titles])
  if ( (it_v.size() >0) && (it_v.size() != if_v.size()) )
    return( amu_error_msg("mismatched " + vana[18] + ": " + to_string(it_v.size()) +
                          " titles for " + to_string(if_v.size()) + " files.") );

  // if specified, their must be a heading for every file (vana[image headings])
  if ( (ih_v.size() >0) && (ih_v.size() != if_v.size()) )
    return( amu_error_msg("mismatched " + vana[20] + ": " + to_string(ih_v.size()) +
                          " headings for " + to_string(if_v.size()) + " files.") );

  // if specified, their must be a url for every file (vana[image urls])
  if ( (iu_v.size() >0) && (iu_v.size() != if_v.size()) )
    return( amu_error_msg("mismatched " + vana[22] + ": " + to_string(iu_v.size()) +
                          " URLs for " + to_string(if_v.size()) + " files.") );


  // embedded newline output control for debugging.
  // when set to true, while useful for visually debugging the output
  // of this filter, may cause Doxygen to produce erroneous results.
  #define IDINL if(_NLE_) result.append("\n")
  bool _NLE_ = false;

  string result;

  // ----------------------------------------------------------------- //
  //                            html table
  // ----------------------------------------------------------------- //
  if ( type.compare("html")==0 ) {

    string table_attr           = " border=\"0\"";
    string column_heading_attr  = " bgcolor=\"#E2E8F2\"";

    // begin table
    result.append("<table align=\"center\"");
    if ( table_width.length() ) result.append(" style=\"width:" + table_width + "\"");
    result.append( table_attr );
    result.append(">");
    IDINL;

    // table heading and id
    if( table_heading.length() ) {
      result.append("<caption");
      if ( id.length() ) result.append(" id=\"" + id + "\"");
      result.append(">" + table_heading + "</caption>");
      IDINL;
    }


    // column headings
    if( column_headings.length() ) {
      result.append("<tr>");
      for( size_t i=0; i<columns_cnt; ++i )
        result.append("<th" + column_heading_attr + ">" + ct_v[i] + "</th>");
      result.append("</tr>");
      IDINL;
    }

    // iterate over image list
    int if_num = 0;
    for ( vector<string>::const_iterator it=if_v.begin(); it!=if_v.end(); ++it )
    {
      const string file = *it;

      // check new row
      if ( (if_num%columns_cnt) == 0 ) {

        // image headings
        if( image_headings.length() ) {
          result.append("<tr>");
          for( size_t i=0; i<columns_cnt; ++i )
            if( (if_num + i) < if_v.size() )
              result.append("<th>" + ih_v[if_num + i] + "</th>");
          result.append("</tr>");
          IDINL;
        }

        // begin row
        result.append("<tr>");
        IDINL;
      }

      // begin cell
      result.append("<td>");

      // begin image
      bool found = false;
      if ( image_urls.length() )
        result.append( "<a href=\""
                     + file_rl( iu_v[if_num], found, true, true, true, get_html_output(), false )
                     + "\">" );

      result.append( "<img src=\""
                   + file_rl( file, found, true, false, true, get_html_output(), false )
                   + "\"" );

      if ( image_titles.length() ) {
        result.append(" title=\"" + it_v[if_num] + "\"");
        result.append(" alt=\"" + it_v[if_num] + "\"");
      }

      if ( image_width.length() ) result.append(" width=\"" + image_width + "\"");
      if ( image_height.length() ) result.append(" height=\"" + image_height + "\"");

      // end image
      result.append(">");
      if ( image_urls.length() ) result.append("</a>");

      // end cell
      result.append("</td>");
      IDINL;

      // end row
      if ( (if_num%columns_cnt) == (columns_cnt-1) ) {
        result.append("</tr>");
        IDINL;
      }

      if_num++;
    }

    // end table
    result.append("</table>");
  }

  // ----------------------------------------------------------------- //
  //                            latex table
  // ----------------------------------------------------------------- //
  if ( type.compare("latex")==0 ) {

    // begin table
    result.append("\\begin{table}[h]");
    IDINL;

    // SKIPPING: table width.
    // if ( table_width.length() ) result.append("" + table_width + "");
    // IDINL;

    // table heading
    if( table_heading.length() ) {
      result.append("\\caption{" + table_heading + "}" );
      IDINL;
    }

    // table id
    if ( id.length() ) {
      result.append("\\label{" + id + "}");
      IDINL;
    }

    result.append("\\begin{center}");
    IDINL;

    result.append("\\begin{tabular}{");
    for( size_t i=0; i<columns_cnt; ++i ) result.append("c");
    result.append("}");
    IDINL;

    // column headings
    if( column_headings.length() ) {
      result.append("\\hline ");
      IDINL;
      for( size_t i=0; i<columns_cnt; ++i ) {
        result.append( ct_v[i] );
        if ( i < (columns_cnt-1) ) result.append( "&" );
        else                       result.append( "\\\\" );
        IDINL;
      }
      result.append("\\hline");
      result.append("\\\\[2pt]");
      IDINL;
    }

    // iterate over image list
    int if_num = 0;
    for ( vector<string>::const_iterator it=if_v.begin(); it!=if_v.end(); ++it )
    {
      const string file = *it;

      // check new row
      if ( (if_num%columns_cnt) == 0 ) {
        // image headings
        if( image_headings.length() ) {
          for( size_t i=0; i<columns_cnt; ++i ) {
            if( (if_num + i) < if_v.size() )  result.append( ih_v[if_num + i] );
            else                              result.append( "~" );

            if ( i < (columns_cnt-1) )        result.append( "&" );
            else                              result.append( "\\\\" );
            IDINL;
          }
        }
        // begin row
      }

      // begin cell

      // begin image
      bool found = false;
      if ( image_urls.length() )
        result.append( "\\href{"
                     + file_rl( iu_v[if_num], found, true, true, true, get_latex_output(), false )
                     + "}{" );

      result.append("\\includegraphics");

      // assemble attributes
      string attr;
      if ( image_width.length() ) {
        if ( attr.length() )          attr.append(",");
                                      attr.append("width=" + image_width); }
      if ( image_height.length() ) {
        if ( attr.length() )          attr.append(",");
                                      attr.append("height=" + image_height); }

      if ( attr.length() )            result.append( "[" + attr + "]" );

      result.append( "{"
                   + file_rl( file, found, false, false, true, get_latex_output(), false )
                   + "}" );

      if ( image_urls.length() ) result.append( "}");

      // end image
      // end cell

      // check end row
      if ( (if_num%columns_cnt) == (columns_cnt-1) ) {
        result.append("\\\\");
        IDINL;

        if( image_titles.length() ) {
          for( size_t i=0; i<columns_cnt; ++i ) {
            int os=1 - columns_cnt;
            if( (if_num + i + os) < if_v.size() ) result.append( it_v[if_num + i + os] );
            else                                  result.append( "~" );

            if ( i < (columns_cnt-1) )            result.append( "&" );
            else                                  result.append( "\\\\[2pt]" );

            IDINL;
          }
        }
      } else {
        result.append("&");
        IDINL;
      }

      if_num++;
    }

    // final partial row
    if ( (if_num%columns_cnt) != 0 ) {
      // fill any remaining cells with "~" to complete the partial row
      for( size_t i=( (if_num%columns_cnt) ) ; i<(columns_cnt-1); ++i ) {
        result.append("~&");
        IDINL;
      }
      // end
      result.append("~\\\\");
      IDINL;

      // partial row titles
      if( image_titles.length() ) {
        for( size_t i=0; i<columns_cnt; ++i ) {
          int os=0 - if_num%columns_cnt;
          if( (if_num + i + os) < if_v.size() ) result.append( it_v[if_num + i + os] );
          else                                  result.append( "~" );

          if ( i < (columns_cnt-1) )            result.append( "&" );
          else                                  result.append( "\\\\" );

          IDINL;
        }
      }
    }

    // end table
    result.append("\\end{tabular}"); IDINL;
    result.append("\\end{center}"); IDINL;
    result.append("\\end{table}");
  }


  return( result );
}

/***************************************************************************//**

  \details

  potential javascript viewers: png, stl, svg, movie...

*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_viewer(void)
{
  string result;

  result = amu_error_msg("unimplemented.");

  return( result );
}

/***************************************************************************//**

  \details

    Run make on a target of a makefile generated by a MFScript embedded
    in a scoped comment section of the input file and capture the result.

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

    The scope identifier is initially the default root scope (specified by the
    program option set on the command line or in a configuration file). Each
    argument is then processes to modify the scope identifier according to the
    description in the above table. All positional arguments are treated as
    append modifiers.

    A makefile build script can generate targets using <tt>*.scad</tt> that
    comes from either an auxiliary script embedded in the \em scope together with
    the build script or the the parent \em source in which the scope is embedded.
    The flagg \p pstarget is used to distinguish between the target origins.
    Setting \p ++pstarget will use targets generated by the parent \em source file.

  \note Under normal operation, the output text from the makefile target should
        be contained within a single line so as to not change the line count of
        the source file.

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

  // handle configuration prefix if not current directory (or empty)
  if ( get_config_prefix().compare(".") && get_config_prefix().length() )
    opts += " --directory=" + get_config_prefix();

  scmd = get_make_path() + opts
       + " -f " + makefile_stem + get_makefile_ext()
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

    Copy the list of files to the output directory for the specified
    documentation types.

     options    | sc  | default | description
    :----------:|:---:|:-------:|:-----------------------------
      files     | f   |         | list of files
      types     | t   |         | html, latex, docbook, or rtf

    The tokenizer character that separates lists are summarized in the
    following table.

     type     | any of
    :--------:|:------------:
     files    | [,[:space:]]
     types    | [,[:space:]]

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

  size_t ap=4;

  // generate options help string.
  string help = "options: [";
  for(size_t it=0; it < ap; it+=2) {
    if (it) help.append( ", " );
    help.append( vana[it] + " (" + vana[it+1] + ")" );
  }
  help.append( "]" );

  // variables
  string files;
  string types;

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

      bool found=false;
      string rl = file_rl(file, found, true, false, true, path, false);

      if ( found == false )
        result.append(" " + rl);

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


/*******************************************************************************
// eof
*******************************************************************************/
