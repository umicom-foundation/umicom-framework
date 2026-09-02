/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/chart_surface_gtk4.c
 *
 * PURPOSE:
 *   Display Framework chart scenes in a reusable GTK4 workstation panel.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/chart_surface.h"

#include "umicom/chart/adapters/cairo_renderer.h"
#include "umicom/chart/plot.h"

/* Identify the private chart state stored on the public root widget. */
#define UMI_GTK4_CHART_STATE_KEY "umicom-chart-surface-state"

/* Own a stable surface snapshot and cloned scene for the complete widget lifetime. */
typedef struct UmiGtk4ChartSurfaceState {
    UmiWsChartSurface surface;
    UmiChartRenderScene *scene;
    GtkWidget *drawing_area;
} UmiGtk4ChartSurfaceState;

/* Release the scene and state when GTK destroys the chart root widget. */
static void destroy_chart_state(gpointer data)
{
    UmiGtk4ChartSurfaceState *state = (UmiGtk4ChartSurfaceState *)data;

    /* A missing state has no owned scene or allocation to release. */
    if (state == NULL) {
        return;
    }

    umi_chart_render_scene_destroy(state->scene);
    state->scene = NULL;
    g_free(state);
}

/* Build a quiet empty frame when an application has not supplied chart data yet. */
static UmiStatus build_empty_scene(
    int width,
    int height,
    UmiChartRenderScene **out_scene)
{
    UmiChartPlotStyle style;
    UmiChartPlotViewport viewport;
    UmiChartRenderScene *scene = NULL;
    UmiStatus status;

    /* Clear the result immediately so an invalid size cannot expose stale memory. */
    if (out_scene == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_scene = NULL;

    /* Very small widgets do not contain enough room for a meaningful chart frame. */
    if (width <= 2 || height <= 2) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_chart_render_scene_create(32U, &scene);
    /* Preserve an allocation failure rather than attempting to draw without a scene. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    umi_chart_plot_style_dark(&style);
    /* A transparent background lets the application theme remain authoritative. */
    style.background_color.alpha = 0.0;
    viewport = (UmiChartPlotViewport){
        .area = {1.0, 1.0, (double)width - 2.0, (double)height - 2.0},
        .start_ms = 0,
        .end_ms = 1,
        .minimum_value = 0.0,
        .maximum_value = 1.0
    };

    status = umi_chart_plot_add_frame(scene, &viewport, &style);
    /* Release the temporary scene when its frame could not be completed. */
    if (status != UMI_STATUS_OK) {
        umi_chart_render_scene_destroy(scene);
        return status;
    }

    *out_scene = scene;
    return UMI_STATUS_OK;
}

/* Render the current cloned scene, or an empty themed frame, into the drawing area. */
static void draw_chart(
    GtkDrawingArea *area,
    cairo_t *drawing_context,
    int width,
    int height,
    gpointer data)
{
    UmiGtk4ChartSurfaceState *state = (UmiGtk4ChartSurfaceState *)data;
    UmiChartCairoRendererContext cairo_context;
    UmiChartRenderer renderer;
    UmiChartRenderScene *empty_scene = NULL;
    const UmiChartRenderScene *scene;
    UmiStatus status;

    (void)area;

    /* GTK should always supply state, but a defensive check protects teardown paths. */
    if (state == NULL || drawing_context == NULL || width <= 0 || height <= 0) {
        return;
    }

    scene = state->scene;
    /* An empty frame replaces the old decorative curve until real data is provided. */
    if (scene == NULL) {
        status = build_empty_scene(width, height, &empty_scene);
        /* Allocation or geometry failure leaves the themed widget background untouched. */
        if (status != UMI_STATUS_OK) {
            return;
        }
        scene = empty_scene;
    }

    status = umi_chart_cairo_renderer_init(
        &cairo_context,
        drawing_context,
        &renderer);
    /* Render only after the adapter has supplied a complete callback contract. */
    if (status == UMI_STATUS_OK) {
        (void)umi_chart_renderer_render_scene(
            &renderer,
            scene,
            (double)width,
            (double)height);
    }

    umi_chart_render_scene_destroy(empty_scene);
}

/* Initialise a chart root and copy all input state needed after this function returns. */
GtkWidget *umi_gtk4_ws_chart_surface_create_with_scene(
    const UmiWsChartSurface *surface,
    const UmiChartRenderScene *scene)
{
    UmiGtk4ChartSurfaceState *state;
    GtkWidget *root;
    GtkWidget *header;
    UmiStatus status;

    state = g_try_new0(UmiGtk4ChartSurfaceState, 1U);
    /* Return NULL so the caller can present an allocation failure safely. */
    if (state == NULL) {
        return NULL;
    }

    /* Copy caller-owned configuration so stack-local input remains safe. */
    if (surface != NULL) {
        state->surface = *surface;
    }

    /* Clone optional scene data so the widget never keeps a dangling caller pointer. */
    if (scene != NULL) {
        status = umi_chart_render_scene_clone(scene, &state->scene);
        /* Release the state if its complete initial scene cannot be copied. */
        if (status != UMI_STATUS_OK) {
            g_free(state);
            return NULL;
        }
    }

    root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    header = gtk_label_new(
        state->surface.title[0] != '\0' ? state->surface.title : "Chart");
    state->drawing_area = gtk_drawing_area_new();
    gtk_widget_add_css_class(root, "umicom-chart-surface");
    gtk_label_set_xalign(GTK_LABEL(header), 0.0F);
    gtk_drawing_area_set_draw_func(
        GTK_DRAWING_AREA(state->drawing_area),
        draw_chart,
        state,
        NULL);
    gtk_widget_set_hexpand(state->drawing_area, TRUE);
    gtk_widget_set_vexpand(state->drawing_area, TRUE);
    gtk_box_append(GTK_BOX(root), header);
    gtk_box_append(GTK_BOX(root), state->drawing_area);

    /* The root owns state exactly once and releases it with the complete widget tree. */
    g_object_set_data_full(
        G_OBJECT(root),
        UMI_GTK4_CHART_STATE_KEY,
        state,
        destroy_chart_state);
    return root;
}

/* Initialise a chart surface without an initial data scene. */
GtkWidget *umi_gtk4_ws_chart_surface_create(const UmiWsChartSurface *surface)
{
    return umi_gtk4_ws_chart_surface_create_with_scene(surface, NULL);
}

/* Replace the scene owned by an existing chart widget and request a redraw. */
UmiStatus umi_gtk4_ws_chart_surface_set_scene(
    GtkWidget *chart_widget,
    const UmiChartRenderScene *scene)
{
    UmiGtk4ChartSurfaceState *state;
    UmiChartRenderScene *replacement = NULL;
    UmiStatus status;

    /* The public root widget is required to locate its private owned state. */
    if (chart_widget == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    state = (UmiGtk4ChartSurfaceState *)g_object_get_data(
        G_OBJECT(chart_widget),
        UMI_GTK4_CHART_STATE_KEY);
    /* Reject unrelated widgets rather than attaching hidden chart state to them. */
    if (state == NULL || state->drawing_area == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Clone before replacing so a failed allocation preserves the visible scene. */
    if (scene != NULL) {
        status = umi_chart_render_scene_clone(scene, &replacement);
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    umi_chart_render_scene_destroy(state->scene);
    state->scene = replacement;
    gtk_widget_queue_draw(state->drawing_area);
    return UMI_STATUS_OK;
}
