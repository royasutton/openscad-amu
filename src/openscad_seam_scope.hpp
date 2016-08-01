/***************************************************************************//**

  \file   openscad_seam_scope.hpp

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
    Script extractor scope class header.

  \ingroup openscad_seam_src
*******************************************************************************/

#ifndef __SEAM_SCOPE_HPP__
#define __SEAM_SCOPE_HPP__ 1

#include <string>

namespace SEAM{

//! Class that tracts scope hierarchy.
class SEAM_Scope {
  public:
    //! \brief constructor for root scope.
    //! \param cs root scope name.
    SEAM_Scope(const std::string& cs)
    {
      mode = 'r';
      cur_scope = cs;
      new_scope.clear();
    }
    //! \brief constructor for non-root scope with existing parent scope.
    //! \param cs parent scope name.
    //! \param ns new scope member name.
    //! \param m  scope naming mode.
    //! \todo the mode character should be converted to lower case and be
    //!       validated to be one of the possible values [r,a,p].
    SEAM_Scope(const std::string& cs,
               const std::string& ns,
               const char m)
    {
      mode = m;
      cur_scope = cs;
      new_scope = ns;
    }
    //! destructor.
    ~SEAM_Scope() {}

    //! return current scope name.
    std::string name(void)
    {
      if ( new_scope.empty() || (mode=='r') )
        return cur_scope;

      if (mode == 'p')
        return new_scope+cur_scope;
      else
        return cur_scope+new_scope;
    }

  private:
    // state variables
    char mode;                //!< scope mode; one of [r,a,p] : Root, Append, Prepend.
    std::string cur_scope;    //!< parent scope.
    std::string new_scope;    //!< this scope.
};


} /* end namespace SEAM */

#endif /* END __SEAM_SCOPE_HPP__ */


/*******************************************************************************
// eof
*******************************************************************************/
