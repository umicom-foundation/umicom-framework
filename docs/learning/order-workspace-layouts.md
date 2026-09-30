# Put workspace layouts in a useful order

A named layout describes an arrangement of panels. You might keep one for
editing, another for debugging, and another for research. Framework keeps these
layouts in an ordered list. Applications such as Studio and Trader display
that list in their workspace selector and in **Layout Library**.

## Change the order

1. Apply or cancel an active panel-layout edit, then open **Layout Library**.
2. Clear the search box so you can see the complete list.
3. Select the layout you want to move. Selecting a library row does not open it.
4. Choose **Move up** or **Move down**. The selected layout moves one place.
5. Repeat until the list is useful to you. The currently open layout stays open.
6. Choose **Save library** if you want to keep the complete list and its order.

The first row cannot move up and the last row cannot move down. The buttons are
also unavailable during a panel edit, while another library action is queued,
when the list could not be read, or while search hides part of the order.
Clear search or finish the pending action, then try again.

**Save layout** stores the active arrangement only. **Save library** stores the
named list, order and active selection. Read the storage message: a memory-only
backend does not survive application exit. To load a saved list, explicitly
confirm **Restore library**, then choose that action. Restoration replaces the
current named list and may discard unsaved layout changes.

Ordering does not save editor documents or place trading orders. Application
data, account state and panel contents keep their existing owners.

## Use the C23 interface

Include `umicom/ui/workspace_library.h` and obtain a copied list with
`umi_ui_workspace_library_snapshot`. Submit a request against its revision:

```c
UmiUiWorkspaceLibrarySnapshot view;
UmiUiWorkspaceLibraryPolicy policy = {"my.application.layout."};
UmiStatus status = umi_ui_workspace_library_snapshot(model, &policy, &view);
if (status == UMI_STATUS_OK && view.layout_count > 1U) {
    UmiUiWorkspaceLibraryRequest request = {
        .action = UMI_UI_WORKSPACE_LIBRARY_MOVE_EARLIER,
        .target_layout_id = view.rows[1].layout_id,
        .expected_customisation_revision = view.customisation_revision
    };
    status = umi_ui_workspace_library_apply(model, &policy, &request, NULL);
}
```

Here `model` is your existing `UmiUiWorkspaceCustomisation`, with IDs in the
policy's namespace. All access stays on the model's owning thread. The request
borrows its identifier only until the call returns. Check `status` before
reporting success. Read a new snapshot after a change; an old revision is
deliberately rejected with `UMI_STATUS_INVALID_STATE`.

Use `UMI_UI_WORKSPACE_LIBRARY_MOVE_LATER` for the opposite direction. A move
exchanges adjacent complete layout records and increments the customisation
revision once. It preserves each layout's revision, lock state, geometry and
identity, the active identifier, and the shared tool, theme and context stores.
A request already at its boundary succeeds without changing any revision.
Active editing returns `UMI_STATUS_BUSY`; revision exhaustion, invalid input,
wrong ownership and allocation failure leave the model and optional output
unchanged. The previous four action values and APIs remain supported.

For a native workstation, use its `library_snapshot` and `library_apply`
adapter instead of editing a layout model behind the renderer. The application
suite, trading suite, Studio and Trader adapters reuse their existing staged
publication paths. Reordering does not rebuild an unchanged active panel.
These calls perform no checkpoint writes. Explicit whole-library saving uses
the existing `umicom/ui/workspace_library_checkpoint.h` service.

## Resolve a rejected move

If the library changed after you selected a row, refresh it, check the selected
identifier and try again. Do not repeatedly submit the stale request. If a
save reports that another writer changed the stored library, review a confirmed
restore before saving again. A successful move in memory does not establish
that the new order has been saved on disk.

## Create a separate arrangement first

If you want to experiment without changing an existing layout, follow
[Make a copy of a workspace layout](copy-workspace-layouts.md). You can leave
the new ID empty and let Framework supply it before moving the copy into order.

## Compare the saved order before restoring

The library's **Preview saved** button reads the saved list without changing
the current session. [Preview a saved layout library](preview-saved-layouts.md)
explains additions, removals, position changes and the difference between a
read-only preview and a later confirmed Restore.

## Open a layout with the keyboard

1. Finish any active layout edit and open **Layout Library**.
2. Search if you want a shorter list, then move keyboard focus to a visible row.
3. Press **Enter** to open that row. You can also double-click it or select it
   and choose **Open**. Single clicks only select; they do not switch layouts.
4. If the list changed while the request was waiting, choose **Refresh**, check
   the visible selection, and try again. The application does not retry a stale
   request automatically.

Opening a layout changes the current panel arrangement. It does not save a
document, send an order or save the layout library. Use **Save library** when
you want to persist the list and its active selection.
