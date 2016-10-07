## Library bootloader.
#/##############################################################################
#
#   \file   bootloader.bash
#
#   \author Roy Allen Sutton <royasutton@hotmail.com>.
#   \date   2016
#
#   \copyright
#     This file is part of OpenSCAD AutoMake Utilities ([openscad-amu]
#     (https://github.com/royasutton/openscad-amu)).
#
#     openscad-amu is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version.
#
#     openscad-amu is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     [GNU General Public License] (https://www.gnu.org/licenses/gpl.html)
#     for more details.
#
#     You should have received a copy of the GNU General Public License
#     along with openscad-amu.  If not, see <http://www.gnu.org/licenses/>.
#
#   \details
#
#     This reads the required library functions in the appropriate
#     order into a makefile script, sets basic environment variables and
#     locates required system commands.
#
#     This is usually done near the top of the script. Prior to sourcing,
#     the environment variable \c ${\__LIB_PATH\__} must be set.
#
#   \ingroup library_core
###############################################################################/


#==============================================================================
# enforce requirements
#==============================================================================

# minimum bash version.
if [[ "$BASH_VERSINFO" -lt 4 ]] ; then
  echo >&2 "requires bash >= 4."
  return 1 2>/dev/null
  exit 1
fi

# ${__LIB_PATH__} must be set.
if [[ -z "${__LIB_PATH__}" ]] ; then
  echo >&2 "\${__LIB_PATH__} not set, aborting."
  return 1 2>/dev/null
  exit 1
fi

# ${__LIB_PATH__}/lib must exist.
if [[ ! -d "${__LIB_PATH__}/lib" ]] ; then
  echo >&2 "\${__LIB_PATH__}=[${__LIB_PATH__}] and" \
           "${__LIB_PATH__}/lib directory does not exist, aborting."
  return 1 2>/dev/null
  exit 1
fi


#==============================================================================
# library functions global variables
#==============================================================================

# print.bash and print_textbox.bash outpt prefix
declare script_base_name="amu"

# exception.bash short file name
declare exception_call_stack_basename="true"

# table.bash prefix
declare table_var_prefix="_amu_"


#==============================================================================
# load library functions
#==============================================================================

# order dependency exists for: system -> exception -> print
lib_list="
  print.bash
  print_textbox.bash
  exception.bash
  table.bash
  openscad_opts.bash
  utility.bash
  make_makefile.bash
"

[[ -n $__VERBOSE__ ]] && \
echo "loading automake utilities library functions:"

for lib in $lib_list ; do
  [[ -n $__VERBOSE__ ]] && \
  echo " " source ${__LIB_PATH__}/lib/$lib

  source ${__LIB_PATH__}/lib/$lib
done


#==============================================================================
# general variables
#==============================================================================

declare __SYSTEM__="$(uname -s)"
declare __MACHINE__="$(uname -m)"

declare __RUN_HOST__="${HOSTNAME%%.*}"
declare __RUN_DIR__="$(pwd)"

declare __RUN_DATE__="$(date +%y%m%d)"
declare __RUN_TIME__="$(date +%H%M%S)"

declare __RUN_TIME_LONG__="$(date "+%a %b %e %T %Z %Y")"


#==============================================================================
# required system commands
#==============================================================================

required_cmd_tovar -v sc_tput -l "/usr/bin/tput" -o "cols"
required_cmd_tovar -v sc_seq -l "/usr/bin/seq"

required_cmd_tovar -v sc_openscad -s -l "openscad-nightly openscad"
required_cmd_tovar -v sc_convert -s -l "convert"


case "$__SYSTEM__" in
  CYGWIN_NT*)
    required_cmd_tovar -v sc_mkdir -l "/usr/bin/mkdir" -o "--parents --verbose"
    required_cmd_tovar -v sc_rmdir -l "/usr/bin/rmdir" -o "--verbose"
    required_cmd_tovar -v sc_rm -l "/usr/bin/rm" -o "--force --verbose"
    required_cmd_tovar -v sc_time -l "/usr/bin/time" -o "--append --verbose -o"
  ;;
  FreeBSD)
    required_cmd_tovar -v sc_mkdir -l "/bin/mkdir" -o "-pv"
    required_cmd_tovar -v sc_rmdir -l "/bin/mkdir" -o "-v"
    required_cmd_tovar -v sc_rm -l "/bin/rm" -o "-fv"
    required_cmd_tovar -v sc_time -l "/usr/bin/time" -o "-a -l -o"
  ;;
  Linux)
    required_cmd_tovar -v sc_mkdir -l "/bin/mkdir" -o "--parents --verbose"
    required_cmd_tovar -v sc_rmdir -l "/bin/rmdir" -o "--verbose"
    required_cmd_tovar -v sc_rm -l "/bin/rm" -o "--force --verbose"
    required_cmd_tovar -v sc_time -l "/usr/bin/time" -o "--append --verbose -o"
  ;;
  *)
    abort_error "Command environment not configured for ($__SYSTEM__)," \
                "please edit system configuration file (system.bash) as needed."
  ;;
esac


#==============================================================================
# filename extensions
#==============================================================================

declare ext_scad="scad"
declare ext_makefile="makefile"
declare ext_time="time"
declare ext_depend="depend"
declare ext_log="log"


#==============================================================================
# other library functions global variables
#==============================================================================

# console textbox widths: current console width and 95%
declare -i print_textbox_term_width=$($sc_tput $sc_tput_opts)
declare -i print_textbox_box_width=$((print_textbox_term_width * 95/100))

# makefile textbox widths:
declare -i makefile_textbox_term_width=80
declare -i makefile_textbox_box_width=80


#==============================================================================
# done
#==============================================================================

[[ -n $__VERBOSE__ ]] && \
echo "done."

return 0


#==============================================================================
# eof
#==============================================================================
