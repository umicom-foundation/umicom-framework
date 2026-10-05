/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/provider_chat/internal.h
 * PURPOSE: Keep immutable chat request storage and test-only dispatch hooks private.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROVIDER_CHAT_INTERNAL_H
#define UMICOM_PROVIDER_CHAT_INTERNAL_H
#include "umicom/provider_connections/chat.h"
#include "../provider_connection_checks/internal.h"
#include <stdatomic.h>
#define UMI_PROVIDER_CHAT_INPUT_CAPACITY 12352U
struct UmiProviderChatPlan
{
    UmiProviderConnectionCheckPlan connection;
    uint32_t output_limit;
    bool responses;
    atomic_bool consumed;
    char input[UMI_PROVIDER_CHAT_INPUT_CAPACITY];
    char body[UMI_CONNECTION_CHECK_BODY_CAPACITY];
};
typedef UmiStatus (*UmiProviderChatTransfer)(const UmiProviderConnectionCheckPlan *, const char *,
                                             const char *, const UmiCancellationToken *, char *, size_t,
                                             unsigned *);
UmiStatus UmiProviderChatEncode(UmiProviderChatPlan *plan, const char *prompt, const char *context);
UmiStatus UmiProviderChatDecode(bool responses, const char *body, size_t length, UmiProviderChatResult *out);
UmiStatus UmiProviderChatRunWith(UmiProviderChatPlan *plan, UmiProviderConnections *store, bool approved,
                                 const char *password, const UmiCancellationToken *cancellation,
                                 UmiProviderChatResult *out, UmiConnectionCheckSecretsFactory factory,
                                 UmiProviderChatTransfer transfer);
#endif
