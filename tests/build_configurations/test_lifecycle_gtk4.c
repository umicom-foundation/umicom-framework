/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build_configurations/test_lifecycle_gtk4.c
 * PURPOSE: Exercise selection-bound removal consent and lifecycle callbacks on the real settings form.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/build_configurations.h"
typedef struct LifecycleHost
{
    UmiDataServer *server;
    UmiGtk4DeveloperDialog *dialog;
    unsigned renamed, removed, applied;
    bool close_on_edit;
} LifecycleHost;
static UmiStatus Capture(const char *root, UmiBuildConfigurationCatalogue *out, void *context)
{
    return UmiBuildConfigurationCapture(((LifecycleHost *)context)->server, root, out);
}
static UmiStatus Load(const char *root, const char *name, uint64_t revision, UmiBuildProfile *out,
                      void *context)
{
    return UmiBuildConfigurationLoad(((LifecycleHost *)context)->server, root, name, revision, out);
}
static UmiStatus Save(const char *name, const UmiBuildProfile *profile, uint64_t revision,
                      uint64_t *out, void *context)
{
    return UmiBuildConfigurationSave(((LifecycleHost *)context)->server, name, profile, revision,
                                     out);
}
static UmiStatus Applied(const UmiBuildProfile *profile, int trusted, void *context)
{
    (void)profile;
    (void)trusted;
    ++((LifecycleHost *)context)->applied;
    return UMI_STATUS_OK;
}
static UmiStatus Rename(const char *root, const char *name, const char *replacement,
                        uint64_t revision, uint64_t *out, void *context)
{
    LifecycleHost *host = context;
    ++host->renamed;
    if (host->close_on_edit)
    {
        UmiGtk4DeveloperDialogDestroy(host->dialog);
        host->dialog = NULL;
    }
    return UmiBuildConfigurationRename(host->server, root, name, replacement, revision, out);
}
static UmiStatus Remove(const char *root, const char *name, uint64_t revision, uint64_t *out,
                        void *context)
{
    LifecycleHost *host = context;
    ++host->removed;
    return UmiBuildConfigurationRemove(host->server, root, name, revision, out);
}
static void ReenterRemove(GtkCheckButton *check, gpointer button)
{
    (void)check;
    g_signal_emit_by_name(button, "clicked");
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *cases[] = {"rename", "remove",   "selection",      "refresh",
                           "stale",  "retained", "callback-close", "reentrant"};
    bool known = false;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = true;
    CHECK(known);
    g_setenv("GTK_A11Y", "test", TRUE);
    g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
    if (!gtk_init_check())
        return 77;
    LifecycleHost host = {0};
    CHECK(umi_data_server_create_memory(&host.server) == UMI_STATUS_OK);
    UmiBuildProfile *profile = malloc(sizeof *profile);
    CHECK(profile != NULL);
    ConfigurationFixtureProfile(profile);
    uint64_t revision = 0U;
    CHECK(UmiBuildConfigurationSave(host.server, "First", profile, revision, &revision) ==
          UMI_STATUS_OK);
    CHECK(UmiBuildConfigurationSave(host.server, "Second", profile, revision, &revision) ==
          UMI_STATUS_OK);
    GtkWindow *parent = GTK_WINDOW(gtk_window_new());
    g_object_ref(parent);
    gtk_window_present(parent);
    CHECK(UmiGtk4BuildSettingsDialogCreate(parent, profile, 0, Applied, &host, &host.dialog) ==
          UMI_STATUS_OK);
    UmiGtk4BuildConfigurationCallbacks callbacks = {Capture, Load, Save, &host};
    UmiGtk4BuildConfigurationLifecycleCallbacks lifecycle = {Rename, Remove, &host};
    CHECK(UmiGtk4BuildSettingsBindConfigurationLifecycle(host.dialog, &lifecycle) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(UmiGtk4BuildSettingsBindConfigurations(host.dialog, &callbacks) == UMI_STATUS_OK);
    CHECK(UmiGtk4BuildSettingsBindConfigurationLifecycle(host.dialog, &lifecycle) == UMI_STATUS_OK);
    CHECK(UmiGtk4BuildSettingsBindConfigurationLifecycle(host.dialog, &lifecycle) ==
          UMI_STATUS_INVALID_STATE);
    GtkWindow *form = NULL;
    GListModel *windows = gtk_window_get_toplevels();
    for (guint index = 0U; index < g_list_model_get_n_items(windows); ++index)
    {
        GtkWindow *candidate = g_list_model_get_item(windows, index);
        if (gtk_window_get_transient_for(candidate) == parent)
        {
            form = candidate;
            break;
        }
        g_object_unref(candidate);
    }
    CHECK(form != NULL);
    const char *tags[] = {"developer.configurations.refresh",
                          "developer.configurations.name",
                          "developer.configurations.choices",
                          "developer.configurations.rename",
                          "developer.configurations.remove-consent",
                          "developer.configurations.remove",
                          "developer.build.field.13"};
    GtkWidget *widgets[7];
    for (size_t index = 0U; index < 7U; ++index)
    {
        widgets[index] = umi_gtk4_automation_find_tagged_widget(GTK_WIDGET(form), tags[index]);
        CHECK(widgets[index] != NULL);
        g_object_ref(widgets[index]);
    }
    GtkWidget *refresh = widgets[0], *name = widgets[1], *choices = widgets[2],
              *rename = widgets[3], *consent = widgets[4], *remove = widgets[5],
              *environment = widgets[6];
    g_signal_emit_by_name(refresh, "clicked");
    gtk_editable_set_text(GTK_EDITABLE(environment), "APP_MODE=unsaved-draft");
    if (strcmp(mode, "retained") == 0)
    {
        UmiGtk4DeveloperDialogDestroy(host.dialog);
        host.dialog = NULL;
        g_signal_emit_by_name(rename, "clicked");
        g_signal_emit_by_name(remove, "clicked");
        CHECK(host.renamed == 0U && host.removed == 0U);
    }
    else if (strcmp(mode, "rename") == 0 || strcmp(mode, "callback-close") == 0)
    {
        host.close_on_edit = strcmp(mode, "callback-close") == 0;
        gtk_editable_set_text(GTK_EDITABLE(name), "Renamed");
        g_signal_emit_by_name(rename, "clicked");
        CHECK(host.renamed == 1U);
        UmiBuildConfigurationCatalogue catalogue;
        CHECK(UmiBuildConfigurationCapture(host.server, CONFIGURATION_ROOT, &catalogue) ==
              UMI_STATUS_OK);
        CHECK(strcmp(catalogue.names[0], "Renamed") == 0 && catalogue.count == 2U);
    }
    else
    {
        g_signal_emit_by_name(remove, "clicked");
        CHECK(host.removed == 0U);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(consent), TRUE);
        if (strcmp(mode, "selection") == 0)
        {
            gtk_drop_down_set_selected(GTK_DROP_DOWN(choices), 1U);
            CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(consent)));
        }
        else if (strcmp(mode, "refresh") == 0)
        {
            g_signal_emit_by_name(refresh, "clicked");
            CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(consent)));
        }
        else if (strcmp(mode, "stale") == 0)
        {
            CHECK(UmiBuildConfigurationSave(host.server, "Another window", profile, revision,
                                            &revision) == UMI_STATUS_OK);
        }
        else if (strcmp(mode, "reentrant") == 0)
        {
            g_signal_connect(consent, "toggled", G_CALLBACK(ReenterRemove), remove);
        }
        g_signal_emit_by_name(remove, "clicked");
        UmiBuildConfigurationCatalogue catalogue;
        CHECK(UmiBuildConfigurationCapture(host.server, CONFIGURATION_ROOT, &catalogue) ==
              UMI_STATUS_OK);
        if (strcmp(mode, "selection") == 0 || strcmp(mode, "refresh") == 0)
            CHECK(host.removed == 0U && catalogue.count == 2U);
        else if (strcmp(mode, "stale") == 0)
            CHECK(host.removed == 1U && catalogue.count == 3U);
        else
            CHECK(host.removed == 1U && catalogue.count == 1U &&
                  strcmp(catalogue.names[0], "Second") == 0);
    }
    CHECK(host.applied == 0U);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(environment)), "APP_MODE=unsaved-draft") == 0);
    UmiGtk4DeveloperDialogDestroy(host.dialog);
    for (size_t index = 0U; index < 7U; ++index)
        g_object_unref(widgets[index]);
    g_object_unref(form);
    gtk_window_destroy(parent);
    g_object_unref(parent);
    umi_data_server_destroy(host.server);
    free(profile);
    return 0;
}
