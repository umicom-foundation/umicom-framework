/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/activity_query.c
 * PURPOSE: Validate explicit report filters with the shared Gregorian date contract.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "activity_private.h"
#include "internal.h"
#include <stdio.h>
#include <string.h>
static int EmptyDate(UmiFinancialDate date)
{ return date.year == 0 && date.month == 0 && date.day == 0; }
const char *UmiBankActivityDirectionName(UmiBankActivityDirection direction)
{
    switch (direction) {
    case UMI_BANK_ACTIVITY_ALL: return "all";
    case UMI_BANK_ACTIVITY_DEBITS: return "debits";
    case UMI_BANK_ACTIVITY_CREDITS: return "credits";
    default: return NULL;
    }
}
UmiStatus UmiBankActivityDateParse(const char *text, UmiFinancialDate *outDate)
{
    if (text == NULL || outDate == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0;
    while (length < 11U && text[length] != '\0') ++length;
    UmiFinancialDate date = {0};
    if (length == 0U) { *outDate = date; return UMI_STATUS_OK; }
    if (length != 10U || text[4] != '-' || text[7] != '-') return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0; i < 10U; ++i)
        if (i != 4U && i != 7U && (text[i] < '0' || text[i] > '9')) return UMI_STATUS_INVALID_ARGUMENT;
    date.year = (text[0]-'0')*1000 + (text[1]-'0')*100 + (text[2]-'0')*10 + text[3]-'0';
    date.month = (uint8_t)((text[5]-'0')*10 + text[6]-'0');
    date.day = (uint8_t)((text[8]-'0')*10 + text[9]-'0');
    if (!umi_financial_date_is_valid(date)) return UMI_STATUS_INVALID_ARGUMENT;
    *outDate = date; return UMI_STATUS_OK;
}
UmiStatus UmiBankActivityQueryValidate(const UmiBankActivityQuery *query)
{
    if (query == NULL || !BankIdValid(&query->accountId, true) ||
        UmiBankActivityDirectionName(query->direction) == NULL ||
        (!EmptyDate(query->fromDate) && !umi_financial_date_is_valid(query->fromDate)) ||
        (!EmptyDate(query->toDate) && !umi_financial_date_is_valid(query->toDate)) ||
        (!EmptyDate(query->fromDate) && !EmptyDate(query->toDate) &&
            umi_financial_date_compare(query->fromDate, query->toDate) > 0))
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *end = memchr(query->reference, '\0', sizeof query->reference);
    if (end == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (const char *p = query->reference; p != end; ++p) {
        unsigned char c = (unsigned char)*p;
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '.' || c == '-' || c == '_'))
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
/* Only validated dates enter a captured report. Empty bounds remain empty. */
void BankActivityDateText(UmiFinancialDate date, char out[11])
{
    out[0] = '\0';
    if (EmptyDate(date) || !umi_financial_date_is_valid(date)) return;
    /* Fixed digits avoid locale dependence and compiler truncation warnings
     * about an unconstrained integer year in an eleven-byte date buffer. */
    unsigned year = (unsigned)date.year;
    out[0] = (char)('0' + year / 1000U);
    out[1] = (char)('0' + year / 100U % 10U);
    out[2] = (char)('0' + year / 10U % 10U);
    out[3] = (char)('0' + year % 10U);
    out[4] = '-'; out[5] = (char)('0' + date.month / 10U); out[6] = (char)('0' + date.month % 10U);
    out[7] = '-'; out[8] = (char)('0' + date.day / 10U); out[9] = (char)('0' + date.day % 10U);
    out[10] = '\0';
}
