_ViFi file management tool_

# Installation

## Platforms and Compatibility

ViFi should be compatible with most unixoid systems - it needs a simple bourne
shell (`/bin/sh`) for its wrapper script and the Boost library for filesystem
operations in the main program.
The dependency on the Boost library will eventually be dropped for the almost
identical implementation in the C++17 standard library (`std::filesystem`).
> Beware that ViFi is still beta software, primarily tested on FreeBSD.

## Build

Building ViFi requires
* CMake for the setup of the build recipes.
* ninja or make for build execution.
* A C++14 standard compatible compiler.
* Boost libraries for filesystem operations.

CMake options include
* `BUILD_TESTS` - builds self tests which require the Google C++ test library,
* `BUILD_DOCUMENTATION` - creates a `doc` build target which requires Doxygen.

These options are set automatically if the Google test library or Doxygen is
found. You may want to explicitly turn them `OFF` for package builds.
For a tryout I would suggest a user local instead of a system wide install,
you can set `CMAKE_INSTALL_PREFIX` to a target directory in your home.

A typical build would go like this:

    <joe@work~> mkdir Build && cd Build
    <joe@work~/Build> cmake -G Ninja -D CMAKE_INSTALL_PREFIX:PATH=/home/joe/Install ../Path/To/Source
    ...
    <joe@work~/Build> ninja
    ...


## Install

CMake creates an `install` target. It will install the `ViFi` main program and
the `vifi` shell script that is invoked by the user. These are placed in a
`bin` directory under the installation path (see CMake documentation and
`CMAKE_INSTALL_PREFIX`). Installation is simply a matter of

    <joe@work~/Build> ninja install

in the build directory. Don't forget to include the installation directory in
the `$PATH` variable of your shell if necessary. Also make sure that the
`$EDITOR` variable is exported when using ViFi. These variables are typically
set in the `.*shrc` file of the shell.

Enjoy!


# Platform Notes

## FreeBSD

Prerequisites:

    root@work:~ # pkg install ninja cmake boost-libs

FreeBSD promotes the `$HOME/bin` directory for user installed software, and
includes it in `$PATH` by default. That would thus be
`CMAKE_INSTALL_PREFIX:PATH=/home/joe` as CMake argument in the example above.

If left unchanged, `$EDITOR` points to one of the basic editors found in
FreeBSD base.

## Ubuntu

Prerequisites:

    <joe@work~> sudo apt-get install ninja-build cmake libboost-all-dev

Ubuntu seems to use either `$HOME/bin` or `$HOME/.local/bin` by default for
software in the home directory, and will set `$PATH` accordingly on login.
That would suggest to use `CMAKE_INSTALL_PREFIX:PATH=/home/joe` or
`CMAKE_INSTALL_PREFIX:PATH=/home/joe/.local` for CMake.

Ubuntu does not set `$EDITOR` by itself, so you have to add something like

    export EDITOR='vim'

to `.bashrc` for the default bash shell.
