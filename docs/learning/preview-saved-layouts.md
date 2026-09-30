# Preview a saved layout library

A layout library stores named panel arrangements and the active layout. You
may have changed the current session since the last save. **Preview saved**
lets you compare the saved list with the list you are using before deciding
whether to restore it.

Preview reads saved data. It does not change the current layout, write a new
checkpoint, save a document or replace an application's data.

## 1. Open a preview

1. Finish the current panel-layout edit with Apply or Cancel.
2. Open **Layout Library** in Studio or Trader.
3. Choose **Preview saved**. You do not need to confirm a restore first.
4. Read the comparison beneath the storage buttons. It includes the full saved
   list even if you have used search to filter the current list above it.

If there is no saved library, the view says so and keeps your current layouts.
Use **Save library** when you intend to create a saved copy. If preview is
unavailable, check the storage message and finish any layout edit first.

## 2. Understand the comparison

The preview shows the saved order and marks its active layout. An added layout
exists in the saved list but not in your current session. A removed layout
exists only in your current session and would disappear from the list if you
restore that saved version. Renames show the current and saved display names.
Position changes show the current and saved list positions, counting from one.
The same layout may have several changes.

Panel counts include hidden stored panels. The comparison also reports lock
state and active-selection changes. It compares summaries: equal names, order
and counts do not prove that individual panel positions, sizes, context links
or document contents are equal.

Read the storage source message. Memory-only storage lasts for the running
process. Persistent storage is connected to a durable backend. If the latest
saved copy is unusable and an earlier valid copy is available, the preview
identifies that recovery copy. Previewing it does not repair or overwrite data.

## 3. Decide what to do next

You can close the library and keep working; preview requires no Apply or Cancel.
To restore, select **Replace this session's named layout list**, then choose
**Restore library**. Preview clears any earlier restore confirmation so that
viewing a summary cannot silently authorize replacement.

Restore reads storage again. Another application window may have saved a newer
library after your preview, so it can restore a newer list than the one shown.
Preview again immediately before restoring if another window is editing the
same library. Preview is an observation, not a reservation of a saved version.

Refresh, a layout action or a storage action clears the old comparison. Changes
to a name or ID draft in the form are only input drafts; they do not change the
workspace until you choose an action. Preview does not discard those drafts.

## 4. Use the read-only Framework API

`umi_ui_workspace_library_checkpoint_preview` reads through the same validated
archive loader as Restore. It returns copied data in a
`UmiUiWorkspaceLibraryPreview`. Applications retain their existing workspace
owner and Data Server connection. A typical call looks like this:

```c
UmiUiWorkspaceLibraryPreview *preview = malloc(sizeof(*preview));
if (preview == NULL) return UMI_STATUS_OUT_OF_MEMORY;
UmiStatus status = umi_ui_workspace_library_checkpoint_preview(
    server, &scope, current_model, preview);
if (status == UMI_STATUS_OK) {
    printf("%zu added, %zu removed, %zu existing layouts changed\n",
        preview->comparison.added_count, preview->comparison.removed_count,
        preview->comparison.changed_count);
}
free(preview);
return status;
```

Include `umicom/ui/workspace_library_checkpoint.h`, `<stdlib.h>` and `<stdio.h>`.
This snippet belongs inside a function returning `UmiStatus`; it assumes an
existing server, scope and model. The output remains unchanged on failure.
Large candidate models are allocated on the heap internally. Call on the
owner thread, outside a caller-owned Data Server transaction.

The returned report is informational. Do not replace your cached Save revision
with the preview's revision: that would bypass a conflict with another writer.
Save and confirmed Restore retain their existing evidence rules.

For an in-memory comparison without storage, call
`umi_ui_workspace_library_compare` with two copied library snapshots. Each
result row contains before/after values and zero-based indexes. `SIZE_MAX`
marks an absent side. Rows follow the proposed order, followed by removed rows
in current order. The function validates both inputs before publishing output.
Its change flags describe list summaries only.

## 5. Check the feature

After configuring and building the normal Applications preset, run:

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
& "C:\msys64\ucrt64\bin\ctest.exe" --preset windows-ucrt64-all-debug -R 'framework\.workspace_library\.preview\.|framework\.ui_workstation\.layout\.preview\.|studio\.workspace\.canvas\.gtk4|trader\.module\.layout_library_order' --parallel 2 --no-tests=error --output-on-failure
```

Stop on failure. A skipped GTK test means the native workflow has not been
checked in that environment. The portable cases cover comparison boundaries,
unchanged output on failure, recovery, editing and transaction restrictions.
Native cases cover explicit dispatch, stale requests, malformed callbacks,
storage rebinding and destruction with queued work.
