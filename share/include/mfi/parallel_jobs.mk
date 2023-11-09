################################################################################
# share/include/mfi/parallel_jobs.mf
################################################################################

undefine os_defined
undefine os_configured

ifdef os
  os_defined := true
else
  os := $(shell uname -s)
endif

#------------------------------------------------------------------------------#

# Linux
ifeq (${os},Linux)
  cpu_cnt := $(shell nproc)
  os_configured := true
endif

# CYGWIN - compare prefix only (ie: CYGWIN_NT-10.0 --> CYGWIN_NT)
ifeq ($(firstword $(strip $(subst -, ,$(os)))),CYGWIN_NT)
  cpu_cnt := $(shell nproc)
  os_configured := true
endif

# FreeBSD
ifeq (${os},FreeBSD)
  cpu_cnt := $(shell sysctl -n hw.ncpu)
  os_configured := true
endif

#------------------------------------------------------------------------------#

ifdef os_configured
  ifdef verbose_make
    $(warning $(cpu_cnt) CPU's detected, configuring parallel execution --jobs=$(cpu_cnt).)
  endif

  MAKEFLAGS += --jobs=$(cpu_cnt)
  undefine os_configured
else
  $(warning OS [$(os)] not configured;)
  $(warning see file [$${__LIB_PATH__}/include/mfi/parallel_jobs.mf])
endif

ifdef os_defined
  undefine os_defined
else
  undefine os
endif

################################################################################
# eof
################################################################################
