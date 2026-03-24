#!/usr/bin/make -f
################################################################################
#
#  Copyright (C) 2026 Roy Allen Sutton
#
#  This file is part of OpenSCAD AutoMake Utilities ([openscad-amu]
#  (https://royasutton.github.io/openscad-amu)).
#
#  openscad-amu is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  openscad-amu is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  [GNU General Public License] (https://www.gnu.org/licenses/gpl.html)
#  for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with openscad-amu.  If not, see <http://www.gnu.org/licenses/>.
#
################################################################################

################################################################################
# Variables
################################################################################

#------------------------------------------------------------------------------#
# Configuration Variable List
#------------------------------------------------------------------------------#

groups_config_variables := \
  groups_name \
  groups_exclude

#------------------------------------------------------------------------------#
# Help Text
#------------------------------------------------------------------------------#

define help_text_groups
 all-groups, clean-groups, install-groups, uninstall-groups:
     do nothing.

 info-groups:
     groups components information.
endef

################################################################################
# eof
################################################################################
