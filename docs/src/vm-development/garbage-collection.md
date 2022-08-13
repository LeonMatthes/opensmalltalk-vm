## Young and Old Space
Objects in Squeak are separated into a young and old space.
Newly allocated objects are (almost always) allocated in the young space.
When more space is needed, the young space is scavenged, and young objects that are no longer referenced are freed.

Young objects that survive multiple scavenging phases will be promoted into old space.

Old space is usually garbage collected when the VM runs out of allocated memory. (See Mark and Compact).

## Mark and Compact
Squeak currently does a stop-the-world Mark and Compact garbage collection.
See [Wikipedia](https://en.wikipedia.org/wiki/Mark%E2%80%93compact_algorithm).

# Important Classes
## [SpurMemoryManager](squeak://SpurMemoryManager)
Called whenever garbage collection needs to be started.

Does marking in a Mark&Compact operation.
During marking, Forwarders are resolved.

Good entry points:
- [SpurMemoryManager>>#markAndTrace:](squeak://SpurMemoryManager>>#markAndTrace:)
- [SpurMemoryManager>>#globalGarbageCollect](squeak://SpurMemoryManager>>#globalGarbageCollect)
- [SpurMemoryManager>>#markLoopFrom:](squeak://SpurMemoryManager>>#markLoopFrom:)

*Future*: Marking might be extracted from the SpurMemoryManager and moved into different marking strategies.
- SpurMarker (abstract super class)
- SpurAllAtOnceMarker
- SpurIncrementalMarker

## [SpurGenerationScavenger](squeak://SpurGenerationScavenger)
Scavanges the young space.

[Class comment](squeak://ToolSet%20browseClassCommentOf:SpurGenerationScavenger) includes a link to a paper that explains the algorithm used.

Entry points:
[SpurGenerationScavenger>>#scavenge:](squeak://SpurGenerationScavenger>>#scavenge:)

## [SpurPlanningCompactor](squeak://SpurPlanningCompactor)
Does compaction on old space.

### 3 Phases
1. Plan where things go
2. Update the pointers
3. Relocate

Entry Points:
- [SpurPlanningCompactor>>#compact](squeak://SpurPlanningCompactor>>#compact)
