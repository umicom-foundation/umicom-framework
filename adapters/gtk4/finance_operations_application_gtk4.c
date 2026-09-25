/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/finance_operations_application_gtk4.c
 *
 * PURPOSE:
 *   Attach local accounting and exchange operations to the established product host without replacing its startup or layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/finance_operations.h"
#include "umicom/finance_operations/input.h"
#include "umicom/application/suite_layout/gtk4_product_content_extension.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <string.h>

typedef struct FinanceApplicationContext {
    UmiDataServer *server;
    UmiFinanceOperations *operations;
    UmiFinanceOperationsGtkPanel *panel;
    GtkWidget *wrapper;
    GtkStack *pages;
    GtkWidget *original;
    int failed;
} FinanceApplicationContext;

static char *DatabasePath(void)
{
    const char *override = g_getenv("UMICOM_FINANCE_OPERATIONS_DB");
    if (override != NULL && override[0] != '\0') return g_strdup(override);
    return g_build_filename(g_get_user_data_dir(), "umicom", "finance-operations", "workstation.sqlite", NULL);
}
static void ReleaseContent(void *data)
{
    FinanceApplicationContext *context = data;
    if (context->original != NULL && context->pages != NULL &&
        gtk_widget_get_parent(context->original) == GTK_WIDGET(context->pages))
        gtk_stack_remove(context->pages, context->original);
    context->original = NULL;
    UmiFinanceOperationsGtkPanelDestroy(context->panel); context->panel = NULL;
    UmiFinanceOperationsDestroy(context->operations); context->operations = NULL;
    umi_data_server_destroy(context->server); context->server = NULL;
    if (context->wrapper != NULL) g_object_unref(context->wrapper);
    context->wrapper = NULL; context->pages = NULL;
}
static UmiStatus BuildContent(GtkWindow *window,
    UmiApplicationProductGtk4Workstation *workstation, GtkWidget *original,
    void *data, GtkWidget **outContent)
{
    FinanceApplicationContext *context = data;
    GtkWidget *switcher, *operationsPage;
    char *path = DatabasePath(), *directory = NULL, *description;
    UmiStatus status = UMI_STATUS_OK;
    (void)window; (void)workstation;
    *outContent = NULL;
    if (!g_path_is_absolute(path)) status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK) {
        directory = g_path_get_dirname(path);
        if (g_mkdir_with_parents(directory, 0700) != 0) status = UMI_STATUS_IO_ERROR;
    }
    if (status == UMI_STATUS_OK) status = umi_data_server_create_sqlite(path, &context->server);
    if (status == UMI_STATUS_OK) status = UmiFinanceOperationsCreate(context->server, &context->operations);
    description = g_strdup_printf("Shared practice database: %s", path);
    if (status == UMI_STATUS_OK) status = UmiFinanceOperationsGtkPanelCreate(context->operations, description, &context->panel);
    context->wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8); g_object_ref_sink(context->wrapper);
    context->pages = GTK_STACK(gtk_stack_new()); gtk_widget_set_vexpand(GTK_WIDGET(context->pages), TRUE);
    switcher = gtk_stack_switcher_new(); gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher), context->pages);
    gtk_widget_set_halign(switcher, GTK_ALIGN_CENTER); gtk_box_append(GTK_BOX(context->wrapper), switcher);
    gtk_box_append(GTK_BOX(context->wrapper), GTK_WIDGET(context->pages));
    if (status == UMI_STATUS_OK) operationsPage = UmiFinanceOperationsGtkPanelWidget(context->panel);
    else {
        char *message = g_strdup_printf("The practice book could not open.\n\n%s\n\n%s\n\nNo memory-only substitute was opened. Your existing layouts are still available.",
            description, UmiFinanceOperationsExplainStatus(status));
        context->failed = 1; operationsPage = gtk_label_new(message); gtk_label_set_wrap(GTK_LABEL(operationsPage), TRUE);
        gtk_label_set_selectable(GTK_LABEL(operationsPage), TRUE); g_free(message);
    }
    gtk_stack_add_titled(context->pages, operationsPage, "operations", "Operations");
    gtk_stack_add_titled(context->pages, original, "layouts", "Existing layouts");
    context->original = original;
    gtk_stack_set_visible_child_name(context->pages, "operations");
    *outContent = context->wrapper;
    g_free(description); g_free(directory); g_free(path);
    return UMI_STATUS_OK;
}
int UmiFinanceOperationsGtkRun(const char *applicationId, const char *title, int argc, char **argv)
{
    FinanceApplicationContext context = {0};
    UmiApplicationProductGtk4WorkstationConfig config =
        umi_application_product_gtk4_workstation_config_default(applicationId, title, NULL, NULL);
    UmiApplicationProductGtk4ContentExtension extension = {BuildContent, ReleaseContent, &context};
    int result;
    config.mode_badge = "Local financial practice";
    result = umi_application_product_gtk4_run_with_content(&config, &extension, argc, argv);
    ReleaseContent(&context);
    return result != 0 ? result : context.failed;
}
