# Important classes
- SpurMemoryManager (inherited classes are less important)
- SpurGenerationScavenger
- SpurPlanningCompactor - global GC

## Mark and Compact
Squeak currently does a stop-the-world Mark and Compact garbage collection.
See: https://en.wikipedia.org/wiki/Mark%E2%80%93compact_algorithm .

## SpurMemoryManager
Called whenever garbage collection needs to be started.

Does marking in a Mark&Compact operation.
During marking, Forwarders are resolved.

Good entry points:
SpurMemoryManager>>#markAndTrace:
SpurMemoryManager>>#globalGarbageCollect
SpurMemoryManager>>#markLoopFrom:

Future: Marking might be extracted from the SpurMemoryManager and moved into different marking strategies.
- SpurMarker (abstract super class)
- SpurAllAtOnceMarker
- SpurIncrementalMarker

## SpurGenerationScavenger
Scavanges the young space.

Class comment includes a link to a paper that explains the algorithm used.

Entry points:
SpurGenerationScavenger>>#scavenge:

## SpurPlanningCompactor
Does compaction on old space.

### 3 Phases
1. Plan where things go
2. Update the pointers
3. Relocate

Entry Points:
- SpurPlanningCompactor>>#compact
