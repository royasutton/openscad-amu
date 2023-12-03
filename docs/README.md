> Compile automation and source-based documentation for OpenSCAD designs.

[![GPL licensed](https://img.shields.io/badge/license-GPL-blue.svg?style=flat)](https://raw.githubusercontent.com/royasutton/openscad-amu/master/COPYING)


What is it?
-----------

The [openscad-amu] has been developed to support the construction of
automated design flows, using [GNU make], and documentation generation,
using [Doxygen], for [OpenSCAD] designs. It provides a framework that
allows for documentation and build-scripts to be embedded in
__\*.scad__ source along side the design. The openscad-amu is composed
of a collection of programs, a makefile script library, and support
scripts that work together to automate and document OpenSCAD designs.

The design documentation is generated using Doxygen and openscad-amu
includes a source-code _preprocessor_ for __\*.scad__ (and
__\*.bash__). This preprocessor provides additional Doxygen [special
commands] and features useful for OpenSCAD design documentation. The
design automation build-scripts, embedded in the source-code, are
extracted by openscad-amu and used to construct makefiles that manage
the generation of design targets.

By utilizing openscad-amu, OpenSCAD design documentation lives close to
to the source code and dependency-based targets generation is scripted
for use in code testing, code documentation and design construction.
Moreover, with GNU make, targets can be constructed in parallel which
drastically reduces compile times for larger designs.


The Design Flow
---------------

Both the design flow automation scripting and documentation is done in
the design __\*.scad__ source files using structured comments. This
meta-data is extracted at compile-time to construct makefiles that
manage design compilation and documentation.

<p align="center">
<img src="assets/flow_intro.svg" alt="" title="<active image map>"
     border="0" usemap="#adf.map"/>
</p>

<map name="adf.map" id="dot_inline_dotgraph_4.map">
<area shape="rect" id="node1" alt=""
      href="embedding.html"
      title="Annotated Design Source"
      coords="5,45,108,100"/>
<area shape="poly" id="node2" alt=""
      href="flow.html"
      title="Automated Design Flow"
      coords="282,73,279,59,270,47,256,37,238,31,219,29,199,31,182,37,168,47,159,59,156,73,159,86,168,99,182,108,199,115,219,117,238,115,256,108,270,99,279,86"/>
<area shape="rect" id="node3" alt=""
      href="http://www.thingiverse.com/thing:2051608"
      title="Dependency-based Target Generation"
      coords="342,5,428,60"/>
<area shape="rect" id="node4" alt=""
      href="http://www.thingiverse.com/thing:2051608"
      title="Design Documentation Set"
      coords="330,84,440,139"/>
</map>

As the design size increases, so does the benefit of using
[openscad-amu]. Compiling design targets one-by-one is times consuming
and error prone and discourages design optimization and/or exploration.

> *For larger design projects, the increase in benefit is significant as
> [openscad-amu] frees designers from mundane dependency and state
> tracking and provides mechanisms for part compilation and coherent
> design documentation.*

A significant side effect is greatly reduce total project compile time
on multi-processor systems via parallel invocations of the
single-threaded [OpenSCAD] compiler. Once compilation flows are
described, using the simple scripting scheme, each design target is
kept current from source as needed via invocations of `make`.

> *For smaller projects, the ability to document a design or library using
> [Doxygen] is a convenient way to both maintain and publish the design
> interface or API.*


Getting Started
---------------

If you are already familiar with [Doxygen], adding basic documentation
to your [OpenSCAD] designs using [openscad-amu] is straight forward.
Simply markup each of your design files with the [special commands],
name each file in the project makefile, and type `make` to generate
your documentation.

You can also start from a template created by the `setup-amu.bash`
script, then customize as needed as described in the [evaluation]
section of the repository home page.

### Setup ###

See the GitHub source [repository] for instructions on
[evaluating][evaluation] and [installing].


Example
-------

* [A Portable solar panel tripod mount](http://www.thingiverse.com/thing:2051608):

  This design took approximately 48 hours from concept to assembly and
  documentation using [omdl] and [openscad-amu]. It demonstrates the
  automated design flow. One can change a design parameter, then type
  `make all` to recompile effected parts. In this design, parts are
  engraved with a version and part identifier using a simple database
  scheme available in [omdl].


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
[GNU Make]: https://www.gnu.org/software/make

[openscad-amu]: https://royasutton.github.io/openscad-amu
[repository]: https://github.com/royasutton/openscad-amu
[issue]: https://github.com/royasutton/openscad-amu/issues

[evaluation]: https://github.com/royasutton/openscad-amu#evaluation
[installing]: https://github.com/royasutton/openscad-amu#installing

[omdl]: https://royasutton.github.io/omdl

[OpenSCAD]: http://www.openscad.org/

[Doxygen]: http://www.doxygen.nl
[special commands]: http://www.doxygen.nl/manual/commands.html

[git]: http://git-scm.com/
[GitHub]: http://github.com/
[forking]: http://help.github.com/forking/
[pull requests]: https://help.github.com/articles/about-pull-requests/
