/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/discovery_lifetime.c
 * PURPOSE: Retire subscriptions on disconnect and release their owned result buffers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <stdlib.h>
#include <string.h>
void UmiIbkrDiscoveryClosed(UmiIbkrConnection *c)
{
    for (size_t i = 0; i < UMI_IBKR_SCANNER_LIMIT; ++i)
    {
        UmiIbkrScannerStore *s = c->scanners[i];
        if (s && s->snapshot.needsCancel)
        {
            s->snapshot.active = false;
            s->snapshot.needsCancel = false;
            s->snapshot.failed = true;
            s->snapshot.stale = true;
            strcpy(s->snapshot.message, "Connection closed; retained scanner results are stale.");
        }
    }
    if (c->optionChains)
    {
        UmiIbkrOptionChainSnapshot *s = &c->optionChains->snapshot;
        s->stale = true;
        if (!s->complete && !s->abandoned)
        {
            s->failed = true;
            strcpy(s->message, "Connection closed before option discovery completed.");
        }
    }
}
void UmiIbkrDiscoveryDestroy(UmiIbkrConnection *c)
{
    for (size_t i = 0; i < UMI_IBKR_SCANNER_LIMIT; ++i)
    {
        free(c->scanners[i]);
        c->scanners[i] = NULL;
    }
    UmiIbkrOptionChainStoreFree(c->optionChains);
    c->optionChains = NULL;
}
