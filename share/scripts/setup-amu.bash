#!/usr/bin/env bash
#/##############################################################################
#
#   \file   setup-amu.bash
#
#   \author Roy Allen Sutton <royasutton@hotmail.com>.
#   \date   2016-2018
#
#   \copyright
#
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
###############################################################################/

declare base_path="${0%/*}"
declare base_name="${0##*/}"
declare root_name="${base_name%.*}"
declare work_path="${PWD}"
declare conf_file="${root_name}.conf"

declare kernel=$(uname -s)
declare sysname=${kernel%%-*}

# verify minimum bash version
if [[ $BASH_VERSINFO -lt 4 ]] ; then
  echo "Bash version is $BASH_VERSION. Version 4 or greater required."
  exit 1
fi

###############################################################################
# global variables
###############################################################################

declare design_flow="df1"

declare skip_check="no"
declare skip_prep="no"

declare apt_cyg_path
declare apt_get_opts="--verbose-versions"
declare git_fetch_opts="--verbose"

declare repo_url_apt_cyg="https://github.com/transcode-open/apt-cyg"

declare repo_url="https://github.com/royasutton/openscad-amu"
declare repo_branch="master"
declare repo_branch_list

declare cache_install="no"
declare repo_cache_root="cache"

declare force_reconfigure="no"
declare configure_opts_add

declare make_job_slots

declare commands

# ( "variable-key" "description" "example-value" ... )
declare -i conf_file_vw=3
declare -a conf_file_va=(
  "design_flow"
      "design flow name"
      "$design_flow"
  "skip_check"
      "skip system prerequisites check"
      "$skip_check"
  "skip_prep"
      "skip source preparation"
      "$skip_prep"
  "apt_cyg_path"
      "path to apt-cyg"
      "/usr/local/bin/apt-cyg"
  "apt_get_opts"
      "apt get options"
      "$apt_get_opts"
  "git_fetch_opts"
      "git fetch options"
      "$git_fetch_opts"
  "repo_url_apt_cyg"
      "apt-cyg git repo url (can be local path)"
      "$repo_url_apt_cyg"
  "repo_url"
      "git repo url (can be local path)"
      "$repo_url"
  "repo_branch"
      "git repo branch or tag"
      "$repo_branch"
  "repo_branch_list"
      "git repo branch and/or tag list"
      "v1.7 v1.8.2 master develop"
  "cache_install"
      "install to cache"
      "$cache_install"
  "repo_cache_root"
      "cache root directory path"
      "$repo_cache_root"
  "force_reconfigure"
      "force source autotools reconfiguration"
      "$force_reconfigure"
  "configure_opts_add"
      "autotools reconfiguration options"
      "--with-boost=DIR --silent --prefix=${HOME}/openscad-amu"
  "make_job_slots"
      "maximum simultaneous make jobs"
      "4"
  "commands"
      "commands to run with each invocation"
      "--branch master --install --template my_project"
)

# derived variables
declare packages
declare packages_installed
declare packages_missing

declare templates

declare repo_cache_apt_cyg
declare repo_cache

declare build_dir

declare cache_prefix
declare cache_bindir
declare configure_opts

###############################################################################
# message printing
###############################################################################

function print_m() {
  local nl
  local es
  local -i rn=1
  local ws=' '
  local ns="${ws}"

  case $1 in
  -j)                                                     ;;
  -J) print_m -j -n -r ${#root_name} ' ' -j ' ' ; shift 1 ;;
   *) echo -n ${root_name}:                               ;;
  esac

  while [[ $# -gt 0 ]] ; do
    case $1 in
    -n) nl=true           ;;
    -e) es=true           ;;
    -E) es=''             ;;
    -j) ns=''             ;;
    -l) echo ;            ;;
    -r) rn=$2   ; shift 1 ;;
    -s) ws="$2" ; shift 1 ;;
     *)
      while ((rn > 0))
      do
        if [[ -z "$es" ]] ; then
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

  [[ -z "$nl" ]] && echo

  return
}

function print_hb () {
  local tput=$(which tput 2>/dev/null)
  local cols=80

  [[ -n "$tput" ]] && cols=$(${tput} cols)

  print_m -j -r ${cols} ${1:-#}
}

function print_h1 () {
  print_hb "#"
  print_m -j "#" -l -j "#" $* -l -j "#"
  print_hb "#"
}

function print_h2 () {
  print_hb "="
  print_m -j $*
  print_hb "="
}

###############################################################################
# os dependent functions
###############################################################################

#------------------------------------------------------------------------------
# update make job slots count
#------------------------------------------------------------------------------
function update_make_job_slots() {
  print_m "${FUNCNAME} begin"

  # when not set, use system processor/thread count
  if [[ -z "${make_job_slots}" ]] ; then
    case "${sysname}" in
      Linux)
        make_job_slots=$(nproc)
      ;;
      CYGWIN_NT)
        make_job_slots=$(nproc)
      ;;
      *)
        print_m "ERROR: Configuration for [$sysname] required."
        exit 1
      ;;
    esac
  else
    print_m "using preset limit."
  fi

  print_m "make job slots = ${make_job_slots}"

  print_m "${FUNCNAME} end"
}

#------------------------------------------------------------------------------
# design flow configuration:
#   (1) prerequisite package list and
#   (2) template file list
#------------------------------------------------------------------------------
function update_prerequisite_list() {
  print_m "${FUNCNAME} begin"

  local packages_Common
  local packages_Linux
  local packages_CYGWIN_NT

  case "${design_flow}" in
    # df1 with latex
    df1t)
      packages_Common="
        doxygen
        texlive
        graphviz
        git
        autoconf
        automake
        libtool
        bash
        make
        flex
        zip
      "

      packages_Linux="
        openscad
        g++
        libboost-all-dev
        imagemagick
        texlive-latex-extra
      "

      packages_CYGWIN_NT="
        gcc-g++
        time
        libboost-devel
        ImageMagick
        texlive-collection-basic
        texlive-collection-bibtexextra
        texlive-collection-binextra
        texlive-collection-fontsrecommended
        texlive-collection-fontutils
        texlive-collection-latex
        texlive-collection-latexextra
        texlive-collection-latexrecommended
        texlive-collection-mathextra
        texlive-collection-pictures
      "

      templates="
        Project_Makefile
        Doxyfile
        design.scad
        library.scad
      "
    ;;

    # df1 without latex
    df1)
      packages_Common="
        doxygen
        graphviz
        git
        autoconf
        automake
        libtool
        bash
        make
        flex
        zip
      "

      packages_Linux="
        openscad
        g++
        libboost-all-dev
        imagemagick
      "

      packages_CYGWIN_NT="
        gcc-g++
        time
        libboost-devel
        ImageMagick
      "

      templates="
        Project_Makefile
        Doxyfile
        design.scad
        library.scad
      "
    ;;

    *)
      print_m "ERROR: Design flow [$design_flow] not supported."
      exit 1
    ;;
  esac

  case "${sysname}" in
    Linux)
      packages="${packages_Common} ${packages_Linux}"
    ;;
    CYGWIN_NT)
      packages="${packages_Common} ${packages_CYGWIN_NT}"
    ;;
    *)
      print_m "ERROR: Configuration for [$sysname] required."
      exit 1
    ;;
  esac

  print_m "${FUNCNAME} end"
}

#==============================================================================
# Linux
#==============================================================================

function prerequisites_check.Linux() {
  dpkg-query --show --showformat='${Status}\n' $1 2>/dev/null |
    grep -q "install ok installed" &&
      return 0

  return 1
}

function prerequisites_status.Linux() {
  dpkg-query --list $*
}

function prerequisites_install.Linux() {
  print_m "apt-get install options: [${apt_get_opts}]"
  if ! sudo apt-get install ${apt_get_opts} $* ; then
    print_m "ERROR: install failed. aborting..."
    exit 1
  fi
}

#==============================================================================
# Cygwin
#==============================================================================

function prerequisites_check.CYGWIN_NT() {
  cygcheck --check-setup --dump-only $1 |
    tail -1 |
    grep -q $1 &&
      return 0

  return 1
}

function prerequisites_status.CYGWIN_NT() {
  cygcheck --check-setup $*
}

function prerequisites_install.CYGWIN_NT() {
  set_apt_cyg_path
  if ! ${apt_cyg_path} install $* ; then
    print_m "ERROR: install failed. aborting..."
    exit 1
  fi
}

function set_apt_cyg_path() {
  local cmd_name="apt-cyg"
  local cmd_cache=${repo_cache_apt_cyg}/${cmd_name}

  if [[ -z "${apt_cyg_path}" ]] ; then
    apt_cyg_path=$(which 2>/dev/null ${cmd_name} ${cmd_cache} | head -1)

    if [[ -x "${apt_cyg_path}" ]] ; then
      print_m "found: ${cmd_name}=${apt_cyg_path}"
    else
      print_m "fetching apt-cyg git repository to ${repo_cache_apt_cyg}"
      repository_update "${repo_url_apt_cyg}" "${repo_cache_apt_cyg}"

      if [[ -e "${cmd_cache}" ]] ; then
        [[ ! -x "${cmd_cache}" ]] && chmod --verbose +x ${cmd_cache}
        apt_cyg_path=$(which 2>/dev/null ${cmd_name} ${cmd_cache} | head -1)
        print_m "using cached: ${cmd_name}=${apt_cyg_path}"
        print_m "adding [${apt_cyg_path%/*}] to shell path"
        PATH=${apt_cyg_path%/*}:${PATH}
      else
        print_m "ERROR: unable to locate or cache ${cmd_name}. aborting..."
        exit 1
      fi
    fi
  fi
}

###############################################################################
# os independent functions
###############################################################################

#==============================================================================
# general / core
#==============================================================================

#------------------------------------------------------------------------------
# write configuration file
#------------------------------------------------------------------------------
function write_configuration_file() {
  print_m "${FUNCNAME} begin"

  local file="$1"         ; shift 1
  local -i cv_w="$1"      ; shift 1
  local -a cv_a=( "$@" )

  local -i cv_s=$(( ${#cv_a[@]} / ${cv_w} ))

  local cv_c="#"

  if [[ -e ${file} ]]
  then
    print_m "configuration file ${file} exists... not writting."
  else
    print_m "writing ${cv_s} keys to ${file}"

    print_m >  ${file} -j -r 80 ${cv_c} -l \
                       -j "${cv_c} file: ${file}" -l \
                       -j -r 80 ${cv_c}

    cat >> ${file} << EOF
# note:
#  - Do not quote variable names or values.
#  - Tokenize multi-value lists with ' ' (space-character).
#  - 'commands' options are specified as they are on the commnad line,
#    with multi-value lists Tokenized by ',' (comma-character).
#  - Values can be split across lines using '\\':
#      commands=\\
#        --branch-list v1.5.1,master,develop --cache --reconfigure \\
#        --install --template-def
EOF

    print_m >> ${file} -j -r 80 ${cv_c} -l -l \
                       -j ${cv_c} -j -r 79 "=" -l \
                       -j "${cv_c} Supported Variables" -l \
                       -j ${cv_c} -j -r 79 "=" -l

    for ((i = 0; i < $cv_s; i++))
    do
      local kv=${cv_a[$(( $i*$cv_w + 0 ))]}
      local kd=${cv_a[$(( $i*$cv_w + 1 ))]}
      local ke=${cv_a[$(( $i*$cv_w + 2 ))]}

      print_m >> ${file} -j "${cv_c} [::: ${kd^} :::]" -l \
                         -j "${cv_c} ${kv}=${ke}" -l
    done

    print_m >> ${file} -j -r 80 ${cv_c} -l \
                       -j "${cv_c} eof" -l \
                       -j -r 80 ${cv_c}
  fi

  print_m "${FUNCNAME} end"
}

#------------------------------------------------------------------------------
# parse configuration file
#------------------------------------------------------------------------------
function parse_configuration_file() {
  print_m "${FUNCNAME} begin"

  local file="$1"         ; shift 1
  local -i cv_w="$1"      ; shift 1
  local -a cv_a=( "$@" )

  local -i cv_s=$(( ${#cv_a[@]} / ${cv_w} ))
  local -i cv_r=0

  function read_key()
  {
    local file="$1"
    local key="$2"

    # support line gobbling
    while IFS= read line || [[ -n "$line" ]]; do
      echo "$line"
    done < "${file}" |
    sed -e '/^[[:space:]]*$/d' \
        -e '/^[[:space:]]*#/d' \
        -e 's/^[[:space:]\t]*//' |
    grep "${key}=" |
    tail -1 |
    sed -e "s/${key}=//"
  }

  print_m "checking for ${cv_s} configuration keys"
  for ((i = 0; i < $cv_s; i++))
  do
    local key=${cv_a[$(( $i*$cv_w + 0 ))]}
    local cfv

    printf -v cfv '%s' "$(read_key ${file} $key)"
    if [[ -n "$cfv" ]] ; then
      printf -v $key '%s' "$cfv"
      print_m "setting $key=$cfv"

      (( ++cv_r ))
    fi
  done
  print_m "read ${cv_r} key values"

  print_m "${FUNCNAME} end"
}

#------------------------------------------------------------------------------
# update build variables
#------------------------------------------------------------------------------
function update_build_variables() {
  print_m "${FUNCNAME} begin"

  repo_cache_apt_cyg=${repo_cache_root}/apt-cyg
  repo_cache=${repo_cache_root}/openscad-amu

  build_dir="${repo_cache}/build/${repo_branch}/${sysname}"

  if [[ "${cache_install}" == "yes" ]] ; then
    cache_prefix="${work_path}/${repo_cache_root}/local"
    cache_bindir="${cache_prefix}/bin/${sysname}"
    configure_opts="--prefix=${cache_prefix} --bindir=${cache_bindir}"
  fi

  if [[ -n "${configure_opts_add}" ]] ; then
    [[ -n "${configure_opts}" ]] && configure_opts+=" ${configure_opts_add}"
    [[ -z "${configure_opts}" ]] && configure_opts="${configure_opts_add}"
  fi

  print_m "${FUNCNAME} end"
}

#------------------------------------------------------------------------------
# list prerequisite packages
#------------------------------------------------------------------------------
function prerequisites_list() {
  print_m "${FUNCNAME} begin"

  print_h2 "[ ${design_flow} prerequisites ]"
  for r in ${packages} ; do
    print_m -j $r
  done

  print_h2 "[ ${design_flow} templates ]"
  for t in ${templates} ; do
    print_m -j $t
  done
  print_hb "="

  print_m "${FUNCNAME} end"
}

#------------------------------------------------------------------------------
# update prerequisite package status lists
#------------------------------------------------------------------------------
function prerequisites_check() {
  print_m "${FUNCNAME} begin"

  packages_installed=""
  packages_missing=""

  update_prerequisite_list
  for r in ${packages} ; do
    if prerequisites_check.${sysname} $r
    then
      packages_installed+=" $r"
    else
      packages_missing+=" $r"
    fi
  done

  print_m "${FUNCNAME} end"
}

#------------------------------------------------------------------------------
# show prerequisite package status
#------------------------------------------------------------------------------
function prerequisites_status() {
  print_m "${FUNCNAME} begin"

  print_h2 "[ Installed ]"
  if [[ -n "${packages_installed}" ]] ; then
    prerequisites_status.${sysname} $packages_installed
  else
    print_m -j "installed prerequisite list is empty."
  fi

  print_h2 "[ Missing ]"
  if [[ -n "${packages_missing}" ]] ; then
    for r in ${packages_missing} ; do
      print_m -j $r
    done
  else
    print_m -j "missing prerequisite list is empty."
  fi
  print_hb "="

  print_m "${FUNCNAME} end"
}

#------------------------------------------------------------------------------
# install missing prerequisite packages
#------------------------------------------------------------------------------
function prerequisites_install() {
  print_m "${FUNCNAME} begin"

  if [[ -n "${packages_missing}" ]] ; then
    print_h2 "[ Missing ]"
    for r in ${packages_missing} ; do
      print_m -j $r
    done

    print_h2 "[ Installing ]"
    for r in ${packages_missing} ; do
      print_h2 "installing: [ $r ]"
      print_m -j $r
      prerequisites_install.${sysname} $r
    done
    print_hb "="
  else
    print_m -j "no missing prerequisites."
  fi

  print_m "${FUNCNAME} end"
}

#------------------------------------------------------------------------------
# clone or update a Git repository
#------------------------------------------------------------------------------
function repository_update() {
  print_m "${FUNCNAME} begin"

  local gitrepo="$1"
  local out_dir="$2"

  print_m "source: [${gitrepo}]"
  print_m " cache: [${out_dir}]"

  local git=$(which 2>/dev/null git)
  if [[ ! -x "${git}" ]] ; then
    print_m "ERROR: please install git:"

    case "${sysname}" in
      Linux)        print_m "ex: sudo apt-get install git" ;;
      CYGWIN_NT)    print_m "ex: run Cygwin setup and select Devel/git." ;;
    esac

    print_m "aborting..."
    exit 1
  fi

  if [[ -d ${out_dir} ]] ; then
    if ( cd ${out_dir} 2>/dev/null && git rev-parse 2>/dev/null ) ; then
      print_m "updating: Git repository cache"
      ( cd ${out_dir} && ${git} pull ${git_fetch_opts} )
    else
      print_m "ERROR: directory [${out_dir}] exists and is not a repository. aborting..."
      exit 1
    fi
  else
    print_m "cloning: Git repository to cache"
    ${git} clone ${gitrepo} ${out_dir} ${git_fetch_opts}
  fi

  if ( cd ${out_dir} 2>/dev/null && git rev-parse 2>/dev/null ) ; then
    print_m -n "repository description: "
    ( cd ${out_dir} && git describe --tags --long --dirty )
  else
    print_m "ERROR: repository update failed. aborting..."
    exit 1
  fi

  print_m "${FUNCNAME} end"
}

#------------------------------------------------------------------------------
# remove directory
#------------------------------------------------------------------------------
function remove_directory() {
  print_m "${FUNCNAME} begin"

  local dir_path="$1"
  local dir_desc="$2"

  [[ -n ${dir_desc} ]] && dir_desc+=" "

  if [[ -x ${dir_path} ]] ; then
    print_m "removing ${dir_desc}directory [${dir_path}]."
    rm -rfv ${dir_path}
  else
    print_m "${dir_desc}directory [${dir_path}] does not exists."
  fi

  print_m "${FUNCNAME} end"
}

#------------------------------------------------------------------------------
# prepare source for compilation
#------------------------------------------------------------------------------
function source_prepare() {
  print_m "${FUNCNAME} begin"

  # check for existing source repository
  if ! ( cd ${repo_cache} 2>/dev/null && git rev-parse 2>/dev/null ) ; then
    print_m "fetching repository cache."
    repository_update "${repo_url}" "${repo_cache}"
  fi

  # checkout branch
  if ! ( cd ${repo_cache} && git checkout ${repo_branch} ) ; then
    print_m "ERROR: failed to checkout branch [${repo_branch}]. aborting..."
    exit 1
  else
    print_m -n "repository branch description: "
    ( cd ${repo_cache} && git describe --tags --long --dirty )
  fi

  # generate autotools configure script
  [[ "${force_reconfigure}" == "yes" ]] && print_m "forcing configure."
  if [[ -x ${repo_cache}/configure && "${force_reconfigure}" == "no" ]] ; then
    print_m "configure script exists."
  else
    print_m "generating configure script."
    ( cd ${repo_cache} && ./autogen.sh )
  fi

  # create build directory
  if [[ -x ${build_dir} ]] ; then
    print_m "source build directory exists."
  else
    print_m "creating source build directory."
    mkdir -pv ${build_dir}
  fi

  # generate autotools makefile
  [[ "${force_reconfigure}" == "yes" ]] && print_m "forcing configure."
  if [[ -r ${build_dir}/Makefile && "${force_reconfigure}" == "no" ]] ; then
    print_m "root Makefile exists."
  else
    print_m "generating Makefiles and configuring source."
    print_m configure_opts: [${configure_opts}]

    # path to autotools configure script depends on structure of
    # ${build_dir}  set in function update_build_variables()
    ( cd ${build_dir} && ../../../configure ${configure_opts} )
  fi

  print_m "${FUNCNAME} end"
}

#------------------------------------------------------------------------------
# run make with targets
#------------------------------------------------------------------------------
function source_make() {
  print_m "${FUNCNAME} begin"

  update_build_variables
  update_make_job_slots

  if [[ "${skip_check}" == "yes" ]] ; then
    print_m "skipping system prerequisite checks."
  else
    print_m "checking system for prerequisites."
    prerequisites_check
    prerequisites_install
  fi

  if [[ "${skip_prep}" == "yes" ]] ; then
    print_m "skipping source preparation."
  else
    print_m "preparing source."
    source_prepare
  fi

  print_m "building [$*]."
  print_m \( cd ${build_dir} \&\& make \-\-jobs=${make_job_slots} $* \)
  ( cd ${build_dir} && make --jobs=${make_job_slots} $* )

  print_m "${FUNCNAME} end"
}

#------------------------------------------------------------------------------
# create project directory and copy design flow template files
#------------------------------------------------------------------------------
function create_template() {
  print_m "${FUNCNAME} begin"

  update_prerequisite_list
  update_build_variables

  local dir_name="$1"
  local def_name_prefix="pm"

  local cmd_name="openscad-seam"
  local cmd_cache="${cache_bindir}/${cmd_name}"

  local cmd_path

  # check for openscad-amu library path in:
  #   (1) cache, and then
  #   (2) system;
  # use the first one located.
  cmd_path=$(which 2>/dev/null ${cmd_cache} ${cmd_name} | head -1)

  if [[ -x "${cmd_path}" ]] ; then
    local LIB_PATH=$(${cmd_path} --version --verbose  | grep "lib path" | awk '{print $4}')
    local LIB_VERSION=${LIB_PATH##*/}

    print_m using: LIB_PATH = ${LIB_PATH}
    print_m using: VERSION = ${LIB_VERSION}

    if [[ -z "${dir_name}" ]] ; then
      dir_name="${def_name_prefix}/${LIB_VERSION}"
    fi

    if [[ -d ${dir_name} ]] ; then
      print_m "directory: [${dir_name}] exists. not creating..."
    else
      print_m "creating project directory: [${dir_name}]."
      mkdir -pv ${dir_name}

      print_m "copying template files to: [${dir_name}]."
      for f in ${templates}
      do
        local file="${LIB_PATH}/templates/${design_flow}/$f"
        if [[ -e ${file} ]] ; then
          cp -v ${file} ${dir_name}
        else
          print_m "template file [${file}] does not exists."
        fi
      done
      if [[ -e ${dir_name}/Project_Makefile ]] ; then
        print_m "renaming project makefile."
        mv -v ${dir_name}/Project_Makefile ${dir_name}/Makefile
      fi
    fi

  else
    print_m "unable to locate openscad-amu library path. not creating..."
  fi

  print_m "${FUNCNAME} end"
}

#------------------------------------------------------------------------------
# parse command line arguments (per-branch)
#------------------------------------------------------------------------------
function parse_commands_branch() {
  print_m "${FUNCNAME} begin"

  while [[ $# -gt 0 ]]; do
      case $1 in
      --flow)
        if [[ -z "$2" ]] ; then
          print_m "syntax: ${base_name} $1 <name>"
          print_m "missing design flow name. aborting..."
          exit 1
        fi
        design_flow="$2" ; shift 1
        print_h2 "setting: design flow [${design_flow}]"
      ;;

      --skip-check)
        print_h2 "setting: skip prerequisite check"
        skip_check="yes"
      ;;
      --skip-prep)
        print_h2 "setting: skip source preparation"
        skip_prep="yes"
      ;;

      --list)
        print_h1 "Listing prerequisites"
        update_prerequisite_list
        prerequisites_list
      ;;
      --check)
        print_h1 "Checking for installed prerequisites"
        prerequisites_check
        prerequisites_status
      ;;
      --required)
        print_h1 "Installing missing prerequisites"
        update_build_variables
        prerequisites_check
        prerequisites_install
      ;;
      --yes)
        local opt="--assume-yes"
        print_h2 "adding: apt-get install option [${opt}]"
        [[ -n "${apt_get_opts}" ]] && apt_get_opts+=" ${opt}"
        [[ -z "${apt_get_opts}" ]] && apt_get_opts="${opt}"
      ;;

      -c|--cache)
        print_h2 "setting: configure source to install to cache"
        cache_install="yes"
      ;;
      --cache-root)
        if [[ -z "$2" ]] ; then
          print_m "syntax: ${base_name} $1 <path>"
          print_m "missing cache root path. aborting..."
          exit 1
        fi
        repo_cache_root="$2" ; shift 1
        print_h2 "setting: cache root path [${repo_cache_root}]"
      ;;

      -r|--reconfigure)
        print_h2 "setting: force source reconfiguration"
        force_reconfigure="yes"
      ;;

      -v|--branch)
        if [[ -z "$2" ]] ; then
          print_m "syntax: ${base_name} $1 <name>"
          print_m "missing repository branch name. aborting..."
          exit 1
        fi
        repo_branch="$2" ; shift 1
        print_h2 "setting: source branch [${repo_branch}]"
      ;;

      -b|--build)
        local targets="all"
        print_h1 "Building openscad-amu: make target=[${targets}]"
        source_make ${targets}
      ;;
      -i|--install)
        local targets="install"
        print_h1 "Building openscad-amu: make target=[${targets}]"
        source_make ${targets}
      ;;
      --installdocs)
        local targets="install-docs"
        print_h1 "Building openscad-amu: make target=[${targets}]"
        source_make ${targets}
      ;;
      -u|--uninstall)
        local targets="uninstall"
        print_h1 "Building openscad-amu: make target=[${targets}]"
        source_make ${targets}
      ;;

      -m|--make)
        if [[ -z "$2" ]] ; then
          print_m "syntax: ${base_name} $1 <name1,name2,...>"
          print_m "missing make target list. aborting..."
          exit 1
        fi
        # get list and tokenize with [,]
        local targets="${2//,/ }" ; shift 1
        print_h1 "Building openscad-amu: make target=[${targets}]"
        source_make ${targets}
      ;;

      -t|--template)
        if [[ -z "$2" ]] ; then
          print_m "syntax: ${base_name} $1 <dir>"
          print_m "missing project directory name. aborting..."
          exit 1
        fi
        local dir_name="$2" ; shift 1
        print_h1 "Creating new project template in [${dir_name}]"
        create_template ${dir_name}
      ;;
      -p|--template-def)
        print_h1 "Creating new project template with default name."
        create_template
      ;;

      --remove)
        print_h1 "Remove source build directory"
        update_build_variables
        remove_directory "${build_dir}" "source build"
      ;;

      *)
        print_m "invalid command [$1]. aborting..."
        exit 1
      ;;
      esac
      shift 1
  done

  print_m "${FUNCNAME} end"
}

#------------------------------------------------------------------------------
# parse command line arguments (per-repository)
#------------------------------------------------------------------------------
function parse_commands_repo() {
  print_m "${FUNCNAME} begin"

  local args

  while [[ $# -gt 0 ]]; do
      case $1 in
      --fetch)
        print_h1 "Updating openscad-amu source cache"
        update_build_variables
        repository_update "${repo_url}" "${repo_cache}"
      ;;

      -l|--branch-list)
        if [[ -z "$2" ]] ; then
          print_m "syntax: ${base_name} $1 <name1,name2,...>"
          print_m "missing repository branch list. aborting..."
          exit 1
        fi

        # get list and tokenize with [,]
        repo_branch_list="${2//,/ }" ; shift 1
        print_h1 "setting: branch list [${repo_branch_list}]"
      ;;

      -h|--help)
        print_help
        exit 0
      ;;
      --examples)
        print_examples
        exit 0
      ;;
      --info)
        print_info
        exit 0
      ;;
      --jobs)
        if [[ -z "$2" ]] ; then
          print_m "syntax: ${base_name} $1 <int>"
          print_m "missing job slots limit. aborting..."
          exit 1
        fi
        make_job_slots="$2" ; shift 1
        print_h2 "setting: make job slots [${make_job_slots}]"
      ;;
      --write-conf)
        write_configuration_file "${conf_file}" "${conf_file_vw}" "${conf_file_va[@]}"
        exit 0
      ;;

      # add to command argument list
      *)
        [[ -n $args ]] && args+=" $1"
        [[ -z $args ]] && args=$1
      ;;
      esac
      shift 1
  done

  if [[ -z ${repo_branch_list} ]] ; then
    # empty list, single branch for command arguments

    parse_commands_branch $args
  else
    # list specified, process command arguments for each branch

    # check for 'tags' keyword
    if [[ ${repo_branch_list:0:4} == "tags" ]] ; then
      # obtain list of tagged releases from git repository

      # force imediate clone/update of source repository (if needed)
      update_build_variables

      # check for source
      if ! ( cd ${repo_cache} 2>/dev/null && git rev-parse 2>/dev/null ) ; then
        print_m "fetching repository cache."
        repository_update "${repo_url}" "${repo_cache}"
      fi

      # check for 'tagsN', where 'N' indicates use last 'N' tags.
      local repo_branch_list_cnt=${repo_branch_list:4}

      if [[ -z ${repo_branch_list_cnt} ]] ; then
        repo_branch_list=$( cd ${repo_cache} && git tag )
        print_m tag limits = [all]
      else
        repo_branch_list=$( cd ${repo_cache} && git tag | tail -${repo_branch_list_cnt} )
        print_m tag limits = last [${repo_branch_list_cnt}]
      fi

      print_m using tag list = [${repo_branch_list}]
    fi

    # handle command set for each branch
    print_m "${FUNCNAME}.branch-list begin"
    for tag in ${repo_branch_list} ; do
      repo_branch="$tag"
      print_h2 "setting: source branch [${repo_branch}]"

      print_m $args
      parse_commands_branch $args
    done
    print_m "${FUNCNAME}.branch-list end"
  fi

  print_m "${FUNCNAME} end"
}

#==============================================================================
# help, examples, and info
#==============================================================================

#------------------------------------------------------------------------------
# help
#------------------------------------------------------------------------------
function print_help() {
print_m -j "${base_name}: (Help)" -l

cat << EOF
This script attempts to setup the openscad-amu development environment.
It downloads the source, builds, and installs the utilities. Detected
missing prerequisites are installed prior when possible. This script
may also be used to start new design projects from a template.

 [ branch options ]

      --flow <name>         : Use design flow 'name' default=(df1).

      --skip-check          : Skip system prerequisites check.
      --skip-prep           : Skip source preparation (use with care).

      --list                : List prerequisites.
      --check               : Check system for prerequisites.
      --required            : Install missing prerequisites.
      --yes                 : Automatic 'yes' to install prompts.

 -c | --cache               : Configure source to install to cache.
      --cache-root          : Set the cache root directory.

 -r | --reconfigure         : Force source autotools reconfiguration.

 -v | --branch <name>       : Use branch 'name' default=(master).

 -b | --build               : Build programs.
 -i | --install             : Install programs.
      --installdocs         : Build and install documentation.
 -u | --uninstall           : Uninstall everything.

 -m | --make <list>         : Run make with target 'list'.

 -t | --template <dir>      : Create project template in directory 'dir'.
 -p | --template-def        : Create project template in default directory.

      --remove              : Remove source build directory.

 [ repository options ]

      --fetch               : Download/update source repository cache.

 -l | --branch-list <list>  : Iterate over list of repository branches:
                              use: tags      for ALL tagged releases.
                                   tagsN     for last 'N' tagged releases.

 [ other options ]

 -h | --help                : Show this help message.
      --examples            : Show some examples uses.
      --info                : Show script information.
      --jobs <int>          : Set make job slots.
      --write-conf          : Write example configuration file.

 NOTES:
  * If used, --flow and --skip-* must be the precede all other options.
  * When changing branches, force source reconfiguration.

EOF
}

#------------------------------------------------------------------------------
# examples
#------------------------------------------------------------------------------
function print_examples() {
print_m -j "${base_name}: (Examples)" -l

cat << EOF
(1) Build and install the default source branch to the local cache. Then
    create and populate a new project with a template. Finally build and
    install the (example) project.

    $ ./setup-amu.bash --cache --install --template my_project
    $ cd ./my_project
    $ make install

    Installing the (example) project will add its documentation to the
    OpenSCAD libraries documentation table of contents which may be viewed
    with a web browser.

    To uninstall the (example) project and uninstall utilities from the
    default system location and remove the local source cache and project
    template:

    $ make uninstall
    $ cd ..
    $ rm -rf cache my_project

(2) Build and install the default source branch to the default system
    location (--fetch will update an existing source cache prior to building).

    $ ./setup-amu.bash --fetch --build
    $ sudo ./setup-amu.bash --install

    To uninstall from the default system location and remove the local
    source cache.

    $ sudo ./setup-amu.bash --uninstall
    $ rm -rf cache

(3) Build both the 'master' and 'develop' branch and install to the local
    cache, then create new project templates for each.

    $ ./setup-amu.bash \\
        --cache --branch master --install --template pt_master \\
        --reconfigure \\
        --cache --branch develop --install --template pt_develop

    When changing --branch and/or --cache on a configured source tree, use
    the --reconfigure option to force source reconfiguration.

(4) Reconfigure an existing source tree for release 'v1.6' then build and
    install to the local cache.

    $ ./setup-amu.bash --reconfigure --cache --branch v1.6 --install

(5) Build release 'v1.6' and install to the default system location.

    $ ./setup-amu.bash --branch v1.6 --build
    $ sudo ./setup-amu.bash --branch v1.6 --install

(6) Create a new project using the most recently install version.

    $ ./setup-amu.bash --template my_project

(7) Fetch updates for and reconfigure an existing source cache for latest
    develop branch then rebuild and install with documentation to the local
    cache and create a project template.

    $ ./setup-amu.bash \\
        --fetch --reconfigure --cache --branch develop \\
        --install --installdocs --template my_project

(8) Compile select tagged release versions, installing to local cache and
    creating project templates for each.

    $ ./setup-amu.bash \\
        --branch-list v1.5.1,v1.6,v1.7,v1.8.2 \\
        --reconfigure --cache --install --template-def

(9) Build and install the last six tagged releases to the default system
    location.

    $ ./setup-amu.bash --required
    $ sudo ./setup-amu.bash \\
        --branch-list tags6 --skip-check --reconfigure --install

    or in a single step:

    $ sudo ./setup-amu.bash --branch-list tags6 --reconfigure --install

    to remove everything installed in the previous step, use:

    $ sudo ./setup-amu.bash --branch-list tags6 --reconfigure --uninstall

EOF
}

#------------------------------------------------------------------------------
# info
#------------------------------------------------------------------------------
function print_info() {
print_m -j "${base_name}: (Info)" -l

cat << EOF
     package: openscad-amu
  bug report: royasutton@hotmail.com
    site url: https://royasutton.github.io/openscad-amu

EOF
}

###############################################################################
###############################################################################
##
## (main) process command line arguments.
##
###############################################################################
###############################################################################

# parse configuration file
if [[ -e ${conf_file} ]] ; then
  print_m "reading configuration file: ${conf_file}"
  parse_configuration_file "${conf_file}" "${conf_file_vw}" "${conf_file_va[@]}"
else
  print_m "configuration file ${conf_file} does not exists."
fi

# parse configuration file commands
if [[ -n "${commands}" ]] ; then
  print_m "processing configuration file commands."
  parse_commands_repo ${commands}
fi

# parse command line arguments
if [[ $# -ne 0 ]] ; then
  print_m "processing command line arguments."
  parse_commands_repo $*
fi

# show help if no command line arguments or configuration file commands
if [[ $# == 0 && -z "${commands}" ]] ; then
  print_help
fi

###############################################################################
# eof
###############################################################################
