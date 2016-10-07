## Formatted text and textbox printing.
#/##############################################################################
#
#   \file   print_textbox.bash
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
#     Function for formating text within text boxes.
#
#     The following global environment variables can be used to set the
#     width of the output terminal and text box. When they are not set,
#     80 is used as the default for both:
#     \c ${print_textbox_term_width} and \c ${print_textbox_box_width}.
#
#   \ingroup library_support
###############################################################################/

## \ingroup library_support
## @{


## Print multi-line text and text boxes.
#/##############################################################################
# \afn print_textbox
#
# \aparami <named integer> -tw
#           Set the terminal output width.
# \aparami <named integer> -bw
#           Set the text box width. Should be less than or equal to the terminal
#           width.
# \aparami <named integer> -sw
#           Set both the terminal output and text box width.
#
# \aparami <named integer> -bc
#           Set the text box outline count.
#
# \aparami <named character> -hc
#           Set the text box horizontal character.
# \aparami <named character> -vc
#           Set the text box vertical character.
# \aparami <named character> -cc
#           Set the text box corner character.
# \aparami <named character> -fc
#           Set the text box fill character.
# \aparami <named character> -hvc
#           Set the text box horizontal, vertical, and corner characters.
#
# \aparami <named> -jbl
#           Justify text box left.
# \aparami <named> -jbc
#           Justify text box center.
# \aparami <named> -jbr
#           Justify text box right.
#
# \aparami <named string> -lt
#           Set the text box left text string.
# \aparami <named string> -ct
#           Set the text box center text string.
# \aparami <named string> -rt
#           Set the text box right text string.
# \aparami <named> -clr
#           Clear all text strings.
#
# \aparami <named string> -ifs
#           Set the temporary input field separator. It will be restored
#           before the function returns.
#
# \aparami <named string> -dcc
#           Default color control string. This string is used after
#           each color control sequence is completed.
#
# \aparami <named string> -hcc
#           Set the horizontal character color control string.
# \aparami <named string> -ccc
#           Set the corner character color control string.
# \aparami <named string> -vcc
#           Set the vertical character color control string.
# \aparami <named string> -fcc
#           Set the fill character color control string.
#
# \aparami <named string> -ltc
#           Set the left text color control string.
# \aparami <named string> -ctc
#           Set the center text color control string.
# \aparami <named string> -rtc
#           Set the right text color control string.
#
# \aparami <named> -sn
#           Prepend script name to each line.
# \aparami <named> -sns
#           Prepend space equal to the number of characters in the script
#           name to each line.
#
# \aparami <named string> -ljml
#           Output left-justified multi-line text.
# \aparami <named string> -cjml
#           Output center-justified multi-line text.
# \aparami <named string> -rjml
#           Output right-justified multi-line text.
#
# \aparami <named string> -ljfml
#           Output left-justified formated multi-line text.
# \aparami <named string> -cjfml
#           Output center-justified formated multi-line text.
# \aparami <named string> -rjfml
#           Output right-justified formated multi-line text.
#
# \aparami <named string> -ifs3cml
#           Output multi-line IFS-formated text.
#
# \aparami <named> -bt
#           Output text box top.
# \aparami <named> -bb
#           Output text box bottom.
# \aparami <named> -bl
#           Output text with blank text line.
# \aparami <named> -pl
#           Output text box with text that has been set using
#           \p -lt, \p -ct, and/or \p -rt.
# \aparami <named> -hr
#           Output horizontal rule line.
#
# \returns void
#
# \details
#   This functions provides a flexible way to output formated text and text
#   boxes. The default output configuration is shown in the following table.
#
#    Paramerter           | Default
#   :--------------------:|:-------:
#    terminal width       | 80
#    box width            | 80
#    box outline count    | 1
#    horizontal character | '='
#    vertical character   | '\|'
#    corner character     | '+'
#    fill character       | ' '
#
###############################################################################/
function print_textbox()
{
    #==========================================================================
    # offset for each new line
    #==========================================================================
    function do_offset() {
        declare -i local cnt

        [ $jb == 'l' ] && let cnt=0
        [ $jb == 'c' ] && let cnt=(tw-bw)/2
        [ $jb == 'r' ] && let cnt=(tw-bw)

        if [ -z "$pn" ] ; then
            print_m -j -n -r $cnt ' '
        else
            (( $pn == 1 )) && print_m    "" -n -r $cnt ' '
            (( $pn == 2 )) && print_m -J "" -n -r $cnt ' '
        fi

        return
    }

    #==========================================================================
    # box top and bottom
    #==========================================================================
    function do_box() {
        function do_box_line() {
            declare -i local vcnt=$1
            declare -i local hcnt=$(( bw - ( ($vcnt+1)*2 ) ))
            [ -n "$pn" ] && let hcnt-=${#script_base_name}+2

            do_offset

            print_m -j -e "$vcc" -j -r $vcnt "$vc" -j "$dcc" \
                    -j    "$ccc" -j          "$cc" -j "$dcc" \
                    -j    "$hcc" -j -r $hcnt "$hc" -j "$dcc" \
                    -j    "$ccc" -j          "$cc" -j "$dcc" \
                    -j    "$vcc" -j -r $vcnt "$vc" -j "$dcc"

            return
        }

        declare    local loc=${1:0:1}
        declare -i local cnt

        if [ "${loc,,}" == "t" ] ; then
            # box top
            for (( cnt=0; cnt < $bc; cnt++ )) ; do
                do_box_line $cnt
            done
        else
            # box bottom
            for (( cnt=$(($bc-1)); cnt >= 0; cnt-- )) ; do
                do_box_line $cnt
            done
        fi

        return
    }

    #==========================================================================
    # a box text line
    #==========================================================================
    function do_line() {
        declare local lt="$1"
        declare local ct="$2"
        declare local rt="$3"

        declare -i local hcnt=$bw
        [ -n "$pn" ] && let hcnt-=${#script_base_name}+2

        declare -i local lts cts rts
        lts=${#lt}
        cts=${#ct}
        rts=${#rt}

        declare -i local lc rc
        if [ $cts -ne 0 ] ; then        # center text exists
            let lc=hcnt-cts-bc*2
            let lc=(lc/2)
            let rc=hcnt-cts-bc*2-lc

            let lc-=lts                 # reduce by left text size
            let rc-=rts                 # reduce by right text size
        else
            let lc=hcnt-lts-rts-bc*2    # no center text
            let rc=0                    # only one spacing needed
        fi

        do_offset

        print_m -j -e "$vcc" -j -r $bc "$vc" -j "$dcc" \
                -j    "$ltc" -j        "$lt" -j "$dcc" \
                -j    "$fcc" -j -r $lc "$fc" -j "$dcc" \
                -j    "$ctc" -j        "$ct" -j "$dcc" \
                -j    "$fcc" -j -r $rc "$fc" -j "$dcc" \
                -j    "$rtc" -j        "$rt" -j "$dcc" \
                -j    "$vcc" -j -r $bc "$vc" -j "$dcc"

        return
    }

    #==========================================================================
    # a box horizontal box interior line
    #==========================================================================
    function do_rule() {
        declare -i local hcnt=$bw
        declare -i local vcnt=0 \
                         ccnt=0
        [ -n "$pn" ] && let hcnt-=${#script_base_name}+2

        let hcnt-=bc*2
        [ $bc -ge 2 ] && let vcnt=bc-1
        [ $bc -ge 1 ] && let ccnt=1

        do_offset

        print_m -j -e "$vcc" -j -r $vcnt "$vc" -j "$dcc" \
                -j    "$ccc" -j -r $ccnt "$cc" -j "$dcc" \
                -j    "$hcc" -j -r $hcnt "$hc" -j "$dcc" \
                -j    "$ccc" -j -r $ccnt "$cc" -j "$dcc" \
                -j    "$vcc" -j -r $vcnt "$vc" -j "$dcc"

        return
    }

    #==========================================================================
    # a box text line placed in a specific justification column
    #==========================================================================
    function do_line_tjp() {
        declare local  tjp=$1
        declare local line="$2"

        [ "$tjp" == "l" ] && do_line "$line" "$ct"   "$rt"
        [ "$tjp" == "c" ] && do_line "$lt"   "$line" "$rt"
        [ "$tjp" == "r" ] && do_line "$lt"   "$ct"   "$line"

        return
    }

    #==========================================================================
    # split a text string across multiple lines as needed
    #==========================================================================
    function do_mline() {
        declare local tjp=$1
        declare local mti="$2"

        declare -i local lmax=$(( $bw - $bc*2 ))

        # reduce max line as needed based on configuration
        [ -n "$pn" ]      && let lmax-=${#script_base_name}+2
        [ "$tjp" == "c" ] && let lmax=$(( lmax - ${#lt}*2 - ${#rt}*2 ))
        if [ "$tjp" == "l" ] ; then
           [ ${#ct} -ne 0 ] && let lmax=$(( lmax / 2 - ${#ct} - ${#rt} ))
           [ ${#ct} -eq 0 ] && let lmax=$(( lmax     - ${#ct} - ${#rt} ))
        fi
        if [ "$tjp" == "r" ] ; then
           [ ${#ct} -ne 0 ] && let lmax=$(( lmax / 2 - ${#lt} - ${#ct} ))
           [ ${#ct} -eq 0 ] && let lmax=$(( lmax     - ${#lt} - ${#ct} ))
        fi

        declare -i local ccnt=${#mti}
        declare -i local idx=0
        declare    local mto

        # remove all but posix print characters
        for (( idx=0; idx<ccnt; idx++ )) ; do
            [[ ${mti:$idx:1} == [[:print:]] ]] && mto+=${mti:$idx:1}
        done

        declare local line=" "
        # divide text into lines
        idx=0; ccnt=${#mto}
        while (( idx < ccnt )) ; do
            declare    local word=""
            declare -i local wcnt
            declare -i local cont=1

            # get next word
            for (( wcnt=idx; wcnt < ccnt && cont; wcnt++ )) ; do
                [[ ${mto:$wcnt:1} == [[:space:]] ]] && cont=0
                word+=${mto:$wcnt:1}
            done

            # will next word fit on line
            if (( (${#line} + ${#word}) < $lmax )) ; then
                line+=$word
            else
                # output current line
                do_line_tjp $tjp "$line"

                # start next line with word
                line=" $word"
            fi

            # advance index to where last word ended
            let idx=wcnt
        done

        # output last line if it exists
        [ -n "$line" ] && do_line_tjp $tjp "$line"

        return
    }

    #==========================================================================
    # output a pre-formated multi-line text string
    #==========================================================================
    function do_fmline() {
        declare local tjp=$1
        declare local mti="$2"

        declare -i local lmax=$(( $bw - $bc*2 ))
        [ -n "$pn" ] && let lmax-=${#script_base_name}+2

        declare local line
        # divide text into lines
        echo "$mti" | while read line ; do
            do_line_tjp $tjp "$line"
        done

        return
    }

    #==========================================================================
    # parse a multi-line text string based on input feild seperator (IFS)
    #==========================================================================
    function do_ifsmline() {
        declare local mti="$1"

        declare -i local lmax=$(( $bw - $bc*2 ))
        [ -n "$pn" ] && let lmax-=${#script_base_name}+2

        declare local lt ct rt
        # divide text into lines
        echo "$mti" | while read lt ct rt ; do
            do_line "$lt" "$ct" "$rt"
        done

        return
    }

    #==========================================================================
    #
    # print_textbox
    #
    #==========================================================================

    declare -i local tw=${print_textbox_term_width:-80}
    declare -i local bw=${print_textbox_box_width:-80}

    declare -i local bc=1

    declare local    hc='='
    declare local    vc='|'
    declare local    cc='+'
    declare local    fc=' '

    declare local    pn

    declare local    jb='c'

    declare local    lt ct rt
    declare local    dcc
    declare local    hcc ccc vcc fcc
    declare local    ltc ctc rtc

    declare local    prior_IFS="$IFS"
    declare local    IFS_changed

    while [ $# -gt 0 ] ; do
        case $1 in
        -tw) tw=$2     ; shift 1 ;; # terminal width
        -bw) bw=$2     ; shift 1 ;; # box width
        -sw) tw=$2
             bw=$2     ; shift 1 ;; # terminal and box width

        -bc) bc=$2     ; shift 1 ;; # box outline count

        -hc) hc=$2     ; shift 1 ;; # box horizontal character
        -vc) vc=$2     ; shift 1 ;; # box vertical character
        -cc) cc=$2     ; shift 1 ;; # box corner character
        -fc) fc=$2     ; shift 1 ;; # box fill character

        -hvc)                       # set hc, vc, & cc together
            hc=$2
            vc=$2
            cc=$2      ; shift 1 ;;

        -jbl) jb='l'             ;; # justify box left
        -jbc) jb='c'             ;; # justify box center
        -jbr) jb='r'             ;; # justify box right

        -lt) lt="$2"   ; shift 1 ;; # left text
        -ct) ct="$2"   ; shift 1 ;; # center text
        -rt) rt="$2"   ; shift 1 ;; # right text

        -clr)                       # clear text strings
            lt=""
            ct=""
            rt=""
        ;;

        -ifs)                       # set temp input feild seperator
            IFS="$2"
            IFS_changed=true
            shift 1
        ;;

        -dcc) dcc="$2" ; shift 1 ;; # reset/default color ctrl string

        -hcc) hcc="$2" ; shift 1 ;; # horizontal character color ctrl string
        -ccc) ccc="$2" ; shift 1 ;; # corner character color ctrl string
        -vcc) vcc="$2" ; shift 1 ;; # vertical character color ctrl string
        -fcc) fcc="$2" ; shift 1 ;; # fill character color ctrl string

        -ltc) ltc="$2" ; shift 1 ;; # left text color ctrl string
        -ctc) ctc="$2" ; shift 1 ;; # center text color ctrl string
        -rtc) rtc="$2" ; shift 1 ;; # right text color ctrl string

        -sn)  pn=1               ;; # prepend script name to each line
        -sns) pn=2               ;; # prepend space of script name to each line

        -ljml|-cjml|-rjml)          # output multi-line text
            do_mline "${1:1:1}" "$2"
            shift 1
        ;;
        -ljfml|-cjfml|-rjfml)       # output multi-line pre-formated text
            do_fmline "${1:1:1}" "$2"
            shift 1
        ;;
        -ifs3cml)                   # output 3 column multi-line
            do_ifsmline "$2"        # IFS-formated text
            shift 1
        ;;

        -bt) do_box 'top'               ;;
        -bb) do_box 'bottom'            ;;
        -bl) do_line "" "" ""           ;;
        -pl) do_line "$lt" "$ct" "$rt"  ;;
        -hr) do_rule                    ;;

        *) abort_invalid_arg $1         ;;
        esac

        shift 1
    done

    # restore prior input feild seperator if temp was set
    [ -n "${IFS_changed}" ] && IFS="$prior_IFS"

    return
}


#==============================================================================
# protect functions
#==============================================================================
readonly -f \
  print_textbox


## @}

#==============================================================================
# eof
#==============================================================================
