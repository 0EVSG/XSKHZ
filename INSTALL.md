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
the `PATH` variable of your shell if necessary. Also make sure that the `EDITOR`
variable is exported when using ViFi.

Enjoy!
