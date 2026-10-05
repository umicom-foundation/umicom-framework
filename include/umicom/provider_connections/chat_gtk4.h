/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/provider_connections/chat_gtk4.h
 * PURPOSE: Expose a shared review-and-send window for one explicitly selected saved connection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROVIDER_CONNECTIONS_CHAT_GTK4_H
#define UMICOM_PROVIDER_CONNECTIONS_CHAT_GTK4_H
#include "umicom/provider_connections/gtk4.h"
#include "umicom/provider_connections/chat.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiProviderChatGtk UmiProviderChatGtk;
    typedef struct UmiProviderChatGtkConfig
    {
        UmiProviderConnectionsGtkConfig settings;
        const char *connection_id;
        uint64_t revision;
    } UmiProviderChatGtkConfig;
    /* GTK thread only. Copy the selected connection scope, absolute SQLite path
 * and revision. Creation does no I/O. Review opens storage on a worker; Send
 * requires a prepared review and separate approval. No implicit file context,
 * transcript persistence, tool execution or change of legacy routing occurs. */
    UmiStatus UmiProviderChatGtkCreate(const UmiProviderChatGtkConfig *config, UmiProviderChatGtk **out);
    GtkWidget *UmiProviderChatGtkWidget(UmiProviderChatGtk *panel);
    bool UmiProviderChatGtkBusy(const UmiProviderChatGtk *panel);
    bool UmiProviderChatGtkCanClose(UmiProviderChatGtk *panel);
    /* Retire callbacks and request cancellation; an active worker retains its
 * controller until completion. Retained controls cannot start another job. */
    void UmiProviderChatGtkDestroy(UmiProviderChatGtk *panel);
    UmiStatus UmiProviderChatGtkPresent(GtkWindow *parent, const UmiProviderChatGtkConfig *config);
    /* Replace only an idle context draft. The copied text is bounded UTF-8;
     * failure leaves the draft and review unchanged. Success invalidates the
     * previous review, approval and password. Nothing is sent or prepared. */
    UmiStatus UmiProviderChatGtkSetContext(UmiProviderChatGtk *panel, const char *text);
    /* Seed a new review window with a copied context, never with live editor
     * pointers. The prompt starts empty and explicit review is still required. */
    UmiStatus UmiProviderChatGtkPresentWithContext(GtkWindow *parent,
        const UmiProviderChatGtkConfig *config, const char *context);

#ifdef __cplusplus
}
#endif
#endif
