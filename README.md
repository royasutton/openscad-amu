openscad-amu
============

> Utilities for build scripting and documenting OpenSCAD designs...

[![GPL licensed](https://img.shields.io/badge/license-GPL-blue.svg?style=flat)](https://raw.githubusercontent.com/royasutton/openscad-amu/master/COPYING)


OpenSCAD Automake Utilities (__openscad-amu__) provide a _framework_
for scripting the compilation of [OpenSCAD] designs and a source code
_pre-processor_ that allows __\*.scad__ and (__\*.bash__) to be documented
using [Doxygen].

View live docs on [GitHib Pages](https://royasutton.github.io/openscad-amu).


Bootstrap
---------

The source includes a script that may be used to setup the development
environment. This script is normally built with the source, however, a
pre-built snapshot is available in the root directory of the
repository.

    $ mkdir tmp && cd tmp

    $ wget https://raw.githubusercontent.com/royasutton/openscad-amu/master/snapshots/bootstrap.{bash.in,conf} .
    $ chmod +x bootstrap.bash

To install the prerequisites, fetch and compile the source, install
*openscad-amu* to a cache in the current directory, and create a
project template type:

    $ ./bootstrap.bash --cache --yes --install --template my_project

The argument *--yes* can be omitted if you prefer to confirm the
installation of each package individually (help is available:
*bootstrap.bash --help*).

If all goes well, you will end up with two new directories: *cache* and
*my_project*. The source will have been compiled and installed to
*cache* and a template, with a *project makefile*, will have been
copied to *my_project*.


Project Makefile
----------------

The project makefile coordinates the design flow. All configurable
aspects of the design flow are set in this control file. To see a menu
of targets:

    $ cd my_project
    $ make help

To build and install the template example (in *my_project*):

    $ cd my_project
    $ make install

This will generate all targets described in the source scopes, build
the documentation, and will install the library files to the
system-dependent [OpenSCAD] user library path. In addition, the
documentation will be installed and added to an index that can be
viewed with a web browser.

    $ make print-install_prefix_html

    $ firefox ${HOME}/.local/share/OpenSCAD/docs/html/index.html  # on Linux
    $ firefox `cygpath --mydocs`/OpenSCAD/docs/html/index.html    # on Cygwin

To remove the installed library and documentation, type:

    $ make uninstall


The Repository
--------------

To clone the repository and work with the _master_ source branch:

    $ git clone https://github.com/royasutton/openscad-amu
    $ cd openscad-amu

To work with the latest development code from the _develop_ branch:

    $ git checkout develop


Building the Source
-------------------

The *bootstrap.bash* script is the most simple way to build the source.
However, here are the steps to build it manually. When all of the
prerequisite packages exists, the development source may be compiled
by:

    $ git clone https://github.com/royasutton/openscad-amu
    $ cd openscad-amu
    $ git checkout master
    $ ./autogen.sh
    $ mkdir -p build && cd build
    $ ../configure
    $ make

To runs post-build sanity checks:

    $ make check

The openscad-amu html documentation can be built and viewed by:

    $ make html
    $ firefox share/lib/doxygen/html/index.html

To install or uninstall openscad-amu, issue:

    $ sudo make install
    $ sudo make uninstall


Release Selection
-----------------

By convention, the *master* branch of the repository will normally be
tagged with the most recent *release version* of the source code and
the *develop* branch is where new development changes take place. When
the development branch reaches a stable point and is ready for release
it is merged back into the master branch and tagged with a new version.

To checkout and work with a specific version, for example release v1.6,
the following can be used:

    $ git tag -l
    $ git checkout tags/v1.6
    $ git describe


Contributing
------------

openscad-amu uses [git] for development tracking, and is hosted on
[GitHub] following the usual practice of [forking] and submitting
[pull requests] to the source [repository].

As it is released under the [GNU General Public License], any file you
change should bear your copyright notice alongside the original
authors' copyright notices typically located at the top of each file.


Contact and Support
-------------------

In case you have any questions or would like to make feature requests,
you can contact the maintainer of the project or file an [issue].


[GNU General Public License]: https://www.gnu.org/licenses/gpl.html

[openscad-amu]: https://royasutton.github.io/openscad-amu
[repository]: https://github.com/royasutton/openscad-amu
[issue]: https://github.com/royasutton/openscad-amu/issues

[OpenSCAD]: http://www.openscad.org/

[Doxygen]: http://www.stack.nl/~dimitri/doxygen/index.html

[git]: http://git-scm.com/
[GitHub]: http://github.com/
[forking]: http://help.github.com/forking/
[pull requests]: https://help.github.com/articles/about-pull-requests/
