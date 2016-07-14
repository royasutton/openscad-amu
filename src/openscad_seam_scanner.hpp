/***************************************************************************//**

  \file   openscad_seam_scanner.hpp

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
    Script extractor scanner class header.

  \ingroup src_openscad_seam
*******************************************************************************/

#ifndef __SEAM_SCANNER_HPP__
#define __SEAM_SCANNER_HPP__ 1

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "openscad_seam_scope.hpp"

#include "boost/filesystem.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <stack>

#include "config.h"

namespace SEAM{

//! Class that implements script extractor scanner.
class SEAM_Scanner : public yyFlexLexer{
  public:
    using FlexLexer::yylex;

    // (con,de)structors
    SEAM_Scanner(const std::string& f, const bool& m=false, const std::string& s="seam: ");
    ~SEAM_Scanner(void);

    // start scanner
    int scan(void);

    void init(void);

    // configuration
    void set_rootscope( const std::string& t );

    void set_debug(bool f) { yy_flex_debug = f; }

    int get_script_count() { return scanner_script_count; }

    // configuration
    void set_ops(const std::string& s) { ops = s; }
    std::string get_ops(void) { return ops; }

    void set_output_prefix(const std::string& s) { output_prefix = s; }
    std::string get_output_prefix(void) { return output_prefix; }

    void set_prefix_scripts(bool f) { prefix_scripts = f; }
    bool get_prefix_scripts(void) { return prefix_scripts; }

    void set_define(const std::vector<std::string>& d) { define = d; }
    std::vector<std::string> get_define(void) { return define; }

    void set_lib_path(const std::string& s) { lib_path = s; }
    std::string get_lib_path(void) { return lib_path; }

    void set_openscad_path(const std::string& s) { openscad_path = s; }
    std::string get_openscad_path(void) { return openscad_path; }

    void set_bash_path(const std::string& s) { bash_path = s; }
    std::string get_bath_path(void) { return bash_path; }

    void set_make_path(const std::string& s) { make_path = s; }
    std::string get_make_path(void) { return make_path; }

    void set_makefile_ext(const std::string& s) { makefile_ext = s; }
    std::string get_makefile_ext(void) { return makefile_ext; }

    void set_mfscript_ext(const std::string& s) { mfscript_ext = s; }
    std::string get_mfscript_ext(void) { return mfscript_ext; }

    void set_openscad_ext(const std::string& s) { openscad_ext = s; }
    std::string get_openscad_ext(void) { return openscad_ext; }

    void set_target(const std::string& s) { target = s; }
    std::string get_target(void) { return target; }

    void set_comments(bool f) { comments = f; }
    bool get_comments(void) { return comments; }

    void set_show(bool f) { show = f; }
    bool get_show(void) { return show; }

    void set_run(bool f) { run = f; }
    bool get_run(void) { return run; }

    void set_make(bool f) { make = f; }
    bool get_make(void) { return make; }

    void set_verbose(bool f) { verbose = f; }
    bool get_verbose(void) { return verbose; }

  private:
    // state
    bool scanner_count_mode;
    bool scanner_output_on;
    int scanner_script_count;

    std::string ops;

    std::string input_name;
    std::string output_name;
    std::ifstream input_file;
    std::ofstream output_file;

    std::stack<SEAM_Scope> scope;

    std::string rootscope;
    std::string output_prefix;
    bool prefix_scripts;

    std::vector<std::string> define;

    std::string lib_path;
    std::string openscad_path;
    std::string bash_path;
    std::string make_path;

    std::string makefile_ext;
    std::string mfscript_ext;
    std::string openscad_ext;

    std::string target;

    bool comments;
    bool show;
    bool run;
    bool make;
    bool verbose;

    // scanner handlers
    void begin_scope( const char c, const std::string& t );
    void end_scope(void);

    void begin_mfscript(void);
    void end_mfscript(void);

    void begin_openscad(void);
    void end_openscad(void);

    // utilities
    void abort(const std::string& m);

    std::string get_word(const std::string& w, const int n);

    std::string get_filename(const std::string& e);

    void switch_output(std::ostream* s);
    void switch_output(const std::string& ext);

    void output_info_header(const std::string& cs);
    void output_info_footer(const std::string& cs);

    void scanner_output( const char* buf, int size );
    void scanner_echo() { scanner_output( yytext, yyleng ); }
};

} /* end namespace SEAM */

#endif /* END __SEAM_SCANNER_HPP__ */


/*******************************************************************************
// eof
*******************************************************************************/
