openscad-amu
============

> Utilities for build scripting and documenting OpenSCAD designs...

[![GPL licensed](https://img.shields.io/badge/license-GPL-blue.svg?style=flat)](https://github.com/royasutton/openscad-amu/blob/master/COPYING)


OpenSCAD Automake Utilities (__openscad-amu__) provide a _framework_
for scripting the compilation of OpenSCAD designs and a source code
_pre-processor_ that allows __*.scad__ (and __*.bash__) to be documented
using Doxygen.


Downloading
-----------

You can find official source releases of the utilities at:

    https://github.com/royasutton/openscad-amu/releases

If you want the latest code from the development branch of the project,
you can follow these steps for cloning the project repository:

    $ git clone https://github.com/royasutton/openscad-amu
    $ cd openscad-amu
    $ git checkout develop


Introduction
------------

[openscad-amu] has been developed to assist in the automated compilation
and documentation markup of OpenSCAD language-based mechanical designs.

It establishes a framework that allows auxiliary scripts to be written in
structured comment blocks of __*.scad__ source code which are used to
construct *Makefiles* that automate design compilation using [OpenSCAD].

With design documentation, it seems natural to leverage existing tools
that extract code documentation from annotated sources. Inasmuch,
openscad-amu provides a __*.scad__ source code input-filter that may be
used in conjunction with the widely used [Doxygen].


Building and Installing
-----------------------

To initialize the (GNU autotools) configuration script issue:

    $ ./autogen.sh

It is recommended, but not required, that you build openscad-amu in a
separate directory apart from the repository source. To configure and
build using the default install prefix issue:

    $ mkdir -p build
    $ cd build
    $ ../configure
    $ make

To runs a few basic post-build sanity checks in the tests sub-directory
issue:

    $ make tests

The openscad-amu documentation can be built and viewed by:

    $ make docs
    $ firefox share/lib/doxygen/html/index.html

To install or uninstall openscad-amu, issue:

    $ make install
    $ make uninstall


Project Template
----------------

Project templates are installed to the default library directory. To get
started on a new project, copy the template files to a working directory
and modify them as needed. The default library directory can be found
by issuing the command:

    $ openscad-seam --version --verbose

To create a working copy assign LIB_PATH to the library directory reported
from the above step:

    $ LIB_PATH=`openscad-seam --version --verbose  | grep "lib path" | awk '{print $4}'`
    $ echo ${LIB_PATH}

Then follow these steps:

    $ mkdir project_name
    $ cd project_name
    $ cp ${LIB_PATH}/templates/df1/{Doxyfile,design.scad,library.scad} .
    $ cp ${LIB_PATH}/templates/df1/Project_Makefile Makefile

    $ unset LIB_PATH

To see a menu of makefile target options:

    $ make help


Selecting a Release
-------------------

By convention, the *master* branch of the repository will normally be
tagged with the most recent *release version* of the source code and
the *develop* branch is were new development changes take place.

When the the development branch reaches a stable point and is ready for
release it is merged back into into the master branch and tagged with a
new version.

To checkout and work with a specific version, for example release v1.3,
the following can be used:

    $ git tag -l
    $ git checkout tags/v1.3
    $ git describe


Contributing
------------

openscad-amu uses [git] for development tracking, and is hosted on [GitHub]
following the usual practice of [forking] and submitting [pull requests]
to the source repository.

As it is released under the [GNU General Public License], any file you
change should bear your copyright notice alongside the original authors'
copyright notices typically located at the top of each file.


Contact and Support
-------------------

In case you have any questions or would like to make feature requests,
you can contact the maintainer of the project below or file an issue
at: https://github.com/royasutton/openscad-amu/issues.

You can reach the project maintainer through:

    Roy Allen Sutton <royasutton@hotmail.com>


[GNU General Public License]: https://www.gnu.org/licenses/gpl.html
[OpenSCAD]: http://www.openscad.org/
[Doxygen]: http://www.stack.nl/~dimitri/doxygen/index.html
[openscad-amu]: https://github.com/royasutton/openscad-amu
[git]: http://git-scm.com/
[GitHub]: http://github.com/
[forking]: http://help.github.com/forking/
[pull requests]: http://help.github.com/pull-requests/
