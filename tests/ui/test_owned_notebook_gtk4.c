/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui/test_owned_notebook_gtk4.c
 *
 * PURPOSE:
 *   Verify notebook target identity through reordering, removal, reentry and owner changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/window_lifecycle.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); failed = 1; goto cleanup; } } while (0)

typedef struct SelectionObserver {
    GtkWindow *owner;
    GtkNotebook *notebook;
    GtkWidget *root, *other_root, *page, *first, *target;
    const char *mode;
    unsigned calls;
    UmiStatus nested;
} SelectionObserver;

static void PageChanged(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property;
    SelectionObserver *observer = data;
    if (observer->calls++ != 0U) return;
    const char *mode = observer->mode;
    if (strcmp(mode, "observer-close") == 0) gtk_window_destroy(observer->owner);
    else if (strcmp(mode, "observer-remove") == 0)
        gtk_notebook_remove_page(observer->notebook,
            gtk_notebook_page_num(observer->notebook, observer->page));
    else if (strcmp(mode, "observer-reparent") == 0) {
        gtk_box_remove(GTK_BOX(observer->root), GTK_WIDGET(observer->notebook));
        gtk_box_append(GTK_BOX(observer->other_root), GTK_WIDGET(observer->notebook));
    } else if (strcmp(mode, "observer-hide") == 0)
        gtk_widget_set_visible(observer->page, FALSE);
    else if (strcmp(mode, "observer-redirect") == 0)
        gtk_notebook_set_current_page(observer->notebook, 0);
    else if (strcmp(mode, "observer-reenter") == 0)
        observer->nested = UmiGtk4NotebookSelectOwnedTarget(observer->owner,
            observer->notebook, observer->target);
    else if (strcmp(mode, "observer-reorder") == 0)
        gtk_notebook_reorder_child(observer->notebook, observer->page, 0);
    else if (strcmp(mode, "observer-target") == 0) {
        gtk_box_remove(GTK_BOX(observer->page), observer->target);
        gtk_box_append(GTK_BOX(observer->first), observer->target);
    }
}

/* The fixture deliberately retains every referenced widget. Native removal
 * must be detected even when that delays finalisation. No main loop is needed
 * to exercise synchronous page observers, and no process provider is involved. */
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    const char *cases[] = {"select", "page-target", "repeat", "invalid", "wrong-owner",
        "outside", "closed-owner", "hidden", "observer-close", "observer-remove",
        "observer-reparent", "observer-hide", "observer-redirect", "observer-reenter",
        "observer-reorder", "observer-target"};
    int known = 0, failed = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(mode, cases[i]) == 0) known = 1;
    if (!known) return 2;
    if (!gtk_init_check()) return 77;
    GtkWindow *owner = g_object_ref(GTK_WINDOW(gtk_window_new()));
    GtkWindow *other = g_object_ref(GTK_WINDOW(gtk_window_new()));
    GtkWidget *root = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkWidget *other_root = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkNotebook *notebook = GTK_NOTEBOOK(g_object_ref_sink(gtk_notebook_new()));
    GtkWidget *first = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkWidget *page = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkWidget *target = g_object_ref_sink(gtk_entry_new());
    GtkWidget *outside = g_object_ref_sink(gtk_entry_new());
    gulong observer_id = 0U;
    SelectionObserver observer = {owner, notebook, root, other_root, page, first,
        target, mode, 0U, UMI_STATUS_OK};
    gtk_window_set_child(owner, root);
    gtk_window_set_child(other, other_root);
    gtk_box_append(GTK_BOX(root), GTK_WIDGET(notebook));
    gtk_box_append(GTK_BOX(root), outside);
    gtk_box_append(GTK_BOX(page), target);
    gtk_notebook_append_page(notebook, first, gtk_label_new("First"));
    gtk_notebook_append_page(notebook, page, gtk_label_new("Selected"));
    gtk_notebook_set_current_page(notebook, 0);
    gtk_editable_set_text(GTK_EDITABLE(target), "unsaved field");
    CHECK(gtk_notebook_get_current_page(notebook) == 0);
    if (strcmp(mode, "invalid") == 0) {
        CHECK(UmiGtk4NotebookSelectOwnedTarget(NULL, notebook, target) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4NotebookSelectOwnedTarget(owner, NULL, target) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4NotebookSelectOwnedTarget(owner, notebook, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(gtk_notebook_get_current_page(notebook) == 0);
        goto cleanup;
    }
    if (strcmp(mode, "wrong-owner") == 0 || strcmp(mode, "outside") == 0) {
        CHECK(UmiGtk4NotebookSelectOwnedTarget(strcmp(mode, "wrong-owner") == 0 ? other : owner,
            notebook, strcmp(mode, "outside") == 0 ? outside : target) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(gtk_notebook_get_current_page(notebook) == 0);
        goto cleanup;
    }
    if (strcmp(mode, "closed-owner") == 0) {
        gtk_window_destroy(owner);
        CHECK(UmiGtk4NotebookSelectOwnedTarget(owner, notebook, target) == UMI_STATUS_CANCELLED);
        goto cleanup;
    }
    if (strcmp(mode, "hidden") == 0) {
        gtk_widget_set_visible(page, FALSE);
        CHECK(UmiGtk4NotebookSelectOwnedTarget(owner, notebook, target) == UMI_STATUS_UNAVAILABLE);
        CHECK(gtk_notebook_get_current_page(notebook) == 0 && !gtk_widget_get_visible(page));
        goto cleanup;
    }
    if (strncmp(mode, "observer-", 9U) == 0)
        observer_id = g_signal_connect(notebook, "notify::page", G_CALLBACK(PageChanged), &observer);
    UmiStatus status = UmiGtk4NotebookSelectOwnedTarget(owner, notebook,
        strcmp(mode, "page-target") == 0 ? page : target);
    int cancelled = observer_id != 0U && strcmp(mode, "observer-reenter") != 0 &&
        strcmp(mode, "observer-reorder") != 0;
    CHECK(status == (cancelled ? UMI_STATUS_CANCELLED : UMI_STATUS_OK));
    if (observer_id != 0U) CHECK(observer.calls != 0U);
    if (strcmp(mode, "observer-reenter") == 0) CHECK(observer.nested == UMI_STATUS_BUSY);
    if (strcmp(mode, "repeat") == 0)
        CHECK(UmiGtk4NotebookSelectOwnedTarget(owner, notebook, target) == UMI_STATUS_OK);
    if (!cancelled) CHECK(gtk_notebook_get_nth_page(notebook,
        gtk_notebook_get_current_page(notebook)) == page);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(target)), "unsaved field") == 0);
cleanup:
    if (observer_id != 0U) g_signal_handler_disconnect(notebook, observer_id);
    gtk_window_destroy(owner); gtk_window_destroy(other);
    g_object_unref(outside); g_object_unref(target); g_object_unref(page);
    g_object_unref(first); g_object_unref(notebook); g_object_unref(root);
    g_object_unref(other_root); g_object_unref(other); g_object_unref(owner);
    return failed;
}
