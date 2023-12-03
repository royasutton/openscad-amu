## OpenSCAD command line options configuration functions.
#/##############################################################################
#
#   \file   openscad_opts.bash
#
#   \author Roy Allen Sutton <royasutton@hotmail.com>.
#   \date   2016-2018
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
#     These functions create options sets stored in hash tables that that
#     can be use with \ref variables \p set_opts_combine for OpenSCAD command
#     line options.
#
#   \todo work on a views_translate() function.
#
#   \ingroup library_core
###############################################################################/

## \ingroup library_core
## @{


## Create a camera view option set table from predefined names.
#/##############################################################################
# \afn views
#
# \aparami <required named string> name
#           Set table name where command line options will be written.
# \aparami <named string> translate
#           String of three integers in the form x,y,z to set the x, y, and z,
#           camera viewport translation.
# \aparami <named integer> distance
#           Integer the viewport camera distance.
# \aparami <named string preset list> views
#           List of preset viewport camera rotations. See bellow for the
#           table of predefined views.
#
# \returns void
#
# \details
#   This function provides an abstracted way to create option set tables
#   for predefined views. When \p translate or \p distance is not specified,
#   the values in the following table will be used for the created
#   option set:
#
#    Parameter | Defaults
#   :---------:|:--------:
#    translate | 0,0,0
#    distance  | 200
#
#   The following table shows the list of predefined views and their viewport
#   rotation values:
#
#    Preset | Alias | Viewport Rotation
#   :------:|:-----:|:-----------------:
#    right  | r     | 90,0,90
#    top    | t     | 0,0,0
#    bottom | bo    | 180,0,0
#    left   | l     | 90,0,270
#    front  | f     | 90,0,0
#    back   | ba    | 90,0,180
#    diag   | d     | 55,0,25
#    all    | a     | all above
#
#   Example:
#   \dontinclude vehicle_document.bash
#   \skipline views
#
#   Arbitrary viewport rotation can be defined using the following syntax:
#   \p cust_id=x,y,z (no spaces within the definition), where \p id sets
#   the arbitrary name and \p x,y,z sets the arbitrary viewport rotation.
#
#    Custom         | Alias      | Viewport Rotation
#   :--------------:|:----------:|:-----------------:
#    cust_id=x,y,z  | c_id=x,y,z | x,y,z
#
#   Example:
#   \code{.sh}
#     views name "views" distance "200" views "all cust_best=10,20,31 c_good=22.99,1.31,0.99"
#   \endcode
#
###############################################################################/
function views()
{
  declare    local name

  # defaults
  declare    local translate="0,0,0"
  declare -i local distance=200

  # openscad standard camera views
  declare local cv_right="90,0,90"
  declare local cv_top="0,0,0"
  declare local cv_bottom="180,0,0"
  declare local cv_left="90,0,270"
  declare local cv_front="90,0,0"
  declare local cv_back="90,0,180"
  declare local cv_diag="55,0,25"

  while [ $# -gt 0 ] ; do
    case ${1,,} in
    name)         name="$2"       ; shift 2  ;;
    translate)    translate="$2"  ; shift 2  ;;
    distance)     distance=$2     ; shift 2  ;;
    views)
        views="$2"

        for v in $views ; do
          case $v in
          right|r)
            table_set "$name" '_right'   "--camera=${translate},${cv_right},${distance}"
          ;;
          top|t)
            table_set "$name" '_top'     "--camera=${translate},${cv_top},${distance}"
          ;;
          bottom|bo)
            table_set "$name" '_bottom'  "--camera=${translate},${cv_bottom},${distance}"
          ;;
          left|l)
            table_set "$name" '_left'    "--camera=${translate},${cv_left},${distance}"
          ;;
          front|f)
            table_set "$name" '_front'   "--camera=${translate},${cv_front},${distance}"
          ;;
          back|ba)
            table_set "$name" '_back'    "--camera=${translate},${cv_back},${distance}"
          ;;
          diagonal|diag|d)
            table_set "$name" '_diag'    "--camera=${translate},${cv_diag},${distance}"
          ;;
          all|a)
            table_set "$name" '_right'   "--camera=${translate},${cv_right},${distance}"
            table_set "$name" '_top'     "--camera=${translate},${cv_top},${distance}"
            table_set "$name" '_bottom'  "--camera=${translate},${cv_bottom},${distance}"
            table_set "$name" '_left'    "--camera=${translate},${cv_left},${distance}"
            table_set "$name" '_front'   "--camera=${translate},${cv_front},${distance}"
            table_set "$name" '_back'    "--camera=${translate},${cv_back},${distance}"
            table_set "$name" '_diag'    "--camera=${translate},${cv_diag},${distance}"
          ;;
          cust_*|c_*)
            # example: cust_name_a_b_c_0=4.5,5.000,6.999
            declare local cv_cust="${v#*_}"         # --> name_a_b_c_0=4.5,5.000,6.999
            declare local cv_cust_id=${cv_cust%%=*} # --> name_a_b_c_0
            declare local cv_cust_vr=${cv_cust#*=}  # --> 4.5,5.000,6.999

            table_set "$name" "_${cv_cust_id}" "--camera=${translate},${cv_cust_vr},${distance}"
          ;;
          *) abort_invalid_arg $v for $1 ;;
          esac
        done

        shift 2
    ;;
    *) abort_invalid_arg $1 ;;
    esac
  done

  return
}


## Create a rotating camera view option set table.
#/##############################################################################
# \afn views_rotate
#
# \aparami <required named string> name
#           Set table name where command line options will be written.
# \aparami <named string> translate
#           String of three integers in the form x,y,z to set the x, y, and z,
#           camera viewport translation.
# \aparami <named integer> distance
#           Integer the viewport camera distance.
# \aparami <named string preset> elevation
#           The camera elevation angle. Can be one of the preset values in
#           the table below. If the preset "angle" is specified, then an
#           integer should immediately follow which sets a custom elevation.
# \aparami <named integer> tilt
#           The camera tilt angle.
# \aparami <named integer> angle0
#           Initial camera rotation angle.
# \aparami <named integer> angle1
#           Ending camera rotation angle.
# \aparami <named integer> delta
#           Increment angle between angle0 and angle1.
# \aparami <named integer> divide
#           Rotation steps between angle0 and angle1. Use this to specify
#           the desired number of steps between angle0 and angle2.
# \aparami <named> number
#           Sequentially number the set key names rather than use the view
#           angles.
# \aparami <named string> prefix
#           Specified a string to prefix the set key name when sequentially
#           numbering. This string must be a single word conforming to
#           BASH variable name rules.
#
# \returns void
#
# \details
#   This function provides an abstracted way to create option set tables
#   for rotating camera views. This functions works well for creating animation
#   image sequences. The functions parameter naming assumes that the X-Y plane
#   establishes the ground and the rotation happens on the Z axis.
#   Default parameter values are shown in the following table:
#
#    Parameter | Defaults
#   :---------:|:--------:
#    translate | 0,0,0
#    distance  | 200
#    elevation | 0 (level)
#    tilt      | 0
#    angle0    | 0
#    angle1    | 360
#    delta     | 1
#    number    | false
#    prefix    | _n
#
#   When specifying \p elevation, the following preset values are
#   defined. If you wish a custom elevation use the \p angle preset followed
#   by an integer elevation value of your choosing.
#
#    Preset | Elevation
#   :------:|:-------------:
#    level  | 90
#    above  | 45
#    below  | 135
#    angle  | custom angle
#
#   Example:
#   \code{.sh}
#     views_rotate name views prefix iu
#     table_dump views
#     table_unset_all views
#
#     views_rotate name views elevation angle 45 angle0 0 angle1 360 divide 30
#     table_dump views
#     table_unset_all views
#
#     views_rotate name views elevation below angle0 0 angle1 360 delta 1 number
#     table_dump views
#     table_unset_all views
#   \endcode
#
# \warning
#   The specified \p delta, or any calculated delta resulting from a
#   specified \p divide can can only assume a positive or negative integer value.
#   Fractional values are not supported.
#
###############################################################################/
function views_rotate()
{
  declare    local name

  # defaults
  declare    local translate="0,0,0"
  declare -i local distance=200
  declare -i local elevation=90
  declare -i local tilt=0
  declare -i local angle0=0
  declare -i local angle1=360
  declare -i local delta=1
  declare -i local number=0
  declare    local prefix="_n"

  # presets
  declare -i local ve_level=90
  declare -i local ve_above=45
  declare -i local ve_below=135

  while [ $# -gt 0 ] ; do
    case ${1,,} in
    name)         name="$2"       ; shift 2 ;;
    translate)    translate="$2"  ; shift 2  ;;
    distance)     distance=$2     ; shift 2  ;;
    tilt)         tilt=$2         ; shift 2  ;;
    angle0)       angle0=$2       ; shift 2 ;;
    angle1)       angle1=$2       ; shift 2 ;;
    delta)        delta=$2        ; shift 2 ;;
    number)       number=1        ; shift 1 ;;
    prefix)       prefix="$2"     ; shift 2  ;;
    divide)
        declare -i local divide=$2
        delta=$(( (angle1 - angle0) / divide ))
        shift 2
    ;;
    elevation)
        case ${2,,} in
        level)  elevation=$ve_level   ;;
        above)  elevation=$ve_above    ;;
        below)  elevation=$ve_below   ;;
        angle)
          [[ -z $3 ]] && abort_invalid_arg "missing argument for" $1 $2
          elevation=$3
          shift 1
        ;;
        *) abort_invalid_arg $2 for $1 ;;
        esac

        shift 2
    ;;
    *) abort_invalid_arg $1 ;;
    esac
  done

  [[ -n $__VERBOSE__ ]] && \
  print_textbox \
  -ct "${FUNCNAME[0]}: elevation=$elevation, angle0=$angle0, angle1=$angle1, delta=$delta" \
  -hc '-' -bt -pl -bb

  declare -i local ic=0

  declare local sk
  declare local kr

  for i in $(${sc_seq} $angle0 $delta $angle1) ; do

    if [[ $number -eq 1 ]] ; then
      printf -v sk "%s%06d" $prefix $ic

      ((ic++))
    else
      printf -v sk "%+04d%+04d%+04d" $elevation $tilt $i

      sk=${sk//-/n}   # set key are stored in environment variables so
      sk=${sk//+/p}   # the +/- characters are not permitted.
    fi

    printf -v kr "%+04d,%+04d,%+04d" $elevation $tilt $i

    table_set "$name" "$sk"   "--camera=${translate},${kr},${distance}"
  done

  return
}


## Create an image size option set table for predefined names and aspect ratios.
#/##############################################################################
# \afn images
#
# \aparami <required named string> name
#           Set table name where command line options will be written.
# \aparami <named string preset list> types
#           List of predefined type names. See the table below for possible
#           names.
# \aparami <named string> aspect
#           String of three integers in the form x:y such as 2:1, 4:3, or
#           16:9 for example, that sets the width to height aspect ratio.
# \aparami <named integer list> wsizes
#           A list of integers that indicate the images width. For each value,
#           the image height will be calculated based on the \c aspect ratio.
# \aparami <named integer list> xsizes
#           Alias to \p wsizes.
# \aparami <named integer list> hsizes
#           A list of integers that indicate the images height. For each value,
#           the image width will be calculated based on the \c aspect ratio.
# \aparami <named integer list> ysizes
#           Alias to \p hsizes.
#
# \returns void
#
# \details
#   This function provides an abstracted way to create option set tables
#   using image aspect ratios and predefined image resolution names. When
#   \p aspect is not specified, the default value 16:9 is used:
#
#    Presets  | Resolution
#   :--------:|:-----------:
#    html     | 320 x 240
#    pdf      | 640 x 480
#    doc      | 640 x 480
#    qqvga    | 160 x 120
#    hqvga    | 240 x 160
#    qvga     | 320 x 240
#    wqvga    | 400 x 240
#    hvga     | 480 x 320
#    vga      | 640 × 480
#    wvga     | 768 x 480
#    svga     | 800 x 600
#    wsvga    | 1024 x 600
#    xga      | 1024 x 768
#    wxga     | 1280 x 768
#    sxga     | 1280 x 1024
#    hd       | 1360 x 768
#    uxga     | 1600 x 1200
#
#   Example:
#   \dontinclude vehicle_document.bash
#   \skipline images
#
###############################################################################/
function images()
{
  declare local name

  # defaults
  declare -i local aspectx=16
  declare -i local aspecty=9

  # standard named-image resolutions
  ir_html="320,240"
  ir_pdf="640,480"
  ir_doc="640,480"

  # common graphics display resolutions
  ir_qqvga="160,120"
  ir_hqvga="240,160"
  ir_qvga="320,240"
  ir_wqvga="400,240"
  ir_hvga="480,320"
  ir_vga="640,480"
  ir_wvga="768,480"
  ir_svga="800,600"
  ir_wsvga="1024,600"
  ir_xga="1024,768"
  ir_wxga="1280,768"
  ir_sxga="1280,1024"
  ir_hd="1360,768"
  ir_uxga="1600,1200"

  while [ $# -gt 0 ] ; do
    case ${1,,} in
    name)
        name="$2"
        shift 2
    ;;
    aspect)
        aspectx="${2%:*}"
        aspecty="${2##*:}"

        shift 2
    ;;
    types)
        types="$2"

        for x in $types ; do
          case $x in
          html|pdf|doc)
            table_set "$name" "_${x}" "--imgsize=\${ir_${x}}"
          ;;
          qqvga|hqvga|qvga|wqvga|hvga|vga|wvga|svga|wsvga|xga|wxga|sxga|hd|uxga)
            table_set "$name" "_${x}" "--imgsize=\${ir_${x}}"
          ;;
          *) abort_invalid_arg $x for $1 ;;
          esac
        done

        shift 2
    ;;
    wsizes|xsizes)
        wsizes="$2"

        for x in $wsizes ; do
          declare -i local y=$((x * aspecty / aspectx ))
          table_set "$name" "_${x}x${y}" "--imgsize=${x},${y}"
        done

        shift 2
    ;;
    hsizes|ysizes)
        hsizes="$2"

        for y in $hsizes ; do
          declare -i local x=$((y * aspectx / aspecty ))
          table_set "$name" "_${x}x${y}" "--imgsize=${x},${y}"
        done

        shift 2
    ;;
    *) abort_invalid_arg $1 ;;
    esac
  done

  return
}


## Create an option set table for arbitrary OpenSCAD -D variable=value pairs.
#/##############################################################################
# \afn defines
#
# \aparami <required named string> name
#           Set table name where command line options will be written.
# \aparami <named string> define
#           String that sets the defined variable name.
# \aparami <named string list> strings
#           List of string values to assign to the variable name.
# \aparami <named integer list> integers
#           List of integer values to assign to the variable name.
# \aparami <named integer first increment last> sequence
#           Generate a sequence of integer values to assign to the variable
#           name. The sequence requires three arguments indicating the start
#           value, the increment, and the stop value. Sequences may be
#           positive or negative, but only integer values are supported.
#
# \returns void
#
# \details
#   This function provides an abstracted way to create option set tables
#   for arbitrary OpenSCAD -D variable=value pairs.
#
#   Example:
#   \dontinclude vehicle_document.bash
#   \skipline defines
#
#   Example:
#   \code{.sh}
#   defines name "joint" define "possition" sequence -100 1 125
#   \endcode
#
###############################################################################/
function defines()
{
  declare local name
  declare local define
  declare local values

  while [ $# -gt 0 ] ; do
    case ${1,,} in
    name)
        name="$2"
        shift 2
    ;;
    define)
        define="$2"
        shift 2
    ;;
    strings)
        values="$2"

        for x in $values ; do
          table_set "$name" "_${x}" '-D $define=\\\"${x}\\\"'
        done

        shift 2
    ;;
    integers)
        values="$2"

        for x in $values ; do
          table_set "$name" "_${x}" '-D $define=${x}'
        done

        shift 2
    ;;
    sequence)
        [[ -z "$4" ]] && abort_invalid_arg "$*" "requires: <first> <increment> <last>"

        declare -i local frst="$2"
        declare -i local incr="$3"
        declare -i local last="$4"

        [[ -n $__VERBOSE__ ]] && \
        print_textbox \
        -ct "${FUNCNAME[0]}: sequence first=$frst, increment=$incr, last=$last" \
        -hc '-' -bt -pl -bb

        declare -i local ic=0
        declare    local sk
        declare    local kr

        declare    local prefix="_s"

        for i in $(${sc_seq} $frst $incr $last) ; do
          printf -v sk "%s%06d" $prefix $ic
          printf -v kr "%+04d" $i

          table_set "$name" "$sk"  "-D $define=${kr}"

          ((ic++))
        done

        shift 4
    ;;
    *) abort_invalid_arg $1 ;;
    esac
  done

  return
}


#==============================================================================
# protect functions
#==============================================================================
readonly -f \
  views \
  views_rotate \
  images \
  defines


## @}

#==============================================================================
# eof
#==============================================================================
