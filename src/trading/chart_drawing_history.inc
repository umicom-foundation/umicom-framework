/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/chart_drawing_history.inc
 * PURPOSE: Commit canonical drawing operations through one atomic shared journal.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

typedef enum ChartHistoryAction { CH_ADD, CH_LOCK, CH_MOVE, CH_DUPLICATE, CH_REMOVE, CH_HIDE, CH_HIDE_PANE, CH_APPEARANCE } ChartHistoryAction;
typedef struct ChartHistoryRequest {
    ChartHistoryAction action;
    const char *pane, *id;
    uint64_t expected;
    int flag;
    UmiChartPoint first, second;
    UmiChartDrawingKind kind;
    const UmiChartDrawingAppearance *appearance;
    char new_id[128];
    size_t output_capacity, changed;
} ChartHistoryRequest;
static UmiStatus ChartHistoryCandidate(UmiChartDrawingRegistry *registry, void *data)
{
    ChartHistoryRequest *r = data; UmiChartDrawingSnapshot drawing; UmiStatus status;
    switch (r->action) {
    case CH_ADD:
        status = NextChartDrawingId(registry, r->new_id);
        if (status == UMI_STATUS_OK) status = UmiChartDrawingInitialize(r->new_id, r->pane, r->kind, r->first, r->second, &drawing);
        return status == UMI_STATUS_OK ? umi_chart_drawing_registry_upsert(registry, &drawing) : status;
    case CH_LOCK: return UmiChartDrawingSetLocked(registry, r->pane, r->id, r->expected, r->flag);
    case CH_MOVE: return UmiChartDrawingSetGeometry(registry, r->pane, r->id, r->expected, r->first, r->second);
    case CH_DUPLICATE:
        status = NextChartDrawingId(registry, r->new_id);
        if (status != UMI_STATUS_OK) return status;
        if (strlen(r->new_id) + 1U > r->output_capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        return UmiChartDrawingDuplicate(registry, r->pane, r->id, r->expected, r->new_id);
    case CH_REMOVE:
        if (r->id == NULL || r->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
        status = umi_chart_drawing_registry_find(registry, r->id, &drawing);
        if (status != UMI_STATUS_OK) return status;
        if (drawing.locked || drawing.revision != r->expected || strcmp(drawing.pane_id, r->pane) != 0 ||
            umi_chart_drawing_registry_revision(registry) == UINT64_MAX) return UMI_STATUS_INVALID_STATE;
        return umi_chart_drawing_registry_remove(registry, r->id);
    case CH_HIDE: return UmiChartDrawingSetHidden(registry, r->pane, r->id, r->expected, r->flag);
    case CH_HIDE_PANE: return UmiChartDrawingSetPaneHidden(registry, r->pane, r->expected, r->flag, &r->changed);
    case CH_APPEARANCE: return UmiChartDrawingSetAppearance(registry, r->pane, r->id, r->expected, r->appearance);
    }
    return UMI_STATUS_INVALID_ARGUMENT;
}
static UmiStatus ChartHistoryApply(UmiTradingWorkspace *workspace, ChartHistoryRequest *request, const char *label)
{
    size_t index; UmiStatus status = ChartSelectedIndex(workspace, request->pane, &index);
    if (status != UMI_STATUS_OK) return status;
    (void)index;
    if (workspace->revision == UINT64_MAX) return UMI_STATUS_INVALID_STATE;
    UmiChartDrawingHistory *history = workspace->drawing_history;
    int created = history == NULL, changed = 0;
    if (created) status = UmiChartDrawingHistoryCreate(umi_chart_workspace_drawings(workspace->charts), &history);
    if (status == UMI_STATUS_OK) status = UmiChartDrawingHistoryApply(history, request->pane, label, ChartHistoryCandidate, request, &changed);
    if (status == UMI_STATUS_OK && changed) { workspace->drawing_history = history; ++workspace->revision; }
    else if (created) UmiChartDrawingHistoryDestroy(history);
    return status;
}
UmiStatus UmiTradingWorkspaceDrawingHistory(const UmiTradingWorkspace *workspace, UmiChartDrawingHistorySnapshot *out)
{
    if (workspace == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (workspace->drawing_history != NULL) return UmiChartDrawingHistoryRead(workspace->drawing_history, out);
    UmiChartDrawingHistorySnapshot empty = {0};
    empty.registry_revision = umi_chart_drawing_registry_revision(umi_chart_workspace_drawings(workspace->charts));
    *out = empty; return UMI_STATUS_OK;
}
static UmiStatus ChartHistoryReplayWorkspace(UmiTradingWorkspace *workspace, const char *instrument, uint64_t expected, int redo)
{
    size_t index; UmiStatus status = ChartSelectedIndex(workspace, instrument, &index);
    if (status != UMI_STATUS_OK) return status;
    (void)index;
    if (workspace->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (workspace->drawing_history == NULL) return expected == 0U ? UMI_STATUS_NOT_FOUND : UMI_STATUS_BUSY;
    status = redo ? UmiChartDrawingHistoryRedo(workspace->drawing_history, instrument, expected) :
        UmiChartDrawingHistoryUndo(workspace->drawing_history, instrument, expected);
    if (status == UMI_STATUS_OK) ++workspace->revision;
    return status;
}
UmiStatus UmiTradingWorkspaceUndoDrawing(UmiTradingWorkspace *workspace, const char *instrument, uint64_t expectedRevision)
{ return ChartHistoryReplayWorkspace(workspace, instrument, expectedRevision, 0); }
UmiStatus UmiTradingWorkspaceRedoDrawing(UmiTradingWorkspace *workspace, const char *instrument, uint64_t expectedRevision)
{ return ChartHistoryReplayWorkspace(workspace, instrument, expectedRevision, 1); }
