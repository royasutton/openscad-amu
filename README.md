openscad-amu
============

> A design flow with compile automation and Doxygen doc generation for OpenSCAD...

[![GPL licensed](https://img.shields.io/badge/license-GPL-blue.svg?style=flat)](https://raw.githubusercontent.com/royasutton/openscad-amu/master/COPYING)


It is a _framework_ and tools for automating the compilation of
[OpenSCAD] designs with a _pre-processor_ that allows __\*.scad__ to be
documented using [Doxygen].

View live docs on [GitHib Pages](https://royasutton.github.io/openscad-amu).


Evaluation
----------

A script is available to bootstrap the development environment. To
install the prerequisites, fetch and compile the source, install
[openscad-amu] to a temporary cache directory, and create an example
project, type:

    $ mkdir tmp && cd tmp
    $ wget https://raw.githubusercontent.com/royasutton/openscad-amu/develop/share/scripts/setup-amu.bash
    $ chmod +x setup-amu.bash

    $ ./setup-amu.bash --cache --yes --install --template my_project

The option `--yes` can be omitted if you prefer to confirm the
installation of each required package (see: `setup-amu.bash --help`).

If all goes well, you will end up with two new directories: *cache* and
*my_project*. The source will have been compiled and installed to
*cache* and an example project, along with the *project makefile*, will
have been copied to *my_project*.


### Example Project

The project *makefile* coordinates the design flow. All [openscad-amu]
design flow configurations are set here. To see a menu of options,
current configurations, and build and install this example:

    $ cd my_project

    $ make help
    $ make info
    $ make all
    $ make install

The third `make` step will compile (via [OpenSCAD]) numerous targets
guided by scripts embedded in source comments, process the embedded
documentation (via [Doxygen]), and the last will install the library
files to the system-dependent [OpenSCAD] user library path. In
addition, the documentation will be added to an index of installed
design libraries.

To see the system-dependent location of and view the index, type:

    $ make print-install_prefix_html
    $ firefox <install_prefix_html>/index.html

To remove this example, type:

    $ make uninstall

This example may be used as a *template* and is a good starting point
for new designs using [openscad-amu].


Installing
----------

To pull repository updates for an existing source tree and install the
latest release of [openscad-amu] on a system for [OpenSCAD] design
development, type:

    $ ./setup-amu.bash --fetch
    $ sudo ./setup-amu.bash --branch-list tags1 --reconfigure --install

In some cases you may wish to install several versions of the
development tools concurrently. To install the last six release
versions, replace `tags1` with `tags6` in the prior command.

To uninstall everything that was installed by the prior step, rerun
the step replacing `--install` with `--uninstall`. More help and
examples are available (see: `setup-amu.bash --help` and
`setup-amu.bash --examples`).


Manual Compilation
------------------

The `setup-amu.bash` script is the easiest way to build and install the
source. However, here are the steps to build it manually. Once all of
the prerequisite packages exists (see: `setup-amu.bash --list` and
`setup-amu.bash --required`), the development source can be compiled:

    $ git clone https://github.com/royasutton/openscad-amu
    $ cd openscad-amu
    $ git tag
    $ git checkout <version>
    $ ./autogen.sh
    $ mkdir -p build && cd build
    $ ../configure
    $ make

To run post-build sanity checks:

    $ make check

The openscad-amu html documentation is lean, but can be built and
viewed by:

    $ make html
    $ firefox share/lib/doxygen/html/index.html

To manually install or uninstall, issue:

    $ sudo make install
    $ sudo make uninstall


Release Convention
------------------

The *master* branch of the repository will be tagged with the most
recent *release version* of the source and the *develop* branch is
where new development changes take place. When the development branch
reaches a stable point and is ready for release it is merged back into
the master branch and tagged with a new version.


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
