# Read a scope only when the user asks

A debugger scope groups the variables available in one stopped function call.
For example, Locals may hold a function's local values while Globals may hold
shared program state. The adapter can mark a group expensive because reading
it takes work or returns many values. Framework's ordinary stopped-state loader
continues to skip expensive groups. A host can now offer an explicit action to
read any retained scope without changing the selected scope or replacing roots.

## Connect the capture to your host

1. Start and inspect a stopped native session using the host's existing debug
   workflow. Use the workspace belonging to that platform's debugger service
   on its owner thread.
2. Read `UmiDebugWorkspaceSnapshot.visible_scope_count`. Enumerate the current
   selected frame's scopes with `umi_debug_workspace_scope_at`. The names are
   display text, not expressions or identities; repeated names are allowed.
3. Call `UmiDebugScopeInspectionCapture(workspace, index, &scope, &target)` while
   building a control. This copies the scope and the full debugger view stamp.
   It sends no request. `scope` is optional; the owned `target` is required.
4. Use `UmiDebugVariableTargetExpandable(target)` to decide whether to enable
   inspection. A zero reference is not expandable. References above the
   protocol's positive 32-bit range are rejected before dispatch.
5. After an explicit gesture, call `UmiDebugRuntimeInspectVariable` with this
   target. The same stopped-frame, owner, queued-event, cycle and capacity
   checks used by variable inspection apply to scopes.
6. Read the returned page and build child targets with the ordinary variable
   inspection API. Keep names and values as plain text, without evaluating
   names or interpreting markup. Destroy every owned target and page.

This helper shows how to retain a displayed capture on a failed request. The
caller owns `*displayed`, initially `NULL`, and destroys it when the view closes.
The `target` must come from the control the user actually clicked, rather than
a new capture of whichever row happens to occupy that position later.

```c
#include "umicom/debug_runtime/scope_inspection.h"

UmiStatus RefreshDisplayedScope(UmiDebugRuntimePlatform *platform,
    UmiDebugWorkspace *workspace, const UmiDebugVariableTarget *target,
    UmiDebugVariablePage **displayed)
{
    if (displayed == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugVariablePage *replacement = NULL;
    UmiStatus status = UmiDebugRuntimeInspectVariable(
        platform, workspace, target, 1500U, &replacement);
    if (status == UMI_STATUS_OK) {
        UmiDebugVariablePageDestroy(*displayed);
        *displayed = replacement;
    }
    return status;
}
```

`UmiDebugVariableTargetRead` on a scope target returns a synthetic container:
its name and reference come from the scope, while value, type and evaluation
expression are empty. This does not claim that the scope has been evaluated.
The optional copied scope record supplies its `expensive` flag and original ID.

## Keep captures separate from current state

Capturing a scope performs the workspace's normal selection repair, but it
does not select that scope. Other groups in the same selected frame can be
inspected independently. A successful request returns an owned page without
publishing its rows into the canonical root-variable registry.

Any debugger detail or selection change invalidates the target for another
request. Pending adapter events also block dispatch or publication until the
normal event loop handles them. Display a `BUSY` result and require fresh
context instead of silently recapturing or retrying an old control.

Captured pages remain readable after the service closes. That ownership rule
does not make their references valid in another process or stop. Visualizers
may execute code, and a timeout cannot establish that no work happened.

The existing limits still apply: 128 rows per response, 16 requests down a
branch including the scope request, and bounded text/JSON transport sizes.
There is no paging or partial prefix publication. The scope's expensive flag
does not override those bounds. See [variable inspection](variable-inspection.md)
for page ownership, decoding and error handling.

The [Debug Adapter Protocol scope contract](https://github.com/microsoft/debug-adapter-protocol/blob/main/specification.md#scope)
describes scope references and their lifetime while execution is suspended.
