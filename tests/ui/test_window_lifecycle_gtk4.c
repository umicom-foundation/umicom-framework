/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui/test_window_lifecycle_gtk4.c
 * PURPOSE: Check retained window removal, cancelled observation and reentrant callbacks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/window_lifecycle.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed = 1; goto cleanup; } } while (0)
typedef struct Notice { unsigned calls; int sawClosed; GtkWindow *child; } Notice;

/* A callback can safely destroy another window after its watch disconnects. */
static void Removed(GtkWidget *window, gpointer data)
{
    Notice *notice = data;
    ++notice->calls;
    notice->sawClosed = !UmiGtk4WindowIsOpen(GTK_WINDOW(window));
    if (notice->child != NULL) gtk_window_destroy(notice->child);
}

/* No window is presented; retaining references deliberately delays finalisation. */
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1]; int failed = 0;
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    if (!gtk_init_check()) return 77;
    GtkWindow *window = g_object_ref(GTK_WINDOW(gtk_window_new()));
    GtkWindow *child = NULL;
    GObject *receiver = g_object_new(G_TYPE_OBJECT, NULL);
    Notice notice = {0}, childNotice = {0};
    CHECK(UmiGtk4WindowIsOpen(window));
    if (strcmp(name, "invalid") == 0) {
        CHECK(!UmiGtk4WindowIsOpen(NULL));
        CHECK(UmiGtk4WatchWindowClosed(NULL, receiver, Removed, &notice) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4WatchWindowClosed(window, NULL, Removed, &notice) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4WatchWindowClosed(window, receiver, NULL, &notice) == UMI_STATUS_INVALID_ARGUMENT);
        goto cleanup;
    }
    if (strcmp(name, "already-closed") == 0) gtk_window_destroy(window);
    if (strcmp(name, "reentrant") == 0) {
        child = g_object_ref(GTK_WINDOW(gtk_window_new())); notice.child = child;
        CHECK(UmiGtk4WatchWindowClosed(child, receiver, Removed, &childNotice) == UMI_STATUS_OK);
    }
    CHECK(UmiGtk4WatchWindowClosed(window, receiver, Removed, &notice) == UMI_STATUS_OK);
    if (strcmp(name, "receiver-gone") == 0) g_clear_object(&receiver);
    else if (strcmp(name, "retained") != 0 && strcmp(name, "already-closed") != 0 && strcmp(name, "reentrant") != 0) { failed = 2; goto cleanup; }
    gtk_window_destroy(window);
    CHECK(!UmiGtk4WindowIsOpen(window));
    CHECK(notice.calls == (strcmp(name, "receiver-gone") == 0 ? 0U : 1U));
    if (notice.calls != 0U) CHECK(notice.sawClosed);
    if (child != NULL) CHECK(childNotice.calls == 1U && childNotice.sawClosed);
    gtk_window_destroy(window);
    CHECK(notice.calls <= 1U);
cleanup:
    g_clear_object(&receiver);
    if (window != NULL) gtk_window_destroy(window);
    if (child != NULL) gtk_window_destroy(child);
    g_clear_object(&window); g_clear_object(&child);
    return failed;
}
