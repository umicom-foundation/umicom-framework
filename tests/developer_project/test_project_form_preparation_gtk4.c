/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_project_form_preparation_gtk4.c
 * PURPOSE: Verify ownership before and during native project form presentation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/developer_dialog.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); failed = 1; goto cleanup; } } while (0)
typedef struct FormProbe {
    GtkWindow *parent;
    UmiGtk4DeveloperDialog *dialog;
    GPtrArray *windows;
    const char *mode;
    unsigned accepted, observed;
} FormProbe;
static UmiStatus Accepted(const UmiDeveloperProjectModel *model, const UmiBuildProfile *profile, void *context)
{
    (void)model; (void)profile; ++((FormProbe *)context)->accepted; return UMI_STATUS_OK;
}
static void Dispose(FormProbe *probe)
{
    UmiGtk4DeveloperDialog *dialog = probe->dialog;
    probe->dialog = NULL;
    UmiGtk4DeveloperDialogDestroy(dialog);
}
static void Observed(GObject *object, GParamSpec *property, gpointer context)
{
    FormProbe *probe = context;
    if (probe->observed != 0U) return;
    if (strcmp(property->name, "visible") == 0 && !gtk_widget_get_visible(GTK_WIDGET(object))) return;
    ++probe->observed;
    if (strstr(probe->mode, "owner-close") != NULL || strstr(probe->mode, "immediate-close") != NULL) Dispose(probe);
    else gtk_window_destroy(probe->parent);
}
static void Added(GListModel *model, guint position, guint removed, guint added, gpointer context)
{
    (void)removed;
    FormProbe *probe = context;
    for (guint i = 0U; i < added; ++i) {
        GtkWindow *window = g_list_model_get_item(model, position + i);
        if (window == NULL) continue;
        g_ptr_array_add(probe->windows, window);
        if (strstr(probe->mode, "create-close") != NULL && probe->observed == 0U) {
            ++probe->observed; gtk_window_destroy(probe->parent);
        } else if (strstr(probe->mode, "title-close") != NULL)
            g_signal_connect(window, "notify::title", G_CALLBACK(Observed), probe);
        else if (strstr(probe->mode, "present-") != NULL || strstr(probe->mode, "immediate-close") != NULL)
            g_signal_connect(window, "notify::visible", G_CALLBACK(Observed), probe);
    }
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *cases[] = {"project-open", "application-open", "project-preclosed", "application-preclosed",
        "project-create-close", "application-create-close", "project-title-close", "application-title-close",
        "project-present-owner-close", "application-present-owner-close", "project-present-parent-close",
        "application-present-parent-close", "project-retained", "application-retained",
        "project-immediate-close", "application-immediate-close"};
    int known = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) if (strcmp(argv[1], cases[i]) == 0) known = 1;
    if (!known) return 2;
    if (!gtk_init_check()) return 77;
    int failed = 0;
    FormProbe probe = {0}; probe.mode = argv[1];
    probe.parent = GTK_WINDOW(gtk_window_new()); g_object_ref(probe.parent);
    probe.windows = g_ptr_array_new_with_free_func(g_object_unref);
    GListModel *windows = gtk_window_get_toplevels();
    gulong handler = g_signal_connect(windows, "items-changed", G_CALLBACK(Added), &probe);
    const int application = strncmp(probe.mode, "application-", 12U) == 0;
    const int immediate = strstr(probe.mode, "immediate-close") != NULL;
    if (strstr(probe.mode, "preclosed") != NULL) gtk_window_destroy(probe.parent);
    UmiStatus status = immediate
        ? (application ? UmiGtk4ApplicationModuleDialogCreate(probe.parent, Accepted, &probe, &probe.dialog)
                       : UmiGtk4NewProjectDialogCreate(probe.parent, Accepted, &probe, &probe.dialog))
        : (application ? UmiGtk4ApplicationModuleDialogPrepare(probe.parent, Accepted, &probe, &probe.dialog)
                       : UmiGtk4NewProjectDialogPrepare(probe.parent, Accepted, &probe, &probe.dialog));
    if (strstr(probe.mode, "preclosed") != NULL) {
        CHECK(status == UMI_STATUS_INVALID_STATE && probe.dialog == NULL && probe.windows->len == 0U); goto cleanup;
    }
    if (strstr(probe.mode, "create-close") != NULL || strstr(probe.mode, "title-close") != NULL || immediate) {
        CHECK(status == UMI_STATUS_CANCELLED && probe.dialog == NULL && probe.observed == 1U); goto cleanup;
    }
    CHECK(status == UMI_STATUS_OK && probe.dialog != NULL && probe.windows->len == 1U);
    GtkWindow *form = g_ptr_array_index(probe.windows, 0U);
    CHECK(!gtk_widget_get_visible(GTK_WIDGET(form)));
    GtkWidget *accept = umi_gtk4_automation_find_tagged_widget(GTK_WIDGET(form), "developer.dialog.accept");
    CHECK(GTK_IS_BUTTON(accept));
    g_object_ref(accept);
    /* Hidden preparation is not consent to write files or adopt a workspace. */
    g_signal_emit_by_name(accept, "clicked");
    if (probe.accepted != 0U) { g_object_unref(accept); CHECK(probe.accepted == 0U); }
    status = UmiGtk4DeveloperDialogPresent(probe.dialog);
    if (strstr(probe.mode, "present-") != NULL) {
        g_object_unref(accept);
        CHECK(status == UMI_STATUS_CANCELLED && probe.observed == 1U && probe.accepted == 0U);
        if (strstr(probe.mode, "owner-close") != NULL) CHECK(probe.dialog == NULL);
        goto cleanup;
    }
    if (strstr(probe.mode, "retained") != NULL) {
        Dispose(&probe);
        g_signal_emit_by_name(accept, "clicked");
        g_object_unref(accept);
        CHECK(!UmiGtk4WindowIsOpen(form) && probe.accepted == 0U);
    } else {
        g_object_unref(accept);
        CHECK(status == UMI_STATUS_OK && gtk_widget_get_visible(GTK_WIDGET(form)));
    }
cleanup:
    g_signal_handler_disconnect(windows, handler);
    for (guint i = 0U; i < probe.windows->len; ++i)
        g_signal_handlers_disconnect_by_data(g_ptr_array_index(probe.windows, i), &probe);
    Dispose(&probe);
    gtk_window_destroy(probe.parent);
    g_object_unref(probe.parent);
    g_ptr_array_unref(probe.windows);
    return failed;
}
