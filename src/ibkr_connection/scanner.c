/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/scanner.c
 * PURPOSE: Own cancellable, independently refreshed scanner result sets.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
UmiIbkrScannerStore *UmiIbkrScannerFind(const UmiIbkrConnection *c, uint32_t request)
{
    for (size_t i = 0; i < UMI_IBKR_SCANNER_LIMIT; ++i)
        if (c->scanners[i] && c->scanners[i]->snapshot.requestId == request)
            return c->scanners[i];
    return NULL;
}
UmiStatus UmiIbkrScannerRequest(UmiIbkrConnection *c, const UmiIbkrScannerQuery *q, uint64_t now,
                                uint32_t *out)
{
    if (!c || !out || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiIbkrScannerQueryValidate(q);
    if (status != UMI_STATUS_OK)
        return status;
    if (c->snapshot.state != UMI_IBKR_READY)
        return UMI_STATUS_INVALID_STATE;
    if (c->snapshot.protocolVersion < 151 || c->snapshot.protocolVersion > 176)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (c->scannerRequested && now - c->lastScannerRequestAt < 1000U)
        return UMI_STATUS_BUSY;
    size_t slot = UMI_IBKR_SCANNER_LIMIT;
    for (size_t i = 0; i < UMI_IBKR_SCANNER_LIMIT; ++i)
        if (!c->scanners[i] || !c->scanners[i]->snapshot.needsCancel)
        {
            slot = i;
            break;
        }
    if (slot == UMI_IBKR_SCANNER_LIMIT || c->nextQuoteRequest >= INT_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiIbkrScannerStore *s = calloc(1, sizeof *s);
    if (!s)
        return UMI_STATUS_OUT_OF_MEMORY;
    uint32_t request = c->nextQuoteRequest ? c->nextQuoteRequest : 36000U;
    char id[32], rows[16], filters[512];
    (void)snprintf(id, sizeof id, "%u", (unsigned)request);
    (void)snprintf(rows, sizeof rows, "%u", q->numberOfRows);
    status = UmiIbkrScannerFilters(q, filters);
    /* This negotiated protocol omits the old subscription version field.
     * Generic filters precede the empty internal-use options field. */
    const char *fields[] = {"22",
                            id,
                            rows,
                            q->instrument,
                            q->locationCode,
                            q->scanCode,
                            q->abovePrice,
                            q->belowPrice,
                            q->aboveVolume,
                            q->marketCapAbove,
                            q->marketCapBelow,
                            q->moodyRatingAbove,
                            q->moodyRatingBelow,
                            q->spRatingAbove,
                            q->spRatingBelow,
                            q->maturityDateAbove,
                            q->maturityDateBelow,
                            q->couponRateAbove,
                            q->couponRateBelow,
                            q->excludeConvertible ? "1" : "0",
                            q->averageOptionVolumeAbove,
                            q->scannerSettingPairs,
                            q->stockTypeFilter,
                            filters,
                            ""};
    if (status == UMI_STATUS_OK)
        status = UmiIbkrQueueFields(c, fields, sizeof fields / sizeof fields[0]);
    if (status != UMI_STATUS_OK)
    {
        free(s);
        return status;
    }
    s->snapshot.query = *q;
    s->snapshot.requestId = request;
    s->snapshot.active = true;
    s->snapshot.needsCancel = true;
    s->snapshot.stale = true;
    s->snapshot.requestedAtMilliseconds = now;
    strcpy(s->snapshot.message, "Waiting for a complete scanner refresh.");
    free(c->scanners[slot]);
    c->scanners[slot] = s;
    c->nextQuoteRequest = request + 1U;
    c->lastNow = now;
    c->scannerRequested = true;
    c->lastScannerRequestAt = now;
    *out = request;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrScannerCancel(UmiIbkrConnection *c, uint32_t request, uint64_t now)
{
    if (!c || !request || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrScannerStore *s = UmiIbkrScannerFind(c, request);
    if (!s)
        return UMI_STATUS_NOT_FOUND;
    if (c->snapshot.state != UMI_IBKR_READY || !s->snapshot.needsCancel)
        return UMI_STATUS_INVALID_STATE;
    char id[32];
    (void)snprintf(id, sizeof id, "%u", (unsigned)request);
    const char *fields[] = {"23", "1", id};
    UmiStatus status = UmiIbkrQueueFields(c, fields, 3U);
    if (status != UMI_STATUS_OK)
        return status;
    s->snapshot.active = false;
    s->snapshot.needsCancel = false;
    s->snapshot.cancelled = true;
    s->snapshot.stale = true;
    c->lastNow = now;
    strcpy(s->snapshot.message, "Scanner cancelled; retained results are stale.");
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrScannerCopy(const UmiIbkrConnection *c, uint32_t request, uint64_t now, uint64_t age,
                             UmiIbkrScannerSnapshot *out)
{
    if (!c || !out || !request || !age || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrScannerStore *s = UmiIbkrScannerFind(c, request);
    if (!s)
        return UMI_STATUS_NOT_FOUND;
    *out = s->snapshot;
    out->stale = !out->active || out->failed || !out->generation || c->snapshot.state != UMI_IBKR_READY ||
                 now - out->receivedAtMilliseconds > age;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrScannerRowCopy(const UmiIbkrConnection *c, uint32_t request, uint64_t generation,
                                size_t index, UmiIbkrScannerRow *out)
{
    if (!c || !out || !request || !generation)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrScannerStore *s = UmiIbkrScannerFind(c, request);
    if (!s)
        return UMI_STATUS_NOT_FOUND;
    if (generation != s->snapshot.generation)
        return UMI_STATUS_BUSY;
    if (index >= s->snapshot.count)
        return UMI_STATUS_NOT_FOUND;
    *out = s->rows[index];
    return UMI_STATUS_OK;
}
bool UmiIbkrScannerProviderMessage(UmiIbkrConnection *c, const char *id, int code, const char *text)
{
    uint64_t request;
    if (!UmiIbkrUnsigned(id, &request) || request > UINT32_MAX)
        return false;
    UmiIbkrScannerStore *s = UmiIbkrScannerFind(c, (uint32_t)request);
    if (!s)
        return false;
    if (s->snapshot.active)
    {
        s->snapshot.active = false;
        s->snapshot.failed = true;
        s->snapshot.stale = true;
        s->snapshot.providerCode = code;
        strcpy(s->snapshot.message, strlen(text) < sizeof s->snapshot.message
                                        ? text
                                        : "Scanner diagnostic exceeds the display limit.");
    }
    return true;
}
