/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compilation_database/test_export_gtk4.c
 * PURPOSE: Verify native export preparation changes only a reviewed form draft.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/developer_dialog.h"
typedef struct ExportProbe
{
    unsigned applies;
    UmiBuildProfile profile;
} ExportProbe;
static UmiStatus Applied(const UmiBuildProfile *profile, int trusted, void *context)
{
    ExportProbe *probe = context;
    if (trusted)
        return UMI_STATUS_INVALID_STATE;
    ++probe->applies;
    probe->profile = *profile;
    return UMI_STATUS_OK;
}
static void Reenter(GtkEditable *entry, gpointer data)
{
    (void)entry;
    g_signal_emit_by_name(data, "clicked");
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    CHECK(strcmp(mode, "draft") == 0 || strcmp(mode, "invalid") == 0 ||
          strcmp(mode, "retained") == 0 || strcmp(mode, "reentrant") == 0);
    g_setenv("GTK_A11Y", "test", TRUE);
    g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
    if (!gtk_init_check())
        return 77;
    UmiBuildProfile *profile = calloc(1U, sizeof *profile);
    ExportProbe *probe = calloc(1U, sizeof *probe);
    CHECK(profile != NULL && probe != NULL);
    umi_build_profile_init(profile);
    CHECK(umi_build_profile_set(profile, "export", DATABASE_ROOT, "build") == UMI_STATUS_OK);
    strcpy(profile->configure_definitions, "-DLESSON=\"two words\"");
    UmiGtk4DeveloperDialog *dialog = NULL;
    GtkWindow *parent = GTK_WINDOW(gtk_window_new()), *form = NULL;
    g_object_ref(parent);
    gtk_window_present(parent);
    CHECK(UmiGtk4BuildSettingsDialogCreate(parent, profile, 0, Applied, probe, &dialog) ==
          UMI_STATUS_OK);
    GListModel *windows = gtk_window_get_toplevels();
    for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i)
    {
        GtkWindow *candidate = g_list_model_get_item(windows, i);
        if (gtk_window_get_transient_for(candidate) == parent)
        {
            form = candidate;
            break;
        }
        g_object_unref(candidate);
    }
    CHECK(form != NULL);
    GtkWidget *export = umi_gtk4_automation_find_tagged_widget(
        GTK_WIDGET(form), "developer.compiler-database.export");
    GtkWidget *entry =
        umi_gtk4_automation_find_tagged_widget(GTK_WIDGET(form), "developer.build.field.12");
    GtkWidget *apply =
        umi_gtk4_automation_find_tagged_widget(GTK_WIDGET(form), "developer.dialog.accept");
    CHECK(export != NULL && entry != NULL && apply != NULL);
    g_object_ref(export);
    g_object_ref(entry);
    g_object_ref(apply);
    CHECK(probe->applies == 0U);
    if (strcmp(mode, "retained") == 0)
    {
        UmiGtk4DeveloperDialogDestroy(dialog);
        dialog = NULL;
        g_signal_emit_by_name(export, "clicked");
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entry)), "-DLESSON=\"two words\"") == 0);
    }
    else if (strcmp(mode, "invalid") == 0)
    {
        gtk_editable_set_text(GTK_EDITABLE(entry), "-DLESSON");
        g_signal_emit_by_name(export, "clicked");
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entry)), "-DLESSON") == 0);
    }
    else
    {
        if (strcmp(mode, "reentrant") == 0)
            g_signal_connect(entry, "changed", G_CALLBACK(Reenter), apply);
        g_signal_emit_by_name(export, "clicked");
        CHECK(probe->applies == 0U);
        UmiArguments *arguments = malloc(sizeof *arguments);
        CHECK(arguments != NULL);
        CHECK(UmiArgumentsParse(gtk_editable_get_text(GTK_EDITABLE(entry)), arguments) ==
              UMI_STATUS_OK);
        CHECK(arguments->count == 2U);
        CHECK(strcmp(arguments->values[0], "-DLESSON=two words") == 0);
        CHECK(strcmp(arguments->values[1], "-DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=ON") == 0);
        free(arguments);
        g_signal_emit_by_name(apply, "clicked");
        CHECK(probe->applies == 1U);
        CHECK(strstr(probe->profile.configure_definitions, "CMAKE_EXPORT_COMPILE_COMMANDS") !=
              NULL);
    }
    if (strcmp(mode, "retained") == 0 || strcmp(mode, "invalid") == 0)
        CHECK(probe->applies == 0U);
    UmiGtk4DeveloperDialogDestroy(dialog);
    gtk_window_destroy(parent);
    g_object_unref(export);
    g_object_unref(entry);
    g_object_unref(apply);
    g_object_unref(form);
    g_object_unref(parent);
    free(probe);
    free(profile);
    return EXIT_SUCCESS;
}
