openscad-amu
============

> Utilities for build scripting and documenting OpenSCAD designs...

[![GPL licensed](https://img.shields.io/badge/license-GPL-blue.svg?style=flat)](https://raw.githubusercontent.com/royasutton/openscad-amu/master/COPYING)


Its a _framework_ for scripting the compilation of [OpenSCAD]
designs and a source code _pre-processor_ that allows __\*.scad__
(and __\*.bash__) to be documented using [Doxygen].


Introduction
------------

[openscad-amu] is being developed to support the construction of
automated design flows and documentation generation for [OpenSCAD]
language-based mechanical designs.

It establishes a framework that allows auxiliary scripts to be embedded
into the design source which are used to construct Makefiles that
control the design flow.

As for documentation, [openscad-amu] provides a __\*.scad__ source code
input-filter that may be used in conjunction with the widely used
[Doxygen]. It also provides additional [Doxygen] <em>special
commands</em> that are useful in documenting mechanical designs.


Using
-----

Please see the GitHub source [repository] for instructions on setup and
use.


API
---


Example
-------


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
