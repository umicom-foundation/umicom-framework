/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_workspace_content_gtk4.c
 * PURPOSE: Check draft-preserving panel rebuilds without presenting windows.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workstation/workspace_layout_host.h"
#include "umicom/ui/workbench_canvas.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Keep assertions active in release builds and use one owned-object cleanup. */
#define CHECK(expression) do { \
    if (!(expression)) { \
        (void)fprintf(stderr, "Line %d: %s\n", __LINE__, #expression); \
        failed = 1; goto cleanup; \
    } \
} while (0)

typedef struct ContentFixture {
    size_t created;
    size_t released;
    GtkWidget *latest_editor;
} ContentFixture;

/* Count final release rather than unparenting, which is normal during moves. */
static void body_released(gpointer user_data, GObject *object)
{
    ContentFixture *fixture = user_data;
    (void)object;
    ++fixture->released;
}

/* An entry provides observable draft text without opening files or services. */
static GtkWidget *create_body(const UmiUiWorkspaceWindow *window, void *user_data)
{
    ContentFixture *fixture = user_data;
    GtkWidget *entry = gtk_entry_new();
    g_object_set_data_full(G_OBJECT(entry), "test-content-id", g_strdup(window->window_id), g_free);
    g_object_weak_ref(G_OBJECT(entry), body_released, fixture);
    if (strcmp(window->window_id, "editor") == 0) fixture->latest_editor = entry;
    ++fixture->created;
    return entry;
}

/* Find a real live widget using the ID attached at construction. */
static GtkWidget *find_data(GtkWidget *widget, const char *key, const char *id)
{
    GtkWidget *child;
    const char *value;
    if (widget == NULL) return NULL;
    value = g_object_get_data(G_OBJECT(widget), key);
    if (value != NULL && strcmp(value, id) == 0) return widget;
    for (child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = find_data(child, key, id);
        if (found != NULL) return found;
    }
    return NULL;
}

/* Both entries initially share one notebook; later tests move one to canvas. */
static UmiStatus add_window(UmiUiWorkspaceLayout *layout, const char *id)
{
    UmiUiWorkspaceWindow window = {0};
    UmiStatus status;
    bool was_locked = layout->locked;
    if (g_strlcpy(window.window_id, id, sizeof(window.window_id)) >= sizeof(window.window_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)g_strlcpy(window.title, id, sizeof(window.title));
    (void)g_strlcpy(window.tool_id, "test.entry", sizeof(window.tool_id));
    (void)g_strlcpy(window.placement_id, "centre", sizeof(window.placement_id));
    (void)g_strlcpy(window.stack_id, "documents", sizeof(window.stack_id));
    window.x = 0.1; window.y = 0.1; window.width = 0.5; window.height = 0.6;
    window.visible = true; window.closable = true; window.resizable = true;
    /* New layouts start locked. Fixture construction temporarily unlocks
     * them, then restores their original policy before the host sees them. */
    status = umi_ui_workspace_layout_set_locked(layout, false);
    if (status == UMI_STATUS_OK) status = umi_ui_workspace_layout_add_window(layout, &window);
    (void)umi_ui_workspace_layout_set_locked(layout, was_locked);
    return status;
}

/* Exercise the actual host with heap models. No GTK window is created or
 * presented, and the provider cannot start external tools. */
int main(void)
{
    ContentFixture fixture = {0};
    UmiUiWorkspaceLayout *layout = NULL;
    UmiUiWorkspaceLayout *blank = NULL;
    UmiGtk4WorkspaceLayoutHost *host = NULL;
    UmiGtk4WorkspaceLayoutHostSnapshot snapshot;
    GtkWidget *root = NULL;
    GtkWidget *old_frame = NULL;
    GtkWidget *editor = NULL;
    GtkWidget *frame;
    GtkWidget *notebook;
    UmiUiWorkspaceWindow second_window;
    UmiUiWindowGroupStore *groups = NULL;
    size_t before_created;
    size_t before_released;
    int failed = 0;

    if (!gtk_init_check()) return 77;
    layout = calloc(1U, sizeof(*layout));
    blank = calloc(1U, sizeof(*blank));
    groups = calloc(1U, sizeof(*groups));
    CHECK(layout != NULL && blank != NULL && groups != NULL);
    CHECK(umi_ui_workspace_layout_init(blank, "test.blank", "Blank") == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_layout_init(layout, "test.work", "Work") == UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_create(blank, create_body, &fixture, &host) == UMI_STATUS_OK);
    root = g_object_ref(umi_gtk4_workspace_layout_host_widget(host));
    CHECK(umi_gtk4_workspace_layout_host_invalidate_content(host, NULL) == UMI_STATUS_INVALID_STATE);
    CHECK(umi_gtk4_workspace_layout_host_set_content_retention(host, 1) == UMI_STATUS_OK);
    CHECK(add_window(layout, "editor") == UMI_STATUS_OK);
    CHECK(add_window(layout, "outline") == UMI_STATUS_OK);
    (void)g_strlcpy(layout->windows[0].context_group_id, "test.context.development",
        sizeof(layout->windows[0].context_group_id));
    CHECK(umi_gtk4_workspace_layout_host_rebuild(host, layout) == UMI_STATUS_OK);
    CHECK(fixture.created == 2U && fixture.released == 0U);
    /* A repeated instance ID must be rejected before a cached body could be
     * mounted twice, even when one of the duplicate records is hidden. */
    second_window = layout->windows[1];
    layout->windows[1] = layout->windows[0];
    layout->windows[1].visible = false;
    CHECK(umi_gtk4_workspace_layout_host_rebuild(host, layout) != UMI_STATUS_OK);
    CHECK(fixture.created == 2U && fixture.released == 0U);
    layout->windows[1] = second_window;
    CHECK(umi_gtk4_workspace_layout_host_set_content_retention(host, 0) == UMI_STATUS_BUSY);
    editor = fixture.latest_editor;
    CHECK(editor != NULL);
    gtk_editable_set_text(GTK_EDITABLE(editor), "unsaved draft survives the move");
    old_frame = find_data(root, "umicom-workspace-window-id", "editor");
    CHECK(old_frame != NULL);
    g_object_ref(old_frame);
    /* Group IDs and palette tokens are different values. Binding and colour
     * updates must not replace the provider or accept arbitrary CSS classes. */
    CHECK(umi_ui_window_group_define(groups, "test.context.development", "blue",
        UMI_UI_WINDOW_CONTEXT_PROJECT) == UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_set_context_groups(host, groups) == UMI_STATUS_OK);
    CHECK(gtk_widget_has_css_class(old_frame, "umicom-context-blue"));
    (void)g_strlcpy(groups->items[0].colour_token, "green", sizeof(groups->items[0].colour_token));
    CHECK(umi_gtk4_workspace_layout_host_set_context_groups(host, groups) == UMI_STATUS_OK);
    CHECK(gtk_widget_has_css_class(old_frame, "umicom-context-green"));
    CHECK(!gtk_widget_has_css_class(old_frame, "umicom-context-blue"));
    (void)g_strlcpy(groups->items[0].colour_token, "untrusted-style", sizeof(groups->items[0].colour_token));
    CHECK(umi_gtk4_workspace_layout_host_set_context_groups(host, groups) == UMI_STATUS_OK);
    CHECK(!gtk_widget_has_css_class(old_frame, "umicom-context-green"));
    CHECK(!gtk_widget_has_css_class(old_frame, "untrusted-style"));
    CHECK(!gtk_widget_has_css_class(old_frame, "umicom-context-untrusted-style"));
    (void)g_strlcpy(groups->items[0].colour_token, "green", sizeof(groups->items[0].colour_token));
    CHECK(umi_gtk4_workspace_layout_host_set_context_groups(host, groups) == UMI_STATUS_OK);
    CHECK(fixture.created == 2U);
    CHECK(umi_gtk4_workspace_layout_host_focus_window(host, "outline") == UMI_STATUS_OK);
    frame = find_data(root, "umicom-workspace-window-id", "outline");
    notebook = gtk_widget_get_ancestor(frame, GTK_TYPE_NOTEBOOK);
    CHECK(notebook != NULL);
    CHECK(gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook),
        gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook))) == frame);

    /* Unlocking, reparenting, and renaming reconstruct chrome, not the entry. */
    layout->locked = false;
    (void)g_strlcpy(layout->windows[0].placement_id, UMI_UI_WORKSPACE_CANVAS_PLACEMENT,
        sizeof(layout->windows[0].placement_id));
    ++layout->revision;
    CHECK(umi_gtk4_workspace_layout_host_rebuild(host, layout) == UMI_STATUS_OK);
    CHECK(fixture.created == 2U && fixture.released == 0U);
    CHECK(find_data(root, "test-content-id", "editor") == editor);
    CHECK(find_data(old_frame, "test-content-id", "editor") == NULL);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(editor)), "unsaved draft survives the move") == 0);
    CHECK(umi_gtk4_workspace_layout_host_focus_window(host, "editor") == UMI_STATUS_OK);
    layout->windows[0].visible = false;
    CHECK(umi_gtk4_workspace_layout_host_rebuild(host, layout) == UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_focus_window(host, "editor") == UMI_STATUS_NOT_FOUND);
    CHECK(gtk_widget_get_parent(editor) == NULL);
    CHECK(umi_gtk4_workspace_layout_host_rebuild(host, blank) == UMI_STATUS_OK);
    snapshot = umi_gtk4_workspace_layout_host_snapshot(host);
    CHECK(snapshot.retained_content_count == 2U && snapshot.panel_count == 0U);
    layout->windows[0].visible = true;
    (void)g_strlcpy(layout->windows[0].title, "Renamed editor", sizeof(layout->windows[0].title));
    CHECK(umi_gtk4_workspace_layout_host_rebuild(host, layout) == UMI_STATUS_OK);
    CHECK(fixture.latest_editor == editor && fixture.created == 2U);

    /* A failed replacement must not consume pending refresh or destroy draft
     * state. Refresh becomes visible only with the next valid replacement. */
    CHECK(umi_gtk4_workspace_layout_host_invalidate_content(host, "missing") == UMI_STATUS_NOT_FOUND);
    CHECK(umi_gtk4_workspace_layout_host_invalidate_content(host, "editor") == UMI_STATUS_OK);
    layout->windows[0].width = 0.0;
    CHECK(umi_gtk4_workspace_layout_host_rebuild(host, layout) != UMI_STATUS_OK);
    CHECK(fixture.created == 2U && fixture.released == 0U);
    layout->windows[0].width = 0.5;
    CHECK(umi_gtk4_workspace_layout_host_rebuild(host, layout) == UMI_STATUS_OK);
    CHECK(fixture.created == 3U && fixture.released == 1U);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(fixture.latest_editor)), "") == 0);
    (void)g_strlcpy(layout->windows[0].tool_id, "test.other", sizeof(layout->windows[0].tool_id));
    CHECK(umi_gtk4_workspace_layout_host_rebuild(host, layout) == UMI_STATUS_OK);
    CHECK(fixture.created == 4U && fixture.released == 2U);

    /* Bound hidden-body retention. A full cache reports capacity instead of
     * silently evicting a view that could still contain unsaved work. */
    for (size_t index = 2U; index < UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS; ++index) {
        char id[32];
        int written = g_snprintf(id, sizeof(id), "retained-%zu", index);
        CHECK(written >= 0 && (size_t)written < sizeof(id));
        CHECK(umi_ui_workspace_layout_init(layout, "test.rotating", "Rotating") == UMI_STATUS_OK);
        CHECK(add_window(layout, id) == UMI_STATUS_OK);
        CHECK(umi_gtk4_workspace_layout_host_rebuild(host, layout) == UMI_STATUS_OK);
    }
    snapshot = umi_gtk4_workspace_layout_host_snapshot(host);
    CHECK(snapshot.retained_content_count == UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS);
    before_created = fixture.created;
    before_released = fixture.released;
    CHECK(umi_ui_workspace_layout_init(layout, "test.overflow", "Overflow") == UMI_STATUS_OK);
    CHECK(add_window(layout, "one-too-many") == UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_rebuild(host, layout) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(fixture.created == before_created && fixture.released == before_released);
    snapshot = umi_gtk4_workspace_layout_host_snapshot(host);
    CHECK(strcmp(snapshot.layout_id, "test.rotating") == 0);
    CHECK(umi_gtk4_workspace_layout_host_invalidate_content(host, NULL) == UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_rebuild(host, blank) == UMI_STATUS_OK);
    CHECK(fixture.created == fixture.released);
    CHECK(umi_gtk4_workspace_layout_host_set_content_retention(host, 0) == UMI_STATUS_OK);

    /* Default hosts still refresh the provider on an explicit rebuild. */
    CHECK(umi_gtk4_workspace_layout_host_rebuild(host, layout) == UMI_STATUS_OK);
    before_created = fixture.created;
    CHECK(umi_gtk4_workspace_layout_host_rebuild(host, layout) == UMI_STATUS_OK);
    CHECK(fixture.created == before_created + 1U);
    /* Default Suite-style hosts also keep the exact provider draft when only
     * the active layout name/revision changes. This does not enable retention. */
    editor = find_data(root, "test-content-id", "one-too-many");
    CHECK(GTK_IS_ENTRY(editor));
    gtk_editable_set_text(GTK_EDITABLE(editor), "draft survives a library rename");
    CHECK(umi_gtk4_workspace_layout_host_maximise_window(host, "one-too-many") == UMI_STATUS_OK);
    before_created = fixture.created;
    before_released = fixture.released;
    *blank = *layout;
    (void)g_strlcpy(blank->name, "Renamed library layout", sizeof(blank->name));
    ++blank->revision;
    /* Equal text with different unused bytes must not be compared as a raw
     * public struct; independent callers need not initialise trailing storage. */
    blank->windows[0].title[strlen(blank->windows[0].title) + 1U] = 'X';
    CHECK(umi_gtk4_workspace_layout_host_update_metadata(host, blank) == UMI_STATUS_OK);
    snapshot = umi_gtk4_workspace_layout_host_snapshot(host);
    CHECK(snapshot.source_layout_revision == blank->revision && !snapshot.content_retention_enabled);
    CHECK(snapshot.maximised && strcmp(snapshot.maximised_window_id, "one-too-many") == 0);
    CHECK(find_data(root, "test-content-id", "one-too-many") == editor);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(editor)), "draft survives a library rename") == 0);
    CHECK(fixture.created == before_created && fixture.released == before_released);
    CHECK(umi_gtk4_workspace_layout_host_update_metadata(host, blank) == UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_snapshot(host).revision == snapshot.revision);
    ++blank->windows[0].z_order;
    CHECK(umi_gtk4_workspace_layout_host_update_metadata(host, blank) == UMI_STATUS_INVALID_STATE);
    --blank->windows[0].z_order;
    blank->windows[0].x += 0.01;
    CHECK(umi_gtk4_workspace_layout_host_update_metadata(host, blank) == UMI_STATUS_INVALID_STATE);
    blank->windows[0].x = layout->windows[0].x;
    blank->locked = !blank->locked;
    CHECK(umi_gtk4_workspace_layout_host_update_metadata(host, blank) == UMI_STATUS_INVALID_STATE);
    blank->locked = layout->locked;
    --blank->revision;
    CHECK(umi_gtk4_workspace_layout_host_update_metadata(host, blank) == UMI_STATUS_INVALID_STATE);
    ++blank->revision;
    (void)g_strlcpy(blank->name, "Another name without a new revision", sizeof(blank->name));
    CHECK(umi_gtk4_workspace_layout_host_update_metadata(host, blank) == UMI_STATUS_INVALID_STATE);
    (void)memset(blank->name, 'X', sizeof(blank->name));
    CHECK(umi_gtk4_workspace_layout_host_update_metadata(host, blank) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_gtk4_workspace_layout_host_snapshot(host).revision == snapshot.revision);
    CHECK(fixture.created == before_created && fixture.released == before_released);
    CHECK(find_data(root, "test-content-id", "one-too-many") == editor);
    CHECK(umi_gtk4_workspace_layout_host_focus_window(host, "missing") == UMI_STATUS_NOT_FOUND);
    CHECK(umi_gtk4_workspace_layout_host_restore_maximised(host) == UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_focus_window(NULL, "editor") == UMI_STATUS_INVALID_ARGUMENT);

cleanup:
    umi_gtk4_workspace_layout_host_destroy(host);
    if (old_frame != NULL) g_object_unref(old_frame);
    if (root != NULL) g_object_unref(root);
    if (!failed && fixture.created != fixture.released) {
        (void)fprintf(stderr, "Provider body reference leak\n");
        failed = 1;
    }
    free(layout);
    free(blank);
    free(groups);
    return failed;
}
