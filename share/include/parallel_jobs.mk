#==============================================================================#
# share/include/parallel_jobs.mf
#==============================================================================#

run.system.os           := $(shell uname -s)

ifeq (${run.system.os},FreeBSD)
openscad                := openscad
number_of_processors    := $(shell sysctl -n hw.ncpu)
endif

ifeq (${run.system.os},CYGWIN_NT-10.0)
openscad                := openscad
number_of_processors    := $(shell nproc)
endif

ifeq (${run.system.os},Linux)
openscad                := openscad
number_of_processors    := $(shell nproc)
endif

MAKEFLAGS += --jobs=$(number_of_processors)

#==============================================================================#
# eof
#==============================================================================#
