/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build_configurations/test_gtk4.c
 * PURPOSE: Exercise review-only loads, explicit replacement, stale rows and retained native controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/build_configurations.h"
typedef struct ConfigurationFormProbe
{
    UmiDataServer *server;
    UmiGtk4DeveloperDialog *dialog;
    unsigned captures, saves, loads, applies;
    bool close_on_capture, wrong_root;
} ConfigurationFormProbe;
static UmiStatus Capture(const char *root, UmiBuildConfigurationCatalogue *out, void *context)
{
    ConfigurationFormProbe *probe = context;
    ++probe->captures;
    if (probe->close_on_capture)
    {
        UmiGtk4DeveloperDialogDestroy(probe->dialog);
        probe->dialog = NULL;
    }
    return UmiBuildConfigurationCapture(probe->server, root, out);
}
static UmiStatus Load(const char *root, const char *name, uint64_t revision, UmiBuildProfile *out,
                      void *context)
{
    ConfigurationFormProbe *probe = context;
    ++probe->loads;
    UmiStatus status = UmiBuildConfigurationLoad(probe->server, root, name, revision, out);
    if (status == UMI_STATUS_OK && probe->wrong_root)
        strcpy(out->source_directory, CONFIGURATION_OTHER);
    return status;
}
static UmiStatus Save(const char *name, const UmiBuildProfile *profile, uint64_t revision,
                      uint64_t *out_revision, void *context)
{
    ConfigurationFormProbe *probe = context;
    ++probe->saves;
    return UmiBuildConfigurationSave(probe->server, name, profile, revision, out_revision);
}
static UmiStatus Applied(const UmiBuildProfile *profile, int trusted, void *context)
{
    (void)profile;
    ConfigurationFormProbe *probe = context;
    CHECK(!trusted);
    ++probe->applies;
    return UMI_STATUS_OK;
}
static GtkWindow *FindForm(GtkWindow *parent)
{
    GListModel *windows = gtk_window_get_toplevels();
    for (guint index = 0U; index < g_list_model_get_n_items(windows); ++index)
    {
        GtkWindow *window = g_list_model_get_item(windows, index);
        if (gtk_window_get_transient_for(window) == parent &&
            gtk_widget_get_visible(GTK_WIDGET(window)))
            return window;
        g_object_unref(window);
    }
    return NULL;
}
static void ReentrantApply(GtkEditable *entry, gpointer context)
{
    (void)entry;
    g_signal_emit_by_name(context, "clicked");
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    g_setenv("GTK_A11Y", "test", TRUE);
    g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
    if (!gtk_init_check())
        return 77;
    ConfigurationFormProbe probe = {0};
    CHECK(umi_data_server_create_memory(&probe.server) == UMI_STATUS_OK);
    UmiBuildProfile *profile = malloc(sizeof *profile), *loaded = malloc(sizeof *loaded);
    CHECK(profile != NULL && loaded != NULL);
    ConfigurationFixtureProfile(profile);
    GtkWindow *parent = GTK_WINDOW(gtk_window_new());
    g_object_ref(parent);
    gtk_window_present(parent);
    CHECK(UmiGtk4BuildSettingsDialogCreate(parent, profile, 0, Applied, &probe, &probe.dialog) ==
          UMI_STATUS_OK);
    UmiGtk4BuildConfigurationCallbacks callbacks = {Capture, Load, Save, &probe};
    CHECK(UmiGtk4BuildSettingsBindConfigurations(probe.dialog, &callbacks) == UMI_STATUS_OK);
    CHECK(UmiGtk4BuildSettingsBindConfigurations(probe.dialog, &callbacks) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(probe.captures == 0U && probe.saves == 0U && probe.applies == 0U);
    GtkWindow *form = FindForm(parent);
    CHECK(form != NULL);
    GtkWidget *root = gtk_window_get_child(form);
    const char *tags[] = {"developer.configurations.refresh", "developer.configurations.name",
                          "developer.configurations.save",    "developer.configurations.load",
                          "developer.configurations.replace", "developer.build.field.13",
                          "developer.dialog.accept"};
    GtkWidget *widgets[7];
    for (size_t index = 0U; index < 7U; ++index)
    {
        widgets[index] = umi_gtk4_automation_find_tagged_widget(root, tags[index]);
        CHECK(widgets[index] != NULL);
        g_object_ref(widgets[index]);
    }
    GtkWidget *refresh = widgets[0], *name = widgets[1], *save = widgets[2], *load = widgets[3],
              *replace = widgets[4], *environment = widgets[5], *apply = widgets[6];
    if (strcmp(argv[1], "retained") == 0)
    {
        UmiGtk4DeveloperDialogDestroy(probe.dialog);
        probe.dialog = NULL;
        g_signal_emit_by_name(refresh, "clicked");
        g_signal_emit_by_name(save, "clicked");
        g_signal_emit_by_name(load, "clicked");
        g_signal_emit_by_name(apply, "clicked");
        CHECK(probe.captures == 0U && probe.saves == 0U && probe.loads == 0U &&
              probe.applies == 0U);
    }
    else if (strcmp(argv[1], "callback-close") == 0)
    {
        probe.close_on_capture = true;
        g_signal_emit_by_name(refresh, "clicked");
        CHECK(probe.dialog == NULL && probe.captures == 1U);
        g_signal_emit_by_name(save, "clicked");
        CHECK(probe.saves == 0U);
    }
    else
    {
        g_signal_emit_by_name(refresh, "clicked");
        gtk_editable_set_text(GTK_EDITABLE(name), "Debug data");
        gtk_editable_set_text(GTK_EDITABLE(environment), "APP_MODE=saved");
        g_signal_emit_by_name(save, "clicked");
        CHECK(probe.saves == 1U && probe.applies == 0U);
        UmiBuildConfigurationCatalogue catalogue;
        CHECK(UmiBuildConfigurationCapture(probe.server, CONFIGURATION_ROOT, &catalogue) ==
                  UMI_STATUS_OK &&
              catalogue.count == 1U);
        CHECK(UmiBuildConfigurationLoad(probe.server, CONFIGURATION_ROOT, "Debug data",
                                        catalogue.revision, loaded) == UMI_STATUS_OK);
        CHECK(strcmp(loaded->run_environment, "APP_MODE=saved") == 0);
        gtk_editable_set_text(GTK_EDITABLE(environment), "APP_MODE=draft");
        if (strcmp(argv[1], "replace") == 0)
        {
            g_signal_emit_by_name(save, "clicked");
            CHECK(probe.saves == 1U);
            gtk_check_button_set_active(GTK_CHECK_BUTTON(replace), TRUE);
            g_signal_emit_by_name(save, "clicked");
            CHECK(probe.saves == 2U);
            CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(replace)));
        }
        else if (strcmp(argv[1], "stale") == 0)
        {
            uint64_t revision = 0U;
            CHECK(UmiBuildConfigurationSave(probe.server, "Other", profile, catalogue.revision,
                                            &revision) == UMI_STATUS_OK);
            g_signal_emit_by_name(load, "clicked");
            CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(environment)), "APP_MODE=draft") == 0);
            CHECK(probe.applies == 0U);
        }
        else
        {
            probe.wrong_root = strcmp(argv[1], "wrong-root") == 0;
            bool reentrant = strcmp(argv[1], "reentrant") == 0;
            CHECK(probe.wrong_root || reentrant || strcmp(argv[1], "roundtrip") == 0);
            gulong handler = 0U;
            if (reentrant)
                handler =
                    g_signal_connect(environment, "changed", G_CALLBACK(ReentrantApply), apply);
            g_signal_emit_by_name(load, "clicked");
            if (handler != 0U)
                g_signal_handler_disconnect(environment, handler);
            CHECK(probe.loads == 1U && probe.applies == 0U);
            CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(environment)),
                         probe.wrong_root ? "APP_MODE=draft" : "APP_MODE=saved") == 0);
            if (!probe.wrong_root)
            {
                g_signal_emit_by_name(apply, "clicked");
                CHECK(probe.applies == 1U);
            }
        }
    }
    UmiGtk4DeveloperDialogDestroy(probe.dialog);
    for (size_t index = 0U; index < 7U; ++index)
        g_object_unref(widgets[index]);
    g_object_unref(form);
    gtk_window_destroy(parent);
    g_object_unref(parent);
    umi_data_server_destroy(probe.server);
    free(loaded);
    free(profile);
    return 0;
}
