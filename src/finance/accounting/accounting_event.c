/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/accounting_event.c
 *
 * PURPOSE:
 *   Implement represent canonical economic events before posting-rule transformation into journals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/accounting_event.h"
#include <string.h>
/*
 * Initialise accounting accounting event from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_accounting_event_init(UmiAccountingAccountingEvent *value,
    const char *id,
    const char *event_type,
    UmiFinancialDate accounting_date,
    int64_t amount_minor,
    const char *currency_code) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_financial_core_copy(value->event_type,sizeof value->event_type,event_type);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    value->accounting_date=accounting_date;
    value->amount_minor=amount_minor;
    rc=umi_accounting_currency_from_code(currency_code,&value->currency);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    return umi_accounting_accounting_event_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that accounting accounting event satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_accounting_event_valid(const UmiAccountingAccountingEvent *value) {
    return value!=NULL && (value->event_type[0]!='\0' && umi_financial_date_is_valid(value->accounting_date) && value->amount_minor!=0);
}

/*
 * Provide the accounting accounting event absolute minor operation used by this module and
 * its client applications.
 */
int64_t umi_accounting_accounting_event_absolute_minor(const UmiAccountingAccountingEvent *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (int64_t)0;
    return umi_accounting_abs_i64(value->amount_minor);
}
