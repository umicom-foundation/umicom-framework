/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/run_environment/test_gtk4.c
 * PURPOSE: Exercise launch-variable editing and retained controls in the shared project settings form.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "fixture.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/developer_dialog.h"
typedef struct FormProbe
{
    unsigned applied;
    UmiBuildProfile accepted;
} FormProbe;
static UmiStatus Applied(const UmiBuildProfile *profile, int trusted, void *context)
{
    FormProbe *probe = context;
    CHECK(!trusted);
    ++probe->applied;
    probe->accepted = *profile;
    return UMI_STATUS_OK;
}
static GtkWindow *Form(GtkWindow *parent)
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
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    (void)g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
    if (!gtk_init_check())
        return 77;
    UmiBuildProfile profile;
    umi_build_profile_init(&profile);
    strcpy(profile.source_directory, PROJECT_ROOT);
    strcpy(profile.run_environment, "APP_MODE=start");
    UmiGtk4DeveloperDialog *dialog = NULL;
    FormProbe *probe = calloc(1U, sizeof *probe);
    CHECK(probe != NULL);
    GtkWindow *parent = GTK_WINDOW(gtk_window_new());
    g_object_ref(parent);
    gtk_window_present(parent);
    CHECK(UmiGtk4BuildSettingsDialogCreate(parent, &profile, 0, Applied, probe, &dialog) ==
          UMI_STATUS_OK);
    GtkWindow *form = Form(parent);
    CHECK(form != NULL);
    GtkWidget *root = gtk_window_get_child(form);
    GtkWidget *definitions =
        umi_gtk4_automation_find_tagged_widget(root, "developer.build.field.13");
    GtkWidget *accept = umi_gtk4_automation_find_tagged_widget(root, "developer.dialog.accept");
    CHECK(GTK_IS_ENTRY(definitions) && GTK_IS_BUTTON(accept));
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(definitions)), profile.run_environment) == 0);
    const char *mode = argv[1];
    if (strcmp(mode, "invalid") == 0)
    {
        const char *invalid[] = {"no-assignment", "1INVALID=value", "MODE=one mode=two"};
        for (size_t index = 0U; index < sizeof invalid / sizeof invalid[0]; ++index)
        {
            gtk_editable_set_text(GTK_EDITABLE(definitions), invalid[index]);
            g_signal_emit_by_name(accept, "clicked");
            CHECK(probe->applied == 0U && gtk_widget_get_visible(GTK_WIDGET(form)));
        }
    }
    else if (strcmp(mode, "retained") == 0)
    {
        g_object_ref(accept);
        UmiGtk4DeveloperDialogDestroy(dialog);
        dialog = NULL;
        g_signal_emit_by_name(accept, "clicked");
        CHECK(probe->applied == 0U);
        g_object_unref(accept);
    }
    else if (strcmp(mode, "cancel") == 0)
    {
        gtk_editable_set_text(GTK_EDITABLE(definitions), "APP_MODE=changed");
        GtkWidget *cancel = umi_gtk4_automation_find_tagged_widget(root, "developer.dialog.cancel");
        CHECK(GTK_IS_BUTTON(cancel));
        g_signal_emit_by_name(cancel, "clicked");
        CHECK(probe->applied == 0U && strcmp(profile.run_environment, "APP_MODE=start") == 0);
    }
    else
    {
        CHECK(strcmp(mode, "values") == 0 || strcmp(mode, "preset") == 0);
        if (strcmp(mode, "preset") == 0)
        {
            GtkWidget *preset =
                umi_gtk4_automation_find_tagged_widget(root, "developer.build.configure-preset");
            CHECK(GTK_IS_ENTRY(preset));
            gtk_editable_set_text(GTK_EDITABLE(preset), "local-configure");
        }
        const char *text = "DATA_FOLDER=\"C:/Data caf\xc3\xa9\" APP_MODE=review";
        gtk_editable_set_text(GTK_EDITABLE(definitions), text);
        g_signal_emit_by_name(accept, "clicked");
        CHECK(probe->applied == 1U && strcmp(probe->accepted.run_environment, text) == 0);
        CHECK(strcmp(profile.run_environment, "APP_MODE=start") == 0);
        UmiGtk4DeveloperDialogDestroy(dialog);
        dialog = NULL;
        g_object_unref(form);
        CHECK(UmiGtk4BuildSettingsDialogCreate(parent, &probe->accepted, 0, Applied, probe,
                                               &dialog) == UMI_STATUS_OK);
        form = Form(parent);
        CHECK(form != NULL);
        definitions = umi_gtk4_automation_find_tagged_widget(gtk_window_get_child(form),
                                                             "developer.build.field.13");
        CHECK(GTK_IS_ENTRY(definitions) &&
              strcmp(gtk_editable_get_text(GTK_EDITABLE(definitions)), text) == 0);
    }
    UmiGtk4DeveloperDialogDestroy(dialog);
    g_object_unref(form);
    gtk_window_destroy(parent);
    g_object_unref(parent);
    free(probe);
    return 0;
}
