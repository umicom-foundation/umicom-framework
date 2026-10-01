# Inspect an object's children while debugging

A local variable can contain several values. A `Notebook` structure, for
example, might contain a title and a list of notes. A debugger can return a
short summary for the structure and a reference that identifies its children.
The reference is temporary: after the program continues, the same number may
identify a different object.

Framework's variable inspection API keeps a copy of the selected row and the
debugger state that produced it. Reading children creates a separate owned page.
It does not replace the scope's root variables or identify children by name.
Two children called `item` remain two different rows.

## Capture one level

1. Use the host's normal debug workflow to start a native session. Pause and
   inspect the stack, then select the required frame and scope. A frame is one
   function call in the stopped program; a scope groups that frame's variables.
2. Include `umicom/debug_runtime/variable_inspection.h` and use the existing
   platform and workspace on their owning thread. The workspace must use the
   platform's debugger service.
3. Call `UmiDebugVariableTargetCapture(workspace, index, &target)`. The index
   comes from the current visible root list. This copies a root without
   sending a request. Reading it returns the existing stored summary, not a
   newly fetched value.
4. Check `UmiDebugVariableTargetExpandable(target)`. A value without children
   returns `INVALID_STATE`. An ancestor reference returns `ALREADY_EXISTS` so
   a self-referencing structure cannot create an endless expansion chain.
5. Call `UmiDebugRuntimeInspectVariable(platform, workspace, target, 1500, &page)`
   only after an explicit inspection action. The timeout is in milliseconds.
   Adapter visualizers may run target code while obtaining children.
6. On success, read `UmiDebugVariablePageCount(page)` and use
   `UmiDebugVariablePageAt(page, index, &value)` for each copied row. Names,
   values and types are plain text; render them without markup interpretation.
7. To inspect a child, call `UmiDebugVariablePageTarget(page, index, &child)`
   and repeat the expandability check and explicit request. The child retains
   its original debugger state even after its parent page is destroyed.
8. Destroy each successful target and page with its matching destroy function.
   An owning output is set to `NULL` on failure. Do not pass the sole pointer
   to an existing page as an output; use a temporary and replace it on success.

This host function demonstrates one level. It assumes the host has already
obtained permission to debug its project and has inspected a stopped frame.

```c
#include "umicom/debug_runtime/variable_inspection.h"
#include <stdio.h>

UmiStatus PrintChildren(UmiDebugRuntimePlatform *platform,
    UmiDebugWorkspace *workspace, size_t rootIndex)
{
    UmiDebugVariableTarget *target = NULL;
    UmiDebugVariablePage *page = NULL;
    UmiStatus status = UmiDebugVariableTargetCapture(workspace, rootIndex, &target);
    if (status == UMI_STATUS_OK)
        status = UmiDebugRuntimeInspectVariable(platform, workspace, target, 1500U, &page);
    if (status == UMI_STATUS_OK) {
        for (size_t i = 0U; i < UmiDebugVariablePageCount(page); ++i) {
            UmiDebugRuntimeVariable value;
            status = UmiDebugVariablePageAt(page, i, &value);
            if (status != UMI_STATUS_OK) break;
            printf("%s = %s\n", value.name, value.value);
        }
    }
    UmiDebugVariablePageDestroy(page);
    UmiDebugVariableTargetDestroy(target);
    return status;
}
```

## Handle changes and failures

A `BUSY` result means the captured context changed or an adapter event needs
processing. Let the ordinary debugger event loop process it, inspect the
current stopped state and capture a new root. Do not reuse an old numeric
reference merely because the displayed name looks familiar. This conservative
check includes changes to configurations, sessions, selections, breakpoints
and watches. Console registry output alone does not change the capture, but
an output event still queued in the adapter blocks a new request.

An adapter refusal or timeout leaves the earlier caller-owned page readable.
It is still an earlier capture. Requests are never automatically repeated.
Do not infer that a timed-out visualizer did no work. A page remains readable
after the session or workspace closes, but it cannot authorize a later request.

The strict decoder accepts an empty child list and empty string values. It
requires complete recognized fields, checks nonnegative 32-bit references and
counts, decodes UTF-8 and Unicode escapes, and rejects ambiguous duplicates,
embedded NUL and malformed text. A failure leaves the caller's decode output
unchanged. Unknown extension values are not interpreted.

## Know the bounds

- A response can contain at most 128 children. A larger list fails; no partial
  prefix is shown. This API does not request paged slices.
- There can be at most 16 successive requests down one branch. Ancestor cycles
  are stopped sooner. Sibling references can legitimately be equal.
- Each child name/type can use 255 UTF-8 bytes; its value 4095; evaluation name
  1023; memory reference 127. These are byte limits, not character counts.
- The existing transport also limits a response to its bounded JSON envelope
  and token capacity. Many large values can exceed that before 128 rows.
- Root summaries retain the established registry's smaller buffers. This new
  API does not re-decode or lengthen an earlier root summary.

`UmiDebugVariablePageCreate` is for another adapter host that has already
validated request completion and stopped state. It copies bounded records; it
does not independently contact a process. A UI should normally call the guarded
native helper instead. Keep large `UmiDebugVariableChildren` decode buffers on
the heap, especially on Windows GUI threads.

This workflow does not assign variables, evaluate watch expressions, persist
child values or automatically refresh them. Existing low-level variable
refresh and decoder contracts remain available to their previous callers.
For expression evaluation, see [watch properties](watch-properties.md).
The protocol's reference lifetime is described in the official
[Debug Adapter Protocol specification](https://github.com/microsoft/debug-adapter-protocol/blob/main/specification.md).

The shared `UmiLanguageRuntimeJsonParseComplete` entry point first limits input
length and nesting, then checks every token, including otherwise ignored
extension values. It rejects invalid JSON literals and whitespace, malformed
Unicode and excessive nesting before a child response can be accepted. Its
successful token document borrows the original input text; keep that input
alive while using the tokens. This additive parser leaves the existing legacy
tokenizer unchanged.

Root loading now keeps its large temporary response/list on the heap as well.
This preserves the existing root decoder and publication behavior while
avoiding a multi-megabyte automatic allocation on a GUI thread's stack.

To inspect a retained scope, including one marked expensive, see
[explicit scope inspection](scope-inspection.md). It uses the same owned pages
without replacing roots or changing the selected scope.
