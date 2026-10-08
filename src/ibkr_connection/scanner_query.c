/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/scanner_query.c
 * PURPOSE: Validate provider fields before creating a scanner subscription.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "order_numbers.h"
#include <limits.h>
#include <string.h>
bool UmiIbkrDiscoveryDate(const char *text)
{
    if (!UmiIbkrText(text, 9U, false) || strlen(text) != 8U)
        return false;
    uint64_t value;
    if (!UmiIbkrUnsigned(text, &value))
        return false;
    unsigned year = (unsigned)(value / 10000U), month = (unsigned)((value / 100U) % 100U),
             day = (unsigned)(value % 100U);
    static const unsigned days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (year < 1900U || year > 9999U || month < 1U || month > 12U)
        return false;
    unsigned maximum = days[month - 1U];
    if (month == 2U && year % 4U == 0U && (year % 100U != 0U || year % 400U == 0U))
        ++maximum;
    return day >= 1U && day <= maximum;
}
static bool OptionalWhole(const char *text, size_t capacity)
{
    uint64_t value;
    return UmiIbkrText(text, capacity, true) &&
           (!*text || (UmiIbkrUnsigned(text, &value) && value <= INT_MAX));
}
static bool DecimalRange(const char *low, const char *high)
{
    UmiIbkrOrderNumber a, b;
    if (!UmiIbkrOrderNumberRead(low, true, true, &a) || !UmiIbkrOrderNumberRead(high, true, true, &b) ||
        (*low && !a.exact) || (*high && !b.exact))
        return false;
    /* Query thresholds must be exactly representable. Returned metadata may retain
     * larger values as text, but requests should not silently round a user's filter. */
    if (*low && *high)
    {
        int comparison;
        if (UmiDecimalCompare(a.value, b.value, &comparison) != UMI_STATUS_OK || comparison > 0)
            return false;
    }
    return true;
}
UmiStatus UmiIbkrScannerFilters(const UmiIbkrScannerQuery *q, char out[512])
{
    if (!q || !out || q->filterCount > UMI_IBKR_SCANNER_FILTER_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    char value[512] = {0};
    size_t used = 0U;
    for (size_t i = 0; i < q->filterCount; ++i)
    {
        const UmiIbkrScannerFilter *f = &q->filters[i];
        if (!UmiIbkrText(f->tag, sizeof f->tag, false) || !UmiIbkrText(f->value, sizeof f->value, false) ||
            strpbrk(f->tag, "=;") || strpbrk(f->value, "=;"))
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t j = 0; j < i; ++j)
            if (!strcmp(f->tag, q->filters[j].tag))
                return UMI_STATUS_INVALID_ARGUMENT;
        size_t tag = strlen(f->tag), data = strlen(f->value);
        if (tag + data + 2U >= sizeof value - used)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        memcpy(value + used, f->tag, tag);
        used += tag;
        value[used++] = '=';
        memcpy(value + used, f->value, data);
        used += data;
        value[used++] = ';';
    }
    memcpy(out, value, sizeof value);
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrScannerQueryValidate(const UmiIbkrScannerQuery *q)
{
    if (!q || q->numberOfRows < 1U || q->numberOfRows > UMI_IBKR_SCANNER_ROW_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiIbkrText(q->instrument, sizeof q->instrument, false) ||
        !UmiIbkrText(q->locationCode, sizeof q->locationCode, false) ||
        !UmiIbkrText(q->scanCode, sizeof q->scanCode, false) || !DecimalRange(q->abovePrice, q->belowPrice) ||
        !DecimalRange(q->marketCapAbove, q->marketCapBelow) ||
        !DecimalRange(q->couponRateAbove, q->couponRateBelow) ||
        !OptionalWhole(q->aboveVolume, sizeof q->aboveVolume) ||
        !OptionalWhole(q->averageOptionVolumeAbove, sizeof q->averageOptionVolumeAbove))
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *text[] = {q->moodyRatingAbove, q->moodyRatingBelow, q->spRatingAbove, q->spRatingBelow,
                          q->stockTypeFilter};
    for (size_t i = 0; i < sizeof text / sizeof text[0]; ++i)
        if (!UmiIbkrText(text[i], 32U, true))
            return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiIbkrText(q->maturityDateAbove, sizeof q->maturityDateAbove, true) ||
        !UmiIbkrText(q->maturityDateBelow, sizeof q->maturityDateBelow, true) ||
        (*q->maturityDateAbove && !UmiIbkrDiscoveryDate(q->maturityDateAbove)) ||
        (*q->maturityDateBelow && !UmiIbkrDiscoveryDate(q->maturityDateBelow)) ||
        (*q->maturityDateAbove && *q->maturityDateBelow &&
         strcmp(q->maturityDateAbove, q->maturityDateBelow) > 0) ||
        !UmiIbkrText(q->scannerSettingPairs, sizeof q->scannerSettingPairs, true))
        return UMI_STATUS_INVALID_ARGUMENT;
    char filters[512];
    return UmiIbkrScannerFilters(q, filters);
}
