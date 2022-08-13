# Build system

The Opensmalltalk-VM supports many operating systems and architectures.
It should therefore be possible to build the VM for as many of these combinations as possible.

As outlined in the [Repository structure page](./repository-structure.md), the `building` folder of the repository contains subfolders for the possible VM configurations. See the `HowToBuild` files in the separate subfolders for an explanation of how to build each subdirectories VM configuration.

The following chapters only give a broad overview of each build system

## Linux - Autoconf
The linux builds use autoconf to build the VM.
For a quick overview, see [this page](https://www-f9.ijs.si/~studen/blogs/posts/2015/Jun/05/autoconf-explained/).

The basic parts involved are:
1. `make configure` in the `platforms/unix/config` folder will generate a `configure` script.
	This `configure` script is assembled from the different `*.m4` files found in the build system (e.g. the `platforms/unix/plugins/*` directories).
2. `./configure` will try to find all your dependencies on your system and then generate the appropriate makefiles for the VM and its plugins.
3. `make` will then build the actual VM.

The `configure` script is usually checked into the repository, and therefore step 1 isn't required, unless you want to introduce a new dependency.

Steps 2 and 3 are also automated in `mvm` the scripts in the `building/linux64x64/` directories.
Therefore, most of the time you only need to go into the appropriate building directory and run `./mvm`.

If you **do** need to rebuild the configure script, you can do so by running `make configure` in the `platforms/unix/config/` directory of the repository.

## MacOS - Makefiles
**Note: In many parts of the build system, Mac OS is referred to as iOS for historic reasons.**

Mac OS uses plain Makefiles.
Just like in Linux, there exists a `mvm` script that wraps the make based build process.

The Makefiles in the `platforms/iOS/plugins` subfolders will be automatically included when building the plugin.
See the `platforms/iOS/plugins/BochsX64Plugin` folder for an example. 