/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/interaction_recording_gtk4.c
 *
 * PURPOSE:
 *   Capture opt-in input observations, screenshots and local problem reports.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/interaction_recording.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/interaction_recording.h"
#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <stdio.h>
#include <string.h>

#define RECORDING_PANEL_KEY "umicom-interaction-recorder-panel"
#define RECORDING_PRIVATE_KEY "umicom-recording-private"
#define RECORDING_SCOPE_MAX 16U
#define RECORDING_IMAGE_MAX 20U
#define RECORDING_IMAGE_BYTES_MAX (64U * 1024U * 1024U)

typedef struct RecordingScope {
    GWeakRef window;
    GtkEventController *controller;
} RecordingScope;
typedef struct RecordingPanel {
    GWeakRef owner;
    GtkWindow *panel;
    GtkWidget *status;
    GtkWidget *note;
    GtkWidget *consent;
    GtkWidget *capture;
    GtkWidget *target;
    GPtrArray *captureWindows;
    UmiUiRecording *recording;
    RecordingScope scopes[RECORDING_SCOPE_MAX];
    size_t scopeCount;
    guint timer;
    gint64 started;
    char *directory;
    char *reportRoot;
    GOutputStream *liveLog;
    size_t imageBytes;
    unsigned images;
    unsigned exports;
    unsigned inspections;
    gboolean closing;
    gboolean logFailed;
} RecordingPanel;

/* Marking does not hide a widget or alter its application's data. */
UmiStatus UmiGtk4RecordingSetPrivate(void *nativeWidget, int isPrivate)
{
    if (nativeWidget == NULL || !GTK_IS_WIDGET(nativeWidget)) return UMI_STATUS_INVALID_ARGUMENT;
    g_object_set_data(G_OBJECT(nativeWidget), RECORDING_PRIVATE_KEY,
        isPrivate ? GINT_TO_POINTER(1) : NULL);
    return UMI_STATUS_OK;
}
static gboolean PrivateWidget(GtkWidget *w)
{
    if (g_object_get_data(G_OBJECT(w), RECORDING_PRIVATE_KEY) != NULL) return TRUE;
    if (GTK_IS_PASSWORD_ENTRY(w)) return TRUE;
    if (GTK_IS_ENTRY(w) && !gtk_entry_get_visibility(GTK_ENTRY(w))) return TRUE;
    if (GTK_IS_TEXT(w) && !gtk_text_get_visibility(GTK_TEXT(w))) return TRUE;
    return FALSE;
}
/* A hidden private field must also be protected from automation reads. */
int UmiGtk4RecordingIsPrivate(void *nativeWidget)
{
    GtkWidget *widget = nativeWidget;
    if (widget == NULL || !GTK_IS_WIDGET(widget)) return 0;
    for (; widget != NULL; widget = gtk_widget_get_parent(widget))
        if (PrivateWidget(widget)) return 1;
    return 0;
}
int UmiGtk4RecordingContainsPrivate(void *nativeWidget)
{
    GtkWidget *w = nativeWidget, *child, *ancestor;
    if (w == NULL || !GTK_IS_WIDGET(w) || !gtk_widget_is_visible(w)) return 0;
    /* Capturing a child must not bypass a private marker on its parent. */
    for (ancestor = w; ancestor != NULL; ancestor = gtk_widget_get_parent(ancestor)) {
        if (PrivateWidget(ancestor)) return 1;
    }
    for (child = gtk_widget_get_first_child(w); child != NULL;
         child = gtk_widget_get_next_sibling(child))
        if (UmiGtk4RecordingContainsPrivate(child)) return 1;
    return 0;
}
static UmiStatus IoStatus(const GError *error)
{
    if (error != NULL && g_error_matches(error, G_IO_ERROR, G_IO_ERROR_EXISTS)) return UMI_STATUS_ALREADY_EXISTS;
    if (error != NULL && g_error_matches(error, G_IO_ERROR, G_IO_ERROR_PERMISSION_DENIED)) return UMI_STATUS_PERMISSION_DENIED;
    return UMI_STATUS_IO_ERROR;
}
/* Exclusive creation prevents overwriting a user's previous image or report. */
static UmiStatus WriteNewFile(const char *path, const void *bytes, gsize length)
{
    GFile *file = g_file_new_for_path(path);
    GError *error = NULL;
    GFileOutputStream *stream = g_file_create(file, G_FILE_CREATE_PRIVATE, NULL, &error);
    UmiStatus status = UMI_STATUS_OK;
    if (stream == NULL) status = IoStatus(error);
    else {
        if (!g_output_stream_write_all(G_OUTPUT_STREAM(stream), bytes, length, NULL, NULL, &error))
            status = IoStatus(error);
        g_clear_error(&error);
        if (!g_output_stream_close(G_OUTPUT_STREAM(stream), NULL, &error) && status == UMI_STATUS_OK)
            status = IoStatus(error);
        g_object_unref(stream);
        if (status != UMI_STATUS_OK) (void)g_file_delete(file, NULL, NULL);
    }
    g_clear_error(&error); g_object_unref(file); return status;
}
UmiStatus UmiGtk4RecordingCapturePng(void *nativeWidget, const char *path)
{
    GtkWidget *widget = nativeWidget;
    GtkNative *native;
    GskRenderer *renderer;
    GdkPaintable *paintable;
    GtkSnapshot *snapshot;
    GskRenderNode *node;
    GdkTexture *texture;
    GBytes *png;
    graphene_rect_t bounds;
    int width, height, scale;
    gsize size;
    const void *bytes;
    UmiStatus status;
    if (widget == NULL || !GTK_IS_WIDGET(widget) || path == NULL || path[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!gtk_widget_get_mapped(widget)) return UMI_STATUS_UNAVAILABLE;
    if (UmiGtk4RecordingContainsPrivate(widget)) return UMI_STATUS_PERMISSION_DENIED;
    native = gtk_widget_get_native(widget);
    renderer = native != NULL ? gtk_native_get_renderer(native) : NULL;
    width = gtk_widget_get_width(widget); height = gtk_widget_get_height(widget);
    scale = gtk_widget_get_scale_factor(widget);
    if (renderer == NULL || width <= 0 || height <= 0 || scale <= 0) return UMI_STATUS_UNAVAILABLE;
    if (width > 4096 / scale || height > 4096 / scale) return UMI_STATUS_CAPACITY_EXCEEDED;
    paintable = gtk_widget_paintable_new(widget);
    snapshot = gtk_snapshot_new();
    gtk_snapshot_scale(snapshot, (float)scale, (float)scale);
    gdk_paintable_snapshot(paintable, GDK_SNAPSHOT(snapshot), (double)width, (double)height);
    node = gtk_snapshot_free_to_node(snapshot); g_object_unref(paintable);
    if (node == NULL) return UMI_STATUS_UNAVAILABLE;
    graphene_rect_init(&bounds, 0.0F, 0.0F, (float)(width * scale), (float)(height * scale));
    texture = gsk_renderer_render_texture(renderer, node, &bounds);
    gsk_render_node_unref(node);
    if (texture == NULL) return UMI_STATUS_UNAVAILABLE;
    png = gdk_texture_save_to_png_bytes(texture); g_object_unref(texture);
    if (png == NULL) return UMI_STATUS_IO_ERROR;
    bytes = g_bytes_get_data(png, &size);
    status = size <= 16U * 1024U * 1024U ? WriteNewFile(path, bytes, size) : UMI_STATUS_CAPACITY_EXCEEDED;
    g_bytes_unref(png); return status;
}
static void Status(RecordingPanel *p, const char *text) { gtk_label_set_text(GTK_LABEL(p->status), text); }
static UmiStatus StreamWrite(void *context, const char *bytes, size_t length)
{
    return g_output_stream_write_all(G_OUTPUT_STREAM(context), bytes, length, NULL, NULL, NULL)
        ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}
/* Session files are local exports, not a separate application database. */
static UmiStatus EnsureDirectory(RecordingPanel *p)
{
    char *base, *id;
    if (p->directory != NULL) return UMI_STATUS_OK;
    base = p->reportRoot != NULL ? g_strdup(p->reportRoot)
        : g_build_filename(g_get_user_data_dir(), "umicom", "gui-recordings", NULL);
    if (g_mkdir_with_parents(base, 0700) != 0) { g_free(base); return UMI_STATUS_IO_ERROR; }
    id = g_uuid_string_random();
    p->directory = g_build_filename(base, id, NULL); g_free(base); g_free(id);
    if (g_mkdir(p->directory, 0700) != 0) { g_clear_pointer(&p->directory, g_free); return UMI_STATUS_IO_ERROR; }
    return UMI_STATUS_OK;
}
static UmiStatus ExportOne(RecordingPanel *p, const char *name, gboolean html);
static UmiStatus StartLiveLog(RecordingPanel *p)
{
    UmiStatus status = EnsureDirectory(p); GFile *file; char *path;
    char header[256];
    if (p->logFailed) return UMI_STATUS_IO_ERROR;
    if (status != UMI_STATUS_OK || p->liveLog != NULL) return status;
    path = g_build_filename(p->directory, "events-live.jsonl", NULL);
    file = g_file_new_for_path(path); g_free(path);
    {
        GFileOutputStream *raw = g_file_create(file, G_FILE_CREATE_PRIVATE, NULL, NULL);
        if (raw != NULL) {
            p->liveLog = g_buffered_output_stream_new_sized(G_OUTPUT_STREAM(raw), 16384U);
            g_object_unref(raw);
        }
    }
    g_object_unref(file);
    if (p->liveLog == NULL) return UMI_STATUS_IO_ERROR;
    /* Retain application/build identity even when a later crash prevents export. */
    if (ExportOne(p, "session.json", FALSE) != UMI_STATUS_OK) {
        p->logFailed = TRUE; return UMI_STATUS_IO_ERROR;
    }
    (void)snprintf(header, sizeof(header),
        "{\"schema\":\"umicom.ui.recording.live\",\"schema_revision\":1,\"gtk\":\"%u.%u.%u\",\"typed_text_recorded\":false}\n",
        gtk_get_major_version(), gtk_get_minor_version(), gtk_get_micro_version());
    if (StreamWrite(p->liveLog, header, strlen(header)) != UMI_STATUS_OK ||
        !g_output_stream_flush(p->liveLog, NULL, NULL)) { p->logFailed = TRUE; return UMI_STATUS_IO_ERROR; }
    return UMI_STATUS_OK;
}
static UmiStatus Append(RecordingPanel *p, UmiUiRecordingEntry *e)
{
    UmiStatus status;
    e->elapsedMs = (uint64_t)((g_get_monotonic_time() - p->started) / 1000);
    status = UmiUiRecordingAppend(p->recording, e);
    if (status == UMI_STATUS_OK && p->liveLog != NULL) {
        status = UmiUiRecordingWriteEntryJson(p->recording,
            UmiUiRecordingCount(p->recording) - 1U, StreamWrite, p->liveLog);
        if (status == UMI_STATUS_OK && !g_output_stream_flush(p->liveLog, NULL, NULL)) status = UMI_STATUS_IO_ERROR;
    }
    if (status != UMI_STATUS_OK) {
        if (status == UMI_STATUS_IO_ERROR) p->logFailed = TRUE;
        (void)UmiUiRecordingSetActive(p->recording, 0);
        Status(p, "Recording stopped: the observation or local log could not be retained. Export the retained entries.");
    } else if (UmiUiRecordingLimitReached(p->recording)) {
        Status(p, "Recording stopped at 4096 observations. Export this report, then open a new recording.");
    }
    return status;
}
/* Names come from stable IDs, never user documents, window titles or values. */
static void Target(GtkWidget *widget, char *out, size_t capacity)
{
    GtkWidget *current; const char *id = NULL;
    for (current = widget; current != NULL; current = gtk_widget_get_parent(current)) {
        if (PrivateWidget(current)) { (void)snprintf(out, capacity, "private-control"); return; }
        if (id == NULL) id = g_object_get_data(G_OBJECT(current), "umicom-automation-id");
    }
    if (id != NULL && strlen(id) < capacity && g_utf8_validate(id, -1, NULL))
        (void)snprintf(out, capacity, "%s", id);
    else (void)snprintf(out, capacity, "unidentified.%s", widget != NULL ? G_OBJECT_TYPE_NAME(widget) : "window");
}
static gboolean Related(GtkWindow *candidate, GtkWindow *owner);

/* Every record's parent is already stored. Depth and capacity bounds prevent
 * malformed or enormous trees from becoming a successful partial export. */
static UmiStatus CaptureControl(GtkWidget *widget, uint32_t scopeId,
    size_t parentIndex, unsigned depth, UmiUiControlInventory *inventory)
{
    UmiUiControlRecord record = {0};
    GtkWidget *child;
    const char *id;
    UmiStatus status;
    size_t index;
    if (depth > 256U) return UMI_STATUS_CAPACITY_EXCEEDED;
    record.scopeId = scopeId; record.parentIndex = parentIndex;
    record.privateControl = UmiGtk4RecordingIsPrivate(widget);
    record.visible = gtk_widget_is_visible(widget) != FALSE;
    record.mapped = gtk_widget_get_mapped(widget) != FALSE;
    record.enabled = gtk_widget_is_sensitive(widget) != FALSE;
    record.focused = gtk_widget_has_focus(widget) != FALSE;
    record.interactive = GTK_IS_BUTTON(widget) || GTK_IS_CHECK_BUTTON(widget) ||
        GTK_IS_EDITABLE(widget) || GTK_IS_TEXT_VIEW(widget) || GTK_IS_DROP_DOWN(widget) ||
        GTK_IS_MENU_BUTTON(widget) || GTK_IS_SWITCH(widget) || GTK_IS_RANGE(widget) ||
        gtk_widget_get_focusable(widget);
    if (record.privateControl) {
        (void)g_strlcpy(record.roleName, "private-control", sizeof(record.roleName));
    } else {
        if (g_strlcpy(record.roleName, G_OBJECT_TYPE_NAME(widget), sizeof(record.roleName)) >= sizeof(record.roleName))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        id = g_object_get_data(G_OBJECT(widget), "umicom-automation-id");
        if (id != NULL && g_strlcpy(record.automationId, id, sizeof(record.automationId)) >= sizeof(record.automationId))
            return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    index = UmiUiControlInventoryCount(inventory);
    status = UmiUiControlInventoryAdd(inventory, &record);
    if (status != UMI_STATUS_OK || record.privateControl) return status;
    for (child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        status = CaptureControl(child, scopeId, index, depth + 1U, inventory);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiGtk4ControlInventoryCapture(void *nativeWindow, size_t capacity,
    UmiUiControlInventory **outInventory)
{
    GtkWindow *owner = nativeWindow;
    UmiUiControlInventory *inventory = NULL;
    UmiStatus status;
    GListModel *windows;
    guint i;
    uint32_t scope = 1U;
    if (outInventory == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outInventory = NULL;
    if (owner == NULL || !GTK_IS_WINDOW(owner) ||
        g_object_get_data(G_OBJECT(owner), "umicom-recorder-window") != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiUiControlInventoryCreate(capacity, &inventory);
    if (status != UMI_STATUS_OK) return status;
    status = CaptureControl(GTK_WIDGET(owner), scope, UMI_UI_CONTROL_NO_PARENT, 0U, inventory);
    windows = gtk_window_get_toplevels();
    for (i = 0U; status == UMI_STATUS_OK && windows != NULL && i < g_list_model_get_n_items(windows); ++i) {
        GtkWindow *window = g_list_model_get_item(windows, i);
        if (window == NULL) continue;
        if (window != owner && Related(window, owner) &&
            g_object_get_data(G_OBJECT(window), "umicom-recorder-window") == NULL) {
            ++scope;
            status = CaptureControl(GTK_WIDGET(window), scope, UMI_UI_CONTROL_NO_PARENT, 0U, inventory);
        }
        g_object_unref(window);
    }
    if (status != UMI_STATUS_OK) { UmiUiControlInventoryDestroy(inventory); return status; }
    *outInventory = inventory;
    return UMI_STATUS_OK;
}

/* Observe input at capture phase without consuming it or emitting commands.
 * A no-op button still leaves an attempted input in the report. */
static gboolean OnInput(GtkEventControllerLegacy *controller, GdkEvent *event, gpointer data)
{
    RecordingPanel *p = data;
    GtkWidget *window = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(controller));
    GtkWidget *target = NULL; UmiUiRecordingEntry e = {0}; GdkEventType type;
    if (p->closing || window == NULL || !UmiUiRecordingIsActive(p->recording)) return FALSE;
    {
        GtkWindow *owner = g_weak_ref_get(&p->owner);
        gboolean related = owner != NULL && Related(GTK_WINDOW(window), owner);
        g_clear_object(&owner);
        if (!related) return FALSE;
    }
    type = gdk_event_get_event_type(event);
    e.kind = UMI_UI_RECORDING_INPUT;
    if (type == GDK_BUTTON_RELEASE) {
        double x, y;
        (void)snprintf(e.action, sizeof(e.action), "pointer-button-%u", gdk_button_event_get_button(event));
        if (gdk_event_get_surface(event) == gtk_native_get_surface(GTK_NATIVE(window)) &&
            gdk_event_get_position(event, &x, &y)) {
            double dx, dy; gtk_native_get_surface_transform(GTK_NATIVE(window), &dx, &dy);
            target = gtk_widget_pick(window, x + dx, y + dy, GTK_PICK_INSENSITIVE | GTK_PICK_NON_TARGETABLE);
        }
        (void)snprintf(e.detail, sizeof(e.detail), "Pointer release observed; application outcome not asserted.");
    } else if (type == GDK_KEY_PRESS) {
        GdkModifierType modifiers = gdk_event_get_modifier_state(event);
        target = gtk_window_get_focus(GTK_WINDOW(window));
        (void)snprintf(e.action, sizeof(e.action), "%s",
            (modifiers & (GDK_CONTROL_MASK | GDK_ALT_MASK)) != 0 ? "shortcut-input" : "keyboard-input");
        (void)snprintf(e.detail, sizeof(e.detail), "Key value omitted. Control=%u Shift=%u Alt=%u.",
            (unsigned)((modifiers & GDK_CONTROL_MASK) != 0), (unsigned)((modifiers & GDK_SHIFT_MASK) != 0), (unsigned)((modifiers & GDK_ALT_MASK) != 0));
    } else if (type == GDK_SCROLL) {
        (void)snprintf(e.action, sizeof(e.action), "scroll-input");
        (void)snprintf(e.detail, sizeof(e.detail), "Scroll observed; application outcome not asserted.");
    } else return FALSE;
    Target(target, e.targetId, sizeof(e.targetId)); (void)Append(p, &e);
    return FALSE;
}
static gboolean Related(GtkWindow *candidate, GtkWindow *owner)
{
    size_t depth = 0U;
    while (candidate != NULL && depth++ < 32U) {
        if (g_object_get_data(G_OBJECT(candidate), "umicom-recorder-window") != NULL) return FALSE;
        if (candidate == owner) return TRUE;
        candidate = gtk_window_get_transient_for(candidate);
    }
    return FALSE;
}
static void FreeWeak(gpointer value) { GWeakRef *w = value; g_weak_ref_clear(w); g_free(w); }
/* Add related GTK dialogs as they appear. Scope is bounded; overflow pauses
 * collection with a visible explanation rather than silently missing windows. */
static gboolean RefreshScopes(RecordingPanel *p)
{
    GtkWindow *owner = g_weak_ref_get(&p->owner);
    GListModel *windows = gtk_window_get_toplevels(); guint i;
    GtkStringList *names = gtk_string_list_new(NULL);
    GPtrArray *next = g_ptr_array_new_with_free_func(FreeWeak);
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(p->target));
    GObject *selectedWindow = selected < p->captureWindows->len
        ? g_weak_ref_get(g_ptr_array_index(p->captureWindows, selected)) : NULL;
    gboolean complete = TRUE, changed;
    /* Retire dead windows in place: GWeakRef objects must not be memmoved. */
    for (size_t j = 0U; j < p->scopeCount; ++j) {
        RecordingScope *scope = &p->scopes[j]; GObject *prior;
        if (scope->controller == NULL) continue;
        prior = g_weak_ref_get(&scope->window);
        if (prior == NULL || owner == NULL || !Related(GTK_WINDOW(prior), owner)) {
            g_signal_handlers_disconnect_by_data(scope->controller, p);
            if (prior != NULL && gtk_event_controller_get_widget(scope->controller) == GTK_WIDGET(prior))
                gtk_widget_remove_controller(GTK_WIDGET(prior), scope->controller);
            g_object_unref(scope->controller); scope->controller = NULL;
            g_weak_ref_clear(&scope->window);
        }
        g_clear_object(&prior);
    }
    if (owner != NULL) for (i = 0U; i < g_list_model_get_n_items(windows); ++i) {
        GtkWindow *w = g_list_model_get_item(windows, i);
        size_t j, available = RECORDING_SCOPE_MAX; gboolean found = FALSE;
        if (!Related(w, owner)) { g_object_unref(w); continue; }
        for (j = 0U; j < p->scopeCount; ++j) {
            GObject *prior;
            if (p->scopes[j].controller == NULL) { if (available == RECORDING_SCOPE_MAX) available = j; continue; }
            prior = g_weak_ref_get(&p->scopes[j].window);
            if (prior == G_OBJECT(w)) found = TRUE;
            g_clear_object(&prior);
        }
        if (!found) {
            if (available == RECORDING_SCOPE_MAX && p->scopeCount < RECORDING_SCOPE_MAX) available = p->scopeCount++;
            if (available != RECORDING_SCOPE_MAX) {
                RecordingScope *scope = &p->scopes[available];
                g_weak_ref_init(&scope->window, w);
                scope->controller = gtk_event_controller_legacy_new();
                g_object_ref(scope->controller);
                gtk_event_controller_set_propagation_phase(scope->controller, GTK_PHASE_CAPTURE);
                g_signal_connect(scope->controller, "event", G_CALLBACK(OnInput), p);
                gtk_widget_add_controller(GTK_WIDGET(w), scope->controller);
            } else complete = FALSE;
        }
        if (gtk_widget_get_mapped(GTK_WIDGET(w))) {
            GWeakRef *weak = g_new0(GWeakRef, 1); char label[64];
            g_weak_ref_init(weak, w); g_ptr_array_add(next, weak);
            (void)snprintf(label, sizeof(label), "%s %u", w == owner ? "Application window" : "Application dialog", next->len);
            gtk_string_list_append(names, label);
        }
        g_object_unref(w);
    }
    changed = next->len != p->captureWindows->len;
    for (i = 0U; !changed && i < next->len; ++i) {
        GObject *a = g_weak_ref_get(g_ptr_array_index(next, i));
        GObject *b = g_weak_ref_get(g_ptr_array_index(p->captureWindows, i));
        changed = a != b; g_clear_object(&a); g_clear_object(&b);
    }
    if (changed) {
        guint chosen = GTK_INVALID_LIST_POSITION;
        for (i = 0U; i < next->len; ++i) {
            GObject *w = g_weak_ref_get(g_ptr_array_index(next, i));
            if (w == selectedWindow || (selectedWindow == NULL && p->captureWindows->len == 0U && w == G_OBJECT(owner))) chosen = i;
            g_clear_object(&w);
        }
        g_ptr_array_unref(p->captureWindows); p->captureWindows = next; next = NULL;
        gtk_drop_down_set_model(GTK_DROP_DOWN(p->target), G_LIST_MODEL(names));
        gtk_drop_down_set_selected(GTK_DROP_DOWN(p->target), chosen);
    }
    if (!complete) {
        (void)UmiUiRecordingSetActive(p->recording, 0);
        Status(p, "Recording paused: more than 16 related windows are open. Close unused dialogs before resuming.");
    }
    if (next != NULL) g_ptr_array_unref(next);
    g_clear_object(&selectedWindow); g_object_unref(names); g_clear_object(&owner);
    return complete;
}

static gboolean Tick(gpointer data)
{
    RecordingPanel *p = data;
    if (p->closing) return G_SOURCE_REMOVE;
    RefreshScopes(p);
    gtk_widget_set_sensitive(p->capture, UmiUiRecordingIsActive(p->recording) &&
        gtk_check_button_get_active(GTK_CHECK_BUTTON(p->consent)) && p->images < RECORDING_IMAGE_MAX);
    return G_SOURCE_CONTINUE;
}
static void Detach(RecordingPanel *p)
{
    size_t i;
    if (p->timer != 0U) { g_source_remove(p->timer); p->timer = 0U; }
    for (i = 0U; i < p->scopeCount; ++i) {
        RecordingScope *s = &p->scopes[i]; GObject *w;
        if (s->controller == NULL) continue;
        w = g_weak_ref_get(&s->window);
        g_signal_handlers_disconnect_by_data(s->controller, p);
        if (w != NULL && gtk_event_controller_get_widget(s->controller) == GTK_WIDGET(w))
            gtk_widget_remove_controller(GTK_WIDGET(w), s->controller);
        g_clear_object(&w); g_object_unref(s->controller); g_weak_ref_clear(&s->window);
    }
    p->scopeCount = 0U;
}
static void Stop(RecordingPanel *p)
{
    (void)UmiUiRecordingSetActive(p->recording, 0);
    if (p->liveLog != NULL) (void)g_output_stream_flush(p->liveLog, NULL, NULL);
}
static void OnUnrealize(GtkWidget *widget, gpointer data)
{
    RecordingPanel *p = data; GObject *owner = g_weak_ref_get(&p->owner); (void)widget;
    p->closing = TRUE; Stop(p); Detach(p);
    if (owner != NULL && g_object_get_data(owner, RECORDING_PANEL_KEY) == p->panel)
        g_object_set_data(owner, RECORDING_PANEL_KEY, NULL);
    g_clear_object(&owner);
}
static void DestroyPanel(gpointer data)
{
    RecordingPanel *p = data;
    GObject *owner = g_weak_ref_get(&p->owner);
    if (owner != NULL && g_object_get_data(owner, RECORDING_PANEL_KEY) == p->panel)
        g_object_set_data(owner, RECORDING_PANEL_KEY, NULL);
    g_clear_object(&owner);
    Detach(p);
    if (p->liveLog != NULL) { (void)g_output_stream_close(p->liveLog, NULL, NULL); g_object_unref(p->liveLog); }
    g_weak_ref_clear(&p->owner); g_ptr_array_unref(p->captureWindows);
    UmiUiRecordingDestroy(p->recording); g_free(p->directory); g_free(p->reportRoot); g_free(p);
}
/* Weak object-bound signals disconnect when the panel is released. A kept
 * button cannot invoke a freed RecordingPanel after its window is closed. */
static RecordingPanel *FromPanel(gpointer object)
{
    RecordingPanel *p = g_object_get_data(G_OBJECT(object), "umicom-recorder-state");
    return p != NULL && !p->closing ? p : NULL;
}
static void OnStart(GtkButton *button, gpointer data)
{
    RecordingPanel *p = FromPanel(data); UmiStatus s; (void)button;
    if (p == NULL) return;
    if (p->closing) return;
    if (!RefreshScopes(p)) return;
    if (p->started == 0) p->started = g_get_monotonic_time();
    s = StartLiveLog(p);
    if (s == UMI_STATUS_OK) s = UmiUiRecordingSetActive(p->recording, 1);
    Status(p, s == UMI_STATUS_OK ? "Recording input observations. Reproduce the problem, then add a note or pause. Typed characters are omitted."
        : "Recording could not start. Check local storage or open a fresh session after exporting this one.");
}
static void OnPause(GtkButton *button, gpointer data)
{
    RecordingPanel *p = FromPanel(data); (void)button;
    if (p == NULL) return;
    if (p->closing) return;
    Stop(p); Status(p, "Paused. No new input is collected. Start resumes this session; Export writes its reports.");
}
static void OnNote(GtkButton *button, gpointer data)
{
    RecordingPanel *p = FromPanel(data); UmiUiRecordingEntry e = {0}; const char *text; (void)button;
    if (p == NULL) return;
    if (p->closing || !UmiUiRecordingIsActive(p->recording)) { Status(p, "Start recording before adding a note."); return; }
    text = gtk_editable_get_text(GTK_EDITABLE(p->note));
    if (text[0] == '\0' || strlen(text) >= sizeof(e.detail)) { Status(p, "Enter a short description before choosing Add note."); return; }
    e.kind = UMI_UI_RECORDING_NOTE;
    (void)snprintf(e.action, sizeof(e.action), "user-note"); memcpy(e.detail, text, strlen(text) + 1U);
    if (Append(p, &e) == UMI_STATUS_OK) {
        gtk_editable_set_text(GTK_EDITABLE(p->note), "");
        if (!UmiUiRecordingLimitReached(p->recording))
            Status(p, "Note added. Continue reproducing the problem or export the report.");
    }
}
static void OnCapture(GtkButton *button, gpointer data)
{
    RecordingPanel *p = FromPanel(data); UmiUiRecordingEntry e = {0}; guint index; GObject *target;
    char *path; UmiStatus status; GStatBuf info; (void)button;
    if (p == NULL) return;
    if (p->closing || !UmiUiRecordingIsActive(p->recording) ||
        !gtk_check_button_get_active(GTK_CHECK_BUTTON(p->consent))) return;
    if (p->images >= RECORDING_IMAGE_MAX || p->imageBytes >= RECORDING_IMAGE_BYTES_MAX) {
        Status(p, "Screenshot limit reached. Export this report before creating another session."); return;
    }
    index = gtk_drop_down_get_selected(GTK_DROP_DOWN(p->target));
    if (index >= p->captureWindows->len) { Status(p, "Select an application window to capture."); return; }
    target = g_weak_ref_get(g_ptr_array_index(p->captureWindows, index));
    if (target == NULL) { Status(p, "That window has closed. Select the current window."); return; }
    /* A dialog may have changed owner since the last dropdown refresh. */
    {
        GtkWindow *owner = g_weak_ref_get(&p->owner);
        gboolean allowed = owner != NULL && Related(GTK_WINDOW(target), owner);
        g_clear_object(&owner);
        if (!allowed) {
            g_object_unref(target);
            Status(p, "That window no longer belongs to this application window. Select the current window.");
            return;
        }
    }
    status = EnsureDirectory(p);
    (void)snprintf(e.attachment, sizeof(e.attachment), "frame-%06u.png", ++p->images);
    path = status == UMI_STATUS_OK ? g_build_filename(p->directory, e.attachment, NULL) : NULL;
    if (status == UMI_STATUS_OK) status = UmiGtk4RecordingCapturePng(target, path);
    if (status == UMI_STATUS_OK && (g_stat(path, &info) != 0 || info.st_size < 0 ||
        (guint64)info.st_size > RECORDING_IMAGE_BYTES_MAX - p->imageBytes)) {
        (void)g_remove(path); status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (status == UMI_STATUS_OK) p->imageBytes += (size_t)info.st_size;
    e.kind = UMI_UI_RECORDING_SCREENSHOT; e.outcomeKnown = 1; e.status = status;
    (void)snprintf(e.action, sizeof(e.action), "capture-window");
    (void)snprintf(e.detail, sizeof(e.detail), "%s", status == UMI_STATUS_OK ? "GTK window content captured with explicit consent." : umi_status_text(status));
    if (status != UMI_STATUS_OK) e.attachment[0] = '\0';
    if (Append(p, &e) == UMI_STATUS_OK && !UmiUiRecordingLimitReached(p->recording)) Status(p, status == UMI_STATUS_OK ? "Screenshot saved. Review the image before sharing it."
        : status == UMI_STATUS_PERMISSION_DENIED ? "Image blocked: this window contains a password or private control. Close it before capturing."
        : "Screenshot failed or exceeded its size limit. No successful capture is claimed.");
    g_free(path); g_object_unref(target);
}
/* One exclusive, buffered output path serves interaction and control maps. */
typedef UmiStatus (*ExportDocumentFn)(const void *, UmiUiRecordingWriteFn, void *);
static UmiStatus RecordingJson(const void *record, UmiUiRecordingWriteFn write, void *context)
{ return UmiUiRecordingWriteJson(record, write, context); }
static UmiStatus RecordingHtml(const void *record, UmiUiRecordingWriteFn write, void *context)
{ return UmiUiRecordingWriteHtml(record, write, context); }
static UmiStatus InventoryJson(const void *record, UmiUiRecordingWriteFn write, void *context)
{ return UmiUiControlInventoryWriteJson(record, write, context); }
static UmiStatus InventoryHtml(const void *record, UmiUiRecordingWriteFn write, void *context)
{ return UmiUiControlInventoryWriteHtml(record, write, context); }
/* Export to new files. A failed stream leaves no completed-looking report. */
static UmiStatus ExportDocument(RecordingPanel *p, const char *name,
    ExportDocumentFn serialize, const void *document)
{
    char *path = g_build_filename(p->directory, name, NULL);
    GFile *file = g_file_new_for_path(path);
    GFileOutputStream *raw = g_file_create(file, G_FILE_CREATE_PRIVATE, NULL, NULL);
    GOutputStream *stream = raw != NULL ? g_buffered_output_stream_new_sized(G_OUTPUT_STREAM(raw), 16384U) : NULL;
    UmiStatus status = UMI_STATUS_IO_ERROR;
    if (raw != NULL) g_object_unref(raw);
    if (stream != NULL) {
        status = serialize(document, StreamWrite, stream);
        if (!g_output_stream_close(stream, NULL, NULL) && status == UMI_STATUS_OK)
            status = UMI_STATUS_IO_ERROR;
        g_object_unref(stream);
        if (status != UMI_STATUS_OK) (void)g_file_delete(file, NULL, NULL);
    }
    g_object_unref(file); g_free(path); return status;
}
static UmiStatus ExportOne(RecordingPanel *p, const char *name, gboolean html)
{
    return ExportDocument(p, name, html ? RecordingHtml : RecordingJson, p->recording);
}
/* Inspection does not start recording or invoke any application control. */
static void OnInspectControls(GtkButton *button, gpointer data)
{
    RecordingPanel *p = FromPanel(data);
    GtkWindow *owner;
    UmiUiControlInventory *inventory = NULL;
    UmiUiControlInventorySummary summary;
    UmiStatus status;
    char name[64];
    char *message;
    (void)button;
    if (p == NULL || p->closing) return;
    if (p->inspections >= 20U) { Status(p, "Twenty control maps have been requested. Open a new report window to capture more."); return; }
    owner = g_weak_ref_get(&p->owner);
    if (owner == NULL) { Status(p, "The application window has closed."); return; }
    status = UmiGtk4ControlInventoryCapture(owner, UMI_UI_CONTROL_INVENTORY_MAX, &inventory);
    g_object_unref(owner);
    if (status == UMI_STATUS_OK) status = UmiUiControlInventorySummarise(inventory, &summary);
    if (status == UMI_STATUS_OK) status = EnsureDirectory(p);
    ++p->inspections;
    (void)snprintf(name, sizeof(name), "controls-%06u.json", p->inspections);
    if (status == UMI_STATUS_OK) status = ExportDocument(p, name, InventoryJson, inventory);
    (void)snprintf(name, sizeof(name), "controls-%06u.html", p->inspections);
    if (status == UMI_STATUS_OK) status = ExportDocument(p, name, InventoryHtml, inventory);
    (void)snprintf(name, sizeof(name), "controls-%06u-session.json", p->inspections);
    if (status == UMI_STATUS_OK) status = ExportOne(p, name, FALSE);
    if (status == UMI_STATUS_OK) {
        message = g_strdup_printf("Control map saved: %zu controls; %zu interactive controls without a test target; %zu repeated target records. Open folder to inspect the HTML. No control was activated.",
            summary.controls, summary.unaddressableInteractive, summary.ambiguousTargets);
        Status(p, message); g_free(message);
    } else {
        message = g_strdup_printf("Control inspection failed: %s. No incomplete control map is published as a successful capture.", umi_status_text(status));
        Status(p, message); g_free(message);
    }
    UmiUiControlInventoryDestroy(inventory);
}
static void OnExport(GtkButton *button, gpointer data)
{
    RecordingPanel *p = FromPanel(data); char name[64]; UmiStatus s; char *message; (void)button;
    if (p == NULL) return;
    if (p->closing) return;
    Stop(p); s = EnsureDirectory(p); ++p->exports;
    (void)snprintf(name, sizeof(name), "events-%06u.json", p->exports);
    if (s == UMI_STATUS_OK) s = ExportOne(p, name, FALSE);
    (void)snprintf(name, sizeof(name), "report-%06u.html", p->exports);
    if (s == UMI_STATUS_OK) s = ExportOne(p, name, TRUE);
    message = s == UMI_STATUS_OK ? g_strdup_printf("Reports saved in %s. Open the folder, review the files, then share only what is needed.", p->directory)
        : g_strdup("Report export failed. Your retained observations are still in this window; check storage and try again.");
    Status(p, message); g_free(message);
}
static void OnOpenFolder(GtkButton *button, gpointer data)
{
    RecordingPanel *p = FromPanel(data); char *uri; (void)button;
    if (p == NULL) return;
    if (p->directory == NULL) { Status(p, "Start recording or export first to create a local report folder."); return; }
    uri = g_filename_to_uri(p->directory, NULL, NULL);
    if (uri == NULL || !g_app_info_launch_default_for_uri(uri, NULL, NULL)) Status(p, "The folder could not be opened. Its path is shown after Export.");
    g_free(uri);
}
static GtkWidget *Button(GtkWidget *box, const char *label, const char *id, GCallback callback, RecordingPanel *p)
{
    GtkWidget *b = gtk_button_new_with_label(label);
    (void)umi_gtk4_automation_tag_widget(b, id);
    g_signal_connect_object(b, "clicked", callback, G_OBJECT(p->panel), 0); gtk_box_append(GTK_BOX(box), b); return b;
}
UmiStatus UmiGtk4RecordingPanelShow(void *nativeWindow, const char *applicationId)
{
    return UmiGtk4RecordingPanelShowAt(nativeWindow, applicationId, NULL);
}
UmiStatus UmiGtk4RecordingPanelShowAt(void *nativeWindow, const char *applicationId,
    const char *reportDirectory)
{
    GtkWindow *owner = nativeWindow; GtkWidget *box, *row, *text; RecordingPanel *p;
    GtkWindow *existing; char build[256]; UmiStatus status;
    if (owner == NULL || !GTK_IS_WINDOW(owner) || !gtk_widget_get_mapped(GTK_WIDGET(owner))) return UMI_STATUS_INVALID_ARGUMENT;
    if (reportDirectory != NULL && !g_path_is_absolute(reportDirectory)) return UMI_STATUS_INVALID_ARGUMENT;
    existing = g_object_get_data(G_OBJECT(owner), RECORDING_PANEL_KEY);
    if (existing != NULL) { gtk_window_present(existing); return UMI_STATUS_OK; }
    p = g_new0(RecordingPanel, 1);
    {
        char *os = g_get_os_info(G_OS_INFO_KEY_NAME);
        char *version = g_get_os_info(G_OS_INFO_KEY_VERSION_ID);
        (void)snprintf(build, sizeof(build), "GTK %u.%u.%u; OS %.60s %.40s; %zu-bit process; attach repository revisions",
            gtk_get_major_version(), gtk_get_minor_version(), gtk_get_micro_version(),
            os != NULL ? os : "unknown", version != NULL ? version : "unknown", sizeof(void *) * 8U);
        g_free(os); g_free(version);
    }
    status = UmiUiRecordingCreate(applicationId, build, UMI_UI_RECORDING_MAX_ENTRIES, &p->recording);
    if (status != UMI_STATUS_OK) { g_free(p); return status; }
    p->reportRoot = g_strdup(reportDirectory);
    g_weak_ref_init(&p->owner, owner); p->captureWindows = g_ptr_array_new_with_free_func(FreeWeak);
    p->panel = GTK_WINDOW(gtk_window_new());
    g_object_set_data(G_OBJECT(p->panel), "umicom-recorder-window", GINT_TO_POINTER(1));
    (void)umi_gtk4_automation_tag_widget(p->panel, "recording.window");
    g_object_set_data_full(G_OBJECT(p->panel), "umicom-recorder-state", p, DestroyPanel);
    g_object_set_data(G_OBJECT(owner), RECORDING_PANEL_KEY, p->panel);
    gtk_window_set_title(p->panel, "Report a GUI problem");
    gtk_window_set_icon_name(p->panel, gtk_window_get_icon_name(owner));
    gtk_window_set_default_size(p->panel, 660, 420);
    gtk_window_set_transient_for(p->panel, owner); gtk_window_set_destroy_with_parent(p->panel, TRUE);
    g_signal_connect(p->panel, "unrealize", G_CALLBACK(OnUnrealize), p);
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_top(box, 18); gtk_widget_set_margin_bottom(box, 18);
    gtk_widget_set_margin_start(box, 18); gtk_widget_set_margin_end(box, 18);
    gtk_window_set_child(p->panel, box);
    text = gtk_label_new("Record the actions that lead to a problem. Start is opt-in; Pause stops collection. Typed characters, clipboard contents and other applications are not recorded. Input observations alone do not prove that an action worked.");
    gtk_label_set_wrap(GTK_LABEL(text), TRUE); gtk_label_set_xalign(GTK_LABEL(text), 0.0F); gtk_box_append(GTK_BOX(box), text);
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6); gtk_box_append(GTK_BOX(box), row);
    (void)Button(row, "Start", "recording.start", G_CALLBACK(OnStart), p);
    (void)Button(row, "Pause", "recording.pause", G_CALLBACK(OnPause), p);
    (void)Button(row, "Export report", "recording.export", G_CALLBACK(OnExport), p);
    (void)Button(row, "Open folder", "recording.open-folder", G_CALLBACK(OnOpenFolder), p);
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6); gtk_box_append(GTK_BOX(box), row);
    text = Button(row, "Inspect controls", "recording.inspect-controls", G_CALLBACK(OnInspectControls), p);
    gtk_widget_set_tooltip_text(text,
        "List control types, test identifiers and availability without reading their values or invoking actions.");
    p->consent = gtk_check_button_new_with_label("Allow screenshots of visible application content");
    gtk_widget_set_tooltip_text(p->consent, "Screenshots can contain source code, names and account information. Review every image before sharing.");
    (void)umi_gtk4_automation_tag_widget(p->consent, "recording.screenshot-consent");
    gtk_box_append(GTK_BOX(box), p->consent);
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6); gtk_box_append(GTK_BOX(box), row);
    p->target = gtk_drop_down_new(NULL, NULL); gtk_widget_set_hexpand(p->target, TRUE);
    gtk_accessible_update_property(GTK_ACCESSIBLE(p->target), GTK_ACCESSIBLE_PROPERTY_LABEL, "Window to capture", -1);
    (void)umi_gtk4_automation_tag_widget(p->target, "recording.capture-target");
    gtk_box_append(GTK_BOX(row), p->target);
    p->capture = Button(row, "Take screenshot", "recording.screenshot", G_CALLBACK(OnCapture), p);
    gtk_widget_set_sensitive(p->capture, FALSE);
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6); gtk_box_append(GTK_BOX(box), row);
    p->note = gtk_entry_new(); gtk_entry_set_max_length(GTK_ENTRY(p->note), 120);
    gtk_entry_set_placeholder_text(GTK_ENTRY(p->note), "What did you expect, and what happened? No secrets.");
    gtk_accessible_update_property(GTK_ACCESSIBLE(p->note), GTK_ACCESSIBLE_PROPERTY_LABEL, "Problem note", -1);
    gtk_widget_set_hexpand(p->note, TRUE); (void)umi_gtk4_automation_tag_widget(p->note, "recording.note");
    gtk_box_append(GTK_BOX(row), p->note); (void)Button(row, "Add note", "recording.add-note", G_CALLBACK(OnNote), p);
    p->status = gtk_label_new("Recording is off. Use a practice workspace and test data, then choose Start.");
    gtk_label_set_wrap(GTK_LABEL(p->status), TRUE); gtk_label_set_wrap_mode(GTK_LABEL(p->status), PANGO_WRAP_WORD_CHAR);
    gtk_label_set_xalign(GTK_LABEL(p->status), 0.0F); gtk_label_set_selectable(GTK_LABEL(p->status), TRUE);
    (void)umi_gtk4_automation_tag_widget(p->status, "recording.status"); gtk_box_append(GTK_BOX(box), p->status);
    RefreshScopes(p); p->timer = g_timeout_add(300U, Tick, p);
    gtk_window_present(p->panel); return UMI_STATUS_OK;
}
