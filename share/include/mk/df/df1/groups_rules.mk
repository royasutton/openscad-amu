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
# Rules
################################################################################

#------------------------------------------------------------------------------#
# Build Targets
#------------------------------------------------------------------------------#

.PHONY: all-groups
all-groups: | silent
	$(call target_end)

#------------------------------------------------------------------------------#
# Clean Targets
#------------------------------------------------------------------------------#

.PHONY: clean-groups
clean-groups: | silent
	$(call target_end)

#------------------------------------------------------------------------------#
# Install / Uninstall Targets
#------------------------------------------------------------------------------#

.PHONY: install-groups
install-groups: | silent
	$(call target_end)

.PHONY: uninstall-groups
uninstall-groups: | silent
	$(call target_end)

#------------------------------------------------------------------------------#
# Information Targets
#------------------------------------------------------------------------------#

.PHONY: info-groups
info-groups: | silent
	$(call heading1, Groups)
	$(call list_variables,$(groups_control_variables),$(true), control)
	$(call list_variables,$(groups_config_variables),$(false), variables)
	$(foreach x, \
		design \
		library \
		groups \
		groups_release_add \
		groups_backup_add \
		groups_project_add \
	,$(call enumerate_variable,$x,$(false),$(false)))

################################################################################
# eof
################################################################################
