openscad-amu
============

> Compile automation and source-based documentation for OpenSCAD designs.

[![GPL licensed](https://img.shields.io/badge/license-GPL-blue.svg?style=flat)](https://raw.githubusercontent.com/royasutton/openscad-amu/master/COPYING)


Evaluation
----------

A script is available to bootstrap the development environment and its
dependencies. To install the prerequisites, fetch and compile the
source, install the latest tagged version of [openscad-amu] to a
temporary cache directory, and create an example project, type:

```bash
mkdir tmp && cd tmp
```

```bash
wget https://git.io/setup-amu.bash && chmod +x setup-amu.bash
```

```bash
./setup-amu.bash --cache --branch-list tags1 --yes --install --template my_project
```

The option `--yes` can be omitted if you prefer to confirm the
installation of each required package (see: `setup-amu.bash --help`).
If you don't like shortened URLs, here is the full URL to
[setup-amu.bash].

Once setup is complete, you will see two new directories: *cache* and
*my_project*. The source will have been compiled and installed to
*cache* and an example project, along with the *project makefile*, will
have been copied to *my_project*.


### Example Project

The project *makefile* coordinates the design flow. All openscad-amu
design flow configurations are set here. To see a menu of options,
current configurations, and build and install this example:

```bash
cd my_project

make help
make info

make all
make install
```

The `make all` step will compile (via [OpenSCAD]) the example design
targets, as specified by scripts embedded in the source comments, and
process the comment-embedded documentation (via [Doxygen]). The `make
install` step will install this example project to the system-dependent
OpenSCAD user library path.

To see a table of contents and this installed example, type:

```bash
firefox `make echo-install_prefix_html`/index.html
```

To remove the installed example project, type:

```bash
make uninstall
```

This example may be used as a *template* and is a good starting point
for new designs using openscad-amu.


Installing
----------

Download or update the source repository:

```bash
./setup-amu.bash --fetch
```

Install the latest release version of openscad-amu to a development
system:

```bash
sudo ./setup-amu.bash --branch-list tags1 --reconfigure --install
```

In some cases you may wish to install several versions of the
development tools concurrently. To install the last three (for example)
release versions, replace `tags1` with `tags3` in the prior command.

To uninstall everything that was installed by these step, repeat the
step above replacing `--install` with `--uninstall`. More help and
examples are available (see: `setup-amu.bash --help` and
`setup-amu.bash --examples`).


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
