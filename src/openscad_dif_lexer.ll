/******************************************************************************/
/**
  \file   openscad_dif_lexer.ll

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

%s COMMENT READCL AMUFUNC FUNCARG FUNCARGDQ FUNCARGSQ AMUMDEFINE DEFINEARG

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

  /* outside comments:
      (1) replace OpenSCAD commands with C equivalents,
      (2) output everything else unchanged */

<INITIAL>{comment_open}           { scanner_echo(); yy_push_state(COMMENT); }
<INITIAL>{comment_line}           { scanner_echo(); yy_push_state(READCL); }
<INITIAL>include                  { scanner_output( "#include", 8 ); }
<INITIAL>use                      { scanner_output( "#include", 8 ); }
<INITIAL>{nr}                     { scanner_echo(); }
<INITIAL>.                        { scanner_echo(); }

  /* inside comments:
      (1) match and handle amu functions
      (2) output everything else unchanged */

<COMMENT>{comment_close}          { scanner_echo(); yy_pop_state(); }
<COMMENT><<EOF>>                  { abort("unterminated comment"); }
<COMMENT>{amu_define}             { def_init(); yy_push_state(AMUMDEFINE); }
<COMMENT>{amu_func}               { fx_init(); yy_push_state(AMUFUNC); }
<COMMENT>{amu_escaped}            { /* remove prefixed escape char, output the rest */
                                    string mt = YYText();
                                    scanner_output( mt.substr(1,mt.length()-1) ); }
<COMMENT>{nr}                     { scanner_echo(); }
<COMMENT>.                        { scanner_echo(); }

  /* comment line: (outside of comment block)
      (1) output everything unchanged until end of line */

<READCL>{nr}                      { scanner_echo(); yy_pop_state(); }
<READCL>.                         { scanner_echo(); }

  /* parse amu function:
      \amu_func1 var ( a1 a2 'a3' "a 4" file="all" ) */

<AMUFUNC>{id}                     { apt(); fx_set_tovar(); }
<AMUFUNC>\(                       { apt(); BEGIN(FUNCARG); }
<AMUFUNC>{ws}+                    { apt(); }
<AMUFUNC>{nr}                     { apt(); }
<AMUFUNC>.                        { abort("in function name", lineno(), YYText()); }

  /* parse arguments */

<FUNCARG>{id}                     { apt(); fx_store_arg(); }
<FUNCARG>{id_var}                 { apt(); fx_store_arg_expanded(); }
<FUNCARG>\\{id_var}               { apt(); fx_store_arg_escaped(); }
<FUNCARG>\\.                      { apt(); fx_store_arg_escaped(); }
<FUNCARG>{id}=                    { apt(); fx_set_arg_name(); }
<FUNCARG>{incr_var_pre}           { apt(); fx_incr_arg(false); }
<FUNCARG>{incr_var_post}          { apt(); fx_incr_arg(true); }
<FUNCARG>\'                       { apt(); fx_app_qarg(); yy_push_state(FUNCARGSQ); }
<FUNCARG>\"                       { apt(); fx_app_qarg(); yy_push_state(FUNCARGDQ); }
<FUNCARG>\)                       { apt(); fx_pend(); yy_pop_state(); }
<FUNCARG>{ws}+                    { apt(); }
<FUNCARG>{nr}                     { apt(); }
<FUNCARG>.                        { abort("in function arguments", lineno(), YYText()); }
<FUNCARG><<EOF>>                  { abort("unterminated function arguments", fi_bline); }

  /* parse single and double quoted argument */

<FUNCARGSQ>\'                     { apt(); fx_app_qarg(); fx_store_qarg(); yy_pop_state(); }
<FUNCARGDQ>\"                     { apt(); fx_app_qarg(); fx_store_qarg(); yy_pop_state(); }
<FUNCARGSQ,FUNCARGDQ>\\\'         { apt(); fx_app_qarg_escaped(); }
<FUNCARGSQ,FUNCARGDQ>\\\"         { apt(); fx_app_qarg_escaped(); }
<FUNCARGSQ,FUNCARGDQ>\\{id_var}   { apt(); fx_app_qarg_escaped(); }
<FUNCARGSQ,FUNCARGDQ>{id_var}     { apt(); fx_app_qarg_expanded(); }
<FUNCARGSQ,FUNCARGDQ>{nr}         { apt(); fx_app_qarg(); }
<FUNCARGSQ,FUNCARGDQ>.            { apt(); fx_app_qarg(); }
<FUNCARGSQ><<EOF>>                { abort("unterminated single quote", fi_bline); }
<FUNCARGDQ><<EOF>>                { abort("unterminated double quote", fi_bline); }

  /* parse amu define:
      \amu_define var ( text of definition with ${variables} ) */

<AMUMDEFINE>{id}                  { apt(); def_set_name(); }
<AMUMDEFINE>\(                    { apt(); BEGIN(DEFINEARG); }
<AMUMDEFINE>{ws}+                 { apt(); }
<AMUMDEFINE>{nr}                  { apt(); }
<AMUMDEFINE>.                     { abort("in define", lineno(), YYText()); }

<DEFINEARG>\)                     { apt(); def_pend(); yy_pop_state(); }
<DEFINEARG>{ws}+                  { apt(); def_app(" "); }
<DEFINEARG>{nr}+                  { apt(); def_app(" "); }
<DEFINEARG>.                      { apt(); def_app(); }
<DEFINEARG><<EOF>>                { abort("unterminated define arguments", fi_bline); }

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
