/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/news_providers.h
 *
 * PURPOSE:
 *   Capture subscribed news providers through the existing read-only broker session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_NEWS_PROVIDERS_H
#define UMICOM_BROKER_NEWS_PROVIDERS_H
#include "umicom/broker_connectivity/connection.h"
#define UMI_IBKR_NEWS_PROVIDER_LIMIT 32U
typedef struct UmiIbkrNewsProvider {
    char code[32];
    char name[256];
} UmiIbkrNewsProvider;
typedef struct UmiIbkrNewsProvidersSnapshot {
    bool requested, complete, timedOut, stale;
    uint64_t requestedAtMilliseconds, receivedAtMilliseconds;
    size_t count;
    UmiIbkrNewsProvider providers[UMI_IBKR_NEWS_PROVIDER_LIMIT];
} UmiIbkrNewsProvidersSnapshot;
/* One capture per connection: this callback has no request ID. Reconnect to
 * refresh subscriptions so a late reply cannot satisfy a replacement request.
 * This enumerates news providers; it does not fetch articles or market prices. */
UmiStatus UmiIbkrNewsProvidersRequest(UmiIbkrConnection *connection, uint64_t now);
UmiStatus UmiIbkrNewsProvidersCopy(const UmiIbkrConnection *connection, uint64_t now,
    UmiIbkrNewsProvidersSnapshot *out);
#endif
