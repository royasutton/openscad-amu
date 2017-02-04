## Core make makefile functions.
#/##############################################################################
#
#   \file   make_makefile.bash
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
#     Core make makefile script functions. Many of these functions should
#     not be used directlyin a script and, as such, are marked \c private.
#
#   \ingroup library_core
###############################################################################/

#==============================================================================
# script global variables
#==============================================================================

declare local makefile_name   # makefile file name
declare local source_name     # source file name
declare local output_path     # output base path
declare local output_ext      # output target extension
declare local opts_common     # common OpenSCAD command line options
declare local opts_sets       # OpenSCAD command line option sets to combine
declare local convert_exts    # list of extension for target conversion
declare local convert_opts    # conversion command line options
declare local log_begin_rec   # log file record begin text string
declare local log_end_rec     # log file record end text string

declare -a local depend_a     # array of files on which the target depends

declare -i local mnum=0       # makefile counter
declare -i local tnum=0       # target counter
declare -i local cnum=0       # conversion counter

declare -i local vcmd=0       # processed variable commands count
declare -i local gcmd=0       # processed generate commands count
declare -i local parg=0       # processed arguments count
declare -i local iarg=0       # argument index advance counter
declare -i local argc=0       # total invoked arguments


## Output a title block at the beginning of a makefile.
#/##############################################################################
# \afn makefile_titleblock_begin
#
# \aparami <arg1 string> title  Title block header text string.
#
# \returns void
#
# \details
#   Output a comment block at the start of a makefile with basic run-time
#   variables about the system, host, path, etc. The title is centered and
#   placed within the text block.
#
# \private
###############################################################################/
function makefile_titleblock_begin()
{
  declare local title="$1"

  print_m -j
  print_textbox -bc 2 -hvc '#' -bt \
    -ct "THIS FILE HAS BEEN GENERATED, CHANGES WILL LIKELY BE OVERRIDDEN." -pl \
    -hc '-' -hr -lt " begin" -rt "begin " -ct "makefile" -pl -hr  \
    -clr -ct "$title" -pl \
    -hc '#' -bb

  print_textbox -bc 1 -hvc '#' \
    -lt " makefile created:" -rt "$__RUN_TIME_LONG__ " -pl \
    -lt " on system:" -rt "$__SYSTEM__ " -pl \
    -lt " machine:" -rt "$__MACHINE__ " -pl \
    -lt " host:" -rt "$__RUN_HOST__ " -pl \
    -bb

  return
} # makefile_titleblock_begin()


## Output a title block at the end of a makefile.
#/##############################################################################
# \afn makefile_titleblock_end
#
# \aparami <arg1 string> title  Title block header text string.
#
# \returns void
#
# \details
#   Output a comment block at the end of a makefile with a title that is
#   centered within the text block.
#
# \private
###############################################################################/
function makefile_titleblock_end()
{
  declare local title="$1"

  print_m -j
  print_textbox -bc 2 -hvc '#' -bt \
    -hc '-' -hr -lt " end" -rt "end " -ct "makefile" -pl -hr  \
    -clr -ct "$title" -pl -hc '#' -bb

  return
} # makefile_titleblock_end()


## Output the list of global variables with descriptions and current values.
#/##############################################################################
# \afn show_variable_summary
#
# \aparami <named> \--mf_format Use format suitable for inclusion in makefiles.
#
# \returns void
#
# \details
#   Show the current values for all global variables configured with one of
#   the \ref variables commands. Optionally, each line is begun with a comment
#   character so that the output is suitable to inclusion within a makefile.
#
# \private
#
###############################################################################/
function show_variable_summary()
{
  declare local format
  declare local mfs

  while [ $# -gt 0 ] ; do
    case $1 in
    --mf_format) format="makefile" ; mfs="-hvc #" ; shift 1 ;;
    *) abort_invalid_arg $1                                   ;;
    esac
  done

  declare local output_stempath="$(file_utility --prefix $output_path $output_ext)"

  print_m -j
  print_textbox $mfs -bt \
      -ct "Variables Summary" -pl -hr -clr \
      -rt "[source name] " -lt " $source_name" -pl \
      -rt "[target ext] " -lt " $output_ext" -pl \
      -rt "[output path] " -lt " $output_path" -pl \
      -rt "[makefile name] " -lt " $makefile_name" -pl \
      -rt "[output stempath] " -lt " $output_stempath" -pl \
      -rt "[combination option sets] " -lt " $opts_sets" -pl

  if [[ ${#depend_a[@]} -gt 0 ]] ; then
    print_textbox $mfs -ct "[target depends]" -fc '-' -pl
    for tdi in ${!depend_a[@]} ; do
      print_textbox $mfs -lt " ${depend_a[$tdi]}" -pl
    done
  fi

  if [[ -n $convert_exts ]] ; then
    print_textbox $mfs -ct "[image convertion targets]" -fc '-' -pl
    for co in $convert_exts ; do
      print_textbox $mfs -lt " ${output_ext}2${co}" \
        -ct "--> ${co}" -rt "[output path] " -pl
    done
  fi

  if [[ -n $convert_opts ]] ; then
    print_textbox $mfs -ct "[image convertion options]" -fc '-' -pl
    for co in $convert_opts ; do
      print_textbox $mfs -lt " ${co}" -pl
    done
  fi

  if [[ -n $log_begin_rec ]]; then
    print_textbox $mfs -ct "[log segregation filter]" -fc '-' -pl
    print_textbox $mfs -lt " ${log_begin_rec}" -rt "[start new record text] " -pl
    [[ -n $log_end_rec ]] && \
    print_textbox $mfs -lt " ${log_end_rec}" -rt "[end record text] " -pl
  fi

  print_textbox $mfs -bb

  if [[ -n $opts_common ]] ; then
    print_m -j
    print_textbox $mfs -bt -ct "Common Options" -pl -hr
    for co in $opts_common ; do
      print_textbox $mfs -lt " $co" -pl
    done
    print_textbox $mfs -bb
  fi


  return
} # show_variable_summary()


## Output all of the option set tables set via set_opts_combine.
#/##############################################################################
# \afn show_opts_set_tables
#
# \aparami <named> \--mf_format Use format suitable for inclusion in makefiles.
#
# \returns void
#
# \details
#   Each table that has been configured and is named by \ref variables
#   \p set_opts_combine  will be output in a tabular format. Optionally,
#   each line is begun with a comment character so that the output is
#   suitable to inclusion within a makefile.
#
# \private
###############################################################################/
function show_opts_set_tables()
{
  declare local format
  declare local mfs

  while [ $# -gt 0 ] ; do
    case $1 in
    --mf_format) format="makefile" ; mfs="-hvc #" ; shift 1 ;;
    *) abort_invalid_arg $1                                   ;;
    esac
  done

  declare -a local opts_cnt_a=($opts_sets)
  declare local tcmtstr="${#opts_cnt_a[@]} set(s)"

  print_m -j
  print_textbox -lt " $tcmtstr" -ct "Combination Options" \
                -rt "$tcmtstr " $mfs -bt -pl

  declare local octotal=1
  for osi in $opts_sets; do
    declare local ossize=$(table_size_echo $osi)
    ((octotal = octotal * ossize))

    [[ -n $format ]] && table_dump_mf $osi
    [[ -z $format ]] && table_dump    $osi
  done

  print_textbox \
    -lt " $tcmtstr" -ct "$octotal total combinations" -rt "$tcmtstr " \
    $mfs -bt -pl -bb

  return
} # show_opts_set_tables()


## Output the processed arguments status summary.
#/##############################################################################
# \afn show_arg_list_status
#
# \aparami <arg1 string> title  Optional title used to label the summary
#                               output.
#
# \returns void
#
# \details
#   Output an accumulative list of the makefile generate commands, variable
#   set commands, processed and unprocessed by the script thus far.
#
# \private
###############################################################################/
function show_arg_list_status()
{
  declare local message="$1"
  shift 1

  print_textbox -bt -ct "$message" -pl -hr -clr \
                -lt " variable set commands processed" -rt "$vcmd " -pl \
                -lt " makefile generate commands processed" -rt "$gcmd " -pl \
                -lt " argument list offset index" -rt "$iarg " -pl \
                -hc '-' -hr \
                -lt " total invoked arguments" -rt "$argc " -pl \
                -lt " total parsed arguments" -rt "$parg " -pl \
                -lt " total unprocessed arguments" -rt "$# " -pl

  declare -i local csi=0

  if [[ $# -gt 0 ]] ; then
    print_textbox -bt -ct "(stanza) argument list" -pl -hr
    while [ $# -gt 0 ] ; do
      print_textbox -lt " [$((csi++))] $1" -pl
      shift 1
    done
  fi

  print_textbox -bb

  return
} # show_arg_list_status()


## Output a makefile target recipe.
#/##############################################################################
# \afn add_target
#
# \aparami <required named string> \--out_stem
#           The target stem name, its path/rootname, without a file extension.
# \aparami <required named string> \--tuple_list
#           This is the list of specific set option names from each of the
#           combined options assigned by \c set_opts_combine for this target.
# \aparami <required named string> \--opts_scad
#           This is a combination of the common options set by \c set_opts
#           and the concatenation of the specific option values from each of the
#           combined options set by \c set_opts_combine for this target.
#
# \returns void
#
# \details
#   Output a makefile target recepie for the given input parameters.
#
# \private
###############################################################################/
function add_target()
{
  declare local out_stem
  declare local tuple_list
  declare local opts_scad

  while [ $# -gt 0 ] ; do
    case $1 in
    --out_stem)     out_stem="$2"     ; shift 2 ;;
    --tuple_list)   tuple_list="$2"   ; shift 2 ;;
    --opts_scad)    opts_scad="$2"    ; shift 2 ;;
    *) abort_invalid_arg $1                     ;;
    esac
  done

  declare local filename_out="$out_stem.$output_ext"
  declare local filename_time="$out_stem.$ext_time"
  declare local filename_depend="$out_stem.$ext_depend"
  declare local filename_log="$out_stem.$ext_log"

  declare local srn="$(file_utility --rootname $source_name)"

  # target comment header
  print_m -j
  print_textbox -hvc '#' \
      -lt " openscad" -ct "$output_ext" -rt "target ${mnum}.${tnum}.${cnum} " \
      -bt -pl -hc '=' -hr
  for optsv in $tuple_list; do
    print_textbox -hvc '#' -ct "$optsv " -pl
  done
  print_textbox -hvc '#' -bb

  # append target to each category
  for tcs in "_${srn}_${output_ext}" "_${srn}" "" ; do
    print_m -j
    print_m -j targets${tcs} += $filename_out
    print_m -j cleanfiles${tcs} += $filename_time
    print_m -j cleanfiles${tcs} += $filename_depend
    print_m -j cleanfiles${tcs} += $filename_log

    # segregate log, if configured
    [[ -n $log_begin_rec ]] && \
    print_m -j cleanfiles${tcs} += \
      \$\(shell ${__LIB_PATH__}/scripts/segregate.bash --file $filename_log \
      --begin \"$log_begin_rec\" --end \"$log_end_rec\" \
      --list --ignore\)
  done

  # target recipe
  print_m -j
  print_m -j $filename_out : $source_name
  print_m -j -n -e "\t" \
    $sc_time $sc_time_opts $filename_time \
    $sc_openscad $sc_openscad_opts \
    -m make -d $filename_depend \
    $opts_scad \
    -o $filename_out $source_name \
    2\> $filename_log

  # segregate log, if configured
  [[ -n $log_begin_rec ]] && \
  print_m -j -n " &&" \
    ${__LIB_PATH__}/scripts/segregate.bash \
    --file $filename_log \
    --begin \"$log_begin_rec\" \
    --end \"$log_end_rec\"

  print_m -j

  # include openscad generated depend file (use if it exsists "-")
  print_m -j
  print_m -j -include $filename_depend

  # add target file directory dependancy
  print_m -j
  print_m -j $filename_out : \| $(file_utility --pathname $filename_out)

  # add target depends
  if [[ ${#depend_a[@]} -gt 0 ]] ; then
    print_m -j -n $out_stem.$output_ext :
    for tdi in ${!depend_a[@]} ; do
      print_m -j -n " ${depend_a[$tdi]}"
    done
    print_m -j
  fi

  return
} # add_target()


## Output a makefile format conversion target recipe.
#/##############################################################################
# \afn add_target_convert
#
# \aparami <required named string> \--in_stem
#           The target stem name; its path/rootname without a file extension.
# \aparami <required named string> \--new_ext
#           The new output extension to which the target is to be converted.
#
# \returns void
#
# \details
#   Output a makefile conversion target recipe for the given input parameters.
#   Currently only image conversions are supported and the conversion
#   is done by [convert] (http://www.imagemagick.org/script/convert.php).
#   To use this feature, ImageMagick must be installed on your system. See
#   The convert documentation for the list of supported image formats.
#
#   Image conversions are specified using \c set_convert_exts and
#   \c set_convert_opts \ref variables command. \c Set_convert_opts are
#   passed directly to \c convert for each target.
#
# \private
###############################################################################/
function add_target_convert()
{
  declare local in_stem
  declare local new_ext

  while [ $# -gt 0 ] ; do
    case $1 in
    --in_stem)      in_stem="$2"    ; shift 2 ;;
    --new_ext)      new_ext="$2"    ; shift 2 ;;
    *) abort_invalid_arg $1                   ;;
    esac
  done

  declare local filename_in="$in_stem.$output_ext"

  declare local new_sdir="${output_ext}2${new_ext}"
  declare local new_path="$(file_utility --prefix $output_path $new_sdir)"
  declare local new_root="$(file_utility --rootname $in_stem)"
  declare local new_stem="$(file_utility --prefix $new_path $new_root)"

  declare local filename_out="$new_stem.$new_ext"

  declare local srn="$(file_utility --rootname $source_name)"

  # comment
  print_m -j
  print_textbox -hvc '#' -bt \
      -lt " convert" -ct "$output_ext --> $new_ext" \
      -rt "target ${mnum}.${tnum}.${cnum} " -pl -bb

  # append target lists
  print_m -j
  print_m -j targets_${srn}_${new_sdir} += $filename_out
  print_m -j targets_${srn} += $filename_out
  print_m -j targets += $filename_out

  # target recipe
  print_m -j
  print_m -j $filename_out : $filename_in
  print_m -j -e "\t" \
    $sc_convert $sc_convert_opts $convert_opts $filename_in $filename_out

  # add target file directory dependancy
  print_m -j
  print_m -j $filename_out : \| $(file_utility --pathname $filename_out)

  return
} # add_target_convert()


## Output target recipes for all remaining possible option set combinations.
#/##############################################################################
# \afn generate_targets
#
# \aparami <required named string> \--out_stem
#           The target stem name; its path/rootname without a file extension.
# \aparami <required named string> \--tuple_list
#           This is the list of specific option key names from each of the
#           combined options set tables named in \c set_opts_combine for the
#           current target.
# \aparami <required named string> \--opts_scad
#           This is a list of the common options set by \c set_opts.
# \aparami <required named string> \--opts_sets
#           This is the remaining list of option set names to combine to
#           generate the remaining makefile targets.
#
# \returns void
#
# \details
#   For a given output stem, tuple list, OpenSCAD option list, and a list of
#   option set table names, produce target recipes for all possible option set
#   combinations by combining each option in each option set table with each
#   of the other option set table members, one at a time.
#
#   This functions uses \e recursion to combine the option sets and update
#   the OpenSCAD command line options for each specific n-tuple combination.
#
#   Specifically: for set[1]={a, b} and set[2]={c, d}, would produce 4
#   2-tuple targets: {ac}, {ad}, {bc}, and {bd}.
#
# \private
###############################################################################/
function generate_targets()
{
  declare local out_stem
  declare local tuple_list
  declare local opts_scad
  declare local opts_sets

  while [ $# -gt 0 ] ; do
    case $1 in
    --out_stem)   out_stem="$2"   ; shift 2 ;;
    --tuple_list) tuple_list="$2" ; shift 2 ;;
    --opts_scad)  opts_scad="$2"  ; shift 2 ;;
    --opts_sets)  opts_sets="$2"  ; shift 2 ;;
    *) abort_invalid_arg $1                 ;;
    esac
  done

  declare -a local opts_sets_a=($opts_sets)

  if [[ ${#opts_sets_a[@]} -eq 0 ]] ; then
    # add openscad target
    ((tnum++)) ; cnum=0
    add_target \
      --out_stem "$out_stem" \
      --tuple_list "$tuple_list" \
      --opts_scad "$opts_scad"

    # add image convert targets, if any
    if [[ -n $convert_exts ]] ; then
      for co in $convert_exts ; do
        ((cnum++))
        add_target_convert \
          --in_stem "$out_stem" \
          --new_ext "$co"
      done
    fi
  else
    declare local this_opts=${opts_sets_a[0]}
    for i in $(table_getkeynames_echo $this_opts); do

      declare local this_opt_val=$(table_getval_echo $this_opts $i)

      # remove leading and trailing whitespace characters
      opts_sets="${opts_sets#"${opts_sets%%[![:space:]]*}"}"
      opts_sets="${opts_sets%"${opts_sets##*[![:space:]]}"}"

      declare local remain_opt_sets=${opts_sets#${opts_sets_a[0]}}

      generate_targets \
        --out_stem "$out_stem$i" \
        --tuple_list "$tuple_list [$i]" \
        --opts_scad "$opts_scad $this_opt_val" \
        --opts_sets "$remain_opt_sets"
    done
  fi

  return
} # end generate_targets()


## Output target recipes for all possible option set table combinations.
#/##############################################################################
# \afn add_targets
#
# \returns void
#
# \details
#   Produce makefile target recipes for all possible option combinations
#   by combining each option in each option set tables with each option
#   of the other option set tables, one at a time.
#
#   This functions calls the recursive function \ref generate_targets with the
#   proper initial conditions for this recursive function.
#
# \private
###############################################################################/
function add_targets()
{
  declare local srn="$(file_utility --rootname $source_name)"
  declare local output_stempath="$(file_utility --prefix $output_path $output_ext)"
  declare local output_stem="$(file_utility --prefix $output_stempath $srn)"

  declare -a local opts_cnt_a=($opts_sets)

  [[ -n $__VERBOSE__ ]] && \
  print_textbox -vc '' -bl -vc '#' -fc '-' -rt " directories " -pl

  add_targets_dirs

  [[ -n $__VERBOSE__ ]] && \
  print_textbox -vc '' -bl -vc '#' -fc '-' -rt " recipes " -pl

  tnum=0
  generate_targets \
    --out_stem "$output_stem" \
    --tuple_list "${#opts_cnt_a[@]}-tuple" \
    --opts_scad "$opts_common" \
    --opts_sets "$opts_sets"

  return
} # add_targets()


## Output all required directory creation target recipes.
#/##############################################################################
# \afn add_targets_dirs
#
# \returns void
#
# \details
#   Output targets to create the output directories required by the
#   OpenSCAD targets and each of the specified conversion targets.
#
# \private
###############################################################################/
function add_targets_dirs()
{
  declare local srn="$(file_utility --rootname $source_name)"
  declare local osp="$(file_utility --prefix $output_path $output_ext)"

  # target for 'stempath' directory
  print_m -j
  print_m -j targetsdir_${srn}_${output_ext} = $osp
  print_m -j targetsdir_${srn} += $osp
  print_m -j targetsdir += $osp

  print_m -j
  print_m -j $osp :
  print_m -j -e "\t" $sc_mkdir $sc_mkdir_opts $osp

  # image convert target directories, if any
  if [[ -n $convert_exts ]] ; then
    for co in $convert_exts ; do
      declare local noutput_ext="${output_ext}2${co}"
      declare local nosp="$(file_utility --prefix $output_path $noutput_ext)"

      print_m -j
      print_m -j targetsdir_${srn}_${noutput_ext} = $nosp
      print_m -j targetsdir_${srn} += $nosp
      print_m -j targetsdir += $nosp

      print_m -j
      print_m -j $nosp :
      print_m -j -e "\t" $sc_mkdir $sc_mkdir_opts $nosp
    done
  fi

  return
} # add_targets_dirs()


## Output phony menu targets for the makefile, source file, and/or output extension.
#/##############################################################################
# \afn add_targets_menu
#
# \aparami <named> \--all   Output <em>all</em> menu targets. This will add
#                           a phony target to generate all of the targets
#                           named in the current makefile.
# \aparami <named> \--src   Output <em>source file</em> menu targets.
#                           This will add a phony target to generate all of
#                           the targets for the current source file.
# \aparami <named> \--ext   Output <em>source file extension</em> menu targets.
#                           This will add a phony target to generate all of the
#                           targets for the current source file's current
#                           output extension.
#
# \returns void
#
# \details
#   Output explicit make targets of the form all<suffix>, echo<suffix>,
#   list<suffix>, and clean<suffix> for the specified category. The suffix
#   for each category is shown in the following table.
#
#   category | suffix
#   :-------:|:------------------------------
#     all    | <none>
#     src    | Source Stemname
#     ext    | Stemname plus output extension
#
# \private
###############################################################################/
function add_targets_menu()
{
  # output menu entries
  function add_targets_menu_suffix()
  {
    declare local suffix=$1

    # all<*>:
    print_m -j
    print_m -j .PHONY : all$suffix
    print_m -j all$suffix : \$\{targets$suffix\}

    # echo_all<*>:
    print_m -j
    print_m -j .PHONY : echo_all$suffix
    print_m -j echo_all$suffix :
    print_m -j -e "\t" "@echo \"\${targetsdir$suffix} \${targets$suffix} \${cleanfiles$suffix}\""

    # echo_targetsdir<*>:
    print_m -j
    print_m -j .PHONY : echo_targetsdir$suffix
    print_m -j echo_targetsdir$suffix :
    print_m -j -e "\t" "@echo \"\${targetsdir$suffix}\""

    # echo_targets<*>:
    print_m -j
    print_m -j .PHONY : echo_targets$suffix
    print_m -j echo_targets$suffix :
    print_m -j -e "\t" "@echo \"\${targets$suffix}\""

    # echo_cleanfiles<*>:
    print_m -j
    print_m -j .PHONY : echo_cleanfiles$suffix
    print_m -j echo_cleanfiles$suffix :
    print_m -j -e "\t" "@echo \"\${cleanfiles$suffix}\""

    # list_all<*>:
    print_m -j
    print_m -j .PHONY : list_all$suffix
    print_m -j list_all$suffix :
    print_m -j -e "\t" "@echo ; echo \"# [ targetsdir$suffix ] (\$(words \${targetsdir$suffix}))\""
    print_m -j -e "\t" "@for x in \$(shell seq 80); do echo -n \"#\" ; done ; echo"
    print_m -j -e "\t" "@\$(foreach v,\${targetsdir$suffix},echo \${v};)"

    print_m -j -e "\t" "@echo ; echo \"# [ targets$suffix ] (\$(words \${targets$suffix}))\""
    print_m -j -e "\t" "@for x in \$(shell seq 80); do echo -n \"#\" ; done ; echo"
    print_m -j -e "\t" "@\$(foreach v,\${targets$suffix},echo \${v};)"

    print_m -j -e "\t" "@echo ; echo \"# [ cleanfiles$suffix ] (\$(words \${cleanfiles$suffix}))\""
    print_m -j -e "\t" "@for x in \$(shell seq 80); do echo -n \"#\" ; done ; echo"
    print_m -j -e "\t" "@\$(foreach v,\${cleanfiles$suffix},echo \${v};)"

    # list_targetsdir<*>:
    print_m -j
    print_m -j .PHONY : list_targetsdir$suffix
    print_m -j list_targetsdir$suffix :
    print_m -j -e "\t" "@echo ; echo \"# [ targetsdir$suffix ] (\$(words \${targetsdir$suffix}))\""
    print_m -j -e "\t" "@for x in \$(shell seq 80); do echo -n \"#\" ; done ; echo"
    print_m -j -e "\t" "@\$(foreach v,\${targetsdir$suffix},echo \${v};)"

    # list_targets<*>:
    print_m -j
    print_m -j .PHONY : list_targets$suffix
    print_m -j list_targets$suffix :
    print_m -j -e "\t" "@echo ; echo \"# [ targets$suffix ] (\$(words \${targets$suffix}))\""
    print_m -j -e "\t" "@for x in \$(shell seq 80); do echo -n \"#\" ; done ; echo"
    print_m -j -e "\t" "@\$(foreach v,\${targets$suffix},echo \${v};)"

    # list_cleanfiles<*>:
    print_m -j
    print_m -j .PHONY : list_cleanfiles$suffix
    print_m -j list_cleanfiles$suffix :
    print_m -j -e "\t" "@echo ; echo \"# [ cleanfiles$suffix ] (\$(words \${cleanfiles$suffix}))\""
    print_m -j -e "\t" "@for x in \$(shell seq 80); do echo -n \"#\" ; done ; echo"
    print_m -j -e "\t" "@\$(foreach v,\${cleanfiles$suffix},echo \${v};)"

    # clean<*>:
    print_m -j
    print_m -j .PHONY : clean$suffix
    print_m -j clean$suffix :
    print_m -j -e "\t" "-@test -z \"\${targets$suffix}\" || $sc_rm $sc_rm_opts \${targets$suffix}"
    print_m -j -e "\t" "-@test -z \"\${cleanfiles$suffix}\" || $sc_rm $sc_rm_opts \${cleanfiles$suffix}"
    print_m -j -e "\t" "-@test -z \"\${targetsdir$suffix}\" || $sc_rmdir $sc_rmdir_opts \${targetsdir$suffix}"
  }

 declare local srn="$(file_utility --rootname $source_name)"

  while [ $# -gt 0 ] ; do
    case $1 in
    --all)
        add_targets_menu_suffix ""
        shift 1
    ;;
    --src)
        add_targets_menu_suffix "_${srn}"
        shift 1
    ;;
    --ext)
        if [[ -n $convert_exts ]] ; then
          for co in $convert_exts ; do
            declare local noutput_ext="${output_ext}2${co}"
            add_targets_menu_suffix "_${srn}_${noutput_ext}"
          done
        fi
        add_targets_menu_suffix "_${srn}_${output_ext}"
        shift 1
    ;;
    *) abort_invalid_arg $1 ;;
    esac
  done

  return
} # add_targets_menu()


## Helper function for \ref script for redirecting output to makefile.
#/##############################################################################
# \afn script_redirect
#
# \returns void
#
# \details
#   This function is a helper function that is part of the function \ref script.
#   When script begin to generate a makefile, output is redirected to the named
#   makefile and the remaining script arguments are passed to this helper
#   function to continue processing. After all of the arguments have been
#   exhausted, control is returned to script.
#
#   The documentation for the arguments of this function may be found in the
#   \ref script parent function below.
#
# \private
###############################################################################/
function script_redirect()
{
  while [ $# -gt 0 ] ; do
    [[ -n $__VERBOSE__ ]] && \
    print_textbox -vc '' -bl -vc '#' -fc '-' -ct " $1 " -pl

    case ${1,,} in
    include)
        declare local file="$2"

        print_m -j
        print_m -j "include $file"

        ((gcmd++)) ; ((parg+=2)) ; ((iarg+=2)) ; shift 2
    ;;
    include_copy)
        declare local file="$2"

        [[ -r $file ]] || abort_error "Unable to read file [$file]."

        print_m -j
        cat $file

        ((gcmd++)) ; ((parg+=2)) ; ((iarg+=2)) ; shift 2
    ;;
    summary)
        show_variable_summary --mf_format

        ((gcmd++)) ; ((parg++)) ; ((iarg++)) ; shift 1
    ;;
    tables)
        show_opts_set_tables --mf_format

        ((gcmd++)) ; ((parg++)) ; ((iarg++)) ; shift 1
    ;;
    targets)
        add_targets

        ((gcmd++)) ; ((parg++)) ; ((iarg++)) ; shift 1
    ;;
    menu)
        add_targets_menu --ext --src --all

        ((gcmd++)) ; ((parg++)) ; ((iarg++)) ; shift 1
    ;;
    menu_all)
        add_targets_menu --all

        ((gcmd++)) ; ((parg++)) ; ((iarg++)) ; shift 1
    ;;
    menu_src)
        add_targets_menu --src

        ((gcmd++)) ; ((parg++)) ; ((iarg++)) ; shift 1
    ;;
    menu_ext)
        add_targets_menu --ext

        ((gcmd++)) ; ((parg++)) ; ((iarg++)) ; shift 1
    ;;
    end_makefile)
        declare local title_info="$source_name --> $output_ext"
        [[ -n $convert_exts ]] && title_info+=" --> $convert_exts"

        makefile_titleblock_end "$title_info"

        ((gcmd++)) ; ((parg++)) ; ((iarg++)) ; shift 1

        break # leave generate mode
    ;;
    status)
        show_arg_list_status "in ${FUNCNAME[0]}." "$@"
        ((gcmd++)) ; ((parg++)) ; ((iarg++)) ; shift 1
    ;;
    abort)
        show_arg_list_status "abort request in ${FUNCNAME[0]}." "$@"
        ((gcmd++)) ; ((parg++)) ; ((iarg++)) ; shift 1
        exit 1
    ;;
    *) abort_invalid_arg $1 "in ${FUNCNAME[0]}." ;;
    esac
  done

  return
}  # script_redirect()


## Script argument keyword processing.
#/##############################################################################
# \afn script
#
# \aparami <named> begin_makefile
#     Begin the generation of a new makefile. Prior to using this keyword,
#     the makefile name, source file name, and output extension must be set
#     using \ref variables \p set_makefile, \p set_source, and
#     \p set_ext. If the named makefile exists, it will be overwritten.
#     When the environment variable \c \__VERBOSE\__ is not \c null, a variable
#     summary and options set tables will be written the makefile following
#     the commented header.
# \aparami <named> begin_makefile_new
#     Alias to \p begin_makefile.
# \aparami <named> begin_makefile_append
#     Identical to \p begin_makefile, with the exception that if the
#     named makefile exists, it will be appended.
# \aparami <named> end_makefile
#     Terminates the current makefile. Output will be redirected to the
#     console.
#
# \aparami <named file> include
#     Adds a makefile [include]
#     (https://www.gnu.org/software/make/manual/html_node/Include.html)
#     directive to the generated makefile. This will caused the \p file_name
#     to be read into the generated makefile when it is being processed by
#     \c make.
# \aparami <named file> include_copy
#     Like /p include, except that this reads and places a static copy of
#     \p file_name into the makefile being generated.
#
# \aparami  <named> summary
#     Output a summary of the set \ref variables with their description and
#     current value to a makefile comment block.
# \aparami <named> tables
#     Output a table of all of the options sets specified by \ref variables
#     \c set_opts_combine to a makefile comment block.
#
# \aparami <named> targets
#     Generate and output all of the makefile targets by looping through
#     all possible combinations in the options sets. The sets must be
#     configured and specified using \ref variables \c set_opts_combine prior.
# \aparami <named> menu_all
#     Add the \c all:, \c list:, and \c clean:
#     targets to the makefile. These phony targets will generates, list, and
#     clean all of the target recipes in the makefile.
# \aparami <named> menu_src
#     Add the \c all_<sf>:, \c list_<sf>:, and \c clean_<sf>:
#     targets to the makefile. These phony targets will generates, list, and
#     clean all of the <em>source file</em> target recipes in the makefile.
# \aparami <named> menu_ext
#     Add the \c all_<sfoe>:, \c list_<sfoe>:, and \c clean_<sfoe>:
#     targets to the makefile. These phony targets will generates, list, and
#     clean all of the <em>source file output extension</em> target recipes
#     in the makefile.
# \aparami <named> menu
#     Alias to \p menu_ext \p menu_src \p menu_all. This is a useful shortcut
#     when there is only one source file and one output extension.
#
# \aparami <named> status
#     Output the script's processed arguments summary status to a makefile
#     comment block.
# \aparami <named> abort
#     Output the current status to the makefile via \p status and abort
#     the script, returning \c error to the calling function.
#
# \retval 0 on success
# \retval 1 on error
#
# \ingroup library_core
###############################################################################/
function script()
{
  ((argc+=$#))

  while [ $# -gt 0 ] ; do
    case ${1,,} in
    begin_makefile|begin_makefile_new|begin_makefile_append)
        declare local mode="${1,,}"
        declare local title_info="$source_name --> $output_ext"

        [[ -n $convert_exts ]] && title_info+=" --> $convert_exts"

        ((gcmd++)) ; ((parg++)) ; shift 1

        ((mnum++))

        if [[ -n "$makefile_name" ]] ; then
          # show summary to console
          if [[ -n $__VERBOSE__ ]] ; then
            show_variable_summary
            show_opts_set_tables
          fi

          # report begin mode to console
          if [[ -n $__VERBOSE__ ]] ; then
            declare local mode_str="creating"

            [[ "$mode" ==  "begin_makefile_append" ]] && mode_str="appending"

            print_m -j
            print_textbox -lt " ($mode_str)" -rt "($mode_str) " \
                          -ct "makefile: $makefile_name" -bc 3 -hc '-' -bt -pl -bb
            print_m -j
          fi

          # save current textbox widths and set for makefile values
          declare -i local saved_tw=${print_textbox_term_width}
          declare -i local saved_bw=${print_textbox_box_width}

          print_textbox_term_width=${makefile_textbox_term_width}
          print_textbox_box_width=${makefile_textbox_box_width}

          # begin makefile with comment title block
          if [[ "$mode" ==  "begin_makefile_append" ]] ; then
            makefile_titleblock_begin >> $makefile_name "$title_info"
          else
            makefile_titleblock_begin  > $makefile_name "$title_info"
          fi

          ######################################
          # redirect script output to makefile #
          ######################################
          script_redirect >> $makefile_name "$@"

          # restore saved textbox column settings
          print_textbox_term_width=$saved_tw
          print_textbox_box_width=$saved_bw
        else
          abort_error "makefile name not set, use command [--makefile_name]."
        fi

        shift $iarg
        iarg=0
    ;;
    status)
        show_arg_list_status "in ${FUNCNAME[0]}." "$@"
        ((gcmd++)) ; ((parg++)) ; shift 1
    ;;
    abort)
        show_arg_list_status "abort request in ${FUNCNAME[0]}." "$@"
        ((gcmd++)) ; ((parg++)) ; shift 1
        exit 1
    ;;
    *) abort_invalid_arg $1 "in ${FUNCNAME[0]}." ;;
    esac
  done

  [[ -n $__VERBOSE__ ]] && \
  show_arg_list_status "exiting ${FUNCNAME[0]}." "$@"

  return 0
}


## Variable argument keyword processing function.
#/##############################################################################
# \afn variables
#
# \aparami <named file> set_makefile
#     Sets the makefile name.
# \aparami <named file> set_source
#     Sets the source file name.
# \aparami <named path> set_prefix
#     Sets the output path prefix.
# \aparami <named extension> set_ext
#     Sets the output extension for the targets.
#
# \aparami <named string> set_log_begin_record
#     Set the text string that starts a new record in an output log file
#     that is to be segregated into individual record files.
# \aparami <named string> set_log_end_record
#     Set the text string that ends a record in an output log file
#     that is to be segregated into individual record files.
#
# \aparami <named string> set_opts
#     Sets the common OpenSCAD command line options string. These options
#     will be used for each and every generated target.
# \aparami <named string> add_opts
#     Appends text to the common OpenSCAD command line options string.
# \aparami <named> clear_opts
#     Clears the common OpenSCAD command line options string.
#
# \aparami <named set list> set_opts_combine
#     Assigns the set names with OpenSCAD command line options that
#     will be looped over and combined during target generation.
# \aparami <named set list> add_opts_combine
#     Appends set names to the combined OpenSCAD command line options.
# \aparami <named> clear_opts_combine
#     Clears the combined set names.
#
# \aparami <named extension list> set_convert_exts
#     Sets the list of output extensions to which a target is to be
#     converted.
# \aparami <named extension list> add_convert_exts
#     Appends to the output extension conversion list.
# \aparami <named> clear_convert_exts
#     Clears the output extension conversion list.
#
# \aparami <named string> set_convert_opts
#     Sets the conversion command line options.
#     For images, see: [image conversion options]
#     (http://www.imagemagick.org/script/command-line-processing.php#option).
# \aparami <named string> add_convert_opts
#     Appends text to the conversion command line options.
# \aparami <named> clear_convert_opts
#     Clears the conversion command line options.
#
# \aparami <named> clear_convert
#     Alias to \p clear_convert_exts \p clear_convert_opts.
#
# \aparami <named file> add_depend
#     Add a dependency for each generated target. The \p file_name specified
#     will be made a prerequisite for each OpenSCAD output target.
#     A separate \p add_depend is required for each desired dependency.
# \aparami <named> clear_depend
#     Clear all target dependencies.
#
# \aparami <named> clear
#     Clear all variables.
#
# \aparami  <named> summary
#     Output a summary of the variables with their description and
#     current value to the console.
# \aparami <named> tables
#     Output a table of all of the options sets specified by
#     \c set_opts_combine to the console.
#
# \aparami <named> status
#     Output the script's processed arguments summary status to the console.
# \aparami <named> abort
#     Output the current status to the console via \p status and abort
#     the script, returning \c error to the calling function.
#
# \retval 0 on success
# \retval 1 on error
#
# \ingroup library_core
###############################################################################/
function variables()
{
  ((argc+=$#))

  while [ $# -gt 0 ] ; do
    case ${1,,} in
    set_makefile)         makefile_name="$2"  ; ((vcmd++)) ; ((parg+=2)) ; shift 2 ;;
    set_source)           source_name="$2"    ; ((vcmd++)) ; ((parg+=2)) ; shift 2 ;;
    set_prefix)           output_path="$2"    ; ((vcmd++)) ; ((parg+=2)) ; shift 2 ;;
    set_ext)              output_ext="$2"     ; ((vcmd++)) ; ((parg+=2)) ; shift 2 ;;

    set_log_begin_record) log_begin_rec="$2"  ; ((vcmd++)) ; ((parg+=2)) ; shift 2 ;;
    set_log_end_record)   log_end_rec="$2"    ; ((vcmd++)) ; ((parg+=2)) ; shift 2 ;;

    set_opts)             opts_common="$2"    ; ((vcmd++)) ; ((parg+=2)) ; shift 2 ;;
    set_opts_combine)     opts_sets="$2"      ; ((vcmd++)) ; ((parg+=2)) ; shift 2 ;;
    set_convert_exts)     convert_exts="$2"   ; ((vcmd++)) ; ((parg+=2)) ; shift 2 ;;
    set_convert_opts)     convert_opts="$2"   ; ((vcmd++)) ; ((parg+=2)) ; shift 2 ;;

    add_opts)             [[ -n ${opts_common} ]] && opts_common+=" "
                          opts_common+="$2"   ; ((vcmd++)) ; ((parg+=2)) ; shift 2 ;;
    add_opts_combine)     [[ -n ${opts_sets} ]] && opts_sets+=" "
                          opts_sets+="$2"     ; ((vcmd++)) ; ((parg+=2)) ; shift 2 ;;
    add_convert_exts)     [[ -n ${convert_exts} ]] && convert_exts+=" "
                          convert_exts+="$2"  ; ((vcmd++)) ; ((parg+=2)) ; shift 2 ;;
    add_convert_opts)     [[ -n ${convert_opts} ]] && convert_opts+=" "
                          convert_opts+="$2"  ; ((vcmd++)) ; ((parg+=2)) ; shift 2 ;;

    add_depend)
        depend_a[${#depend_a[@]}]="$2"        ; ((vcmd++)) ; ((parg+=2)) ; shift 2 ;;

    clear_opts)           opts_common=""      ; ((vcmd++)) ; ((parg+=1)) ; shift 1 ;;
    clear_opts_combine)   opts_sets=""        ; ((vcmd++)) ; ((parg+=1)) ; shift 1 ;;
    clear_convert_exts)   convert_exts=""     ; ((vcmd++)) ; ((parg+=1)) ; shift 1 ;;
    clear_convert_opts)   convert_opts=""     ; ((vcmd++)) ; ((parg+=1)) ; shift 1 ;;
    clear_convert)        convert_exts=""
                          convert_opts=""     ; ((vcmd++)) ; ((parg+=1)) ; shift 1 ;;

    clear_depend)
        for i in "${!depend_a[@]}"; do unset "depend_a[$i]"; done
        ((vcmd++)) ; ((parg+=1)) ; shift 1 ;;

    clear)
        # strings
        makefile_name=""
        source_name=""
        output_path=""
        output_ext=""
        opts_common=""
        opts_sets=""
        convert_exts=""
        convert_opts=""
        log_begin_rec=""
        log_end_rec=""

        # array: depend_a
        for i in "${!depend_a[@]}"; do unset "depend_a[$i]"; done

        ((vcmd++)) ; ((parg++)) ; shift 1
    ;;
    summary)
        show_variable_summary
        ((vcmd++)) ; ((parg++)) ; shift 1
    ;;
    tables)
        show_opts_set_tables
        ((vcmd++)) ; ((parg++)) ; shift 1
    ;;
    status)
        show_arg_list_status "in ${FUNCNAME[0]}." "$@"
        ((vcmd++)) ; ((parg++)) ; shift 1
    ;;
    abort)
        show_arg_list_status "abort request in ${FUNCNAME[0]}." "$@"
        ((vcmd++)) ; ((parg++)) ; shift 1
        exit 1
    ;;
    *) abort_invalid_arg $1 "in ${FUNCNAME[0]}." ;;
    esac
  done

  return 0
} # variables()


#==============================================================================
# protect functions
#==============================================================================
readonly -f \
  makefile_titleblock_begin \
  makefile_titleblock_end \
  show_variable_summary \
  show_opts_set_tables \
  show_arg_list_status \
  add_target \
  add_target_convert \
  generate_targets \
  add_targets \
  add_targets_dirs \
  add_targets_menu \
  script_redirect \
  script \
  variables


#==============================================================================
# eof
#==============================================================================
