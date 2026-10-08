/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/depth_decode.c
 * PURPOSE: Apply broker depth deltas only after validating the complete operation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "observation_wire.h"
#include <limits.h>
#include <string.h>
static bool DepthUnsigned(const char *text, uint64_t maximum, uint64_t *out)
{
    return UmiIbkrUnsigned(text, out) && *out <= maximum;
}
UmiStatus UmiIbkrDepthFrame(UmiIbkrConnection *c, uint64_t message, const unsigned char *body, size_t length,
                            uint64_t now)
{
    UmiIbkrObservationFields fields = {0};
    UmiStatus status = UmiIbkrObservationFieldsOpen(body, length, 10U, &fields);
    if (status != UMI_STATUS_OK)
        return status;
    uint64_t version, request;
    if (fields.count < 3U || !DepthUnsigned(fields.values[1], INT_MAX, &version) || version == 0U ||
        !DepthUnsigned(fields.values[2], INT_MAX, &request))
    {
        status = UMI_STATUS_PARSE_ERROR;
        goto done;
    }
    UmiIbkrDepthSnapshot *slot = NULL;
    for (size_t i = 0U; i < UMI_IBKR_DEPTH_STREAM_LIMIT; ++i)
        if (request != 0U && c->depth[i].requestId == request)
            slot = &c->depth[i];
    if (slot == NULL || !slot->subscribed || slot->failed || c->snapshot.state != UMI_IBKR_READY)
    {
        status = UmiIbkrObservationIgnore(c);
        goto done;
    }
    bool levelTwo = message == 13U;
    if ((message != 12U && message != 13U) || fields.count != (levelTwo ? 10U : 8U) ||
        now < slot->requestedAtMilliseconds)
    {
        status = UMI_STATUS_PARSE_ERROR;
        goto done;
    }
    size_t opAt = levelTwo ? 5U : 4U;
    uint64_t position, operation, side, smart = 0U;
    if (!DepthUnsigned(fields.values[3], UMI_IBKR_DEPTH_ROW_LIMIT - 1U, &position) ||
        !DepthUnsigned(fields.values[opAt], 2U, &operation) ||
        !DepthUnsigned(fields.values[opAt + 1U], 1U, &side) ||
        (levelTwo && !DepthUnsigned(fields.values[9], 1U, &smart)) ||
        slot->selection.smartDepth != (smart != 0U))
    {
        status = UMI_STATUS_PARSE_ERROR;
        goto done;
    }
    UmiIbkrDepthRow row = {0};
    row.receivedAtMilliseconds = now;
    row.hasMarketMaker = levelTwo;
    if (levelTwo)
    {
        if (!UmiIbkrText(fields.values[4], sizeof row.marketMaker, true))
        {
            status = UMI_STATUS_PARSE_ERROR;
            goto done;
        }
        strcpy(row.marketMaker, fields.values[4]);
    }
    /* Deletion removes a position. Its ignored numeric payload is never used
     * as a replacement price or size. Insert/update values must be exact. */
    if (operation != 2U)
    {
        if (UmiDecimalParseScientificExact(fields.values[opAt + 2U], strlen(fields.values[opAt + 2U]),
                                           &row.price) != UMI_STATUS_OK ||
            UmiDecimalParseScientificExact(fields.values[opAt + 3U], strlen(fields.values[opAt + 3U]),
                                           &row.size) != UMI_STATUS_OK ||
            row.size.coefficient < 0)
        {
            status = UMI_STATUS_PARSE_ERROR;
            goto done;
        }
    }
    size_t *count = side == 1U ? &slot->bidCount : &slot->askCount;
    UmiIbkrDepthRow *rows = side == 1U ? slot->bids : slot->asks;
    size_t at = (size_t)position;
    if (at >= slot->selection.rows || (operation == 0U ? at > *count : at >= *count))
    {
        status = UMI_STATUS_PARSE_ERROR;
        goto done;
    }
    if (slot->revision == UINT64_MAX)
    {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto done;
    }
    /* Validation ends before mutation. Inserting into a full visible ladder
     * shifts rows and discards only its old last level, as positional depth
     * semantics require. Price sorting here would corrupt later row indexes. */
    if (operation == 0U)
    {
        size_t next = *count < slot->selection.rows ? *count + 1U : *count;
        for (size_t i = next - 1U; i > at; --i)
            rows[i] = rows[i - 1U];
        rows[at] = row;
        *count = next;
    }
    else if (operation == 1U)
        rows[at] = row;
    else
    {
        for (size_t i = at; i + 1U < *count; ++i)
            rows[i] = rows[i + 1U];
        --*count;
        memset(&rows[*count], 0, sizeof rows[*count]);
    }
    slot->received = true;
    slot->stale = false;
    slot->receivedAtMilliseconds = now;
    ++slot->revision;
    strcpy(slot->message,
           "Depth delta received. Displayed liquidity may change before an order reaches the market.");
done:
    UmiIbkrObservationFieldsClose(&fields);
    return status;
}
