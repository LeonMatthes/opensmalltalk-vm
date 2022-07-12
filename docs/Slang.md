# Slang

Slang is a subset of Smalltalk, that is both:
- Valid Smalltalk code
- Transpile-able to C

It is written like normal Smalltalk code and can (mostly) be executed like normal Smalltalk.

For a detailed Overview, see: http://wiki.squeak.org/squeak/2267

## Relevant Classes & Packages
### Class: VMMakerTool
An in-image GUI to generate C code for the VM or any external plugins.

Use `VMMakerTool openInWorld` to open it.

**Further documentation in class comment**

### Class: VMMaker
This class is responsible for generating the different configurations of the OpenSmalltalk-VM from the in-image Slang sources.

**Further documentation in class comment**

### Package: VMMaker-Translation to C
Contains the code necessary for Slang-to-C code generation

The classes in here that start with `T` (i.e. `TParseNode` subclasses) represent nodes of the Slang [Abstract Syntax Tree](https://en.wikipedia.org/wiki/Abstract_syntax_tree) that will be converted into C code.

### Class: CCodeGenerator
The main entry point for Slang-to-C code generation.

See: `CCodeGenerator>>#cCodeForMethod:`
