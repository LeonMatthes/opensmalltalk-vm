# The VMMaker image

OpenSmalltalk-VM development is mostly done from within a working Squeak image called the VMMaker image.
This image contains the VM sources as well as tooling specific to Opensmalltalk-VM development.

The `image` folder contains multiple scripts to build yourself both a VMMaker image and a SpurReader image (see below).

Generally, the VMMaker image is just a normal trunk image that contains:
- The VM sources, downloaded in BuildSpurTrunkVMMakerImage.st
- Multiple Workspaces with useful code snippets
    - These are loaded from the `*.text` files in the `image` folder.

## Troubleshooting
After you have built your VMMaker image, it's likely you won't have to build it again for a long time.
This does also mean that these scripts aren't executed all that often.
Therefore they are likely out of date.

So don't expect them to work from the get-go, but be prepared to fix things here and there, and possibly to emulate what the scripts do.

The good part is: Building the VMMaker image isn't all that complex.
You can try to get the entire script running, or read the scripts and perform them yourself, one step at a time and save your progress in-between.

## SpurReader image
The SpurReader image isn't entirely necessary, but very helpful when simulating the VM. (See: `vm-simulation.md`)

It is a very simple image that only waits for input from stdout and then evaluates the given input.
Note that the input for the SpurReader image is given in `.st` format. Which means code segments are terminated with exclamation marks (`