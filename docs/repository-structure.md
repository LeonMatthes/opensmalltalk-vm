VM source directories
---------------------
The Slang output of the various VMs are kept in "vm source" directories.  These
C sources define the core VM (the Smalltalk execution engine and the memory
manager), and a substantial set of "plugins" that provide interfaces to various
external facilities via Smalltalk primitive methods.  Each vm source directory
is specific to a particular VM, be it Squeak Cog Spur, or V3 Stack, etc.
The plugins can be shared between VMs, choosing the set of plugins to include
in a VM at build time (see plugins.int & plugins.ext in build directories).

The VM source are in directories such as
```
	src/v3.sista		- Smalltalk Sista V3
	src/spur32.sista	- Smalltalk Sista Spur
	src/spur32.cog 		- Smalltalk Cog Spur
	src/spur64.cog 		- Smalltalk Cog Spur 64-bit
	src/spur32.stack 	- Smalltalk Stack Spur
	src/spur64.stack 	- Smalltalk Stack Spur 64-bit
	src/v3.cog			- Smalltalk Cog V3
	src/v3.stack 		- Smalltalk Stack V3
```

All plugins are in the directory
```
	src/plugins
```

These contain many, but not all, of the plugins available for the VM.  Others
can be found in Cog, or in various Monticello packages in various repositories.

Each vm source directory contains several files, a subset of the following:
```
	cogit.c				- the JIT; a Cogit cooperates with a CoInterpreter.
                          This simply includes a processor-specific JIT file
	cogitIA32.c et al   - relevant processor-specific JIT, selected by cogit.c
	cogit.h				- the Cogit's API, as used by the CoInterpreter
	cogmethod.h			- the structure of a CogMethod, the output of the Cogit
	cointerp.c			- the CoInterpreter's source file
	cointerp.h			- the API of the CoInterpreter, as used by the Cogit
	cointerpmt.c		- the multi-threaded CoInterpreterMT's source file
	cointerpmt.h		- the API of the CoInterpreterMT, as used by the Cogit
	gcc3x-cointerp.c	- cointerp.c massaged to interpret faster if using gcc
	gcc3x-cointerpmt.c	- ditto for cointerpmt.c
	gcc3x-interp.c		- ditto for interp.c
	interp.c			- the StackInterpreter's source file
	interp.h			- defines for the VM configuration, word size, etc
	vmCallback.h		- the structure of the VM's VMCallbackContext
```

Platform build directories
--------------------------
The current "official" build directories are of the form
building/OS_WordSize_Processor, and include
```
	building/linux32x86	- uses autoconf, gcc and make
	building/macos32x86	- 32-bit Mac OS X using clang and gmake
	building/macos64x64	- 64-bit Mac OS X using clang and gmake
	building/win32x86		- uses cygwin, gcc and gmake
```
More can be added as required.  In each there is a HowToBuild that describes
the necessary steps to compile a VM.

Within each building/OS_WordSize_Processor directory are a set of build directories
for specific configurations of Cog, and for support code and makefiles.  For
example, there exist
```
	building/macos32x86/squeak.cog.spur   - A Cog JIT VM with Squeak branding,
                                         using the Spur memory manager.
	building/macos32x86/squeak.stack.spur - A Stack interpreter VM with Squeak
                                         branding, and the Spur memory manager.
	building/macos32x86/squeak.cog.v3     - A Cog JIT VM with Squeak branding,
                                         using the old Squeak memory manager.
	building/macos32x86/pharo.cog.spur    - A Cog JIT VM with Pharo branding and
                                         plugins (not yet implemented) using the
                                         Spur memory manager.
```
    etc.

There exist
```
    building/macos64x64/bochsx86 - Support libraries for the BochsIA32Plugin which
                                is used to develop Cog itself.
    building/macos64x64/bochsx64 - Support libraries for the BochsX64Plugin which
                                is used to develop Cog itself.
    building/macos64x64/gdbarm32 - Support libraries for the GdbARMPlugin which
                                is used to develop Cog itself.
    building/macos64x64/gdbarm64 - Support libraries for the GdbARMv8Plugin which
                                is used to develop Cog itself.
```
and the intention is to add such directories to contain e.g. support code for
the Pharo Cairo and Freetype plugins, and anything else needed.  By placing
support directories in each build directory they can be shared between various
branded VM builds, avoiding duplication.

There exist
```
	building/macos32x86/common - Gnu Makefiles for building the various branded VMs
	building/macos64x64/common - Gnu Makefiles for building the various branded VMs
	building/win32x86/common   - Gnu Makefiles for building the various branded VMs
	building/win64x64/common   - Gnu Makefiles for building the various branded VMs
```
And the intention is to add building/linuxNN????/common as soon as possible to
use Gnu Makefiles to build all VMs on all platforms.

The scripts directory contains various scripts for validating and checking-in
generated sources, packaging builds into installable artifacts (tar, msi, zip),
and so on.  The linux builds and the packaging scripts produce outputs in the
products directory tree.  The packaging scripts may choose to include Smalltalk
source files included in the sources directory.


Other directories
-----------------
The platforms directory contains the associated platform-specific files that
combine with the Slang-generated source to produce complete VMs.  The structure
is
```
	platforms/Cross/vm
	platforms/Cross/plugins
	platforms/iOS/vm
	platforms/iOS/plugins
	platforms/unix/vm*
	platforms/unix/plugins
	platforms/win32/vm
	platforms/win32/plugins
```
Each vm directory contains support for the core VM.  Each plugin directory
contains run-time and build-time support for various plugins.

The processors directory contains the source for various processor simulators.
The JIT is developed in Smalltalk by using one of these processor simulators
to execute the code the JIT produces.  Currently x86 & x86-64 are derived from
Bochs, and ARMv6/v7 & ARMv8 are derived from gdb.

Finally the image directory contains scripts that will build a "VMMaker" image,
a Squeak Smalltalk image containing all the packages that comprise the Cog
system, suitable for developing the VM and for generating (or updating) the
sources in the vm source directories. There is also a script for generating a
64-bit Spur image from a 32-bit Spur image.
