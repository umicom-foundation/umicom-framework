/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/workspace_canvas_gtk4.inc
 *
 * PURPOSE:
 *   Render and preview canonical canvas panels inside the shared GTK host.
 *   Geometry remains owned by the existing portable workspace transaction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#define CANVAS_GRID_STEP 0.025
#define CANVAS_MIN_WIDTH 144.0
#define CANVAS_MIN_HEIGHT 96.0
#define CANVAS_RESIZE_HANDLES 8U

/* The first entry keeps the established southeast grip and automation tag.
 * Every other edge shares the same portable, opposite-edge-anchored projector. */
static const struct {
    UmiApplicationSuiteLayoutCanvasGesture kind;
    const char *suffix;
    const char *cursor;
    const char *description;
} canvas_resize_edges[CANVAS_RESIZE_HANDLES] = {
    { UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_SOUTH_EAST, "", "se-resize", "Resize bottom right" },
    { UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH, "north.", "n-resize", "Resize top edge" },
    { UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH_EAST, "north-east.", "ne-resize", "Resize top right" },
    { UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_EAST, "east.", "e-resize", "Resize right edge" },
    { UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_SOUTH, "south.", "s-resize", "Resize bottom edge" },
    { UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_SOUTH_WEST, "south-west.", "sw-resize", "Resize bottom left" },
    { UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_WEST, "west.", "w-resize", "Resize left edge" },
    { UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_NORTH_WEST, "north-west.", "nw-resize", "Resize top left" }
};

/* Each panel owns its controller references until every callback is detached.
 * Widget data is cleared before this record is released, including when a
 * caller retains an old widget after rebuilding the host. */
struct CanvasEntry {
    UmiGtk4WorkspaceLayoutHost *host;
    GtkWidget *panel;
    GtkWidget *drag_handle;
    GtkWidget *resize_handles[CANVAS_RESIZE_HANDLES];
    GtkGesture *move_gesture;
    GtkGesture *resize_gestures[CANVAS_RESIZE_HANDLES];
    GtkGesture *active_gesture;
    GtkEventController *keys;
    GtkEventController *focus;
    char window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    UmiApplicationSuiteLayoutRect model_rect;
    UmiApplicationSuiteLayoutRect preview_rect;
    UmiApplicationSuiteLayoutRect start_rect;
    UmiApplicationSuiteLayoutCanvasGesture gesture_kind;
    uint64_t source_revision;
    double viewport_width;
    double viewport_height;
    double pointer_start_x;
    double pointer_start_y;
    int pointer_origin_valid;
    int native_sequence;
    int motion_seen;
    int active;
    int keyboard_preview;
    int pinned;
    int resizable;
};

/* The idle owns a value copy, not pointers into a soon-to-be-rebuilt panel. */
typedef struct PendingCanvasGeometry {
    UmiGtk4WorkspaceLayoutHost *host;
    char window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    UmiApplicationSuiteLayoutRect rect;
    uint64_t expected_revision;
} PendingCanvasGeometry;

/* Custom viewport widgets deliberately do not propagate panel minimum sizes.
 * GtkFixed would let a large editor/table force the entire native window to
 * grow; these bounded containers clip content to the canonical rectangle. */
typedef struct UmiWorkspaceCanvasLayer {
    GtkWidget parent_instance;
    GtkWidget *docked_child;
    int editing;
} UmiWorkspaceCanvasLayer;
typedef struct UmiWorkspaceCanvasLayerClass {
    GtkWidgetClass parent_class;
} UmiWorkspaceCanvasLayerClass;
typedef struct UmiWorkspaceCanvasPanel {
    GtkWidget parent_instance;
    GtkWidget *content;
    GtkWidget *resize_handles[CANVAS_RESIZE_HANDLES];
} UmiWorkspaceCanvasPanel;
typedef struct UmiWorkspaceCanvasPanelClass {
    GtkWidgetClass parent_class;
} UmiWorkspaceCanvasPanelClass;

/* Corner visuals occupy a square, but pointer input belongs only to its outer
 * L-shaped border. This keeps header buttons and provider controls clickable. */
typedef struct UmiWorkspaceCanvasGrip {
    GtkWidget parent_instance;
    GtkWidget *glyph;
    size_t edge_index;
} UmiWorkspaceCanvasGrip;
typedef struct UmiWorkspaceCanvasGripClass {
    GtkWidgetClass parent_class;
} UmiWorkspaceCanvasGripClass;

/* Keep the viewport, panel and resize-grip types private to this adapter. */
static GType umi_workspace_canvas_layer_get_type(void);
static GType umi_workspace_canvas_panel_get_type(void);
static GType umi_workspace_canvas_grip_get_type(void);
G_DEFINE_TYPE(UmiWorkspaceCanvasLayer, umi_workspace_canvas_layer, GTK_TYPE_WIDGET)
G_DEFINE_TYPE(UmiWorkspaceCanvasPanel, umi_workspace_canvas_panel, GTK_TYPE_WIDGET)
G_DEFINE_TYPE(UmiWorkspaceCanvasGrip, umi_workspace_canvas_grip, GTK_TYPE_WIDGET)

/* Restrict corner picking to the same five-pixel border used by edge strips. */
static gboolean canvas_grip_contains(GtkWidget *widget, double x, double y)
{
    UmiWorkspaceCanvasGrip *grip = (UmiWorkspaceCanvasGrip *)widget;
    const double width = gtk_widget_get_width(widget);
    const double height = gtk_widget_get_height(widget);
    if (x < 0.0 || y < 0.0 || x >= width || y >= height) return FALSE;
    switch (grip->edge_index) {
    case 0U: return x >= width - 5.0 || y >= height - 5.0;
    case 2U: return x >= width - 5.0 || y < 5.0;
    case 5U: return x < 5.0 || y >= height - 5.0;
    case 7U: return x < 5.0 || y < 5.0;
    default: return TRUE;
    }
}

/* Keep the established southeast visual without giving its label hit targets. */
static void canvas_grip_allocate(GtkWidget *widget, int width, int height, int baseline)
{
    UmiWorkspaceCanvasGrip *grip = (UmiWorkspaceCanvasGrip *)widget;
    (void)baseline;
    if (grip->glyph != NULL) gtk_widget_allocate(grip->glyph, width, height, -1, NULL);
}

/* Snapshot only the optional glyph; GTK supplies the grip's CSS background. */
static void canvas_grip_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    UmiWorkspaceCanvasGrip *grip = (UmiWorkspaceCanvasGrip *)widget;
    if (grip->glyph != NULL) gtk_widget_snapshot_child(widget, grip->glyph, snapshot);
}

/* Parent ownership is released even if automation retains an old grip. */
static void canvas_grip_dispose(GObject *object)
{
    UmiWorkspaceCanvasGrip *grip = (UmiWorkspaceCanvasGrip *)object;
    GtkWidget *glyph = grip->glyph;
    grip->glyph = NULL;
    if (glyph != NULL) gtk_widget_unparent(glyph);
    G_OBJECT_CLASS(umi_workspace_canvas_grip_parent_class)->dispose(object);
}

/* Grips never contribute a minimum size to a provider's allocation. */
static void canvas_grip_measure(GtkWidget *widget, GtkOrientation orientation, int for_size,
    int *minimum, int *natural, int *minimum_baseline, int *natural_baseline)
{
    (void)widget;
    (void)orientation;
    (void)for_size;
    if (minimum != NULL) *minimum = 0;
    if (natural != NULL) *natural = 18;
    if (minimum_baseline != NULL) *minimum_baseline = -1;
    if (natural_baseline != NULL) *natural_baseline = -1;
}

/* Register explicit measurement, painting and picking for the border shape. */
static void umi_workspace_canvas_grip_class_init(UmiWorkspaceCanvasGripClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    widget_class->measure = canvas_grip_measure;
    widget_class->contains = canvas_grip_contains;
    widget_class->size_allocate = canvas_grip_allocate;
    widget_class->snapshot = canvas_grip_snapshot;
    G_OBJECT_CLASS(klass)->dispose = canvas_grip_dispose;
    gtk_widget_class_set_css_name(widget_class, "umicom-canvas-grip");
}

/* A narrow grip must not draw beyond its clipped canvas panel. */
static void umi_workspace_canvas_grip_init(UmiWorkspaceCanvasGrip *grip)
{
    gtk_widget_set_overflow(GTK_WIDGET(grip), GTK_OVERFLOW_HIDDEN);
}

/* Measure the available surface, never the potentially oversized providers. */
static void canvas_layer_measure(
    GtkWidget *widget, GtkOrientation orientation, int for_size,
    int *minimum, int *natural, int *minimum_baseline, int *natural_baseline)
{
    (void)widget;
    (void)for_size;
    if (minimum != NULL) *minimum = 1;
    if (natural != NULL)
        *natural = orientation == GTK_ORIENTATION_HORIZONTAL ? 640 : 480;
    if (minimum_baseline != NULL) *minimum_baseline = -1;
    if (natural_baseline != NULL) *natural_baseline = -1;
}

/* Allocate each child against the complete viewport in paint-order. */
static void canvas_layer_allocate(
    GtkWidget *widget, int width, int height, int baseline)
{
    UmiWorkspaceCanvasLayer *layer = (UmiWorkspaceCanvasLayer *)widget;
    GtkWidget *child;

    (void)baseline;
    for (child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        CanvasEntry *entry;
        graphene_point_t origin;
        const UmiApplicationSuiteLayoutRect *rect;

        if (!gtk_widget_get_visible(child)) continue;
        if (child == layer->docked_child) {
            gtk_widget_allocate(child, width, height, -1, NULL);
            continue;
        }
        entry = g_object_get_data(G_OBJECT(child), "umicom-canvas-entry");
        if (entry == NULL) continue;
        rect = &entry->preview_rect;
        origin = GRAPHENE_POINT_INIT(
            (float)(rect->x * width), (float)(rect->y * height));
        gtk_widget_allocate(
            child,
            MAX(1, (int)(rect->width * width + 0.5)),
            MAX(1, (int)(rect->height * height + 0.5)), -1,
            gsk_transform_translate(NULL, &origin));
    }
}

/* Paint a quiet canvas grid only during layout editing, then the real panels.
 * Children already follow stable ascending z-order, including equal-z ties. */
static void canvas_layer_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    UmiWorkspaceCanvasLayer *layer = (UmiWorkspaceCanvasLayer *)widget;
    const float width = (float)gtk_widget_get_width(widget);
    const float height = (float)gtk_widget_get_height(widget);
    const GdkRGBA grid = { 0.55F, 0.64F, 0.72F, 0.15F };
    graphene_rect_t bounds = GRAPHENE_RECT_INIT(0.0F, 0.0F, width, height);
    GtkWidget *child;
    unsigned int line;

    gtk_snapshot_push_clip(snapshot, &bounds);
    /* Leave the surface transparent so Framework light, retro and custom
     * appearance backgrounds continue to belong to the enclosing shell. */
    if (layer->editing) {
        for (line = 1U; line < 40U; ++line) {
            graphene_rect_t vertical = GRAPHENE_RECT_INIT(
                width * (float)(line * CANVAS_GRID_STEP), 0.0F, 1.0F, height);
            graphene_rect_t horizontal = GRAPHENE_RECT_INIT(
                0.0F, height * (float)(line * CANVAS_GRID_STEP), width, 1.0F);
            gtk_snapshot_append_color(snapshot, &grid, &vertical);
            gtk_snapshot_append_color(snapshot, &grid, &horizontal);
        }
    }
    for (child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        if (gtk_widget_get_visible(child))
            gtk_widget_snapshot_child(widget, child, snapshot);
    }
    gtk_snapshot_pop(snapshot);
}

/* Release GTK children before the layer's own object storage disappears. */
static void canvas_layer_dispose(GObject *object)
{
    UmiWorkspaceCanvasLayer *layer = (UmiWorkspaceCanvasLayer *)object;
    GtkWidget *child;

    layer->docked_child = NULL;
    while ((child = gtk_widget_get_first_child(GTK_WIDGET(object))) != NULL)
        gtk_widget_unparent(child);
    G_OBJECT_CLASS(umi_workspace_canvas_layer_parent_class)->dispose(object);
}

/* Register the allocation and snapshot policy for the private canvas type. */
static void umi_workspace_canvas_layer_class_init(
    UmiWorkspaceCanvasLayerClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    widget_class->measure = canvas_layer_measure;
    widget_class->size_allocate = canvas_layer_allocate;
    widget_class->snapshot = canvas_layer_snapshot;
    G_OBJECT_CLASS(klass)->dispose = canvas_layer_dispose;
    gtk_widget_class_set_css_name(widget_class, "umicom-workspace-canvas");
}

/* Establish clipping before any provider child is attached. */
static void umi_workspace_canvas_layer_init(UmiWorkspaceCanvasLayer *layer)
{
    gtk_widget_set_overflow(GTK_WIDGET(layer), GTK_OVERFLOW_HIDDEN);
    gtk_widget_set_hexpand(GTK_WIDGET(layer), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(layer), TRUE);
}

/* Suppress the provider minimum at the panel boundary as well as the canvas. */
static void canvas_panel_measure(
    GtkWidget *widget, GtkOrientation orientation, int for_size,
    int *minimum, int *natural, int *minimum_baseline, int *natural_baseline)
{
    (void)widget;
    (void)orientation;
    (void)for_size;
    if (minimum != NULL) *minimum = 1;
    if (natural != NULL) *natural = 1;
    if (minimum_baseline != NULL) *minimum_baseline = -1;
    if (natural_baseline != NULL) *natural_baseline = -1;
}

/* Narrow edge strips and corner grips overlay the frame only during editing.
 * They meet without overlapping and shrink inside even a tiny viewport. */
static void canvas_panel_allocate(
    GtkWidget *widget, int width, int height, int baseline)
{
    UmiWorkspaceCanvasPanel *panel = (UmiWorkspaceCanvasPanel *)widget;
    const int corner_width = MIN(18, MAX(0, width / 2));
    const int corner_height = MIN(18, MAX(0, height / 2));
    const int edge_width = MIN(5, corner_width);
    const int edge_height = MIN(5, corner_height);
    const int rectangles[CANVAS_RESIZE_HANDLES][4] = {
        { width - corner_width, height - corner_height, corner_width, corner_height },
        { corner_width, 0, width - 2 * corner_width, edge_height },
        { width - corner_width, 0, corner_width, corner_height },
        { width - edge_width, corner_height, edge_width, height - 2 * corner_height },
        { corner_width, height - edge_height, width - 2 * corner_width, edge_height },
        { 0, height - corner_height, corner_width, corner_height },
        { 0, corner_height, edge_width, height - 2 * corner_height },
        { 0, 0, corner_width, corner_height }
    };
    size_t index;

    (void)baseline;
    if (panel->content != NULL)
        gtk_widget_allocate(panel->content, width, height, -1, NULL);
    for (index = 0U; index < CANVAS_RESIZE_HANDLES; ++index) {
        GtkWidget *handle = panel->resize_handles[index];
        graphene_point_t origin = GRAPHENE_POINT_INIT(
            (float)rectangles[index][0], (float)rectangles[index][1]);
        if (handle != NULL && gtk_widget_get_visible(handle))
            gtk_widget_allocate(handle, rectangles[index][2], rectangles[index][3],
                -1, gsk_transform_translate(NULL, &origin));
    }
}

/* Clip overlarge editor contents while retaining their native GTK behaviour. */
static void canvas_panel_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    GtkWidget *child;
    graphene_rect_t bounds = GRAPHENE_RECT_INIT(
        0.0F, 0.0F, (float)gtk_widget_get_width(widget),
        (float)gtk_widget_get_height(widget));

    gtk_snapshot_push_clip(snapshot, &bounds);
    for (child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        if (gtk_widget_get_visible(child))
            gtk_widget_snapshot_child(widget, child, snapshot);
    }
    gtk_snapshot_pop(snapshot);
}

/* Release content and all grips, including externally retained child widgets. */
static void canvas_panel_dispose(GObject *object)
{
    UmiWorkspaceCanvasPanel *panel = (UmiWorkspaceCanvasPanel *)object;
    GtkWidget *child;

    panel->content = NULL;
    memset(panel->resize_handles, 0, sizeof(panel->resize_handles));
    while ((child = gtk_widget_get_first_child(GTK_WIDGET(object))) != NULL)
        gtk_widget_unparent(child);
    G_OBJECT_CLASS(umi_workspace_canvas_panel_parent_class)->dispose(object);
}

/* Register the private fixed-geometry panel boundary. */
static void umi_workspace_canvas_panel_class_init(
    UmiWorkspaceCanvasPanelClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    widget_class->measure = canvas_panel_measure;
    widget_class->size_allocate = canvas_panel_allocate;
    widget_class->snapshot = canvas_panel_snapshot;
    G_OBJECT_CLASS(klass)->dispose = canvas_panel_dispose;
    gtk_widget_class_set_css_name(widget_class, "umicom-canvas-panel");
}

/* Keep native picking and drawing inside the allocated panel rectangle. */
static void umi_workspace_canvas_panel_init(UmiWorkspaceCanvasPanel *panel)
{
    gtk_widget_set_overflow(GTK_WIDGET(panel), GTK_OVERFLOW_HIDDEN);
}

/* Resolve only currently visible in-canvas records, never native windows. */
static CanvasEntry *find_canvas_entry(
    const UmiGtk4WorkspaceLayoutHost *host, const char *window_id)
{
    guint index;

    if (host == NULL || host->canvas_entries == NULL || window_id == NULL)
        return NULL;
    for (index = 0U; index < host->canvas_entries->len; ++index) {
        CanvasEntry *entry = g_ptr_array_index(host->canvas_entries, index);
        if (strcmp(entry->window_id, window_id) == 0) return entry;
    }
    return NULL;
}

/* Reflect edit permissions without disabling the provider's normal controls. */
static void update_canvas_controls(UmiGtk4WorkspaceLayoutHost *host)
{
    guint index;
    size_t edge;

    if (host == NULL || host->canvas_entries == NULL) return;
    for (index = 0U; index < host->canvas_entries->len; ++index) {
        CanvasEntry *entry = g_ptr_array_index(host->canvas_entries, index);
        const gboolean editable = !host->layout.locked && !host->maximise_mode.active && !entry->pinned &&
            host->canvas_geometry_handler != NULL &&
            host->pending_geometry_id == 0U;

        gtk_widget_set_sensitive(entry->drag_handle, editable);
        gtk_widget_set_focusable(entry->drag_handle, editable);
        gtk_widget_set_cursor_from_name(entry->drag_handle,
            editable ? "move" : "default");
        for (edge = 0U; edge < CANVAS_RESIZE_HANDLES; ++edge) {
            gtk_widget_set_visible(entry->resize_handles[edge],
                !host->layout.locked && entry->resizable && !entry->pinned);
            gtk_widget_set_sensitive(entry->resize_handles[edge],
                editable && entry->resizable);
            gtk_widget_set_cursor_from_name(entry->resize_handles[edge],
                editable && entry->resizable ? canvas_resize_edges[edge].cursor : "default");
        }
    }
}

/* Drop presentation-only geometry before any owner callback can rebuild it. */
static void restore_canvas_previews(UmiGtk4WorkspaceLayoutHost *host)
{
    guint index;

    if (host == NULL) return;
    host->active_canvas_entry = NULL;
    if (host->canvas_entries != NULL) {
        for (index = 0U; index < host->canvas_entries->len; ++index) {
            CanvasEntry *entry = g_ptr_array_index(host->canvas_entries, index);
            entry->active = 0;
            entry->active_gesture = NULL;
            entry->keyboard_preview = 0;
            gtk_widget_remove_css_class(entry->panel, "canvas-keyboard-edit");
            entry->preview_rect = entry->model_rect;
        }
    }
    if (host->canvas_layer != NULL)
        gtk_widget_queue_allocate(host->canvas_layer);
}

/* Cancel queued work when the model, callback or host lifetime changes. */
static void cancel_canvas_geometry(UmiGtk4WorkspaceLayoutHost *host)
{
    guint source_id;

    if (host == NULL) return;
    source_id = host->pending_geometry_id;
    host->pending_geometry_id = 0U;
    if (source_id != 0U) (void)g_source_remove(source_id);
    restore_canvas_previews(host);
}

/* Disconnect first: resetting a GTK gesture may itself emit cancellation. */
static void canvas_entry_destroy(gpointer data)
{
    CanvasEntry *entry = (CanvasEntry *)data;
    size_t edge;

    if (entry == NULL) return;
    entry->host = NULL;
    entry->active = 0;
    if (entry->panel != NULL)
        g_object_set_data(G_OBJECT(entry->panel), "umicom-canvas-entry", NULL);
    if (entry->move_gesture != NULL) {
        g_signal_handlers_disconnect_by_data(entry->move_gesture, entry);
        gtk_event_controller_reset(GTK_EVENT_CONTROLLER(entry->move_gesture));
        g_object_unref(entry->move_gesture);
    }
    for (edge = 0U; edge < CANVAS_RESIZE_HANDLES; ++edge) {
        if (entry->resize_gestures[edge] != NULL) {
            g_signal_handlers_disconnect_by_data(entry->resize_gestures[edge], entry);
            gtk_event_controller_reset(GTK_EVENT_CONTROLLER(entry->resize_gestures[edge]));
            g_object_unref(entry->resize_gestures[edge]);
        }
    }
    if (entry->keys != NULL) {
        g_signal_handlers_disconnect_by_data(entry->keys, entry);
        g_object_unref(entry->keys);
    }
    if (entry->focus != NULL) {
        g_signal_handlers_disconnect_by_data(entry->focus, entry);
        g_object_unref(entry->focus);
    }
    if (entry->panel != NULL) g_object_unref(entry->panel);
    g_free(entry);
}

/* Invalidate every old view record before removing the GTK child tree. */
static void clear_canvas_entries(UmiGtk4WorkspaceLayoutHost *host)
{
    if (host == NULL) return;
    host->active_canvas_entry = NULL;
    host->canvas_layer = NULL;
    if (host->canvas_entries != NULL)
        g_ptr_array_set_size(host->canvas_entries, 0U);
}

/* Dispatch copied geometry after the GTK gesture stack has unwound. The owner
 * may free the host here, so no host or entry is read after this call. */
static gboolean dispatch_canvas_geometry_from_idle(gpointer user_data)
{
    PendingCanvasGeometry *pending = (PendingCanvasGeometry *)user_data;
    UmiGtk4WorkspaceLayoutHost *host = pending->host;
    UmiGtk4WorkspaceCanvasGeometryHandler handler;
    void *handler_data;

    host->pending_geometry_id = 0U;
    restore_canvas_previews(host);
    update_canvas_controls(host);
    handler = host->canvas_geometry_handler;
    handler_data = host->canvas_geometry_user_data;
    if (handler != NULL)
        (void)handler(pending->window_id, &pending->rect,
            pending->expected_revision, handler_data);
    return G_SOURCE_REMOVE;
}

/* Replace a borrowed owner callback and invalidate pending work from its past. */
UmiStatus umi_gtk4_workspace_layout_host_set_canvas_geometry_handler(
    UmiGtk4WorkspaceLayoutHost *host,
    UmiGtk4WorkspaceCanvasGeometryHandler handler, void *user_data)
{
    if (host == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    cancel_canvas_geometry(host);
    host->canvas_geometry_handler = handler;
    host->canvas_geometry_user_data = user_data;
    update_canvas_controls(host);
    return UMI_STATUS_OK;
}

/* Validate a view request before it can reach the authoritative model owner. */
UmiStatus umi_gtk4_workspace_layout_host_request_canvas_geometry(
    UmiGtk4WorkspaceLayoutHost *host, const char *window_id,
    const UmiApplicationSuiteLayoutRect *rect,
    uint64_t expected_layout_revision)
{
    CanvasEntry *entry;
    PendingCanvasGeometry *pending;

    if (host == NULL || window_id == NULL || window_id[0] == '\0' ||
        !umi_application_suite_layout_canvas_rect_valid(rect))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (host->pending_geometry_id != 0U || host->maximise_mode.active) return UMI_STATUS_BUSY;
    entry = find_canvas_entry(host, window_id);
    if (entry == NULL) return UMI_STATUS_NOT_FOUND;
    if (expected_layout_revision != host->layout.revision)
        return UMI_STATUS_INVALID_STATE;
    if (host->layout.locked || entry->pinned ||
        (!entry->resizable &&
         (rect->width != entry->model_rect.width ||
          rect->height != entry->model_rect.height)))
        return UMI_STATUS_PERMISSION_DENIED;
    if (host->canvas_geometry_handler == NULL) return UMI_STATUS_UNAVAILABLE;
    pending = g_try_new0(PendingCanvasGeometry, 1);
    if (pending == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    pending->host = host;
    pending->rect = *rect;
    pending->expected_revision = expected_layout_revision;
    (void)snprintf(pending->window_id, sizeof(pending->window_id), "%s",
        window_id);
    restore_canvas_previews(host);
    host->pending_geometry_id = g_idle_add_full(G_PRIORITY_DEFAULT_IDLE,
        dispatch_canvas_geometry_from_idle, pending, g_free);
    if (host->pending_geometry_id == 0U) {
        g_free(pending);
        return UMI_STATUS_UNAVAILABLE;
    }
    update_canvas_controls(host);
    return UMI_STATUS_OK;
}

/* Expose view geometry for accessibility/automation without exposing widgets. */
UmiStatus umi_gtk4_workspace_layout_host_canvas_geometry(
    const UmiGtk4WorkspaceLayoutHost *host, const char *window_id,
    UmiApplicationSuiteLayoutRect *out_rect)
{
    CanvasEntry *entry;

    if (host == NULL || window_id == NULL || out_rect == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    entry = find_canvas_entry(host, window_id);
    if (entry == NULL) return UMI_STATUS_NOT_FOUND;
    *out_rect = entry->preview_rect;
    return UMI_STATUS_OK;
}

/* Translate pointer locations through the moving handle into a stable canvas
 * coordinate system. Synthetic signal tests can use the supplied drag deltas
 * when no real event sequence exists. */
static int canvas_pointer_position(
    CanvasEntry *entry, GtkGesture *gesture, double x, double y,
    double *out_x, double *out_y)
{
    GtkWidget *handle = gtk_event_controller_get_widget(
        GTK_EVENT_CONTROLLER(gesture));
    graphene_point_t local = GRAPHENE_POINT_INIT((float)x, (float)y);
    graphene_point_t canvas;

    if (handle == NULL || entry->host->canvas_layer == NULL ||
        !gtk_widget_compute_point(handle, entry->host->canvas_layer,
                                  &local, &canvas)) return 0;
    *out_x = canvas.x;
    *out_y = canvas.y;
    return 1;
}

/* Capture one permissible pointer operation without opening a model edit. */
static void on_canvas_drag_begin(
    GtkGestureDrag *gesture, double start_x, double start_y, gpointer user_data)
{
    CanvasEntry *entry = (CanvasEntry *)user_data;
    UmiGtk4WorkspaceLayoutHost *host = entry->host;
    UmiApplicationSuiteLayoutCanvasGesture kind = UMI_APPLICATION_SUITE_LAYOUT_CANVAS_MOVE;
    size_t edge;
    for (edge = 0U; edge < CANVAS_RESIZE_HANDLES; ++edge) {
        if (GTK_GESTURE(gesture) == entry->resize_gestures[edge]) {
            kind = canvas_resize_edges[edge].kind;
            break;
        }
    }
    /* Switching from an unfinished keyboard preview to a pointer operation
     * cancels the preview; it cannot implicitly publish its geometry. */
    if (host != NULL && host->active_canvas_entry != NULL &&
        host->active_canvas_entry->keyboard_preview)
        restore_canvas_previews(host);

    if (host == NULL || host->maximise_mode.active || host->layout.locked || entry->pinned ||
        (kind != UMI_APPLICATION_SUITE_LAYOUT_CANVAS_MOVE && !entry->resizable) ||
        host->canvas_geometry_handler == NULL ||
        host->pending_geometry_id != 0U || host->active_canvas_entry != NULL ||
        host->canvas_layer == NULL) return;
    entry->viewport_width = gtk_widget_get_width(host->canvas_layer);
    entry->viewport_height = gtk_widget_get_height(host->canvas_layer);
    if (entry->viewport_width <= 0.0 || entry->viewport_height <= 0.0) return;
    entry->start_rect = entry->model_rect;
    entry->preview_rect = entry->model_rect;
    entry->source_revision = host->layout.revision;
    entry->gesture_kind = kind;
    entry->active_gesture = GTK_GESTURE(gesture);
    entry->keyboard_preview = 0;
    entry->pointer_origin_valid = canvas_pointer_position(entry,
        GTK_GESTURE(gesture), start_x, start_y,
        &entry->pointer_start_x, &entry->pointer_start_y);
    entry->native_sequence = gtk_gesture_get_point(GTK_GESTURE(gesture),
        gtk_gesture_get_last_updated_sequence(GTK_GESTURE(gesture)), NULL, NULL);
    entry->motion_seen = 0;
    entry->active = 1;
    host->active_canvas_entry = entry;
}

/* Apply only a view preview; the total delta is projected by the core helper. */
static void on_canvas_drag_update(
    GtkGestureDrag *gesture, double delta_x, double delta_y, gpointer user_data)
{
    CanvasEntry *entry = (CanvasEntry *)user_data;
    UmiApplicationSuiteLayoutRect projected;
    double pointer_x;
    double pointer_y;
    double canvas_x;
    double canvas_y;

    if (entry->host == NULL || !entry->active || entry->keyboard_preview ||
        entry->active_gesture != GTK_GESTURE(gesture) ||
        entry->host->active_canvas_entry != entry) return;
    if (entry->pointer_origin_valid &&
        gtk_gesture_get_point(GTK_GESTURE(gesture),
            gtk_gesture_get_last_updated_sequence(GTK_GESTURE(gesture)),
            &pointer_x, &pointer_y) &&
        canvas_pointer_position(entry, GTK_GESTURE(gesture),
            pointer_x, pointer_y, &canvas_x, &canvas_y)) {
        delta_x = canvas_x - entry->pointer_start_x;
        delta_y = canvas_y - entry->pointer_start_y;
        entry->native_sequence = 1;
    } else if (entry->native_sequence) {
        /* GTK may have removed the final event point before drag-end. Keep
         * the last stable-canvas preview instead of reinterpreting an offset
         * relative to a handle that has already moved. */
        return;
    }
    if (!entry->motion_seen && delta_x == 0.0 && delta_y == 0.0) return;
    entry->motion_seen = 1;
    if (umi_application_suite_layout_project_canvas_gesture(
            &entry->start_rect, entry->gesture_kind, delta_x, delta_y,
            entry->viewport_width, entry->viewport_height,
            CANVAS_MIN_WIDTH, CANVAS_MIN_HEIGHT, CANVAS_GRID_STEP,
            &projected) == UMI_STATUS_OK) {
        entry->preview_rect = projected;
        gtk_widget_queue_allocate(entry->host->canvas_layer);
    }
}

/* Revert the preview and queue one owner request only after a full gesture. */
static void on_canvas_drag_end(
    GtkGestureDrag *gesture, double delta_x, double delta_y, gpointer user_data)
{
    CanvasEntry *entry = (CanvasEntry *)user_data;
    UmiGtk4WorkspaceLayoutHost *host = entry->host;
    UmiApplicationSuiteLayoutRect requested;
    uint64_t expected_revision;
    char window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];

    if (host == NULL || !entry->active || entry->keyboard_preview ||
        entry->active_gesture != GTK_GESTURE(gesture) || host->active_canvas_entry != entry)
        return;
    on_canvas_drag_update(gesture, delta_x, delta_y, user_data);
    if (!entry->motion_seen) {
        restore_canvas_previews(host);
        return;
    }
    requested = entry->preview_rect;
    expected_revision = entry->source_revision;
    (void)snprintf(window_id, sizeof(window_id), "%s", entry->window_id);
    if (requested.x == entry->model_rect.x &&
        requested.y == entry->model_rect.y &&
        requested.width == entry->model_rect.width &&
        requested.height == entry->model_rect.height) {
        restore_canvas_previews(host);
        return;
    }
    restore_canvas_previews(host);
    (void)umi_gtk4_workspace_layout_host_request_canvas_geometry(
        host, window_id, &requested, expected_revision);
}

/* Cancellation never emits a model request or leaves transient geometry. */
static void on_canvas_drag_cancel(
    GtkGesture *gesture, GdkEventSequence *sequence, gpointer user_data)
{
    CanvasEntry *entry = (CanvasEntry *)user_data;

    (void)sequence;
    if (entry->host != NULL && entry->active && entry->active_gesture == gesture)
        restore_canvas_previews(entry->host);
}

/* Own an extra controller reference so teardown can disconnect retained UI. */
static GtkGesture *attach_canvas_gesture(GtkWidget *handle, CanvasEntry *entry)
{
    GtkGesture *gesture = gtk_gesture_drag_new();

    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture), GDK_BUTTON_PRIMARY);
    gtk_event_controller_set_propagation_phase(
        GTK_EVENT_CONTROLLER(gesture), GTK_PHASE_CAPTURE);
    g_signal_connect(gesture, "drag-begin", G_CALLBACK(on_canvas_drag_begin), entry);
    g_signal_connect(gesture, "drag-update", G_CALLBACK(on_canvas_drag_update), entry);
    g_signal_connect(gesture, "drag-end", G_CALLBACK(on_canvas_drag_end), entry);
    g_signal_connect(gesture, "cancel", G_CALLBACK(on_canvas_drag_cancel), entry);
    g_object_ref(gesture);
    gtk_widget_add_controller(handle, GTK_EVENT_CONTROLLER(gesture));
    return gesture;
}

/* Keyboard editing belongs only to the focusable title, never to the editor
 * body. Arrows preview a grid step; Shift resizes the bottom/right edges.
 * Enter submits through the same deferred, revision-checked owner request.
 * Escape or leaving the title cancels, so Tab cannot accidentally save a move. */
static gboolean on_canvas_key_pressed(GtkEventControllerKey *controller,
    guint keyval, guint keycode, GdkModifierType state, gpointer user_data)
{
    CanvasEntry *entry = user_data;
    UmiGtk4WorkspaceLayoutHost *host = entry->host;
    UmiApplicationSuiteLayoutRect projected;
    UmiApplicationSuiteLayoutCanvasGesture kind;
    double delta_x = 0.0;
    double delta_y = 0.0;
    (void)controller;
    (void)keycode;

    if (host == NULL) return FALSE;
    if (keyval == GDK_KEY_Escape && entry->keyboard_preview) {
        restore_canvas_previews(host);
        return TRUE;
    }
    if ((state & (GDK_CONTROL_MASK | GDK_ALT_MASK | GDK_SUPER_MASK |
                  GDK_HYPER_MASK | GDK_META_MASK)) != 0U) return FALSE;
    if (host->maximise_mode.active || host->layout.locked || entry->pinned || host->canvas_geometry_handler == NULL ||
        host->pending_geometry_id != 0U || host->canvas_layer == NULL ||
        (host->active_canvas_entry != NULL && host->active_canvas_entry != entry) ||
        (entry->active && !entry->keyboard_preview)) return FALSE;

    if (keyval == GDK_KEY_Return || keyval == GDK_KEY_KP_Enter) {
        uint64_t expected_revision;
        if (!entry->keyboard_preview) return FALSE;
        projected = entry->preview_rect;
        expected_revision = entry->source_revision;
        restore_canvas_previews(host);
        if (projected.x != entry->model_rect.x || projected.y != entry->model_rect.y ||
            projected.width != entry->model_rect.width || projected.height != entry->model_rect.height)
            (void)umi_gtk4_workspace_layout_host_request_canvas_geometry(
                host, entry->window_id, &projected, expected_revision);
        return TRUE;
    }
    switch (keyval) {
    case GDK_KEY_Left: delta_x = -CANVAS_GRID_STEP; break;
    case GDK_KEY_Right: delta_x = CANVAS_GRID_STEP; break;
    case GDK_KEY_Up: delta_y = -CANVAS_GRID_STEP; break;
    case GDK_KEY_Down: delta_y = CANVAS_GRID_STEP; break;
    default: return FALSE;
    }
    kind = (state & GDK_SHIFT_MASK) != 0U
        ? UMI_APPLICATION_SUITE_LAYOUT_CANVAS_RESIZE_SOUTH_EAST
        : UMI_APPLICATION_SUITE_LAYOUT_CANVAS_MOVE;
    if (kind != UMI_APPLICATION_SUITE_LAYOUT_CANVAS_MOVE && !entry->resizable)
        return FALSE;
    if (!entry->keyboard_preview) {
        entry->viewport_width = gtk_widget_get_width(host->canvas_layer);
        entry->viewport_height = gtk_widget_get_height(host->canvas_layer);
        if (entry->viewport_width <= 0.0 || entry->viewport_height <= 0.0) return FALSE;
        entry->start_rect = entry->model_rect;
        entry->preview_rect = entry->model_rect;
        entry->source_revision = host->layout.revision;
        entry->keyboard_preview = 1;
        entry->active = 1;
        entry->active_gesture = NULL;
        host->active_canvas_entry = entry;
        gtk_widget_add_css_class(entry->panel, "canvas-keyboard-edit");
    }
    if (umi_application_suite_layout_project_canvas_gesture(
            &entry->preview_rect, kind,
            delta_x * entry->viewport_width, delta_y * entry->viewport_height,
            entry->viewport_width, entry->viewport_height,
            CANVAS_MIN_WIDTH, CANVAS_MIN_HEIGHT, CANVAS_GRID_STEP, &projected) == UMI_STATUS_OK) {
        entry->preview_rect = projected;
        gtk_widget_queue_allocate(host->canvas_layer);
    }
    return TRUE;
}

/* Losing focus is cancellation, not implicit acceptance of an unfinished edit. */
static void on_canvas_title_focus_leave(GtkEventControllerFocus *controller, gpointer user_data)
{
    CanvasEntry *entry = user_data;
    (void)controller;
    if (entry->host != NULL && entry->keyboard_preview)
        restore_canvas_previews(entry->host);
}

/* Adopt a normal Framework frame as one genuine free-positioned canvas panel. */
static CanvasEntry *create_canvas_entry(
    UmiGtk4WorkspaceLayoutHost *host,
    const UmiApplicationSuiteLayoutCanvasItem *item)
{
    const UmiUiWorkspaceWindow *window = &host->layout.windows[item->window_index];
    CanvasEntry *entry = g_try_new0(CanvasEntry, 1);
    UmiWorkspaceCanvasPanel *panel;
    GtkWidget *frame;
    GtkWidget *frame_box;
    GtkWidget *header;
    size_t edge;
    char automation_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY + 48U];

    if (entry == NULL) return NULL;
    frame = create_panel(host, window);
    if (frame == NULL) {
        g_free(entry);
        return NULL;
    }
    panel = g_object_new(umi_workspace_canvas_panel_get_type(), NULL);
    entry->panel = GTK_WIDGET(panel);
    g_object_ref_sink(entry->panel);
    panel->content = frame;
    gtk_widget_set_parent(frame, entry->panel);
    for (edge = 0U; edge < CANVAS_RESIZE_HANDLES; ++edge) {
        UmiWorkspaceCanvasGrip *grip = g_object_new(umi_workspace_canvas_grip_get_type(), NULL);
        GtkWidget *handle = GTK_WIDGET(grip);
        grip->edge_index = edge;
        if (edge == 0U) {
            grip->glyph = gtk_label_new("\342\227\242");
            gtk_widget_set_can_target(grip->glyph, FALSE);
            gtk_widget_set_parent(grip->glyph, handle);
        }
        panel->resize_handles[edge] = handle;
        entry->resize_handles[edge] = handle;
        gtk_widget_set_parent(handle, entry->panel);
        gtk_widget_add_css_class(handle, "umicom-canvas-resize-grip");
        gtk_widget_set_tooltip_text(handle, canvas_resize_edges[edge].description);
        gtk_accessible_update_property(GTK_ACCESSIBLE(handle),
            GTK_ACCESSIBLE_PROPERTY_LABEL, canvas_resize_edges[edge].description, -1);
    }
    /* Shared panel_frame owns this header structure. Attach only to its title
     * area so context, pin, detach and settings buttons keep their clicks. */
    frame_box = gtk_frame_get_child(GTK_FRAME(frame));
    header = gtk_widget_get_first_child(frame_box);
    entry->drag_handle = gtk_widget_get_first_child(header);
    entry->host = host;
    entry->model_rect = item->rect;
    entry->preview_rect = item->rect;
    entry->pinned = window->pinned;
    entry->resizable = window->resizable;
    (void)snprintf(entry->window_id, sizeof(entry->window_id), "%s",
        window->window_id);
    g_object_set_data(G_OBJECT(entry->panel), "umicom-canvas-entry", entry);
    (void)snprintf(automation_id, sizeof(automation_id),
        "workstation.canvas.panel.%s", window->window_id);
    (void)umi_gtk4_automation_tag_widget(entry->panel, automation_id);
    (void)snprintf(automation_id, sizeof(automation_id),
        "workstation.canvas.drag.%s", window->window_id);
    (void)umi_gtk4_automation_tag_widget(entry->drag_handle, automation_id);
    for (edge = 0U; edge < CANVAS_RESIZE_HANDLES; ++edge) {
        (void)snprintf(automation_id, sizeof(automation_id),
            "workstation.canvas.resize.%s%s", canvas_resize_edges[edge].suffix, window->window_id);
        (void)umi_gtk4_automation_tag_widget(entry->resize_handles[edge], automation_id);
        entry->resize_gestures[edge] = attach_canvas_gesture(entry->resize_handles[edge], entry);
    }
    gtk_widget_set_tooltip_text(entry->drag_handle,
        "Edit Layout: drag to move, or focus this title and use arrows to move; "
        "Shift+arrows resize; Enter applies; Escape or Tab cancels the preview.");
    gtk_widget_add_css_class(entry->drag_handle, "umicom-canvas-title-handle");
    gtk_accessible_update_property(GTK_ACCESSIBLE(entry->drag_handle),
        GTK_ACCESSIBLE_PROPERTY_DESCRIPTION,
        "In Edit Layout, arrow keys move this panel. Shift and arrows resize it. "
        "Enter applies the preview. Escape or leaving the title cancels.", -1);
    entry->move_gesture = attach_canvas_gesture(entry->drag_handle, entry);
    entry->keys = gtk_event_controller_key_new();
    g_signal_connect(entry->keys, "key-pressed", G_CALLBACK(on_canvas_key_pressed), entry);
    g_object_ref(entry->keys);
    gtk_widget_add_controller(entry->drag_handle, entry->keys);
    entry->focus = gtk_event_controller_focus_new();
    g_signal_connect(entry->focus, "leave", G_CALLBACK(on_canvas_title_focus_leave), entry);
    g_object_ref(entry->focus);
    gtk_widget_add_controller(entry->drag_handle, entry->focus);
    return entry;
}

/* Keep docked content intact underneath the canvas and sort only view indexes.
 * No model record or factory result is substituted with a placeholder tab. */
static GtkWidget *build_canvas_layer(
    UmiGtk4WorkspaceLayoutHost *host, GtkWidget *docked_child)
{
    UmiWorkspaceCanvasLayer *layer = g_object_new(
        umi_workspace_canvas_layer_get_type(), NULL);
    size_t order[UMI_APPLICATION_SUITE_LAYOUT_MAX_CANVAS_ITEMS];
    size_t index;

    layer->editing = !host->layout.locked;
    layer->docked_child = docked_child;
    host->canvas_layer = GTK_WIDGET(layer);
    if (docked_child != NULL)
        gtk_widget_set_parent(docked_child, GTK_WIDGET(layer));
    (void)umi_gtk4_automation_tag_widget(GTK_WIDGET(layer),
        "workstation.workspace-canvas");
    /* Stable insertion order also defines native hit-testing at overlapping
     * rectangles: later/higher-z children are picked above earlier children. */
    for (index = 0U; index < host->plan.canvas_item_count; ++index) {
        size_t position = index;
        while (position > 0U &&
               host->plan.canvas_items[order[position - 1U]].z_order >
                   host->plan.canvas_items[index].z_order) {
            order[position] = order[position - 1U];
            --position;
        }
        order[position] = index;
    }
    for (index = 0U; index < host->plan.canvas_item_count; ++index) {
        CanvasEntry *entry = create_canvas_entry(
            host, &host->plan.canvas_items[order[index]]);
        if (entry == NULL) {
            clear_canvas_entries(host);
            release_unparented_widget(GTK_WIDGET(layer));
            return NULL;
        }
        gtk_widget_set_parent(entry->panel, GTK_WIDGET(layer));
        g_ptr_array_add(host->canvas_entries, entry);
    }
    update_canvas_controls(host);
    return GTK_WIDGET(layer);
}

/* Validate every bounded string before native GTK presentation or comparison.
 * Hidden records matter too: they may become visible during a later edit and
 * the portable render plan intentionally skips their presentation details. */
static int workspace_layout_text_valid(const UmiUiWorkspaceLayout *layout)
{
    size_t index;

    if (memchr(layout->layout_id, '\0', sizeof(layout->layout_id)) == NULL ||
        memchr(layout->name, '\0', sizeof(layout->name)) == NULL) return 0;
    if (!g_utf8_validate(layout->layout_id, -1, NULL) ||
        !g_utf8_validate(layout->name, -1, NULL)) return 0;
    for (index = 0U; index < layout->window_count; ++index) {
        const UmiUiWorkspaceWindow *window = &layout->windows[index];

        if (memchr(window->window_id, '\0', sizeof(window->window_id)) == NULL ||
            memchr(window->title, '\0', sizeof(window->title)) == NULL ||
            memchr(window->tool_id, '\0', sizeof(window->tool_id)) == NULL ||
            memchr(window->group_id, '\0', sizeof(window->group_id)) == NULL ||
            memchr(window->placement_id, '\0', sizeof(window->placement_id)) == NULL ||
            memchr(window->stack_id, '\0', sizeof(window->stack_id)) == NULL ||
            memchr(window->context_group_id, '\0', sizeof(window->context_group_id)) == NULL)
            return 0;
        if (!g_utf8_validate(window->window_id, -1, NULL) ||
            !g_utf8_validate(window->title, -1, NULL)) return 0;
        /* An auto-hide member has exactly one renderer. Reject contradictory
         * imported records before a dock and drawer could share one body. */
        if (strncmp(window->placement_id, "auto-hide:", 10U) == 0 &&
            (window->visible || window->floating || tool_window_edge_index(window) < 0))
            return 0;
    }
    return 1;
}

/* Compare semantic metadata explicitly: struct padding and unused text bytes
 * must not decide whether moving a panel discards an editor's unsaved draft. */
static int canvas_window_metadata_equal(
    const UmiUiWorkspaceWindow *left, const UmiUiWorkspaceWindow *right)
{
    return strcmp(left->window_id, right->window_id) == 0 &&
        strcmp(left->title, right->title) == 0 &&
        strcmp(left->tool_id, right->tool_id) == 0 &&
        strcmp(left->group_id, right->group_id) == 0 &&
        strcmp(left->placement_id, right->placement_id) == 0 &&
        strcmp(left->stack_id, right->stack_id) == 0 &&
        strcmp(left->context_group_id, right->context_group_id) == 0 &&
        left->visible == right->visible && left->floating == right->floating &&
        left->maximised == right->maximised && left->closable == right->closable &&
        left->z_order == right->z_order && left->pinned == right->pinned &&
        left->resizable == right->resizable;
}

/* Retain live provider widgets, selection and draft state for geometry-only
 * updates. Changes to identity, chrome, docking or native windows still take
 * the full rebuild path. The new render plan was validated before this call. */
static int refresh_canvas_geometry_without_rebuild(
    UmiGtk4WorkspaceLayoutHost *host, const UmiUiWorkspaceLayout *layout)
{
    size_t index;
    int geometry_changed = 0;

    if (host->canvas_layer == NULL || host->canvas_entries == NULL ||
        host->canvas_entries->len != host->plan.canvas_item_count ||
        host->layout.window_count != layout->window_count ||
        host->layout.locked != layout->locked ||
        strcmp(host->layout.layout_id, layout->layout_id) != 0 ||
        strcmp(host->layout.name, layout->name) != 0) return 0;
    for (index = 0U; index < layout->window_count; ++index) {
        const UmiUiWorkspaceWindow *old = &host->layout.windows[index];
        const UmiUiWorkspaceWindow *next = &layout->windows[index];
        const int canvas = next->visible && !next->floating &&
            strcmp(next->placement_id, UMI_UI_WORKSPACE_CANVAS_PLACEMENT) == 0;

        if (!canvas_window_metadata_equal(old, next)) return 0;
        if (!canvas && (old->x != next->x || old->y != next->y ||
            old->width != next->width || old->height != next->height)) return 0;
        if (canvas && find_canvas_entry(host, next->window_id) == NULL) return 0;
        if (canvas && (old->x != next->x || old->y != next->y ||
            old->width != next->width || old->height != next->height))
            geometry_changed = 1;
    }
    /* An explicit refresh of an unchanged model still asks the provider for
     * fresh data. Only an actual placement change opts into view retention. */
    if (!geometry_changed) return 0;
    host->layout = *layout;
    for (index = 0U; index < host->plan.canvas_item_count; ++index) {
        const UmiApplicationSuiteLayoutCanvasItem *item =
            &host->plan.canvas_items[index];
        CanvasEntry *entry = find_canvas_entry(host,
            host->layout.windows[item->window_index].window_id);

        entry->model_rect = item->rect;
        entry->preview_rect = item->rect;
        entry->source_revision = layout->revision;
    }
    update_canvas_controls(host);
    gtk_widget_queue_allocate(host->canvas_layer);
    host->revision += 1U;
    return 1;
}
