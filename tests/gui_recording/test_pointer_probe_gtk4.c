/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/gui_recording/test_pointer_probe_gtk4.c
 *
 * PURPOSE:
 *   Verify that native UAT distinguishes real hit routing from control
 *   activation, rejects ambiguous tags and preserves private application state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../support/assertions.h"
#include "umicom/ui/gtk4/automation_pointer.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/interaction_recording.h"
#include <gtk/gtk.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void clicked(GtkButton *button, gpointer data)
{
    unsigned *count = data;
    (void)button;
    ++*count;
}

/* Observe a condition while dispatching bounded GTK work. The outer CTest
 * deadline also covers a callback that does not return to this test. */
static UmiGtk4AutomationPointerSample wait_ready(GtkWidget *scope)
{
    UmiGtk4AutomationPointerSample sample;
    const gint64 deadline = g_get_monotonic_time() + 3000000;
    do {
        for (unsigned n = 0U; n < 8U && g_main_context_pending(NULL); ++n)
            (void)g_main_context_iteration(NULL, FALSE);
        assert(UmiGtk4AutomationProbePointer(scope, "uat.target", &sample) == UMI_STATUS_OK);
        if (sample.state == UMI_GTK4_POINTER_READY) return sample;
        g_usleep(1000U);
    } while (g_get_monotonic_time() < deadline);
    fprintf(stderr, "Pointer fixture not ready: state=%d hit=%s\n", (int)sample.state, sample.hitType);
    assert(sample.state == UMI_GTK4_POINTER_READY);
    return sample;
}

int main(int argc, char **argv)
{
    GtkWidget *window;
    GtkWidget *overlay;
    GtkWidget *button;
    GtkWidget *cover;
    UmiGtk4AutomationPointerSample sample;
    unsigned activations = 0U;
    const char *which;
    assert(argc == 2);
    which = argv[1];
    if (!gtk_init_check()) return 77;
    window = gtk_window_new();
    g_object_ref_sink(window);
    gtk_window_set_default_size(GTK_WINDOW(window), 440, 240);
    overlay = gtk_overlay_new();
    button = gtk_button_new_with_label("UAT synthetic target");
    cover = gtk_drawing_area_new();
    gtk_overlay_set_child(GTK_OVERLAY(overlay), button);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), cover);
    gtk_widget_set_can_target(cover, FALSE);
    gtk_window_set_child(GTK_WINDOW(window), overlay);
    assert(umi_gtk4_automation_tag_widget(button, "uat.target") == UMI_STATUS_OK);
    g_signal_connect(button, "clicked", G_CALLBACK(clicked), &activations);
    if (strcmp(which, "unmapped") == 0) {
        assert(UmiGtk4AutomationProbePointer(overlay, "uat.target", &sample) == UMI_STATUS_OK);
        assert(sample.state == UMI_GTK4_POINTER_UNMAPPED);
    } else {
        gtk_window_present(GTK_WINDOW(window));
        sample = wait_ready(overlay);
        if (strcmp(which, "ready") == 0) {
            assert(sample.windowX > 0.0F && sample.windowY > 0.0F);
            assert(strcmp(sample.targetType, "GtkButton") == 0);
            assert(sample.hitType[0] != '\0');
        } else if (strcmp(which, "obstructed") == 0) {
            gtk_widget_set_can_target(cover, TRUE);
            /* Search within the button, but inspect hits from the window so
             * an overlay outside that lookup scope cannot escape detection. */
            assert(UmiGtk4AutomationProbePointer(button, "uat.target", &sample) == UMI_STATUS_OK);
            assert(sample.state == UMI_GTK4_POINTER_OBSTRUCTED);
            assert(strcmp(sample.hitType, "GtkDrawingArea") == 0);
            gtk_widget_set_can_target(cover, FALSE);
            assert(UmiGtk4AutomationProbePointer(button, "uat.target", &sample) == UMI_STATUS_OK);
            assert(sample.state == UMI_GTK4_POINTER_READY);
        } else if (strcmp(which, "disabled") == 0) {
            gtk_widget_set_sensitive(overlay, FALSE);
            assert(UmiGtk4AutomationProbePointer(overlay, "uat.target", &sample) == UMI_STATUS_OK);
            assert(sample.state == UMI_GTK4_POINTER_DISABLED);
        } else if (strcmp(which, "hidden") == 0) {
            gtk_widget_set_visible(button, FALSE);
            assert(UmiGtk4AutomationProbePointer(overlay, "uat.target", &sample) == UMI_STATUS_OK);
            assert(sample.state == UMI_GTK4_POINTER_UNMAPPED);
        } else if (strcmp(which, "ambiguous") == 0) {
            assert(umi_gtk4_automation_tag_widget(cover, "uat.target") == UMI_STATUS_OK);
            assert(UmiGtk4AutomationProbePointer(overlay, "uat.target", &sample) == UMI_STATUS_ALREADY_EXISTS);
            assert(sample.state == UMI_GTK4_POINTER_UNKNOWN && sample.targetType[0] == '\0');
        } else if (strcmp(which, "missing") == 0) {
            assert(UmiGtk4AutomationProbePointer(overlay, "uat.missing", &sample) == UMI_STATUS_NOT_FOUND);
            assert(sample.state == UMI_GTK4_POINTER_UNKNOWN && sample.hitType[0] == '\0');
        } else if (strcmp(which, "private") == 0) {
            assert(UmiGtk4RecordingSetPrivate(overlay, 1) == UMI_STATUS_OK);
            assert(UmiGtk4AutomationProbePointer(overlay, "uat.target", &sample) == UMI_STATUS_PERMISSION_DENIED);
            assert(sample.state == UMI_GTK4_POINTER_UNKNOWN && sample.targetType[0] == '\0');
        } else if (strcmp(which, "private-cover") == 0) {
            gtk_widget_set_can_target(cover, TRUE);
            assert(UmiGtk4RecordingSetPrivate(cover, 1) == UMI_STATUS_OK);
            assert(UmiGtk4AutomationProbePointer(overlay, "uat.target", &sample) == UMI_STATUS_OK);
            assert(sample.state == UMI_GTK4_POINTER_OBSTRUCTED && sample.hitType[0] == '\0');
        } else if (strcmp(which, "invalid") == 0) {
            assert(UmiGtk4AutomationProbePointer(overlay, "uat.target", NULL) == UMI_STATUS_INVALID_ARGUMENT);
            assert(UmiGtk4AutomationProbePointer(NULL, "uat.target", &sample) == UMI_STATUS_INVALID_ARGUMENT);
            assert(sample.state == UMI_GTK4_POINTER_UNKNOWN);
        } else assert(!"Unknown pointer fixture");
    }
    assert(activations == 0U);
    assert(strcmp(gtk_button_get_label(GTK_BUTTON(button)), "UAT synthetic target") == 0);
    gtk_window_destroy(GTK_WINDOW(window));
    g_object_unref(window);
    return 0;
}
