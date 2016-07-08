## General support utility functions.
#/##############################################################################
#
#   \file   utility.bash
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
#     General support utility functions.
#
#   \ingroup library_support
###############################################################################/

## \ingroup library_support
## @{


## Echo the current system date.
#/##############################################################################
# \afn date_echo
#
# \aparami <named> \--date            Output the current date.
# \aparami <named> \--year            Output the current year.
# \aparami <named> \--month           Output the current month.
# \aparami <named> \--day             Output the current day.
#
# \aparami <named> \--time            Output the current time.
# \aparami <named> \--hour            Output the current hour.
# \aparami <named> \--minute          Output the current minutes.
# \aparami <named> \--second          Output the current seconds.
#
# \aparami <named> \--space           Ouput a space character.
# \aparami <named string> \--string   Output the named string.
#
# \returns void
#
# \details
#   This function can be used to echo the current system date and time. It
#   uses the system \c date command.
#
###############################################################################/
function date_echo()
{
  while [ $# -gt 0 ] ; do
    case $1 in
    --date)       echo -n "$(date +%y%m%d)"                  ;;
    --year)       echo -n "$(date +%y)"                      ;;
    --month)      echo -n "$(date +%m)"                      ;;
    --day)        echo -n "$(date +%d)"                      ;;

    --time)       echo -n "$(date +%H%M%S)"                  ;;
    --hour)       echo -n "$(date +%H)"                      ;;
    --minute)     echo -n "$(date +%M)"                      ;;
    --second)     echo -n "$(date +%S)"                      ;;

    --space)      echo -n " "                                ;;
    --string)     echo -n "$2"                       ; shift ;;
    *) abort_invalid_arg $1                                  ;;
    esac

    shift 1
  done
  echo

  return
}


## Locate a required system shell command.
#/##############################################################################
# \afn required_cmd_tovar
#
# \aparamo <named path> \--vname
#           Variable name to store command path when found.
# \aparamo <named path> -v
#           Alias to \--vname.
# \aparami <optional named string> \--opts
#           Option string to store with command. These options will be stored
#           to the variable ${*}_opts where ${*} is the variable name
#           specified by \--vname.
# \aparami <optional named string> -o
#           Alias to \--opts.
# \aparami <required named path list> \--list
#           List of directory paths to check for the command. They are check
#           in the order listed.
# \aparami <required named path list> -l
#           Alias to \--list.
# \aparami <named> \--search
#           Search shell path for command. When specifying command names
#           without a specific path location, this option indicates that
#           the \c ${PATH} should be searched.
# \aparami <named> -s
#           Alias to \--search.
#
# \returns void
#
# \details
#   This functions will check for the existence of a shell command from a
#   list of possible locatations. Optionally it can search for \c ${PATH} for the
#   command. The shell command \c which is used to perform the search.
#   If the command is not found an error is reported and the script
#   will abort. When found the command is stored to the named variable and
#   if command options are specified, they will be stored as well.
#
# Example:
# \code{.sh}
#    required_cmd_tovar -v sc_tput \
#                       -l "/bin/tput /usr/bin/tput /usr/local/bin/tput" \
#                       -o "cols"
#
#    required_cmd_tovar -v sc_openscad -s -l "openscad-nightly openscad"
#
#    echo $sc_tput $sc_tput_opts
# \endcode
#
###############################################################################/
function required_cmd_tovar()
{
  declare local vname
  declare local search=false
  declare local list
  declare local opts

  declare local path

  while [ $# -gt 0 ] ; do
    case $1 in
    -v|--vname)   vname="$2"  ; shift 2 ;;
    -o|--opts)    opts="$2"   ; shift 2 ;;
    -s|--search)  search=true ; shift 1 ;;
    -l|--list)
        list="$2"
        shift 2

        declare local found=false
        for p in $list ; do
          [[ $search == true ]] && p=$(which 2>/dev/null $p)
          if [[ -e $p ]] ; then
            found=true
            path=$p
            break
          fi
        done

        [[ -z $path ]] &&
          abort_error "No usable command found in [$list] for [$__SYSTEM__]."
    ;;
    *) abort_invalid_arg $1 ;;
    esac
  done

  [[ -x $path ]] ||
      abort_error "File [$path] exists but is not executable for [$__SYSTEM__]."

  [[ -n $vname  ]] && eval $vname=$path
  [[ -n "$opts" ]] && eval ${vname}_opts=\"$opts\"

  return
}


## Filename, pathname, and directory conversion utility.
#/##############################################################################
# \afn file_utility
#
# \aparami <named file> \--pathname
#           Given a string containing a path to a file or a directory, this
#           option returns the path up-to the last directory separator. If
#           there are no directory separators, it returns the empty string.
#           Form: /path/file.ext -> /path.
# \aparami <named file> \--basename
#           Given a string containing a path to a file or a directory, this
#           option returns the part after the last directory separator. If
#           there are no directory separators, it returns the original path.
#           Form: /path/file.ext -> file.ext
# \aparami <named file> \--rootname
#           Given a string containing a path to a file or a directory, this
#           option returns the part after the last directory separator up to
#           the extension. The extension is not returned. If there are no
#           directory separators or extensions, it returns the original path.
#           Form: /path/file.ext -> file
# \aparami <named file> \--extname
#           Given a string containing a path to a file or a directory, this
#           options returns the part after the last dot '.' (the extension)
#           of the part after the last directory separator. If there is no
#           dot separators, it returns the empty string.
#           Form: /path/file.ext -> ext
# \aparami <named file> \--stemname
#           Given a string containing a path to a file or a directory, this
#           option returns everything up to part after the last dot '.'
#           (the extension). If there is none, it returns the original path.
#           Form: /path/file.ext -> /path/file
#
# \aparami <named path> \--addpathsep
#           Given a string containing a path to a file or a directory, this
#           option adds the directory separator to end of the path if,
#           and only if, it does not exists.
# \aparami <named path> \--delrootsep
#           Given a string containing a path to a file or a directory, this
#           option removes the leading directory separator from path if it
#           exists.
#           Form: /path/file.ext -> path/file.ext
# \aparami <named path> \--delallpresep
#           Given a string containing a path to a file or a directory, this
#           option removes all of the leading directory separator from path.
#           Form: ../../path/file.ext -> path/file.ext
#
# \aparami <named path1 path2> \--prefix
#           Given a string containing two paths to files or directories, this
#           option will prefix path1 to path2 with a directory separator.
# \aparami <named path1 path2> \--prefixstem
#           Given a string containing two paths to files or directories, this
#           option will prefix path1 to the stem of path2. Namely, if path2 has
#           an extension, it will be removed.
#
# \aparami <named path> \--mkdir
#           Given a string containing a path to a directory, this option will
#           create the directory for named path.
#
# \returns void
#
# \details
#   Utility for file, path, and directory name manipulation.
#
#   Example:
#   \code{.sh}
#     new_path="$(file_utility --prefix $output_path $new_dir)"
#
#     new_root="$(file_utility --rootname $in_stem)"
#
#     new_stem="$(file_utility --prefix $new_path $new_root)"
#   \endcode
#
###############################################################################/
function file_utility()
{

  while [ $# -gt 0 ] ; do
    case $1 in
    --pathname)
        [[ -z "$2" ]] && break

        # check if has one or more '/'
        if [[ $2 =~ .*/.* ]] ; then
          declare local path=${2%/*}

          # removed last '/' for $path, now check if more remain
          if [[ $path =~ .*/.* ]]; then
            echo $path
          else
            [[ -z ${path} ]] && echo '/'    # must be '/'
            [[ -n ${path} ]] && echo $path  # likely './' or '../''
          fi
        fi

        shift 2
    ;;
    --basename)
        [[ -z "$2" ]] && break

        echo "${2##*/}"
        shift 2
    ;;
    --rootname)
        [[ -z "$2" ]] && break

        declare local basename="$(file_utility --basename $2)"
        echo "${basename%.*}"
        shift 2
    ;;
    --extname)
        [[ -z "$2" ]] && break

        declare local basename="$(file_utility --basename $2)"

        # check for existence of '.' If it does not exists then
        # there is no extension.
        if [[ "${basename##*.}" == "${basename}" ]] ; then
          echo ""
        else
          echo "${basename##*.}"
        fi
        shift 2
    ;;
    --stemname)
        [[ -z "$2" ]] && break

        declare local pathname="$(file_utility --pathname $2)"
        declare local rootname="$(file_utility --rootname $2)"

        pathname="$(file_utility --addpathsep $pathname)"

        echo "$pathname$rootname"
        shift 2
    ;;
    --addpathsep)
        [[ -z "$2" ]] && break

        declare local pathname="$2"

        # add trailing slash to path name if:
        # 1: pathname is not empty (checked above via break)
        # 2: last char != '/'
        [[ ${pathname:${#pathname}-1:1} != '/' ]] &&
            pathname="$pathname/"

        echo "$pathname"
        shift 2
    ;;
    --delrootsep)
        [[ -z "$2" ]] && break

        declare local pathname="$2"

        # remove leading path slash if:
        # 1: pathname is not empty (checked above via break)
        # 2: first char == '/'
        [[ ${pathname:0:1} == '/' ]] &&
            pathname="${pathname:1:${#pathname}-1}"

        echo "$pathname"
        shift 2
    ;;
    --delallpresep)
        [[ -z "$2" ]] && break

        declare local pathname="$2"
        declare -i local p=0

        while [[ ${pathname:$p:1} == '.' ]] || \
              [[ ${pathname:$p:1} == '/' ]]; do
          (( p++ ))
        done

        echo ${pathname:$p:${#pathname}-$p}
        shift 2
    ;;
    --prefix)
        [[ -z "$2" ]] && break
        [[ -z "$3" ]] && { echo "$2" ; break ; }

        declare local prefix="$2"
        declare local pathname="$3"

        prefix="$(file_utility --addpathsep $prefix)"
        pathname="$(file_utility --delallpresep $pathname)"

        echo "$prefix$pathname"
        shift 3
    ;;
    --prefixstem)
        [[ -z "$2" ]] && break
        [[ -z "$3" ]] && { echo "$2" ; break ; }

        declare local prefix="$2"
        declare local pathname="$3"

        prefix="$(file_utility --addpathsep $prefix)"
        pathname="$(file_utility --delallpresep $pathname)"
        pathname="$(file_utility --stemname $pathname)"

        echo "$prefix$pathname"
        shift 3
    ;;
    --mkdir)
        [[ -z "$2" ]] && break

        declare local pathname="$2"

        $sc_mkdir $sc_mkdir_opts $pathname

        shift 2
    ;;
    *) abort_invalid_arg $1 ;;
    esac
  done

  return
} # file_utility()


## Source multiple files into script at once.
#/##############################################################################
# \afn include
#
# \aparami <named path> \--path
#           Specifies path to prepend to subsequently included files.
# \aparami <argv> files
#
# \returns void
#
# \details
#   This function will source multiple files into a script. An include
#   \p path can be specified which will be prepended to all subsequent
#   included \p files. Standard shell expansion, such as brace expansion,
#   may be used.
#
#   Example:
#   \code{.sh}
#     include /patha/file{1,2,3}.bash \
#             -path /pathb file4.bash \
#             -path /pathc file5.bash
#   \endcode
#
#   This example will include five files at once. Namely:
#   <tt>/patha/file1.bash, /patha/file2.bash, /patha/file3.bash,
#   /pathb/file4.bash, and /pathb/file5.bash</tt>.
#
###############################################################################/
function include()
{
  declare local path

  while [ $# -gt 0 ] ; do
    case $1 in
    --path)
        [[ -z "$2" ]] && break

        path="$2"
        shift 2
    ;;
    *)
      declare local file

      [[ -z $path ]] && file="$1"
      [[ -n $path ]] && file="$path/$1"

      [[ -n $__VERBOSE__ ]] && \
      echo "source $file"

      source $file

      shift 1
    ;;
    esac
  done

  return
} # include()


## @}

#==============================================================================
# eof
#==============================================================================
