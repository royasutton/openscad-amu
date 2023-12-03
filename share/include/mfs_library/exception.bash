## Exceptions, abort on error, and stack dump, functions.
#/##############################################################################
#
#   \file   exception.bash
#
#   \author Roy Allen Sutton <royasutton@hotmail.com>.
#   \date   2016-2017
#
#   \copyright
#     This file is part of OpenSCAD AutoMake Utilities ([openscad-amu]
#     (https://royasutton.github.io/openscad-amu)).
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
#     These functions provide uniform methods for reporting scripts
#     errors. They report the file, function, and line number as well
#     as an error message. By default, the full file name path is
#     reported. However, if the environment variable
#     \c ${exception_call_stack_basename} is non-zero, only the file
#     basename will be reported.
#
#   \ingroup library_support
###############################################################################/

## \ingroup library_support
## @{


## Dump scripts current function call stack.
#/##############################################################################
# \afn dump_call_stack
#
# \aparami <arg1 integer> index   Function call stack index. Indicates the call
#                                 stack level at which reporting will begin.
#
# \returns void
#
# \details
#   Output the file, function, and line number  pointer for each
#   function in the call stack. if the environment variable
#   \c ${exception_call_stack_basename} is non-zero, only the file
#   basename will be reported.
#
###############################################################################/
function dump_call_stack()
{
    declare    local start=${1-1}

    print_m "*** [call-stack] ***"

    declare -i local idx=${start}
    declare -i local cnt=${#FUNCNAME[@]}

    while (( idx<cnt )) ; do
        print_m -J -n "[" -j $(( $cnt - $idx - 1 )) -j "]--> "

        if [ $idx -eq $start ] ; then
            print_m -j -n "in function"
        else
            print_m -j -n "called from"
        fi

        declare local srcf=${BASH_SOURCE[$idx]}
        declare local name=${FUNCNAME[$idx]}
        declare local line=${BASH_LINENO[ $(($idx-1)) ]}

        # basename: remove leading path if set
        if [ -n "$exception_call_stack_basename" ] ; then
            srcf=${srcf##*/}
            print_m -j " [${name}()] at line [${line}] in [${srcf}]"
        else
            print_m -j " [${name}()] at line [${line}]"
            print_m -J -r 6 ' ' "file [${srcf}]"
        fi

        let idx++
    done

    return
}


## Output a multi-line error message and exit.
#/##############################################################################
# \afn abort_error
#
# \aparami <argv> messages  Error message to report. Each quoted message
#                           string will be placed on a separate line.
#
# \retval 1
#
# \details
#   Output an error notice followed by the list of argument messages one
#   line at a time and dump the function call stack.
#
###############################################################################/
function abort_error()
{
    print_m "*** [error] ***"

    while [ $# -gt 0 ] ; do
        [ -n "$1" ] && print_m -J $1

        shift 1
    done

    dump_call_stack 2
    print_m "aborting..."

    exit 1
}


## Output error message with appended list of words and exit.
#/##############################################################################
# \afn abort_message_list
#
# \aparami <named string> \--abort-message-base
#          Set base error message to output. "unspecified exception" is used
#          by default.
# \aparami <named integer> \--abort-stack-start
#          Function call stack index. Indicates the call stack level at which
#          reporting will begin.
# \aparami <argv> messages  Error words to print after base message. Each
#          quoted message string will be grouped together in brackets.
#
# \retval 1
#
# \details
#   Output a base error message followed by a list of appended messages
#   grouped into related words [inside of brackets], dump the function
#   call stack and exit.
#
###############################################################################/
function abort_message_list()
{
    declare local message="unspecified exception"
    declare local stack=2
    declare local list

    print_m "*** [error] ***"

    declare -i local cnt=0

    while [ $# -gt 0 ] ; do
        case $1 in
        --abort-message-base)   message="$2"    ; shift 1   ;;
        --abort-stack-start)    stack=$2        ; shift 1   ;;
         *) list+=" [$1]"                       ; let cnt++ ;;
        esac
        shift 1
    done

    [ $cnt -ge 2 ] && message+="s"

    print_m -J $message $list
    dump_call_stack $stack
    print_m "aborting..."

    exit 1
}


## Output invalid argument error message and exit.
#/##############################################################################
# \afn abort_invalid_arg
#
# \aparami <argv> messages  List of offending argument words.
#
# \retval 1
#
# \details
#   Output an "invalid argument" error message followed by a list of
#   offending arguments, dump the function call stack, and exit.
#
###############################################################################/
function abort_invalid_arg()
{
    abort_message_list \
        --abort-message-base "invalid argument" \
        --abort-stack-start 3 "$@"
}


## Output undefined required variable error message and exit.
#/##############################################################################
# \afn abort_not_defined
#
# \aparami <argv> messages  List of missing variables words.
#
# \retval 1
#
# \details
#   Output an "undefined required variable" error message followed by a list of
#   missing variables, dump the function call stack, and exit.
#
###############################################################################/
function abort_not_defined()
{
    abort_message_list \
        --abort-message-base "undefined required variable" \
        --abort-stack-start 3 "$@"
}


## Abort script if a variable is not defined.
#/##############################################################################
# \afn abort_if_not_defined
#
# \aparami <arg1 string> value  String value. Should be quoted to capture when
#                               the value is equal to the \c null string.
# \aparami <argv> messages      Variable description messages.
#
# \retval 0 when value is not null
# \retval 1 when value is null
#
# \details
#   Check that the string \p value is not \c null. If it is, output an
#   "undefined required variable" error message followed by a list of
#   missing variables, dump the function call stack, and exit.
#
###############################################################################/
function abort_if_not_defined()
{
    [ -n "$1" ] && return 0

    shift 1
    abort_message_list \
        --abort-message-base "undefined required variable" \
        --abort-stack-start 3 "$@"
}


#==============================================================================
# protect functions
#==============================================================================
readonly -f \
  dump_call_stack \
  abort_error \
  abort_message_list \
  abort_invalid_arg \
  abort_not_defined \
  abort_if_not_defined


## @}

#==============================================================================
# eof
#==============================================================================
