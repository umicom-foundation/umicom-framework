/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_simulation_starters_gtk4.c
 *
 * PURPOSE:
 *   Exercise catalogue selection, file creation, profile adoption and cancellation through the real New Project form.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/developer_dialog.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/platform/filesystem.h"
#include "creation_wait.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { fprintf(stderr, "line %d: %s\n", __LINE__, #value); result = EXIT_FAILURE; goto done; } } while (0)
typedef struct Capture {
    unsigned count;
    UmiDeveloperProjectModel model;
    UmiBuildProfile profile;
} Capture;
static UmiStatus Created(const UmiDeveloperProjectModel *model,
    const UmiBuildProfile *profile, void *context)
{
    Capture *capture = context;
    if (model == NULL || profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    ++capture->count;
    capture->model = *model;
    capture->profile = *profile;
    return UMI_STATUS_OK;
}
/* The test owns the returned reference independently of the dialog handle. */
static GtkWindow *Form(GtkWindow *parent)
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
int main(int argc, char **argv)
{
    int result = EXIT_SUCCESS, ownsProject = 0;
    int creationSettled = 1;
    int vhdl, cancel;
    UmiGtk4DeveloperDialog *dialog = NULL;
    GtkWindow *parent = NULL, *form = NULL;
    GtkWidget *picker, *folder, *accept;
    Capture capture = {0};
    char *temporary = NULL, *project = NULL, *source = NULL, *build = NULL;
    guint selected = GTK_INVALID_LIST_POSITION;
    if (argc != 2) return EXIT_FAILURE;
    vhdl = strcmp(argv[1], "vhdl") == 0;
    cancel = strcmp(argv[1], "cancel") == 0;
    if (!vhdl && !cancel && strcmp(argv[1], "umicom-c") != 0) return EXIT_FAILURE;
    if (!gtk_init_check()) return 77;
    temporary = g_dir_make_tmp("umicom-starter-XXXXXX", NULL);
    CHECK(temporary != NULL);
    project = g_build_filename(temporary, "Learning Project", NULL);
    source = g_build_filename(project, vhdl ? "rtl" : "src", vhdl ? "learning_cpu.vhd" : "main.uc", NULL);
    build = g_build_filename(project, "build", NULL);
    CHECK(!umi_fs_exists(project));
    parent = GTK_WINDOW(gtk_window_new());
    g_object_ref_sink(parent);
    CHECK(UmiGtk4NewProjectDialogCreate(parent, Created, &capture, &dialog) == UMI_STATUS_OK);
    form = Form(parent);
    CHECK(form != NULL);
    picker = umi_gtk4_automation_find_tagged_widget(form, "developer.project.template");
    folder = umi_gtk4_automation_find_tagged_widget(form, "developer.project.folder");
    accept = umi_gtk4_automation_find_tagged_widget(form, "developer.dialog.accept");
    CHECK(GTK_IS_DROP_DOWN(picker) && GTK_IS_ENTRY(folder) && GTK_IS_BUTTON(accept));
    GListModel *choices = gtk_drop_down_get_model(GTK_DROP_DOWN(picker));
    CHECK(GTK_IS_STRING_LIST(choices));
    const char *title = vhdl ? "VHDL CPU Simulation" : "Umicom C Arithmetic Preview";
    for (guint index = 0U; index < g_list_model_get_n_items(choices); ++index) {
        if (strcmp(gtk_string_list_get_string(GTK_STRING_LIST(choices), index), title) == 0) {
            CHECK(selected == GTK_INVALID_LIST_POSITION);
            selected = index;
        }
    }
    CHECK(selected != GTK_INVALID_LIST_POSITION);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(picker), selected);
    gtk_editable_set_text(GTK_EDITABLE(folder), project);
    CHECK(capture.count == 0U && !umi_fs_exists(project));
    if (cancel) {
        gtk_window_close(form);
        CHECK(capture.count == 0U && !umi_fs_exists(project));
        CHECK(!gtk_widget_get_visible(GTK_WIDGET(form)));
    } else {
        /* This invokes the registered GTK signal, not the generator directly. */
        ownsProject = 1;
        creationSettled = 0;
        g_signal_emit_by_name(accept, "clicked");
        creationSettled = UmiTestProjectCreationWait(form);
        CHECK(creationSettled);
        CHECK(capture.count == 1U);
        CHECK(!gtk_widget_get_visible(GTK_WIDGET(form)));
        CHECK(umi_fs_is_file(source) && !umi_fs_exists(build));
        CHECK(strcmp(capture.model.template_id, vhdl ?
            "developer.template.vhdl-cpu" : "developer.template.umicom-c-preview") == 0);
        CHECK(capture.model.kind == UMI_DEVELOPER_PROJECT_META);
        CHECK(strcmp(capture.profile.preset, vhdl ? "simulation" : "preview") == 0);
        CHECK(strcmp(capture.profile.build_directory, vhdl ? "build/simulation" : "build/preview") == 0);
        CHECK(capture.profile.run_program[0] == '\0');
        if (vhdl) {
            /* Project creation must deliver the connected program, not merely
             * a new menu entry or the isolated CPU instruction model. */
            const char *parts[] = {
                "rtl/learning_program.vhd", "rtl/learning_system.vhd",
                "tests/tb_learning_system.vhd"
            };
            for (size_t part = 0U; part < sizeof parts / sizeof parts[0]; ++part) {
                char *generated = g_build_filename(project, parts[part], NULL);
                int exists = umi_fs_is_file(generated);
                g_free(generated);
                CHECK(exists);
            }
        }
        g_signal_emit_by_name(accept, "clicked");
        CHECK(capture.count == 1U);
    }
done:
    UmiGtk4DeveloperDialogDestroy(dialog);
    if (form != NULL) g_object_unref(form);
    if (parent != NULL) { gtk_window_destroy(parent); g_object_unref(parent); }
    /* Only the unique fixture subtree created above belongs to this test. */
/* A timed-out writer may still own its fixture. Preserve that directory instead of racing cleanup. The previous implementation is retained for review. */
#if 0
    if (ownsProject && umi_fs_exists(project) && umi_fs_remove_tree(project) != UMI_STATUS_OK)
        result = EXIT_FAILURE;
    if (temporary != NULL && g_rmdir(temporary) != 0) result = EXIT_FAILURE;

#endif
    if (creationSettled && ownsProject && umi_fs_exists(project) && umi_fs_remove_tree(project) != UMI_STATUS_OK)
        result = EXIT_FAILURE;
    if (creationSettled && temporary != NULL && g_rmdir(temporary) != 0) result = EXIT_FAILURE;

    g_free(build); g_free(source); g_free(project); g_free(temporary);
    return result;
}
