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

#include "boost/filesystem.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "config.h"

#include "openscad_dif_util.hpp"

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

    std::string ops;                    //!< output prefix string.

    std::string input_name;             //!< scanner input file name.
    std::ifstream input_file;           //!< scanner input file.

    std::string lib_path;               //!< path the library.

    //! write the entire string s to the scanner output.
    void scanner_output(const std::string& s) { scanner_output( s.c_str(), s.length() ); }
    //! write the first size characters of buf to the scanner output.
    void scanner_output(const char* buf, int size);
    //! copy to entire lexed text (yytext) to the scanner output.
    void scanner_echo(void) { scanner_output( yytext, yyleng ); }
  //void scanner_echo(void) { scanner_output( YYText(), YYLeng(); }

    //! output the error message m and abort the scanner.
    void abort(const std::string& m, const int &n = 0, const std::string &t = "");

    //! return word number n from string w.
    std::string get_word(const std::string& w, const int n);

    //! remove all characters in c from from string s.
    std::string remove_chars(const std::string &s, const std::string &c);

    env_var     varm;


    // amu parsed text
    std::string amu_parsed_text;
    size_t      amu_parsed_leng;

    void apt_clear(void) { amu_parsed_text.clear(); amu_parsed_leng=0; }
    void apt(void) { amu_parsed_text+=YYText(); amu_parsed_leng+=YYLeng(); }


    // amu function
    std::string fx_name;
    std::string fx_tovar;
    std::string fx_path;
    func_args   fx_argv;

    std::string fx_qarg;

    size_t      fi_bline;
    size_t      fi_eline;

    void fx_init(void);
    void fx_eval(void);

    void fx_set_tovar(void);
    void fx_set_path(void);
    void fx_set_arg_name(void);

    void fx_store_arg(const std::string &s) { fx_argv.store( s ); }
    void fx_store_arg(void) { fx_argv.store( YYText() ); }
    void fx_store_arg_expanded(void) { fx_argv.store( varm.expand( YYText() ) ); }
    void fx_store_arg_escaped(void);

    void fx_store_qarg(void) { fx_argv.store( fx_qarg ); fx_qarg.clear(); }
    void fx_app_qarg(void) { fx_qarg+=YYText(); }
    void fx_app_qarg_expanded(void) { fx_qarg+=varm.expand( YYText() ); }
    void fx_app_qarg_escaped(void);


    // amu define
    std::string def_name;
    std::string def_text;

    size_t      def_bline;
    size_t      def_eline;

    void def_init(void);
    void def_store(void);
    void def_set_name(void);
    void def_app(const std::string &s) { def_text+=s; }
    void def_app(void) { def_text+=YYText(); }


    // built-in functions
    void bif_eval(void);
    void bif_shell(void);
    void bif_combine(void);
    void bif_combineR(const std::string &s, std::vector<std::string> sv,
                            std::string &r, const std::string &rs=",");

};

} /* end namespace ODIF */

#endif /* END __ODIF_SCANNER_HPP__ */


/*******************************************************************************
// eof
*******************************************************************************/
