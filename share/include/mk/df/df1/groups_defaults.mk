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
# Defaults
################################################################################

# default group name (predefine before calling AMU_PM_INIT)
ifndef groups_name
  groups_name                           := group
else
  $(call amu_pm_m,using predefined value for $$(groups_name))
endif

#------------------------------------------------------------------------------#
# initialize file lists
#------------------------------------------------------------------------------#
# use recursive assignment '=' for references that use derived paths

design                                  :=
library                                 :=

groups                                  :=

groups_release_add                       =
groups_backup_add                       :=
groups_project_add                      :=

ignore_groups_exclude                   := $(false)

#------------------------------------------------------------------------------#
# Control Variable List
#------------------------------------------------------------------------------#

groups_control_variables                := \
  ignore_groups_exclude

################################################################################
# eof
################################################################################
