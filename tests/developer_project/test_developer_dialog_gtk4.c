/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_developer_dialog_gtk4.c
 * PURPOSE: Exercise the real project and settings forms without running generated code.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/developer_dialog.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(test) do { if (!(test)) { fprintf(stderr, "line %d: %s\n", __LINE__, #test); failed = 1; goto cleanup; } } while (0)

typedef struct Probe {
    unsigned created;
    unsigned applied;
    int trusted;
    UmiBuildProfile profile;
} Probe;

/* Match stable automation identifiers, not translated captions. */
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, id);
        if (found != NULL) return found;
    }
    return NULL;
}

/* Return an owned reference to the newly presented transient form. */
static GtkWindow *FindForm(GtkWindow *parent)
{
    GListModel *windows = gtk_window_get_toplevels();
    for (guint index = 0U; index < g_list_model_get_n_items(windows); ++index) {
        GtkWindow *window = g_list_model_get_item(windows, index);
        if (gtk_window_get_transient_for(window) == parent &&
            gtk_widget_get_visible(GTK_WIDGET(window))) return window;
        g_object_unref(window);
    }
    return NULL;
}
static UmiStatus Created(const UmiDeveloperProjectModel *model,
    const UmiBuildProfile *profile, void *context)
{
    Probe *probe = context;
    if (model == NULL || profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    ++probe->created;
    probe->profile = *profile;
    return UMI_STATUS_OK;
}
static UmiStatus Applied(const UmiBuildProfile *profile, int trusted, void *context)
{
    Probe *probe = context;
    ++probe->applied;
    probe->trusted = trusted;
    probe->profile = *profile;
    return UMI_STATUS_OK;
}
int main(void)
{
    UmiGtk4DeveloperDialog *dialog = NULL;
    GtkWindow *parent = NULL, *form = NULL;
    Probe probe = {0};
    char cwd[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY], file[UMI_PATH_CAPACITY];
    int failed = 0, generated = 0;
    if (!gtk_init_check()) return 77;
    CHECK(umi_fs_current_directory(cwd, sizeof(cwd)) == UMI_STATUS_OK);
    CHECK(umi_fs_join(root, sizeof(root), cwd, "Umicom Notes Form") == UMI_STATUS_OK);
    CHECK(!umi_fs_exists(root));
    parent = GTK_WINDOW(gtk_window_new());
    g_object_ref_sink(parent);
    CHECK(UmiGtk4NewProjectDialogCreate(parent, Created, &probe, &dialog) == UMI_STATUS_OK);
    form = FindForm(parent);
    CHECK(form != NULL);
    GtkWidget *folder = Find(GTK_WIDGET(form), "developer.project.folder");
    GtkWidget *target = Find(GTK_WIDGET(form), "developer.project.target");
    GtkWidget *accept = Find(GTK_WIDGET(form), "developer.dialog.accept");
    CHECK(GTK_IS_ENTRY(folder) && GTK_IS_ENTRY(target) && GTK_IS_BUTTON(accept));
    gtk_editable_set_text(GTK_EDITABLE(folder), root);
    gtk_editable_set_text(GTK_EDITABLE(target), "invalid) target");
    g_signal_emit_by_name(accept, "clicked");
    CHECK(probe.created == 0U && !umi_fs_exists(root));
    gtk_editable_set_text(GTK_EDITABLE(target), "umicom_notes");
    g_signal_emit_by_name(accept, "clicked");
    generated = probe.created == 1U;
    CHECK(generated && !gtk_widget_get_visible(GTK_WIDGET(form)));
    CHECK(umi_fs_join(file, sizeof(file), root, "src/main.c") == UMI_STATUS_OK && umi_fs_is_file(file));
    CHECK(umi_fs_join(file, sizeof(file), root, "build") == UMI_STATUS_OK && !umi_fs_exists(file));
    g_object_unref(form); form = NULL;
    UmiGtk4DeveloperDialogDestroy(dialog); dialog = NULL;
    CHECK(UmiGtk4BuildSettingsDialogCreate(parent, &probe.profile, 0, Applied, &probe, &dialog) == UMI_STATUS_OK);
    form = FindForm(parent); CHECK(form != NULL);
    GtkWidget *trust = Find(GTK_WIDGET(form), "developer.build.trusted");
    accept = Find(GTK_WIDGET(form), "developer.dialog.accept");
    CHECK(GTK_IS_CHECK_BUTTON(trust) && GTK_IS_BUTTON(accept));
    CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(trust)));
    gtk_check_button_set_active(GTK_CHECK_BUTTON(trust), TRUE);
    g_signal_emit_by_name(accept, "clicked");
    CHECK(probe.applied == 1U && probe.trusted == 1);
    CHECK(!gtk_widget_get_visible(GTK_WIDGET(form)));
    /* Test the parent-first teardown order as well as dialog-first above. */
    gtk_window_destroy(parent);
cleanup:
    if (form != NULL) g_object_unref(form);
    UmiGtk4DeveloperDialogDestroy(dialog);
    if (parent != NULL) { gtk_window_destroy(parent); g_object_unref(parent); }
    if (generated && umi_fs_remove_tree(root) != UMI_STATUS_OK) failed = 1;
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
