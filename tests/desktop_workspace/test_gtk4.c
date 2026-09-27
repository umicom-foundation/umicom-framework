/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Actual GTK construction/lifetime checks. No display or SQLite means NOT RUN.
 * These tests do not boot an OS or exercise the entire Desk product. */
#include "umicom/desktop_workspace/gtk4.h"
#include "umicom/desktop_workspace/workspace.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <string.h>
static GtkWidget *Find(GtkWidget *widget, const char *name)
{
    if (!strcmp(gtk_widget_get_name(widget), name)) return widget;
    for (GtkWidget *child = gtk_widget_get_first_child(widget); child; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, name); if (found) return found;
    }
    return NULL;
}
static int Wait(GtkWindow *window)
{
    gint64 until = g_get_monotonic_time() + 10000000;
    while (g_object_get_data(G_OBJECT(window), "umicom.desktop-workspace.busy")) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        if (g_get_monotonic_time() >= until) return 0;
        g_usleep(1000);
    }
    return 1;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) { puts("NOT RUN: no GTK display."); return 77; }
    GError *error = NULL; char *parent = g_dir_make_tmp("umicom-workspace-gtk-XXXXXX", &error);
    if (!parent) { g_clear_error(&error); return 1; }
    char *path = g_build_filename(parent, "workspace", NULL);
    GtkWindow *window = UmiDesktopWorkspaceGtkCreate(NULL, path);
    if (!window) return 1;
    g_object_ref(window);
    GtkWidget *open = Find(GTK_WIDGET(window), "umicom.workspace.open");
    GtkWidget *save = Find(GTK_WIDGET(window), "umicom.workspace.save");
    if (!open || !save || gtk_widget_get_sensitive(save)) return 1;
    if (strcmp(argv[1], "construct")) {
        g_signal_emit_by_name(open, "clicked");
        if (!strcmp(argv[1], "close-during-open")) gtk_window_close(window);
        if (!Wait(window)) return 1;
        if (!strcmp(argv[1], "open-close")) { gtk_window_close(window); if (!Wait(window)) return 1; }
        UmiDesktopWorkspace *w = NULL; UmiStatus status = UmiDesktopWorkspaceOpenDirectory(path, &w);
        if (status == UMI_STATUS_UNAVAILABLE) { gtk_window_destroy(window);g_object_unref(window);g_free(path);g_free(parent);return 77; }
        if (status != UMI_STATUS_OK || UmiDesktopWorkspacePreviousSessionUnfinished(w)) return 1;
        if (UmiDesktopWorkspaceCloseClean(w) != UMI_STATUS_OK) return 1;
        UmiDesktopWorkspaceDestroy(w);
    }
    gtk_window_destroy(window);g_object_unref(window);g_free(path);g_free(parent);
    while (g_main_context_iteration(NULL, FALSE)) {}
    return 0;
}
