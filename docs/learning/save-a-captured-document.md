# Save the document revision you actually wrote

A document can change while a storage provider is writing it. A provider is the
small adapter that stores bytes on disk, in memory, or in another service. Some
providers call application code before returning. That code might edit a draft,
close a tab, or start another action.

For example, a save begins with `First draft`. During the write, the editor
changes to `Second draft`. The write may succeed, but it only stored `First
draft`. Marking `Second draft` as saved would hide unsaved work.

Framework's document coordinator checks for these changes. Applications using
`UmiDocumentCoordinatorSaveAs`, ordinary Save, Save All, or a save session receive
the same protection. Keep using the coordinator when your application has a
workbench and document views.

## 1. Understand the two copies of text

The document store owns the working copy and its revision number. The editor's
view may also contain a newer draft. Before saving, the coordinator synchronises
that draft into the store. It then copies the text and its metadata together
under one store lock.

The provider receives this independent copy. Editing or closing a document does
not change the bytes already passed to the provider. After the provider returns,
the coordinator finds the same document by its stable ID and view ID. It checks
the visible text revision and the store snapshot before acknowledging success.

Closing a different tab can move entries in an array. The stable IDs ensure that
the save still belongs to the original document. Moving the caret or activating
another tab does not cancel an otherwise unchanged save.

## 2. Handle the result without losing the draft

Call the coordinator on its owning thread and inspect the returned status:

| Status | What to do |
| --- | --- |
| `UMI_STATUS_OK` | The provider succeeded and the captured document was acknowledged. |
| `UMI_STATUS_BUSY` | A provider callback tried to start another save through the same coordinator. Retry after the outer operation returns. |
| `UMI_STATUS_INVALID_STATE` | The draft, stored revision, path, saved state, external-change flag, or baseline changed, or the existing external-change check found a conflict. Keep the current work and inspect the file. |
| `UMI_STATUS_NOT_FOUND` | The target document or view is no longer available, or a required resource was missing. Do not apply completion to another tab. |
| `UMI_STATUS_ALREADY_EXISTS` | Another open document owns the destination. Choose a different path. |
| Other error | Report the specific provider, allocation, path, or presentation failure. Do not display a successful completion. |

A rejection after a successful provider call does **not** undo the file write.
The file can contain the earlier captured text while the editor retains the
newer draft. Review both versions; if needed, use Save As to preserve the draft
at a different destination. Do not clear the modified marker or blindly retry an
overwrite. The next normal save may report an external conflict because the
rejected completion did not update the coordinator's baseline.

Save All stops at the rejected document. Its completed count includes only the
earlier acknowledged saves. A save session records the rejection as a failed
step. Save All follows the document IDs present when it started: closing an
earlier tab cannot make it skip a later one, and newly opened tabs wait for the
next batch. If a still-pending document closes, the batch reports `NOT_FOUND`
and preserves its earlier completed count. Neither operation promises a
transaction across several files.

## 3. Use the store contract for a provider without a workbench

If you are implementing a reusable provider service, include these headers:

```c
#include "umicom/platform/document_store.h"
#include "umicom/document/saver.h"
```

The following function captures one revision, writes it using an existing
provider, and acknowledges only that revision:

```c
static UmiStatus SaveOneRevision(UmiDocumentStore *store, UmiDocumentId id,
    const UmiDocumentProvider *provider, const char *path)
{
    UmiDocumentSnapshot captured;
    char *text = NULL;
    UmiStatus status = UmiDocumentStoreCopySnapshot(store, id, &captured, &text);
    if (status != UMI_STATUS_OK) return status;

    UmiDocumentSaveOptions options = umi_document_save_options_default();
    status = umi_document_saver_write(provider, path, text, captured.length,
        &options, NULL);
    umi_document_store_free_text(text);

    if (status == UMI_STATUS_OK)
        status = UmiDocumentStoreMarkSavedSnapshot(store, &captured, path);
    return status;
}
```

Pass a pointer initially set to `NULL` for the text output. A failed copy leaves
both outputs unchanged. Free a successful copy with
`umi_document_store_free_text`, even when the provider fails. The snapshot is a
value, so it needs no separate destroy function. Keep it unchanged and use it
only with the same live store from which it was captured.

`UmiDocumentStoreMarkSavedSnapshot` checks the document ID, revision, saved
revision, text length, previous path, and external-change state while holding the
store lock. On success it updates the destination and name and marks the
captured revision saved. On rejection it leaves the store unchanged. An empty
new document is a valid snapshot, including its initial revision of zero.

This short function demonstrates acknowledgement only. Your service must still
check whether overwriting the destination is allowed, preserve its chosen text
encoding, and serialize writes to the same document or destination. The store
check is not an operating-system file lock and does not prevent an unrelated
program from changing the file. The coordinator supplies its existing encoding,
open-document destination checks, and external-change comparison for GUI use.

The older `umi_document_store_mark_saved_as` API remains available. Use it only
when your caller already guarantees that the current revision was the one
persisted. It has no captured revision to check.

## 4. Build and run the focused regression cases

From an already configured Applications checkout with `BUILD_TESTING=ON`, use
PowerShell:

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
& "C:\msys64\ucrt64\bin\cmake.exe" --build --preset windows-ucrt64-all-debug --target umicom-document-store-snapshot-test umicom-document-save-snapshot-test --parallel 2 -- -k 0
& "C:\msys64\ucrt64\bin\ctest.exe" --preset windows-ucrt64-all-debug -R '^framework\.document_saving\.(store_snapshot|save_snapshot)\.' --parallel 2 --no-tests=error --output-on-failure
```

The headless cases cover unchanged and empty saves, Unicode text and paths,
late edits, text changed and restored, closed tabs, tab compaction, nested saves,
destination ownership, provider failures, and Save All/session accounting.
They use an in-memory provider and do not write user files. Linux static-library
builds with GCC or Clang also register an allocation-failure case for snapshot
copying. This is not Windows allocation-failure coverage or a GUI acceptance test.

If CTest reports no matching tests, configure again with testing enabled and
check that the new test sources and CMake registration were both merged. If a
case fails, retain its output and the current draft instead of bypassing the
snapshot check. Provider callbacks must keep the coordinator, its store,
workbench, and provider instance alive until the outer call returns. This guard
does not make coordinator operations safe to call from several threads. Changes
to its store and views must also be serialized on their common owning thread.
