/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/provider_chat/history_internal.h
 * PURPOSE: Keep exchange storage private and centralize revision and text validation rules.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROVIDER_CHAT_HISTORY_INTERNAL_H
#define UMICOM_PROVIDER_CHAT_HISTORY_INTERNAL_H
#include "umicom/provider_connections/history.h"
struct UmiProviderChatHistory
{
    uint64_t revision, next_id;
    size_t count;
    UmiProviderChatHistoryEntry entries[UMI_PROVIDER_CHAT_HISTORY_CAPACITY];
};
UmiStatus UmiProviderChatHistoryText(const char *text, size_t capacity);
const UmiProviderChatHistoryEntry *UmiProviderChatHistoryFind(const UmiProviderChatHistory *history,
                                                              uint64_t id);
#endif
