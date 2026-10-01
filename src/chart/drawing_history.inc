/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/drawing_history.inc
 * PURPOSE: Publish edits and sparse reversible evidence together inside the canonical registry owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/drawing_history.h"
typedef struct DrawingHistoryChange {
    int has_before, has_after;
    size_t before_index, after_index;
    UmiChartDrawingSnapshot before, after;
} DrawingHistoryChange;
typedef struct DrawingHistoryStep {
    char pane[128], label[96];
    size_t count, bytes;
    DrawingHistoryChange changes[];
} DrawingHistoryStep;
struct UmiChartDrawingHistory {
    UmiChartDrawingRegistry *registry;
    uint64_t revision, known_revision;
    DrawingHistoryStep *steps[UMI_CHART_DRAWING_HISTORY_STEPS];
    size_t count, cursor, bytes;
    int editing;
};
static int DrawingHistoryText(const char *text, size_t capacity)
{
    if (text == NULL) return 0;
    for (size_t i = 0U; i < capacity; ++i) if (text[i] == '\0') return i != 0U;
    return 0;
}
/* Compare fields, not structure padding or historical revision numbers.
 * Opaque style and fixed text bytes are retained exactly, including their tails. */
static int DrawingHistorySame(const UmiChartDrawingSnapshot *a, const UmiChartDrawingSnapshot *b)
{
    return memcmp(a->id, b->id, sizeof a->id) == 0 && memcmp(a->pane_id, b->pane_id, sizeof a->pane_id) == 0 &&
        memcmp(a->tool, b->tool, sizeof a->tool) == 0 && memcmp(a->style, b->style, sizeof a->style) == 0 &&
        a->time1 == b->time1 && a->time2 == b->time2 &&
        memcmp(&a->value1, &b->value1, sizeof a->value1) == 0 && memcmp(&a->value2, &b->value2, sizeof a->value2) == 0 &&
        a->selected == b->selected && a->locked == b->locked && a->visibility_flags == b->visibility_flags;
}
static void DrawingHistoryTruncate(UmiChartDrawingHistory *history, size_t count)
{
    while (history->count > count) {
        DrawingHistoryStep *step = history->steps[--history->count];
        history->bytes -= step->bytes; free(step); history->steps[history->count] = NULL;
    }
    if (history->cursor > count) history->cursor = count;
}
UmiStatus UmiChartDrawingHistoryCreate(UmiChartDrawingRegistry *registry, UmiChartDrawingHistory **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiChartDrawingHistory *history = calloc(1, sizeof *history);
    if (history == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    history->registry = registry; history->revision = 1U; history->known_revision = registry->revision;
    *out = history; return UMI_STATUS_OK;
}
void UmiChartDrawingHistoryDestroy(UmiChartDrawingHistory *history)
{
    if (history == NULL) return;
    DrawingHistoryTruncate(history, 0U); free(history);
}
UmiStatus UmiChartDrawingHistoryRead(const UmiChartDrawingHistory *history, UmiChartDrawingHistorySnapshot *out)
{
    if (history == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (history->editing) return UMI_STATUS_BUSY;
    UmiChartDrawingHistorySnapshot result = {0};
    result.revision = history->revision; result.registry_revision = history->registry->revision;
    result.undo_count = history->cursor; result.redo_count = history->count - history->cursor;
    result.retained_bytes = history->bytes; result.stale = history->known_revision != history->registry->revision;
    if (history->cursor != 0U) {
        const DrawingHistoryStep *step = history->steps[history->cursor - 1U];
        memcpy(result.undo_pane, step->pane, sizeof result.undo_pane); memcpy(result.undo_label, step->label, sizeof result.undo_label);
    }
    if (history->cursor < history->count) {
        const DrawingHistoryStep *step = history->steps[history->cursor];
        memcpy(result.redo_pane, step->pane, sizeof result.redo_pane); memcpy(result.redo_label, step->label, sizeof result.redo_label);
    }
    *out = result; return UMI_STATUS_OK;
}
UmiStatus UmiChartDrawingHistoryReset(UmiChartDrawingHistory *history)
{
    if (history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (history->editing) return UMI_STATUS_BUSY;
    if (history->count == 0U && history->known_revision == history->registry->revision) return UMI_STATUS_OK;
    if (history->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    DrawingHistoryTruncate(history, 0U); history->known_revision = history->registry->revision;
    ++history->revision; return UMI_STATUS_OK;
}
static UmiStatus DrawingHistoryDifference(const UmiChartDrawingRegistry *before,
    const UmiChartDrawingRegistry *after, const char *pane, size_t *outCount)
{
    size_t count = 0U, lastUnchanged = 0U; int haveUnchanged = 0;
    if (before->count > UMI_CHART_DRAWING_CAPACITY || after->count > UMI_CHART_DRAWING_CAPACITY) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < before->count; ++i) {
        const UmiChartDrawingSnapshot *a = &before->items[i]; size_t at = find_index(after, a->id);
        if (at == SIZE_MAX || !DrawingHistorySame(a, &after->items[at])) {
            if (strcmp(a->pane_id, pane) != 0 || (at != SIZE_MAX && strcmp(after->items[at].pane_id, pane) != 0))
                return UMI_STATUS_INVALID_ARGUMENT;
            ++count;
        } else {
            /* Sparse replay retains the relative order of unaffected rows. */
            if (haveUnchanged && at < lastUnchanged) return UMI_STATUS_INVALID_ARGUMENT;
            lastUnchanged = at; haveUnchanged = 1;
        }
    }
    for (size_t i = 0U; i < after->count; ++i) {
        const UmiChartDrawingSnapshot *a = &after->items[i];
        if (umi_chart_drawing_snapshot_validate(a, NULL) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
        if (find_index(before, a->id) == SIZE_MAX) {
            if (strcmp(a->pane_id, pane) != 0) return UMI_STATUS_INVALID_ARGUMENT;
            ++count;
        }
    }
    *outCount = count; return UMI_STATUS_OK;
}
UmiStatus UmiChartDrawingHistoryApply(UmiChartDrawingHistory *history, const char *pane,
    const char *label, UmiChartDrawingHistoryEdit edit, void *userData, int *outChanged)
{
    if (outChanged != NULL) *outChanged = 0;
    if (history == NULL || edit == NULL || !DrawingHistoryText(pane, 128U) || !DrawingHistoryText(label, 96U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (history->editing) return UMI_STATUS_BUSY;
    UmiChartDrawingRegistry *live = history->registry;
    if (history->revision == UINT64_MAX || live->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiChartDrawingRegistry *candidate = malloc(sizeof *candidate);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *candidate = *live; uint64_t beforeRevision = live->revision;
    history->editing = 1;
    UmiStatus status = edit(candidate, userData);
    history->editing = 0;
    if (live->revision != beforeRevision) status = UMI_STATUS_BUSY;
    size_t count = 0U;
    if (status == UMI_STATUS_OK) status = DrawingHistoryDifference(live, candidate, pane, &count);
    if (status != UMI_STATUS_OK || count == 0U) { free(candidate); return status; }
    if (candidate->revision <= beforeRevision) { free(candidate); return UMI_STATUS_INVALID_STATE; }
    size_t bytes = sizeof(DrawingHistoryStep) + count * sizeof(DrawingHistoryChange);
    if (bytes > UMI_CHART_DRAWING_HISTORY_BYTES) { free(candidate); return UMI_STATUS_CAPACITY_EXCEEDED; }
    DrawingHistoryStep *step = calloc(1, bytes);
    if (step == NULL) { free(candidate); return UMI_STATUS_OUT_OF_MEMORY; }
    strcpy(step->pane, pane); strcpy(step->label, label); step->bytes = bytes;
    for (size_t i = 0U; i < live->count; ++i) {
        const UmiChartDrawingSnapshot *a = &live->items[i]; size_t at = find_index(candidate, a->id);
        if (at != SIZE_MAX && DrawingHistorySame(a, &candidate->items[at])) continue;
        DrawingHistoryChange *change = &step->changes[step->count++];
        change->has_before = 1; change->before = *a; change->before_index = i;
        if (at != SIZE_MAX) { change->has_after = 1; change->after = candidate->items[at]; change->after_index = at; }
    }
    for (size_t i = 0U; i < candidate->count; ++i) {
        if (find_index(live, candidate->items[i].id) != SIZE_MAX) continue;
        DrawingHistoryChange *change = &step->changes[step->count++];
        change->has_after = 1; change->after = candidate->items[i]; change->after_index = i;
    }
    /* No allocation or fallible operation follows this point. Publish history
     * and registry together, preserving object identity for existing callers. */
    DrawingHistoryTruncate(history, history->known_revision == beforeRevision ? history->cursor : 0U);
    while (history->count >= UMI_CHART_DRAWING_HISTORY_STEPS || history->bytes > UMI_CHART_DRAWING_HISTORY_BYTES - bytes) {
        history->bytes -= history->steps[0]->bytes; free(history->steps[0]);
        memmove(history->steps, history->steps + 1, (--history->count) * sizeof history->steps[0]);
    }
    history->steps[history->count++] = step; history->bytes += bytes; history->cursor = history->count;
    *live = *candidate; free(candidate); history->known_revision = live->revision; ++history->revision;
    if (outChanged != NULL) *outChanged = 1;
    return UMI_STATUS_OK;
}
static const DrawingHistoryChange *DrawingHistoryFind(const DrawingHistoryStep *step, const char *id)
{
    for (size_t i = 0U; i < step->count; ++i) {
        const DrawingHistoryChange *change = &step->changes[i];
        const char *name = change->has_before ? change->before.id : change->after.id;
        if (strcmp(name, id) == 0) return change;
    }
    return NULL;
}
static UmiStatus DrawingHistoryReplay(UmiChartDrawingHistory *history, const char *pane,
    uint64_t expectedRevision, int redo)
{
    if (history == NULL || !DrawingHistoryText(pane, 128U)) return UMI_STATUS_INVALID_ARGUMENT;
    if (history->editing || history->revision != expectedRevision || history->known_revision != history->registry->revision)
        return UMI_STATUS_BUSY;
    if ((!redo && history->cursor == 0U) || (redo && history->cursor == history->count)) return UMI_STATUS_NOT_FOUND;
    if (history->revision == UINT64_MAX || history->registry->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    const DrawingHistoryStep *step = history->steps[redo ? history->cursor : history->cursor - 1U];
    if (strcmp(pane, step->pane) != 0) return UMI_STATUS_INVALID_STATE;
    UmiChartDrawingRegistry *live = history->registry;
    size_t targetCount = live->count;
    for (size_t i = 0U; i < step->count; ++i) {
        const DrawingHistoryChange *change = &step->changes[i];
        const UmiChartDrawingSnapshot *source = redo ? &change->before : &change->after;
        int hasSource = redo ? change->has_before : change->has_after;
        int hasTarget = redo ? change->has_after : change->has_before;
        const char *id = change->has_before ? change->before.id : change->after.id;
        size_t at = find_index(live, id);
        if ((hasSource && (at == SIZE_MAX || !DrawingHistorySame(source, &live->items[at]))) || (!hasSource && at != SIZE_MAX))
            return UMI_STATUS_BUSY;
        if (hasSource && !hasTarget) --targetCount;
        else if (!hasSource && hasTarget) ++targetCount;
    }
    if (targetCount > UMI_CHART_DRAWING_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiChartDrawingRegistry *candidate = calloc(1, sizeof *candidate);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    candidate->revision = live->revision + 1U;
    size_t next = 0U;
    for (size_t i = 0U; i < targetCount; ++i) {
        const UmiChartDrawingSnapshot *replacement = NULL;
        for (size_t j = 0U; j < step->count; ++j) {
            const DrawingHistoryChange *change = &step->changes[j];
            if ((redo ? change->has_after : change->has_before) && (redo ? change->after_index : change->before_index) == i) {
                replacement = redo ? &change->after : &change->before; break;
            }
        }
        UmiChartDrawingSnapshot *out = &candidate->items[candidate->count++];
        if (replacement != NULL) { *out = *replacement; out->revision = candidate->revision; }
        else {
            while (next < live->count && DrawingHistoryFind(step, live->items[next].id) != NULL) ++next;
            if (next == live->count) { free(candidate); return UMI_STATUS_INVALID_STATE; }
            *out = live->items[next++];
        }
    }
    *live = *candidate; free(candidate);
    if (redo) ++history->cursor; else --history->cursor;
    history->known_revision = live->revision; ++history->revision; return UMI_STATUS_OK;
}
UmiStatus UmiChartDrawingHistoryUndo(UmiChartDrawingHistory *history, const char *pane, uint64_t expectedRevision)
{ return DrawingHistoryReplay(history, pane, expectedRevision, 0); }
UmiStatus UmiChartDrawingHistoryRedo(UmiChartDrawingHistory *history, const char *pane, uint64_t expectedRevision)
{ return DrawingHistoryReplay(history, pane, expectedRevision, 1); }
