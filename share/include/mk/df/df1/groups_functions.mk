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
# Functions
################################################################################

# append group configuration
#------------------------------------------------------------------------------#
# macro: add-group (arg1,arg2,arg3,arg4,arg5,arg6)
#------------------------------------------------------------------------------#
# arg1: directory prefix (with trailing '/')
# arg2: design files
# arg3: library files
# arg4: (sub)group list
# arg5: release files
# arg6: backup files
#------------------------------------------------------------------------------#
define add-group
  $(amu_pm_d6)

  # $2: design
  ifneq ($(strip $2),$(empty))
    design += $(addprefix $(if $(strip $1),$1),$2)
  endif

  # $3: library
  ifneq ($(strip $3),$(empty))
    library += $(addprefix $(if $(strip $1),$1),$3)
  endif

  # $5: groups_release_add
  # released files are generated targets and the output path of the target
  # must be identified with the macro $(call scopes_target_path,<input-path>).
  # use delayed expansion '$$' for the correct evaluation within this macro.
  # (a) add to local $(groups_release_add)
  # (b) add to project $(release_files_add)
  ifneq ($(strip $5),$(empty))
    groups_release_add += $(addprefix $$(call scopes_target_path,$(if $(strip $1),$1)),$5)
    release_files_add += $(addprefix $$(call scopes_target_path,$(if $(strip $1),$1)),$5)
  endif

  # $6: groups_backup_add
  # (a) add to local $(groups_backup_add)
  # (b) add to project $(backup_files_add)
  ifneq ($(strip $6),$(empty))
    groups_backup_add += $(addprefix $(if $(strip $1),$1),$6)
    backup_files_add += $(addprefix $(if $(strip $1),$1),$6)
  endif

  # auto-record caller makefile as project file
  # (a) add to local $(groups_project_add)
  # (b) add to project $(project_files_add)
  groups_project_add += $(lastword $(MAKEFILE_LIST))
  project_files_add += $(lastword $(MAKEFILE_LIST))

  # $4: (sub)group
  # remove groups in $(groups_exclude) when $(ignore_groups_exclude) not true
  ifneq ($(strip $4),$(empty))

    # (sub)group
    groups += \
      $(addsuffix /$(groups_name)$(AMU_PM_SUFFIX),
        $(filter-out
          $(if $(call bool_decode,$(ignore_groups_exclude)),$(empty),$(groups_exclude)),
          $(addprefix $(if $(strip $1),$1),$4)
        )
      )

    # include (sub)group makefiles
    include \
      $(addsuffix /$(groups_name)$(AMU_PM_SUFFIX),
        $(filter-out
          $(if $(call bool_decode,$(ignore_groups_exclude)),$(empty),$(groups_exclude)),
          $(addprefix $(if $(strip $1),$1),$4)
        )
      )
  endif
endef

# macro: local group addition
#------------------------------------------------------------------------------#
# macro: add-local-group ()
#------------------------------------------------------------------------------#
define add-local-group
  $(call add-group, \
    $(local_path_prefix), \
    $(local_design), \
    $(local_library), \
    $(local_subgroups), \
    $(local_release_add), \
    $(local_backup_add) \
  )
endef

# macro: local group addition (auto)
# local_path_prefix = group makefile path prefix
#------------------------------------------------------------------------------#
# macro: add-local-group-auto ()
#------------------------------------------------------------------------------#
define add-local-group-auto
  $(call add-group, \
    $(dir $(lastword $(MAKEFILE_LIST))), \
    $(local_design), \
    $(local_library), \
    $(local_subgroups), \
    $(local_release_add), \
    $(local_backup_add) \
  )
endef

# macro: local group variable cleanup
#------------------------------------------------------------------------------#
# macro: clear-local-group ()
#------------------------------------------------------------------------------#
define clear-local-group
  undefine local_path_prefix

  undefine local_design
  undefine local_library

  undefine local_subgroups

  undefine local_release_add
  undefine local_backup_add
endef

################################################################################
# eof
################################################################################
