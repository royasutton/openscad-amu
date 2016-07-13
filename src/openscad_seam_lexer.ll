/******************************************************************************/
/**
  \file   openscad_seam_lexer.ll

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
    Lexical analyzer for script extraction source.

  \ingroup src_openscad_seam
*******************************************************************************/

%{

#include "openscad_seam_scanner.hpp"

#undef  YY_DECL
#define YY_DECL int SEAM::SEAM_Scanner::scan(void)

bool need_sp=false;       //!< state variable - space needed before next output

#ifndef DOXYGEN_SHOULD_SKIP_THIS

%}

%option c++
%option yyclass="SEAM::SEAM_Scanner"
%option stack
%option yywrap
%option nounput
%option default
%option yylineno
%option debug

%s COMMENT SCOPE MFSCRIPT OPENSCAD

digit                             [[:digit:]]
id                                [_\-[:alnum:]]+
ws                                [ \t]
nr                                [\n\r]
eol                               ";"

bcmt                              "/*"[*!]?
ecmt                              "*"+"/"
cmtlstxt                          "//"[/!]?.*[^\n\r]
cmtls                             "//"[/!]?
cmtlmtxt                          "#".*[^\n\r]
cmtlm                             "#"

bscope                            (?i:begin_scope){ws}+{id}{eol}
bscopea                           (?i:begin_scope_append){ws}+{id}{eol}
bscopep                           (?i:begin_scope_prepend){ws}+{id}{eol}
bscoper                           (?i:begin_scope_root){ws}+{id}{eol}
escope                            (?i:end_scope){eol}

bmfscript                         (?i:begin_mfscript){eol}
emfscript                         (?i:end_mfscript){eol}

bopenscad                         (?i:begin_openscad){eol}
eopenscad                         (?i:end_openscad){eol}

%%

<INITIAL>{bcmt}                   { yy_push_state(COMMENT); }
<INITIAL>{cmtlstxt}               ;
<INITIAL>{cmtls}                  ;
<INITIAL>{ws}                     ;
<INITIAL>{nr}                     ;
<INITIAL>.                        ;

<COMMENT,SCOPE>{bscope}           { begin_scope( 'a', YYText() ); yy_push_state(SCOPE); }
<COMMENT,SCOPE>{bscopea}          { begin_scope( 'a', YYText() ); yy_push_state(SCOPE); }
<COMMENT,SCOPE>{bscopep}          { begin_scope( 'p', YYText() ); yy_push_state(SCOPE); }
<COMMENT,SCOPE>{bscoper}          { begin_scope( 'r', YYText() ); yy_push_state(SCOPE); }
<COMMENT,SCOPE>{bmfscript}        { begin_mfscript(); yy_push_state(MFSCRIPT); }
<COMMENT,SCOPE>{bopenscad}        { begin_openscad(); yy_push_state(OPENSCAD); }
<COMMENT,SCOPE>{cmtlmtxt}         ;
<COMMENT,SCOPE>{cmtlm}            ;
<COMMENT,SCOPE>{cmtlstxt}         ;
<COMMENT,SCOPE>{cmtls}            ;
<COMMENT,SCOPE>{ws}               ;
<COMMENT,SCOPE>{nr}               ;

<COMMENT>{ecmt}                   { yy_pop_state(); }
<COMMENT><<EOF>>                  { abort("Unterminated comment error"); }
<COMMENT>{escope}                 { abort("Syntax error in comment"); }
<COMMENT>{emfscript}              { abort("Syntax error in comment"); }
<COMMENT>{eopenscad}              { abort("Syntax error in comment"); }
<COMMENT>.                        ;

<SCOPE>{escope}                   { end_scope(); yy_pop_state(); }
<SCOPE><<EOF>>                    { abort("Unterminated scope error"); }
<SCOPE>.                          { abort("Syntax error in scope"); }

<MFSCRIPT>{emfscript}             { end_mfscript(); yy_pop_state(); }
<MFSCRIPT><<EOF>>                 { abort("Unterminated mfscript error"); }
<MFSCRIPT>{cmtlmtxt}              { if (comments) { scanner_echo(); scanner_output( "\n", 1 ); } }
<MFSCRIPT>{cmtlm}                 { if (comments) { scanner_echo(); scanner_output( "\n", 1 ); } }
<MFSCRIPT>{ws}+                   { if (need_sp) scanner_output( " ", 1 ); }
<MFSCRIPT>{nr}                    ;
<MFSCRIPT>{eol}                   { scanner_output( "\n", 1 ); need_sp=false; }
<MFSCRIPT>.                       { scanner_echo(); need_sp=true; }

<OPENSCAD>{eopenscad}             { end_openscad(); yy_pop_state(); }
<OPENSCAD><<EOF>>                 { abort("Unterminated openscad error"); }
<OPENSCAD>{cmtlstxt}              { if (comments) { scanner_echo(); } }
<OPENSCAD>{cmtls}                 { if (comments) { scanner_echo(); } }
<OPENSCAD>{ws}                    { scanner_echo(); }
<OPENSCAD>{nr}                    { scanner_echo(); }
<OPENSCAD>.                       { scanner_echo(); }

%%

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
//! \ingroup src_openscad_seam
//! @{

//! Lexer end-of-file handler.
int
yyFlexLexer::yywrap(void)
{
  if (need_sp)
    std::cerr
        << std::endl
        << "# WARNING: Script ended with unterminated line." << std::endl
        << "# WARNING: check for missing ';' at end of script." << std::endl
        << std::endl;

  return 1;
}

//! @}


/*******************************************************************************
// eof
*******************************************************************************/
