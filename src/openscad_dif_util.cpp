/***************************************************************************//**

  \file   openscad_dif_util.cpp

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
    OpenSCAD Doxygen input filter utility classes source.

  \ingroup openscad_dif_src
*******************************************************************************/

using namespace std;

#include "openscad_dif_util.hpp"


////////////////////////////////////////////////////////////////////////////////
// env_var
////////////////////////////////////////////////////////////////////////////////

ODIF::env_var::env_var( const string& p, const string& s,
                        bool r, const string& rm)
{
  set_prefix( p );
  set_suffix( s );
  set_report( r );
  set_report_message( rm );
}

ODIF::env_var::~env_var( void )
{
  clear();
}

std::string
ODIF::env_var::expand(const std::string& v, bool r, const std::string& rm)
{
  std::string mn;

  // remove prefix and suffix strings from named variable:
  // ${VAR} --> VAR
  mn = v.substr( prefix.length()
               , v.length()-(prefix.length()+suffix.length()) );

  std::string rs;

  if ( map.find( mn ) != map.end() ) {
    rs = map[ mn ];
  } else {
    if ( r ) rs = v + "=" + rm;
  }

  return( rs );
}

void
ODIF::env_var::dump(void) {
  std::cout << "(var map begin)" << std::endl;

  for ( std::map<std::string, std::string>::iterator
        it=map.begin();
        it!=map.end();
        ++it )
  {
    std::cout << it->first << "=" << it->second << std::endl;
  }

  std::cout << "(var map begin)" << std::endl;
}


////////////////////////////////////////////////////////////////////////////////
// func_args
////////////////////////////////////////////////////////////////////////////////

ODIF::func_args::func_args(const std::string& p)
{
  clear();
  set_prefix( p );
}

ODIF::func_args::~func_args(void)
{
  clear();
}

void
ODIF::func_args::store(const std::string& v)
{
  bool p;

  // if no argument name has been specified, use (positional naming)
  // incrementing arguments prefixed with 'pos_prefix' + argument number
  if ( next_name.length() == 0 ) {
    next_name = pos_prefix + to_string( pos_count++ );
    p = true;
  } else {
    p = false;
  }

  arg_term ta;

  ta.name       = next_name;
  ta.value      = v;
  ta.positional = p;

  argV.push_back( ta );

  // next_name consumed, clear it.
  next_name.clear();
}

size_t
ODIF::func_args::size(bool n, bool p)
{
  size_t tc=0, nc=0, pc=0;

  for ( std::vector<arg_term>::iterator it=argV.begin();
                                      it!=argV.end();
                                      ++it )
  {
    if ( it->positional ) pc++;
    else                  nc++;
  }

  if ( n ) tc+= nc;
  if ( p ) tc+= pc;

  return ( tc );
}

bool
ODIF::func_args::exists(const std::string& n)
{
  bool found = false;

  for ( std::vector<arg_term>::iterator it=argV.begin();
                                      it!=argV.end() && !found;
                                      ++it )
  {
    if ( it->name.compare( n )==0 )
      found = true;
  }

  return ( found );
}

std::string
ODIF::func_args::value(const std::string& n)
{
  bool found = false;
  std::string rs;

  for ( std::vector<arg_term>::iterator it=argV.begin();
                                      it!=argV.end() && !found;
                                      ++it )
  {
    if ( it->name.compare( n )==0 )
      rs = it->value;
  }

  return ( rs );
}

std::vector<std::string>
ODIF::func_args::values_v(bool n, bool p)
{
  std::vector<std::string> rv;

  for ( std::vector<arg_term>::iterator it=argV.begin();
                                      it!=argV.end();
                                      ++it )
  {
    if (   it->positional && p )  rv.push_back( it->value );
    if ( ! it->positional && n )  rv.push_back( it->value );
  }

  return ( rv );
}

std::string
ODIF::func_args::values_str(bool n, bool p)
{
  std::string rv;

  for ( std::vector<arg_term>::iterator it=argV.begin();
                                      it!=argV.end();
                                      ++it )
  {
    if (   it->positional && p ) {
      if ( rv.length() ) rv += " ";
      rv += it->value;
    }
    if ( ! it->positional && n ) {
      if ( rv.length() ) rv += " ";
      rv += it->value;
    }
  }

  return ( rv );
}

std::vector<std::string>
ODIF::func_args::names_v(bool n, bool p)
{
  std::vector<std::string> rv;

  for ( std::vector<arg_term>::iterator it=argV.begin();
                                      it!=argV.end();
                                      ++it )
  {
    if (   it->positional && p )  rv.push_back( it->name );
    if ( ! it->positional && n )  rv.push_back( it->name );
  }

  return ( rv );
}

std::string
ODIF::func_args::names_str(bool n, bool p)
{
  std::string rv;

  for ( std::vector<arg_term>::iterator it=argV.begin();
                                      it!=argV.end();
                                      ++it )
  {
    if (   it->positional && p ) {
      if ( rv.length() ) rv += " ";
      rv += it->name;
    }
    if ( ! it->positional && n ) {
      if ( rv.length() ) rv += " ";
      rv += it->name;
    }
  }

  return ( rv );
}

std::string
ODIF::func_args::pairs_str(bool n, bool p, std::string a)
{
  std::string rv;

  for ( std::vector<arg_term>::iterator it=argV.begin();
                                      it!=argV.end();
                                      ++it )
  {
    if (   it->positional && p ) {
      if ( rv.length() ) rv += " ";
      rv += it->name + a + it->value;
    }
    if ( ! it->positional && n ) {
      if ( rv.length() ) rv += " ";
      rv += it->name + a + it->value;
    }
  }

  return ( rv );
}


void
ODIF::func_args::dump(void)
{
  std::cout << "(";

  for ( std::vector<arg_term>::iterator it=argV.begin(); it!=argV.end(); ) {
    std::cout << it->name << "=" << it->value;

    ++it;
    if ( it!=argV.end() ) std::cout << " ";
  }

  std::cout << ")";
}


/*******************************************************************************
// eof
*******************************************************************************/
