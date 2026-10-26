/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/accounting_calendar.c
 *
 * PURPOSE:
 *   Implement non-overlapping accounting calendar insertion and date resolution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/accounting_calendar.h"
#include <string.h>

/* Retain the former implementation for review. The replacement validates
 * complete restored values and bounded containers before lookup or eligibility,
 * and stages initialization before publication to preserve caller data. */
#if 0
/*
 * Initialise accounting accounting calendar from caller-provided values so later
 * operations receive a known state.
 */
void umi_accounting_accounting_calendar_init(UmiAccountingAccountingCalendar *value){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value!=NULL)memset(value,0,sizeof *value); }
/*
 * Add accounting accounting calendar only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_accounting_accounting_calendar_add(UmiAccountingAccountingCalendar *value,const UmiAccountingAccountingPeriod *period){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value==NULL||period==NULL||!umi_accounting_accounting_period_valid(period))return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value->count>=UMI_ACCOUNTING_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<value->count;++i){ const UmiAccountingAccountingPeriod *p=&value->items[i]; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_financial_date_compare(period->start_date,p->end_date)<=0 && umi_financial_date_compare(period->end_date,p->start_date)>=0)return UMI_STATUS_ALREADY_EXISTS; } value->items[value->count++]=*period; return UMI_STATUS_OK; }
/*
 * Provide the accounting accounting calendar find date operation used by this module and
 * its client applications.
 */
const UmiAccountingAccountingPeriod *umi_accounting_accounting_calendar_find_date(const UmiAccountingAccountingCalendar *value,UmiFinancialDate date){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value==NULL||!umi_financial_date_is_valid(date))return NULL; /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<value->count;++i){ const UmiAccountingAccountingPeriod *p=&value->items[i]; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_financial_date_compare(date,p->start_date)>=0&&umi_financial_date_compare(date,p->end_date)<=0)return p; } return NULL; }

#endif

/* A restored calendar must be bounded and unambiguous before a date is resolved.
 * Both end dates are inclusive; adjacent periods must begin on different dates. */
UmiStatus umi_accounting_accounting_calendar_validate(const UmiAccountingAccountingCalendar *value)
{
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (value->count > UMI_ACCOUNTING_MAX_ITEMS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (size_t i = 0U; i < value->count; ++i) {
        const UmiAccountingAccountingPeriod *period = &value->items[i];
        if (!umi_accounting_accounting_period_valid(period)) {
            return UMI_STATUS_INVALID_STATE;
        }
        for (size_t j = 0U; j < i; ++j) {
            const UmiAccountingAccountingPeriod *other = &value->items[j];
            if (umi_financial_id_compare(&period->id, &other->id) == 0 ||
                (umi_financial_date_compare(period->start_date, other->end_date) <= 0 &&
                 umi_financial_date_compare(period->end_date, other->start_date) >= 0)) {
                return UMI_STATUS_INVALID_STATE;
            }
        }
    }
    return UMI_STATUS_OK;
}

void umi_accounting_accounting_calendar_init(UmiAccountingAccountingCalendar *value)
{
    if (value != NULL) {
        memset(value, 0, sizeof *value);
    }
}

UmiStatus umi_accounting_accounting_calendar_add(UmiAccountingAccountingCalendar *value,
    const UmiAccountingAccountingPeriod *period)
{
    if (!umi_accounting_accounting_period_valid(period)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiStatus status = umi_accounting_accounting_calendar_validate(value);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (value->count >= UMI_ACCOUNTING_MAX_ITEMS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (size_t i = 0U; i < value->count; ++i) {
        const UmiAccountingAccountingPeriod *other = &value->items[i];
        if (umi_financial_id_compare(&period->id, &other->id) == 0 ||
            (umi_financial_date_compare(period->start_date, other->end_date) <= 0 &&
             umi_financial_date_compare(period->end_date, other->start_date) >= 0)) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    UmiAccountingAccountingPeriod accepted = *period;
    value->items[value->count] = accepted;
    ++value->count;
    return UMI_STATUS_OK;
}

const UmiAccountingAccountingPeriod *umi_accounting_accounting_calendar_find_date(
    const UmiAccountingAccountingCalendar *value, UmiFinancialDate date)
{
    if (!umi_financial_date_is_valid(date) ||
        umi_accounting_accounting_calendar_validate(value) != UMI_STATUS_OK) {
        return NULL;
    }
    for (size_t i = 0U; i < value->count; ++i) {
        if (umi_accounting_accounting_period_contains(&value->items[i], date)) {
            return &value->items[i];
        }
    }
    return NULL;
}
