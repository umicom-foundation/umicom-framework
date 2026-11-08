# Plan and supervise a dependency-closed task operation

The Framework task orchestrator combines an owned task graph with operation identity, selected-target planning, parallel slots, retry counts and cooperative stopping. Process creation remains the responsibility of a supervised provider. A successful operation state means reported outcomes satisfy the model's policy; it is not independent evidence that a compiler ran.

## Establish ownership

1. Create a source task registry and publish valid descriptors into it.
2. Create an orchestrator with the intended maximum parallel count. Zero selects the serial default of one.
3. Keep the source registry alive until after the orchestrator is destroyed. Serialize source edits and planning through the owning workspace.
4. Plan an operation with a complete operation ID and an optional requested task ID. A null or empty target selects all enabled roots.
5. Begin the accepted plan, request ready tasks and record their lifecycle through the orchestrator API.

Planning includes the selected task's transitive prerequisites exactly once in dependency-first order. Disabled prerequisites remain included when required by an enabled root; the graph's existing skipped-dependency policy applies. Unlike the general graph's disabled-only relationship handling, the selected closure walk resolves every prerequisite it traverses. Missing IDs and cycles therefore refuse the plan.

## Preserve a rejected plan

The operation ID and requested task ID must fit their bounded fields in full. The planner rejects oversized text instead of silently changing its identity through truncation. It builds a fresh owned registry, resolves closure, then asks the graph service to publish a validated replacement. Any refusal preserves the previous operation, copied tasks, generation and graph. A successful replacement retains the graph object's address for borrowed observers.

The source registry is borrowed, but the planned definitions are copied. Editing source tasks does not retroactively change an accepted operation. Replan explicitly to adopt those edits. A refused plan must not erase the evidence needed to explain or run the older selection.

## Bound dependency traversal

The closure walk uses explicit arrays of task indices and dependency positions. One task snapshot is reused while the walk descends and returns. This retains depth-first dependency ordering without placing a large fixed snapshot on each recursive stack frame. A chain at the 128-task registry capacity and a cycle at that depth have dedicated regression cases.

An included marker avoids copying a shared prerequisite twice. A visiting marker identifies a back edge into the current path. These markers express different facts: completed traversal is safe to reuse, while an unfinished ancestor indicates a cycle.

## Admit owner and child state together

The orchestrator reserves its revision before any nested graph mutation. Planning also reserves its generation, and retry reserves its count. The graph independently admits all affected node identities. If either layer refuses, the prior owner and graph remain consistent.

A stop request cancels waiting tasks through the graph before publishing the owner's stop flag. Running tasks remain active until the host reports their actual terminal outcomes. A refused stop does not imply that a process was interrupted. Finish requires a begun operation with no active tasks; it cannot turn an idle or merely planned object into a successful run.

The mutable graph accessor remains for compatibility. Treat the borrowed pointer as an observation surface while using orchestration. Direct graph mutations bypass the owner's active counters and terminal-state bookkeeping; normal callers should use the orchestrator's lifecycle methods.

## Qualify the implementation and installed SDK

Use the intended Framework build directory already configured with BUILD_TESTING=ON. Run commands separately and stop on an unexpected error. Replace the directory consistently if your preset uses a different binary path.

```powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --build .\build --target umicom-task-orchestrator-transactions-test umicom-task-orchestrator-closure-test --parallel 2
ctest --test-dir .\build -R "^framework\.task_orchestrator\." --output-on-failure --no-tests=error
```

The transaction suite declares 25 cases for rejected plans, allocation failure, full IDs, owner/child identity limits and finish admission. The public closure suite declares 12 cases for depth, cycles, deduplication, selected roots, source isolation, parallel slots, retries and stopping. Existing task, graph and orchestrator tests remain relevant integration coverage.

The [installed orchestration laboratory](https://github.com/umicom-foundation/Bits2Banking/tree/main/labs/native/framework-task-orchestration) exercises five modeled scenarios through installed headers and archives. Identify those artifacts when results differ. Keep selected-test counts and failure output with compiler and source identity. Authored tests, static source checks and modeled process outcomes must remain distinct from actual build and application qualification.
