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

declare kernel=$(uname -s)
declare sysname=${kernel%%-*}

# verify minimum bash version
if [[ $BASH_VERSINFO -lt 4 ]] ; then
  echo "Bash version is $BASH_VERSION. Version 4 or greater required."
  exit 1
fi

###############################################################################
# prerequisite package lists
###############################################################################

declare packages_Common="
  doxygen
  texlive
  graphviz
  git
  autoconf
  automake
  libtool
  bash
"

declare packages_Linux="
  openscad
  texlive-latex-extra
  libboost-all-dev
  imagemagick
"

declare packages_CYGWIN_NT="
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
  libboost-devel
  ImageMagick
"

###############################################################################
# message printing
###############################################################################

function print_m()
{
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

  [[ -n $tput ]] && cols=$(${tput} cols)

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

declare git_fetch_opts="--verbose"

declare repo_cache_root="cache"

declare repo_url_apt_cyg="https://github.com/transcode-open/apt-cyg"
declare repo_cache_apt_cyg=${repo_cache_root}/apt_cyg
declare apt_cyg_path

declare repo_url="https://github.com/royasutton/openscad-amu"
declare repo_cache=${repo_cache_root}/openscad-amu
declare repo_branch="master"

declare packages
declare packages_installed
declare packages_missing

case "${sysname}" in
  Linux)        packages="${packages_Common} ${packages_Linux}"           ;;
  CYGWIN_NT)    packages="${packages_Common} ${packages_CYGWIN_NT}"       ;;
  *)            print_m "Configuration for [$sysname] required." ; exit 1 ;;
esac

declare openscad_amu_builddir

declare cache_install="no"
declare cache_prefix
declare cache_bindir

declare force_reconfigure="no"
declare configure_opts

function update_cache_paths() {
  openscad_amu_builddir="${repo_cache}/build/${repo_branch}/${sysname}"

  if [[ ${cache_install} == "yes" ]] ; then
    cache_prefix="${work_path}/${repo_cache_root}/local"
    cache_bindir="${cache_prefix}/bin/${sysname}"
    configure_opts="--prefix=${cache_prefix} --bindir=${cache_bindir}"

    print_m configure_opts: [${configure_opts}]
  fi
}

###############################################################################
# functions
###############################################################################

#==============================================================================
# Linux
#==============================================================================

function check.Linux() {
  dpkg --list $1 &> /dev/null && return 0
  return 1
}

function show.Linux() {
  dpkg --list $*
}

function install.Linux() {
  if ! sudo apt-get install $* ; then
    print_m "install failed. aborting..."
    exit 1
  fi
}

#==============================================================================
# Cygwin
#==============================================================================

function check.CYGWIN_NT() {
  cygcheck --check-setup --dump-only $1 | tail -1 | grep -q $1 && return 0
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

  if [[ -z ${apt_cyg_path} ]] ; then
    apt_cyg_path=$(which 2>/dev/null ${cmd_name} ${cmd_cache} | head -1)

    if [[ -x "${apt_cyg_path}" ]] ; then
      print_m "found: ${cmd_name}=${apt_cyg_path}"
    else
      print_m "fetching apt-cyg git repository to ${repo_cache_apt_cyg}"
      update_repo ${repo_url_apt_cyg} ${repo_cache_apt_cyg}

      if [[ -e "${cmd_cache}" ]] ; then
        [[ ! -x "${cmd_cache}" ]] && chmod --verbose +x ${cmd_cache}
        apt_cyg_path=$(which 2>/dev/null ${cmd_name} ${cmd_cache} | head -1)
        print_m "using cached: ${cmd_name}=${apt_cyg_path}"
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
  show.${sysname} $packages_installed

  print_h2 "[ Missing ]"
  if [[ -n ${packages_missing} ]] ; then
    for r in ${packages_missing} ; do
      print_m -j $r
    done
  else
    print_m -j "no missing prerequisites."
  fi
  print_hb "="
}

#
# install missing packages
#
function install_missing() {
  if [[ -n ${packages_missing} ]] ; then
    print_h2 "[ Missing ]"
    for r in ${packages_missing} ; do
      print_m -j $r
    done

    print_h2 "[ Installing ]"
    install.${sysname} ${packages_missing}
    print_hb "="
  else
    print_m -j "no missing prerequisites."
  fi
}

#
# clone or update a Git repository
#
function update_repo() {
  declare local gitrepo=$1
  declare local out_dir=$2

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
      print_m "updating: Git repository [${gitrepo}]"
      ( cd ${out_dir} && ${git} pull ${git_fetch_opts} )
    else
      print_m "target directory [${out_dir}] exists and is not a repository. aborting..."
      exit 1
    fi
  else
    print_m "cloning: Git repository [${gitrepo}]"
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
  print_m "${FUNCNAME} begin"

  update_cache_paths

  if [[ -x ${openscad_amu_builddir} ]] ; then
    print_m "removing source build directory."
    rm -rfv ${openscad_amu_builddir}
  else
    print_m "source build directory [${openscad_amu_builddir}] does not exists."
  fi

  print_m "${FUNCNAME} end"
}

#
# prepare source for compile
#
function prep_openscad_amu() {
  print_m "${FUNCNAME} begin"

  update_cache_paths

  # check missing
  check_update
  install_missing

  # check for source
  if ! ( cd ${repo_cache} 2>/dev/null && git rev-parse 2>/dev/null ) ; then
    print_m "fetching repository cache."
    update_repo ${repo_url} ${repo_cache}
  fi

  # checkout branch
  if ! ( cd ${repo_cache} && git checkout ${repo_branch} ) ; then
    echo "good"
    print_m "failed to checkout branch [${repo_branch}]. aborting..."
    exit 1
  fi

  if [[ -x ${repo_cache}/configure && ${force_reconfigure} == "no" ]] ; then
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

  if [[ -r ${openscad_amu_builddir}/Makefile && ${force_reconfigure} == "no" ]] ; then
    print_m "root Makefile exists."
  else
    print_m "generating Makefiles and configuring source."
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

  update_cache_paths

  declare local dirname="$1"
  declare local cmd_name="openscad-seam"
  declare local cmd_cache="${cache_bindir}/${cmd_name}"
  declare local cmd_path

  cmd_path=$(which 2>/dev/null ${cmd_cache} ${cmd_name} | head -1)
  if [[ -x "${cmd_path}" ]] ; then
    declare local LIB_PATH=$(${cmd_path} --version --verbose  | grep "lib path" | awk '{print $4}')
    print_m using: LIB_PATH = ${LIB_PATH}

    if [[ -d ${dirname} ]] ; then
      print_m "directory: [${dirname}] exists. not creating..."
    else
      print_m "creating project directory: [${dirname}]."
      mkdir -v ${dirname}

      print_m "copying template files to: [${dirname}]."
      cp -v ${LIB_PATH}/templates/df1/{Doxyfile,design.scad,library.scad} ${dirname}
      cp -v ${LIB_PATH}/templates/df1/Project_Makefile ${dirname}/Makefile
    fi

  else
    print_m "unable to locate required command [${cmd_name}]. not creating..."
  fi

  print_m "${FUNCNAME} end"
}

###############################################################################
# help and examples
###############################################################################
function print_help() {
print_m -j "${base_name}: (Help)" -l

cat << EOF
This script attempts to bootstrap the openscad-amu development environment.
It downloads the latest source from the development repository, builds, and
install the utilities. Detected missing prerequisites are installed prior
when possible. This script may also be used to start new design projects
from a template.

      --check          : Check for installed prerequisites.
      --required       : Install missing prerequisites.
      --list           : List prerequisites.

 -r | --reconfigure    : Force source reconfiguration.
 -v | --branch <name>  : Use branch <name> default=(master)
 -c | --cache          : Configure source for cache install.

 -b | --build          : Build all.
      --programs       : Build programs.
      --docs           : Build documentation.
      --tests          : Build tests.

 -i | --install        : Install openscad-amu.
 -u | --uninstall      : Uninstall openscad-amu.

 -t | --template <dir> : Create new project directory with templates.

      --fetch          : Update source repository cache.
      --remove         : Remove source build directory.

 -h | --help           : Show this help message.
      --examples       : Show some examples uses.

 NOTES:
  * When using a branch other than the default, the --branch option
    must be used with each command invokation.
  * If used, {--branch|--cache|--reconfigure} must precede --build
    (et al.), --install, and --template.
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

    $ bootstrap.bash --fetch --programs
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

    $ bootstrap.bash --branch v1.6 --programs
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

###############################################################################
###############################################################################
##
## (main) process command line arguments.
##
###############################################################################
###############################################################################

# show help if no arguments
if [[ $# == 0 ]] ; then
  print_help
  exit 0
fi

# parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
    --check)
      print_h1 "Prerequisite Check"
      check_update
      check_show_status
    ;;
    --required)
      print_h1 "Install Missing"
      check_update
      install_missing
    ;;
    --list)
      print_h1 "Prerequisite List"
      for r in ${packages} ; do
        print_m -j $r
      done
    ;;

    -r|--reconfigure)
      print_h2 "forcing source reconfigure"
      force_reconfigure="yes"
    ;;
    -v|--branch)
      if [[ -z "$2" ]] ; then
        print_m "syntax: ${base_name} $1 <name>"
        print_m "missing repository branch name. aborting..."
        exit 1
      fi
      repo_branch="$2"
      print_h2 "compiling source branch [${repo_branch}]"
      shift 1
    ;;
    -c|--cache)
      print_h2 "configure source for [cache install]"
      cache_install="yes"
    ;;

    -b|--build)
      declare local targets="all docs tests"
      print_h1 "Building openscad-amu [${targets}]"
      make_openscad_amu ${targets}
    ;;
    --programs)
      declare local targets="all"
      print_h1 "Building openscad-amu [${targets}]"
      make_openscad_amu ${targets}
    ;;
    --docs)
      declare local targets="docs"
      print_h1 "Building openscad-amu [${targets}]"
      make_openscad_amu ${targets}
    ;;
    --tests)
      declare local targets="tests"
      print_h1 "Building openscad-amu [${targets}]"
      make_openscad_amu ${targets}
    ;;

    -i|--install)
      declare local targets="install"
      print_h1 "Building openscad-amu [${targets}]"
      make_openscad_amu ${targets}
    ;;
    -u|--uninstall)
      declare local targets="uninstall"
      print_h1 "Building openscad-amu [${targets}]"
      make_openscad_amu ${targets}
    ;;

    -t|--template)
      if [[ -z "$2" ]] ; then
        print_m "syntax: ${base_name} $1 <dir>"
        print_m "missing project directory name. aborting..."
        exit 1
      fi
      declare local dirname="$2"
      shift 1

      print_h1 "Creating new project template in [${dirname}]"
      create_template ${dirname}
    ;;

    --fetch)
      print_h1 "Updating openscad-amu source cache"
      update_repo ${repo_url} ${repo_cache}
    ;;
    --remove)
      print_h1 "Remove source build directory"
      remove_build_directory
    ;;

    -h|--help)
      print_help
      exit 0
    ;;
    --examples)
      print_examples
      exit 0
    ;;

    *)
      print_m "invalid command line option at [$1]. aborting..."
      exit 1
    ;;
    esac
    shift 1
done

###############################################################################
# eof
###############################################################################
