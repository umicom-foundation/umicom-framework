/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/native_learning_help/main.c
 *
 * PURPOSE:
 *   Demonstrate shared explicit Help and F1 using a locally admitted guide.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/learning_help_binding.h"
#include "umicom/base/version.h"
#include <stdio.h>
#include <string.h>

static void Help(GtkButton *button, gpointer unused)
{
    (void)unused;
    UmiStatus status = UmiLearningHelpGtk4Open(GTK_WIDGET(button));
    if (status != UMI_STATUS_OK)
        fprintf(stderr, "Help admission: %s\n", umi_status_text(status));
}
static void ExplicitGuide(GtkButton *button,gpointer catalogue)
{
    (void)button;
    UmiLearningHelpRoute route={0};
    strcpy(route.request.product,"learning-example");
    strcpy(route.request.locale,"en");
    route.request.kind=UMI_LEARNING_HELP_TOPIC;
    strcpy(route.request.key,"explicit-guide");
    strcpy(route.pack_id,"framework.native-help-example");
    strcpy(route.resource_id,"guide");
    /* A short-lived product route need not alter the surrounding F1 policy. */
    UmiStatus status=UmiLearningHelpGtk4OpenRoute(GTK_WIDGET(catalogue),&route);
    if(status!=UMI_STATUS_OK)
        fprintf(stderr,"Guide admission: %s\n",umi_status_text(status));
}
static gboolean Closing(GtkWindow *window, gpointer panel)
{
    (void)window;
    UmiLearningCatalogueGtk4Retire(GTK_WIDGET(panel));
    return FALSE;
}
static void Destroying(GtkWidget *window, gpointer panel)
{
    (void)window;
    UmiLearningCatalogueGtk4Retire(GTK_WIDGET(panel));
}
static void Activate(GtkApplication *application, gpointer unused)
{
    (void)unused;
    GtkWindow *existing = gtk_application_get_active_window(application);
    if (existing != NULL) { gtk_window_present(existing); return; }
    UmiVersion version = {UMICOM_FRAMEWORK_VERSION_MAJOR,
        UMICOM_FRAMEWORK_VERSION_MINOR, UMICOM_FRAMEWORK_VERSION_PATCH};
    GtkWidget *panel = UmiLearningCatalogueGtk4Create(version);
    if (panel == NULL) {
        g_object_set_data(G_OBJECT(application), "example-creation-failed", GINT_TO_POINTER(1));
        fprintf(stderr, "Could not create the learning catalogue.\n"); return;
    }
    GtkWidget *window = gtk_application_window_new(application);
    GtkWidget *context = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *help = gtk_button_new_with_label("Help for this example");
    gtk_box_append(GTK_BOX(context), help);
    GtkWidget *explicit_guide=gtk_button_new_with_label("Open guide without changing F1");
    gtk_box_append(GTK_BOX(context),explicit_guide);
    gtk_box_append(GTK_BOX(context), panel);
    gtk_window_set_child(GTK_WINDOW(window), context);
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 720);
    gtk_window_set_title(GTK_WINDOW(window), "Local contextual help");
    UmiLearningHelpRoute route = {0};
    strcpy(route.request.product, "learning-example");
    strcpy(route.request.locale, "en");
    route.request.kind = UMI_LEARNING_HELP_TOPIC;
    strcpy(route.request.key, "getting-started");
    strcpy(route.pack_id, "framework.native-help-example");
    strcpy(route.resource_id, "guide");
    UmiStatus status = UmiLearningHelpGtk4Bind(context, panel, &route);
    if (status != UMI_STATUS_OK) {
        g_object_set_data(G_OBJECT(application), "example-creation-failed", GINT_TO_POINTER(1));
        fprintf(stderr, "Could not bind help: %s\n", umi_status_text(status));
        UmiLearningCatalogueGtk4Retire(panel);
        gtk_window_destroy(GTK_WINDOW(window));
        return;
    }
    /* The button looks up its nearest bound ancestor. No callback borrows a
     * stack route or application controller. F1 uses that same copied policy. */
    g_signal_connect(help, "clicked", G_CALLBACK(Help), NULL);
    /* The object connection disconnects when its catalogue is released. */
    g_signal_connect_object(explicit_guide,"clicked",G_CALLBACK(ExplicitGuide),panel,0);
    g_signal_connect_object(window, "close-request", G_CALLBACK(Closing), panel, 0);
    g_signal_connect_object(window, "destroy", G_CALLBACK(Destroying), panel, 0);
    gtk_window_present(GTK_WINDOW(window));
}
int main(int argc, char **argv)
{
    GtkApplication *application = gtk_application_new(
        "org.umicom.examples.local-help", G_APPLICATION_NON_UNIQUE);
    g_signal_connect(application, "activate", G_CALLBACK(Activate), NULL);
    int result = g_application_run(G_APPLICATION(application), argc, argv);
    /* Catalogue jobs hold this application until their GTK cleanup completes.
     * This example closes its window normally; it never calls quit while a
     * worker still owns a read or a user-requested output write. */
    if (UmiLearningCatalogueGtk4PendingReads() != 0U) {
        fprintf(stderr, "Catalogue work did not finish before application exit.\n");
        result = 1;
    }
    if (g_object_get_data(G_OBJECT(application), "example-creation-failed") != NULL) result = 1;
    g_object_unref(application);
    return result;
}
