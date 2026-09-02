/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/accounting_period.c
 *
 * PURPOSE:
 *   Implement represent accounting period boundaries and posting status.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/accounting_period.h"
#include <string.h>
/*
 * Initialise accounting accounting period from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_accounting_period_init(UmiAccountingAccountingPeriod *value,
    const char *id,
    UmiFinancialDate start_date,
    UmiFinancialDate end_date,
    UmiAccountingPeriodStatus status) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->start_date=start_date;
    value->end_date=end_date;
    value->status=status;
    return umi_accounting_accounting_period_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that accounting accounting period satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_accounting_period_valid(const UmiAccountingAccountingPeriod *value) {
    return value!=NULL && (umi_financial_date_is_valid(value->start_date) && umi_financial_date_is_valid(value->end_date) && umi_financial_date_compare(value->start_date,value->end_date)<=0 && value->status>=UMI_ACCOUNTING_PERIOD_OPEN && value->status<=UMI_ACCOUNTING_PERIOD_CLOSED);
}

/*
 * Provide the accounting accounting period open operation used by this module and its
 * client applications.
 */
bool umi_accounting_accounting_period_open(const UmiAccountingAccountingPeriod *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->status==UMI_ACCOUNTING_PERIOD_OPEN;
}
