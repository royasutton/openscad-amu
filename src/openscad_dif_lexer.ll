/******************************************************************************/
/**
  \file   openscad_dif_lexer.ll

  \author Roy Allen Sutton
  \date   2016-2017

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
    Doxygen input filter lexical analyzer source for OpenSCAD script.

  \ingroup openscad_dif_src
*******************************************************************************/

%{

#include "openscad_dif_scanner.hpp"

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#undef  YY_DECL
#define YY_DECL int ODIF::ODIF_Scanner::scan(void)

using namespace std;

#ifndef DOXYGEN_SHOULD_SKIP_THIS

%}

%option c++
%option yyclass="ODIF::ODIF_Scanner"
%option stack
%option yywrap
%option nounput
%option default
%option yylineno
%option debug

%s COMMENT COMMENTLN
%s AMUFN AMUFNARG AMUFNAQS AMUFNAQD
%s AMUDEF AMUDEFARG

comment_line                      "//"[/!]?
comment_open                      "/*"[*!]?
comment_close                     "*"+"/"

ws                                [ \t]
nr                                [\n\r]

id                                [_[:alnum:]]+
id_var                            "${"{id}"}"

incr_var_pre                      ("++"|"--"){id}
incr_var_post                     {id}("++"|"--")

amu_func                          [\\@](?i:amu)_{id}
amu_define                        [\\@](?i:amu_define)
amu_escaped                       "\\"[\\@]

%%

  /*
    outside of comments:
      (1) replace OpenSCAD commands with C equivalents,
      (2) output everything else unchanged
  */

<INITIAL>{comment_open}           { scanner_echo(); yy_push_state(COMMENT); }
<INITIAL>{comment_line}           { scanner_echo(); yy_push_state(COMMENTLN); }
<INITIAL>include                  { scanner_output( "#include", 8 ); }
<INITIAL>use                      { scanner_output( "#include", 8 ); }
<INITIAL>{nr}                     { scanner_echo(); }
<INITIAL>.                        { scanner_echo(); }

  /*
    inside comment block:
      (1) match and handle amu functions
      (2) output everything else unchanged
  */

<COMMENT>{comment_close}          { scanner_echo(); yy_pop_state(); }
<COMMENT><<EOF>>                  { abort("unterminated comment"); }
<COMMENT>{amu_define}             { def_init(); yy_push_state(AMUDEF); }
<COMMENT>{amu_func}               { fx_init(); yy_push_state(AMUFN); }
<COMMENT>{amu_escaped}            { /* remove prefixed escape char, output the rest */
                                    string mt = YYText();
                                    scanner_output( mt.substr(1,mt.length()-1) ); }
<COMMENT>{nr}                     { scanner_echo(); }
<COMMENT>.                        { scanner_echo(); }

  /*
    on comment line, outside of comment block:
      (1) output everything unchanged until end of line
  */

<COMMENTLN>{nr}                   { scanner_echo(); yy_pop_state(); }
<COMMENTLN>.                      { scanner_echo(); }

  /*
    amu_func:
    amu_func var ( a1 a2 'a3' "a 4" file="all" )
  */

<AMUFN>{id}                       { apt(); fx_set_tovar(); }
<AMUFN>\(                         { apt(); BEGIN(AMUFNARG); }
<AMUFN>{ws}+                      { apt(); }
<AMUFN>{nr}                       { apt(); }
<AMUFN>.                          { abort("in function name", lineno(), YYText()); }

  /*
    amu function arguments:
  */

<AMUFNARG>{id}                    { apt(); fx_store_arg(); }
<AMUFNARG>{id_var}                { apt(); fx_store_arg_expanded(); }
<AMUFNARG>\\{id_var}              { apt(); fx_store_arg_escaped(); }
<AMUFNARG>\\.                     { apt(); fx_store_arg_escaped(); }
<AMUFNARG>{id}=                   { apt(); fx_set_arg_name(); }
<AMUFNARG>{incr_var_pre}          { apt(); fx_incr_arg(false); }
<AMUFNARG>{incr_var_post}         { apt(); fx_incr_arg(true); }
<AMUFNARG>\'                      { apt(); fx_app_qarg(); yy_push_state(AMUFNAQS); }
<AMUFNARG>\"                      { apt(); fx_app_qarg(); yy_push_state(AMUFNAQD); }
<AMUFNARG>\)                      { apt(); fx_pend(); yy_pop_state(); }
<AMUFNARG>{ws}+                   { apt(); }
<AMUFNARG>{nr}                    { apt(); }
<AMUFNARG>.                       { abort("in function arguments", lineno(), YYText()); }
<AMUFNARG><<EOF>>                 { abort("unterminated function arguments", fi_bline); }

  /*
    amu function single and double quoted arguments:
  */

<AMUFNAQS>\'                      { apt(); fx_app_qarg(); fx_store_qarg(); yy_pop_state(); }
<AMUFNAQD>\"                      { apt(); fx_app_qarg(); fx_store_qarg(); yy_pop_state(); }
<AMUFNAQS,AMUFNAQD>\\\'           { apt(); fx_app_qarg_escaped(); }
<AMUFNAQS,AMUFNAQD>\\\"           { apt(); fx_app_qarg_escaped(); }
<AMUFNAQS,AMUFNAQD>\\{id_var}     { apt(); fx_app_qarg_escaped(); }
<AMUFNAQS,AMUFNAQD>{id_var}       { apt(); fx_app_qarg_expanded(); }
<AMUFNAQS,AMUFNAQD>{nr}           { apt(); fx_app_qarg(); }
<AMUFNAQS,AMUFNAQD>.              { apt(); fx_app_qarg(); }
<AMUFNAQS><<EOF>>                 { abort("unterminated single quote", fi_bline); }
<AMUFNAQD><<EOF>>                 { abort("unterminated double quote", fi_bline); }

  /*
    amu_define:
    amu_define var ( text of definition with ${variables} )
  */

<AMUDEF>{id}                      { apt(); def_set_tovar(); }
<AMUDEF>\(                        { apt(); BEGIN(AMUDEFARG); }
<AMUDEF>{ws}+                     { apt(); }
<AMUDEF>{nr}                      { apt(); }
<AMUDEF>.                         { abort("in define", lineno(), YYText()); }

<AMUDEFARG>\)                     { apt(); def_pend(); yy_pop_state(); }
<AMUDEFARG>\\{nr}                 { apt(); def_app(""); }
<AMUDEFARG>{nr}                   { apt(); def_app(); }
<AMUDEFARG>.                      { apt(); def_app(); }
<AMUDEFARG><<EOF>>                { abort("unterminated define arguments", fi_bline); }

%%

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
//! \ingroup openscad_dif_src
//! @{

//! Lexer end-of-file handler.
int yyFlexLexer::yywrap(void) { return 1; }

//! @}


/*******************************************************************************
// eof
*******************************************************************************/
