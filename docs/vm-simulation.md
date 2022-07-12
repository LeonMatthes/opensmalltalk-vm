# VM Simulation
As mentioned in the [Slang documentation](slang.md), Slang is a Smalltalk subset.
This means it can also be executed as normal Smalltalk code and use the normal Smalltalk tooling.

Using this mechanism, almost all of the VM code can therefore be executed as if it were normal Smalltalk code.
Combine that with a few methods that replicate behavior that is normally implemented in native C code, and you can have a VM running entirely inside the [VMMaker image](vmmaker-image-creation.md).

The VMMaker image contains a "Simulation Workspace" that contains code snippets for starting such a simulation.
Usually you should use the SpurReader image (see vmmaker-image-creation.md) as the image to load into this Simulation, as it is relatively simple and easy to control.

## Building a Simulator plugin
Modern versions of the OpenSmalltalk-VM use a JIT (called Cog) to improve VM performance.
Therefore we need a way to simulate the execution of native code for whatever our target Instruction set is.
See: http://www.mirandabanda.org/cogblog/2008/12/12/simulate-out-of-the-bochs/

For this purpose, multiple plugins exist:
- Bochs for x86
- GDB for Arm & Armv8

...and possibly more in the future.

Code for these plugins can be found in multiple places:
- `processors` - the main sources for the simulators
- `building/<myOS>/` - each OS folder should contain a subfolder for each processor.
    - e.g. `building/linux64x64/bochsx64`

## Relevant Classes & Packages

### Package: VMMaker-JITSimulation
Contains all the support code necessary for simulation.

### Class: CogVMSimulator
The main entry point for VM simulation.
See the **class comment** for examples.
