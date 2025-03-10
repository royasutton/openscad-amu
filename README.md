openscad-amu
============

> Compile automation and source-based documentation for OpenSCAD designs.

[![GPL licensed](https://img.shields.io/badge/license-GPL-blue.svg?style=flat)](https://raw.githubusercontent.com/royasutton/openscad-amu/master/COPYING)


Setup
-----

To use [openscad-amu], it needs to be installed to your development
system. Although this can be done manually, it is recommended to use
the provided setup script.


Installing
----------

The setup script can be used to bootstrap the development environment
and its dependencies. To install the prerequisites, fetch and compile
the source, and install the latest tagged version of [openscad-amu],
open a shell and type:

```bash
mkdir tmp && cd tmp
```

```bash
wget https://git.io/setup-amu.bash && chmod +x setup-amu.bash
```

```bash
./setup-amu.bash --branch-list tags1 --yes --build --sudo --install
```

The option `--yes` can be omitted if you prefer to confirm the
installation of required packages. If you don't like shortened URLs,
here is the full URL to [setup-amu.bash].

Once setup has completed, the *cache* directory can be removed.

### Options

In some cases you may wish to install several versions of the
development tools concurrently. For example, to install the last three
tagged versions, replace `tags1` with `tags3` in the prior command.

To uninstall everything that was installed by these step, repeat the
step above replacing `--install` with `--uninstall`. More help and
examples are available (see: `setup-amu.bash --help` and
`setup-amu.bash --examples`).

To build and install the [openscad-amu] documentation, type:

```bash
./setup-amu.bash --branch-list tags1 --yes --build-docs --sudo --install-docs
```
Subsequently, the compiled documentation can be viewed with:

```bash
google-chrome /usr/local/share/openscad-amu/v3.4/doc/doxygen/html/index.html
```


Project Template
----------------

To create a new project from a template, type:

```bash
./setup-amu.bash --template my_project
```

This will create a folder name *my_project* with a basic design
template and project *makefile*. The project *makefile* coordinates the
design flow and invocation of [OpenSCAD]. All openscad-amu design flow
configurations are set here. To see a menu of options, current
configurations, build, and install this example project, type:

```bash
cd my_project

make help
make info

make all
make install
```

The `make all` step will invoke OpenSCAD to compile the example design
targets, as specified by scripts embedded in the source comments, and
process the comment-embedded documentation (via [Doxygen]). The `make
install` step will install this example project to the standard
OpenSCAD user library path.

To see this documentation, type:

```bash
google-chrome `make echo-install_prefix_html`/index.html
```

To remove the installed example project documentation, type:

```bash
make uninstall
```

This template provides a basic staring point for new designs using
openscad-amu.


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

[setup-amu.bash]: https://raw.githubusercontent.com/royasutton/openscad-amu/master/share/scripts/setup-amu.bash

[openscad-amu]: https://royasutton.github.io/openscad-amu
[repository]: https://github.com/royasutton/openscad-amu
[issue]: https://github.com/royasutton/openscad-amu/issues

[OpenSCAD]: http://www.openscad.org/

[Doxygen]: http://www.doxygen.nl

[git]: http://git-scm.com/
[GitHub]: http://github.com/
[forking]: http://help.github.com/forking/
[pull requests]: https://help.github.com/articles/about-pull-requests/
