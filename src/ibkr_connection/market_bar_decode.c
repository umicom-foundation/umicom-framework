/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/market_bar_decode.c
 * PURPOSE: Use one decimal and OHLC validator for all broker bar observations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "order_numbers.h"
#include "market_bar_private.h"
#include <limits.h>
#include <string.h>
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
bool UmiIbkrMarketBarRead(char **f, UmiIbkrHistoricalDataKind kind, UmiIbkrHistoricalBar *bar)
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
