/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/provider_chat_internal.h
 * PURPOSE: Separate immutable chat jobs from GTK-owned review widgets.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROVIDER_CHAT_GTK_INTERNAL_H
#define UMICOM_PROVIDER_CHAT_GTK_INTERNAL_H
#include "umicom/provider_connections/chat_gtk4.h"
#include "umicom/platform/path.h"
#include "umicom/provider_connections/history.h"
#include "umicom/security/local_profile.h"
typedef UmiStatus (*UmiProviderChatGtkRun)(UmiProviderChatPlan *, UmiProviderConnections *, bool,
                                           const char *, const UmiCancellationToken *,
                                           UmiProviderChatResult *);
struct UmiProviderChatGtk
{
    unsigned references, denials;
    bool busy, closed, dirty, painting;
    gint64 retry_after;
    char path[UMI_PATH_CAPACITY], application[49], profile[49], connection[49];
    uint64_t revision;
    /* The GTK thread owns history; workers never borrow this model or its text. */
    UmiProviderChatHistory *history;
    uint64_t history_revision, history_entry;
    UmiAiRole history_role;
    bool history_painting;
    GtkWidget *history_box, *history_selector, *history_part, *history_view, *history_detail, *history_message;
    UmiProviderChatPlan *plan;
    UmiCancellationToken *token; /* Borrowed from the active job. */
    GtkWidget *root, *fields, *prompt, *context, *limit, *password, *approve, *discard;
    GtkWidget *review, *send, *cancel, *clear, *details, *preview, *result, *message;
    UmiProviderChatGtkRun run; /* Private regression hook; defaults to public Run. */
};
typedef struct UmiProviderChatGtkJob
{
    bool send;
    char path[UMI_PATH_CAPACITY], application[49], profile[49], connection[49];
    uint64_t revision;
    char prompt[UMI_PROVIDER_CHAT_PROMPT_CAPACITY], context[UMI_PROVIDER_CHAT_CONTEXT_CAPACITY];
    char password[UMI_LOCAL_PROFILE_PASSWORD_CAPACITY];
    uint32_t limit;
    UmiProviderChatPlan *plan;
    UmiCancellationToken *token;
    UmiProviderChatResult result;
    UmiStatus status;
    UmiProviderChatGtkRun run;
} UmiProviderChatGtkJob;
void UmiProviderChatGtkRelease(UmiProviderChatGtk *panel);
void UmiProviderChatGtkSensitivity(UmiProviderChatGtk *panel);
void UmiProviderChatGtkLaunch(UmiProviderChatGtk *panel, UmiProviderChatGtkJob *job);
void UmiProviderChatGtkJobFree(gpointer data);
void UmiProviderChatGtkText(GtkWidget *view, const char *text);
void UmiProviderChatGtkHistoryControls(UmiProviderChatGtk *panel, GtkWidget *box);
void UmiProviderChatGtkHistoryRefresh(UmiProviderChatGtk *panel, uint64_t selected_id);
void UmiProviderChatGtkHistoryRecord(UmiProviderChatGtk *panel, const UmiProviderChatGtkJob *job);
UmiStatus UmiProviderChatGtkHistoryClear(UmiProviderChatGtk *panel);
void UmiProviderChatGtkHistoryRetire(UmiProviderChatGtk *panel);
#endif
