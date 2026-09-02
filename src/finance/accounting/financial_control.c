/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/financial_control.c
 *
 * PURPOSE:
 *   Implement aggregate accounting control breaches, unreconciled items and pending approvals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/financial_control.h"
#include <string.h>
/*
 * Initialise accounting financial control from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_financial_control_init(UmiAccountingFinancialControl *value,
    const char *id,
    size_t control_breaches,
    size_t unreconciled_items,
    size_t pending_approvals) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->control_breaches=control_breaches;
    value->unreconciled_items=unreconciled_items;
    value->pending_approvals=pending_approvals;
    return umi_accounting_financial_control_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that accounting financial control satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_financial_control_valid(const UmiAccountingFinancialControl *value) {
    return value!=NULL && (true);
}

/*
 * Provide the accounting financial control healthy operation used by this module and its
 * client applications.
 */
bool umi_accounting_financial_control_healthy(const UmiAccountingFinancialControl *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->control_breaches==0U && value->unreconciled_items==0U;
}
