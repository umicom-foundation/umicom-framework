# Admit workspace tasks safely

A workspace task is a bounded description of work. Framework owns validation and registry storage so a CLI, Studio panel or build-recipe importer can share one contract. The task service does not execute its command, expand file patterns, inspect a toolchain or authorize network access.

## Construct and publish a task

1. Call `umi_build_task_init` with an ID, label and kind. Initialization clears all storage and establishes API metadata and a revision.
2. Add inputs, outputs, dependency IDs and environment entries with the builder functions. A command task may remain incomplete during construction.
3. Set the command and optional working directory. They are admitted together: if either new value is too large, the original descriptor and revision remain unchanged. A null directory retains the old directory; an empty string explicitly clears it.
4. Call `umi_build_task_validate` to obtain a human-readable reason for refusal. Publish only an accepted descriptor with `umi_build_task_registry_upsert`.
5. Destroy the registry when its owning workspace closes. Serialize mutations; this API provides no internal locking.

## Storage validity and meaning

The service checks structure size, API version, bounded counts and termination of fixed strings before traversing active entries. It then checks enum values, flags, required fields, positive attempt count, distinct dependencies and distinct nonempty input/output entries. Environment entries require a nonempty name followed by an equals sign. Identical whole entries are rejected; entries with the same name and different values retain the previous overlay behavior.

Unused array rows are not active metadata and need not contain valid strings. External string arguments must still be valid null-terminated C strings. A shape check cannot make a dangling pointer safe. The structure must be a complete accessible object matching the current header; a false structure-size field is rejected, not interpreted as permission to read a shorter allocation.

Local validation cannot find missing dependency IDs or cycles. Use the existing task graph for relationships across registry entries. An input pattern is declarative metadata, not proof of file presence, and a command string is not a checked process argument vector.

## Copies, replacements and revision limits

The registry copies accepted snapshots. It owns those values independently of the caller's later edits. Replacing an ID retains the registry order and advances the stored task revision rather than trusting the caller's revision. New task revisions of zero retain the compatibility normalization to one.

A revision identifies an observed state. Wrapping it could make a different task appear unchanged. Mutations therefore refuse with `UMI_STATUS_CAPACITY_EXCEEDED` when a required next identity cannot be represented. The full previous state remains intact. Checked `umi_build_task_registry_reset` returns this refusal; the older void clear remains available and preserves the exhausted registry without a return value.

## Qualify the boundaries

Use the same configured Framework build directory and toolchain that produced the intended SDK. Run each command separately and stop on an unexpected error. The following build target names and test prefixes select the new boundary checks, including private-counter exhaustion without exposing setters in the public API.

```powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --build .\build --target umicom-task-admission-test umicom-task-registry-test --parallel 2
ctest --test-dir .\build -R "^framework\.task\." --output-on-failure --no-tests=error
```

This assumes `build` was configured with `BUILD_TESTING=ON`. If your preset uses another binary directory, use that directory for both commands. There are 37 descriptor scenarios and 16 registry scenarios. Existing build-task, task-graph and orchestrator tests remain relevant integration coverage. Check the selected count before interpreting a pass; a successful command with no selected tests is not evidence.

The [Bits2Banking task laboratory](https://github.com/umicom-foundation/Bits2Banking/tree/main/labs/native/framework-tasks) then checks five observable contracts against an installed SDK. It deliberately stores a fictional compiler command and never executes it. Header/library mismatch, an old SDK or a failed assertion should be diagnosed before changing expected outputs. Source review and authored tests alone do not establish compiler or runtime qualification.
