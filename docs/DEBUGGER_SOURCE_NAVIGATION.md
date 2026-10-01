# Keeping debugger selections attached to the right source

A debugger can replace its entire call stack when a program pauses again. A
frame identifier is therefore a name inside current debugger state, not a
permanent link to a source line. Framework provides owned selections so hosts
can keep this distinction explicit.

## Capture and use a row

1. Get the window's `UmiDebugWorkspace`. Keep registry writes and UI actions on
   its owner thread. The selection API does not provide cross-thread locking.
2. Call `UmiDebugSelectionCapture` with `UMI_DEBUG_SELECT_FRAME` and the visible
   frame index. The index belongs to the currently selected thread. A frame
   requires a retained thread marked stopped. A thread row may be captured
   while running, but the host still decides whether an action is allowed.
3. Store the returned selection with the control and destroy it when the
   control is released. The selection owns its copies of the thread, frame and
   source directory. It does not retain the workspace or the target process.
4. Immediately before dispatch, call `UmiDebugSelectionValidate`. `BUSY` means
   debugger state or selection changed. Ask the user to choose a current row;
   do not retry automatically against a reused identifier.
5. Copy the returned snapshot before dispatching any command that can rebuild
   controls. Check that command's result before opening its copied source.
6. Call `UmiDebugFrameOpenSource` from `Umicom::debug_ui`, passing the document
   coordinator, copied frame and copied source directory. Report failures.
   Destroy the captured selection with `UmiDebugSelectionDestroy` when finished.

The first five steps use `Umicom::debug`. The source-opening composition lives
in `Umicom::debug_ui` so the core model does not depend on editor ownership.
Both are exported Framework targets; the aggregate `Umicom::Framework` includes
them. The public headers are `umicom/debug/selection.h` and
`umicom/debug_ui/navigation.h`.

## Source paths and drafts

Relative frame paths use the launch directory found through the thread's
session and its configuration. Native launches publish that configuration
before publishing their session. Each retained program, directory and argument
string must fit its 1024-byte configuration array, including the terminator.
An overlong native launch input returns `CAPACITY_EXCEEDED` before starting an
adapter; it is never silently shortened. Missing launch information does not fall back
to the application's current working directory. Absolute paths and local
`file:///` URIs can be opened without a source base. Other URI schemes are not
opened by an external program.

Lines are one-based. Columns follow the document coordinator's one-based UTF-8
byte convention; zero selects the beginning of the line. A provider that uses
different column units must convert them before publishing a frame. The source
may have changed since compilation: navigation is not proof of a matching build.

Opening a missing file leaves the previous active document and caret alone. If
the file opens but the requested line no longer exists, its tab remains open
and `NOT_FOUND` is returned. Existing unsaved text is retained. Opening a frame
does not save, reload or replace a draft.

## Refreshing the display

Use `UmiDebugWorkspaceViewStamp` and `UmiDebugViewStampEqual` to compare all
relevant registry generations. Comparing only their largest revision can miss
a change in a smaller counter. Equal stamps let a host keep existing controls
and keyboard focus. Console output is excluded because it is not a detail row.

When a selected thread, frame or scope disappears, workspace refresh repairs
the dependent selection. It never chooses an orphan as a default selection.
Direct variable or watch registry edits refresh the display without changing a
captured navigation target. Actions through the workspace conservatively
invalidate old captures. Removal and reuse of a thread or frame ID also
invalidates the old capture. A new workspace has a new process-local identity,
even if its memory address is reused.

## Recovering from a refused selection

1. Keep the original source or draft open.
2. Wait for a stable debugger pause and refresh the debugger view.
3. Choose a row in the current thread's Call Stack.
4. If the file is unavailable, inspect the launch directory and the reported
   path. Correct the launch configuration or source location explicitly.

These contracts validate retained evidence. The host must separately check
whether a native session is active and paused, and whether the user authorised
the debugger operation. They do not claim that an adapter is responsive.
