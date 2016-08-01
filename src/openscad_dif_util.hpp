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

//! \ingroup openscad_dif_src
//! @{

namespace ODIF{

//! Class to manage environment variables.
class env_var {
  public:
    //! \brief environment variable class constructor.
    //! \param p    variable prefix.
    //! \param s    variable suffix.
    //! \param e    variable matching regular expression.
    //! \param ep   variable matching escape prefix.
    //! \param es   variable matching escape suffix.
    //! \param epl  variable matching escape prefix string length.
    //! \param esl  variable matching escape suffix string length.
    //! \param r    report non-existent variables.
    //! \param rm   report message for non-existent variables.
    env_var(const std::string& p="${", const std::string& s="}",
            const std::string& e="\\${[_[:alnum:]]+}",
            const std::string& ep="\\\\", const std::string& es="",
            const size_t& epl=1, const size_t& esl=0,
            bool r=true, const std::string& rm="<UNDEFINED>");
    //! environment variable class destructor.
    ~env_var(void);

    //! clear all stored environment variables.
    void clear(void) { map.clear(); }

    //! set variable prefix.
    void set_prefix(const std::string& s) { prefix=s; }
    //! get variable prefix.
    std::string get_prefix(void) { return ( prefix ); }
    //! set variable suffix.
    void set_suffix(const std::string& s) { suffix=s; }
    //! get variable suffix.
    std::string get_suffix(void) { return ( suffix ); }
    //! set variable matching regular expression.
    void set_regexp(const std::string& s) { regexp=s; }
    //! get variable matching regular expression.
    std::string get_regexp(void) { return ( regexp ); }
    //! set variable matching escape prefix.
    void set_escape_prefix(const std::string& s) { escape_prefix=s; }
    //! get variable matching escape prefix.
    std::string get_escape_prefix(void) { return ( escape_prefix ); }
    //! set variable matching escape suffix.
    void set_escape_suffix(const std::string& s) { escape_suffix=s; }
    //! get variable matching escape suffix.
    std::string get_escape_suffix(void) { return ( escape_suffix ); }
    //! set variable matching escape prefix string length.
    void set_escape_prefix_length(const size_t& s) { escape_prefix_length=s; }
    //! get variable matching escape prefix string length.
    size_t get_escape_prefix_length(void) { return ( escape_prefix_length ); }
    //! set variable matching escape suffix string length.
    void set_escape_suffix_length(const size_t& s) { escape_suffix_length=s; }
    //! get variable matching escape suffix string length.
    size_t get_escape_suffix_length(void) { return ( escape_suffix_length ); }

    //! set report non-existent variables.
    void set_report(bool s) { report=s; }
    //! get report non-existent variables.
    bool get_report(void) { return ( report ); }
    //! set report message for non-existent variables.
    void set_report_message(const std::string& s) { report_message=s; }
    //! get report message for non-existent variables.
    std::string get_report_message(void) { return ( report_message ); }

    //! test for the existence of variable name n
    bool exists(const std::string& n) { return( map.find( n ) != map.end() ); }

    //! store a variable name and value pair.
    void store(const std::string& n, const std::string& v) { map[ n ] = v; }

    //! \brief expand a variable.
    //! \param v  formatted variable name (prefix+name+suffix).
    //! \param r  report if non-existent.
    //! \param rm report message.
    //! \returns the value of the variable or the report message.
    std::string expand(const std::string& v, bool r, const std::string& rm);
    //! \copybrief expand
    //! \details \see expand. default is used for parameter \p rm.
    //! \overload
    std::string expand(const std::string& v, bool r);
    //! \copybrief expand
    //! \details \see expand. default is used for parameters \p r and \p rm.
    //! \overload
    std::string expand(const std::string& v);

    //! \brief expand all variables in the text string.
    //! \param t  text string to expand.
    //! \param r  report all non-existent variables.
    //! \param rm non-existent variables report message.
    //! \returns a text string with all variables replaced by their value or the report message.
    std::string expand_text(const std::string& t, bool r, const std::string& rm);
    //! \copybrief expand_text
    //! \details \see expand_text. default is used for parameter \p rm.
    //! \overload
    std::string expand_text(const std::string& t, bool r);
    //! \copybrief expand_text
    //! \details \see expand_text. default is used for parameters \p r and \p rm.
    //! \overload
    std::string expand_text(const std::string& t);

    //! helper function for \ref expand_text that performs each expansion pass.
    size_t expand_textP(const std::string& t, bool r, const std::string& rm,
                        std::string& et);

    //! simple dump of all variables to standard out.
    void dump(void);

  private:
    std::string     prefix;               //!< variable prefix.
    std::string     suffix;               //!< variable suffix.
    std::string     regexp;               //!< variable match regular expression.
    std::string     escape_prefix;        //!< variable matching escape prefix.
    std::string     escape_suffix;        //!< variable matching escape suffix.
    size_t          escape_prefix_length; //!< variable matching escape prefix string length.
    size_t          escape_suffix_length; //!< variable matching escape suffix string length.

    bool            report;               //!< report non-existent variables.
    std::string     report_message;       //!< report message for non-existent variables.

    std::map<std::string,
             std::string> map;            //!< variable storage map.
};


//! Class that manages both named and positional arguments.
class func_args {
  public:

    //! Class that records each argument.
    class arg_term {
      public:
        std::string name;               //!< argument name.
        std::string value;              //!< argument value.
        bool positional;                //!< argument type.
    };

    //! \brief environment variable class constructor.
    //! \param p  positional argument name prefix.
    func_args(const std::string& p="arg");
    //! environment variable class destructor.
    ~func_args(void);

    //! clears all stored arguments.
    void clear(void) { argv.clear(); next_name.clear(); pos_count=0; }

    //! set the positional argument name prefix.
    void set_prefix(const std::string& s) { pos_prefix = s; }
    //! get the positional argument name prefix.
    std::string get_prefix(void) { return ( pos_prefix ); }

    //! set the name for the next argument.
    void set_next_name(const std::string& n) { next_name = n; }
    //! get the name for the next argument.
    std::string get_next_name(void) { return ( next_name ); }

    //! store a positional (or named) argument value.
    void store(const std::string& v);
    //! store a named argument name-value pair.
    //! \overload
    void store(const std::string& n, const std::string& v);

    //! test if the argument vector contains zero arguments.
    bool empty(void) { return( argv.empty() ); }

    //! return the number of all arguments stored.
    size_t size(void) { return( argv.size() ); }
    //! \brief return the number of named and/or positional arguments stored.
    //! \param n  include named arguments.
    //! \param p  include positional arguments.
    size_t size(bool n, bool p=false);

    //! test for the existence of the named argument n.
    bool exists(const std::string& n);

    //! return the named or positional argument value at location n.
    std::string arg(const size_t n);
    //! return the value for the argument named n.
    std::string arg(const std::string& n);

    //! return the value of the first argument that exists of n1, n2, ..., n6.
    std::string arg_firstof(const std::string& n1,
                            const std::string& n2="",
                            const std::string& n3="",
                            const std::string& n4="",
                            const std::string& n5="",
                            const std::string& n6="");

    //! \brief return the vector of the values for all arguments.
    //! \param n  include named arguments.
    //! \param p  include positional arguments.
    std::vector<std::string> values_v(bool n=false, bool p=true);
    //! \brief return a string of the values for all arguments.
    //! \param n  include named arguments.
    //! \param p  include positional arguments.
    std::string values_str(bool n=false, bool p=true);

    //! \brief return the vector of the names for all arguments.
    //! \param n  include named arguments.
    //! \param p  include positional arguments.
    std::vector<std::string> names_v(bool n=true, bool p=false);
    //! \brief return a string of the names for all arguments.
    //! \param n  include named arguments.
    //! \param p  include positional arguments.
    std::string names_str(bool n=false, bool p=true);

    //! \brief return a string of the names-value pairs for all arguments.
    //! \param n  include named arguments.
    //! \param p  include positional arguments.
    //! \param a  name-value pair relation string.
    std::string pairs_str(bool n=true, bool p=false, std::string a="=");

    //! simple dump of all arguments to standard out.
    void dump(void);

    std::vector<arg_term> argv;         //!< vector of argument terms.

  private:
    std::string           pos_prefix;   //!< positional argument name prefix.
    size_t                pos_count;    //!< positional argument count.

    std::string           next_name;    //!< next argument name string.

    //! convert an unsigned integer to a string.
    std::string to_string(const unsigned v) {
      std::ostringstream os;
      os << std::dec << v;
      return ( os.str() );
    }
};


} /* end namespace ODIF */

#endif /* END __ODIF_UTIL_HPP__ */

//! @}


/*******************************************************************************
// eof
*******************************************************************************/
