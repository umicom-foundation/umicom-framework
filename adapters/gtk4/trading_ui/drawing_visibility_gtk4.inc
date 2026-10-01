/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/trading_ui/drawing_visibility_gtk4.inc
 * PURPOSE: Connect native chart visibility controls to guarded Framework operations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Included after ChartSelectedDrawing so native commands bind to the exact
 * row displayed, including its revision and the instrument of this panel. */
static void ChartHideClicked(GtkButton *button, gpointer root)
{
    (void)button;
    InteractiveChart *state = ChartState(root);
    if (!ChartAttached(state)) return;
    UmiChartDrawingSnapshot drawing;
    UmiStatus status = ChartSelectedDrawing(state, &drawing);
    int hide = status == UMI_STATUS_OK && (drawing.visibility_flags & UMI_CHART_DRAWING_VISIBILITY_HIDDEN) == 0U;
    if (status == UMI_STATUS_OK)
        status = UmiTradingWorkspaceSetChartDrawingHidden(state->context->workspace,
            state->info.instrument_id, drawing.id, drawing.revision, hide);
    state->have_first = 0;
    if (state->edit_id[0] != '\0') ChartFinishMove(state);
    ChartMessage(state, status == UMI_STATUS_OK
        ? (hide ? "Drawing hidden and retained in the object list. Save chart to keep this choice."
                : "Drawing shown. Save chart to keep this choice.")
        : "Drawing visibility unchanged. Select the current drawing and try again.");
    ChartRefresh(state);
}

/* The displayed registry version prevents a delayed group action from also
 * changing drawings that were added after the user saw this object list. */
static void ChartPaneVisibilityClicked(GtkButton *button, gpointer root)
{
    InteractiveChart *state = ChartState(root);
    if (!ChartAttached(state)) return;
    int hide = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "hide-drawings"));
    size_t changed = 0U;
    UmiStatus status = UmiTradingWorkspaceSetChartDrawingsHidden(state->context->workspace,
        state->info.instrument_id, state->drawing_revision, hide, &changed);
    state->have_first = 0;
    if (state->edit_id[0] != '\0') ChartFinishMove(state);
    if (status == UMI_STATUS_OK) {
        char message[192];
        (void)snprintf(message, sizeof message, "%zu drawing%s %s for this instrument. Save chart to keep visibility.",
            changed, changed == 1U ? "" : "s", hide ? "hidden" : "shown");
        ChartMessage(state, message);
    } else ChartMessage(state, "Drawing list changed. Review its current entries before changing all visibility.");
    ChartRefresh(state);
}
