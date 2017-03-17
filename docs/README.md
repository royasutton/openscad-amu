openscad-amu
============

> A design flow with compile automation and Doxygen doc generation for OpenSCAD...

[![GPL licensed](https://img.shields.io/badge/license-GPL-blue.svg?style=flat)](https://raw.githubusercontent.com/royasutton/openscad-amu/master/COPYING)


It is a _framework_ and tools for automating the compilation of
[OpenSCAD] designs with a _pre-processor_ that allows __\*.scad__ to be
documented using [Doxygen].


Introduction
------------

[openscad-amu] is being developed to support the construction of
automated design flows and Doxygen-based documentation for [OpenSCAD]
language-based mechanical designs. It establishes a framework that
allows auxiliary documentation- and build-scripts to be embedded into
the [OpenSCAD] design source.

The build scripts are automatically extracted at compile-time to
construct [Makefiles] that automate the design flow. It provides source
code input-filters that extend [Doxygen], providing additional [special
commands] with features useful in mechanical design. The flow automates
documentation generation for each [OpenSCAD] design source with
embedded [Doxygen] content.

<p align="center">
<img src="{{ site.url }}/assets/flow_intro.svg" alt="" border="0"
     usemap="#adf.map"/>
</p>

<map name="adf.map" id="dot_inline_dotgraph_4.map">
<area shape="rect" id="node1" href="{{ site.url }}/embedding.html"
      title="Annotated\nSource\n(vehicle.scad)" alt="" coords="5,45,108,100"/>
<area shape="poly" id="node2" href="{{ site.url }}/flow.html"
      title="Automatic\nDesign\nFlow" alt="" coords="282,73,279,59,270,47,256,37,238,31,219,29,199,31,182,37,168,47,159,59,156,73,159,86,168,99,182,108,199,115,219,117,238,115,256,108,270,99,279,86"/>
<area shape="rect" id="node3" href="https://royasutton.github.io/omdl/examples/solar_mount/psptm_build_all.stl"
      title="Target (1)\n...\nTarget (96)" alt="" coords="342,5,428,60"/>
<area shape="rect" id="node4" href="https://royasutton.github.io/omdl/examples/solar_mount/index.html"
      title="Design\nLibrary\nDocumentation" alt="" coords="330,84,440,139"/>
</map>


Getting Started
---------------

[openscad-amu] incorporated two distinct but complementary features
that may be used together or independently: (1) design automation and
(2) design documentation. In either case, a design source file is
annotated with structured comments.

If you are already familiar with [Doxygen], adding basic documentation
to your [OpenSCAD] designs using [openscad-amu] is effortless. Simply
markup each of your design files with the [special commands], name each
file in the [Project Makefile], and type __make__ to generate your
documentation. You can start from a ready-made template created by the
[bootstrap], then customize as needed.

See the GitHub source [repository] for setup instructions.


Example
-------

* [A Portable solar panel tripod mount](https://royasutton.github.io/omdl/examples/solar_mount/index.html):

  Design took approximately 48 hours from concept to assembly and
  documentation using [omdl] and [openscad-amu]. Demonstrates the fully
  automated design flow. Change any design parameter, type **make
  all**, then print resulting parts. In this design, parts are
  automatically engraved with the version and part identifier using a
  simple database scheme. Design is published on
  [thingiverse](http://www.thingiverse.com/thing:2051608).


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
[Makefiles]: https://www.gnu.org/software/make

[openscad-amu]: https://royasutton.github.io/openscad-amu
[repository]: https://github.com/royasutton/openscad-amu
[issue]: https://github.com/royasutton/openscad-amu/issues
[bootstrap]: https://github.com/royasutton/openscad-amu#bootstrap
[Project Makefile]: https://github.com/royasutton/openscad-amu#project-makefile

[omdl]: https://royasutton.github.io/omdl

[OpenSCAD]: http://www.openscad.org/

[Doxygen]: http://www.stack.nl/~dimitri/doxygen/index.html
[special commands]: http://www.stack.nl/~dimitri/doxygen/manual/commands.html

[git]: http://git-scm.com/
[GitHub]: http://github.com/
[forking]: http://help.github.com/forking/
[pull requests]: https://help.github.com/articles/about-pull-requests/
