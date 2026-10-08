/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/historical_query.c
 * PURPOSE: Validate bounded historical requests without locale or local clock assumptions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <limits.h>
#include <string.h>
const char *UmiIbkrHistoricalBarSetting(uint32_t seconds)
{
    switch (seconds)
    {
    case 60U:
        return "1 min";
    case 300U:
        return "5 mins";
    case 900U:
        return "15 mins";
    case 1800U:
        return "30 mins";
    case 3600U:
        return "1 hour";
    default:
        return NULL;
    }
}
const char *UmiIbkrHistoricalDataSetting(UmiIbkrHistoricalDataKind kind)
{
    switch (kind)
    {
    case UMI_IBKR_HISTORY_TRADES:
        return "TRADES";
    case UMI_IBKR_HISTORY_MIDPOINT:
        return "MIDPOINT";
    case UMI_IBKR_HISTORY_BID:
        return "BID";
    case UMI_IBKR_HISTORY_ASK:
        return "ASK";
    default:
        return NULL;
    }
}
static unsigned Digits(const char *text, size_t length)
{
    unsigned value = 0U;
    for (size_t i = 0U; i < length; ++i)
        value = value * 10U + (unsigned)(text[i] - '0');
    return value;
}
static bool EndUtc(const char *text)
{
    if (!UmiIbkrText(text, 32U, true))
        return false;
    if (!*text)
        return true;
    if (strlen(text) != 21U || text[8] != ' ' || text[11] != ':' || text[14] != ':' ||
        strcmp(text + 17, " UTC"))
        return false;
    const size_t indices[] = {0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 12, 13, 15, 16};
    for (size_t i = 0U; i < sizeof indices / sizeof indices[0]; ++i)
        if (text[indices[i]] < '0' || text[indices[i]] > '9')
            return false;
    unsigned year = Digits(text, 4U), month = Digits(text + 4, 2U), day = Digits(text + 6, 2U);
    static const unsigned days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (year < 1970U || !month || month > 12U || !day)
        return false;
    unsigned limit = days[month - 1U];
    if (month == 2U && year % 4U == 0U && (year % 100U != 0U || year % 400U == 0U))
        ++limit;
    return day <= limit && Digits(text + 9, 2U) < 24U && Digits(text + 12, 2U) < 60U &&
           Digits(text + 15, 2U) < 60U;
}
UmiStatus UmiIbkrHistoricalQueryValidate(const UmiIbkrHistoricalQuery *q)
{
    if (!q || !q->contract.contractId || q->contract.contractId > (uint32_t)INT_MAX ||
        !UmiIbkrText(q->contract.exchange, sizeof q->contract.exchange, false) ||
        !UmiIbkrHistoricalBarSetting(q->barSeconds) || !UmiIbkrHistoricalDataSetting(q->dataKind) ||
        !EndUtc(q->endUtc))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Leave room for one edge bar. Bounds prevent a small UI request from
     * allocating an unbounded series when a provider returns dense data. */
    if (!q->durationSeconds || q->durationSeconds > 86400U ||
        q->durationSeconds > (UMI_IBKR_HISTORICAL_BAR_LIMIT - 1U) * q->barSeconds)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
