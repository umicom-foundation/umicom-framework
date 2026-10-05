/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cash_planning/date.c
 * PURPOSE: Parse explicit Gregorian dates independently of locale and timezone.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/cash_planning/plan.h"
#include <stdio.h>
#include <string.h>
UmiStatus UmiCashPlanDateParse(const char *text, size_t bytes, UmiFinancialDate *out)
{
    if (text == NULL || out == NULL || bytes != 10U || text[4] != '-' || text[7] != '-')
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < bytes; ++i)
        if (i != 4U && i != 7U && (text[i] < '0' || text[i] > '9'))
            return UMI_STATUS_PARSE_ERROR;
    UmiFinancialDate date = {
        (text[0] - '0') * 1000 + (text[1] - '0') * 100 + (text[2] - '0') * 10 + text[3] - '0',
        (uint8_t)((text[5] - '0') * 10 + text[6] - '0'), (uint8_t)((text[8] - '0') * 10 + text[9] - '0')};
    if (!umi_financial_date_is_valid(date))
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = date;
    return UMI_STATUS_OK;
}
UmiStatus UmiCashPlanDateFormat(UmiFinancialDate date, char *out, size_t capacity)
{
    if (out == NULL || !umi_financial_date_is_valid(date))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < 11U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char text[11];
    int length = snprintf(text, sizeof text, "%04d-%02u-%02u", (int)date.year, (unsigned)date.month,
                          (unsigned)date.day);
    if (length != 10)
        return UMI_STATUS_INTERNAL_ERROR;
    memcpy(out, text, sizeof text);
    return UMI_STATUS_OK;
}
