#!/usr/bin/make -f
################################################################################
#
#  openscad-amu: Project Makefile (this file should be renamed to Makefile).
#
################################################################################

# AMU_TOOL_VERSION and AMU_TOOL_PREFIX may be commented to use the default
# version found in the shell search path.

AMU_TOOL_VERSION    := @__PACKAGE_VERSION__@

# Use library/tools from install path
AMU_TOOL_PREFIX     := @bindir@/
AMU_LIB_PATH        := @__LIB_PATH__@

# Use library/tools from source and build path (uncomment both)
#AMU_TOOL_PREFIX    := @abs_top_builddir@/src/
#AMU_LIB_PATH       := @abs_top_srcdir@/share

AMU_PM_PREFIX       := $(AMU_LIB_PATH)/include/pmf/
AMU_PM_INIT         := $(AMU_PM_PREFIX)amu_pm_init
AMU_PM_RULES        := $(AMU_PM_PREFIX)amu_pm_rules

#AMU_PM_VERBOSE     := defined
#AMU_PM_DEBUG       := defined

#------------------------------------------------------------------------------#
# Project Makefile Init (DO NO EDIT THIS SECTION)
#------------------------------------------------------------------------------#
define AMU_SETUP_ANNOUNCE

 $1 not found...
 Tried [$2].

 Please update AMU_TOOL_PREFIX and AMU_LIB_PATH in $(lastword $(MAKEFILE_LIST))
 as needed for your installation or setup openscad-amu ($(AMU_TOOL_VERSION))
 using the following:

 $$ wget http://git.io/setup-amu.bash && chmod +x setup-amu.bash
 $$ sudo ./setup-amu.bash --branch $(AMU_TOOL_VERSION) --yes --install

endef

ifeq ($(wildcard $(AMU_PM_INIT)),)
$(info $(call AMU_SETUP_ANNOUNCE,Init file,$(AMU_PM_INIT)))
$(error unable to continue.)
else
include $(AMU_PM_INIT)
endif

#------------------------------------------------------------------------------#
# Default Overrides
#------------------------------------------------------------------------------#
#parallel_jobs                          := $(true)
#version_checks                         := $(false)
#target_headings                        := $(false)
#verbose_seam                           := $(false)
generate_latex                          := $(false)
#debug_dif_filter                       := $(true)
release_archive_doxygen                 := $(true)
#release_archive_scopes                 := $(true)

#------------------------------------------------------------------------------#
# Version Checks
#------------------------------------------------------------------------------#
#ifeq ($(version_checks),$(true))
#$(call check_version,doxygen,ge,1.8.12,$(false),designed for v1.8.12.)
#$(call check_version,openscad,gt,2016.08,$(true),uses new feature.)
#$(call check_version,amuseam,eq,1.5,$(true),works only with v1.5.)
#$(call check_version,amudif,lt,1.5,$(true),depreciated feature required.)
#endif

#------------------------------------------------------------------------------#
# Project
#------------------------------------------------------------------------------#
project_name        := template
project_version     := 1.0
project_brief       := openscad-amu Project Template (df1).

docs_group_id       := none
project_logo        := design_logo_top_200x50

#design_prefix      := ./
design              := design

#library_prefix     := ./
library             := library

doxygen_config      := Doxyfile

#------------------------------------------------------------------------------#
# Project Makefile Rules (DO NO EDIT THIS SECTION)
#------------------------------------------------------------------------------#
ifeq ($(wildcard $(AMU_PM_RULES)),)
$(info $(call AMU_SETUP_ANNOUNCE,Rules file,$(AMU_PM_RULES)))
$(error unable to continue.)
else
include $(AMU_PM_RULES)
endif

################################################################################
# eof
################################################################################
