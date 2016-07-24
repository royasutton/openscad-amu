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

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <string>


namespace ODIF{

//! Class that .
class env_var {
  public:
    //! constructor.
    env_var(const std::string& p="${", const std::string& s="}",
            bool r=true, const std::string& rm="<UNDEFINED>");
    //! destructor.
    ~env_var(void);

    void clear(void) { map.clear(); }

    void set_prefix(const std::string& s) { prefix=s; }
    void set_suffix(const std::string& s) { suffix=s; }

    void set_report(bool s) { report=s; }
    void set_report_message(const std::string& s) { report_message=s; }

    void store(const std::string& n, const std::string& v) { map[ n ] = v; }

    std::string expand(const std::string& v) {
      return (expand(v, report, report_message) ); }
    std::string expand(const std::string& v, bool r) {
      return (expand(v, r, report_message) ); }
    std::string expand(const std::string& v, bool r, const std::string& rm);

    void dump(void);

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

    class arg_term {
      public:
        std::string name;
        std::string value;
        bool positional;
    };

    //! constructor.
    func_args(const std::string& p="arg");
    //! destructor.
    ~func_args(void);

    void clear(void) { argV.clear(); next_name.clear(); pos_count=0; }

    void set_prefix(const std::string& s) { pos_prefix = s; }

    void set_next_name(const std::string& n) { next_name = n; }

    void store(const std::string& v);

    bool empty(void) { return( argV.empty() ); }

    size_t size(void) { return( argV.size() ); }
    size_t size(bool n, bool p=false);

    bool exists(const std::string& n);

    std::string arg(const size_t n);
    std::string arg(const std::string& n);

    std::string arg_firstof(const std::string& n1,
                            const std::string& n2="",
                            const std::string& n3="",
                            const std::string& n4="",
                            const std::string& n5="",
                            const std::string& n6="");

    std::vector<std::string> values_v(bool n=false, bool p=true);
    std::string values_str(bool n=false, bool p=true);

    std::vector<std::string> names_v(bool n=true, bool p=false);
    std::string names_str(bool n=false, bool p=true);

    std::string pairs_str(bool n=true, bool p=false, std::string a="=");

    void dump(void);

    std::vector<arg_term>   argV;

  private:
    std::string             pos_prefix;
    size_t                  pos_count;

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
