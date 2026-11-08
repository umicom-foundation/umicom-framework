# Publish dependency-graph transitions atomically

A task registry owns descriptions; a task graph copies those descriptions and tracks their current scheduling states. Neither object executes a compiler. The graph is reusable by CLI orchestration, Studio workflow views and source-build recipe review.

## Build and replace a graph

1. Initialize and validate task descriptors, then publish them into a registry.
2. Create a graph and call `umi_build_task_graph_rebuild`.
3. Handle `NOT_FOUND` for a missing enabled dependency and `INVALID_STATE` for a cycle among enabled tasks. Do not interpret an old snapshot as the rejected candidate.
4. On success, use `next_ready` to select work, then record start, progress and terminal outcomes through the graph API.
5. Destroy the graph when its owner closes. Mutations must be serialized.

Rebuild creates a temporary graph because it replaces task descriptions as well as runtime state. It publishes only after dependency checking and readiness calculation succeed. Allocation failure, a cycle, a missing dependency or an exhausted graph revision preserves the previous graph, including its copied source revision. The snapshot's cyclic field remains a description of committed state; the failed candidate is represented by the rebuild return status.

## Record a runtime transition

Ordinary lifecycle operations stage only compact runtime fields: state, dependency counts, progress, attempts, status, exit code and revision. Task strings and input/output arrays remain in their owned graph storage. The service calculates all affected readiness changes and reserves every changed identity before publication.

For example, completing configure may make build ready and update a later task's dependency counts. If any required node revision cannot advance, none of that transition is published. Configure remains in its former state. The supervising host must preserve its actual process outcome for diagnosis; a state-model refusal cannot undo a process that already ran.

A node revision now advances when its observable satisfied/failed dependency counts change even if its state remains Pending. An unchanged terminal node at its maximum revision does not block unrelated transitions. Graph revision increments retain the existing operation/refresh convention; consumers should compare identities rather than assume every API call adds exactly one.

## Understand the retained policy

Tasks are selected deterministically in registry order among ready entries. A failed dependency blocks its consumers unless that dependency declares continue-on-error. A skipped task counts as satisfied, and disabled tasks start skipped. Dependency lookup and cycle checks apply to enabled tasks. A disabled-only cycle or missing relationship remains accepted for compatibility. Importers requiring all declared relationships to resolve must apply a stronger policy before construction.

Retry keeps the attempt count, resets transient result fields and recalculates readiness; it refuses once maximum attempts are consumed. Cancel-waiting affects Pending, Ready and Blocked nodes and leaves running work alone. Recording graph cancellation does not terminate a process. Reset clears runtime outcomes and attempts while retaining task definitions.

## Qualify source and installed consumers

Use your already configured Framework build directory with `BUILD_TESTING=ON`. Run each command separately and stop on any unexpected error. Replace `build` consistently if your preset uses another binary directory.

```powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --build .\build --target umicom-task-graph-transactions-test umicom-task-graph-policy-test --parallel 2
ctest --test-dir .\build -R "^framework\.task_graph\." --output-on-failure --no-tests=error
```

The transaction suite declares 29 cases, including rejected rebuilds, allocation failure, graph and node exhaustion, missing dependency during refresh and downstream refusal. The policy suite declares 20 cases covering chains, a diamond dependency, retries, cancellation, progress and existing disabled-task behavior. The earlier task graph and orchestrator suites remain integration coverage.

The [installed-SDK graph laboratory](https://github.com/umicom-foundation/Bits2Banking/tree/main/labs/native/framework-task-graph) adds five observable scenarios. It models outcomes and never starts stored build commands. A source-only check, a stale installed archive or a test run selecting no cases cannot establish this contract. Record the source revision, compiler, installed SDK and actual selected results before claiming qualification.
