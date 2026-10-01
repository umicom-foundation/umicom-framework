# Undo and redo chart drawings

Drawing history remembers completed drawing changes in memory. It lets a chart
owner reverse a mistake and then reapply it. The trading workspace shares this
implementation with native Trader, so there is only one drawing registry.

## Try the workflow

1. Select an instrument and create a range or trend drawing.
2. Choose **Undo drawing**. The completed drawing disappears.
3. Choose **Redo drawing**. The same identity, coordinates and appearance return.
4. Select the drawing, choose **Load selected**, change its appearance, and apply
   it. Undo now restores the previous appearance without removing the drawing.
5. Choose **Save chart** when the displayed state is the one you want to keep.
   Saving leaves undo and redo available in the current session.

With the chart canvas focused, use Ctrl+Z to undo, Ctrl+Shift+Z or Ctrl+Y to redo.
Text fields retain their own keyboard handling. The history label names the next
operation and its instrument. History follows the time order of workspace edits:
select the instrument named in that label before reversing its step. Selecting a
different instrument does not discard history or undo a hidden drawing elsewhere.

## What a drawing step contains

Creation, movement, duplication, deletion, lock changes, visibility changes and
appearance changes are reversible. Hide all and Show all form one step each.
The original order of drawing objects returns when a deletion is undone. Changes
to selection metadata, locks and opaque styles are retained by the shared journal.
Trader's object dropdown selection alone does not create a history step.

Navigation, timeframes, market data, indicators, order drafts, orders, fills and
broker connections are outside drawing history. Undoing a drawing cannot cancel
an order. An unfinished gesture is not a completed history step; a successful
undo or redo cancels that gesture. Escape can cancel it directly.

An appearance draft stays in its fields after undo. If its captured drawing
revision has changed, apply is rejected. Choose **Load selected** again to review
the current drawing before applying an appearance.

## Saved charts and limits

History holds at most 64 steps and 8 MiB of retained change records per workspace.
Older steps are dropped when needed. A single oversized step fails before
changing drawings. Temporary candidate registries require additional memory;
the 8 MiB limit describes retained history, not total process memory.

History is not written into chart checkpoints and is lost when the workspace
closes. **Restore preview** changes both the drawings and their saved view, so a
successful restore clears drawing history. A failed restore leaves history
intact. Saving after undo stores the currently displayed drawings, even when a
redo remains available.

If another owner changes the raw drawing registry, the old history becomes stale.
Undo and redo reject it. The next real, successful drawing edit starts a new
history from the current state. Failed edits and unchanged values do not erase a
redo branch. A new successful edit after undo replaces that branch.

## Use the shared API

1. Include `umicom/trading/chart_history.h` when using a trading workspace.
2. Make edits with the existing `UmiTradingWorkspace*ChartDrawing*` operations.
   They record history automatically and keep the existing selection checks.
3. Call `UmiTradingWorkspaceDrawingHistory` and display the returned next-step
   label and instrument. Keep its `revision` with the displayed action.
4. Pass that revision and the selected instrument to
   `UmiTradingWorkspaceUndoDrawing` or `UmiTradingWorkspaceRedoDrawing`.
5. If the call reports changed evidence, refresh the display and ask for another
   explicit action. Do not silently retry against a newer history revision.

Non-trading chart hosts can include `umicom/chart/drawing_history.h`, create a
history around their registry, and use `UmiChartDrawingHistoryApply`. The callback
receives a temporary candidate registry. Use only registry operations on that
candidate, change the named pane, preserve the relative order of unchanged rows,
and return a status. Do not retain its pointer or perform external side effects.
Callback outputs are provisional until Apply succeeds.

Candidate validation and allocation complete before publication. Failure leaves
the live registry and journal unchanged. Undo and redo restore semantic values
but assign fresh revisions to changed rows so old editing controls cannot write
through restored records. Registry identity remains stable for existing callers.

Use one owning thread. Destroy the history before its borrowed registry. A host
that imports a complete view can call `UmiChartDrawingHistoryReset` to establish
an explicit history boundary, after checking that its full import can succeed.

Continue with [drawing appearance](CHART_DRAWING_APPEARANCE.md) and
[chart checkpoints](CHART_CHECKPOINTS.md).
