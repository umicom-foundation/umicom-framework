/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/historical_decode.c
 * PURPOSE: Decode each finite historical payload completely before publishing any chart bars.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "observation_wire.h"
#include "order_numbers.h"
#include "market_bar_private.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Historical-only parsing is retained for review. The shared market-bar decoder
 * now applies these same numeric and OHLC rules to finite and streaming data. */
#if 0
static bool Ordered(const UmiIbkrHistoricalBar *bar)
{
    const UmiIbkrOrderNumber *numbers[] = {&bar->open, &bar->high, &bar->low, &bar->close};
    for (size_t i = 0U; i < 4U; ++i)
        if (!numbers[i]->exact)
            return true;
    int comparison;
    const UmiDecimal lesser[] = {bar->low.value, bar->open.value, bar->close.value};
    const UmiDecimal greater[] = {bar->high.value, bar->open.value, bar->close.value};
    for (size_t i = 0U; i < 3U; ++i)
    {
        if (UmiDecimalCompare(lesser[i], bar->high.value, &comparison) != UMI_STATUS_OK || comparison > 0)
            return false;
        if (UmiDecimalCompare(greater[i], bar->low.value, &comparison) != UMI_STATUS_OK || comparison < 0)
            return false;
    }
    return true;
}
static bool ReadBar(char **f, UmiIbkrHistoricalDataKind kind, UmiIbkrHistoricalBar *bar)
{
    uint64_t seconds, count;
    if (!UmiIbkrUnsigned(f[0], &seconds) || !seconds || seconds > UINT64_C(253402300799))
        return false;
    bar->timeMilliseconds = (int64_t)(seconds * 1000U);
    if (!UmiIbkrOrderNumberRead(f[1], false, false, &bar->open) ||
        !UmiIbkrOrderNumberRead(f[2], false, false, &bar->high) ||
        !UmiIbkrOrderNumberRead(f[3], false, false, &bar->low) ||
        !UmiIbkrOrderNumberRead(f[4], false, false, &bar->close) ||
        !UmiIbkrOrderNumberRead(f[5], true, false, &bar->volume) ||
        !UmiIbkrOrderNumberRead(f[6], true, false, &bar->weightedAveragePrice))
        return false;
    if (!strcmp(f[7], "-1"))
        bar->tradeCount = -1;
    else
    {
        if (!UmiIbkrUnsigned(f[7], &count) || count > (uint64_t)INT_MAX)
            return false;
        bar->tradeCount = (int32_t)count;
    }
    /* The provider uses unavailable sentinels on non-trade series. Keep their
     * original text, but never advertise an absent volume as a measured zero. */
    if (bar->volume.exact && bar->volume.value.coefficient < 0 && strcmp(f[5], "-1"))
        return false;
    bar->volumeAvailable =
        kind == UMI_IBKR_HISTORY_TRADES && bar->volume.exact && bar->volume.value.coefficient >= 0;
    bar->tradeCountAvailable = kind == UMI_IBKR_HISTORY_TRADES && bar->tradeCount >= 0;
    bar->weightedAverageAvailable =
        kind == UMI_IBKR_HISTORY_TRADES && bar->weightedAveragePrice.exact && bar->volumeAvailable;
    return Ordered(bar);
}
UmiStatus UmiIbkrHistoricalFrame(UmiIbkrConnection *c, const unsigned char *body, size_t length, uint64_t now)
{
    /* Legacy negotiated protocols carry the count, rows and end range together.
     * The newer standalone end message is deliberately outside this decoder. */
    UmiIbkrObservationFields fields = {0};
    UmiStatus status =
        UmiIbkrObservationFieldsOpen(body, length, 5U + UMI_IBKR_HISTORICAL_BAR_LIMIT * 8U, &fields);
    if (status != UMI_STATUS_OK)
        return status;
    uint64_t request, count;
    if (fields.count < 2U || strcmp(fields.values[0], "17") || !UmiIbkrUnsigned(fields.values[1], &request))
    {
        status = UMI_STATUS_PARSE_ERROR;
        goto done;
    }
    UmiIbkrHistoricalExpire(c, now);
    if (!c->history || request != c->history->snapshot.requestId || !c->history->snapshot.pending)
    {
        status = UmiIbkrObservationIgnore(c);
        goto done;
    }
    if (fields.count < 5U || !UmiIbkrUnsigned(fields.values[4], &count) ||
        count > UMI_IBKR_HISTORICAL_BAR_LIMIT || fields.count != 5U + (size_t)count * 8U ||
        !UmiIbkrText(fields.values[2], 96U, true) || !UmiIbkrText(fields.values[3], 96U, true))
    {
        status = UMI_STATUS_PARSE_ERROR;
        goto failed;
    }
    UmiIbkrHistoricalStore *next = calloc(1, sizeof *next);
    if (!next)
    {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto failed;
    }
    next->snapshot = c->history->snapshot;
    for (size_t i = 0U; i < (size_t)count; ++i)
    {
        if (!ReadBar(fields.values + 5U + i * 8U, next->snapshot.query.dataKind, &next->bars[i]) ||
            (i && next->bars[i].timeMilliseconds <= next->bars[i - 1U].timeMilliseconds))
        {
            free(next);
            status = UMI_STATUS_PARSE_ERROR;
            goto failed;
        }
    }
    /* No caller can observe a prefix of a malformed response. Gaps are retained
     * as gaps; this owner does not invent missing candles or sort bad packets. */
    next->snapshot.count = (size_t)count;
    next->snapshot.complete = true;
    next->snapshot.pending = false;
    next->snapshot.stale = false;
    next->snapshot.completedAtMilliseconds = now;
    strcpy(next->snapshot.startText, fields.values[2]);
    strcpy(next->snapshot.endText, fields.values[3]);
    (void)snprintf(next->snapshot.message, sizeof next->snapshot.message,
                   "Historical capture complete: %zu bars. The last bar may be unfinished.", (size_t)count);
    free(c->history);
    c->history = next;
    goto done;
failed:
    c->history->snapshot.pending = false;
    c->history->snapshot.failed = true;
    c->history->snapshot.stale = true;
    (void)snprintf(c->history->snapshot.message, sizeof c->history->snapshot.message,
                   "Historical response could not be validated; no partial series was published.");
done:
    UmiIbkrObservationFieldsClose(&fields);
    return status;
}
#endif
UmiStatus UmiIbkrHistoricalFrame(UmiIbkrConnection *c, const unsigned char *body, size_t length, uint64_t now)
{
    /* Legacy negotiated protocols carry the count, rows and end range together.
     * The newer standalone end message is deliberately outside this decoder. */
    UmiIbkrObservationFields fields = {0};
    UmiStatus status =
        UmiIbkrObservationFieldsOpen(body, length, 5U + UMI_IBKR_HISTORICAL_BAR_LIMIT * 8U, &fields);
    if (status != UMI_STATUS_OK)
        return status;
    uint64_t request, count;
    if (fields.count < 2U || strcmp(fields.values[0], "17") || !UmiIbkrUnsigned(fields.values[1], &request))
    {
        status = UMI_STATUS_PARSE_ERROR;
        goto done;
    }
    UmiIbkrHistoricalExpire(c, now);
    if (!c->history || request != c->history->snapshot.requestId || !c->history->snapshot.pending)
    {
        status = UmiIbkrObservationIgnore(c);
        goto done;
    }
    if (fields.count < 5U || !UmiIbkrUnsigned(fields.values[4], &count) ||
        count > UMI_IBKR_HISTORICAL_BAR_LIMIT || fields.count != 5U + (size_t)count * 8U ||
        !UmiIbkrText(fields.values[2], 96U, true) || !UmiIbkrText(fields.values[3], 96U, true))
    {
        status = UMI_STATUS_PARSE_ERROR;
        goto failed;
    }
    UmiIbkrHistoricalStore *next = calloc(1, sizeof *next);
    if (!next)
    {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto failed;
    }
    next->snapshot = c->history->snapshot;
    for (size_t i = 0U; i < (size_t)count; ++i)
    {
        if (!UmiIbkrMarketBarRead(fields.values + 5U + i * 8U, next->snapshot.query.dataKind, &next->bars[i]) ||
            (i && next->bars[i].timeMilliseconds <= next->bars[i - 1U].timeMilliseconds))
        {
            free(next);
            status = UMI_STATUS_PARSE_ERROR;
            goto failed;
        }
    }
    /* No caller can observe a prefix of a malformed response. Gaps are retained
     * as gaps; this owner does not invent missing candles or sort bad packets. */
    next->snapshot.count = (size_t)count;
    next->snapshot.complete = true;
    next->snapshot.pending = false;
    next->snapshot.stale = false;
    next->snapshot.completedAtMilliseconds = now;
    strcpy(next->snapshot.startText, fields.values[2]);
    strcpy(next->snapshot.endText, fields.values[3]);
    (void)snprintf(next->snapshot.message, sizeof next->snapshot.message,
                   "Historical capture complete: %zu bars. The last bar may be unfinished.", (size_t)count);
    free(c->history);
    c->history = next;
    goto done;
failed:
    c->history->snapshot.pending = false;
    c->history->snapshot.failed = true;
    c->history->snapshot.stale = true;
    (void)snprintf(c->history->snapshot.message, sizeof c->history->snapshot.message,
                   "Historical response could not be validated; no partial series was published.");
done:
    UmiIbkrObservationFieldsClose(&fields);
    return status;
}
