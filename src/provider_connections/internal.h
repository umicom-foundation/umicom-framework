/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/provider_connections/internal.h
 * PURPOSE: Share bounded persistence helpers within the connection service.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROVIDER_CONNECTIONS_INTERNAL_H
#define UMICOM_PROVIDER_CONNECTIONS_INTERNAL_H
#include "umicom/provider_connections/connections.h"
#include "umicom/platform/threading.h"
#define UMI_PROVIDER_CONNECTION_WIRE_CAPACITY 4096U
#define UMI_PROVIDER_CONNECTION_KEY_CAPACITY 192U
struct UmiProviderConnections {
    UmiDataServer *server;
    UmiMutex *mutex;
    char prefix[128];
    bool poisoned;
};
bool UmiProviderConnectionIdValid(const char *text, size_t capacity);
UmiStatus UmiProviderConnectionEncode(const UmiProviderConnection *connection,
    char *out, size_t capacity);
UmiStatus UmiProviderConnectionDecode(const char *wire, UmiProviderConnection *out);
UmiStatus UmiProviderConnectionIndexEncode(const UmiProviderConnectionSnapshot *snapshot,
    char *out, size_t capacity);
UmiStatus UmiProviderConnectionIndexDecode(const char *wire,
    UmiProviderConnectionSnapshot *out);
#endif
