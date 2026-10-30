/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_workspace_focus_gtk4.c
 *
 * PURPOSE:
 *   Verify panel keyboard navigation after native workspace rebuild and maximisation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workstation/workspace_layout_host.h"
#include "umicom/ui/gtk4/workstation/focus_cycle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed = 1; goto done; } } while (0)

typedef struct Fixture { GtkWidget *bodies[3]; unsigned count; } Fixture;
static const char *const Ids[] = {"files", "editor", "output"};
static const char *const Places[] = {"left", "centre", "bottom"};

static GtkWidget *CreateBody(const UmiUiWorkspaceWindow *model, void *context)
{
    Fixture *fixture = context;
    for (size_t i = 0U; i < 3U; ++i) {
        if (strcmp(Ids[i], model->window_id) == 0) {
            GtkWidget *body = gtk_entry_new();
            fixture->bodies[i] = g_object_ref_sink(body);
            gtk_editable_set_text(GTK_EDITABLE(body), "Unsaved draft");
            ++fixture->count;
            return body;
        }
    }
    return NULL;
}
static void Settle(void)
{
    gint64 until = g_get_monotonic_time() + G_TIME_SPAN_SECOND / 3;
    while (g_get_monotonic_time() < until) {
        for (unsigned i = 0U; i < 32U && g_main_context_pending(NULL); ++i)
            (void)g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
}
static gboolean Focused(GtkWidget *window, GtkWidget *body)
{
    GtkWidget *focus = gtk_window_get_focus(GTK_WINDOW(window));
    return focus != NULL && (focus == body || gtk_widget_is_ancestor(focus, body));
}
static GtkWidget *FirstPane(GtkWidget *root)
{
    if (GTK_IS_PANED(root)) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = FirstPane(child);
        if (found != NULL) return found;
    }
    return NULL;
}
int main(void)
{
    if (!gtk_init_check()) return 77;
    int failed = 0;
    Fixture fixture = {{NULL, NULL, NULL}, 0U};
    UmiUiWorkspaceLayout *layout = calloc(1U, sizeof(*layout));
    UmiGtk4WorkspaceLayoutHost *host = NULL;
    GtkWidget *window = g_object_ref_sink(gtk_window_new());
    GtkWidget *root = NULL;
    CHECK(layout != NULL);
    CHECK(umi_ui_workspace_layout_init(layout, "keyboard", "Keyboard workspace") == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_layout_set_locked(layout, false) == UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_create(layout, CreateBody, &fixture, &host) == UMI_STATUS_OK);
    CHECK(umi_gtk4_workspace_layout_host_set_content_retention(host, 1) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 3U; ++i) {
        UmiUiWorkspaceWindow model = {0};
        (void)g_strlcpy(model.window_id, Ids[i], sizeof(model.window_id));
        (void)g_strlcpy(model.title, Ids[i], sizeof(model.title));
        (void)g_strlcpy(model.tool_id, "test.entry", sizeof(model.tool_id));
        (void)g_strlcpy(model.placement_id, Places[i], sizeof(model.placement_id));
        (void)g_strlcpy(model.stack_id, Ids[i], sizeof(model.stack_id));
        model.x = 0.1; model.y = 0.1; model.width = 0.5; model.height = 0.6;
        model.visible = true; model.resizable = true; model.closable = true;
        CHECK(umi_ui_workspace_layout_add_window(layout, &model) == UMI_STATUS_OK);
    }
    /* Retention is selected on the empty host, before creating provider bodies. */
    CHECK(umi_gtk4_workspace_layout_host_rebuild(host, layout) == UMI_STATUS_OK);
    root = umi_gtk4_workspace_layout_host_widget(host);
    gtk_window_set_child(GTK_WINDOW(window), root);
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 700);
    gtk_window_present(GTK_WINDOW(window)); Settle();
    CHECK(gtk_widget_grab_focus(fixture.bodies[0]));
    GtkWidget *pane = FirstPane(root);
    CHECK(pane != NULL);
    gboolean handled = FALSE;
    g_signal_emit_by_name(pane, "cycle-child-focus", FALSE, &handled);
    CHECK(handled && Focused(window, fixture.bodies[1]));
    CHECK(gtk_widget_grab_focus(fixture.bodies[0]));
    CHECK(UmiGtk4FocusCycleAdvance(root, FALSE) && Focused(window, fixture.bodies[1]));
    CHECK(UmiGtk4FocusCycleAdvance(root, FALSE) && Focused(window, fixture.bodies[2]));
    CHECK(UmiGtk4FocusCycleAdvance(root, FALSE) && Focused(window, fixture.bodies[0]));
    layout->windows[1].visible = false;
    ++layout->revision;
    CHECK(umi_gtk4_workspace_layout_host_rebuild(host, layout) == UMI_STATUS_OK);
    Settle();
    CHECK(gtk_widget_grab_focus(fixture.bodies[0]));
    CHECK(UmiGtk4FocusCycleAdvance(root, FALSE) && Focused(window, fixture.bodies[2]));
    layout->windows[1].visible = true;
    ++layout->revision;
    CHECK(umi_gtk4_workspace_layout_host_rebuild(host, layout) == UMI_STATUS_OK);
    Settle();
    CHECK(umi_gtk4_workspace_layout_host_maximise_window(host, "editor") == UMI_STATUS_OK);
    Settle();
    CHECK(gtk_widget_grab_focus(fixture.bodies[1]));
    CHECK(!UmiGtk4FocusCycleAdvance(root, FALSE));
    CHECK(Focused(window, fixture.bodies[1]));
    CHECK(umi_gtk4_workspace_layout_host_restore_maximised(host) == UMI_STATUS_OK);
    Settle();
    CHECK(gtk_widget_grab_focus(fixture.bodies[0]));
    CHECK(UmiGtk4FocusCycleAdvance(root, FALSE) && Focused(window, fixture.bodies[1]));
    CHECK(fixture.count == 3U);
    for (size_t i = 0U; i < 3U; ++i)
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(fixture.bodies[i])), "Unsaved draft") == 0);
done:
    gtk_window_set_child(GTK_WINDOW(window), NULL);
    if (host != NULL) umi_gtk4_workspace_layout_host_destroy(host);
    gtk_window_destroy(GTK_WINDOW(window));
    for (size_t i = 0U; i < 3U; ++i) g_clear_object(&fixture.bodies[i]);
    g_object_unref(window);
    free(layout);
    return failed;
}
