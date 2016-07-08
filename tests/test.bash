#!/bin/bash

__VERBOSE__=true
__LIBPATH__="/vol/home/rsutton/projects/cnc/src/openscad-amu/v1.0/share"
source "${__LIBPATH__}/lib/bootloader.bash"

defines name "joint" define "possition" sequence -100 1 125

table_dump joint
