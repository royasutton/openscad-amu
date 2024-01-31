/***************************************************************************//**

  \file   openscad_dif_scanner_bif4.cpp

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
    OpenSCAD Doxygen input filter scanner class built-in functions source.

  \ingroup openscad_dif_src
*******************************************************************************/

#include "openscad_dif_scanner.hpp"
#include <set>

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

using namespace std;


/***************************************************************************//**
  \details


    Output a table formatted from a list of text phrases. The options
    and their short codes are summarized in the following table.

     options            | sc  | default  | tokenizer | description
    :------------------:|:---:|:--------:|:---------:|:----------------------------
      id                | i   |          |           |  table id
      table_caption     | t   |          |           |  table caption
      columns           | c   |    6     |           |  number of columns
      table_class       | sc  | doxtable |           |  table css class
      column_headings   | chl |          |  titles   |  column headings list
      cell_texts        | cdl |          |  titles   |  cell text list
      cell_captions     | ccl |          |  titles   |  cell caption list
      cell_urls         | cul |          |  urls     |  cell URL list

    The tokenizer character that separates list members are summarized in the
    following table.

     type   | any of
    :------:|:---------------:
     titles | [~^]
     urls   | [^\|#[:space:]]

    For more information on how to specify and use function arguments
    see \ref openscad_dif_sm_a.

*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_table(void)
{
  using namespace UTIL;

  // options declaration: vana & vans.
  // !!DO NOT REORDER WITHOUT UPDATING POSITIONAL DEPENDENCIES BELOW!!
  string vana[] =
  {
  "id",               "i",
  "table_caption",    "t",
  "columns",          "c",
  "table_class",      "sc",
  "column_headings",  "chl",
  "cell_texts",       "cdl",
  "cell_captions",    "ccl",
  "cell_urls",        "cul"
  };
  set<string> vans(vana, vana + sizeof(vana)/sizeof(string));

  // assign local variable values: positions must match declaration above.
  size_t ap=0;
  string id               = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string table_caption    = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string columns          = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string table_class      = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string column_headings  = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string cell_texts       = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string cell_captions    = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string cell_urls        = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;

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

  // apply default: columns.
  size_t columns_cnt = 6;
  if ( columns.length() ) {
    if ( is_number( columns ) )
      columns_cnt = atoi( columns.c_str() );
    else
      return( amu_error_msg(vana[4] + "=[" + columns + "] is invalid.") );
  }

  // apply default: table_class
  if ( table_class.empty() ) table_class = "doxtable";

  //
  // tokenize arguments with list members to vectors
  //
  typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

  boost::char_separator<char> tsep("~^");   // general text separators
  boost::char_separator<char> usep("^|# "); // url text separators

  // column_headings
  tokenizer chl_tok( column_headings, tsep );
  vector<string> chl_v;
  for ( tokenizer::iterator it=chl_tok.begin(); it!=chl_tok.end(); ++it )
    chl_v.push_back( boost::trim_copy( *it ) );

  // cell_texts
  tokenizer cdl_tok( cell_texts, tsep );
  vector<string> cdl_v;
  for ( tokenizer::iterator it=cdl_tok.begin(); it!=cdl_tok.end(); ++it )
    cdl_v.push_back( boost::trim_copy( *it ) );

  // cell_captions
  tokenizer ccl_tok( cell_captions, tsep );
  vector<string> ccl_v;
  for ( tokenizer::iterator it=ccl_tok.begin(); it!=ccl_tok.end(); ++it )
    ccl_v.push_back( boost::trim_copy( *it ) );

  // cell_urls
  tokenizer cul_tok( cell_urls, usep );
  vector<string> cul_v;
  for ( tokenizer::iterator it=cul_tok.begin(); it!=cul_tok.end(); ++it )
    cul_v.push_back( boost::trim_copy( *it ) );

  // must be a heading for every column (vana[ column_headings ])
  if ( (chl_v.size() >0) && (chl_v.size() != columns_cnt) )
    return( amu_error_msg("mismatched " + vana[8] + ": " + to_string(chl_v.size()) +
                          " headings for " + to_string(columns_cnt) + " columns.") );

  // must be a caption for every cell (vana[ cell_captions ])
  if ( (ccl_v.size() >0) && (ccl_v.size() != cdl_v.size()) )
    return( amu_error_msg("mismatched " + vana[12] + ": " + to_string(ccl_v.size()) +
                          " captions for " + to_string(cdl_v.size()) + " cells.") );

  // must be a url for every cell (vana[ cell_urls ])
  if ( (cul_v.size() >0) && (cul_v.size() != cdl_v.size()) )
    return( amu_error_msg("mismatched " + vana[14] + ": " + to_string(cul_v.size()) +
                          " URLs for " + to_string(cdl_v.size()) + " cells.") );


  // embedded newline output control for debugging.
  // when set to true, while useful for visually debugging the output
  // of this filter, may cause Doxygen to produce erroneous results.
  #define IDINL if(_NLE_) result.append("\n")
  bool _NLE_ = false;

  string result;

  // ----------------------------------------------------------------- //
  //                            html table
  // ----------------------------------------------------------------- //

  // begin table
  IDINL;
  result.append("<table class=\"" + table_class + "\">");
  IDINL;

  // table_caption and optional id
  if( table_caption.length() ) {
    result.append("<caption");
    if ( id.length() ) result.append(" id=\"" + id + "\"");
    result.append(">" + table_caption + "</caption>");
    IDINL;
  }

  // column_headings
  if( column_headings.length() ) {
    result.append("<tr>");
    for( size_t i=0; i<columns_cnt; ++i )
      result.append("<th>" + chl_v[i] + "</th>");
    result.append("</tr>");
    IDINL;
  }

  // iterate over cells
  int cdl_num = 0;
  for ( vector<string>::const_iterator it=cdl_v.begin(); it!=cdl_v.end(); ++it )
  {
    const string text = *it;

    // check new row
    if ( (cdl_num%columns_cnt) == 0 ) {
      // cell captions
      if( cell_captions.length() ) {
        result.append("<tr>");
        for( size_t i=0; i<columns_cnt; ++i )
          if( (cdl_num + i) < cdl_v.size() )
            result.append("<th>" + ccl_v[cdl_num + i] + "</th>");
        result.append("</tr>");
        IDINL;
      }

      // begin row
      result.append("<tr>");
      IDINL;
    }

    // begin cell text
    result.append("<td>");

    bool found = false;
    if ( cell_urls.length() ) result.append( "<a href=\""
        + file_rl( cul_v[cdl_num], get_html_output(), found )
        + "\">" );

    result.append( text );

    if ( cell_urls.length() ) result.append("</a>");

    // end cell
    result.append("</td>");
    IDINL;

    // end row
    if ( (cdl_num%columns_cnt) == (columns_cnt-1) ) {
      result.append("</tr>");
      IDINL;
    }

    cdl_num++;
  }

  // end table
  result.append("</table>");

  return( result );
}

/***************************************************************************//**
  \details

    Output an image in \c html or \c latex format. The options and
    their short codes are summarized in the following table.

     options            | sc  | default  | description
    :------------------:|:---:|:--------:|:----------------------------
      type              | o   | html     | output type (html or latex)
      caption           | c   |          | caption
      class             | d   | image    | css div class
      width             | w   |          | width
      height            | h   |          | height
      begin             | b   |          | div initial text
      end               | e   |          | div final text
      file              | f   |          | image file
      title             | t   |          | hover title
      url               | u   |          | selection URL

    When using \p url, the function may need to be enclosed by \p
    \\htmlonly and \p \\endhtmlonly to prevent Doxygen from rewriting
    the assigned url. Text with Doxygen managed links may be placed in
    the \p begin and \p end text.

    \b Example
    \verbatim
    \amu_image
    (
      caption="A Cone"
        begin="See \ref library for details."
          end="(Cone in [wikipedia])"
         file="${get_png_file_stem}.png"
        title="Base radius 4 top radius 1."
    )
    \endverbatim

    For more information on how to specify and use function arguments
    see \ref openscad_dif_sm_a.

*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_image(void)
{
  using namespace UTIL;

  // options declaration: vana & vans.
  // !!DO NOT REORDER WITHOUT UPDATING POSITIONAL DEPENDENCIES BELOW!!
  string vana[] =
  {
  "type",     "o",
  "caption",  "c",
  "class",    "d",
  "width",    "w",
  "height",   "h",
  "begin",    "b",
  "end",      "e",
  "file",     "f",
  "title",    "t",
  "url",      "u",
  };
  set<string> vans(vana, vana + sizeof(vana)/sizeof(string));

  // assign local variable values: positions must match declaration above.
  size_t ap=0;
  string type       = unquote_trim(fx_argv.arg_firstof("html"  ,vana[ap],vana[ap+1])); ap+=2;
  string caption    = unquote_trim(fx_argv.arg_firstof(""      ,vana[ap],vana[ap+1])); ap+=2;
  string div_class  = unquote_trim(fx_argv.arg_firstof("image" ,vana[ap],vana[ap+1])); ap+=2;
  string width      = unquote_trim(fx_argv.arg_firstof(""      ,vana[ap],vana[ap+1])); ap+=2;
  string height     = unquote_trim(fx_argv.arg_firstof(""      ,vana[ap],vana[ap+1])); ap+=2;
  string div_begin  = unquote_trim(fx_argv.arg_firstof(""      ,vana[ap],vana[ap+1])); ap+=2;
  string div_end    = unquote_trim(fx_argv.arg_firstof(""      ,vana[ap],vana[ap+1])); ap+=2;
  string file       = unquote_trim(fx_argv.arg_firstof(""      ,vana[ap],vana[ap+1])); ap+=2;
  string title      = unquote_trim(fx_argv.arg_firstof(""      ,vana[ap],vana[ap+1])); ap+=2;
  string url        = unquote_trim(fx_argv.arg_firstof(""      ,vana[ap],vana[ap+1])); ap+=2;

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
  if ( type.compare("html") && type.compare("latex") )
    return( amu_error_msg( "type " + type + " is invalid. may be (html|latex).") );

  string result;

  // ----------------------------------------------------------------- //
  //                            html image
  // ----------------------------------------------------------------- //
  if ( type.compare("html")==0 )
  {
    // div
    result.append("<div class=\"" + div_class + "\">");

    // div begin text
    if ( div_begin.length() ) result.append( div_begin );

    bool found = false;
    string output = get_html_output();

    // url
    if ( url.length() )
      result.append( "<a href=\"" + file_rl( url, output, found ) + "\">" );

    // image
    result.append( "<img src=\"" + file_rl( file, output, found ) + "\"" );

    // title
    if ( title.length() )
    {
      result.append(" title=\"" + title + "\"");
      result.append(" alt=\"" + title + "\"");
    }

    if ( width.length() ) result.append(" width=\"" + width + "\"");
    if ( height.length() ) result.append(" height=\"" + height + "\"");

    // end image
    result.append(">");

    // end url
    if ( url.length() ) result.append("</a>");

    // caption
    if ( caption.length() )
      result.append("<div class=\"caption\">" + caption + "</div>");

    // div end text
    if ( div_end.length() ) result.append( div_end );

    // div
    result.append("</div>");
  }

  // ----------------------------------------------------------------- //
  //                            latex image
  // ----------------------------------------------------------------- //
  if ( type.compare("latex")==0 )
  {
    //! \todo implement code for amu_image latex output
    result = amu_error_msg("unimplemented.");
  }

  return( result );
}

/***************************************************************************//**
  \details

    Output a table of images in \c html or \c latex format. The options
    and their short codes are summarized in the following table.

     options            | sc  | default  | tokenizer | description
    :------------------:|:---:|:--------:|:---------:|:----------------------------
      type              | f   |          |           |  table type (html or latex)
      id                | i   |          |           |  table id
      table_caption     | t   |          |           |  table caption
      columns           | c   |    3     |           |  number of columns
      table_class       | sc  | doxtable |           |  table css class
      image_width       | iw  |          |           |  width for each image
      image_height      | ih  |          |           |  height for each image
      column_headings   | chl |          |  titles   |  column headings list
      cell_begin        | cdb |          |  titles   |  initial text for each cell
      cell_end          | cde |          |  titles   |  final text for each cell
      cell_files        | cdl |          |  files    |  cell image list
      cell_titles       | ctl |          |  titles   |  cell title list
      cell_captions     | ccl |          |  titles   |  cell caption list
      cell_urls         | cul |          |  urls     |  cell URL list

    The tokenizer character that separates list members are summarized in the
    following table.

     type   | any of
    :------:|:---------------:
     files  | [,[:space:]]
     titles | [~^]
     urls   | [^\|#[:space:]]

    When using \p cell_urls, the function may need to be enclosed by \p
    \\htmlonly and \p \\endhtmlonly to prevent Doxygen from rewriting
    the assigned urls as show below.

    \verbatim
      // image table with cell urls

      \htmlonly
        \amu_image_table (
            type=html table_caption="Downloadable"
            cell_files="${png_files}" cell_captions="${titles}"
            cell_urls="${stl_files}"
        )
      \endhtmlonly
    \endverbatim

    For more information on how to specify and use function arguments
    see \ref openscad_dif_sm_a.

*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_image_table(void)
{
  using namespace UTIL;

  // options declaration: vana & vans.
  // !!DO NOT REORDER WITHOUT UPDATING POSITIONAL DEPENDENCIES BELOW!!
  string vana[] =
  {
  "type",             "f",
  "id",               "i",
  "table_caption",    "t",
  "columns",          "c",
  "table_class",      "sc",
  "image_width",      "iw",
  "image_height",     "ih",
  "column_headings",  "chl",
  "cell_begin",       "cdb",
  "cell_end",         "cde",
  "cell_files",       "cdl",
  "cell_titles",      "ctl",
  "cell_captions",    "ccl",
  "cell_urls",        "cul"
  };
  set<string> vans(vana, vana + sizeof(vana)/sizeof(string));

  // assign local variable values: positions must match declaration above.
  size_t ap=0;
  string type             = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string id               = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string table_caption    = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string columns          = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string table_class      = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string image_width      = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string image_height     = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string column_headings  = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string cell_begin       = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string cell_end         = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string cell_files       = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string cell_titles      = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string cell_captions    = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;
  string cell_urls        = unquote_trim(fx_argv.arg_firstof("",vana[ap],vana[ap+1])); ap+=2;

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

  // apply default: columns.
  size_t columns_cnt = 3;
  if ( columns.length() ) {
    if ( is_number( columns ) )
      columns_cnt = atoi( columns.c_str() );
    else
      return( amu_error_msg(vana[6] + "=[" + columns + "] is invalid.") );
  }

  // apply default: table_class
  if ( table_class.empty() ) table_class = "doxtable";

  //
  // tokenize arguments with list members to vectors
  //
  typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

  boost::char_separator<char> fsep(", ");   // file name separators
  boost::char_separator<char> tsep("~^");   // general text separators
  boost::char_separator<char> usep("^|# "); // url text separators

  // column_headings
  tokenizer chl_tok( column_headings, tsep );
  vector<string> chl_v;
  for ( tokenizer::iterator it=chl_tok.begin(); it!=chl_tok.end(); ++it )
    chl_v.push_back( boost::trim_copy( *it ) );

  // cell_begin
  tokenizer cdb_tok( cell_begin, tsep );
  vector<string> cdb_v;
  for ( tokenizer::iterator it=cdb_tok.begin(); it!=cdb_tok.end(); ++it )
    cdb_v.push_back( boost::trim_copy( *it ) );

  // cell_end
  tokenizer cde_tok( cell_end, tsep );
  vector<string> cde_v;
  for ( tokenizer::iterator it=cde_tok.begin(); it!=cde_tok.end(); ++it )
    cde_v.push_back( boost::trim_copy( *it ) );

  // cell_files
  tokenizer cdl_tok( cell_files, fsep );
  vector<string> cdl_v;
  for ( tokenizer::iterator it=cdl_tok.begin(); it!=cdl_tok.end(); ++it )
    cdl_v.push_back( boost::trim_copy( *it ) );

  // cell_titles
  tokenizer ctl_tok( cell_titles, tsep );
  vector<string> ctl_v;
  for ( tokenizer::iterator it=ctl_tok.begin(); it!=ctl_tok.end(); ++it )
    ctl_v.push_back( boost::trim_copy( *it ) );

  // cell_captions
  tokenizer ccl_tok( cell_captions, tsep );
  vector<string> ccl_v;
  for ( tokenizer::iterator it=ccl_tok.begin(); it!=ccl_tok.end(); ++it )
    ccl_v.push_back( boost::trim_copy( *it ) );

  // cell_urls
  tokenizer cul_tok( cell_urls, usep );
  vector<string> cul_v;
  for ( tokenizer::iterator it=cul_tok.begin(); it!=cul_tok.end(); ++it )
    cul_v.push_back( boost::trim_copy( *it ) );

  // must be a heading for every column (vana[ column_headings ])
  if ( (chl_v.size() >0) && (chl_v.size() != columns_cnt) )
    return( amu_error_msg("mismatched " + vana[14] + ": " + to_string(chl_v.size()) +
                          " headings for " + to_string(columns_cnt) + " columns.") );

  // must be a begin for every cell (vana[ cell_begin ])
  if ( (cdb_v.size() >0) && (cdb_v.size() != cdl_v.size()) )
    return( amu_error_msg("mismatched " + vana[16] + ": " + to_string(cdb_v.size()) +
                          " begins for " + to_string(cdl_v.size()) + " cells.") );

  // must be an end for every cell (vana[ cell_end ])
  if ( (cde_v.size() >0) && (cde_v.size() != cdl_v.size()) )
    return( amu_error_msg("mismatched " + vana[18] + ": " + to_string(cde_v.size()) +
                          " ends for " + to_string(cdl_v.size()) + " cells.") );

  // must be a title for every cell (vana[ cell_titles ])
  if ( (ctl_v.size() >0) && (ctl_v.size() != cdl_v.size()) )
    return( amu_error_msg("mismatched " + vana[22] + ": " + to_string(ctl_v.size()) +
                          " titles for " + to_string(cdl_v.size()) + " cells.") );

  // must be a caption for every cell (vana[ cell_captions ])
  if ( (ccl_v.size() >0) && (ccl_v.size() != cdl_v.size()) )
    return( amu_error_msg("mismatched " + vana[24] + ": " + to_string(ccl_v.size()) +
                          " captions for " + to_string(cdl_v.size()) + " cells.") );

  // must be a url for every cell (vana[ cell_urls ])
  if ( (cul_v.size() >0) && (cul_v.size() != cdl_v.size()) )
    return( amu_error_msg("mismatched " + vana[26] + ": " + to_string(cul_v.size()) +
                          " URLs for " + to_string(cdl_v.size()) + " cells.") );


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

    // begin table
    IDINL;
    result.append("<table class=\"" + table_class + "\">");
    IDINL;

    // table caption and id
    if( table_caption.length() ) {
      result.append("<caption");
      if ( id.length() ) result.append(" id=\"" + id + "\"");
      result.append(">" + table_caption + "</caption>");
      IDINL;
    }

    // column headings
    if( column_headings.length() ) {
      result.append("<tr>");
      for( size_t i=0; i<columns_cnt; ++i )
        result.append("<th>" + chl_v[i] + "</th>");
      result.append("</tr>");
      IDINL;
    }

    // iterate over cells
    int cdl_num = 0;
    for ( vector<string>::const_iterator it=cdl_v.begin(); it!=cdl_v.end(); ++it )
    {
      const string file = *it;

      // check new row
      if ( (cdl_num%columns_cnt) == 0 ) {
        // image headings
        if( cell_captions.length() ) {
          result.append("<tr>");
          for( size_t i=0; i<columns_cnt; ++i )
            if( (cdl_num + i) < cdl_v.size() )
              result.append("<th>" + ccl_v[cdl_num + i] + "</th>");
          result.append("</tr>");
          IDINL;
        }

        // begin row
        result.append("<tr>");
        IDINL;
      }

      // begin cell
      result.append("<td>");

      // first text for each cell
      if ( cell_begin.length() ) result.append( cdb_v[cdl_num] );

      // begin cell image
      bool found = false;
      if ( cell_urls.length() ) result.append( "<a href=\""
          + file_rl( cul_v[cdl_num], get_html_output(), found )
          + "\">" );

      result.append( "<img src=\"" + file_rl( file, get_html_output(), found ) + "\"" );

      if ( cell_titles.length() ) {
        result.append(" title=\"" + ctl_v[cdl_num] + "\"");
        result.append(" alt=\"" + ctl_v[cdl_num] + "\"");
      }

      if ( image_width.length() ) result.append(" width=\"" + image_width + "\"");
      if ( image_height.length() ) result.append(" height=\"" + image_height + "\"");

      // end cell image
      result.append(">");
      if ( cell_urls.length() ) result.append("</a>");

      // last text for each cell
      if ( cell_end.length() ) result.append( cde_v[cdl_num] );

      // end cell
      result.append("</td>");
      IDINL;

      // end row
      if ( (cdl_num%columns_cnt) == (columns_cnt-1) ) {
        result.append("</tr>");
        IDINL;
      }

      cdl_num++;
    }

    // end table
    result.append("</table>");
  }

  // ----------------------------------------------------------------- //
  //                            latex table
  // ----------------------------------------------------------------- //
  if ( type.compare("latex")==0 ) {

    // begin table
    IDINL;
    result.append("\\begin{table}[h]");
    IDINL;

    // SKIPPING: table_class, local formatting required here.

    // table caption
    if( table_caption.length() ) {
      result.append("\\caption{" + table_caption + "}" );
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
        result.append( chl_v[i] );
        if ( i < (columns_cnt-1) ) result.append( "&" );
        else                       result.append( "\\\\" );
        IDINL;
      }
      result.append("\\hline");
      result.append("\\\\[2pt]");
      IDINL;
    }

    // iterate over cells
    int cdl_num = 0;
    for ( vector<string>::const_iterator it=cdl_v.begin(); it!=cdl_v.end(); ++it )
    {
      const string file = *it;

      // check new row
      if ( (cdl_num%columns_cnt) == 0 ) {
        // image captions
        if( cell_captions.length() ) {
          for( size_t i=0; i<columns_cnt; ++i ) {
            if( (cdl_num + i) < cdl_v.size() ) result.append( ccl_v[cdl_num + i] );
            else                               result.append( "~" );

            if ( i < (columns_cnt-1) )         result.append( "&" );
            else                               result.append( "\\\\" );
            IDINL;
          }
        }
        // begin row
      }

      // begin cell

      // begin cell image
      bool found = false;
      if ( cell_urls.length() ) result.append( "\\href{"
          + file_rl( cul_v[cdl_num], get_latex_output(), found )
          + "}{" );

      result.append("\\includegraphics");

      // assemble attributes
      string attr;
      if ( image_width.length() ) {
        if ( attr.length() )       { attr.append(","); }
                                     attr.append("width=" + image_width); }
      if ( image_height.length() ) {
        if ( attr.length() )       { attr.append(","); }
                                     attr.append("height=" + image_height); }

      if ( attr.length() )           result.append( "[" + attr + "]" );

      result.append( "{" + file_rl( file, get_latex_output(), found, false ) + "}" );

      if ( cell_urls.length() ) result.append( "}");

      // end cell image

      // end cell

      // check end row
      if ( (cdl_num%columns_cnt) == (columns_cnt-1) ) {
        result.append("\\\\");
        IDINL;

        if( cell_titles.length() ) {
          for( size_t i=0; i<columns_cnt; ++i ) {
            int os=1 - columns_cnt;
            if( (cdl_num + i + os) < cdl_v.size() ) result.append( ctl_v[cdl_num + i + os] );
            else                                    result.append( "~" );

            if ( i < (columns_cnt-1) )              result.append( "&" );
            else                                    result.append( "\\\\[2pt]" );

            IDINL;
          }
        }
      } else {
        result.append("&");
        IDINL;
      }

      cdl_num++;
    }

    // final partial row
    if ( (cdl_num%columns_cnt) != 0 ) {
      // fill any remaining cells with "~" to complete the partial row
      for( size_t i=( (cdl_num%columns_cnt) ) ; i<(columns_cnt-1); ++i ) {
        result.append("~&");
        IDINL;
      }
      // end
      result.append("~\\\\");
      IDINL;

      // partial row titles
      if( cell_titles.length() ) {
        for( size_t i=0; i<columns_cnt; ++i ) {
          int os=0 - cdl_num%columns_cnt;
          if( (cdl_num + i + os) < cdl_v.size() ) result.append( ctl_v[cdl_num + i + os] );
          else                                    result.append( "~" );

          if ( i < (columns_cnt-1) )              result.append( "&" );
          else                                    result.append( "\\\\" );

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

    javascript viewers: png, stl, svg, movie...

    For more information on how to specify and use function arguments
    see \ref openscad_dif_sm_a.

*******************************************************************************/
string
ODIF::ODIF_Scanner::bif_viewer(void)
{
  string result;

  result = amu_error_msg("unimplemented.");

  return( result );
}


/*******************************************************************************
// eof
*******************************************************************************/
