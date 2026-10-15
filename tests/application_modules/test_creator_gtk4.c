/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_modules/test_creator_gtk4.c
 * PURPOSE: Exercise reviewed creation, stale drafts and native form ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ui/gtk4/developer_dialog.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #value); failed = 1; goto cleanup; \
} } while (0)

typedef struct Probe {
    UmiGtk4DeveloperDialog *dialog;
    GtkWidget *accept;
    unsigned calls;
    int destroy, retry, recursive, rootGone, valid;
    GtkWidget *form;
    unsigned notifications;
} Probe;

static void Dispose(Probe *probe)
{
    UmiGtk4DeveloperDialog *dialog = probe->dialog;
    probe->dialog = NULL;
    UmiGtk4DeveloperDialogDestroy(dialog);
}

/* Acceptance may retire its own form. Borrowed results must remain valid until
 * it returns; recursive accept signals must never adopt a second project. */
static UmiStatus Created(const UmiDeveloperProjectModel *model,
    const UmiBuildProfile *profile, void *context)
{
    Probe *probe = context;
    ++probe->calls;
    if (probe->recursive) g_signal_emit_by_name(probe->accept, "clicked");
    if (probe->destroy) Dispose(probe);
    probe->valid = model->generated && !model->trusted &&
        umi_path_equal(model->root, profile->source_directory) &&
        profile->run_program[0] == '\0';
    return probe->retry && probe->calls == 1U ? UMI_STATUS_BUSY : UMI_STATUS_OK;
}

static void RootGone(gpointer context, GObject *object)
{
    (void)object;
    ((Probe *)context)->rootGone = 1;
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

/* Bounded event pumping waits for observable completion rather than assuming a
 * particular disk speed. The tests do not run any generated program or compiler. */
static int Await(GtkWidget *widget, const char *label, Probe *probe)
{
    gint64 end = g_get_monotonic_time() + 10000000;
    do {
        while (g_main_context_iteration(NULL, FALSE)) {}
        if (probe != NULL && probe->rootGone) return 1;
        if (widget != NULL && label == NULL && gtk_widget_get_sensitive(widget)) return 1;
        if (widget != NULL && label != NULL &&
            g_str_has_prefix(gtk_label_get_text(GTK_LABEL(widget)), label)) return 1;
        g_usleep(1000U);
    } while (g_get_monotonic_time() < end);
    return 0;
}
static void Edit(GtkWidget *form, const char *id, const char *text)
{
    GtkWidget *entry = umi_gtk4_automation_find_tagged_widget(form, id);
    if (GTK_IS_EDITABLE(entry)) gtk_editable_set_text(GTK_EDITABLE(entry), text);
}

/* A host may respond to the preview model's notification by editing its draft.
 * That synchronous callback must invalidate the just-published source safely. */
static void PreviewModelChanged(GObject *object, GParamSpec *property, gpointer context)
{
    (void)object; (void)property;
    Probe *probe = context;
    if (++probe->notifications == 1U)
        Edit(probe->form, "developer.application.name", "Edited during publication");
}

int main(int argc, char **argv)
{
    int failed = 0;
    Probe probe = {0};
    GtkWindow *parent = NULL, *form = NULL;
    GtkWidget *preview = NULL, *status = NULL;
    char output[UMI_PATH_CAPACITY], templates[UMI_PATH_CAPACITY], target[UMI_PATH_CAPACITY];
    CHECK(argc == 3);
    if (!gtk_init_check()) return 77;
    CHECK(umi_path_join(argv[2], "output", output, sizeof output) == UMI_STATUS_OK);
    CHECK(umi_path_join(argv[2], "templates", templates, sizeof templates) == UMI_STATUS_OK);
    CHECK(umi_path_join(output, "weather", target, sizeof target) == UMI_STATUS_OK);
    if (strcmp(argv[1], "invalid_utf8") == 0) {
        char source[UMI_PATH_CAPACITY];
        const unsigned char invalid[] = {0xffU};
        CHECK(umi_path_join(templates, "README.md.in", source, sizeof source) == UMI_STATUS_OK);
        CHECK(umi_fs_write_bytes(source, invalid, sizeof invalid) == UMI_STATUS_OK);
    }
    parent = GTK_WINDOW(gtk_window_new()); g_object_ref(parent);
    CHECK(UmiGtk4ApplicationModuleDialogCreate(parent, Created, &probe, &probe.dialog) == UMI_STATUS_OK);
    form = FindForm(parent); CHECK(form != NULL);
    probe.form = GTK_WIDGET(form);
    g_object_weak_ref(G_OBJECT(gtk_window_get_child(form)), RootGone, &probe);
    preview = umi_gtk4_automation_find_tagged_widget(GTK_WIDGET(form), "developer.application.preview");
    probe.accept = umi_gtk4_automation_find_tagged_widget(GTK_WIDGET(form), "developer.dialog.accept");
    status = umi_gtk4_automation_find_tagged_widget(GTK_WIDGET(form), "developer.application.status");
    if (preview != NULL) g_object_ref(preview);
    if (probe.accept != NULL) g_object_ref(probe.accept);
    CHECK(GTK_IS_BUTTON(preview) && GTK_IS_BUTTON(probe.accept) && GTK_IS_LABEL(status));
    Edit(GTK_WIDGET(form), "developer.application.templates", templates);
    Edit(GTK_WIDGET(form), "developer.application.parent", output);
    Edit(GTK_WIDGET(form), "developer.application.slug", "weather");
    Edit(GTK_WIDGET(form), "developer.application.name", "Weather");
    Edit(GTK_WIDGET(form), "developer.application.id", "org.example.weather");
    CHECK(!gtk_widget_get_sensitive(probe.accept));
    /* Even a directly emitted signal cannot create before source review. */
    g_signal_emit_by_name(probe.accept, "clicked");
    CHECK(!umi_fs_exists(target));
    if (strcmp(argv[1], "notify_edit") == 0) {
        GtkWidget *files = umi_gtk4_automation_find_tagged_widget(GTK_WIDGET(form), "developer.application.files");
        CHECK(GTK_IS_DROP_DOWN(files));
        g_signal_connect(files, "notify::model", G_CALLBACK(PreviewModelChanged), &probe);
    }
    g_signal_emit_by_name(preview, "clicked");

    if (strcmp(argv[1], "retained") == 0 || strcmp(argv[1], "parent_close") == 0) {
        if (strcmp(argv[1], "parent_close") == 0) gtk_window_destroy(parent);
        Dispose(&probe);
        g_signal_emit_by_name(preview, "clicked");
        g_signal_emit_by_name(probe.accept, "clicked");
        CHECK(Await(NULL, NULL, &probe));
        CHECK(!umi_fs_exists(target) && probe.calls == 0U);
        goto cleanup;
    }
    if (strcmp(argv[1], "pending_edit") == 0 || strcmp(argv[1], "pending_edit_back") == 0) {
        Edit(GTK_WIDGET(form), "developer.application.name", "Different");
        if (strcmp(argv[1], "pending_edit_back") == 0)
            Edit(GTK_WIDGET(form), "developer.application.name", "Weather");
        CHECK(Await(preview, NULL, NULL));
        CHECK(!gtk_widget_get_sensitive(probe.accept));
        g_signal_emit_by_name(probe.accept, "clicked");
        CHECK(!umi_fs_exists(target));
        goto cleanup;
    }
    if (strcmp(argv[1], "hide") == 0) {
        gtk_widget_set_visible(GTK_WIDGET(form), FALSE);
        g_signal_emit_by_name(probe.accept, "clicked");
        Dispose(&probe);
        CHECK(Await(NULL, NULL, &probe));
        CHECK(!umi_fs_exists(target));
        goto cleanup;
    }
    CHECK(Await(preview, NULL, NULL));
    if (strcmp(argv[1], "invalid_utf8") == 0 || strcmp(argv[1], "notify_edit") == 0) {
        CHECK(!gtk_widget_get_sensitive(probe.accept));
        g_signal_emit_by_name(probe.accept, "clicked");
        CHECK(!umi_fs_exists(target) && probe.calls == 0U);
        goto cleanup;
    }
    CHECK(gtk_widget_get_sensitive(probe.accept));
    GtkWidget *files = umi_gtk4_automation_find_tagged_widget(GTK_WIDGET(form), "developer.application.files");
    CHECK(GTK_IS_DROP_DOWN(files));
    CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(files))) == 11U);
    CHECK(!umi_fs_exists(target));
    if (strcmp(argv[1], "preview") == 0) goto cleanup;
    if (strcmp(argv[1], "edit_after") == 0) {
        Edit(GTK_WIDGET(form), "developer.application.slug", "different");
        CHECK(!gtk_widget_get_sensitive(probe.accept));
        g_signal_emit_by_name(probe.accept, "clicked");
        CHECK(!umi_fs_exists(target));
        goto cleanup;
    }
    if (strcmp(argv[1], "hide_failure") == 0)
        CHECK(umi_fs_make_directories(target) == UMI_STATUS_OK);
    if (strcmp(argv[1], "collision") == 0)
        CHECK(umi_fs_make_directories(target) == UMI_STATUS_OK);
    g_signal_emit_by_name(probe.accept, "clicked");
    if (strcmp(argv[1], "hide_apply") == 0 || strcmp(argv[1], "hide_failure") == 0) {
        /* The completion is observed while hidden. Reopening must retain its
         * outcome, but a retained hidden accept signal must not adopt a project. */
        gtk_widget_set_visible(GTK_WIDGET(form), FALSE);
        CHECK(Await(status, strcmp(argv[1], "hide_failure") == 0
            ? "Creation stopped:" : "Source files created.", NULL));
        g_signal_emit_by_name(probe.accept, "clicked");
        CHECK(probe.calls == 0U);
        gtk_window_present(form);
        if (strcmp(argv[1], "hide_failure") == 0) {
            CHECK(!gtk_widget_get_sensitive(probe.accept));
            goto cleanup;
        }
    }
    if (strcmp(argv[1], "collision") == 0) {
        CHECK(Await(status, "Creation stopped:", NULL));
        CHECK(!gtk_widget_get_sensitive(probe.accept));
        CHECK(probe.calls == 0U);
        goto cleanup;
    }
    CHECK(Await(status, "Source files created.", NULL));
    CHECK(probe.calls == 0U);
    CHECK(umi_fs_is_directory(target));
    CHECK(strcmp(gtk_button_get_label(GTK_BUTTON(probe.accept)), "Open Project") == 0);
    probe.destroy = strcmp(argv[1], "adopt_destroy") == 0;
    probe.retry = strcmp(argv[1], "adopt_retry") == 0;
    probe.recursive = strcmp(argv[1], "adopt_recursive") == 0;
    g_signal_emit_by_name(probe.accept, "clicked");
    CHECK(probe.calls == 1U && probe.valid);
    if (probe.retry) {
        CHECK(gtk_widget_get_visible(GTK_WIDGET(form)));
        g_signal_emit_by_name(probe.accept, "clicked");
        CHECK(probe.calls == 2U && probe.valid);
    }
cleanup:
    Dispose(&probe);
    if (form != NULL) (void)Await(NULL, NULL, &probe);
    g_clear_object(&preview);
    g_clear_object(&probe.accept);
    g_clear_object(&form);
    if (parent != NULL) { gtk_window_destroy(parent); g_object_unref(parent); }
    return failed;
}
