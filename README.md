openscad-amu
============

Authors
-------
Roy Allen Sutton <royasutton@hotmail.com>

Date: 2016-05-10


Introduction
------------

OpenSCAD Automake Utilities
([openscad-amu] (https://github.com/royasutton/openscad-amu))
are a collection of programs, a script library written in
[bash] (https://www.gnu.org/software/bash/), and scripts
to automate the invocation of [OpenSCAD] (http://www.openscad.org/).

OpenSCAD currently lacks a scripting interface and openscad-amu attempts
to bridge this gap by offering basic scripting capabilities via the
OpenSCAD command line interface. Openscad-amu also aims to provide a
framework for documenting OpenSCAD design scripts and, more importantly,
OpenSCAD library CAD models using [Doxygen]
(http://www.stack.nl/~dimitri/doxygen/index.html).


Install
-------

To make and install using the default prefix issue:

$ autogen.sh

$ configure && make

$ make install

To make the documentation:

$ make docs

To view the resulting docs:

$ firefox share/lib/doxygen/html/index.html

To run the tests:

$ make tests

to uninstall issue:

$ make uninstall
