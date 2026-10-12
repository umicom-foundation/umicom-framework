/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ai_workspace_surface_gtk4.c
 * PURPOSE: Open AI libraries explicitly and replace complete service lifetimes safely.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/ai_workspace.h"
#include "umicom/ui/gtk4/workspace_library.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/* Services stay at a stable address: a workspace borrows its runtime and a
 * panel borrows all four services. Never copy this structure after opening it. */
typedef struct AiLibrarySession {
    UmiDataServer *data;
    UmiAiRuntime runtime;
    UmiAiWorkspace *workspace;
    UmiAiWorkspaceCancellation *cancellation;
    UmiAiWorkspaceGtkPanel *panel;
    char *path;
} AiLibrarySession;

struct UmiAiWorkspaceGtkSurface {
    GtkWidget *root, *original;
    GtkStack *pages;
    UmiWorkspaceLibraryGtk *library;
    AiLibrarySession *session;
    char *applicationId;
};

static void SessionDestroy(AiLibrarySession *session)
{
    if (session == NULL) return;
    /* Retire callbacks and join work before freeing any borrowed service. */
    UmiAiWorkspaceGtkPanelDestroy(session->panel);
    UmiAiWorkspaceDestroy(session->workspace);
    umi_ai_runtime_destroy(&session->runtime);
    UmiAiWorkspaceCancellationDestroy(session->cancellation);
    umi_data_server_destroy(session->data);
    g_free(session->path);
    g_free(session);
}

static UmiStatus AddProvider(UmiAiRuntime *runtime, UmiAiProvider *provider)
{
    UmiStatus status = umi_ai_provider_registry_add(&runtime->providers, provider);
    /* A failed registry insertion does not take ownership of this instance. */
    if (status != UMI_STATUS_OK && provider->destroy != NULL)
        provider->destroy(provider->instance);
    return status;
}

static UmiStatus SessionOpen(const char *applicationId, const char *path, bool create,
    AiLibrarySession **outSession)
{
    *outSession = NULL;
    unsigned long port = 8080UL;
    const char *portText = g_getenv("UMICOM_AI_WORKSPACE_PORT");
    if (portText != NULL && portText[0] != '\0') {
        char *end = NULL; errno = 0; port = strtoul(portText, &end, 10);
        if (errno != 0 || portText[0] < '0' || portText[0] > '9' ||
            *end != '\0' || port < 1024UL || port > 65535UL)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    AiLibrarySession *candidate = g_new0(AiLibrarySession, 1);
    umi_ai_runtime_init(&candidate->runtime);
    UmiStatus status = UmiAiWorkspaceCancellationCreate(&candidate->cancellation);
    UmiAiProvider provider = {0};
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceExtractiveProviderCreate(&provider);
    if (status == UMI_STATUS_OK) status = AddProvider(&candidate->runtime, &provider);
    if (status == UMI_STATUS_OK && UmiAiWorkspaceLocalProviderAvailable()) {
        status = UmiAiWorkspaceLocalProviderCreate((uint16_t)port, 30000U, candidate->cancellation, &provider);
        if (status == UMI_STATUS_OK) status = AddProvider(&candidate->runtime, &provider);
    }
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceRegisterCapacityTool(&candidate->runtime);
    /* Retain the existing reviewed, read-only tool policy. Opening a library
     * registers providers but never starts a model request or downloads a model. */
    candidate->runtime.policy.allow_tools = 1;
    if (status == UMI_STATUS_OK) status = UmiWorkspaceLibraryOpenData(path, create, &candidate->data);
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceCreate(candidate->data, &candidate->runtime, "shared", &candidate->workspace);
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceAllowTool(candidate->workspace, "umicom.workspace.capacities");
    char *description = g_strdup_printf("Library: %s\n%s | loopback port %lu | request timeout 30 seconds\n"
        "The extractive preview is not an LLM. No model is contacted by opening a library.",
        path, UmiAiWorkspaceLocalProviderAvailable() ? "Local HTTP transport compiled" : "Local HTTP transport unavailable", port);
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceGtkPanelCreate(candidate->workspace, &candidate->runtime,
        candidate->cancellation, description, applicationId, &candidate->panel);
    g_free(description);
    if (status != UMI_STATUS_OK) { SessionDestroy(candidate); return status; }
    candidate->path = g_strdup(path);
    *outSession = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiWorkspaceGtkSurfaceOpenLibrary(UmiAiWorkspaceGtkSurface *surface, const char *path, bool create)
{
    if (surface == NULL || path == NULL || !g_path_is_absolute(path)) return UMI_STATUS_INVALID_ARGUMENT;
    char *canonical = g_canonicalize_filename(path, NULL);
    if (surface->session != NULL && strcmp(canonical, surface->session->path) == 0) {
        g_free(canonical); return create ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_OK;
    }
    if (surface->session != NULL && UmiAiWorkspaceGtkPanelBusy(surface->session->panel)) {
        g_free(canonical); return UMI_STATUS_BUSY;
    }
    AiLibrarySession *candidate = NULL;
    UmiStatus status = SessionOpen(surface->applicationId, canonical, create, &candidate);
    g_free(canonical);
    if (status != UMI_STATUS_OK) return status;
    /* Candidate-first replacement keeps the old data, widgets and unsaved forms
     * usable when a file is missing, corrupt or cannot be opened. */
    if (surface->session != NULL) {
        gtk_stack_remove(surface->pages, UmiAiWorkspaceGtkPanelWidget(surface->session->panel));
        SessionDestroy(surface->session);
    }
    surface->session = candidate;
    UmiWorkspaceLibraryGtkSetCurrentPath(surface->library, candidate->path);
    GtkWidget *widget = UmiAiWorkspaceGtkPanelWidget(candidate->panel);
    gtk_stack_add_titled(surface->pages, widget, "ai-workspace", "AI workspace");
    gtk_stack_set_visible_child(surface->pages, widget);
    return UMI_STATUS_OK;
}

static UmiStatus OpenLibrary(const char *path, bool create, void *context)
{
    return UmiAiWorkspaceGtkSurfaceOpenLibrary(context, path, create);
}

UmiStatus UmiAiWorkspaceGtkSurfaceCreate(GtkWidget *original, const char *applicationId, UmiAiWorkspaceGtkSurface **outSurface)
{
    if (outSurface == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outSurface = NULL;
    if (original == NULL || gtk_widget_get_parent(original) != NULL || applicationId == NULL || applicationId[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiWorkspaceGtkSurface *surface = g_new0(UmiAiWorkspaceGtkSurface, 1);
    surface->applicationId = g_strdup(applicationId);
    surface->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8); g_object_ref_sink(surface->root);
    surface->pages = GTK_STACK(gtk_stack_new());
    gtk_widget_set_vexpand(GTK_WIDGET(surface->pages), TRUE);
    GtkWidget *switcher = gtk_stack_switcher_new();
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher), surface->pages);
    gtk_widget_set_halign(switcher, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(surface->root), switcher);
    gtk_box_append(GTK_BOX(surface->root), GTK_WIDGET(surface->pages));
    UmiStatus status = UmiWorkspaceLibraryGtkCreate(OpenLibrary, surface, &surface->library);
    if (status != UMI_STATUS_OK) { UmiAiWorkspaceGtkSurfaceDestroy(surface); return status; }
    /* Preferences are separate from the user-selected database. Hashing the
     * application identifier keeps arbitrary identifiers out of path segments. */
    char *profileKey = g_compute_checksum_for_string(G_CHECKSUM_SHA256, applicationId, -1);
    const char *historyOverride = g_getenv("UMICOM_LIBRARY_HISTORY_FILE");
    char *historyPath = historyOverride != NULL && historyOverride[0] != '\0' ? g_strdup(historyOverride) : g_build_filename(g_get_user_config_dir(), "umicom", "library-history", profileKey, "ai-libraries.txt", NULL);
    (void)UmiWorkspaceLibraryGtkEnableHistory(surface->library, historyPath);
    g_free(historyPath); g_free(profileKey);
    GtkWidget *libraryPage = UmiWorkspaceLibraryGtkWidget(surface->library);
    gtk_stack_add_titled(surface->pages, libraryPage, "library", "Library location");
    /* Keep environment-based workflows available as a visible suggestion.
     * An old default file is offered only if it already exists, never created. */
    const char *override = g_getenv("UMICOM_AI_WORKSPACE_DB");
    char *previous = g_build_filename(g_get_user_data_dir(), "umicom", "ai-workspace", "workstation.sqlite", NULL);
    if (override != NULL && override[0] != '\0') UmiWorkspaceLibraryGtkSetPath(surface->library, override);
    else if (g_file_test(previous, G_FILE_TEST_IS_REGULAR)) UmiWorkspaceLibraryGtkSetPath(surface->library, previous);
    g_free(previous);
    gtk_stack_add_titled(surface->pages, original, "layouts", "Existing layouts");
    surface->original = original;
    gtk_stack_set_visible_child(surface->pages, libraryPage);
    *outSurface = surface;
    return UMI_STATUS_OK;
}

GtkWidget *UmiAiWorkspaceGtkSurfaceWidget(UmiAiWorkspaceGtkSurface *surface)
{
    return surface != NULL ? surface->root : NULL;
}
void UmiAiWorkspaceGtkSurfaceDestroy(UmiAiWorkspaceGtkSurface *surface)
{
    if (surface == NULL) return;
    UmiWorkspaceLibraryGtkDestroy(surface->library);
    SessionDestroy(surface->session);
    /* The product owns the original layout. Detach it before destroying the
     * extension, exactly as the previous host did for its layout page. */
    if (surface->original != NULL && gtk_widget_get_parent(surface->original) == GTK_WIDGET(surface->pages))
        gtk_stack_remove(surface->pages, surface->original);
    g_object_unref(surface->root);
    g_free(surface->applicationId); g_free(surface);
}
