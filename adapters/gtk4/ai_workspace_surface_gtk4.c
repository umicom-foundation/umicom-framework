/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ai_workspace_surface_gtk4.c
 *
 * PURPOSE:
 *   Open AI libraries explicitly and replace complete service lifetimes safely.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/ai_workspace.h"
#include "umicom/ui/gtk4/workspace_library.h"
#include "umicom/platform/threading.h"
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
    /* A failed candidate cleanup must retain its borrowed service chain too. */
    AiLibrarySession *pending;
    UmiAiLocalTransport *transport;
    uint64_t ownerThread;
    bool closed;
    bool retirementFailed;
    char *applicationId;
};

/* Release the replaceable library through checked ownership boundaries instead of advancing after an unreportable panel teardown. The former implementation is retained for owner review. */
#if 0
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


#endif
static UmiStatus SessionRelease(AiLibrarySession **in_out_session)
{
    AiLibrarySession *session = *in_out_session;
    if (session == NULL) return UMI_STATUS_OK;
    /* Each checked release must succeed before freeing its borrowed services.
     * A later failure retains the remaining chain at this stable address. */
    UmiStatus status = UmiAiWorkspaceGtkPanelRelease(&session->panel);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiWorkspaceRelease(&session->workspace);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiRuntimeRelease(&session->runtime);
    if (status != UMI_STATUS_OK) return status;
    UmiAiWorkspaceCancellationDestroy(session->cancellation);
    umi_data_server_destroy(session->data);
    g_free(session->path);
    g_free(session);
    *in_out_session = NULL;
    return UMI_STATUS_OK;
}

static UmiStatus AddProvider(UmiAiRuntime *runtime, UmiAiProvider *provider)
{
    UmiStatus status = umi_ai_provider_registry_add(&runtime->providers, provider);
    /* A failed registry insertion does not take ownership of this instance. */
    if (status != UMI_STATUS_OK && provider->destroy != NULL)
        provider->destroy(provider->instance);
    return status;
}

/* Pass the startup transport into each complete library lifetime and retain an unsuccessful candidate if checked cleanup refuses. The former implementation is retained for owner review. */
#if 0
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


#endif
static UmiStatus SessionOpen(const char *applicationId, const char *path, bool create,
    UmiAiLocalTransport *transport, AiLibrarySession **outSession)
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
    if (status == UMI_STATUS_OK && transport != NULL)
        status = UmiAiWorkspaceGtkPanelEnableLocalStreaming(candidate->panel, transport);
    if (status != UMI_STATUS_OK) {
        UmiStatus release = SessionRelease(&candidate);
        if (release != UMI_STATUS_OK) {
            *outSession = candidate; /* Internal caller retains failed cleanup. */
            return release;
        }
        return status;
    }
    candidate->path = g_strdup(path);
    *outSession = candidate;
    return UMI_STATUS_OK;
}

/* Retain both service chains if library retirement fails; do not free borrowed runtime or replace controls beneath an active native owner. The former implementation is retained for owner review. */
#if 0
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


#endif
UmiStatus UmiAiWorkspaceGtkSurfaceOpenLibrary(UmiAiWorkspaceGtkSurface *surface, const char *path, bool create)
{
    if (surface == NULL || path == NULL || !g_path_is_absolute(path)) return UMI_STATUS_INVALID_ARGUMENT;
    if (surface->ownerThread != umi_thread_current_id() || surface->closed) return UMI_STATUS_INVALID_STATE;
    if (surface->retirementFailed) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = SessionRelease(&surface->pending);
    if (status != UMI_STATUS_OK) return status;
    char *canonical = g_canonicalize_filename(path, NULL);
    if (surface->session != NULL && surface->session->path != NULL &&
        strcmp(canonical, surface->session->path) == 0) {
        g_free(canonical); return create ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_OK;
    }
    if (surface->session != NULL && UmiAiWorkspaceGtkPanelBusy(surface->session->panel)) {
        g_free(canonical); return UMI_STATUS_BUSY;
    }
    AiLibrarySession *candidate = NULL;
    status = SessionOpen(surface->applicationId, canonical, create, surface->transport, &candidate);
    g_free(canonical);
    if (status != UMI_STATUS_OK) {
        surface->pending = candidate;
        return status;
    }
    /* Opening and validating the candidate still precedes retirement. Missing
     * or corrupt input leaves the existing forms intact. Native teardown failure
     * is different: retain the retired old chain and the unopened candidate. */
    if (surface->session != NULL) {
        GtkWidget *old_widget = UmiAiWorkspaceGtkPanelWidget(surface->session->panel);
        if (old_widget != NULL) g_object_ref(old_widget);
        status = SessionRelease(&surface->session);
/* A failed retirement can leave the old panel closed; report recovery-required state instead of a successful same-path reopen. The former implementation is retained for owner review. */
#if 0
        if (status != UMI_STATUS_OK) {
            surface->pending = candidate;
            if (old_widget != NULL) g_object_unref(old_widget);
            return status;
        }
#endif
        if (status != UMI_STATUS_OK) {
            surface->pending = candidate;
            /* Retirement may already have disconnected the panel or released
             * part of its service chain. Refuse every reopen, including a
             * same-path no-op, until the caller completes checked shutdown. */
            surface->retirementFailed = true;
            if (old_widget != NULL) g_object_unref(old_widget);
            return status;
        }
        if (old_widget != NULL) {
            gtk_stack_remove(surface->pages, old_widget);
            g_object_unref(old_widget);
        }
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
    surface->ownerThread = umi_thread_current_id();
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
    /* Recent locations belong in a file selected by the user. Keep the former
     * profile-directory default for review; the shared library selector now
     * offers a visible opt-in. An explicit environment setting remains supported. */
#if 0
    /* Preferences are separate from the user-selected database. Hashing the
     * application identifier keeps arbitrary identifiers out of path segments. */
    char *profileKey = g_compute_checksum_for_string(G_CHECKSUM_SHA256, applicationId, -1);
    const char *historyOverride = g_getenv("UMICOM_LIBRARY_HISTORY_FILE");
    char *historyPath = historyOverride != NULL && historyOverride[0] != '\0' ? g_strdup(historyOverride) : g_build_filename(g_get_user_config_dir(), "umicom", "library-history", profileKey, "ai-libraries.txt", NULL);
    (void)UmiWorkspaceLibraryGtkEnableHistory(surface->library, historyPath);
    g_free(historyPath); g_free(profileKey);
#endif
    const char *historyOverride = g_getenv("UMICOM_LIBRARY_HISTORY_FILE");
    if (historyOverride != NULL && historyOverride[0] != '\0')
        (void)UmiWorkspaceLibraryGtkEnableHistory(surface->library, historyOverride);
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
/* Checked surface retirement keeps all remaining library owners alive if a child refuses native release; legacy destruction remains available. The former implementation is retained for owner review. */
#if 0
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

#endif
UmiStatus UmiAiWorkspaceGtkSurfaceEnableLocalStreaming(UmiAiWorkspaceGtkSurface *surface,
    UmiAiLocalTransport *transport)
{
    if (surface == NULL || transport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (surface->ownerThread != umi_thread_current_id() || surface->closed) return UMI_STATUS_INVALID_STATE;
    if (surface->session != NULL || surface->pending != NULL) return UMI_STATUS_BUSY;
    size_t requests = 0U;
    UmiStatus status = UmiAiLocalTransportRequestCount(transport, &requests);
    if (status != UMI_STATUS_OK) return status;
    surface->transport = transport;
    return UMI_STATUS_OK;
}
UmiStatus UmiAiWorkspaceGtkSurfaceRelease(UmiAiWorkspaceGtkSurface **in_out_surface)
{
    if (in_out_surface == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiWorkspaceGtkSurface *surface = *in_out_surface;
    if (surface == NULL) return UMI_STATUS_OK;
    if (surface->ownerThread != umi_thread_current_id()) return UMI_STATUS_INVALID_STATE;
    if (!surface->closed) {
        surface->closed = true;
        UmiWorkspaceLibraryGtkDestroy(surface->library);
        surface->library = NULL;
        gtk_widget_set_sensitive(surface->root, FALSE);
    }
    UmiStatus status = SessionRelease(&surface->session);
    if (status != UMI_STATUS_OK) return status;
    status = SessionRelease(&surface->pending);
    if (status != UMI_STATUS_OK) return status;
    if (surface->original != NULL && gtk_widget_get_parent(surface->original) == GTK_WIDGET(surface->pages))
        gtk_stack_remove(surface->pages, surface->original);
    g_object_unref(surface->root);
    g_free(surface->applicationId);
    g_free(surface);
    *in_out_surface = NULL;
    return UMI_STATUS_OK;
}
void UmiAiWorkspaceGtkSurfaceDestroy(UmiAiWorkspaceGtkSurface *surface)
{
    (void)UmiAiWorkspaceGtkSurfaceRelease(&surface);
}
