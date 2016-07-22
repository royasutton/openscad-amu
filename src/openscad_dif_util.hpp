/***************************************************************************//**

  \file   openscad_dif_util.hpp

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
    OpenSCAD Doxygen input filter utility classes header.

  \ingroup openscad_dif_src
*******************************************************************************/

#ifndef __ODIF_UTIL_HPP__
#define __ODIF_UTIL_HPP__ 1

#include <string>
#include <sstream>
#include <map>


namespace ODIF{

//! Class that .
class env_var {
  public:
    //! constructor.
    env_var(const std::string& p="${", const std::string& s="}",
            bool r=true, const std::string& rm="<UNDEFINED>") {
      set_prefix( p );
      set_suffix( s );
      set_report( r );
      set_report_message( rm );
    }
    //! destructor.
    ~env_var(void) { }

    void clear(void) { map.clear(); }

    void set_prefix(const std::string& s) { prefix=s; }
    void set_suffix(const std::string& s) { suffix=s; }
    void set_report(bool s) { report=s; }
    void set_report_message(const std::string& s) { report_message=s; }

    void store(const std::string& n, const std::string& v) { map[ n ] = v; }

    std::string expand(const std::string& v) {
      return (expand(v, report, report_message) );
    }
    std::string expand(const std::string& v, bool r) {
      return (expand(v, r, report_message) );
    }
    std::string expand(const std::string& v,
                       bool r, const std::string& rm) {
      std::string mn;

      // remove prefix and suffix strings from named variable: ${VAR} --> VAR
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

    // replace cout with scanner_output(buf, size)
    void dump(void) {
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

  private:
    std::string                         prefix;
    std::string                         suffix;

    bool                                report;
    std::string                         report_message;

    std::map<std::string, std::string>  map;

};


//! Class that .
class func_args {
  public:
    class arg_term { public: std::string name, value; bool positional; };

    //! constructor.
    func_args(const std::string& p="arg") {
      clear();
      set_pos_prefix( p );
    }
    //! destructor.
    ~func_args(void) { }

    void set_pos_prefix(const std::string& s) { pos_prefix = s; }

    void clear(void) { argV.clear(); next_name.clear(); pos_count=0; }

    void set_next_name(const std::string& n) { next_name = n; }

    void store(const std::string& v) {
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

    bool empty(void) { return( argV.empty() ); }

    size_t size(void) { return( argV.size() ); }
    size_t size(bool n=true, bool p=true) {
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

    bool exists(const std::string& n) {
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

    std::string value(const std::string& n) {
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

    std::vector<std::string> values_v(bool n=false, bool p=true) {
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

    std::string values_str(bool n=false, bool p=true) {
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

    std::vector<std::string> names_v(bool n=true, bool p=false) {
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

    std::string names_str(bool n=false, bool p=true) {
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

    std::string pairs_str(bool n=true, bool p=false, std::string a="=") {
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

    // replace cout with scanner_output(buf, size)
    void dump(void) {
      std::cout << "(";

      for ( std::vector<arg_term>::iterator it=argV.begin(); it!=argV.end(); ) {
        std::cout << it->name << "=" << it->value;

        ++it;
        if ( it!=argV.end() ) std::cout << " ";
      }

      std::cout << ")";
    }

    std::vector<arg_term>   argV;

  private:

    size_t                  pos_count;
    std::string             pos_prefix;
    std::string             next_name;

    std::string to_string(const unsigned v) {
      std::ostringstream os;
      os << std::dec << v;

      return ( os.str() );
    }
};


} /* end namespace ODIF */

#endif /* END __ODIF_UTIL_HPP__ */


/*******************************************************************************
// eof
*******************************************************************************/
