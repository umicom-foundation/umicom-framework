/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_workspace_maximise_gtk4.c
 *
 * PURPOSE:
 *   Verify reversible internal panel presentation and callback ownership using
 *   real GTK widgets without presenting windows or starting application tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workstation/workspace_layout_host.h"
#include "umicom/ui/workbench_canvas.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    (void)fprintf(stderr, "Line %d: %s\n", __LINE__, #expression); \
    failed = 1; goto cleanup; } } while (0)

typedef struct MaximiseFixture {
    UmiGtk4WorkspaceLayoutHost *host;
    size_t created;
    size_t released;
    size_t actions;
    size_t geometry_requests;
    int callback_was_restored;
    int destroy_on_action;
} MaximiseFixture;

/* Weak notification distinguishes final release from harmless reparenting. */
static void body_released(gpointer user_data, GObject *body)
{
    MaximiseFixture *fixture = user_data;
    (void)body;
    ++fixture->released;
}

/* Entries expose a real mutable draft that must survive every presentation. */
static GtkWidget *create_body(const UmiUiWorkspaceWindow *window, void *user_data)
{
    MaximiseFixture *fixture = user_data;
    GtkWidget *entry = gtk_entry_new();
    g_object_set_data_full(G_OBJECT(entry), "test-body-id", g_strdup(window->window_id), g_free);
    g_object_weak_ref(G_OBJECT(entry), body_released, fixture);
    ++fixture->created;
    return entry;
}

/* An owner may destroy its host; it must receive the restored normal tree. */
static void on_action(const char *window_id, UmiWsPanelAction action, void *user_data)
{
    MaximiseFixture *fixture = user_data;
    (void)window_id;
    (void)action;
    ++fixture->actions;
    fixture->callback_was_restored = !umi_gtk4_workspace_layout_host_snapshot(fixture->host).maximised;
    if (fixture->destroy_on_action) {
        umi_gtk4_workspace_layout_host_destroy(fixture->host);
        fixture->host = NULL;
    }
}

/* Count requests without changing the fixture's canonical model. */
static UmiStatus on_geometry(const char *window_id,
    const UmiApplicationSuiteLayoutRect *rect, uint64_t revision, void *user_data)
{
    MaximiseFixture *fixture = user_data;
    (void)window_id; (void)rect; (void)revision;
    ++fixture->geometry_requests;
    return UMI_STATUS_OK;
}

/* Search actual widgets, including unpresented notebook and menu descendants. */
static GtkWidget *find_data(GtkWidget *widget, const char *key, const char *id)
{
    const char *value;
    if (widget == NULL) return NULL;
    value = g_object_get_data(G_OBJECT(widget), key);
    if (value != NULL && strcmp(value, id) == 0) return widget;
    for (GtkWidget *child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = find_data(child, key, id);
        if (found != NULL) return found;
    }
    return NULL;
}

/* Keep one owned controller reference for synthetic signals, never OS input. */
static GObject *find_keys(GtkWidget *widget)
{
    GListModel *controllers = gtk_widget_observe_controllers(widget);
    GObject *found = NULL;
    for (guint index = 0U; index < g_list_model_get_n_items(controllers); ++index) {
        GObject *candidate = g_list_model_get_item(controllers, index);
        if (GTK_IS_EVENT_CONTROLLER_KEY(candidate)) { found = candidate; break; }
        g_object_unref(candidate);
    }
    g_object_unref(controllers);
    return found;
}

/* Emit an isolated controller signal and observe whether its callback handles it. */
static gboolean emit_key(GObject *keys, guint key, GdkModifierType modifiers)
{
    gboolean handled = FALSE;
    g_signal_emit_by_name(keys, "key-pressed", key, 0U, modifiers, &handled);
    return handled;
}

/* Process only ready sources with a fixed bound, avoiding an application loop. */
static void drain_ready(void)
{
    for (size_t index = 0U; index < 128U && g_main_context_pending(NULL); ++index)
        (void)g_main_context_iteration(NULL, FALSE);
}

/* Populate canonical records while temporarily unlocked during construction. */
static UmiStatus add_window(UmiUiWorkspaceLayout *layout, const char *id,
    const char *placement, bool visible, bool pinned, bool resizable)
{
    UmiUiWorkspaceWindow window = {0};
    (void)g_strlcpy(window.window_id, id, sizeof(window.window_id));
    (void)g_strlcpy(window.tool_id, id, sizeof(window.tool_id));
    (void)g_strlcpy(window.title, id, sizeof(window.title));
    (void)g_strlcpy(window.placement_id, placement, sizeof(window.placement_id));
    (void)g_strlcpy(window.stack_id,
        strcmp(placement, "centre") == 0 ? "documents" : id, sizeof(window.stack_id));
    window.x = 0.2; window.y = 0.2; window.width = 0.4; window.height = 0.4;
    window.visible = visible; window.pinned = pinned; window.resizable = resizable;
    window.closable = true;
    return umi_ui_workspace_layout_add_window(layout, &window);
}

/* Check dock/canvas retention, protections, keyboard fallback, rebuild and
 * callback teardown without any native top-level window or external command. */
int main(void)
{
    MaximiseFixture fixture = {0};
    UmiUiWorkspaceLayout *layout = NULL;
    UmiUiWorkspaceLayout *saved = NULL;
    UmiUiWorkspaceLayout *blank = NULL;
    UmiGtk4WorkspaceLayoutHostOptions options = {1};
    UmiGtk4WorkspaceLayoutHostSnapshot initial;
    UmiGtk4WorkspaceLayoutHostSnapshot snapshot;
    UmiApplicationSuiteLayoutRect rect = {0.25, 0.2, 0.4, 0.4};
    GtkWidget *root = NULL;
    GtkWidget *normal;
    GtkWidget *frame;
    GtkWidget *frame_child;
    GtkWidget *notebook;
    GtkWidget *tab;
    GtkWidget *body;
    GtkWidget *other_body;
    GtkWidget *button;
    GtkWidget *held_restore = NULL;
    GtkWidget *held_action = NULL;
    GObject *keys = NULL;
    GObject *drag_keys = NULL;
    int page;
    int selection_start;
    int selection_end;
    int failed = 0;
    size_t created;

    /* Use GTK's test accessibility backend, as in adjacent native fixtures. */
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    if (!gtk_init_check()) return 77;
    layout = calloc(1U, sizeof(*layout)); saved = calloc(1U, sizeof(*saved));
    blank = calloc(1U, sizeof(*blank));
    CHECK(layout != NULL && saved != NULL && blank != NULL);
    CHECK(umi_ui_workspace_layout_init(blank, "test.blank", "Blank") == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_layout_init(layout, "test.work", "Work") == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_layout_set_locked(layout, false) == UMI_STATUS_OK);
    CHECK(add_window(layout, "editor", "centre", true, false, true) == UMI_STATUS_OK);
    CHECK(add_window(layout, "notes", "centre", true, false, true) == UMI_STATUS_OK);
    CHECK(add_window(layout, "chart", "canvas", true, false, true) == UMI_STATUS_OK);
    CHECK(add_window(layout, "protected", "right", true, true, true) == UMI_STATUS_OK);
    CHECK(add_window(layout, "fixed", "left", true, false, false) == UMI_STATUS_OK);
    CHECK(add_window(layout, "hidden", "bottom", false, false, true) == UMI_STATUS_OK);
    CHECK(add_window(layout, "rail", "auto-hide:left", false, false, true) == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_layout_set_locked(layout, true) == UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_create_with_options(blank, create_body, &fixture,
        on_action, &fixture, &options, &fixture.host) == UMI_STATUS_OK);
    root = g_object_ref(umi_gtk4_workspace_layout_host_widget(fixture.host));
    CHECK(umi_gtk4_workspace_layout_host_set_content_retention(fixture.host, 1) == UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_rebuild(fixture.host, layout) == UMI_STATUS_OK);
    *saved = *layout;
    initial = umi_gtk4_workspace_layout_host_snapshot(fixture.host);
    normal = gtk_widget_get_first_child(root);
    frame = find_data(root, "umicom-workspace-window-id", "editor");
    body = find_data(root, "test-body-id", "editor");
    other_body = find_data(root, "test-body-id", "notes");
    CHECK(frame != NULL && body != NULL && other_body != NULL);
    frame_child = gtk_frame_get_child(GTK_FRAME(frame));
    gtk_widget_add_css_class(frame, "test-preserved-context-class");
    notebook = gtk_widget_get_ancestor(frame, GTK_TYPE_NOTEBOOK);
    CHECK(notebook != NULL);
    page = gtk_notebook_page_num(GTK_NOTEBOOK(notebook), frame);
    tab = gtk_notebook_get_tab_label(GTK_NOTEBOOK(notebook), frame);
    gtk_editable_set_text(GTK_EDITABLE(body), "Draft: \xCE\xBB \xF0\x9F\x9A\x80");
    gtk_editable_select_region(GTK_EDITABLE(body), 2, 6);
    created = fixture.created;
    CHECK(umi_gtk4_workspace_layout_host_maximise_window(NULL, "editor") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_gtk4_workspace_layout_host_restore_maximised(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_gtk4_workspace_layout_host_maximise_window(fixture.host, "") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_gtk4_workspace_layout_host_maximise_window(fixture.host, "hidden") == UMI_STATUS_NOT_FOUND);
    CHECK(umi_gtk4_workspace_layout_host_maximise_window(fixture.host, "rail") == UMI_STATUS_NOT_FOUND);
    CHECK(umi_gtk4_workspace_layout_host_maximise_window(fixture.host, "missing") == UMI_STATUS_NOT_FOUND);
    CHECK(umi_gtk4_workspace_layout_host_maximise_window(fixture.host, "protected") == UMI_STATUS_PERMISSION_DENIED);
    CHECK(umi_gtk4_workspace_layout_host_maximise_window(fixture.host, "fixed") == UMI_STATUS_PERMISSION_DENIED);
    button = find_data(root, "umicom-automation-id", "editor.action.maximise");
    CHECK(button != NULL && gtk_widget_get_sensitive(button));
    g_signal_emit_by_name(button, "clicked"); drain_ready();
    snapshot = umi_gtk4_workspace_layout_host_snapshot(fixture.host);
    CHECK(snapshot.maximised && strcmp(snapshot.maximised_window_id, "editor") == 0);
    CHECK(snapshot.revision == initial.revision && snapshot.source_layout_revision == initial.source_layout_revision);
    CHECK(fixture.actions == 0U && fixture.created == created && fixture.released == 0U);
    CHECK(!gtk_widget_get_visible(normal) && gtk_frame_get_child(GTK_FRAME(frame)) == NULL);
    CHECK(gtk_widget_has_css_class(gtk_widget_get_last_child(gtk_widget_get_last_child(root)),
        "test-preserved-context-class"));
    CHECK(find_data(root, "test-body-id", "editor") == body && find_data(root, "test-body-id", "notes") == other_body);
    CHECK(gtk_notebook_page_num(GTK_NOTEBOOK(notebook), frame) == page &&
        gtk_notebook_get_tab_label(GTK_NOTEBOOK(notebook), frame) == tab);
    CHECK(umi_gtk4_workspace_layout_host_maximise_window(fixture.host, "editor") == UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_focus_window(fixture.host, "editor") == UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_snapshot(fixture.host).maximised);
    /* Invalid replacements leave both transient presentation and model intact. */
    layout->windows[2].width = 2.0;
    CHECK(umi_gtk4_workspace_layout_host_rebuild(fixture.host, layout) != UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_snapshot(fixture.host).maximised);
    *layout = *saved;
    CHECK(umi_gtk4_workspace_layout_host_focus_window(fixture.host, "notes") == UMI_STATUS_OK);
    CHECK(!umi_gtk4_workspace_layout_host_snapshot(fixture.host).maximised);
    CHECK(gtk_widget_get_visible(normal) && gtk_frame_get_child(GTK_FRAME(frame)) == frame_child);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(body)), "Draft: \xCE\xBB \xF0\x9F\x9A\x80") == 0);
    CHECK(gtk_editable_get_selection_bounds(GTK_EDITABLE(body), &selection_start, &selection_end));
    CHECK(selection_start == 2 && selection_end == 6);
    CHECK(memcmp(layout, saved, sizeof(*layout)) == 0);
    /* Switching directly validates the new target before restoring the old. */
    CHECK(umi_gtk4_workspace_layout_host_maximise_window(fixture.host, "editor") == UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_maximise_window(fixture.host, "protected") == UMI_STATUS_PERMISSION_DENIED);
    CHECK(strcmp(umi_gtk4_workspace_layout_host_snapshot(fixture.host).maximised_window_id, "editor") == 0);
    CHECK(umi_gtk4_workspace_layout_host_maximise_window(fixture.host, "chart") == UMI_STATUS_OK);
    CHECK(strcmp(umi_gtk4_workspace_layout_host_snapshot(fixture.host).maximised_window_id, "chart") == 0);
    CHECK(gtk_frame_get_child(GTK_FRAME(frame)) == frame_child && fixture.created == created);
    /* Inspect actual allocated full-host bounds, not just a maximised flag. */
    gtk_widget_allocate(root, 1200, 900, -1, NULL);
    CHECK(gtk_widget_get_width(gtk_widget_get_last_child(gtk_widget_get_last_child(root))) >= 1100);
    CHECK(gtk_widget_get_height(gtk_widget_get_last_child(gtk_widget_get_last_child(root))) >= 700);
    /* Explicit rail navigation leaves maximisation and preserves the body. */
    CHECK(umi_gtk4_workspace_layout_host_maximise_window(fixture.host, "chart") == UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_reveal_tool_window(fixture.host, "rail") == UMI_STATUS_OK);
    CHECK(!umi_gtk4_workspace_layout_host_snapshot(fixture.host).maximised);
    created = fixture.created;
    CHECK(umi_gtk4_workspace_layout_host_maximise_window(fixture.host, "chart") == UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_snapshot(fixture.host).revealed_tool_count == 0U);
    keys = find_keys(gtk_widget_get_last_child(root));
    CHECK(keys != NULL && gtk_event_controller_get_propagation_phase(GTK_EVENT_CONTROLLER(keys)) == GTK_PHASE_BUBBLE);
    CHECK(!emit_key(keys, GDK_KEY_Escape, GDK_CONTROL_MASK));
    CHECK(!emit_key(keys, GDK_KEY_a, 0));
    CHECK(emit_key(keys, GDK_KEY_Escape, 0));
    CHECK(umi_gtk4_workspace_layout_host_snapshot(fixture.host).maximised);
    drain_ready();
    CHECK(!umi_gtk4_workspace_layout_host_snapshot(fixture.host).maximised);
    CHECK(!emit_key(keys, GDK_KEY_Escape, 0));
    g_clear_object(&keys);
    CHECK(fixture.created == created && fixture.released == 0U);
    /* A retained Restore button is disconnected once its presentation ends. */
    CHECK(umi_gtk4_workspace_layout_host_maximise_window(fixture.host, "editor") == UMI_STATUS_OK);
    button = find_data(root, "umicom-automation-id", "workstation.workspace.restore-maximised");
    CHECK(button != NULL);
    held_restore = g_object_ref(button);
    g_signal_emit_by_name(held_restore, "clicked");
    CHECK(umi_gtk4_workspace_layout_host_rebuild(fixture.host, layout) == UMI_STATUS_OK);
    drain_ready();
    CHECK(!umi_gtk4_workspace_layout_host_snapshot(fixture.host).maximised);
    CHECK(umi_gtk4_workspace_layout_host_maximise_window(fixture.host, "chart") == UMI_STATUS_OK);
    g_signal_emit_by_name(held_restore, "clicked"); drain_ready();
    CHECK(umi_gtk4_workspace_layout_host_snapshot(fixture.host).maximised);
    CHECK(umi_gtk4_workspace_layout_host_restore_maximised(fixture.host) == UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_restore_maximised(fixture.host) == UMI_STATUS_OK);
    g_clear_object(&held_restore);
    /* Geometry operations and internal maximisation cannot race. */
    CHECK(umi_ui_workspace_layout_set_locked(layout, false) == UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_rebuild(fixture.host, layout) == UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_set_canvas_geometry_handler(fixture.host, on_geometry, &fixture) == UMI_STATUS_OK);
    gtk_widget_allocate(root, 1200, 900, -1, NULL);
    button = find_data(root, "umicom-automation-id", "workstation.canvas.drag.chart");
    CHECK(button != NULL);
    drag_keys = find_keys(button);
    CHECK(drag_keys != NULL);
    CHECK(emit_key(drag_keys, GDK_KEY_Right, 0));
    CHECK(umi_gtk4_workspace_layout_host_maximise_window(fixture.host, "chart") == UMI_STATUS_BUSY);
    CHECK(emit_key(drag_keys, GDK_KEY_Escape, 0));
    CHECK(umi_gtk4_workspace_layout_host_maximise_window(fixture.host, "chart") == UMI_STATUS_OK);
    CHECK(!emit_key(drag_keys, GDK_KEY_Right, 0));
    CHECK(!gtk_widget_get_sensitive(button));
    CHECK(umi_gtk4_workspace_layout_host_request_canvas_geometry(fixture.host, "chart", &rect, layout->revision) == UMI_STATUS_BUSY);
    CHECK(umi_gtk4_workspace_layout_host_restore_maximised(fixture.host) == UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_request_canvas_geometry(fixture.host, "chart", &rect, layout->revision) == UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_maximise_window(fixture.host, "chart") == UMI_STATUS_BUSY);
    CHECK(umi_gtk4_workspace_layout_host_set_canvas_geometry_handler(fixture.host, NULL, NULL) == UMI_STATUS_OK);
    g_clear_object(&drag_keys);
    drain_ready();
    CHECK(fixture.geometry_requests == 0U);
    /* A real Close callback sees normal parentage and can destroy the host. */
    CHECK(umi_gtk4_workspace_layout_host_maximise_window(fixture.host, "editor") == UMI_STATUS_OK);
    button = find_data(root, "umicom-automation-id", "workstation.workspace.restore-maximised");
    CHECK(button != NULL);
    held_restore = g_object_ref(button);
    button = find_data(root, "umicom-automation-id", "editor.action.close");
    CHECK(button != NULL);
    held_action = g_object_ref(button);
    fixture.destroy_on_action = 1;
    g_signal_emit_by_name(held_action, "clicked"); drain_ready();
    CHECK(fixture.host == NULL && fixture.actions == 1U && fixture.callback_was_restored);
    CHECK(gtk_widget_get_first_child(root) == NULL);
    g_signal_emit_by_name(held_restore, "clicked");
    g_signal_emit_by_name(held_action, "clicked"); drain_ready();
    CHECK(fixture.actions == 1U);
    g_clear_object(&held_action);
    g_clear_object(&held_restore);
    g_clear_object(&root);
    /* A read-only host still owns transient presentation; no application
     * action callback is required and unsupported model controls stay absent. */
    CHECK(umi_gtk4_workspace_layout_host_create(layout, create_body, &fixture, &fixture.host) == UMI_STATUS_OK);
    root = g_object_ref(umi_gtk4_workspace_layout_host_widget(fixture.host));
    button = find_data(root, "umicom-automation-id", "editor.action.maximise");
    CHECK(button != NULL && gtk_widget_get_sensitive(button));
    CHECK(find_data(root, "umicom-automation-id", "editor.action.close") == NULL);
    created = fixture.created;
    g_signal_emit_by_name(button, "clicked"); drain_ready();
    CHECK(umi_gtk4_workspace_layout_host_snapshot(fixture.host).maximised && fixture.created == created);
    button = find_data(root, "umicom-automation-id", "workstation.workspace.restore-maximised");
    CHECK(button != NULL);
    held_restore = g_object_ref(button);
    /* Destruction cancels a queued restore even if its widget is retained. */
    g_signal_emit_by_name(button, "clicked");
    umi_gtk4_workspace_layout_host_destroy(fixture.host);
    fixture.host = NULL;
    drain_ready();
    g_signal_emit_by_name(held_restore, "clicked"); drain_ready();
    CHECK(gtk_widget_get_first_child(root) == NULL && fixture.actions == 1U);

cleanup:
    umi_gtk4_workspace_layout_host_destroy(fixture.host);
    if (keys != NULL) g_object_unref(keys);
    if (drag_keys != NULL) g_object_unref(drag_keys);
    if (held_restore != NULL) g_object_unref(held_restore);
    if (held_action != NULL) g_object_unref(held_action);
    if (root != NULL) g_object_unref(root);
    if (!failed && fixture.created != fixture.released) {
        (void)fprintf(stderr, "Provider lifetime mismatch\n"); failed = 1;
    }
    free(layout); free(saved); free(blank);
    return failed;
}
