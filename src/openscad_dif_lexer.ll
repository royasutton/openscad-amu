/******************************************************************************/
/**
  \file   openscad_dif_lexer.ll

  \author Roy Allen Sutton
  \date   2016-2023

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

  /* scanner options */

%option c++
%option yyclass="ODIF::ODIF_Scanner"
%option stack
%option noyywrap
%option nounput
%option default
%option yylineno
%option debug

  /* scanner states */

%s COMBLCK COMLINE COMNEST
%s AMUFN AMUFNARG AMUFNAQS AMUFNAQD AMUFNTEXTBLCK
%s AMUINC AMUINCFILE
%s AMUDEF AMUDEFARG
%s AMUIF AMUIFEXPR AMUIFTEXT AMUIFTEXTBLCK AMUIFELSE

  /* comment lines, blocks, and nested blocks */

com_line                          "//"[/!]?

com_blck_open                     "/*"[*!]?
com_blck_clse                     "*"+"/"

com_nest_open                     "/""+"+
com_nest_clse                     "+"+"/"

  /* whitespace */

ws                                [ \t]
nr                                [\n\r]
wsnr                              ({ws}|{nr})

  /* identifiers, variables, and paths */

id                                [_[:alnum:]]+
id_var                            "${"{id}"}"
path                              [_./\-\\[:alnum:]]+

  /* functions */

amu_esc                           "\\"[\\@]
amu_include                       [\\@](?i:amu_include)
amu_define                        [\\@](?i:amu_define)
amu_if                            [\\@](?i:amu_if)
amu_bif                           [\\@](?i:amu)_{id}

  /* increment operations */

incr_var_pre                      ("++"|"--"){id}
incr_var_post                     {id}("++"|"--")

  /* amu_include arguments */

inc_copy                          ((?i:copy)|(?i:c))
inc_no_switch                     ((?i:no_switch)|(?i:n))
inc_search                        ((?i:search)|(?i:s))

  /* amu_if values, operations, arguments, and expressions */

if_val_true                       ("1"|(?i:t)|(?i:true))
if_val_false                      ("0"|(?i:f)|(?i:false))

if_opr_not                        "!"
if_opr_and                        "&&"
if_opr_or                         "\|\|"

if_func_1a                        "-"[nzwedl]
if_func_2a                        ("=="|"!="|"<"|">"|"<="|">=")

if_arg_uq                         ({id}|{id_var}|{path})
if_arg_sq                         \'{if_arg_uq}?\'
if_arg_dq                         \"{if_arg_uq}?\"
if_arg                            ({if_arg_uq}|{if_arg_sq}|{if_arg_dq})

if_expr_1a                        {if_func_1a}{wsnr}+{if_arg}
if_expr_2a                        {if_arg}{wsnr}+{if_func_2a}{wsnr}+{if_arg}

%%

  /*
    outside of comments:
      + replace OpenSCAD commands with C equivalents,
      + output everything else unchanged
  */

<INITIAL>{com_blck_open}          { scanner_echo(); yy_push_state(COMBLCK); }
<INITIAL>{com_line}               { scanner_echo(); yy_push_state(COMLINE); }
<INITIAL>include                  { scanner_output( "#include", 8 ); }
<INITIAL>use                      { scanner_output( "#include", 8 ); }
<INITIAL>{nr}                     { scanner_echo(); }
<INITIAL>.                        { scanner_echo(); }

  /*
    comments:
      + escaped amu function reduction
      + match and handle amu functions
      + output everything else unchanged

    inside block:
      + output new lines
      + exit comment when block is closed

    on line:
      + exit comment at end of line
  */

<COMBLCK,COMLINE>{amu_esc}        { fx_remove_esc(); }
<COMBLCK,COMLINE>{amu_include}    { inc_init(); yy_push_state(AMUINC); }
<COMBLCK,COMLINE>{amu_define}     { def_init(); yy_push_state(AMUDEF); }
<COMBLCK,COMLINE>{amu_if}         { if_init(); yy_push_state(AMUIF); }
<COMBLCK,COMLINE>{amu_bif}        { fx_init(); yy_push_state(AMUFN); }
<COMBLCK,COMLINE>.                { scanner_echo(); }

<COMBLCK>{nr}                     { scanner_echo(); }
<COMBLCK>{com_blck_clse}          { scanner_echo(); yy_pop_state(); }

<COMLINE>{nr}                     { scanner_echo(); yy_pop_state(); }

  /*
    nested comments:
      + output nothing
      + exit comment when block is closed
  */

<COMNEST>{com_nest_clse}          { nc_end(); yy_pop_state(); }
<COMNEST>{nr}                     { }
<COMNEST>.                        { }
<COMNEST><<EOF>>                  { abort("unterminated nested comment block", nc_bline); }

  /* allowable entry into nested comments: */

<COMBLCK,AMUFNARG,AMUFNTEXTBLCK,AMUIFEXPR,AMUIFTEXTBLCK>{com_nest_open} { nc_init(); yy_push_state(COMNEST); }

  /*
    amu_bif:
    amu_bif var ( a1 a2 'a3' "a 4" file="all" ) { text }
  */

<AMUFN>{id}                       { apt(); fx_set_var(); }
<AMUFN>\(                         { apt(); BEGIN(AMUFNARG); }
<AMUFN>\{                         { apt(); fx_body_level++; BEGIN(AMUFNTEXTBLCK); }
<AMUFN>{ws}+                      { apt(); }
<AMUFN>{nr}                       { apt(); }
<AMUFN>.                          { error("in function variable name", lineno(), YYText()); }

  /* amu_bif: ( arguments ) */

<AMUFNARG>{id}                    { apt(); fx_store_arg(); }
<AMUFNARG>{id_var}                { apt(); fx_store_arg_expanded(); }
<AMUFNARG>\\{id_var}              { apt(); fx_store_arg_escaped(); }
<AMUFNARG>\\.                     { apt(); fx_store_arg_escaped(); }
<AMUFNARG>{id}=                   { apt(); fx_set_arg_name(); }
<AMUFNARG>{incr_var_pre}          { apt(); fx_incr_arg(false); }
<AMUFNARG>{incr_var_post}         { apt(); fx_incr_arg(true); }
<AMUFNARG>\'                      { apt(); fx_app_qarg(); yy_push_state(AMUFNAQS); }
<AMUFNARG>\"                      { apt(); fx_app_qarg(); yy_push_state(AMUFNAQD); }
<AMUFNARG>\){wsnr}*\{             { apt(); fx_body_level++; BEGIN(AMUFNTEXTBLCK); }
<AMUFNARG>\)                      { apt(); fx_end(); yy_pop_state(); }
<AMUFNARG>{ws}+                   { apt(); }
<AMUFNARG>{nr}                    { apt(); }
<AMUFNARG>.                       { error("in function arguments", lineno(), YYText()); }
<AMUFNARG><<EOF>>                 { abort("unterminated function arguments", fx_bline); }

  /* amu_bif: ( 'arguments' and/or "arguments" )  */

<AMUFNAQS>\'                      { apt(); fx_app_qarg(); fx_store_qarg(); yy_pop_state(); }
<AMUFNAQD>\"                      { apt(); fx_app_qarg(); fx_store_qarg(); yy_pop_state(); }

<AMUFNAQS,AMUFNAQD>\\\'           { apt(); fx_app_qarg_escaped(); }
<AMUFNAQS,AMUFNAQD>\\\"           { apt(); fx_app_qarg_escaped(); }
<AMUFNAQS,AMUFNAQD>\\{id_var}     { apt(); fx_app_qarg_escaped(); }
<AMUFNAQS,AMUFNAQD>{id_var}       { apt(); fx_app_qarg_expanded(); }
<AMUFNAQS,AMUFNAQD>\\{nr}         { apt(); fx_app_qarg(""); }
<AMUFNAQS,AMUFNAQD>{nr}           { apt(); fx_app_qarg(); }
<AMUFNAQS,AMUFNAQD>.              { apt(); fx_app_qarg(); }

<AMUFNAQS><<EOF>>                 { abort("unterminated single quote", fx_bline); }
<AMUFNAQD><<EOF>>                 { abort("unterminated double quote", fx_bline); }

  /* amu_bif: { text block } */

<AMUFNTEXTBLCK>\}                 { apt(); if (--fx_body_level) { fx_app_body(); }
                                           else { fx_end(); yy_pop_state(); } }
<AMUFNTEXTBLCK>\{                 { apt(); fx_app_body(); fx_body_level++; }
<AMUFNTEXTBLCK>\\{nr}             { apt(); fx_app_body(""); }
<AMUFNTEXTBLCK>{nr}               { apt(); fx_app_body(); }
<AMUFNTEXTBLCK>.                  { apt(); fx_app_body(); }
<AMUFNTEXTBLCK><<EOF>>            { abort("unterminated function body block", fx_bline); }

  /*
    amu_include:
    amu_include copy no_switch ( ${var1}/path/${var2}/file )
  */

<AMUINC>\(                        { apt(); BEGIN(AMUINCFILE); }
<AMUINC>{inc_copy}                { apt(); inc_set_copy(true); }
<AMUINC>{inc_no_switch}           { apt(); inc_set_switch(false); }
<AMUINC>{inc_search}              { apt(); inc_set_search(true); }
<AMUINC>{ws}+                     { apt(); }
<AMUINC>{nr}                      { apt(); }
<AMUINC>.                         { error("in include", lineno(), YYText()); }

<AMUINCFILE>\)                    { apt(); inc_end(); yy_pop_state(); }
<AMUINCFILE>\\{nr}                { apt(); inc_app(""); }
<AMUINCFILE>{nr}                  { apt(); inc_app(); }
<AMUINCFILE>.                     { apt(); inc_app(); }
<AMUINCFILE><<EOF>>               { abort("unterminated include filename", inc_bline); }

  /*
    amu_define:
    amu_define var ( text of definition with ${variables} )
  */

<AMUDEF>{id}                      { apt(); def_set_var(); }
<AMUDEF>\(                        { apt(); def_nest_level++; BEGIN(AMUDEFARG); }
<AMUDEF>{ws}+                     { apt(); }
<AMUDEF>{nr}                      { apt(); }
<AMUDEF>.                         { error("in define variable name", lineno(), YYText()); }

<AMUDEFARG>\)                     { apt(); if (--def_nest_level) { def_app(); }
                                           else { def_end(); yy_pop_state(); } }
<AMUDEFARG>\(                     { apt(); def_app();  }
<AMUDEFARG>\\{nr}                 { apt(); def_app(""); }
<AMUDEFARG>{nr}                   { apt(); def_app(); }
<AMUDEFARG>.                      { apt(); def_app(); }
<AMUDEFARG><<EOF>>                { abort("unterminated define arguments", def_bline); }

  /*
    amu_if:
    amu_if var ( expr ) { text } else*if ( expr ) ${var} else { text } endif
  */

<AMUIF>{id}                       { apt(); if_set_var(); }
<AMUIF>\(                         { apt(); if_init_case(true); BEGIN(AMUIFEXPR); }
<AMUIF>{ws}+                      { apt(); }
<AMUIF>{nr}                       { apt(); }
<AMUIF>.                          { error("in if variable name", lineno(), YYText()); }

<AMUIFEXPR>\)                     { apt(); if_eval_expr(); if (if_opr.empty()) BEGIN(AMUIFTEXT); }
<AMUIFEXPR>\(                     { apt(); if_opr.push('('); }
<AMUIFEXPR>{if_opr_not}           { apt(); if_opr.push('!'); }
<AMUIFEXPR>{if_opr_and}           { apt(); if_opr.push('&'); }
<AMUIFEXPR>{if_opr_or}            { apt(); if_opr.push('|'); }
<AMUIFEXPR>{if_val_true}          { apt(); if_val.push(true); }
<AMUIFEXPR>{if_val_false}         { apt(); if_val.push(false); }
<AMUIFEXPR>{if_expr_1a}           { apt(); if_val.push(bif_if_exp_1a( YYText() )); }
<AMUIFEXPR>{if_expr_2a}           { apt(); if_val.push(bif_if_exp_2a( YYText() )); }
<AMUIFEXPR>{ws}+                  { apt(); }
<AMUIFEXPR>{nr}                   { apt(); }
<AMUIFEXPR>.                      { error("in if expression", lineno(), YYText()); }
<AMUIFEXPR><<EOF>>                { abort("unterminated if expression", if_bline); }

<AMUIFTEXT>{id_var}               { apt(); if_get_var_text(); if_end_case(); BEGIN(AMUIFELSE); }
<AMUIFTEXT>\{                     { apt(); if_case_level++; BEGIN(AMUIFTEXTBLCK); }
<AMUIFTEXT>{ws}+                  { apt(); }
<AMUIFTEXT>{nr}                   { apt(); }
<AMUIFTEXT>.                      { error("in if case body", lineno(), YYText()); }
<AMUIFTEXT><<EOF>>                { abort("unterminated if case body", if_bline); }

<AMUIFTEXTBLCK>\}                 { apt(); if (--if_case_level) { if_app(); }
                                           else { if_end_case(); BEGIN(AMUIFELSE); } }
<AMUIFTEXTBLCK>\{                 { apt(); if_app(); if_case_level++; }
<AMUIFTEXTBLCK>\\{nr}             { apt(); if_app(""); }
<AMUIFTEXTBLCK>{nr}               { apt(); if_app(); }
<AMUIFTEXTBLCK>.                  { apt(); if_app(); }
<AMUIFTEXTBLCK><<EOF>>            { abort("unterminated if case body block", if_bline); }

<AMUIFELSE>"else"{wsnr}*"if"      { apt(); BEGIN(AMUIF);}
<AMUIFELSE>"else"                 { apt(); if_init_case(false); BEGIN(AMUIFTEXT);}
<AMUIFELSE>"endif"                { apt(); if_end(); yy_pop_state(); }
<AMUIFELSE>{ws}+                  { apt(); }
<AMUIFELSE>{nr}                   { apt(); }
<AMUIFELSE>.                      { error("in if else", lineno(), YYText()); }
<AMUIFELSE><<EOF>>                { abort("unterminated if", if_bline); }

%%

#endif /* DOXYGEN_SHOULD_SKIP_THIS */


/*******************************************************************************
// eof
*******************************************************************************/
