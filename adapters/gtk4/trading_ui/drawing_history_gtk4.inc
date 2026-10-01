/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/trading_ui/drawing_history_gtk4.inc
 * PURPOSE: Bind explicit native undo and redo to the displayed drawing history revision.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static void ChartHistoryRefresh(InteractiveChart *state)
{
    if (!ChartAttached(state) || state->history_note == NULL) return;
    UmiChartDrawingHistorySnapshot history = {0};
    UmiStatus status = UmiTradingWorkspaceDrawingHistory(state->context->workspace, &history);
    state->history = history;
    int valid = status == UMI_STATUS_OK && !history.stale;
    gtk_widget_set_sensitive(state->history_undo, valid && history.undo_count != 0U && strcmp(history.undo_pane, state->info.instrument_id) == 0);
    gtk_widget_set_sensitive(state->history_redo, valid && history.redo_count != 0U && strcmp(history.redo_pane, state->info.instrument_id) == 0);
    char text[640];
    if (!valid) (void)snprintf(text, sizeof text, "Drawing history is out of date. The next drawing edit starts a new history.");
    else if (history.undo_count == 0U && history.redo_count == 0U) (void)snprintf(text, sizeof text, "No drawing history. Saved-chart restore starts a new history.");
    else (void)snprintf(text, sizeof text, "Undo: %s [%s] | Redo: %s [%s]. Select the named instrument.",
        history.undo_count ? history.undo_label : "none", history.undo_count ? history.undo_pane : "-",
        history.redo_count ? history.redo_label : "none", history.redo_count ? history.redo_pane : "-");
    char *display = g_utf8_make_valid(text, -1);
    gtk_label_set_text(GTK_LABEL(state->history_note), display); g_free(display);
}
static void ChartHistoryAction(InteractiveChart *state, int redo)
{
    if (!ChartAttached(state)) return;
    const UmiChartDrawingHistorySnapshot *shown = &state->history;
    size_t count = redo ? shown->redo_count : shown->undo_count;
    const char *pane = redo ? shown->redo_pane : shown->undo_pane;
    if (shown->stale || count == 0U || strcmp(pane, state->info.instrument_id) != 0) {
        ChartMessage(state, "No available drawing step for this instrument. Read the history label."); return;
    }
    /* Never refresh before dispatch: the copied revision represents the action
     * the user saw, and the workspace rejects intervening edits or selection. */
    UmiStatus status = redo ? UmiTradingWorkspaceRedoDrawing(state->context->workspace, pane, shown->revision) :
        UmiTradingWorkspaceUndoDrawing(state->context->workspace, pane, shown->revision);
    if (status == UMI_STATUS_OK) {
        ChartFinishMove(state); state->have_first = 0; state->dragging = 0;
        state->preview_id = 0U; gtk_widget_set_sensitive(state->restore_button, FALSE);
        ChartMessage(state, redo ? "Drawing step redone. Save chart to retain this state." : "Drawing step undone. Save chart to retain this state.");
    } else ChartMessage(state, "Drawing history changed or the instrument differs. Review the refreshed history before retrying.");
    ChartRefresh(state);
}
static void ChartHistoryUndoClicked(GtkButton *button, gpointer root)
{ (void)button; ChartHistoryAction(ChartState(root), 0); }
static void ChartHistoryRedoClicked(GtkButton *button, gpointer root)
{ (void)button; ChartHistoryAction(ChartState(root), 1); }
static void ChartHistoryCreate(InteractiveChart *state)
{
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    state->history_undo = ChartButton(row, "Undo drawing", "trading.chart.undo-drawing", G_CALLBACK(ChartHistoryUndoClicked), state->root);
    state->history_redo = ChartButton(row, "Redo drawing", "trading.chart.redo-drawing", G_CALLBACK(ChartHistoryRedoClicked), state->root);
    gtk_box_append(GTK_BOX(state->root), row);
    state->history_note = gtk_label_new(""); gtk_label_set_wrap(GTK_LABEL(state->history_note), TRUE);
    gtk_label_set_xalign(GTK_LABEL(state->history_note), 0);
    (void)umi_gtk4_automation_tag_widget(state->history_note, "trading.chart.drawing-history");
    gtk_box_append(GTK_BOX(state->root), state->history_note);
    gtk_widget_set_tooltip_text(state->history_undo, "Undo the last drawing step on its selected instrument. Chart canvas: Ctrl+Z.");
    gtk_widget_set_tooltip_text(state->history_redo, "Redo a drawing step. Chart canvas: Ctrl+Shift+Z or Ctrl+Y.");
}
