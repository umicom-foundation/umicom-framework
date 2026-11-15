/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui/test_owned_window_present_gtk4.c
 *
 * PURPOSE:
 *   Verify checked transient presentation across removal, reparenting, hiding and reentrant observers.
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
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); \
    failed = 1; goto cleanup; } } while (0)

typedef struct Observer {
    GtkWindow *owner;
    GtkWindow *child;
    GtkWindow *other;
    const char *mode;
    unsigned calls;
    UmiStatus nested;
} Observer;

static void Visible(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)object; (void)spec;
    Observer *observer = data;
    if (observer->calls++ != 0U) return;
    if (strcmp(observer->mode, "observer-owner") == 0)
        gtk_window_destroy(observer->owner);
    else if (strcmp(observer->mode, "observer-child") == 0)
        gtk_window_destroy(observer->child);
    else if (strcmp(observer->mode, "observer-hide") == 0)
        gtk_widget_set_visible(GTK_WIDGET(observer->child), FALSE);
    else if (strcmp(observer->mode, "observer-reparent") == 0)
        gtk_window_set_transient_for(observer->child, observer->other);
    else observer->nested = UmiGtk4PresentOwnedWindow(observer->owner, observer->child);
}

int main(int argc, char **argv)
{
    const char *cases[] = {"present", "repeat", "invalid", "self", "wrong-owner",
        "closed-owner", "closed-child", "observer-owner", "observer-child",
        "observer-hide", "observer-reparent", "observer-reenter"};
    if (argc != 2) return 2;
    int known = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(argv[1], cases[i]) == 0) known = 1;
    if (!known) return 2;
    if (!gtk_init_check()) return 77;
    int failed = 0;
    const char *mode = argv[1];
    GtkWindow *owner = g_object_ref(GTK_WINDOW(gtk_window_new()));
    GtkWindow *child = g_object_ref(GTK_WINDOW(gtk_window_new()));
    GtkWindow *other = g_object_ref(GTK_WINDOW(gtk_window_new()));
    GObject *plain = g_object_new(G_TYPE_OBJECT, NULL);
    gulong observer_id = 0U;
    Observer observer = {owner, child, other, mode, 0U, UMI_STATUS_OK};
    gtk_window_set_transient_for(child, owner);
    if (strcmp(mode, "invalid") == 0) {
        CHECK(UmiGtk4PresentOwnedWindow(NULL, child) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4PresentOwnedWindow(owner, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4PresentOwnedWindow((GtkWindow *)plain, child) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4PresentOwnedWindow(owner, (GtkWindow *)plain) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!gtk_widget_get_visible(GTK_WIDGET(child)));
        goto cleanup;
    }
    if (strcmp(mode, "self") == 0) {
        CHECK(UmiGtk4PresentOwnedWindow(owner, owner) == UMI_STATUS_INVALID_ARGUMENT);
        goto cleanup;
    }
    if (strcmp(mode, "wrong-owner") == 0) {
        CHECK(UmiGtk4PresentOwnedWindow(other, child) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(gtk_window_get_transient_for(child) == owner);
        CHECK(!gtk_widget_get_visible(GTK_WIDGET(child)));
        goto cleanup;
    }
    if (strcmp(mode, "closed-owner") == 0) gtk_window_destroy(owner);
    if (strcmp(mode, "closed-child") == 0) gtk_window_destroy(child);
    if (strncmp(mode, "observer-", 9U) == 0)
        observer_id = g_signal_connect(child, "notify::visible", G_CALLBACK(Visible), &observer);
    UmiStatus status = UmiGtk4PresentOwnedWindow(owner, child);
    int expected_success = strcmp(mode, "present") == 0 || strcmp(mode, "repeat") == 0 ||
        strcmp(mode, "observer-reenter") == 0;
    CHECK(status == (expected_success ? UMI_STATUS_OK : UMI_STATUS_CANCELLED));
    if (observer_id != 0U) {
        CHECK(observer.calls != 0U);
        g_signal_handler_disconnect(child, observer_id); observer_id = 0U;
    }
    if (expected_success) {
        CHECK(UmiGtk4WindowIsOpen(owner) && UmiGtk4WindowIsOpen(child));
        CHECK(gtk_widget_get_visible(GTK_WIDGET(child)));
        CHECK(gtk_window_get_transient_for(child) == owner);
    }
    if (strcmp(mode, "observer-reenter") == 0) CHECK(observer.nested == UMI_STATUS_BUSY);
    if (strcmp(mode, "repeat") == 0 || strcmp(mode, "observer-reenter") == 0) {
        gtk_widget_set_visible(GTK_WIDGET(child), FALSE);
        CHECK(UmiGtk4PresentOwnedWindow(owner, child) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "observer-hide") == 0 || strcmp(mode, "observer-reparent") == 0) {
        gtk_window_set_transient_for(child, owner);
        CHECK(UmiGtk4PresentOwnedWindow(owner, child) == UMI_STATUS_OK);
    }
cleanup:
    if (observer_id != 0U) g_signal_handler_disconnect(child, observer_id);
    gtk_window_destroy(child); gtk_window_destroy(owner); gtk_window_destroy(other);
    g_object_unref(child); g_object_unref(owner); g_object_unref(other); g_object_unref(plain);
    return failed;
}
