# Chart checkpoints for application developers

A checkpoint is a saved copy that an application can inspect before restoring.
Framework separates three responsibilities: an immutable chart document, a
transactional storage adapter, and a trading review coordinator. This lets an
application reuse the same implementation without owning a second drawing store.

## Capture a document

1. Keep all chart registry and workspace access on their owning thread.
2. Call `UmiChartDocumentCapture` with a registry, pane identity and navigation.
   A pane identity is a stable instrument or chart identifier, not a file path.
3. Read the result through `UmiChartDocumentGetSummary` and
   `UmiChartDocumentDrawingAt`. These return copies.
4. Destroy the document with `UmiChartDocumentDestroy` when finished.

The document owns its drawings. Later changes to the registry do not change it.
It records both point coordinates, style text, selected and locked flags, drawing
identity, navigation and source revision. Zero drawings is a valid capture.
The capacity is the chart registry's full 4,096 drawings, across all panes.

For a trading workspace, use `UmiTradingWorkspaceCaptureChart`. It reads a known
instrument without changing the selected instrument, order ticket or prices.
`UmiTradingWorkspaceRestoreChart` checks the live drawing revision and navigation
before replacing both together. It rejects tools Trader cannot render rather
than dropping those drawings. Current tools are trend, support and resistance.

## Use persistent storage

1. Open a Data Server explicitly. SQLite file storage survives a restart;
   the memory backend and SQLite `:memory:` do not.
2. Choose a stable scope identity, such as the application identity. Scope and
   pane each allow 1 through 127 bytes and together identify one saved chart.
3. Load the chart with `UmiChartCheckpointLoad` before replacing an existing save.
   A fresh namespace returns `UMI_STATUS_NOT_FOUND` and a known revision of zero.
4. Save using the last reviewed storage revision. A successful save advances
   that revision. Keep the returned token for the next save.
5. Treat a revision mismatch as a conflict. Load and review the competing copy
   before deciding whether to replace it. Do not automatically retry with the
   competing revision.

The adapter borrows the server. Close it only after borrowers are unbound or
destroyed. Checkpoint operations own one complete transaction and reject a
caller-owned active transaction with `UMI_STATUS_BUSY`. Failed writes roll back
both the new primary and the previous-valid-copy update.

Each drawing occupies a bounded record. The manifest records its count, view,
scope, save time and revisions. A digest covers the exact stored metadata and
all expected drawings. Loading checks the complete document before returning
it. Encoding preserves binary64 coordinates exactly, including negative zero,
and supports the full signed timestamp range. Non-finite values are rejected.
An unsupported double representation returns `UMI_STATUS_UNAVAILABLE`.

The memory backend's total record limit is smaller than two full chart saves.
Capacity failure is explicit and rolls back. Use file-backed SQLite for a
durable full-capacity workspace. Database busy, I/O and allocation errors remain
errors; they do not silently select an older copy.

## Add reviewed trading actions

1. Create `UmiTradingChartPersistence` with a workspace that outlives it.
2. Bind a borrowed server and scope. Binding performs no I/O and clears old
   save tokens and previews; it does not change the chart.
3. Connect Save to `UmiTradingChartPersistenceSave`. A first save can create a
   fresh chart, but refuses an existing stored chart until explicitly previewed.
4. Connect Preview to `UmiTradingChartPersistencePreview`. Display its summary
   and use `UmiTradingChartPersistencePreviewDrawing` for copied drawing details.
5. Pass the preview identity to Restore. Verify the displayed instrument is
   still selected before calling from a native control.

Restore rereads storage, compares the reviewed digest and revision, then checks
the live drawing revision and view. Another panel's preview cannot substitute
its content behind an older Restore button. The coordinator is synchronous on
the owner thread; it does not lock out another process after storage validation.
Unrelated instruments and trading data are not replaced. A successful restore
gives drawings fresh local revisions while retaining their stable identities.

## Native ownership and profiles

The GTK trading suite owns one coordinator. Profile storage uses the existing
user-local layout database through a separate connection and chart namespace.
Opening profile storage does not automatically restore any chart. Save, Preview
and Restore remain separate actions; no database work runs on the chart timer.

A chart widget borrows its context and persistence service. Call
`UmiGtk4TradingInteractiveChartDetach` before either owner dies. The suite and
provider mount do this automatically for their charts. Detach stops the timer
and makes retained controls inert while the copied render scene can remain alive.
Standalone hosts must follow the same rule.

## Recovery boundaries

A valid former primary becomes the previous valid copy on the next successful
save. Missing or malformed primary records can recover this older document.
`recovered_last_good` identifies that result, and `checkpoint_revision` reports
the recovered document's saved revision. Recovery is read-only and does not
make the primary's revision trustworthy: check `storage_revision_known` before
offering Save. A missing primary with a surviving backup cannot restart the
revision counter. Damaged primary storage requires a separate repair workflow;
this API intentionally does not provide one.

The digest detects accidental corruption. It is not a signature or permission
check. Scope separation does not provide encryption. Credentials, broker state,
order data, candles and global indicator choices are not part of this format.

## Saved chart timeframes

View metadata now includes the fixed UTC interval. Old views open in Source
mode. [Chart timeframes](CHART_TIMEFRAMES.md) explains aggregation, compatibility,
explicit saves and restoring a view when provider data has changed.
