The Design Flow
---------------

<p align="center">
<img src="{{ site.url }}/assets/flow.svg" alt="" border="0"/>
</p>

[openscad-amu] brings together [OpenSCAD], [Doxygen], [GNU Make], and
_custom scripting_ (based on [GNU Bash]) to automate the generation of
the design documentation and design targets.

A project includes a Project Makefile, the project source files, and a
documentation configuration file (Doxyfile). Any source file may be
annotated with documentation and/or one or more automation scripts.
Each script is extracted at compile time to generate a _scope_ makefile
that controls the generation of corresponding targets.

[openscad-amu]: https://royasutton.github.io/openscad-amu
[OpenSCAD]: http://www.openscad.org
[Doxygen]: http://www.stack.nl/~dimitri/doxygen
[GNU Make]: https://www.gnu.org/software/make
[GNU Bash]: https://www.gnu.org/software/bash
