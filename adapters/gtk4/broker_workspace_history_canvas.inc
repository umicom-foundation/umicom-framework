/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/broker_workspace_history_canvas.inc
 *
 * PURPOSE:
 *   Own immutable chart scenes independently of retired native broker workspace controllers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* The drawing widget can outlive its controller. Its independent data owns only
 * an immutable scene, never a socket or a pointer back to the workspace. */
typedef struct WorkspaceHistoryCanvas {
    UmiChartRenderScene *scene;
    uint32_t request;
    uint64_t received; /* Rolling streams change even when their retained count is full. */
    size_t first, count;
    gboolean attempted;
    UmiStatus status;
} WorkspaceHistoryCanvas;
static void WorkspaceHistoryClear(WorkspaceHistoryCanvas *canvas)
{
#ifdef UMI_IBKR_HAS_HISTORY_MODEL
    umi_chart_render_scene_destroy(canvas->scene);
#endif
    memset(canvas, 0, sizeof(*canvas));
}
static void WorkspaceHistoryCanvasFree(gpointer data)
{
    WorkspaceHistoryClear(data);
    g_free(data);
}
static void WorkspaceHistoryDraw(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer data)
{
    (void)area;
    WorkspaceHistoryCanvas *canvas = data;
#ifdef UMI_IBKR_HAS_HISTORY_CHART
    if (canvas->scene != NULL && width > 0 && height > 0) {
        UmiChartCairoRendererContext context;
        UmiChartRenderer renderer;
        if (umi_chart_cairo_renderer_init(&context, cr, &renderer) == UMI_STATUS_OK)
            (void)umi_chart_renderer_render_scene(&renderer, canvas->scene, (double)width, (double)height);
    }
#else
    (void)canvas; (void)cr; (void)width; (void)height;
#endif
}
