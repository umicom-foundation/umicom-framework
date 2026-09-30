# Make a copy of a workspace layout

A layout is an arrangement of panels. Copying one gives you a separate layout
to customise while keeping the original arrangement available. The copy opens
when Duplicate succeeds. Documents and trading records keep their existing
owners; this action is not a backup of application data.

## Copy a layout in Studio or Trader

1. Apply and lock, or cancel, any panel-layout edit.
2. Open **Layout Library** and select the layout you want to copy.
3. Enter a recognisable **Layout name**, such as `Research on a small screen`.
4. Leave **New layout ID (optional)** empty.
5. Click **Duplicate**. The new layout opens and appears in the library.
6. Arrange its panels using the normal layout editing controls, then apply
   and lock those changes.
7. Choose **Save library** to keep the complete named list, its order and
   active selection. Read the storage message to check whether saving is
   persistent or memory-only. Save documents separately.

Framework supplies an unused identifier such as `my.layout.research.copy.1`.
An identifier is the internal name used to distinguish two layouts, even when
they have the same display name. You normally do not need to type it. Existing
copies are checked across the complete library, including rows hidden by search.

You can still enter a manual full identifier if you need a particular one.
It must be unused and belong to the same application namespace: keep the
application prefix visible in the selected layout's identifier. Manual IDs
are checked by the same workspace owner as automatic ones.

## If the copy cannot be created

- If the library changed after your click, choose **Refresh**, check the source
  and name, and click **Duplicate** again. The application does not silently
  retry against a layout list you have not reviewed.
- A full library cannot accept another layout. Review your saved copy before
  explicitly removing an unwanted layout, then try again.
- A very long source identifier may leave no room for an automatic suffix.
  Enter a shorter, unused full identifier in the same application namespace.
  Framework never shortens your source ID silently.
- Finish an active panel edit before changing the library. Closing the view's
  owner before queued work runs cancels that work.

**Save layout** covers the active arrangement only. To restore the complete
named list, confirm **Restore library** and click it. This replaces unsaved
library changes. A memory-only saved library disappears when the process exits.

## Use the C23 interface

Include `umicom/ui/workspace_library.h` and link `Umicom::ui`. Start with a
copied snapshot of your existing owner. Ask for a suggestion, then send its
copied identities and revision to the existing Duplicate action:

```c
UmiUiWorkspaceLibrarySnapshot view;
UmiUiWorkspaceLibraryCopySuggestion suggestion;
UmiUiWorkspaceLibraryPolicy policy = {"my.application.layout."};
UmiStatus status = umi_ui_workspace_library_snapshot(model, &policy, &view);
if (status == UMI_STATUS_OK && view.layout_count != 0U) {
    status = umi_ui_workspace_library_suggest_copy(
        &view, view.rows[0].layout_id, &suggestion);
    if (status == UMI_STATUS_OK) {
        UmiUiWorkspaceLibraryRequest request = {
            .action = UMI_UI_WORKSPACE_LIBRARY_DUPLICATE,
            .target_layout_id = suggestion.target_layout_id,
            .new_layout_id = suggestion.new_layout_id,
            .name = "My layout copy",
            .expected_customisation_revision =
                suggestion.expected_customisation_revision
        };
        status = umi_ui_workspace_library_apply(model, &policy, &request, NULL);
    }
}
```

Here `model` points to your existing `UmiUiWorkspaceCustomisation`. Use its
owning thread and check `status` before reporting success. A native application
uses its workstation's `library_snapshot` and `library_apply` adapters so the
renderer and model change together; it must not modify the model behind GTK.

The suggestion function does not change the snapshot, reserve an identifier,
allocate memory or access storage. It validates the complete copied list and
tries `.copy.1`, `.copy.2` and so on, choosing the first unused candidate that
fits. Repeated requests from the same snapshot return the same suggestion.
The source ID is retained in full, so an owning namespace cannot be truncated.

The output owns its strings and is left unchanged on failure. Do not place it
inside the input snapshot. An active edit returns `UMI_STATUS_BUSY`; malformed
snapshots return `UMI_STATUS_INVALID_STATE`; a missing source returns
`UMI_STATUS_NOT_FOUND`. Full libraries, exhausted revisions and insufficient
ID space return `UMI_STATUS_CAPACITY_EXCEEDED`. The later apply still checks
the real model's revision, ownership and collisions. An old proposal is never
permission to overwrite a layout created in the meantime.
