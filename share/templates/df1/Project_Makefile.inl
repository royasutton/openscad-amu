#!/usr/bin/make -f
################################################################################
#
#  openscad-amu: Project Makefile (this file should be renamed to Makefile).
#
################################################################################

# Use library/tools from install path
AMU_LIB_PATH            := @__LIB_PATH__@
AMU_TOOL_PREFIX         := @bindir@/

# Use library/tools from source and build path
#AMU_LIB_PATH           := @abs_top_srcdir@/share
#AMU_TOOL_PREFIX        := @abs_top_builddir@/src/

AMU_PM_PREFIX           := $(AMU_LIB_PATH)/include/pmf/
#AMU_PM_VERBOSE         := defined
#AMU_PM_DEBUG           := defined
#AMU_TOOL_VERSION       := @__PACKAGE_VERSION__@

include $(AMU_PM_PREFIX)amu_pm_init

#------------------------------------------------------------------------------#
# Default Overrides
#------------------------------------------------------------------------------#
#amu_pm_parallel_jobs  := $(true)
#verbose_seam          := $(false)
#debug_dif_filter      := $(true)

#------------------------------------------------------------------------------#
# Project
#------------------------------------------------------------------------------#
#design_prefix  := ./
#library_prefix := ./

version        := 0.1

design         := design
library        := library

doxygen_config := Doxyfile

#------------------------------------------------------------------------------#
include $(AMU_PM_PREFIX)amu_pm_rules
################################################################################
# eof
################################################################################
