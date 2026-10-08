/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/pnl_decode.c
 * PURPOSE: Publish complete P&L callbacks atomically and retain unavailable numeric values.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "observation_wire.h"
#include <string.h>
static UmiStatus PnlNumber(const char *text, UmiIbkrPnlNumber *out)
{
    if (!UmiIbkrText(text, sizeof out->reportedText, true))
        return UMI_STATUS_PARSE_ERROR;
    UmiIbkrPnlNumber candidate = {0};
    if (*text != '\0' && !UmiIbkrDecimalText(text))
        return UMI_STATUS_PARSE_ERROR;
    strcpy(candidate.reportedText, text);
    if (*text != '\0')
    {
        UmiStatus status = UmiDecimalParseScientificExact(text, strlen(text), &candidate.value);
        candidate.exact = status == UMI_STATUS_OK;
        /* A provider unset sentinel or a value beyond our exact decimal range
         * remains raw evidence. It must never be silently added as zero. */
    }
    *out = candidate;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrPnlFrame(UmiIbkrConnection *c, uint64_t message, const unsigned char *body, size_t length,
                          uint64_t now)
{
    UmiIbkrObservationFields fields = {0};
    UmiStatus status = UmiIbkrObservationFieldsOpen(body, length, 7U, &fields);
    if (status != UMI_STATUS_OK)
        return status;
    uint64_t request = 0U;
    if (fields.count < 2U || !UmiIbkrUnsigned(fields.values[1], &request) || request > UINT32_MAX)
    {
        status = UMI_STATUS_PARSE_ERROR;
        goto done;
    }
    UmiIbkrPnlSnapshot *slot = NULL;
    for (size_t i = 0U; i < UMI_IBKR_PNL_CAPACITY; ++i)
        if (c->pnl[i].requestId == request && request != 0U)
            slot = &c->pnl[i];
    if (slot == NULL || !slot->subscribed || slot->failed || c->snapshot.state != UMI_IBKR_READY)
    {
        status = UmiIbkrObservationIgnore(c);
        goto done;
    }
    if ((message != 94U && message != 95U) || fields.count != (message == 94U ? 5U : 7U) ||
        (slot->selection.contractId != 0U) != (message == 95U) || now < slot->requestedAtMilliseconds)
    {
        status = UMI_STATUS_PARSE_ERROR;
        goto done;
    }
    if (slot->revision == UINT64_MAX)
    {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto done;
    }
    UmiIbkrPnlSnapshot candidate = *slot;
    size_t first = message == 94U ? 2U : 3U;
    status = PnlNumber(fields.values[first], &candidate.daily);
    if (status == UMI_STATUS_OK)
        status = PnlNumber(fields.values[first + 1U], &candidate.unrealized);
    if (status == UMI_STATUS_OK)
        status = PnlNumber(fields.values[first + 2U], &candidate.realized);
    if (status == UMI_STATUS_OK && message == 95U)
        status = PnlNumber(fields.values[2], &candidate.position);
    if (status == UMI_STATUS_OK && message == 95U)
        status = PnlNumber(fields.values[6], &candidate.marketValue);
    if (status != UMI_STATUS_OK)
        goto done;
    candidate.received = true;
    candidate.stale = false;
    candidate.receivedAtMilliseconds = now;
    ++candidate.revision;
    strcpy(candidate.message, "Provider P&L received. Reset schedule and currency are not inferred here.");
    *slot = candidate;
done:
    UmiIbkrObservationFieldsClose(&fields);
    return status;
}
