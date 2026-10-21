/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/gui_control_checks/test_list_publication.c
 * PURPOSE: Verify bounded list replacement through real GTK ownership callbacks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/list_publication.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { fprintf(stderr, "%d: %s\n", __LINE__, #value); failed = 1; goto done; } } while (0)
typedef struct PublicationProbe {
    const char *mode;
    GtkWidget **callerRows;
    GtkWidget *extra;
    size_t calls;
    UmiStatus nested;
    int retired;
} PublicationProbe;
static void RetireFromParent(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property;
    PublicationProbe *probe = data;
    probe->retired = 1;
}
static UmiStatus PublicationCurrent(GtkListBox *list, void *data)
{
    PublicationProbe *probe = data;
    ++probe->calls;
    if (probe->retired || strcmp(probe->mode, "denied") == 0) return UMI_STATUS_CANCELLED;
    if (probe->calls == 1U && strcmp(probe->mode, "reenter") == 0)
        probe->nested = UmiGtk4ListBoxPublish(list, NULL, 0U, PublicationCurrent, probe);
    if (probe->calls == 1U && strcmp(probe->mode, "release-input") == 0) {
        g_object_unref(probe->callerRows[0]);
        g_object_unref(probe->callerRows[1]);
        free(probe->callerRows); probe->callerRows = NULL;
    }
    if (probe->calls == 2U && strcmp(probe->mode, "foreign-removal-row") == 0)
        gtk_list_box_append(list, probe->extra);
    if (probe->calls == 3U && strcmp(probe->mode, "foreign-append-row") == 0)
        gtk_list_box_append(list, probe->extra);
    return UMI_STATUS_OK;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *cases[] = {"normal", "empty", "denied", "cancel-remove", "cancel-append",
        "reenter", "release-input", "foreign-removal-row", "foreign-append-row",
        "parented", "duplicate", "floating", "invalid", "capacity", "old-capacity", "ancestor", "native-root"};
    int known = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(cases[i], argv[1]) == 0) known = 1;
    if (!known) return 2;
    if (!gtk_init_check()) return 77;
    int failed = 0;
    GtkWidget *list = g_object_ref_sink(gtk_list_box_new());
    GtkWidget *old = g_object_ref_sink(gtk_list_box_row_new());
    GtkWidget *first = g_object_ref_sink(gtk_list_box_row_new());
    GtkWidget *second = g_object_ref_sink(gtk_label_new("second"));
    GtkWidget *extra = g_object_ref_sink(gtk_list_box_row_new());
    GtkWidget *other = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkWidget *floating = NULL;
    GtkWidget *nativeRoot = NULL;
    gulong handler = 0U;
    GtkWidget *observed = NULL;
    PublicationProbe probe = {argv[1], NULL, extra, 0U, UMI_STATUS_OK, 0};
    gtk_list_box_append(GTK_LIST_BOX(list), old);
    GtkWidget *rows[] = {first, second};
    UmiStatus status;
    if (strcmp(argv[1], "invalid") == 0) {
        CHECK(UmiGtk4ListBoxPublish(NULL, rows, 2U, PublicationCurrent, &probe) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4ListBoxPublish(GTK_LIST_BOX(list), NULL, 2U, PublicationCurrent, &probe) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4ListBoxPublish(GTK_LIST_BOX(list), rows, 2U, NULL, &probe) == UMI_STATUS_INVALID_ARGUMENT);
        rows[1] = NULL;
    } else if (strcmp(argv[1], "parented") == 0) gtk_box_append(GTK_BOX(other), second);
    else if (strcmp(argv[1], "ancestor") == 0) { gtk_box_append(GTK_BOX(other), list); rows[1] = other; }
    else if (strcmp(argv[1], "native-root") == 0) { nativeRoot = g_object_ref_sink(gtk_window_new()); rows[1] = nativeRoot; }
    else if (strcmp(argv[1], "duplicate") == 0) rows[1] = first;
    else if (strcmp(argv[1], "floating") == 0) { floating = gtk_label_new("floating"); rows[1] = floating; }
    else if (strcmp(argv[1], "old-capacity") == 0) {
        for (size_t i = 0U; i < UMI_GTK4_LIST_PUBLICATION_LIMIT; ++i)
            gtk_list_box_append(GTK_LIST_BOX(list), gtk_label_new("retained"));
    } else if (strcmp(argv[1], "cancel-remove") == 0 || strcmp(argv[1], "cancel-append") == 0) {
        observed = strcmp(argv[1], "cancel-remove") == 0 ? old : first;
        handler = g_signal_connect(observed, "notify::parent", G_CALLBACK(RetireFromParent), &probe);
    }
    if (strcmp(argv[1], "release-input") == 0) {
        probe.callerRows = malloc(2U * sizeof *probe.callerRows);
        CHECK(probe.callerRows != NULL);
        probe.callerRows[0] = g_object_ref(first); probe.callerRows[1] = g_object_ref(second);
        status = UmiGtk4ListBoxPublish(GTK_LIST_BOX(list), probe.callerRows, 2U, PublicationCurrent, &probe);
        CHECK(probe.callerRows == NULL);
    } else status = UmiGtk4ListBoxPublish(GTK_LIST_BOX(list), rows,
        strcmp(argv[1], "capacity") == 0 ? UMI_GTK4_LIST_PUBLICATION_LIMIT + 1U :
        strcmp(argv[1], "empty") == 0 ? 0U : 2U, PublicationCurrent, &probe);
    if (strcmp(argv[1], "invalid") == 0 || strcmp(argv[1], "parented") == 0 ||
        strcmp(argv[1], "duplicate") == 0 || strcmp(argv[1], "floating") == 0 ||
        strcmp(argv[1], "ancestor") == 0 || strcmp(argv[1], "native-root") == 0) {
        CHECK(status == UMI_STATUS_INVALID_ARGUMENT && probe.calls == 0U);
        CHECK(gtk_list_box_get_row_at_index(GTK_LIST_BOX(list), 0) == GTK_LIST_BOX_ROW(old));
    } else if (strcmp(argv[1], "capacity") == 0 || strcmp(argv[1], "old-capacity") == 0) {
        CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && probe.calls == 0U);
        CHECK(gtk_list_box_get_row_at_index(GTK_LIST_BOX(list), 0) == GTK_LIST_BOX_ROW(old));
    } else if (strcmp(argv[1], "denied") == 0) {
        CHECK(status == UMI_STATUS_CANCELLED);
        CHECK(gtk_list_box_get_row_at_index(GTK_LIST_BOX(list), 0) == GTK_LIST_BOX_ROW(old));
    } else if (strncmp(argv[1], "cancel-", 7U) == 0) {
        CHECK(status == UMI_STATUS_CANCELLED && probe.retired);
        CHECK(gtk_widget_get_parent(second) == NULL);
    } else if (strncmp(argv[1], "foreign-", 8U) == 0) {
        CHECK(status == UMI_STATUS_INVALID_STATE);
        CHECK(gtk_widget_get_parent(extra) == list);
        CHECK(gtk_widget_get_parent(second) == NULL);
    } else {
        CHECK(status == UMI_STATUS_OK);
        CHECK(gtk_widget_get_parent(old) == NULL);
        if (strcmp(argv[1], "empty") == 0) CHECK(gtk_list_box_get_row_at_index(GTK_LIST_BOX(list), 0) == NULL);
        else {
            CHECK(gtk_list_box_get_row_at_index(GTK_LIST_BOX(list), 0) == GTK_LIST_BOX_ROW(first));
            CHECK(gtk_list_box_row_get_child(gtk_list_box_get_row_at_index(GTK_LIST_BOX(list), 1)) == second);
            CHECK(gtk_list_box_get_row_at_index(GTK_LIST_BOX(list), 2) == NULL);
        }
        if (strcmp(argv[1], "reenter") == 0) CHECK(probe.nested == UMI_STATUS_BUSY);
    }
    /* A completed or cancelled operation releases its reservation. A fresh
     * caller may clear the surviving page; no rollback is performed for it. */
    if (handler != 0U) { g_signal_handler_disconnect(observed, handler); handler = 0U; }
    probe.mode = "normal"; probe.retired = 0;
    if (strcmp(argv[1], "old-capacity") != 0)
        CHECK(UmiGtk4ListBoxPublish(GTK_LIST_BOX(list), NULL, 0U, PublicationCurrent, &probe) == UMI_STATUS_OK);
done:
    if (handler != 0U) g_signal_handler_disconnect(observed, handler);
    if (probe.callerRows != NULL) { g_object_unref(probe.callerRows[0]); g_object_unref(probe.callerRows[1]); free(probe.callerRows); }
    if (floating != NULL) g_object_ref_sink(floating);
    g_clear_object(&floating);
    if (nativeRoot != NULL) { gtk_window_destroy(GTK_WINDOW(nativeRoot)); g_object_unref(nativeRoot); }
    g_object_unref(list); g_object_unref(other);
    g_object_unref(old); g_object_unref(first); g_object_unref(second); g_object_unref(extra);
    return failed;
}
