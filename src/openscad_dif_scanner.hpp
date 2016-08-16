/***************************************************************************//**

  \file   openscad_dif_scanner.hpp

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
    OpenSCAD Doxygen input filter scanner class header.

  \ingroup openscad_dif_src
*******************************************************************************/

#ifndef __ODIF_SCANNER_HPP__
#define __ODIF_SCANNER_HPP__ 1

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "openscad_dif_util.hpp"

//! \ingroup openscad_dif_src
//! @{

namespace ODIF{

//! Class that implements the OpenSCAD Doxygen input filter scanner.
class ODIF_Scanner : public yyFlexLexer{
  public:
    using FlexLexer::yylex;

    //! \brief scanner constructor.
    //! \param f  scanner input file name.
    //! \param s  scanner output prefix text string.
    ODIF_Scanner(const std::string& f, const std::string& s="openscad-dif: ");
    //! destructor.
    ~ODIF_Scanner(void);

    // scan() implementation generated by flex. see #define YY_DECL in lexer source.
    //! start scanning input file.
    int scan(void);

    //! reset scanner to initial condition.
    void init(void);

    //! turn scanner debugging on or off.
    void set_debug(bool f) { yy_flex_debug = f; }

    //! turn filter debugging output on or off.
    void set_debug_filter(bool f) { debug_filter = f; }

    //! set the output prefix text string.
    void set_ops(const std::string& s) { ops = s; }
    //! get the output prefix text string.
    std::string get_ops(void) { return ops; }

    //! set the base path to the package library.
    void set_lib_path(const std::string& s) { lib_path = s; }
    //! get the base path of the package library.
    std::string get_lib_path(void) { return lib_path; }

  private:
    // scanner
    bool scanner_output_on;             //!< scanner output on.
    bool debug_filter;                  //!< filter debugging output.

    std::string ops;                    //!< output prefix string.

    std::string input_name;             //!< scanner input file name.
    std::ifstream input_file;           //!< scanner input file.

    std::string lib_path;               //!< path the library.

    //! write the entire string s to the scanner output.
    void scanner_output(const std::string& s) { scanner_output( s.c_str(), s.length() ); }
    //! write the first size characters of buf to the scanner output.
    void scanner_output(const char* buf, int size);
    //! copy to entire lexed text (yytext) to the scanner output.
    void scanner_echo(void) { scanner_output( YYText(), YYLeng() ); }

    //! output the error message m and abort the scanner.
    void abort(const std::string& m, const int &n = 0, const std::string &t = "");

    //! generate standard error message string with message m.
    std::string amu_error_msg(const std::string& m);

    //! return word number n from string w.
    std::string get_word(const std::string& w, const int n);

    //! remove all characters in c from from string s.
    std::string remove_chars(const std::string &s, const std::string &c);

    //! replace all characters in c in string s with replacement character r.
    std::string replace_chars(const std::string &s, const std::string &c, const char r='\0');

    //! unquote outermost matching quotation characters, '' or "", from string.
    std::string unquote(const std::string &s);

    //! unquote outermost matching quotation characters and trim whitespace.
    std::string unquote_trim(const std::string &s) { return ( boost::trim_copy(unquote(s)) ); }

    //! convert a long integer to a string.
    std::string to_string(const long v) {
      std::ostringstream os;
      os << std::dec << v;
      return ( os.str() );
    }

    env_var     varm;                   //!< scanner environment variable map.


    // amu parsed text
    std::string amu_parsed_text;        //!< complete text of parsed amu commands.
    size_t      amu_parsed_leng;        //!< character count parsed for 'amu_parsed_text'.

    //! clear the complete text of parsed amu commands.
    void apt_clear(void) { amu_parsed_text.clear(); amu_parsed_leng=0; }
    //! append the current match text to complete text of parsed amu commands.
    void apt(void) { amu_parsed_text+=YYText(); amu_parsed_leng+=YYLeng(); }


    // amu function
    std::string fx_name;                //!< parsed amu function name.
    std::string fx_tovar;               //!< parsed amu function output variable name.
    func_args   fx_argv;                //!< parsed amu function arguments class.

    std::string fx_qarg;                //!< parsed amu quoted argument string.

    size_t      fi_bline;               //!< beginning line of parsed amu function.
    size_t      fi_eline;               //!< ending line of parsed amu function.

    //! store parsed function name, reset related state, and begin parsing amu function.
    void fx_init(void);
    //! end of parsed amu function definition handler.
    void fx_pend(void);

    //! store parsed function output variable name.
    void fx_set_tovar(void);
    //! store parsed variable name for argument form name=<value>.
    void fx_set_arg_name(void);

    //! store the string s to the function argument.
    void fx_store_arg(const std::string &s) { fx_argv.store( s ); }
    //! store the current parsed text the function argument.
    void fx_store_arg(void) { fx_argv.store( YYText() ); }
    //! expand parsed variable and store to the function argument.
    void fx_store_arg_expanded(void) { fx_argv.store( varm.expand( YYText() ) ); }
    //! remove escaping in the escaped-parsed variable and store to the function argument.
    void fx_store_arg_escaped(void);

    //! store and clear the current quoted argument string.
    void fx_store_qarg(void) { fx_argv.store( fx_qarg ); fx_qarg.clear(); }
    //! append the current matched text to the quoted argument string.
    void fx_app_qarg(void) { fx_qarg+=YYText(); }
    //! expand parsed variable and append to the quoted argument string.
    void fx_app_qarg_expanded(void) { fx_qarg+=varm.expand( YYText() ); }
    //! remove escaping in the escaped-parsed variable and append to the quoted argument string.
    void fx_app_qarg_escaped(void);

    //! increment or decrement variable with post or pre assignment.
    void fx_incr_arg(bool post=true);


    // amu define
    std::string def_name;               //!< parsed amu definition name.
    std::string def_text;               //!< parsed amu definition text.

    size_t      def_bline;              //!< beginning line of parsed amu definition.
    size_t      def_eline;              //!< ending line of parsed amu definition.

    //! store parsed function name, reset related state, and begin parsing amu function.
    void def_init(void);
    //! end of parsed amu function definition handler.
    void def_pend(void);

    //! store parsed definition variable name.
    void def_set_name(void);
    //! append the string s to the definition text.
    void def_app(const std::string &s) { def_text+=s; }
    //! append the current parsed text to the definition text.
    void def_app(void) { def_text+=YYText(); }


    // built-in amu functions
    //! evaluate and output the function arguments.
    std::string bif_eval(void);
    //! execute a shell command and output its results.
    std::string bif_shell(void);
    //! combine each element from each set to form all possible word combinations.
    std::string bif_combine(void);
    //! recursive helper function for \ref bif_combine.
    void bif_combineR( std::string &r, std::vector<std::string> sv,
                       const std::string &p, const std::string &s,
                       const std::string &j, const std::string &ws,
                       const std::string &t );
    //! generate an image table for a list of images.
    std::string bif_image_table(void);
    //! generate a file viewer for various file formates (png, svg, stl, video, etc.).
    std::string bif_file_viewer(void);
};

} /* end namespace ODIF */

#endif /* END __ODIF_SCANNER_HPP__ */

//! @}


/*******************************************************************************
// eof
*******************************************************************************/
