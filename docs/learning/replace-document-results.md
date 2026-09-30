# Replace one document's results safely

A language provider returns a list of findings, such as compiler messages or
completion suggestions. The next reply often replaces the previous list for
that document. Removing the old rows first is risky: an invalid later row can
leave the document with only part of its new results.

Framework offers `registry_replace_document` for this workflow. It checks the
whole incoming list and prepares a private replacement. A successful call
publishes that replacement once. A failed call leaves the previous records,
count and registry revision unchanged. Other documents keep their records.

## 1. Give each document and result an identity

A document ID identifies the document that owns the results. Each result also
has its own ID, unique across that registry. Use complete, terminated C strings
that fit the arrays in the public header. Do not shorten IDs to fit: different
identities can otherwise become the same stored value.

Start snapshots with `{0}`, fill their text and numeric fields, and check every
copy of input whose length is unknown. The [bounded snapshot lesson](validate-bounded-snapshots.md)
explains the terminator and capacity rules. A row in a replacement must have
the same `document_id` as the operation. A row cannot take an ID currently owned
by another document. That conflict returns `UMI_STATUS_PERMISSION_DENIED`.

## 2. Capture the current registry revision

The registry revision is a counter that changes when its records change. Read
it before preparing the replacement, on the thread that owns the registry:

```c
uint64_t expected = umi_language_diagnostic_registry_revision(diagnostics);
```

Later, pass that value to the replacement call. A different current revision
returns `UMI_STATUS_INVALID_STATE`, even for an empty replacement. Read the
current records and decide whether your input is still relevant before retrying.
Do not silently refresh the expected number to bypass a stale-data decision.

## 3. Publish the complete list

After creating the `diagnostics` registry and preparing `incoming` rows:

```c
UmiSnapshotBatchResult result;
UmiStatus status = umi_language_diagnostic_registry_replace_document(
    diagnostics, "main.c", expected, incoming, incoming_count, &result);
```

On success, the previous rows for `main.c` are gone and the new rows follow all
retained rows in input order. Other documents retain their relative order,
values and record revisions. Each removed and inserted row advances the
registry revision once. Input snapshots are borrowed only during the call;
Framework stores its own copies and normalises their size, API version and
record revision using the ordinary upsert rules.

`result.applied` is the input row count on success and zero on failure.
`result.rejected_index` identifies a failing row, or is `SIZE_MAX` for a
whole-operation error. The optional validation detail identifies malformed
text, duplicate IDs or a scope conflict. Keep this output separate from the
input and registry memory.

To publish an empty result, pass `NULL` and a count of zero. This removes only
the selected document's rows. If that document has no rows, it is a successful
no-op. Use this for an accepted empty provider reply, not for a transport error.

## 4. Handle failures while keeping useful results

| Status | What to do |
| --- | --- |
| `INVALID_ARGUMENT` | Check required pointers, the nonempty document ID and every snapshot text array. |
| `INVALID_STATE` | Read the latest registry state and reconsider the replacement. |
| `ALREADY_EXISTS` | Resolve repeated IDs within the incoming list. |
| `PERMISSION_DENIED` | Check document membership or an ID collision with another document. |
| `CAPACITY_EXCEEDED` | Check text lengths, final record count and revision limits. Existing rows of this document free slots for their replacements. |
| `OUT_OF_MEMORY` | Keep the previous results and retry after addressing memory pressure. |

The scoped operation allocates one complete temporary registry. It scans
bounded arrays and does not promise constant-time publication. All registry
access must be serialized by its owner. This is in-memory all-or-nothing
publication; it supplies no thread lock, disk transaction or crash recovery.

## 5. Follow the runnable example

[document_replace.c](../../examples/snapshot_contracts/document_replace.c)
creates diagnostics for two documents, rejects an incorrect document scope,
replaces one list, then clears only that list. After configuring the normal
Applications preset, build and run this focused lesson:

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
& "C:\msys64\ucrt64\bin\cmake.exe" --build --preset windows-ucrt64-all-debug --target umicom-document-replace-example --parallel 2
& "C:\msys64\ucrt64\bin\ctest.exe" --preset windows-ucrt64-all-debug -R '^framework\.snapshot_contracts\.document-example$' --no-tests=error --output-on-failure
```

Stop on any failure. The program prints `Other document retained; selected
document cleared.` on success. Use CTest's `-V` option to see successful output.
The example is also installed beside the bookmark lesson in
`share/umicom-framework/examples/snapshot_contracts`. Its SDK consumer target
links `Umicom::language`; use matching headers and libraries.

## Available scoped registries

| Public header directory | Headers without `.h` |
| --- | --- |
| `umicom/language` | completion, hover, signature, reference, symbol, diagnostic, code_action, semantic_token, inlay_hint, folding_range |
| `umicom/editor` | completion, diagnostic, symbol, code_action |

Each header documents its typed `registry_replace_document` function. Other
value registries may offer [batch upsert](import-registry-batches.md), which
inserts or replaces IDs without removing an existing document list.

## Language Runtime and Editor ownership

Language Runtime's service bridge converts a complete provider reply before
replacing its Language list. Oversized arrays, unterminated source fields and
IDs that cannot fit are rejected before publication. Semantic-token replies
must contain complete groups of five integers; position arithmetic cannot wrap.
Reference publication preserves results for other documents.

The Editor bridge separately converts Language snapshots and replaces the
corresponding Editor list. If Editor synchronization fails, its previous list
remains and Language keeps its authoritative result. Report the error and retry
Editor synchronization when appropriate. These are two separate publications;
there is no transaction spanning both owners.

Provider reply freshness still depends on request and document-version
correlation by the calling service. Capturing a registry revision at publication
time does not prove that a server reply describes the latest text. These model
contracts also do not establish that a particular GUI is connected to a server.
