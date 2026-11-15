/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_workspace_focus_ownership_gtk4.c
 *
 * PURPOSE:
 *   Exercise production workspace focus through synchronous page, focus and native window lifetime observers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/workspace_layout_host.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); failed = 1; goto done; } } while (0)

typedef struct Fixture {
    UmiGtk4WorkspaceLayoutHost *host;
    UmiUiWorkspaceLayout *layout;
    GtkWidget *entries[2], *window, *outside, *frame, *notebook, *foreign;
    const char *mode;
    unsigned calls, created;
    UmiStatus nested;
} Fixture;

static GtkWidget *Body(const UmiUiWorkspaceWindow *model, void *data)
{
    Fixture *fixture = data;
    unsigned index = strcmp(model->window_id, "editor") == 0 ? 1U : 0U;
    GtkWidget *entry = g_object_ref_sink(gtk_entry_new());
    fixture->entries[index] = entry;
    ++fixture->created;
    gtk_editable_set_text(GTK_EDITABLE(entry), "Draft before panel selection");
    return entry;
}
static GtkWidget *Frame(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-workspace-window-id");
    if (tag != NULL && strcmp(tag, id) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Frame(child, id);
        if (found != NULL) return found;
    }
    return NULL;
}
static gboolean Within(GtkWidget *widget, GtkWidget *parent)
{
    return widget != NULL && (widget == parent || gtk_widget_is_ancestor(widget, parent));
}
static void Settle(void)
{
    gint64 end = g_get_monotonic_time() + G_TIME_SPAN_SECOND / 4;
    while (g_get_monotonic_time() < end) {
        for (unsigned i = 0U; i < 32U && g_main_context_pending(NULL); ++i)
            (void)g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
}

/* Observers call public host operations or change actual GTK parentage. They
 * never reach into the host structure or substitute its focus implementation. */
static void Observe(GObject *object, GParamSpec *property, gpointer data)
{
    (void)property;
    Fixture *fixture = data;
    const char *mode = fixture->mode;
    if (fixture->calls != 0U) return;
    if (strncmp(mode, "focus-", 6U) == 0 &&
        !Within(gtk_window_get_focus(GTK_WINDOW(object)), fixture->frame)) return;
    if (strncmp(mode, "floating-", 9U) == 0 && !gtk_widget_get_visible(GTK_WIDGET(object))) return;
    ++fixture->calls;
    if (strstr(mode, "destroy-host") != NULL) {
        UmiGtk4WorkspaceLayoutHost *retired = fixture->host;
        fixture->host = NULL;
        umi_gtk4_workspace_layout_host_destroy(retired);
    } else if (strstr(mode, "close-owner") != NULL)
        gtk_window_destroy(GTK_WINDOW(object == G_OBJECT(fixture->notebook) ? fixture->window : GTK_WIDGET(object)));
    else if (strcmp(mode, "page-remove") == 0 || strcmp(mode, "page-reparent") == 0) {
        int page = gtk_notebook_page_num(GTK_NOTEBOOK(fixture->notebook), fixture->frame);
        gtk_notebook_remove_page(GTK_NOTEBOOK(fixture->notebook), page);
        if (strcmp(mode, "page-reparent") == 0)
            gtk_window_set_child(GTK_WINDOW(fixture->foreign), fixture->frame);
    } else if (strstr(mode, "hide") != NULL)
        gtk_widget_set_visible(fixture->frame, FALSE);
    else if (strcmp(mode, "page-redirect") == 0)
        gtk_notebook_set_current_page(GTK_NOTEBOOK(fixture->notebook), 0);
    else if (strcmp(mode, "page-reorder") == 0)
        gtk_notebook_reorder_child(GTK_NOTEBOOK(fixture->notebook), fixture->frame, 0);
    else if (strcmp(mode, "focus-redirect") == 0)
        (void)gtk_widget_grab_focus(fixture->outside);
    else if (strstr(mode, "reenter") != NULL)
        fixture->nested = umi_gtk4_workspace_layout_host_focus_window(fixture->host, "editor");
    else if (strcmp(mode, "page-rebuild") == 0)
        fixture->nested = umi_gtk4_workspace_layout_host_rebuild(fixture->host, fixture->layout);
    else if (strcmp(mode, "page-maximise") == 0)
        fixture->nested = umi_gtk4_workspace_layout_host_maximise_window(fixture->host, "editor");
    else if (strcmp(mode, "page-restore") == 0)
        fixture->nested = umi_gtk4_workspace_layout_host_restore_maximised(fixture->host);
    else if (strcmp(mode, "page-metadata") == 0) {
        g_strlcpy(fixture->layout->name, "Renamed during selection", sizeof fixture->layout->name);
        ++fixture->layout->revision;
        fixture->nested = umi_gtk4_workspace_layout_host_update_metadata(fixture->host, fixture->layout);
    }
}

int main(int argc, char **argv)
{
    static const char *const cases[] = {
        "select", "unmounted", "repeat", "invalid", "missing", "hidden-model",
        "hidden-frame", "disabled-frame", "page-destroy-host", "page-close-owner",
        "page-remove", "page-reparent", "page-hide", "page-redirect", "page-reorder",
        "page-reenter", "page-rebuild", "page-maximise", "page-restore", "page-metadata",
        "focus-destroy-host", "focus-close-owner", "focus-hide", "focus-redirect",
        "focus-reenter", "floating-present", "floating-destroy-host", "floating-close-owner"
    };
    if (argc != 2) return 2;
    int known = 0, failed = 0;
    for (size_t i = 0U; i < G_N_ELEMENTS(cases); ++i)
        if (strcmp(argv[1], cases[i]) == 0) known = 1;
    if (!known) return 2;
    if (!gtk_init_check()) return 77;
    Fixture fixture = {0};
    fixture.mode = argv[1]; fixture.nested = UMI_STATUS_OK;
    fixture.layout = calloc(1U, sizeof(*fixture.layout));
    GtkWidget *root = NULL, *floating = NULL;
    GObject *observed = NULL;
    gulong handler = 0U;
    fixture.window = g_object_ref_sink(gtk_window_new());
    fixture.foreign = g_object_ref_sink(gtk_window_new());
    GtkWidget *outer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    fixture.outside = g_object_ref_sink(gtk_entry_new());
    gtk_window_set_child(GTK_WINDOW(fixture.window), outer);
    gtk_box_append(GTK_BOX(outer), fixture.outside);
    CHECK(fixture.layout != NULL);
    CHECK(umi_ui_workspace_layout_init(fixture.layout, "focus-owner", "Focus ownership") == UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_create(fixture.layout, Body, &fixture, &fixture.host) == UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_set_content_retention(fixture.host, 1) == UMI_STATUS_OK);
    for (unsigned i = 0U; i < 2U; ++i) {
        UmiUiWorkspaceWindow model = {0};
        const char *id = i == 0U ? "files" : "editor";
        g_strlcpy(model.window_id, id, sizeof model.window_id);
        g_strlcpy(model.title, id, sizeof model.title);
        g_strlcpy(model.tool_id, "test.entry", sizeof model.tool_id);
        g_strlcpy(model.placement_id, "centre", sizeof model.placement_id);
        g_strlcpy(model.stack_id, "centre", sizeof model.stack_id);
        model.visible = true; model.resizable = true; model.closable = true;
        model.width = 0.6; model.height = 0.6; model.x = 0.1; model.y = 0.1;
        model.floating = i == 1U && strncmp(fixture.mode, "floating-", 9U) == 0;
        CHECK(umi_ui_workspace_layout_add_window(fixture.layout, &model) == UMI_STATUS_OK);
    }
    CHECK(umi_gtk4_workspace_layout_host_rebuild(fixture.host, fixture.layout) == UMI_STATUS_OK);
    root = g_object_ref(umi_gtk4_workspace_layout_host_widget(fixture.host));
    if (strncmp(fixture.mode, "floating-", 9U) == 0) {
        floating = GTK_WIDGET(gtk_widget_get_root(fixture.entries[1]));
        CHECK(GTK_IS_WINDOW(floating)); g_object_ref(floating);
        fixture.frame = Frame(floating, "editor");
    } else fixture.frame = Frame(root, "editor");
    CHECK(fixture.frame != NULL); g_object_ref(fixture.frame);
    GtkWidget *ancestor = gtk_widget_get_ancestor(fixture.frame, GTK_TYPE_NOTEBOOK);
    if (ancestor != NULL) fixture.notebook = g_object_ref(ancestor);
    if (strcmp(fixture.mode, "unmounted") != 0) {
        gtk_box_append(GTK_BOX(outer), root);
        gtk_window_set_default_size(GTK_WINDOW(fixture.window), 900, 600);
        gtk_window_present(GTK_WINDOW(fixture.window)); Settle();
    }
    if (fixture.notebook != NULL) gtk_notebook_set_current_page(GTK_NOTEBOOK(fixture.notebook), 0);
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(fixture.mode, "invalid") == 0) {
        CHECK(umi_gtk4_workspace_layout_host_focus_window(NULL, "editor") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_gtk4_workspace_layout_host_focus_window(fixture.host, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_gtk4_workspace_layout_host_focus_window(fixture.host, "") == UMI_STATUS_INVALID_ARGUMENT);
        goto done;
    }
    const char *id = strcmp(fixture.mode, "missing") == 0 ? "absent" : "editor";
    if (strcmp(fixture.mode, "missing") == 0) expected = UMI_STATUS_NOT_FOUND;
    if (strcmp(fixture.mode, "hidden-model") == 0) {
        fixture.layout->windows[1].visible = false;
        ++fixture.layout->revision;
        CHECK(umi_gtk4_workspace_layout_host_rebuild(fixture.host, fixture.layout) == UMI_STATUS_OK);
        expected = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(fixture.mode, "hidden-frame") == 0) {
        gtk_widget_set_visible(fixture.frame, FALSE); expected = UMI_STATUS_UNAVAILABLE;
    }
    if (strcmp(fixture.mode, "disabled-frame") == 0) {
        gtk_widget_set_sensitive(fixture.frame, FALSE); expected = UMI_STATUS_UNAVAILABLE;
    }
    if (strncmp(fixture.mode, "focus-", 6U) == 0 || strcmp(fixture.mode, "repeat") == 0) {
        gtk_notebook_set_current_page(GTK_NOTEBOOK(fixture.notebook), 1);
        Settle();
        CHECK(gtk_widget_grab_focus(fixture.outside));
    }
    if (strcmp(fixture.mode, "repeat") == 0) {
        CHECK(gtk_widget_grab_focus(fixture.entries[1]));
        GtkWidget *before = gtk_window_get_focus(GTK_WINDOW(fixture.window));
        gtk_editable_select_region(GTK_EDITABLE(fixture.entries[1]), 2, 8);
        for (unsigned i = 0U; i < 3U; ++i) {
            CHECK(umi_gtk4_workspace_layout_host_focus_window(fixture.host, id) == UMI_STATUS_OK);
            CHECK(gtk_window_get_focus(GTK_WINDOW(fixture.window)) == before);
            int start = 0, end = 0;
            CHECK(gtk_editable_get_selection_bounds(GTK_EDITABLE(fixture.entries[1]), &start, &end));
            CHECK(start == 2 && end == 8);
        }
        goto done;
    }
    if (strncmp(fixture.mode, "page-", 5U) == 0) {
        observed = G_OBJECT(fixture.notebook);
        handler = g_signal_connect(observed, "notify::page", G_CALLBACK(Observe), &fixture);
    } else if (strncmp(fixture.mode, "focus-", 6U) == 0) {
        observed = G_OBJECT(fixture.window);
        handler = g_signal_connect(observed, "notify::focus-widget", G_CALLBACK(Observe), &fixture);
    } else if (strncmp(fixture.mode, "floating-", 9U) == 0) {
        gtk_widget_set_visible(floating, FALSE);
        if (strcmp(fixture.mode, "floating-present") != 0) {
            observed = G_OBJECT(floating);
            handler = g_signal_connect(observed, "notify::visible", G_CALLBACK(Observe), &fixture);
        }
    }
    gboolean busy_case = strstr(fixture.mode, "reenter") != NULL ||
        strcmp(fixture.mode, "page-rebuild") == 0 ||
        strcmp(fixture.mode, "page-maximise") == 0 || strcmp(fixture.mode, "page-restore") == 0;
    if (handler != 0U && !busy_case && strcmp(fixture.mode, "page-reorder") != 0)
        expected = UMI_STATUS_CANCELLED;
    CHECK(umi_gtk4_workspace_layout_host_focus_window(fixture.host, id) == expected);
    if (handler != 0U) CHECK(fixture.calls == 1U);
    if (busy_case) CHECK(fixture.nested == UMI_STATUS_BUSY);
    if (strcmp(fixture.mode, "page-metadata") == 0) CHECK(fixture.nested == UMI_STATUS_OK);
    if (strstr(fixture.mode, "destroy-host") != NULL) CHECK(fixture.host == NULL);
    if (expected == UMI_STATUS_OK && fixture.notebook != NULL) {
        int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(fixture.notebook));
        CHECK(page >= 0 && gtk_notebook_get_nth_page(GTK_NOTEBOOK(fixture.notebook), page) == fixture.frame);
    }
    if (strcmp(fixture.mode, "focus-redirect") == 0)
        CHECK(Within(gtk_window_get_focus(GTK_WINDOW(fixture.window)), fixture.outside));
    CHECK(fixture.created == 2U);
    for (unsigned i = 0U; i < 2U; ++i)
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(fixture.entries[i])), "Draft before panel selection") == 0);
done:
    if (handler != 0U) g_signal_handler_disconnect(observed, handler);
    if (fixture.host != NULL) umi_gtk4_workspace_layout_host_destroy(fixture.host);
    gtk_window_destroy(GTK_WINDOW(fixture.window));
    gtk_window_destroy(GTK_WINDOW(fixture.foreign));
    g_clear_object(&fixture.frame); g_clear_object(&fixture.notebook);
    g_clear_object(&fixture.entries[0]); g_clear_object(&fixture.entries[1]);
    g_clear_object(&fixture.outside); g_clear_object(&root); g_clear_object(&floating);
    g_clear_object(&fixture.window); g_clear_object(&fixture.foreign);
    free(fixture.layout);
    return failed;
}
