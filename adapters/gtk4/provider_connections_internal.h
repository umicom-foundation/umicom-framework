/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/provider_connections_internal.h
 * PURPOSE: Keep connection-editor worker ownership separate from GTK state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_GTK4_PROVIDER_CONNECTIONS_INTERNAL_H
#define UMICOM_GTK4_PROVIDER_CONNECTIONS_INTERNAL_H
#include "umicom/provider_connections/gtk4.h"
#include "umicom/platform/path.h"
#include "umicom/provider_connections/check.h"
#include "umicom/provider_connections/chat_gtk4.h"
typedef enum UmiProviderEditorOperation {
    UMI_PROVIDER_EDITOR_LOAD,
    UMI_PROVIDER_EDITOR_SAVE,
    UMI_PROVIDER_EDITOR_REMOVE
} UmiProviderEditorOperation;
typedef struct UmiProviderEditorJob {
    UmiProviderEditorOperation operation;
    char path[UMI_PATH_CAPACITY];
    char application[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
    char profile[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
    UmiProviderConnection draft;
    uint64_t expected_revision, saved_revision;
    bool replace_existing, committed;
    UmiStatus status, reload_status;
    UmiProviderConnectionSnapshot snapshot;
} UmiProviderEditorJob;
struct UmiProviderConnectionsGtk {
    unsigned references;
    bool closed, busy, loaded, painting, dirty, editing_existing;
    char path[UMI_PATH_CAPACITY], application[UMI_PROVIDER_CONNECTION_ID_CAPACITY], profile[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
    char selected_id[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
    uint64_t draft_revision;
    UmiProviderConnectionSnapshot snapshot;
    GtkWidget *root, *form, *fields, *message, *selector, *saved;
    GtkWidget *id, *provider, *label, *endpoint, *model, *reference, *route, *timeout, *enabled;
    GtkWidget *discard, *confirm_remove, *confirm_review, *save, *remove, *rebase;
    GtkWidget *check_detail, *check_password, *check_confirm, *check_start, *check_cancel, *check_result;
    /* Context belongs to this presentation only, never to a settings job. */
    GtkWidget *chat_context, *chat_context_summary;
    UmiCancellationToken *check_token; /* Borrowed from the active owned job. */
    unsigned check_failures;
    gint64 check_retry_after;
    UmiStatus (*check_run)(const UmiProviderConnectionCheckPlan *, UmiProviderConnections *, bool,
        const char *, const UmiCancellationToken *, UmiProviderConnectionCheckResult *);
};
void UmiProviderEditorStart(UmiProviderConnectionsGtk *panel, UmiProviderEditorJob *job);
void UmiProviderEditorCompleted(UmiProviderConnectionsGtk *panel, const UmiProviderEditorJob *job);
void UmiProviderEditorRelease(UmiProviderConnectionsGtk *panel);
void UmiProviderCheckControls(UmiProviderConnectionsGtk *panel, GtkWidget *box);
void UmiProviderCheckRefresh(UmiProviderConnectionsGtk *panel);
void UmiProviderCheckRetire(UmiProviderConnectionsGtk *panel);
void UmiProviderChatContextControls(UmiProviderConnectionsGtk *panel, GtkWidget *box);
void UmiProviderChatContextRetire(UmiProviderConnectionsGtk *panel);
UmiStatus UmiProviderChatContextOpen(UmiProviderConnectionsGtk *panel,
    GtkWindow *parent, const UmiProviderChatGtkConfig *config);
#endif
