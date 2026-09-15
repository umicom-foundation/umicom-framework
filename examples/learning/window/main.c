/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/window/main.c
 *
 * PURPOSE:
 *   Compose a native window with existing Framework component factories.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

#include "umicom/ui/components/component.h"
#include "umicom/ui/gtk4/component_factory.h"
#include "umicom/ui/gtk4/workstation/shell_header.h"
#include "umicom/ui/gtk4/workstation/window_fit.h"

/* This callback is the exercise's behaviour, not another button implementation.
 * The label is owned by its parent; the signal connection observes its lifetime. */
static void SayHello(GtkButton *button, GtkLabel *label)
{
    (void)button;
    gtk_label_set_text(label, "You connected a button to a C function.");
}

static void Activate(GtkApplication *application, gpointer userData)
{
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *label;
    GtkWidget *button;
    UmiUiComponentSpec spec;
    UmiStatus status;
    int *startupFailed = userData;

    /* Re-activation presents the same window instead of leaking a second tree. */
    GtkWindow *existingWindow = gtk_application_get_active_window(application);
    if (existingWindow != NULL) {
        gtk_window_present(existingWindow);
        return;
    }

    spec = umi_ui_component_spec_default(UMI_UI_COMPONENT_WINDOW);
    status = umi_ui_component_spec_set_id(&spec, "learning.window");
    if (status == UMI_STATUS_OK)
        status = umi_ui_component_spec_set_text(&spec, "Learning with Umicom");
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Window specification failed: %d\n", (int)status);
        *startupFailed = 1;
        g_application_quit(G_APPLICATION(application));
        return;
    }
    spec.visible = false;
    spec.width = 640;
    spec.height = 360;
    window = umi_gtk4_component_create(&spec);
    if (window == NULL) {
        *startupFailed = 1;
        g_application_quit(G_APPLICATION(application));
        return;
    }
    gtk_window_set_application(GTK_WINDOW(window), application);
    /* Reuse the established branding and sizing policies. Do not draw a text
     * substitute for the logo. Missing installed assets are reported. */
    status = umi_gtk4_ws_apply_window_identity(GTK_WINDOW(window));
    if (status != UMI_STATUS_OK)
        (void)fprintf(stderr, "Window identity unavailable: %d\n", (int)status);
    (void)umi_gtk4_ws_window_fit(GTK_WINDOW(window), 640, 360, 320, 200);

    spec = umi_ui_component_spec_default(UMI_UI_COMPONENT_BOX);
    spec.orientation = UMI_UI_VERTICAL;
    spec.spacing = 16;
    box = umi_gtk4_component_create(&spec);
    spec = umi_ui_component_spec_default(UMI_UI_COMPONENT_LABEL);
    status = umi_ui_component_spec_set_text(&spec, "Press the button to run your callback.");
    label = status == UMI_STATUS_OK ? umi_gtk4_component_create(&spec) : NULL;
    spec = umi_ui_component_spec_default(UMI_UI_COMPONENT_BUTTON);
    status = umi_ui_component_spec_set_text(&spec, "Say hello");
    button = status == UMI_STATUS_OK ? umi_gtk4_component_create(&spec) : NULL;

    if (box == NULL || label == NULL || button == NULL) {
        /* Unparented widgets have floating references. Sink and release each
         * one exactly once on this rare failure path. */
        if (button != NULL) { g_object_ref_sink(button); g_object_unref(button); }
        if (label != NULL) { g_object_ref_sink(label); g_object_unref(label); }
        if (box != NULL) { g_object_ref_sink(box); g_object_unref(box); }
        gtk_window_destroy(GTK_WINDOW(window));
        *startupFailed = 1;
        g_application_quit(G_APPLICATION(application));
        return;
    }
    gtk_widget_set_margin_top(box, 24);
    gtk_widget_set_margin_bottom(box, 24);
    gtk_widget_set_margin_start(box, 24);
    gtk_widget_set_margin_end(box, 24);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_box_append(GTK_BOX(box), label);
    gtk_box_append(GTK_BOX(box), button);
    gtk_window_set_child(GTK_WINDOW(window), box);
    g_signal_connect_object(button, "clicked", G_CALLBACK(SayHello), label, 0);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv)
{
    GtkApplication *application = gtk_application_new(
        "org.umicom.learning.window", G_APPLICATION_DEFAULT_FLAGS);
    if (application == NULL) return EXIT_FAILURE;
    int startupFailed = 0;
    g_signal_connect(application, "activate", G_CALLBACK(Activate), &startupFailed);
    const int result = g_application_run(G_APPLICATION(application), argc, argv);
    g_object_unref(application);
    return result != 0 ? result : (startupFailed ? EXIT_FAILURE : EXIT_SUCCESS);
}
