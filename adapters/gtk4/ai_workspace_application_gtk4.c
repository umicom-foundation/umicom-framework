/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ai_workspace_application_gtk4.c
 *
 * PURPOSE:
 *   Attach one shared AI service to LLM, RAG and Creator while preserving the established product host and layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ui/gtk4/ai_workspace.h"
#include "umicom/application/suite_layout/gtk4_product_content_extension.h"
#include <glib/gstdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

typedef struct AiApplication {
    UmiDataServer *data; UmiAiRuntime runtime; UmiAiWorkspace *workspace;
    UmiAiWorkspaceCancellation *cancellation; UmiAiWorkspaceGtkPanel *panel;
    GtkWidget *wrapper, *original; GtkStack *pages;
    const char *applicationId; int failed;
} AiApplication;
static void Release(void *data)
{
    AiApplication *application = data;
    if (application->original != NULL && application->pages != NULL &&
        gtk_widget_get_parent(application->original) == GTK_WIDGET(application->pages))
        gtk_stack_remove(application->pages, application->original);
    application->original = NULL;
    UmiAiWorkspaceGtkPanelDestroy(application->panel); application->panel = NULL;
    UmiAiWorkspaceDestroy(application->workspace); application->workspace = NULL;
    umi_ai_runtime_destroy(&application->runtime);
    UmiAiWorkspaceCancellationDestroy(application->cancellation); application->cancellation = NULL;
    umi_data_server_destroy(application->data); application->data = NULL;
    if (application->wrapper != NULL) g_object_unref(application->wrapper);
    application->wrapper = NULL; application->pages = NULL;
}
static UmiStatus Build(GtkWindow *window, UmiApplicationProductGtk4Workstation *workstation,
    GtkWidget *original, void *data, GtkWidget **outContent)
{
    AiApplication *application = data; UmiStatus status = UMI_STATUS_OK;
    (void)window; (void)workstation; *outContent = NULL;
    const char *override = g_getenv("UMICOM_AI_WORKSPACE_DB");
    char *path = override != NULL && override[0] != '\0' ? g_strdup(override) :
        g_build_filename(g_get_user_data_dir(), "umicom", "ai-workspace", "workstation.sqlite", NULL);
    char *directory = NULL;
    if (!g_path_is_absolute(path)) status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK) { directory = g_path_get_dirname(path); if (g_mkdir_with_parents(directory, 0700) != 0) status = UMI_STATUS_IO_ERROR; }
    umi_ai_runtime_init(&application->runtime);
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceCancellationCreate(&application->cancellation);
    UmiAiProvider provider = {0};
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceExtractiveProviderCreate(&provider);
    if (status == UMI_STATUS_OK) status = umi_ai_provider_registry_add(&application->runtime.providers, &provider);
    unsigned long port = 8080UL;
    const char *portText = g_getenv("UMICOM_AI_WORKSPACE_PORT");
    if (portText != NULL && portText[0] != '\0') {
        char *end = NULL; errno = 0; port = strtoul(portText, &end, 10);
        if (errno != 0 || portText[0] < '0' || portText[0] > '9' || *end != '\0' || port < 1024UL || port > 65535UL) status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (status == UMI_STATUS_OK && UmiAiWorkspaceLocalProviderAvailable()) {
        status = UmiAiWorkspaceLocalProviderCreate((uint16_t)port, 30000U, application->cancellation, &provider);
        if (status == UMI_STATUS_OK) {
            status = umi_ai_provider_registry_add(&application->runtime.providers, &provider);
            if (status != UMI_STATUS_OK && provider.destroy != NULL) provider.destroy(provider.instance);
        }
    }
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceRegisterCapacityTool(&application->runtime);
    /* Only the known read-only limit query is registered here. Its allowlist
     * and the per-job human review remain separate from this host policy. */
    application->runtime.policy.allow_tools = 1;
    if (status == UMI_STATUS_OK) status = umi_data_server_create_sqlite(path, &application->data);
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceCreate(application->data, &application->runtime, "shared", &application->workspace);
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceAllowTool(application->workspace, "umicom.workspace.capacities");
    char *description = g_strdup_printf("Shared local workspace: %s\n%s | loopback port %lu | request timeout 30 seconds\n"
        "The extractive preview is not an LLM. No model is contacted just by opening this window.", path,
        UmiAiWorkspaceLocalProviderAvailable() ? "Local HTTP transport compiled" : "Local HTTP transport unavailable in this build", port);
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceGtkPanelCreate(application->workspace, &application->runtime,
        application->cancellation, description, application->applicationId, &application->panel);
    application->wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8); g_object_ref_sink(application->wrapper);
    application->pages = GTK_STACK(gtk_stack_new()); gtk_widget_set_vexpand(GTK_WIDGET(application->pages), TRUE);
    GtkWidget *switcher = gtk_stack_switcher_new(); gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher), application->pages);
    gtk_widget_set_halign(switcher, GTK_ALIGN_CENTER); gtk_box_append(GTK_BOX(application->wrapper), switcher);
    gtk_box_append(GTK_BOX(application->wrapper), GTK_WIDGET(application->pages));
    GtkWidget *workspacePage;
    if (status == UMI_STATUS_OK) workspacePage = UmiAiWorkspaceGtkPanelWidget(application->panel);
    else {
        char *message = g_strdup_printf("The AI workspace could not open (status %d).\n\n%s\n\nCheck the absolute database path, write permission, SQLite support and port configuration.\n"
            "No memory-only replacement or model request was started. Existing layouts remain available.", (int)status, description);
        workspacePage = gtk_label_new(message); gtk_label_set_wrap(GTK_LABEL(workspacePage), TRUE);
        gtk_label_set_selectable(GTK_LABEL(workspacePage), TRUE); g_free(message); application->failed = 1;
    }
    gtk_stack_add_titled(application->pages, workspacePage, "ai-workspace", "AI workspace");
    gtk_stack_add_titled(application->pages, original, "layouts", "Existing layouts");
    application->original = original; *outContent = application->wrapper;
    g_free(description); g_free(directory); g_free(path); return UMI_STATUS_OK;
}
int UmiAiWorkspaceGtkRun(const char *applicationId, const char *title, int argc, char **argv)
{
    AiApplication application = {0}; application.applicationId = applicationId;
    UmiApplicationProductGtk4WorkstationConfig config =
        umi_application_product_gtk4_workstation_config_default(applicationId, title, NULL, NULL);
    UmiApplicationProductGtk4ContentExtension extension = {Build, Release, &application};
    config.mode_badge = "Reviewed AI workspace";
    int result = umi_application_product_gtk4_run_with_content(&config, &extension, argc, argv);
    Release(&application); return result != 0 ? result : application.failed;
}
