/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/instrument_search.c
 *
 * PURPOSE:
 *   Resolve shares, futures and bonds through the existing bounded contract request owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>
UmiStatus UmiIbkrInstrumentSearch(UmiIbkrConnection *c, const UmiIbkrInstrumentQuery *q,
    uint64_t now, uint32_t *out)
{
    if (!c || !q || !out || now < c->lastNow ||
        !UmiIbkrText(q->symbol, sizeof q->symbol, false) ||
        !UmiIbkrText(q->securityType, sizeof q->securityType, false) ||
        !UmiIbkrText(q->exchange, sizeof q->exchange, false) ||
        !UmiIbkrText(q->currency, sizeof q->currency, false) ||
        !UmiIbkrText(q->expiry, sizeof q->expiry, true)) return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(q->securityType,"STK") && strcmp(q->securityType,"FUT") &&
        strcmp(q->securityType,"BOND")) return UMI_STATUS_NOT_IMPLEMENTED;
    size_t expiry = strlen(q->expiry);
    if (expiry && (strcmp(q->securityType,"FUT") || (expiry != 6U && expiry != 8U)))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < expiry; ++i)
        if (q->expiry[i] < '0' || q->expiry[i] > '9') return UMI_STATUS_INVALID_ARGUMENT;
    if (c->snapshot.state != UMI_IBKR_READY || c->snapshot.stale) return UMI_STATUS_INVALID_STATE;
    if (c->snapshot.protocolVersion < 164 || c->snapshot.protocolVersion > 176)
        return UMI_STATUS_NOT_IMPLEMENTED;
    UmiIbkrContractDetailsSnapshot *d = &c->contractDetails;
    if (d->requestId && !d->complete && !d->failed &&
        now >= d->requestedAtMilliseconds &&
        now - d->requestedAtMilliseconds < c->options.timeoutMilliseconds) return UMI_STATUS_BUSY;
    uint32_t id = c->nextQuoteRequest ? c->nextQuoteRequest : 36000U;
    if (id >= INT32_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    char text[24]; (void)snprintf(text,sizeof text,"%u",(unsigned)id);
    const char *fields[] = {"9","8",text,"0",q->symbol,q->securityType,q->expiry,
        "0","","",q->exchange,"",q->currency,"","","0","","",""};
    UmiStatus status = UmiIbkrQueueFields(c,fields,c->snapshot.protocolVersion >= 176 ? 19U : 18U);
    if (status != UMI_STATUS_OK) return status;
    /* Copy input before replacing shared storage: callers may reuse a prior query. */
    UmiIbkrInstrumentQuery query = *q;
    memset(d,0,sizeof *d); d->requestId = id; d->instrumentLookup = true;
    d->requestedInstrument = query; strcpy(d->requestedContract.exchange,query.exchange);
    d->requestedAtMilliseconds = now;
    strcpy(d->message,"Instrument search pending. Select an exact result after completion.");
    c->nextQuoteRequest = id + 1U; c->lastNow = now; *out = id;
    return UMI_STATUS_OK;
}
