/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/scanner_catalog.c
 * PURPOSE: Retain one complete scanner catalogue with explicit ownership and timeout.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <stdlib.h>
#include <string.h>
bool UmiIbkrScannerCatalogPending(const UmiIbkrConnection *c, uint64_t now)
{
    return c->scannerCatalog.snapshot.requested && !c->scannerCatalog.snapshot.complete &&
           !c->scannerCatalog.snapshot.failed && now >= c->scannerCatalog.snapshot.requestedAtMilliseconds &&
           now - c->scannerCatalog.snapshot.requestedAtMilliseconds < 60000U;
}
UmiStatus UmiIbkrScannerCatalogRequest(UmiIbkrConnection *c, uint64_t now)
{
    if (!c || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (c->snapshot.state != UMI_IBKR_READY || c->scannerCatalog.snapshot.requested)
        return UMI_STATUS_INVALID_STATE;
    if (c->snapshot.protocolVersion < 151 || c->snapshot.protocolVersion > 176)
        return UMI_STATUS_NOT_IMPLEMENTED;
    const char *fields[] = {"24", "1"};
    UmiStatus status = UmiIbkrQueueFields(c, fields, 2U);
    if (status != UMI_STATUS_OK)
        return status;
    c->scannerCatalog.snapshot.requested = true;
    c->scannerCatalog.snapshot.stale = true;
    c->scannerCatalog.snapshot.requestedAtMilliseconds = now;
    strcpy(c->scannerCatalog.snapshot.message, "Waiting for the scanner parameter catalogue.");
    c->lastNow = now;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrScannerCatalogCopy(const UmiIbkrConnection *c, uint64_t now,
                                    UmiIbkrScannerCatalogSnapshot *out)
{
    if (!c || !out || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!c->scannerCatalog.snapshot.requested)
        return UMI_STATUS_NOT_FOUND;
    *out = c->scannerCatalog.snapshot;
    if (!out->complete && !out->failed && !UmiIbkrScannerCatalogPending(c, now))
    {
        out->failed = true;
        strcpy(out->message, "Scanner catalogue timed out. Reconnect before requesting again.");
    }
    out->stale = !out->complete || out->failed || c->snapshot.state != UMI_IBKR_READY;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrScannerCatalogTextCopy(const UmiIbkrConnection *c, char *out, size_t capacity)
{
    if (!c || !out || !capacity)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!c->scannerCatalog.snapshot.complete)
        return UMI_STATUS_INVALID_STATE;
    if (capacity <= c->scannerCatalog.snapshot.byteCount)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, c->scannerCatalog.xml, c->scannerCatalog.snapshot.byteCount + 1U);
    return UMI_STATUS_OK;
}
void UmiIbkrScannerCatalogClosed(UmiIbkrConnection *c)
{
    UmiIbkrScannerCatalogSnapshot *s = &c->scannerCatalog.snapshot;
    s->stale = true;
    if (s->requested && !s->complete)
    {
        s->failed = true;
        strcpy(s->message, "Connection closed before the catalogue arrived.");
    }
    free(c->catalogFrame);
    c->catalogFrame = NULL;
    c->catalogFrameSize = c->catalogFrameCapacity = 0U;
}
void UmiIbkrScannerCatalogDestroy(UmiIbkrConnection *c)
{
    free(c->scannerCatalog.xml);
    c->scannerCatalog.xml = NULL;
}
/* Ordinary observations retain their small receive bound. Only a pending
 * catalogue request may allocate a larger envelope, and only once at a time.
 * The decoder checks message identity before publishing any text. */
UmiStatus UmiIbkrScannerCatalogBeginFrame(UmiIbkrConnection *c, size_t length)
{
    if (length > UMI_IBKR_SCANNER_CATALOG_BYTES + 6U || !UmiIbkrScannerCatalogPending(c, c->lastNow) ||
        c->catalogFrame)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    unsigned char *buffer = malloc(length + 4U);
    if (!buffer)
        return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(buffer, c->rx, c->rxSize);
    c->catalogFrame = buffer;
    c->catalogFrameSize = c->rxSize;
    c->catalogFrameCapacity = length + 4U;
    c->rxSize = 0U;
    return UMI_STATUS_OK;
}
