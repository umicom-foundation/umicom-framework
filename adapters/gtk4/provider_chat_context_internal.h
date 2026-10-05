/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/provider_chat_context_internal.h
 * PURPOSE: Share bounded native context validation without exporting a second text policy.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_GTK4_PROVIDER_CHAT_CONTEXT_INTERNAL_H
#define UMICOM_GTK4_PROVIDER_CHAT_CONTEXT_INTERNAL_H
#include "umicom/provider_connections/chat_gtk4.h"
UmiStatus UmiProviderChatGtkContextValidate(const char *text);
#endif
