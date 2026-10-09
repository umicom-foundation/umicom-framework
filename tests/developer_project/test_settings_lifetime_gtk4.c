/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_settings_lifetime_gtk4.c
 * PURPOSE: Exercise settings acceptance, stage selection and GTK callback ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/developer_dialog.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(test)                                                                                          \
    do                                                                                                       \
    {                                                                                                        \
        if (!(test))                                                                                         \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #test);                                       \
            failed = 1;                                                                                      \
            goto cleanup;                                                                                    \
        }                                                                                                    \
    } while (0)

typedef struct Probe
{
    UmiGtk4DeveloperDialog *dialog;
    GtkWidget *accept;
    const char *mode;
    unsigned calls;
    int trusted;
    UmiBuildProfile profile;
} Probe;

/* Match stable identifiers rather than translated labels. External references
 * intentionally keep controls alive after their controller has been retired. */
/* The rendered-child walk omitted controls owned by collapsed expanders. The shared bounded logical-tree lookup replaces it; retain the earlier traversal for review. */
#if 0
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0)
        return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child))
    {
        GtkWidget *found = Find(child, id);
        if (found != NULL)
            return found;
    }
    return NULL;
}
#endif
/* Use the Framework logical tree so a collapsed panel can be inspected
 * without changing the user's layout or overlooking an ambiguous identifier. */
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    return umi_gtk4_automation_find_tagged_widget(root, id);
}
static GtkWindow *Form(GtkWindow *parent)
{
    GListModel *windows = gtk_window_get_toplevels();
    for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i)
    {
        GtkWindow *window = g_list_model_get_item(windows, i);
        if (gtk_window_get_transient_for(window) == parent && gtk_widget_get_visible(GTK_WIDGET(window)))
            return window;
        g_object_unref(window);
    }
    return NULL;
}
static void Dispose(Probe *probe)
{
    UmiGtk4DeveloperDialog *dialog = probe->dialog;
    probe->dialog = NULL;
    UmiGtk4DeveloperDialogDestroy(dialog);
}
static UmiStatus Applied(const UmiBuildProfile *profile, int trusted, void *context)
{
    Probe *probe = context;
    ++probe->calls;
    probe->trusted = trusted;
    if (strcmp(probe->mode, "recursive") == 0 && probe->calls == 1U)
        g_signal_emit_by_name(probe->accept, "clicked");
    if (strcmp(probe->mode, "owner-destroy") == 0 || strcmp(probe->mode, "project-owner-destroy") == 0)
        Dispose(probe);
    /* Borrowed results remain valid for the remainder of this callback even
     * when it released the application-owned handle above. */
    probe->profile = *profile;
    return strcmp(probe->mode, "retry") == 0 && probe->calls == 1U ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
}
static UmiStatus Created(const UmiDeveloperProjectModel *model, const UmiBuildProfile *profile, void *context)
{
    if (model == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return Applied(profile, 0, context);
}
static void VisibilityChanged(GObject *object, GParamSpec *property, gpointer context)
{
    (void)property;
    if (!gtk_widget_get_visible(GTK_WIDGET(object)))
        Dispose(context);
}
int main(int argc, char **argv)
{
    Probe probe = {0};
    GtkWindow *parent = NULL, *form = NULL;
    GtkWidget *cancel = NULL;
    GtkWidget *chooseParent = NULL;
    char *projectRoot = NULL, *scratch = NULL;
    int failed = 0;
    CHECK(argc == 2);
    probe.mode = argv[1];
    if (!gtk_init_check())
        return 77;
    parent = GTK_WINDOW(gtk_window_new());
    g_object_ref(parent);
    umi_build_profile_init(&probe.profile);
    int project = strncmp(probe.mode, "project-", 8U) == 0;
    if (project)
    {
        /* The test owns this unique directory. No generated program is built
         * or run; only its source-file creation and adoption are exercised. */
        scratch = g_dir_make_tmp("umicom-form-XXXXXX", NULL);
        CHECK(scratch != NULL);
        projectRoot = g_build_filename(scratch, "Notes", NULL);
        CHECK(UmiGtk4NewProjectDialogCreate(parent, Created, &probe, &probe.dialog) == UMI_STATUS_OK);
    }
    else
    {
        CHECK(UmiGtk4BuildSettingsDialogCreate(parent, &probe.profile, 0, Applied, &probe, &probe.dialog) ==
              UMI_STATUS_OK);
    }
    form = Form(parent);
    CHECK(form != NULL);
    probe.accept = Find(GTK_WIDGET(form), "developer.dialog.accept");
    cancel = Find(GTK_WIDGET(form), "developer.dialog.cancel");
    if (probe.accept != NULL)
        g_object_ref(probe.accept);
    if (cancel != NULL)
        g_object_ref(cancel);
    CHECK(GTK_IS_BUTTON(probe.accept) && GTK_IS_BUTTON(cancel));
    if (project)
    {
        GtkWidget *folder = Find(GTK_WIDGET(form), "developer.project.folder");
        CHECK(GTK_IS_ENTRY(folder));
        chooseParent = Find(GTK_WIDGET(form), "developer.project.choose-parent");
        CHECK(GTK_IS_BUTTON(chooseParent));
        g_object_ref(chooseParent);
        gtk_editable_set_text(GTK_EDITABLE(folder), projectRoot);
    }
    else
    {
        const char *ids[] = {"developer.build.configure-preset", "developer.build.build-preset",
                             "developer.build.test-preset", "developer.build.run-directory"};
        const char *names[] = {"notes-configure", "notes-build", "notes-test", "sample data"};
        for (size_t i = 0U; i < sizeof(ids) / sizeof(ids[0]); ++i)
        {
            GtkWidget *field = Find(GTK_WIDGET(form), ids[i]);
            CHECK(GTK_IS_ENTRY(field));
            CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(field)), "") == 0);
            gtk_editable_set_text(GTK_EDITABLE(field), names[i]);
        }
    }
    if (strcmp(probe.mode, "retained") == 0 || strcmp(probe.mode, "project-retained") == 0)
    {
        Dispose(&probe);
        g_signal_emit_by_name(probe.accept, "clicked");
        g_signal_emit_by_name(cancel, "clicked");
        /* Retaining a folder chooser button must not retain authority after
         * the dialog owner was destroyed. This must not open a native dialog. */
        if (chooseParent != NULL) g_signal_emit_by_name(chooseParent, "clicked");
        CHECK(probe.calls == 0U);
        if (project)
            CHECK(!umi_fs_exists(projectRoot));
    }
    else if (strcmp(probe.mode, "parent-destroy") == 0)
    {
        gtk_window_destroy(parent);
        g_signal_emit_by_name(probe.accept, "clicked");
        CHECK(probe.calls == 0U);
    }
    else if (strcmp(probe.mode, "cancel") == 0 || strcmp(probe.mode, "project-cancel") == 0)
    {
        g_signal_emit_by_name(cancel, "clicked");
        CHECK(!gtk_widget_get_visible(GTK_WIDGET(form)));
        g_signal_emit_by_name(probe.accept, "clicked");
        CHECK(probe.calls == 0U);
        if (project)
            CHECK(!umi_fs_exists(projectRoot));
    }
    else if (strcmp(probe.mode, "mixed-fields") == 0)
    {
        GtkWidget *shared = Find(GTK_WIDGET(form), "developer.build.field.6");
        CHECK(GTK_IS_ENTRY(shared));
        gtk_editable_set_text(GTK_EDITABLE(shared), "shared-name");
        g_signal_emit_by_name(probe.accept, "clicked");
        CHECK(probe.calls == 0U && gtk_widget_get_visible(GTK_WIDGET(form)));
        gtk_editable_set_text(GTK_EDITABLE(shared), "");
        g_signal_emit_by_name(probe.accept, "clicked");
        CHECK(probe.calls == 1U);
    }
    else
    {
        if (strcmp(probe.mode, "notify-close") == 0)
            g_signal_connect(form, "notify::visible", G_CALLBACK(VisibilityChanged), &probe);
        g_signal_emit_by_name(probe.accept, "clicked");
        CHECK(probe.calls == 1U);
        if (strcmp(probe.mode, "retry") == 0)
        {
            CHECK(gtk_widget_get_visible(GTK_WIDGET(form)));
            g_signal_emit_by_name(probe.accept, "clicked");
            CHECK(probe.calls == 2U);
        }
        CHECK(!gtk_widget_get_visible(GTK_WIDGET(form)));
        unsigned accepted = probe.calls;
        g_signal_emit_by_name(probe.accept, "clicked");
        CHECK(probe.calls == accepted);
        if (strcmp(probe.mode, "owner-destroy") == 0 || strcmp(probe.mode, "project-owner-destroy") == 0 ||
            strcmp(probe.mode, "notify-close") == 0)
            CHECK(probe.dialog == NULL);
        if (project)
            CHECK(umi_fs_is_directory(projectRoot));
    }
    if (strcmp(probe.mode, "stage-fields") == 0 || strcmp(probe.mode, "mixed-fields") == 0)
    {
        CHECK(strcmp(probe.profile.configure_preset, "notes-configure") == 0);
        CHECK(strcmp(probe.profile.build_preset, "notes-build") == 0);
        CHECK(strcmp(probe.profile.test_preset, "notes-test") == 0);
        CHECK(strcmp(probe.profile.run_working_directory, "sample data") == 0);
        CHECK(probe.profile.preset[0] == '\0' && !probe.trusted);
    }
cleanup:
    if (chooseParent != NULL) g_object_unref(chooseParent);
    if (form != NULL)
        g_signal_handlers_disconnect_by_data(form, &probe);
    Dispose(&probe);
    if (form != NULL)
        g_object_unref(form);
    if (probe.accept != NULL)
        g_object_unref(probe.accept);
    if (cancel != NULL)
        g_object_unref(cancel);
    if (parent != NULL)
    {
        gtk_window_destroy(parent);
        g_object_unref(parent);
    }
    if (scratch != NULL && umi_fs_remove_tree(scratch) != UMI_STATUS_OK)
        failed = 1;
    g_free(projectRoot);
    g_free(scratch);
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
