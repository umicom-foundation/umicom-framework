/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_installation/test_settings_gtk4.c
 * PURPOSE: Exercise SDK selection through the real settings form without configuring or running a project.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ui/gtk4/developer_dialog.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/build/configure_definitions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #value); return 1; \
} } while (0)
typedef struct SdkSettingsProbe {
    unsigned applied;
    UmiBuildProfile accepted;
} SdkSettingsProbe;
static UmiStatus Applied(const UmiBuildProfile *profile, int trusted, void *context)
{
    SdkSettingsProbe *probe = context;
    if (trusted) return UMI_STATUS_INVALID_STATE;
    ++probe->applied;
    probe->accepted = *profile;
    return UMI_STATUS_OK;
}
static GtkWindow *FindForm(GtkWindow *parent)
{
    GListModel *windows = gtk_window_get_toplevels();
    for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i) {
        GtkWindow *window = g_list_model_get_item(windows, i);
        if (gtk_window_get_transient_for(window) == parent) return window;
        g_object_unref(window);
    }
    return NULL;
}
static int WaitReady(GtkWidget *read)
{
    gint64 until = g_get_monotonic_time() + 5000000;
    while (!gtk_widget_get_sensitive(read) && g_get_monotonic_time() < until) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        g_usleep(1000U);
    }
    return gtk_widget_get_sensitive(read);
}
static void ReentrantApply(GtkEditable *entry, gpointer context)
{
    (void)entry;
    g_signal_emit_by_name(context, "clicked");
}
int main(int argc, char **argv)
{
    CHECK(argc == 3);
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    (void)g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
    if (!gtk_init_check()) return 77;
    UmiBuildProfile *profile = calloc(1U, sizeof *profile);
    SdkSettingsProbe *probe = calloc(1U, sizeof *probe);
    UmiArguments *arguments = calloc(1U, sizeof *arguments);
    CHECK(profile != NULL && probe != NULL && arguments != NULL);
    umi_build_profile_init(profile);
    CHECK(umi_build_profile_set(profile, "sdk-review", argv[2], "build") == UMI_STATUS_OK);
    strcpy(profile->configure_definitions, "-DKEEP=one");
    GtkWindow *parent = GTK_WINDOW(gtk_window_new());
    g_object_ref_sink(parent);
    gtk_window_present(parent);
    UmiGtk4DeveloperDialog *dialog = NULL;
    CHECK(UmiGtk4BuildSettingsDialogCreate(parent, profile, 0, Applied, probe, &dialog) == UMI_STATUS_OK);
    GtkWindow *form = FindForm(parent);
    CHECK(form != NULL);
    GtkWidget *root = gtk_window_get_child(form);
    GtkWidget *prefix = umi_gtk4_automation_find_tagged_widget(root, "developer.build.sdk.prefix");
    GtkWidget *read = umi_gtk4_automation_find_tagged_widget(root, "developer.build.sdk.read");
    GtkWidget *use = umi_gtk4_automation_find_tagged_widget(root, "developer.build.sdk.use");
    GtkWidget *definitions = umi_gtk4_automation_find_tagged_widget(root, "developer.build.field.12");
    GtkWidget *accept = umi_gtk4_automation_find_tagged_widget(root, "developer.dialog.accept");
    CHECK(GTK_IS_ENTRY(prefix) && GTK_IS_BUTTON(read) && GTK_IS_BUTTON(use) &&
        GTK_IS_ENTRY(definitions) && GTK_IS_BUTTON(accept));
    const char *mode = argv[1];
    if (strcmp(mode, "retained") == 0) {
        g_object_ref(read); g_object_ref(use);
        UmiGtk4DeveloperDialogDestroy(dialog); dialog = NULL;
        g_signal_emit_by_name(read, "clicked"); g_signal_emit_by_name(use, "clicked");
        CHECK(probe->applied == 0U); g_object_unref(read); g_object_unref(use);
    } else {
        CHECK(!gtk_widget_get_sensitive(use));
        gtk_editable_set_text(GTK_EDITABLE(prefix), argv[2]);
        g_signal_emit_by_name(read, "clicked");
        CHECK(!gtk_widget_get_sensitive(read));
        if (strcmp(mode, "edit_back") == 0) {
            gtk_editable_set_text(GTK_EDITABLE(prefix), "different");
            gtk_editable_set_text(GTK_EDITABLE(prefix), argv[2]);
        } else if (strcmp(mode, "hide") == 0) {
            gtk_widget_set_visible(GTK_WIDGET(form), FALSE);
            gtk_window_present(form);
        }
        CHECK(WaitReady(read));
        if (strcmp(mode, "edit_back") == 0 || strcmp(mode, "hide") == 0 ||
            strcmp(mode, "incomplete") == 0) {
            CHECK(!gtk_widget_get_sensitive(use));
            g_signal_emit_by_name(use, "clicked");
            CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(definitions)), "-DKEEP=one") == 0);
        } else {
            CHECK(strcmp(mode, "select") == 0 || strcmp(mode, "merge") == 0 ||
                strcmp(mode, "invalid") == 0 || strcmp(mode, "reentrant") == 0);
            CHECK(gtk_widget_get_sensitive(use));
            if (strcmp(mode, "merge") == 0)
                gtk_editable_set_text(GTK_EDITABLE(definitions), "-DKEEP=edited -DNEW=two");
            if (strcmp(mode, "invalid") == 0)
                gtk_editable_set_text(GTK_EDITABLE(definitions), "-P forbidden.cmake");
            gulong handler = 0U;
            if (strcmp(mode, "reentrant") == 0)
                handler = g_signal_connect(definitions, "changed", G_CALLBACK(ReentrantApply), accept);
            g_signal_emit_by_name(use, "clicked");
            if (handler != 0U) g_signal_handler_disconnect(definitions, handler);
            CHECK(probe->applied == 0U);
            if (strcmp(mode, "invalid") == 0)
                CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(definitions)), "-P forbidden.cmake") == 0);
            else {
                g_signal_emit_by_name(accept, "clicked");
                CHECK(probe->applied == 1U);
                CHECK(UmiBuildConfigureDefinitions(&probe->accepted, arguments) == UMI_STATUS_OK);
                CHECK(arguments->count == (strcmp(mode, "merge") == 0 ? 3U : 2U));
                CHECK(strcmp(arguments->values[0], strcmp(mode, "merge") == 0 ? "-DKEEP=edited" : "-DKEEP=one") == 0);
                CHECK(strncmp(arguments->values[arguments->count - 1U], "-DUmicomFramework_DIR:PATH=", sizeof "-DUmicomFramework_DIR:PATH=" - 1U) == 0);
            }
        }
        CHECK(strcmp(profile->configure_definitions, "-DKEEP=one") == 0);
    }
    UmiGtk4DeveloperDialogDestroy(dialog);
    g_object_unref(form); gtk_window_destroy(parent); g_object_unref(parent);
    free(arguments); free(probe); free(profile);
    return 0;
}
