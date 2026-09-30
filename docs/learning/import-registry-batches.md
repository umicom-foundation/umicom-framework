# Import several snapshots together

An upsert inserts a new identifier or replaces the record with that identifier.
A batch upsert applies several such changes together. This is useful when a
service imports a set of bookmarks, debugger records, project settings or UI
model entries: an invalid later row should not leave an earlier row published.

Framework's `registry_upsert_many` functions first validate the input, then
apply normal upsert operations to a temporary copy of the registry. They publish
that copy only after every row succeeds. On failure, the original records,
count and revision stay unchanged.

## 1. Follow the bookmark lesson

The complete source is [main.c](../../examples/snapshot_contracts/main.c). Read
it alongside the [bounded-text introduction](validate-bounded-snapshots.md).
The example:

1. Creates a bookmark registry and prepares two input snapshots.
2. Fills the second label without a terminator to simulate a producer error.
3. Calls `umi_platform_bookmarks_registry_upsert_many` and checks that the
   registry is still empty.
4. Corrects the second label and retries the same two-row import.
5. Looks up the imported bookmark, prints its label and destroys the registry.

The location strings are examples. This program does not open those locations
or save files.

In your code, the central call looks like this after you have created a valid
`bookmarks` registry and initialised two `incoming` snapshots:

```c
UmiSnapshotBatchResult result;
UmiStatus status = umi_platform_bookmarks_registry_upsert_many(
    bookmarks, incoming, 2U, &result);
if (status != UMI_STATUS_OK) {
    if (result.rejected_index != SIZE_MAX) {
        fprintf(stderr, "Rejected input row %zu\n", result.rejected_index + 1U);
    }
    if (result.validation.issue != UMI_SNAPSHOT_VALID) {
        fprintf(stderr, "%s: %s\n",
            result.validation.field != NULL ? result.validation.field : "snapshot",
            UmiSnapshotIssueText(result.validation.issue));
    } else {
        fprintf(stderr, "%s\n", umi_status_text(status));
    }
}
```

`applied` is the number of input rows on success and zero on every failure.
`rejected_index` is zero-based. It is `SIZE_MAX` on success or when the error
concerns the whole operation, such as failing to allocate the temporary copy.
The optional output is initialised on every return. Keep it separate from the
input array and registry storage.

## 2. Build and run the lesson in Applications

From an Applications checkout that contains these Framework sources, use
PowerShell with the MSYS2 UCRT64 tools installed. This is a focused way to build
the lesson after configuring the normal Applications preset:

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
& "C:\msys64\ucrt64\bin\cmake.exe" --preset windows-ucrt64-all-debug -DBUILD_TESTING=ON
& "C:\msys64\ucrt64\bin\cmake.exe" --build --preset windows-ucrt64-all-debug --target umicom-snapshot-batch-example --parallel 2
& "C:\msys64\ucrt64\bin\ctest.exe" --preset windows-ucrt64-all-debug -R '^framework\.snapshot_contracts\.example$' --no-tests=error --output-on-failure
```

Run one command at a time and stop if it fails. Configuration still needs the
dependencies selected by that preset. Expected lesson output is:

```text
Row 2, field label: text field has no terminator within its capacity
Imported 2 bookmarks. Second label: Beginner manual
```

CTest hides successful program output by default. Add `-V` to its command if you
want to see these lines. To exercise all the new registry consumers, build and
run their dedicated group:

```powershell
& "C:\msys64\ucrt64\bin\cmake.exe" --build --preset windows-ucrt64-all-debug --target umicom-snapshot-contract-checks --parallel 2
& "C:\msys64\ucrt64\bin\ctest.exe" --preset windows-ucrt64-all-debug -L snapshot-contracts --parallel 2 --no-tests=error --output-on-failure
```

These checks cover text boundaries, stored-copy ownership, invalid input,
duplicate identifiers, batch rollback, ordering, capacity and retry. The shared
allocator-failure check additionally needs a Linux GNU/Clang static-library
build with linker wrapping; it is not part of Windows qualification.

## 3. Use the example with an installed SDK

The full Framework installation includes the example under
`share/umicom-framework/examples/snapshot_contracts`. Copy that directory to a
working directory you own. Configure it against a full SDK built with a
compatible compiler and architecture. For example, if your SDK is installed
at `C:\umicom-sdk` and the copied example is at `C:\umicom-lessons\snapshot_contracts`:

```powershell
Set-Location "C:\umicom-lessons\snapshot_contracts"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
& "C:\msys64\ucrt64\bin\cmake.exe" -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe -DCMAKE_PREFIX_PATH=C:/umicom-sdk
& "C:\msys64\ucrt64\bin\cmake.exe" --build build --parallel 2
& "C:\msys64\ucrt64\bin\ctest.exe" --test-dir build --no-tests=error --output-on-failure
```

The example links to `Umicom::platform`; its dependency on `Umicom::base` brings
in the shared text validator. A small SDK built by an unrelated focused example
may not include these APIs. An unknown symbol usually means the library is older
than the headers, or the wrong SDK was selected. Rebuild and install matching
headers and libraries, then configure a fresh consumer build directory.

## 4. Choose the right batch size and error handling

| Situation | Result and next step |
| --- | --- |
| Every row is valid and fits | `UMI_STATUS_OK`; all rows are published. |
| A text array is unterminated or the identifier is empty | `UMI_STATUS_INVALID_ARGUMENT`; use the row and field diagnostic to fix the producer. |
| Two input rows have the same identifier | `UMI_STATUS_ALREADY_EXISTS`; the second occurrence is rejected. Decide which value belongs in the import. |
| An identifier already exists in the registry | Its row is replaced in its existing position. This is a normal successful upsert. |
| The batch exceeds the registry's input limit or there is no room for a new ID | `UMI_STATUS_CAPACITY_EXCEEDED`; review the existing records and the intended import size. |
| The upsert revision counter cannot advance without wrapping | `UMI_STATUS_CAPACITY_EXCEEDED`; this import is rejected without changing the registry. |
| The temporary registry cannot be allocated | `UMI_STATUS_OUT_OF_MEMORY`; keep the original input and retry after addressing memory pressure. |
| Zero rows and a valid registry | Success without allocating; the input pointer may be `NULL`. |
| A null registry, or a null input pointer with nonzero count | `UMI_STATUS_INVALID_ARGUMENT`. |

Every batch is limited to the capacity constant in its public header, even if
all its rows would replace existing identifiers. Replacement does not consume
another slot. New identifiers are appended in input order. The registry's
revision advances once for each accepted row, just as it does for individual
upserts. Identifier matching uses the existing case-sensitive byte comparison.
Text validation and duplicate detection run before any staged upsert, so an
input error can take precedence over a later storage-capacity error.

The temporary copy occupies roughly another registry's worth of memory. Lookup
and duplicate checks scan bounded arrays; this interface does not promise
constant-time imports. Splitting an import into smaller batches reduces each
batch's input size, but each still copies the entire registry and only that
individual batch is atomic.

Keep the input stable during the call and serialise registry access on its
owning thread. Here, *atomic* means that a returning call has published all its
rows or none. It does not provide thread synchronisation, crash recovery,
filesystem transactions or rollback of external operations. Importing a
source-control snapshot does not execute Git; importing a file-operation
snapshot does not move a file.

## Available registries

Each listed header exports its existing single-record API plus a matching
`snapshot_validate` and `registry_upsert_many` function. For example,
`umicom/debug/breakpoint.h` provides `umi_debug_breakpoint_snapshot_validate`
and `umi_debug_breakpoint_registry_upsert_many`.

| Header directory and library | Registry headers, without `.h` |
| --- | --- |
| `umicom/platform`, `Umicom::platform` | bookmarks, file_operation_queue, resource_location, workspace_history |
| `umicom/ui`, `Umicom::ui` | command_history, command_surface, context_menu, dock_model, drag_drop, extension_point, list_model, navigation_stack, notification_item, output_channel, panel_model, problem, progress, property_inspector, selection_model, sort_filter_model, status_item, tab_model, task_monitor, tree_model, view_state, welcome_view |
| `umicom/project`, `Umicom::project` | build_node, capability, configuration, dependency, descriptor, environment, launch_profile, reference, target, task, template, variable |
| `umicom/source_control`, `Umicom::source_control` | branch, change, change_set, commit, diff_session, history_entry, operation, remote, repository, staging, tag |
| `umicom/debug`, `Umicom::debug` | breakpoint, console_entry, event, exception, launch_configuration, module, scope, session, source, stack_frame, thread, variable, watch |
| `umicom/frontend`, `Umicom::frontend` | binding, render_tree, signal, transport, web_session, web_style, widget_tree |

| `umicom/chart`, `Umicom::chart` | annotation, crosshair, drawing, extension, marker, pane, scale, stream |
| `umicom/designer`, `Umicom::designer` | action_binding, alignment, clipboard, property_schema, signal_binding, template_palette |
| `umicom/editor`, `Umicom::editor` | code_action, completion, configuration, cursor, diagnostic, diff_hunk, document, fold_region, marker, selection_range, symbol |
| `umicom/language`, `Umicom::language` | code_action, completion, definition, diagnostic, document, folding_range, formatting, hover, inlay_hint, provider, reference, rename, semantic_token, signature, symbol |
| `umicom/product`, `Umicom::product` | installation_state, marketplace, metadata_provider, update_policy |
| `umicom/test_platform`, `Umicom::test_platform` | attachment, benchmark, coverage, output, result, run_profile, run_session |

For complete replacement of one document's provider results, follow
[Replace one document's results safely](replace-document-results.md).

These are value-only registries: records contain fixed arrays and scalar values,
not owned pointers or live resource handles. Other stores, including recent
items and project file sets, retain their specialised interfaces and validation.
