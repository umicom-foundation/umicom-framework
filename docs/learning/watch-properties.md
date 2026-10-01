# Editing and evaluating debugger watches

A watch is an expression you want to inspect while a program is paused. For
example, a watch named by the expression `savedNotes` can show the number of
notes in a small application. Editing that expression and evaluating it are two
different operations. Evaluation asks the debugger to run the expression and
can call functions in the program.

## Change an expression without running it

1. Obtain the application's existing `UmiDebugWorkspace`. Keep its service,
   controller and workspace on their owning thread.
2. Call `UmiDebugWatchEditCapture(workspace, index, &edit)` to copy one watch.
   The index is its current position in the workspace watch list.
3. Prepare settings with `UmiDebugWatchSettingsInit(&settings, 1, "savedNotes + 1")`.
   Use 0 to disable the watch. Expressions must be nonempty and fit in 1023 bytes.
4. Call `UmiDebugWatchEditApply(workspace, edit, &settings, &change)`.
   This changes only local desired settings. It does not start a process or
   send a debugger request.
5. Inspect the returned status. A real edit clears the old value, type and
   evaluation session. An unchanged expression and enabled flag keep the value
   and revision unchanged.
6. Destroy the capture with `UmiDebugWatchEditDestroy(edit)`. Capture the row
   again before making another change.

`UmiDebugWatchEditRemove` removes just the captured row. It cannot recreate
a deleted watch. A capture owns its copy and can still be read after the
workspace is destroyed; applying it always requires the original live workspace.

A BUSY result means the evidence is stale. Another watch change, a session
change, a configuration change or replacement of the workspace can cause this.
Refresh the list and inspect the current row. Do not automatically repeat the
old edit against the same identifier. Refreshing variables or selecting a
different frame does not itself invalidate a local expression draft.

## Evaluate the applied expression

1. Start a native debug session through the host application's normal reviewed
   launch workflow. Watch editing never launches an adapter.
2. Pause at a breakpoint and inspect the stopped stack. Select the frame whose
   local variables the expression should use.
3. Capture the current watch row. Confirm that it is enabled.
4. Call `UmiDebugRuntimeEvaluateWatchEdit(platform, workspace, edit, timeoutMs)`
   with a nonzero timeout. The workspace must use that platform's service.
5. On success, refresh the watch row. The value is a capture from this explicit
   evaluation. Continuing, stepping or changing frames does not automatically
   evaluate it again.

Frame zero is supported; it is not treated as a missing frame. The operation
checks the retained frame, stopped thread, selected workspace context and
active native session before sending a request. It rejects a disabled,
removed, stale or foreign watch.

Queued adapter events return BUSY. Events arriving while a reply is awaited also
prevent publication of the value, even if the event only contains output. The
events stay queued for the normal debugger pump. Process them, inspect the
current state and decide whether another explicit evaluation is appropriate.
A timeout or failed response does not prove the expression had no side effects.
There is no automatic retry and no reversal of target execution.

## Understand result limits

The guarded path requires a string result and accepts an optional string type.
An empty result is a valid captured value. A result of more than 1023 UTF-8 bytes
or a type of more than 255 bytes is rejected rather than shortened. Missing,
wrongly typed or duplicate relevant fields cannot become a successful value.
The previous stored value remains on failure.

The shared `UmiLanguageRuntimeJsonText` helper supports UTF-8 text and paired
Unicode surrogate escapes, including characters outside the basic multilingual
plane. It rejects malformed UTF-8, embedded NUL and lone surrogates, leaving
the destination unchanged on failure. The existing legacy JSON reader and
low-level evaluation API keep their original contracts for older callers.

This workflow has no watch persistence, automatic evaluation, editable target
variables, result history or expandable child values. All changes belong to the
current in-memory workspace. Use [source breakpoint properties](source-breakpoint-properties.md)
to learn the separate breakpoint workflow.

To explore local structures without changing a watch expression, follow
[variable child inspection](variable-inspection.md). This separate workflow
starts from the Variables list; watch results remain unexpanded.
