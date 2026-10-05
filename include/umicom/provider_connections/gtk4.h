/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/provider_connections/gtk4.h
 * PURPOSE: Expose a shared native editor for non-secret provider settings.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROVIDER_CONNECTIONS_GTK4_H
#define UMICOM_PROVIDER_CONNECTIONS_GTK4_H
#include <gtk/gtk.h>
#include "umicom/provider_connections/connections.h"
#include "umicom/ui/document_view.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProviderConnectionsGtk UmiProviderConnectionsGtk;
typedef struct UmiProviderConnectionsGtkConfig {
    const char *application_id;
    const char *profile_id;
    /* Required absolute local SQLite filename, copied during creation. The
     * worker creates its parent directory if needed. Keep it outside a source
     * repository or a shared/synchronised folder. No memory fallback is used. */
    const char *database_path;
} UmiProviderConnectionsGtkConfig;

/* GTK thread only. Creates a retained widget and starts an asynchronous load.
 * The panel owns its database jobs; it borrows no application service, user
 * callback or credential provider. A failed load is shown in the panel, with
 * saving disabled until Reload succeeds. No network or vault calls are made. */
UmiStatus UmiProviderConnectionsGtkCreate(const UmiProviderConnectionsGtkConfig *config,
    UmiProviderConnectionsGtk **out_panel);
GtkWidget *UmiProviderConnectionsGtkWidget(UmiProviderConnectionsGtk *panel);
bool UmiProviderConnectionsGtkBusy(const UmiProviderConnectionsGtk *panel);
/* Explains why a normal window close should wait or require explicit draft
 * discard. A host may force shutdown with Destroy, but must not describe a
 * write already running on a worker as cancelled or rolled back. */
bool UmiProviderConnectionsGtkCanClose(UmiProviderConnectionsGtk *panel);
/* Retire callbacks immediately. Pending jobs own their copied inputs and may
 * finish after this call, but cannot update widgets or access the host. Retained
 * buttons become inert. Join is not required on the GTK thread. */
void UmiProviderConnectionsGtkDestroy(UmiProviderConnectionsGtk *panel);
/* Thin hosts can open the shared window using the Framework's per-user data
 * directory. Scopes are aliases, not authentication. The parent is borrowed
 * during this call; the transient window closes with its parent. */
UmiStatus UmiProviderConnectionsGtkPresent(GtkWindow *parent,
    const char *application_id, const char *profile_id);
/* Attach an explicitly supplied, bounded UTF-8 context draft to this panel.
 * It is copied in memory, never persisted with connection metadata. An empty
 * string clears it. A successful chat handoff clears the panel's copy, so a
 * later chat cannot accidentally reuse the earlier selection. GTK thread only. */
UmiStatus UmiProviderConnectionsGtkSetChatContext(UmiProviderConnectionsGtk *panel,
    const char *text);
/* Capture one active view's current selection and open connection choice.
 * The caller serializes coordinator/view changes on its owning GTK thread.
 * No source-file reads, provider calls or credential access occur. Unsupported
 * text, an empty selection or more than 8192 selected bytes is rejected.
 * The model and view_id are borrowed only during this call; subsequent editor
 * changes do not update the captured draft. */
UmiStatus UmiProviderConnectionsGtkPresentSelection(GtkWindow *parent,
    const char *application_id, const char *profile_id,
    const UmiUiDocumentViewModel *documents, const char *view_id);
#ifdef __cplusplus
}
#endif
#endif
