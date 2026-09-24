/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/workspace_split_policy_gtk4.inc
 *
 * PURPOSE:
 *   Give the shared GTK4 workstation sensible initial split positions across
 *   small and large windows without replacing saved layouts or user resizing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

typedef struct UmiWorkspaceAdaptiveSplitState {
    GtkOrientation orientation;
    double ratio;
    int minimumStart;
    int minimumEnd;
    int applied;
} UmiWorkspaceAdaptiveSplitState;

/* Release one transient sizing record after GTK disconnects its map handler. */
static void UmiWorkspaceAdaptiveSplitStateDestroy(
    gpointer data,
    GClosure *closure)
{
    (void)closure;
    g_free(data);
}

/* Keep each visible side usable while allowing small windows to divide evenly. */
static int UmiWorkspaceAdaptiveSplitPosition(
    int extent,
    double ratio,
    int minimumStart,
    int minimumEnd)
{
    int position;
    int maximum;

    if (extent <= 0) return 0;
    if (minimumStart < 0) minimumStart = 0;
    if (minimumEnd < 0) minimumEnd = 0;

    if (extent <= minimumStart + minimumEnd) {
        return extent / 2;
    }

    position = (int)((double)extent * ratio + 0.5);
    maximum = extent - minimumEnd;
    if (position < minimumStart) position = minimumStart;
    if (position > maximum) position = maximum;
    return position;
}

/*
 * Apply the policy once after GTK has allocated the real workstation size.
 * Later pointer/keyboard splitter movement remains entirely user-controlled.
 */
static void UmiWorkspaceAdaptiveSplitMapped(
    GtkWidget *widget,
    gpointer userData)
{
    UmiWorkspaceAdaptiveSplitState *state =
        (UmiWorkspaceAdaptiveSplitState *)userData;
    int extent;
    int position;

    if (state == NULL || state->applied || !GTK_IS_PANED(widget)) return;

    extent = state->orientation == GTK_ORIENTATION_HORIZONTAL
        ? gtk_widget_get_width(widget)
        : gtk_widget_get_height(widget);
    if (extent <= 0) return;

    position = UmiWorkspaceAdaptiveSplitPosition(
        extent,
        state->ratio,
        state->minimumStart,
        state->minimumEnd);
    gtk_paned_set_position(GTK_PANED(widget), position);
    state->applied = 1;
}

/*
 * Preserve the existing join_regions() implementation and its established
 * fixed fallback position. This wrapper only adds a one-time map policy after
 * both sides are present.
 */
static GtkWidget *UmiWorkspaceJoinRegionsResponsive(
    GtkOrientation orientation,
    GtkWidget *start,
    GtkWidget *end,
    int position,
    bool resizeStart,
    bool resizeEnd)
{
    GtkWidget *joined = join_regions(
        orientation,
        start,
        end,
        position,
        resizeStart,
        resizeEnd);
    UmiWorkspaceAdaptiveSplitState *state;

    if (start == NULL || end == NULL || !GTK_IS_PANED(joined)) {
        return joined;
    }

    state = g_try_new0(UmiWorkspaceAdaptiveSplitState, 1);
    if (state == NULL) return joined;

    state->orientation = orientation;

    if (orientation == GTK_ORIENTATION_HORIZONTAL && position <= 400) {
        /* Left navigation/project regions stay compact beside the work area. */
        state->ratio = 0.20;
        state->minimumStart = 210;
        state->minimumEnd = 440;
    } else if (orientation == GTK_ORIENTATION_HORIZONTAL) {
        /* Right inspectors keep useful width without dominating the centre. */
        state->ratio = 0.72;
        state->minimumStart = 440;
        state->minimumEnd = 260;
    } else if (position <= 300) {
        /* Top regions remain concise when a workspace elects to show them. */
        state->ratio = 0.18;
        state->minimumStart = 130;
        state->minimumEnd = 360;
    } else {
        /* Bottom output/blotter/problem regions receive room but not the page. */
        state->ratio = 0.70;
        state->minimumStart = 360;
        state->minimumEnd = 180;
    }

    g_signal_connect_data(
        joined,
        "map",
        G_CALLBACK(UmiWorkspaceAdaptiveSplitMapped),
        state,
        UmiWorkspaceAdaptiveSplitStateDestroy,
        0);
    return joined;
}

/*
 * The existing workspace host calls join_regions() after this include. Route
 * those later calls through the responsive wrapper while leaving the original
 * function available and used by the wrapper above.
 */
#define join_regions(orientation, start, end, position, resizeStart, resizeEnd) \
    UmiWorkspaceJoinRegionsResponsive( \
        orientation, start, end, position, resizeStart, resizeEnd)
