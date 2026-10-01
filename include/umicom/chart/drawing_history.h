/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/drawing_history.h
 * PURPOSE: Provide bounded atomic undo and redo for canonical chart drawing edits.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CHART_DRAWING_HISTORY_H
#define UMICOM_CHART_DRAWING_HISTORY_H
#include "umicom/chart/drawing.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CHART_DRAWING_HISTORY_STEPS 64U
#define UMI_CHART_DRAWING_HISTORY_BYTES (8U * 1024U * 1024U)
typedef struct UmiChartDrawingHistory UmiChartDrawingHistory;
typedef struct UmiChartDrawingHistorySnapshot {
    uint64_t revision, registry_revision;
    size_t undo_count, redo_count, retained_bytes;
    int stale;
    char undo_pane[128], redo_pane[128];
    char undo_label[96], redo_label[96];
} UmiChartDrawingHistorySnapshot;
/** An edit receives a private candidate registry with the same rows/revisions
 * as the live registry. Use the existing bounded registry/edit operations.
 * The candidate is borrowed: do not destroy it. Change only the named pane;
 * do not retain the candidate, reorder unchanged
 * records, access the live registry, recurse, perform I/O or invoke host callbacks.
 * Failure discards the candidate. Output through userData is provisional until
 * Apply succeeds; a host must not publish it earlier. */
typedef UmiStatus (*UmiChartDrawingHistoryEdit)(UmiChartDrawingRegistry *candidate, void *userData);
/** One owner thread, no concurrent or reentrant access. The registry outlives
 * the history. The history borrows it; destroy history before the registry. */
UmiStatus UmiChartDrawingHistoryCreate(UmiChartDrawingRegistry *registry, UmiChartDrawingHistory **out);
void UmiChartDrawingHistoryDestroy(UmiChartDrawingHistory *history);
UmiStatus UmiChartDrawingHistoryRead(const UmiChartDrawingHistory *history,
    UmiChartDrawingHistorySnapshot *out);
/** Atomically edit the live registry and retain only changed before/after rows.
 * label is nonempty and at most 95 bytes. Allocation and validation finish before
 * publication. No-op edits retain the live revision and redo branch. Successful
 * edits discard redo and evict oldest steps to honor both bounds above.
 * An external registry mutation makes old history stale; the next real successful
 * edit starts a fresh history. A failed/no-op edit does not erase old history.
 * outChanged is optional, set to zero on failure/no-op, and must not alias inputs. */
UmiStatus UmiChartDrawingHistoryApply(UmiChartDrawingHistory *history, const char *pane,
    const char *label, UmiChartDrawingHistoryEdit edit, void *userData, int *outChanged);
/** Reverse/reapply exactly the most recent workspace drawing step. The caller
 * supplies the displayed history revision and must select the step's pane.
 * Another pane returns INVALID_STATE, stale evidence BUSY, no step NOT_FOUND.
 * Changed rows receive fresh revisions; unaffected rows retain theirs. Original
 * IDs, ordering, geometry, opaque style, selection, locks and visibility return.
 * Undo intentionally reverses a recorded lock/hidden change without executing
 * a new geometry edit. It never overrides an unrecorded external mutation. */
UmiStatus UmiChartDrawingHistoryUndo(UmiChartDrawingHistory *history, const char *pane, uint64_t expectedRevision);
UmiStatus UmiChartDrawingHistoryRedo(UmiChartDrawingHistory *history, const char *pane, uint64_t expectedRevision);
/** Clear history after an explicit restore/import barrier. No drawing changes.
 * Repeated empty reset at the same registry revision is a no-op. */
UmiStatus UmiChartDrawingHistoryReset(UmiChartDrawingHistory *history);
#ifdef __cplusplus
}
#endif
#endif
