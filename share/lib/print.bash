## Uniform message printing functions.
#/##############################################################################
#
#   \file   print.bash
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
#     These functions provide uniform methods for outputting text messages.
#
#     The environment variable \c ${script_base_name} should be set prior to
#     the name of the script or any desired text that identifies the source
#     of the output messages.
#
#     When using the functions that produce output based on a minimum verbosity
#     level, an environment variable must be set that contains the integer value
#     for the current output verbosity. To communicate this variable
#     to these functions assign its name to \c ${print_verb_level_var_name}:
#
#     example:
#     \code{.sh}
#       declare -i my_verbosity=10
#       declare    print_verb_level_var_name=${my_verbosity}
#     \endcode
#
#     When \c ${print_verb_level_var_name} does not exist, these functions
#     check \c ${verb_level} by default.
#
#     example use:
#     \code{.sh}
#       print_m -j -n -e "\n*\n* word1\n*" -E "word2" word3 \$\{var\} -e "\n"
#
#       declare -i verb_level=10
#
#       check_vl 15 && echo "this will not be printed"
#       check_vl 15 || echo "this will be printed"
#
#       print_m_vl 5 "this will" be printed
#       print_debug_vl 20 this debug message will not be printed
#     \endcode
#
#   \ingroup library_support
###############################################################################/

## \ingroup library_support
## @{


## Output text message.
#/##############################################################################
# \afn print_m
#
# \aparami <keyword> -j         When this is the first parameter, don't append
#                               the script name to start of each line. Otherwise,
#                               joins the subsequent word by omitting the next
#                               word separator.
# \aparami <keyword> -J         When the first parameter, skip the number of
#                               characters in script name at start of the line.
# \aparami <keyword> -n         Do not output new-line at end of the output.
# \aparami <keyword> -e         Enables \c echo escape interpretation for
#                               subsequent words.
# \aparami <keyword> -E         Disables \c echo escape interpretation for
#                               subsequent words.
# \aparami <keyword integer> -r Repeat count for next word. The next word will
#                               be outputted this number of times.
# \aparami <keyword string> -s  Set the word separator string. This string will
#                               be used to separate all subsequent words. By
#                               default the space character is used.
# \aparami <string> text        Text words to copy to output.
#
# \returns void
#
# \details
#   Output each argument one by one. Each \p keyword encountered
#   effect the format and/or behavior for all subsequent text words.
#   Unless otherwise configured, the script's name is appended to start
#   of each line of output.
#
###############################################################################/
function print_m()
{
    declare    local nl
    declare    local es
    declare -i local rn=1
    declare    local ws=' '
    declare    local ns="${ws}"

    # first argument processing
    case $1 in
    -j)                                                             ;;
    -J) print_m -j -n -r ${#script_base_name} ' ' -j ' ' ; shift 1  ;;
     *) echo -n ${script_base_name}:                                ;;
    esac


    while [ $# -gt 0 ] ; do
        case $1 in
        -n) nl=true           ;; # do not output new-line at end
        -e) es=true           ;; # enable echo escape interpretation
        -E) es=''             ;; # disable echo escape interpretation
        -j) ns=''             ;; # joint the subsequent word
        -r) rn=$2   ; shift 1 ;; # repeat count for next word
        -s) ws="$2" ; shift 1 ;; # set the word seperator string
         *)
            while ((rn > 0))
            do
                if [ -z "$es" ] ; then
                    echo -n -E "${ns}${1}"
                else
                    echo -n -e "${ns}${1}"
                fi

                ns=''
                let rn--
            done
            ns="${ws}"
            rn=1
        ;;
        esac

        shift 1
    done

    [ -z "$nl" ] && echo

    return
}


## Output text debug message.
#/##############################################################################
# \afn print_debug
#
# \details
#   Functions like \ref print_m except that each line is begun with the
#   string <tt>${script_base_name}.${fname}():</tt> where \c ${fname} is
#   the name of the current function.
#
# \copydoc print_m
#
###############################################################################/
function print_debug()
{
    declare local fname=${FUNCNAME[1]}

    print_m -j "${script_base_name}.${fname}():" $*

    return
}


## Check for minimum verbosity level.
#/##############################################################################
# \afn check_vl
#
# \aparami <integer> value  the minimum integer verbosity level condition.
#
# \details
#   Compares the verbosity variable value assigned in
#   \c ${print_verb_level_var_name} to the function's argument \p value.
#
#   When \c ${print_verb_level_var_name} does not exist, use \c ${verb_level}.
#
# \retval 0 on when \p value is greater than or equal to the verbosity level.
# \retval 1 otherwise.
#
###############################################################################/
function check_vl()
{
    [[ ${print_verb_level_var_name:-$verb_level} -ge $1 ]] && return 0

    return 1
}


## Output text message conditioned on a minimum verbosity level.
#/##############################################################################
# \afn print_m_vl
#
# \aparami <integer> value  the minimum integer verbosity level condition.
#
# \details
#   Functions like \ref print_m except that output is generated only when
#   \p value is greater than or equal to the verbosity variable value
#   assigned in \c ${print_verb_level_var_name}.
#   The first arguments must be \p value. All remaining arguments are
#   processed by \ref print_m.
#
# \copydoc print_m
#
###############################################################################/
function print_m_vl()
{
    declare local level=$1
    shift 1

    check_vl $level && print_m $*

    return
}


## Output text debug message conditioned on a minimum verbosity level.
#/##############################################################################
# \afn print_debug_vl
#
# \aparami <integer> value  the minimum integer verbosity level condition.
#
# \details
#   Functions like \ref print_debug except that output is generated only when
#   \p value is greater than or equal to the verbosity variable value
#   assigned in \c ${print_verb_level_var_name}.
#   The first arguments must be \p value. All remaining arguments are
#   processed by \ref print_m.
#
# \copydoc print_debug
#
###############################################################################/
function print_debug_vl()
{
    declare local level=$1
    shift 1

    declare local fname=${FUNCNAME[1]}

    check_vl $level && print_m -j "${script_base_name}.${fname}():" $*

    return
}


## Print message to output and wait on input for [enter key] to be pressed.
#/##############################################################################
# \afn print_pause
#
# \details
#   Functions like \ref print_m except that after the message is outputted,
#   The function waits for the [enter]/[return] key to be pressed. If no
#   message argument is provided, the default is:
#   <em>"press [enter] to continue :"</em>.
#
# \copydoc print_m
#
###############################################################################/
function print_pause()
{
    if [ $# -eq 0 ] ; then
        print_m -n "press [enter] to continue :"
    else
        print_m $*
    fi

    declare local choice
    read choice

    return
}


## Output message and wait on input for a character in the character list to be pressed.
#/##############################################################################
# \afn print_select_char
#
# \aparami <keyword string> -m  Message to be written to output.
# \aparami <keyword string> -l  List of valid characters. The characters
#                               should not be separated by spaces.
# \aparami <keyword>        -i  Ignore case of inputted characters;
#                               input is converted to lower case.
# \aparami <keyword>        -j  Do not output the script name prior to the
#                               output message.
# \aparamo <keyword string> -v  Name of the environment variable to write the
#                               valid character that was pressed.
#
# \returns void
#
# \details
#   Write the message to output and wait on input for one of the characters
#   in the list of valid characters to be pressed. Each time an invalid
#   character is pressed, the message is repeated to output. The pressed
#   input can optionally be converted to lower case for case insensitivity.
#   When one of the valid charactes is detected, the function returns. If
#   an environment variable is specified using \p -v, then the read character
#   will be stored to the named variable.
#
#   example:
#   \code{.sh}
#     print_select_char -m "press [a, b, c, A, B, or C]:" -i -l "abc" -v res
#
#     print_m $res
#   \endcode
#
###############################################################################/
function print_select_char()
{
    declare local message
    declare local ch_list
    declare local icase
    declare local join
    declare local __rv_name

    # parse function arguments
    while [ $# -gt 0 ] ; do
        case $1 in
        -m) message="$2"    ; shift 1   ;;
        -l) ch_list=$2      ; shift 1   ;;
        -i) icase=true                  ;;
        -j) join="-j"                   ;;
        -v) __rv_name=$2    ; shift 1   ;;
        *) abort_invalid_arg $1         ;;
        esac

        shift 1
    done

    # if ch_list is empty, return
    [ -z "$ch_list" ] && return

    # if ignoring case
    [ -n "$icase" ] && ch_list=${ch_list,,}

    declare local got_ch
    while [ -z "$got_ch" ]
    do
        declare local choice

        print_m $join -n "$message"
        read -n 1 choice

        # if ignoring case
        [ -n "$icase" ] && choice=${choice,,}

        # check to see if choice is in $ch_list
        declare -i local idx cnt=${#ch_list}
        for (( idx=0; idx < cnt; idx++ )) ; do
            if [ "$choice" == ${ch_list:$idx:1} ] ; then
                got_ch=$choice
                break
            fi
        done

        print_m -j
    done

    declare local __rv_value=$got_ch

    [ -n "$__rv_name" ] && eval "$__rv_name=\"${__rv_value}\""

    return
}


#==============================================================================
# protect functions
#==============================================================================
readonly -f \
  print_m \
  print_debug \
  check_vl \
  print_m_vl \
  print_debug_vl \
  print_pause \
  print_select_char


## @}

#==============================================================================
# eof
#==============================================================================
