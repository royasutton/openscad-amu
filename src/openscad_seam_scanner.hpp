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

  \ingroup openscad_seam_src
*******************************************************************************/

#ifndef __SEAM_SCANNER_HPP__
#define __SEAM_SCANNER_HPP__ 1

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "openscad_seam_scope.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>

//! \ingroup openscad_seam_src
//! @{

namespace SEAM{

//! Class that implements the script extractor scanner.
class SEAM_Scanner : public yyFlexLexer{
  public:
    using FlexLexer::yylex;

    //! \brief scanner constructor.
    //! \param f  scanner input file name.
    //! \param m  count scanner mode.
    //! \param s  scanner output prefix text string.
    SEAM_Scanner(const std::string& f, const bool& m=false, const std::string& s="seam: ");
    //! destructor.
    ~SEAM_Scanner(void);

    // scan() implementation generated by flex. see #define YY_DECL in lexer source.
    //! start scanning input file.
    int scan(void);

    //! reset scanner to initial condition.
    void init(void);

    //! set scanner root scope name.
    void set_rootscope( const std::string& t );

    //! set the scope hierarchy joiner string.
    void set_scopejoiner(const std::string& s) { scopejoiner = s; }

    //! turn scanner debugging on or off.
    void set_debug(bool f) { yy_flex_debug = f; }

    //! return the number of scripts found in the input file.
    int get_script_count(void) { return scanner_script_count; }

    //! set the output prefix text string.
    void set_ops(const std::string& s) { ops = s; }
    //! get the output prefix text string.
    std::string get_ops(void) { return ops; }

    //! set the output prefix path.
    void set_output_prefix(const std::string& s) { output_prefix = s; }
    //! get the output prefix path.
    std::string get_output_prefix(void) { return output_prefix; }

    //! set whether to prefix the output prefix path to extracted scripts.
    void set_prefix_scripts(bool f) { prefix_scripts = f; }
    //! get if whether the extracted scripts will be prefix with the output prefix path.
    bool get_prefix_scripts(void) { return prefix_scripts; }

    //! set the vector of macro definitions.
    void set_define(const std::vector<std::string>& d) { define = d; }
    //! get the vector of macro definitions.
    std::vector<std::string> get_define(void) { return define; }

    //! set the base path to the package library.
    void set_lib_path(const std::string& s) { lib_path = s; }
    //! get the base path of the package library.
    std::string get_lib_path(void) { return lib_path; }

    //! set the path to the OpenSCAD binary to be used in the generated makefiles.
    void set_openscad_path(const std::string& s) { openscad_path = s; }
    //! get the path to the OpenSCAD binary being used in the generated makefiles.
    std::string get_openscad_path(void) { return openscad_path; }

    //! set the path to the bash shell to be used to interpret MFScripts.
    void set_bash_path(const std::string& s) { bash_path = s; }
    //! get the path of the bash shell to be used to interpret MFScripts.
    std::string get_bath_path(void) { return bash_path; }

    //! set the path to the GNUMake used to process the generated makefiles.
    void set_make_path(const std::string& s) { make_path = s; }
    //! get the path of the GNUMake used to process the generated makefiles.
    std::string get_make_path(void) { return make_path; }

    //! set the file extension used for generated makefiles.
    void set_makefile_ext(const std::string& s) { makefile_ext = s; }
    //! get the file extension used for generated makefiles.
    std::string get_makefile_ext(void) { return makefile_ext; }

    //! set the file extension used for extracted MFScripts.
    void set_mfscript_ext(const std::string& s) { mfscript_ext = s; }
    //! get the file extension used for extracted MFScripts.
    std::string get_mfscript_ext(void) { return mfscript_ext; }

    //! set the file extension used for extracted OpenSCAD scripts.
    void set_openscad_ext(const std::string& s) { openscad_ext = s; }
    //! get the file extension used for extracted OpenSCAD scripts.
    std::string get_openscad_ext(void) { return openscad_ext; }

    //! set the default make target(s) string.
    void set_target(const std::string& s) { target = s; }
    //! get the default make target(s) string.
    std::string get_target(void) { return target; }

    //! set whether comments are included in extracted scripts.
    void set_comments(bool f) { comments = f; }
    //! get whether comments are included in extracted scripts.
    bool get_comments(void) { return comments; }

    //! set whether to show script once extracted.
    void set_show(bool f) { show = f; }
    //! get whether to show script once extracted.
    bool get_show(void) { return show; }

    //! set whether to run MFScript once extracted.
    void set_run(bool f) { run = f; }
    //! get whether MFScript will be run once extracted.
    bool get_run(void) { return run; }

    //! set whether to run GNUMake on generated makefiles.
    void set_make(bool f) { make = f; }
    //! get whether if GNUMake is run on generated makefiles.
    bool get_make(void) { return make; }

    //! set whether to run in verbose mode.
    void set_verbose(bool f) { verbose = f; }
    //! get if running in verbose mode.
    bool get_verbose(void) { return verbose; }

  private:
    // state
    bool scanner_count_mode;            //!< scanner in count mode.
    bool scanner_output_on;             //!< scanner output on.
    int scanner_script_count;           //!< script count.

    std::string ops;                    //!< output prefix string.

    std::string input_name;             //!< scanner input file name.
    std::string output_name;            //!< scanner output file name.
    std::ifstream input_file;           //!< scanner input file.
    std::ofstream output_file;          //!< scanner output file.

    std::stack<SEAM_Scope> scope;       //!< scope stack.

    std::string rootscope;              //!< scanner root scope name.
    std::string scopejoiner;            //!< scanner scope hierarchy conjoiner string.
    std::string output_prefix;          //!< scanner output path prefix.
    bool prefix_scripts;                //!< prefixing extracted scripts?

    std::vector<std::string> define;    //!< vector of macro definitions.

    std::string lib_path;               //!< path the library.
    std::string openscad_path;          //!< path the openscad.
    std::string bash_path;              //!< path to bash.
    std::string make_path;              //!< path to gnumake.

    std::string makefile_ext;           //!< makefile extension.
    std::string mfscript_ext;           //!< mfscript extension.
    std::string openscad_ext;           //!< openscad extension.

    std::string target;                 //!< make target(s).

    bool comments;                      //!< copy comments to scripts.
    bool show;                          //!< show extracted scripts.
    bool run;                           //!< run extracted mfscripts.
    bool make;                          //!< run gnumake on generated makefiles.
    bool verbose;                       //!< verbose output.

    //! \brief scanner handler: begin scope
    //! \param mc scope naming mode character. \see SEAM_Scope.
    //! \param lt lexed text (with scope name as second word).
    void begin_scope(const char mc, const std::string& lt);
    //! scanner handler: end scope
    void end_scope(void);

    //! scanner handler: begin mfscript
    void begin_mfscript(void);
    //! scanner handler: end mfscript
    void end_mfscript(void);

    //! scanner handler: begin openscad
    void begin_openscad(void);
    //! scanner handler: end openscad
    void end_openscad(void);

    //! output the error message m and abort the scanner.
    void abort(const std::string& m);

    //! return word number n from string w.
    std::string get_word(const std::string& w, const int n);

    //! return a script file name for the current scanned scope with extension e.
    std::string get_filename(const std::string& e);

    //! switch the output to the output stream s.
    void switch_output(std::ostream* s);
    //! switch output to a scope script file with extension e.
    void switch_output(const std::string& ext);

    //! append a text header to the extracted script.
    void output_info_header(const std::string& cs);
    //! append a text footer to the extracted script.
    void output_info_footer(const std::string& cs);

    //! write the first size characters of buf to the scanner output.
    void scanner_output(const char* buf, int size);
    //! copy to entire lexed text (yytext) to the scanner output.
    void scanner_echo(void) { scanner_output( yytext, yyleng ); }
};

} /* end namespace SEAM */

#endif /* END __SEAM_SCANNER_HPP__ */

//! @}


/*******************************************************************************
// eof
*******************************************************************************/
