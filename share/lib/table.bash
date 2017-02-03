## Hash table data storage functions.
#/##############################################################################
#
#   \file   table.bash
#
#   \author Roy Allen Sutton <royasutton@hotmail.com>.
#   \date   2016-2017
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
#     These functions emulate a hash table data structure. Data keys are
#     stored as environment variables with associated values. Key names
#     must conform to BASH environment variable naming rules: namely,
#     in the form <tt>[a-zA-Z_]+[a-zA-Z0-9_]*</tt>.
#
#     For applications that require multiple table maps, the environment
#     variable \c ${table_var_prefix} may be used to separate the table
#     storage. When not specified, the prefix \c \__table\__ is used.
#
#   \ingroup library_support
###############################################################################/

## \ingroup library_support
## @{


## Store the key and value string in the hash table.
#/##############################################################################
# \afn table_set
#
# \aparami <required arg1 string> table
# \aparami <required arg2 string> key
# \aparami <required arg3 string> value
#
# \returns void
#
# \details
#   Stores the \p value string hashed by \p key in the \p table.
#
###############################################################################/
function table_set()
{
  declare local table=$1
  declare local key=$2
  declare local value=$3
  shift 3

  eval "${table_var_prefix:-__table__}${table}_${key}=\"${value}\""

  return
}


## Echo the key name for given table keyid.
#/##############################################################################
# \afn table_getkeyname_echo
#
# \aparami <required arg1 string> table
# \aparami <required arg2 string> keyid
# \aparami <optional argv> prefixes
#
# \returns void
#
# \details
#   Set keys are stored in environment variables prefixed by \c $table_var_prefix
#   and the \p table name. This forms the \p keyid. This function removes these
#   prefixes and echo just the key name. All other arguments are prefixed to
#   the extracted key name.
#
###############################################################################/
function table_getkeyname_echo()
{
  declare local table=$1
  declare local key=$2
  shift 2

  echo $* ${key##${table_var_prefix:-__table__}${table}_}

  return
}


## Write the key name for given table keyid to a variable.
#/##############################################################################
# \afn table_getkeyname_tovar
#
# \aparami <required arg1 string> table
# \aparami <required arg2 string> keyid
# \aparamo <required arg3 string> variable
# \aparami <optional argv> suffixes
#
# \returns void
#
# \details
#   Set keys are stored in environment variables prefixed by \c $table_var_prefix
#   and the \p table name. This forms the \p keyid. This function removes these
#   prefixes and writes just the key name to \p variable. All other arguments
#   are appended after extracted key name.
#
###############################################################################/
function table_getkeyname_tovar()
{
  declare local table=$1
  declare local key=$2
  declare local var=$3
  shift 3

  [ -n "$var" ] && eval "$var=\"$(table_getkeyname_echo $table $key) $*\""

  return
}


## Echo the key's value string stored in the table.
#/##############################################################################
# \afn table_getval_echo
#
# \aparami <required arg1 string> table
# \aparami <required arg2 string> key
# \aparami <optional argv> prefixes
#
# \returns void
#
# \details
#   Get and echo the string \p value stored under \p key in the hash table.
#   All other arguments are prefixed to the \p value.
#
###############################################################################/
function table_getval_echo()
{
  declare local table=$1
  declare local key=$2
  shift 2

  eval "echo $* \"\$${table_var_prefix:-__table__}${table}_${key}\""

  return
}


## Write the key's value string stored in the table to a variable.
#/##############################################################################
# \afn table_getval_tovar
#
# \aparami <required arg1 string> table
# \aparami <required arg2 string> key
# \aparamo <required arg3 string> variable
# \aparami <optional argv> suffixes
#
# \returns void
#
# \details
#   Get the string \p value stored under \p key in the hash table
#   and write it to \p variable. All other arguments are appended
#   after the \p value.
#
###############################################################################/
function table_getval_tovar()
{
  declare local table=$1
  declare local key=$2
  declare local var=$3
  shift 3

  #[ -n "$var" ] && eval "$var=\"$(table_getval_echo $table $key) $*\""

  # avoid double eval that results with above code to retain escaped quotes
  # in variable vales repeat table_getval_echo() function directly here:
  [ -n "$var" ] &&
    eval "$var=\"$(echo \"\$${table_var_prefix:-__table__}${table}_${key}\") $*\""

  return
}


## Unset a key and its value in the hash table.
#/##############################################################################
# \afn table_unset
#
# \aparami <required arg1 string> table
# \aparami <required arg2 string> key
#
# \returns void
#
# \details
#   Unset the string \p value and \p key stored in the hash table.
#
###############################################################################/
function table_unset()
{
  declare local table=$1
  declare local key=$2
  shift 2

  eval "unset ${table_var_prefix:-__table__}${table}_${key}"

  return
}


## Test if a key exists in the hash table.
#/##############################################################################
# \afn table_key_exists
#
# \aparami <required arg1 string> table
# \aparami <required arg2 string> key
#
# \retval 0 if key exists
# \retval 1 if key does not exist
#
# \details
#   Test if the named \p key exists in the hash \p table. Return 0 when it does
#   exists and 1 when it does not.
#
###############################################################################/
function table_key_exists()
{
  [[ -z "$2" ]] && return 1

  declare local table=$1
  declare local key=$2
  shift 2

  eval "test -z \${${table_var_prefix:-__table__}${table}_${key}+x}"

  if [[ $? -eq 0 ]] ; then
    return 1
  else
    return 0
  fi
}


## Test if the stored value for a key is empty.
#/##############################################################################
# \afn table_key_value_empty
#
# \aparami <required arg1 string> table
# \aparami <required arg2 string> key
#
# \retval 0 if value is the null empty string
# \retval 1 if value is not empty
#
# \details
#   Test if the valued stored in the hash \p table for a \p key is empty.
#   Return 0 when empty and 1 otherwise.
#
###############################################################################/
function table_key_value_empty()
{
  [[ -z "$2" ]] && return 0

  declare local table=$1
  declare local key=$2
  shift 2

  [[ -z "$(table_getval_echo $table $key)" ]] && return 0

  return 1
}


## Echo the list of keyids in the named hash table.
#/##############################################################################
# \afn table_getkeyids_echo
#
# \aparami <required arg1 string> table
# \aparami <optional argv> prefixes
#
# \returns void
#
# \details
#   Get and echo the list of all \c keyids in the named hash table.
#   All other arguments are prefixed to the list.
#
###############################################################################/
function table_getkeyids_echo()
{
  declare local table=$1
  shift 1

  eval "echo $* \${!${table_var_prefix:-__table__}${table}_*}"

  return
}


## Write the list of keyids in the named hash table to a variable.
#/##############################################################################
# \afn table_getkeyids_tovar
#
# \aparami <required arg1 string> table
# \aparamo <required arg2 string> variable
# \aparami <optional argv> suffixes
#
# \returns void
#
# \details
#   Get the list of all \c keyids in the named hash table and write to the.
#   \p variable. All other arguments are appended after the list.
#
###############################################################################/
function table_getkeyids_tovar()
{
  declare local table=$1
  declare local var=$2
  shift 2

  [ -n "$var" ] && eval "$var=\"$(table_getkeyids_echo $table) $*\""

  return
}


## Invoke a function for each key in the hash table.
#/##############################################################################
# \afn table_foreach
#
# \aparami <required arg1 string> table
# \aparamo <required arg2 function> function
# \aparami <optional argv> args
#
# \returns void
#
# \details
#   Call the named \p function for each key in the named hash table. The
#   \p key will be passed as the second argument; the \p value will be passed
#   as the third argument; and any arguments \p args will be appended to the
#   end of each function invocation.
#
#   Each invocation will be of the form: <tt>function key "value" args</tt>.
#
#   Example:
#   \code{.sh}
#   function foreach_function() {
#     declare local key=$1
#     declare local value="$2"
#     shift 2
#     declare local argv="$*"
#
#     echo $key $value $argv
#   }
#
#   table_foreach views foreach_function arg1 arg2 ... argn
#   \endcode
#
###############################################################################/
function table_foreach()
{
  declare local table=$1
  declare local func=$2
  shift 2

  declare local keyname oldIFS="$IFS"
  IFS=' '
  for i in $(table_getkeyids_echo $table); do
    keyname=$(table_getkeyname_echo $table $i)
    eval "$func $keyname \"\$$i\" $*"
  done
  IFS="$oldIFS"

  return
}


## Echo the list of key names in the named hash table.
#/##############################################################################
# \afn table_getkeynames_echo
#
# \aparami <required arg1 string> table
# \aparami <optional argv> prefixes
#
# \returns void
#
# \details
#   Get and echo the list of all key names in the named hash table.
#   All other arguments are prefixed to the list.
#
###############################################################################/
function table_getkeynames_echo()
{
  declare local table=$1
  shift 1

  declare local keyids=$(table_getkeyids_echo $table)
  declare local keynames

  for i in $keyids; do
    keynames+=" $(table_getkeyname_echo $table $i)"
  done

  echo $* $keynames

  return
}


## Write the list of key names in the named hash table to a variable.
#/##############################################################################
# \afn table_getkeynames_tovar
#
# \aparami <required arg1 string> table
# \aparamo <required arg2 string> variable
# \aparami <optional argv> suffixes
#
# \returns void
#
# \details
#   Get and write the list of all key names in the named hash table to
#   the \p variable. All other arguments are appended after the list.
#
###############################################################################/
function table_getkeynames_tovar()
{
  declare local table=$1
  declare local var=$2
  shift 2

  [ -n "$var" ] && eval "$var=\"$(table_getkeynames_echo $table) $*\""

  return
}


## Echo the number of keys in the named hash table.
#/##############################################################################
# \afn table_size_echo
#
# \aparami <required arg1 string> table
# \aparami <optional argv> prefixes
#
# \returns void
#
# \details
#   Count each key in the named hash table and then echo the result.
#   All other arguments are prefixed to the result.
#
###############################################################################/
function table_size_echo()
{
  declare local table=$1
  shift 1

  declare -i local __rv_value=0

  function foreach_function() { let __rv_value++; }

  table_foreach $table foreach_function

  echo $* ${__rv_value}

  return
}


## Write the number of keys in the named hash table to a variable.
#/##############################################################################
# \afn table_size_tovar
#
# \aparami <required arg1 string> table
# \aparamo <required arg2 string> variable
# \aparami <optional argv> suffixes
#
# \returns void
#
# \details
#   Count each key in the named hash table and then write the result to the
#   \p variable. All other arguments are appended after the result.
#
###############################################################################/
function table_size_tovar()
{
  declare local table=$1
  declare local var=$2
  shift 2

  [ -n "$var" ] && eval "$var=\"$(table_size_echo $table) $*\""

  return
}


## Copy all members of the source table to the destination table.
#/##############################################################################
# \afn table_copy
#
# \aparami <required arg1 string> src_table
# \aparamo <required arg2 string> dst_table
#
# \returns void
#
# \details
#   For each key in the source table, retrieve its value and copy the key
#   and value to the destination table.
#
###############################################################################/
function table_copy()
{
  declare local src_opts=$1
  declare local dst_opts=$2
  shift 2

  function foreach_function()
  {
    declare local key=$1
    declare local value="$2"
    declare local table=$3

    print_m_vl 6 "copying to [$table]:key=[$key] value=[$value]"
    table_set $table $key "$value"
  }

  table_foreach $src_opts foreach_function $dst_opts

  return
}


## Unset all key members in a hash table.
#/##############################################################################
# \afn table_unset_all
#
# \aparami <required arg1 string> table
#
# \returns void
#
# \details
#   For each key in the named hash table, unset its key and value.
#
###############################################################################/
function table_unset_all()
{
  declare local table=$1
  shift 1

  function foreach_function()
  {
    declare local key=$1
    declare local value="$2"
    declare local table=$3

    print_m_vl 6 -J "table_unset [$table]:key=[$key]"
    table_unset $table $key
  }

  table_foreach $table foreach_function $table

  return
}


## Tabular formatted output of all entries in hash table.
#/##############################################################################
# \afn table_dump
#
# \aparami <required arg1 string> table
# \aparami <optional arg2 string> title
#
# \returns void
#
# \details
#   Formatted output of all entries in \p table with optional \p title. If the
#   title is not specified, then the table name is used.
#
###############################################################################/
function table_dump()
{
  declare local table=$1
  declare local title=${2-table [$table]}

  function foreach_function()
  {
    declare local key=$1
    declare local value="$2"

    let size++

    declare local num
    printf -v num "% 3d" $size

    print_textbox -lt "$num: [$key] = [$value]" -pl
  }

  declare -i local size=0

  print_textbox -bc 2 -lt " begin" -ct "$title" -rt "begin " \
                -bt -pl -hr

  table_foreach $table foreach_function

  print_textbox -lt " end" -ct "$size entries" -rt "end " \
                -hc '~' -hr -bc 1 \
                -hc '=' -pl -bb

  return
}


## Tabular formatted output of all entries in table in makefile comment block.
#/##############################################################################
# \afn table_dump_mf
#
# \aparami <required arg1 string> table
# \aparami <optional arg2 string> title
#
# \returns void
#
# \details
#   Formatted output of all entries in \p table with optional \p title. If the
#   title is not specified, then the table name is used. This version uses
#   format compatible for embedding in makefile comment block.
#
###############################################################################/
function table_dump_mf()
{
  declare local table=$1
  declare local title=${2-table [$table]}

  function foreach_function()
  {
    declare local key=$1
    declare local value="$2"

    let size++

    declare local num
    printf -v num "% 3d" $size

    print_textbox -lt "$num: [$key] = [$value]" -vc '#' -pl
  }

  declare -i local size=0

  print_textbox -bc 1 -hvc '#' -bt \
                -lt " begin" -ct "$title" -rt "begin " -pl \
                -bc 2 -cc '+' -hc '-' -hr

  table_foreach $table foreach_function

  print_textbox -bc 2 -vc '#' -cc '+' -hc '-' -hr \
                -bc 1 -cc '#' -hc '=' \
                -lt " end" -ct "$size entries" -rt "end " -pl -bb

  return
}


#==============================================================================
# protect functions
#==============================================================================
readonly -f \
  table_set \
  table_getkeyname_echo \
  table_getkeyname_tovar \
  table_getval_echo \
  table_getval_tovar \
  table_unset \
  table_key_exists \
  table_key_value_empty \
  table_getkeyids_echo \
  table_getkeyids_tovar \
  table_foreach \
  table_getkeynames_echo \
  table_getkeynames_tovar \
  table_size_echo \
  table_size_tovar \
  table_copy \
  table_unset_all \
  table_dump \
  table_dump_mf


## @}

#==============================================================================
# eof
#==============================================================================
