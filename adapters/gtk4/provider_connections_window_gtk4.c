/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/provider_connections_window_gtk4.c
 *
 * PURPOSE:
 *   Share provider windows across legacy paths and user-selected connection stores.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/provider_connections/gtk4.h"
#include "umicom/platform/resource_location.h"
#include "umicom/provider_connections/chat.h"
#include "umicom/security/secrets.h"
#include <stdlib.h>
#include "provider_chat_context_internal.h"

static void ReleasePanel(gpointer data)
{
    UmiProviderConnectionsGtkDestroy(data);
}
static void RetirePanel(GtkWidget *window, gpointer data)
{
    (void)data;
    /* Unrealizing a presented window retires callbacks even when a test or
     * another component retains the GtkWindow after gtk_window_destroy. The
     * data destructor also covers finalization before the first realization. */
    g_object_set_data(G_OBJECT(window), "umicom-provider-panel", NULL);
}
static gboolean CloseRequested(GtkWindow *window, gpointer data)
{
    (void)data;
    UmiProviderConnectionsGtk *panel = g_object_get_data(G_OBJECT(window), "umicom-provider-panel");
    return !UmiProviderConnectionsGtkCanClose(panel);
}
/* Window construction now shares one context-aware path. The ordinary settings window still starts empty; selected-code callers supply an owned copy without extending editor lifetime.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiProviderConnectionsGtkPresent(GtkWindow *parent,
    const char *application_id, const char *profile_id)
{
    if (parent != NULL && !GTK_IS_WINDOW(parent)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiApplicationPaths paths;
    UmiApplicationPathsConfig request = UmiApplicationPathsConfigDefault(application_id);
    UmiStatus status = UmiApplicationPathsResolve(&request, &paths);
    char path[UMI_PATH_CAPACITY];
    if (status == UMI_STATUS_OK) status = umi_path_join(paths.data, "provider-connections.sqlite3", path, sizeof(path));
    if (status != UMI_STATUS_OK) return status;
    UmiProviderConnectionsGtkConfig config = {application_id, profile_id, path};
    UmiProviderConnectionsGtk *panel = NULL;
    status = UmiProviderConnectionsGtkCreate(&config, &panel);
    if (status != UMI_STATUS_OK) return status;
    GtkWidget *window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "Provider connections");
    gtk_window_set_default_size(GTK_WINDOW(window), 720, 780);
    if (parent != NULL) {
        gtk_window_set_transient_for(GTK_WINDOW(window), parent);
        gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
    }
    gtk_window_set_child(GTK_WINDOW(window), UmiProviderConnectionsGtkWidget(panel));
    g_object_set_data_full(G_OBJECT(window), "umicom-provider-panel", panel, ReleasePanel);
    g_signal_connect(window, "close-request", G_CALLBACK(CloseRequested), NULL);
    g_signal_connect(window, "unrealize", G_CALLBACK(RetirePanel), NULL);
    gtk_window_present(GTK_WINDOW(window));
    return UMI_STATUS_OK;
}
#endif
/* Explicit-path hosts share the same window and teardown policy. The
 * former path-resolving construction is retained below for comparison. */
#if 0
static UmiStatus PresentContext(GtkWindow *parent,
    const char *application_id, const char *profile_id, const char *context)
{
    if (parent != NULL && !GTK_IS_WINDOW(parent)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiApplicationPaths paths;
    UmiApplicationPathsConfig request = UmiApplicationPathsConfigDefault(application_id);
    UmiStatus status = UmiApplicationPathsResolve(&request, &paths);
    char path[UMI_PATH_CAPACITY];
    if (status == UMI_STATUS_OK) status = umi_path_join(paths.data, "provider-connections.sqlite3", path, sizeof(path));
    if (status != UMI_STATUS_OK) return status;
    UmiProviderConnectionsGtkConfig config = {application_id, profile_id, path};
    UmiProviderConnectionsGtk *panel = NULL;
    status = UmiProviderConnectionsGtkCreate(&config, &panel);
    if (status != UMI_STATUS_OK) return status;
    status = UmiProviderConnectionsGtkSetChatContext(panel, context);
    if (status != UMI_STATUS_OK) { UmiProviderConnectionsGtkDestroy(panel); return status; }
    GtkWidget *window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "Provider connections");
    gtk_window_set_default_size(GTK_WINDOW(window), 720, 780);
    if (parent != NULL) {
        gtk_window_set_transient_for(GTK_WINDOW(window), parent);
        gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
    }
    gtk_window_set_child(GTK_WINDOW(window), UmiProviderConnectionsGtkWidget(panel));
    g_object_set_data_full(G_OBJECT(window), "umicom-provider-panel", panel, ReleasePanel);
    g_signal_connect(window, "close-request", G_CALLBACK(CloseRequested), NULL);
/* Retire controllers after GTK unrealizes the child tree so text input contexts remain alive through native surface teardown. The former implementation is retained for review. */
#if 0
    g_signal_connect(window, "unrealize", G_CALLBACK(RetirePanel), NULL);
#endif
    g_signal_connect_after(window, "unrealize", G_CALLBACK(RetirePanel), NULL);
    gtk_window_present(GTK_WINDOW(window));
    return UMI_STATUS_OK;
}
#endif
UmiStatus UmiProviderConnectionsGtkPresentAt(GtkWindow *parent,
    const UmiProviderConnectionsGtkConfig *config, const char *context)
{
    if (config == NULL || (parent != NULL && !GTK_IS_WINDOW(parent)))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus valid = UmiProviderChatGtkContextValidate(context);
    if (valid != UMI_STATUS_OK) return valid;
    UmiProviderConnectionsGtk *panel = NULL;
    UmiStatus status = UmiProviderConnectionsGtkCreate(config, &panel);
    if (status != UMI_STATUS_OK) return status;
    status = UmiProviderConnectionsGtkSetChatContext(panel, context);
    if (status != UMI_STATUS_OK) { UmiProviderConnectionsGtkDestroy(panel); return status; }
    GtkWidget *window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "Provider connections");
    gtk_window_set_default_size(GTK_WINDOW(window), 720, 780);
    if (parent != NULL) {
        gtk_window_set_transient_for(GTK_WINDOW(window), parent);
        gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
    }
    gtk_window_set_child(GTK_WINDOW(window), UmiProviderConnectionsGtkWidget(panel));
    g_object_set_data_full(G_OBJECT(window), "umicom-provider-panel", panel, ReleasePanel);
    g_signal_connect(window, "close-request", G_CALLBACK(CloseRequested), NULL);
/* Retire controllers after GTK unrealizes the child tree so text input contexts remain alive through native surface teardown. The former implementation is retained for review. */
#if 0
    g_signal_connect(window, "unrealize", G_CALLBACK(RetirePanel), NULL);
#endif
    g_signal_connect_after(window, "unrealize", G_CALLBACK(RetirePanel), NULL);
    gtk_window_present(GTK_WINDOW(window));
    return UMI_STATUS_OK;
}
static UmiStatus PresentContext(GtkWindow *parent,
    const char *application_id, const char *profile_id, const char *context)
{
    /* Keep legacy admission ahead of path resolution; an invalid parent must
     * not trigger setup of a settings location. PresentAt checks it again for
     * callers which supply an explicit path. */
    if (parent != NULL && !GTK_IS_WINDOW(parent)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiApplicationPaths paths;
    UmiApplicationPathsConfig request = UmiApplicationPathsConfigDefault(application_id);
    UmiStatus status = UmiApplicationPathsResolve(&request, &paths);
    char path[UMI_PATH_CAPACITY];
    if (status == UMI_STATUS_OK)
        status = umi_path_join(paths.data, "provider-connections.sqlite3", path, sizeof(path));
    if (status != UMI_STATUS_OK) return status;
    UmiProviderConnectionsGtkConfig config = {application_id, profile_id, path};
    return UmiProviderConnectionsGtkPresentAt(parent, &config, context);
}

UmiStatus UmiProviderConnectionsGtkPresent(GtkWindow *parent,
    const char *application_id, const char *profile_id)
{
    return PresentContext(parent, application_id, profile_id, "");
}
UmiStatus UmiProviderConnectionsGtkPresentSelection(GtkWindow *parent,
    const char *application_id, const char *profile_id,
    const UmiUiDocumentViewModel *documents, const char *view_id)
{
    if (documents == NULL || view_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    char *text = calloc(UMI_PROVIDER_CHAT_CONTEXT_CAPACITY, 1U);
    if (text == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiUiDocumentSelectionInfo info;
    /* Capture at the explicit action, not while opening or polling AI panels.
     * Only the selected bytes enter context; identity/revision stay local. */
    UmiStatus status = UmiUiDocumentViewModelCopySelection(documents, view_id,
        umi_ui_document_view_model_revision(documents), text, UMI_PROVIDER_CHAT_CONTEXT_CAPACITY, &info);
    if (status == UMI_STATUS_OK) status = PresentContext(parent, application_id, profile_id, text);
    umi_secret_clear(text, UMI_PROVIDER_CHAT_CONTEXT_CAPACITY); free(text);
    return status;
}

