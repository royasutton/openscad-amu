#!/bin/bash
#!/usr/bin/env bash
#/##############################################################################
#
#   \file   bootstrap.bash
#
#   \author Roy Allen Sutton <royasutton@hotmail.com>.
#   \date   2016
#
#   \copyright
#
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
# design flow: (1) prerequisite packages and (2) template files
###############################################################################

function update_design_flow_variables() {
  declare local packages_Common
  declare local packages_Linux
  declare local packages_CYGWIN_NT

  case "${design_flow}" in
    df1)
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
    *)
      print_m "Design flow [$design_flow] not supported."
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
      print_m "Configuration for [$sysname] required."
      exit 1
    ;;
  esac
}

###############################################################################
# message printing
###############################################################################

function print_m() {
  declare    local nl
  declare    local es
  declare -i local rn=1
  declare    local ws=' '
  declare    local ns="${ws}"

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
  declare local tput=$(which tput 2>/dev/null)
  declare local cols=80

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
# variables
###############################################################################

declare design_flow="df1"

declare skip_check="no"
declare apt_get_opts="--verbose-versions"

declare git_fetch_opts="--verbose"

declare repo_cache_root="cache"

declare repo_url_apt_cyg="https://github.com/transcode-open/apt-cyg"
declare repo_cache_apt_cyg
declare apt_cyg_path

declare repo_url="https://github.com/royasutton/openscad-amu"
declare repo_cache
declare repo_branch="master"

declare templates

declare packages
declare packages_installed
declare packages_missing

declare openscad_amu_builddir

declare cache_install="no"
declare cache_prefix
declare cache_bindir

declare force_reconfigure="no"
declare configure_opts
declare configure_opts_add

function update_build_variables() {
  repo_cache_apt_cyg=${repo_cache_root}/apt_cyg
  repo_cache=${repo_cache_root}/openscad-amu

  openscad_amu_builddir="${repo_cache}/build/${repo_branch}/${sysname}"

  if [[ "${cache_install}" == "yes" ]] ; then
    cache_prefix="${work_path}/${repo_cache_root}/local"
    cache_bindir="${cache_prefix}/bin/${sysname}"
    configure_opts="--prefix=${cache_prefix} --bindir=${cache_bindir}"
  fi

  if [[ -n "${configure_opts_add}" ]] ; then
    [[ -n "${configure_opts}" ]] && configure_opts+=" ${configure_opts_add}"
    [[ -z "${configure_opts}" ]] && configure_opts="${configure_opts_add}"
  fi
}

function parse_configuration() {
  declare local file="$1"
  declare local varl="$2"

  function read_key()
  {
    declare local file="$1"
    declare local key="$2"

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

  for v in  ${varl} ; do
    declare local t

    printf -v t '%s' "$(read_key ${file} $v)"
    if [[ -n "$t" ]] ; then
      printf -v $v '%s' "$t"
      print_m "setting $v=$t"
    fi
  done
}

###############################################################################
# functions
###############################################################################

#==============================================================================
# Linux
#==============================================================================

function check.Linux() {
  dpkg-query --show --showformat='${Status}\n' $1 2>/dev/null |
    grep -q "install ok installed" &&
      return 0

  return 1
}

function show.Linux() {
  dpkg-query --list $*
}

function install.Linux() {
  print_m "apt-get install options: [${apt_get_opts}]"
  if ! sudo apt-get install ${apt_get_opts} $* ; then
    print_m "install failed. aborting..."
    exit 1
  fi
}

#==============================================================================
# Cygwin
#==============================================================================

function check.CYGWIN_NT() {
  cygcheck --check-setup --dump-only $1 |
    tail -1 |
    grep -q $1 &&
      return 0

  return 1
}

function show.CYGWIN_NT() {
  cygcheck --check-setup $*
}

function install.CYGWIN_NT() {
  set_apt_cyg_path
  if ! ${apt_cyg_path} install $* ; then
    print_m "install failed. aborting..."
    exit 1
  fi
}

function set_apt_cyg_path() {
  declare local cmd_name="apt-cyg"
  declare local cmd_cache=${repo_cache_apt_cyg}/${cmd_name}

  if [[ -z "${apt_cyg_path}" ]] ; then
    apt_cyg_path=$(which 2>/dev/null ${cmd_name} ${cmd_cache} | head -1)

    if [[ -x "${apt_cyg_path}" ]] ; then
      print_m "found: ${cmd_name}=${apt_cyg_path}"
    else
      print_m "fetching apt-cyg git repository to ${repo_cache_apt_cyg}"
      update_repo "${repo_url_apt_cyg}" "${repo_cache_apt_cyg}"

      if [[ -e "${cmd_cache}" ]] ; then
        [[ ! -x "${cmd_cache}" ]] && chmod --verbose +x ${cmd_cache}
        apt_cyg_path=$(which 2>/dev/null ${cmd_name} ${cmd_cache} | head -1)
        print_m "using cached: ${cmd_name}=${apt_cyg_path}"
        print_m "adding [${apt_cyg_path%/*}] to shell path"
        PATH=${apt_cyg_path%/*}:${PATH}
      else
        print_m "unable to locate or cache ${cmd_name}. aborting..."
        exit 1
      fi
    fi
  fi
}

#==============================================================================
# System Independent
#==============================================================================

#
# update missing package list
#
function check_update() {
  packages_installed=""
  packages_missing=""

  if [[ "${skip_check}" == "yes" ]] ; then
    print_m "skipping prerequisite checks."
    return
  fi

  update_design_flow_variables
  for r in ${packages} ; do
    if check.${sysname} $r
    then
      packages_installed+=" $r"
    else
      packages_missing+=" $r"
    fi
  done
}

#
# show packages
#
function check_show_status() {
  print_h2 "[ Installed ]"
  if [[ -n "${packages_installed}" ]] ; then
    show.${sysname} $packages_installed
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
}

#
# install missing packages
#
function install_missing() {
  if [[ -n "${packages_missing}" ]] ; then
    print_h2 "[ Missing ]"
    for r in ${packages_missing} ; do
      print_m -j $r
    done

    update_build_variables
    print_h2 "[ Installing ]"
    for r in ${packages_missing} ; do
      print_h2 "installing: [ $r ]"
      print_m -j $r
      install.${sysname} $r
    done
    print_hb "="
  else
    print_m -j "no missing prerequisites."
  fi
}

#
# clone or update a Git repository
#
function update_repo() {
  declare local gitrepo="$1"
  declare local out_dir="$2"

  print_m "source: [${gitrepo}]"
  print_m " cache: [${out_dir}]"

  declare local git=$(which 2>/dev/null git)
  if [[ ! -x "${git}" ]] ; then
    print_m "please install git:"

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
      print_m "directory [${out_dir}] exists and is not a repository. aborting..."
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
    print_m "repository update failed. aborting..."
    exit 1
  fi
}

#
# remove source build directory
#
function remove_build_directory() {
  declare local builddir="$1"

  if [[ -x ${builddir} ]] ; then
    print_m "removing source build directory [${builddir}]."
    rm -rfv ${builddir}
  else
    print_m "source build directory [${builddir}] does not exists."
  fi
}

#
# prepare source for compile
#
function prep_openscad_amu() {
  print_m "${FUNCNAME} begin"

  update_build_variables

  # check missing
  check_update
  install_missing

  # check for source
  if ! ( cd ${repo_cache} 2>/dev/null && git rev-parse 2>/dev/null ) ; then
    print_m "fetching repository cache."
    update_repo "${repo_url}" "${repo_cache}"
  fi

  # checkout branch
  if ! ( cd ${repo_cache} && git checkout ${repo_branch} ) ; then
    print_m "failed to checkout branch [${repo_branch}]. aborting..."
    exit 1
  else
    print_m -n "repository branch description: "
    ( cd ${repo_cache} && git describe --tags --long --dirty )
  fi

  if [[ -x ${repo_cache}/configure && "${force_reconfigure}" == "no" ]] ; then
    print_m "configure script exists."
  else
    print_m "generating configure script."
    ( cd ${repo_cache} && ./autogen.sh )
  fi

  if [[ -x ${openscad_amu_builddir} ]] ; then
    print_m "source build directory exists."
  else
    print_m "creating source build directory."
    mkdir -pv ${openscad_amu_builddir}
  fi

  if [[ -r ${openscad_amu_builddir}/Makefile && "${force_reconfigure}" == "no" ]] ; then
    print_m "root Makefile exists."
  else
    print_m "generating Makefiles and configuring source."
    print_m configure_opts: [${configure_opts}]
    ( cd ${openscad_amu_builddir} && ../../../configure ${configure_opts} )
  fi

  print_m "${FUNCNAME} end"
}

#
# run make with targets
#
function make_openscad_amu() {
  print_m "${FUNCNAME} begin"

  prep_openscad_amu

  print_m "building [$*]."
  ( cd ${openscad_amu_builddir} && make $* )

  print_m "${FUNCNAME} end"
}

#
# create project directory and copy template files
#
function create_template() {
  print_m "${FUNCNAME} begin"

  update_design_flow_variables
  update_build_variables

  declare local dir_name="$1"
  declare local def_name_prefix="pm"
  declare local cmd_name="openscad-seam"
  declare local cmd_cache="${cache_bindir}/${cmd_name}"
  declare local cmd_path

  cmd_path=$(which 2>/dev/null ${cmd_cache} ${cmd_name} | head -1)
  if [[ -x "${cmd_path}" ]] ; then
    declare local LIB_PATH=$(${cmd_path} --version --verbose  | grep "lib path" | awk '{print $4}')
    declare local LIB_VERSION=${LIB_PATH##*/}

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
        declare local file="${LIB_PATH}/templates/${design_flow}/$f"
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
    print_m "unable to locate required command [${cmd_name}]. not creating..."
  fi

  print_m "${FUNCNAME} end"
}

#
# process commands
#
function process_commands() {
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

      --skip)
        print_h2 "setting: skip prerequisite check"
        skip_check="yes"
      ;;
      --yes)
        declare local opt="--assume-yes"
        print_h2 "adding: apt-get install option [${opt}]"
        [[ -n "${apt_get_opts}" ]] && apt_get_opts+=" ${opt}"
        [[ -z "${apt_get_opts}" ]] && apt_get_opts="${opt}"
      ;;
      --check)
        print_h1 "Checking for installed prerequisites"
        check_update
        check_show_status
      ;;
      --required)
        print_h1 "Installing missing prerequisites"
        check_update
        install_missing
      ;;
      --list)
        print_h1 "Listing prerequisites"
        update_design_flow_variables
        for r in ${packages} ; do
          print_m -j $r
        done
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
      -c|--cache)
        print_h2 "setting: configure source to install to cache"
        cache_install="yes"
      ;;

      -b|--build)
        declare local targets="all"
        print_h1 "Building openscad-amu: make target=[${targets}]"
        make_openscad_amu ${targets}
      ;;
      -i|--install)
        declare local targets="install"
        print_h1 "Building openscad-amu: make target=[${targets}]"
        make_openscad_amu ${targets}
      ;;
      --installdocs)
        declare local targets="install-docs"
        print_h1 "Building openscad-amu: make target=[${targets}]"
        make_openscad_amu ${targets}
      ;;
      -u|--uninstall)
        declare local targets="uninstall"
        print_h1 "Building openscad-amu: make target=[${targets}]"
        make_openscad_amu ${targets}
      ;;

      -m|--make)
        if [[ -z "$2" ]] ; then
          print_m "syntax: ${base_name} $1 <name>"
          print_m "missing make target name. aborting..."
          exit 1
        fi
        declare local targets="$2" ; shift 1
        print_h1 "Building openscad-amu: make target=[${targets}]"
        make_openscad_amu ${targets}
      ;;

      -t|--template)
        if [[ -z "$2" ]] ; then
          print_m "syntax: ${base_name} $1 <dir>"
          print_m "missing project directory name. aborting..."
          exit 1
        fi
        declare local dir_name="$2" ; shift 1
        print_h1 "Creating new project template in [${dir_name}]"
        create_template ${dir_name}
      ;;
      --template-def)
        print_h1 "Creating new project template with default name."
        create_template
      ;;

      --fetch)
        print_h1 "Updating openscad-amu source cache"
        update_build_variables
        update_repo "${repo_url}" "${repo_cache}"
      ;;
      --remove)
        print_h1 "Remove source build directory"
        update_build_variables
        remove_build_directory "${openscad_amu_builddir}"
      ;;

      -h|--help)
        print_help
        exit 0
      ;;
      --examples)
        print_examples
        exit 0
      ;;
      --version)
        print_version
        exit 0
      ;;

      *)
        print_m "invalid command [$1]. aborting..."
        exit 1
      ;;
      esac
      shift 1
  done
}

###############################################################################
# version, help and examples
###############################################################################
function print_help() {
print_m -j "${base_name}: (Help)" -l

cat << EOF
This script attempts to bootstrap the openscad-amu development environment.
It downloads the latest source from the development repository, builds, and
install the utilities (detected missing prerequisites are installed prior
when possible). This script may also be used to start new design projects
from a template.

      --flow <name>    : Use design flow <name> default=(df1).

      --skip           : Skip prerequisites check.
      --yes            : Automatic yes to apt-get install prompts.
      --check          : Check for installed prerequisites.
      --required       : Install missing prerequisites.
      --list           : List prerequisites.

 -r | --reconfigure    : Force source reconfiguration.
 -v | --branch <name>  : Use branch <name> default=(master).
 -c | --cache          : Configure source for cache install.

 -b | --build          : Build programs.
 -i | --install        : Install programs.
      --installdocs    : Build and install documentation.
 -u | --uninstall      : Uninstall everything.

 -m | --make <name>    : Run make with target <name>.

 -t | --template <dir> : Create new project directory with templates.
      --template-def   : Create new project/templates with default name.

      --fetch          : Update source repository cache.
      --remove         : Remove source build directory.

 -h | --help           : Show this help message.
      --examples       : Show some examples uses.
      --version        : Show version information.

 NOTES:
  * When using a flow or branch other than the default, the --flow
    and/or --branch option must be used with each command invokation
    or set in the configuration file.
  * If used, --flow and --skip must be the precede all other options.
  * If used, {--reconfigure|--branch|--cache} must precede --build
    (et al.), --install, --make, and --template.
  * When changing {--branch|--cache}, force source reconfiguration
    by prefixing --reconfigure.

EOF
}

function print_examples() {
print_m -j "${base_name}: (Examples)" -l

cat << EOF
(1) Build and install the default source branch to the local cache. Then
    create and populate a new project with a template. Finally build and
    install the (example) project.

    $ bootstrap.bash --cache --install --template my_project
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

    $ bootstrap.bash --fetch --build
    $ sudo ./bootstrap.bash --install

    To uninstall from the default system location and remove the local
    source cache.

    $ sudo ./bootstrap.bash --uninstall
    $ rm -rf cache

(3) Build both the 'master' and 'develop' branch and install to the local
    cache, then create new project templates for each.

    $ bootstrap.bash \\
        --cache --branch master --install --template pt_master \\
        --reconfigure \\
        --cache --branch develop --install --template pt_develop

    When changing --branch and/or --cache on a configured source tree, use
    the --reconfigure option to force reconfiguration of the source.

(4) Reconfigure an existing source cache for release 'v1.6' then build and
    install to the local cache.

    $ bootstrap.bash --reconfigure --cache --branch v1.6 --install

(5) Build release 'v1.6' and install to the default system location.

    $ bootstrap.bash --branch v1.6 --build
    $ ./bootstrap.bash --branch v1.6 --install

(6) Create a new project using the most recently install version.

    $ bootstrap.bash --template my_project

(7) Fetch updates for and reconfigure an existing source cache for latest
    develop branch then build and install to the local cache and create
    a project template.

    $ bootstrap.bash \\
        --fetch --reconfigure \\
        --cache --branch develop --install --template my_project

EOF
}

function print_version() {
print_m -j "${base_name}: (Version)" -l

cat << EOF
     package: openscad-amu
     version: v1.7
  bug report: royasutton@hotmail.com
    site url: https://github.com/royasutton/openscad-amu

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
  parse_configuration "${conf_file}" "
    design_flow
    skip_check
    apt_get_opts
    repo_cache_root
    repo_url
    repo_branch
    repo_url_apt_cyg
    apt_cyg_path
    configure_opts_add
    force_reconfigure
    cache_install
    git_fetch_opts
    commands
  "
fi

# parse configuration file commands
if [[ -n "${commands}" ]] ; then
  print_m "processing configuration file commands."
  process_commands ${commands}
fi

# parse command line arguments
if [[ $# -ne 0 ]] ; then
  print_m "processing command line arguments."
  process_commands $*
fi

# show help if no command line arguments or configuration file commands
if [[ $# == 0 && -z "${commands}" ]] ; then
  print_help
fi

###############################################################################
# eof
###############################################################################
